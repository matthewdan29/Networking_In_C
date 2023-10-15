/*Our UDP server program begins by including the necessary headers, starting the "main()" function, and initializing Winsock, as follows*/
#include "chap04.h"
#include <ctype.h>

int main()
{
#if defined(_WIN32)
	WSADATA d; 
	if (WSAStartup(MAIKEWORD(2, 2), &d))
	{
		fprintf(stderr, "failed to initialize.\n"); 
		return 1; 
	}
#endif
	/*Below is the code for setting the address and creating a new socket:*/
	printf("Configuring local address...\n"); 
	struct addrinfo hints; 
	memset(&hints, 0, sizeof(hints)); 
	hints.ai_family = AF_INET; 
	hints.ai_socktype = SOCK_DGRAM; 
	hints.ai_flags = AI_PASSIVE; 

	struct addrinfo *bind_address; 
	getaddrinfo(0, "8080", &hints, &bind_address); 
	
	printf("Creating socket...\n"); 
	SOCKET socket_listen; 
	socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol); 
	if (!ISVALIDSOCKET(socket_listen))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO()); 
		return 1; 
	}

	/*Binding the new socket to thte local address is done as follows:*/
	printf("Binding socket to local address...\n"); 
	if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO()); 
		return 1; 
	}
	freeaddrinfo(bind_address); 

	/*The server uses "select()", we need to create a new "fd_set" to store our listening socket. 
	 * We zero the set using "FD_ZERO()", and then add our socket to this set using "FD_SET()". 
	 * We also maintain the maximum socket in the set using "max_socket"*/
	fd_set master; 
	FD_ZERO(&master); 
	FD_SET(socket_listen, &master); 
	SOCKET max_socket = socket_listen; 

	printf("Waiting for connections...\n"); 

	/*The mian program loop can be seen in the following code:*/
	while(1)
	{
		fd_set reads; 
		reads = master; 
		if (select(max_socket+1, &reads, 0, 0, 0) < 0)
		{
			fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO()); 
			return 1; 
		}

		if (FD_ISSET(socket_listen, &reads))
		{
			struct sockaddr_storage client_address; 
			socklen_t client_len = sizeof(client_address); 

			char read[1024]; 
			int bytes_received = recvfrom(socket_listen, read, 1024, 0, (struct sockaddr *)&client_address, &client_len); 
			if (bytes_received < 1)
			{
				fprintf(stderr, "connection closed. (%d)\n", GETSOCKETERRNO()); 
				return 1; 
			}

			int j; 
			for (j = 0; j < bytes_received; ++j)
				read[j] = toupper(read[j]); 
			sendto(socket_listen, read, bytes_received, 0, (struct sockaddr*)&client_address, client_len); 
		}/*if FD_ISSET*/
	}/*while(1)*/

	/*By now you can tell that UDP and TCP socket programing is very alike besides the "SOCK_DGRAM" and several other changes its 1 to 1. 
	 * Last is the clean up code.*/
	printf("Closing listening socket...\n"); 
	CLOSESOCKET(socket_listen); 

#if defined(_WIN32)
	WSACleanup(); 
#endif

	printf("Finished.\n"); 
	return 0; 
}
