#include "chap03.h"
#include <ctype.h>

int main()
{
#if defined(_WIN32)
	WSADATA d; 
	if(WSAStartup(MAKEWORD(2,2), &d))
	{
		fprintf(stderr, "Failed to initialize.\n"); 
		return 1; 
	}
#endif 
	/*We then get our local address, create our socket, and "bind()".*/
	printf("Configuring local address...\n"); 
	struct addrinfo hints; 
	memset(&hints, 0, sizeof(hints)); 
	hints.ai_family = AF_INET; 
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_flags = AI_PASSIVE; 

	struct addrinfo *bind_address; 
	getaddrinfo(0, "8080", &hints, &bind_address); 

	printf("Creating socket...\n"); 
	SOCKET socket_listen; 
	socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol); 

	if(!ISVALIDSOCKET(socket_listen))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO()); 
		return 1; 
	}

	/*We then "bind()" our socket to the local address and have it enter a listening state.*/
	printf("Binding socket to local address...\n"); 
	if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO()); 
		return 1; 
	}
	freeaddrinfo(bind_address); 

	printf("Listening...\n"); 
	if(listen(socket_listen, 10) < 0)
	{
		fprintf(stderr, "Listen() failed. (%d)\n", GETSOCKETERRNO())	; 
		return 1; 
	}

	/*We now define an "fd_set" structure that stores all of the active sockets. 
	 * We also maintain a "max_socket" variable, which holds the largest socket descriptor. 
	 * For now, we add only our listening socket to the set. 
	 * It's the only socket, it must also be the largest, se we set "max_socket = socket_listen" too: */
	fd_set master; 
	FD_ZERO(&master); 
	FD_SET(socket_listen, &master); 
	SOCKET max_socket = socket_listen; 
	/*We will add new connections to "master" as they are established.*/

	/*We then print a status message, enter the main loop, and set up our call to "select()"*/
	printf("Waiting for connections...\n"); 

	while(1)
	{
		fd_set reads; 
		reads = master; 
		if (select(max_socket+1, &reads, 0, 0, 0) < 0)
		{
			fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO()); 
			return 1; 
		}

		/*We now loop throught each possible socket and see whether it was flagged by "select()" as being ready.
		 * If a socket, 'X', was flagged by "select()", then "FD_ISSET(X, &reads)" is true. 
		 * Socket descriptors are positive integers, so we can try every possible socket descriptor up to "max_socket". 
		 * The basic structure of our loop is as follows:*/
		SOCKET i; 
		for(i = 1; i <= max_socket; ++i)
		{
			if (FD_ISSET(i, &reads))
			{
				/*handle socket*/
				if(i == socket_listen)
				{
					struct sockaddr_storage client_address;
					socklen_t client_len = sizeof(client_address); 
					SOCKET socket_client = accept(socket_listen, (struct sockaddr*) &client_address, &client_len); 
					if (!ISVALIDSOCKET(socket_client))
					{
						fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO()); 
						return 1; 
					}

					FD_SET(socket_client, &master); 
					if (socket_client > max_socket)
						max_socket = socket_client; 

					char address_buffer[100]; 
					getnameinfo((struct sockaddr*)&client_address, client_len, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
					printf("New connection from %s\n", address_buffer); 
					/*If the socket 'i' is not "socket_listen", then it is instead a request for an establisihed connections. 
					 * We now need to read it with "recv()", convert it into uppercase using the built-in "toupper()" function, and send the data back: below the else is in it tooo*/
				} else 
				{
					char read[1024]; 
					int bytes_received = recv(i, read, 1024, 0); 
					if (bytes_received < 1)
					{
						FD_CLR(i, &master); 
						CLOSESOCKET(i); 
						continue; 
					}
					int j; 
					for (j = 0; j < bytes_received; ++j)
						read[j] = toupper(read[j]); 
					send(i, read, bytes_received, 0); 
				}

				/*We can end the "if FD_ISSET()" statement, end the "for" loop, end the "while" loop, close the listening socket, and clean up Winsock:*/
			}/*if FD_ISSET*/
		}/*For i to max_socket*/
	}/*while(1)*/

	printf("Closing listening socket...\n"); 
	CLOSESOCKET(socket_listen); 

#if defined(_WIN32)
	WSACleanup(); 
#endif 

	printf("Finished.\n"); 
	return 0; 
}
