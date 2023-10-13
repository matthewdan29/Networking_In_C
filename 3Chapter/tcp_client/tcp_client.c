/*We begin the TCP client by including the header file, "chap03.h". 
 * This header file includes the various other headers and macros we need for cross platform networking*/
#include "chap03.h"

/*We also need the "conio.h" header. 
 * This is required for the "_kbhit()" function, which helps us by indicating whether terminal input is waiting.*/
#if defined(_WIN32)
#include <conio.h>
#endif

/*We can then begin the "main()" function and initialize Winsock(windows)*/
int main(int argc, char *argv[])
{
#if defined(_WIN32)
	WSADATA d; 
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		fprintf(stderr, "Failed to initialize.\n"); 
		return 1; 
	}
#endif 

	/*We would like our program to take the hostname and port number of the server it should connect to as command-line arguments. 
	 * This makes the program flexable. 
	 * We have our program check that these command-line arguments are given. 
	 * if they aren't, it displays usage information*/
	if (argc < 3)	/*"argc" first value is always the program's name thats why we are checking 3 (Computer Scientist start couting from 0 remember that)*/
	{
		fprintf(stderr, "usage: tcp_client hostname port\n"); 
		return 1; 
	}
	
	/*we then use these values to configure a remote address for connection*/
	printf("Configuring remote address...\n"); 
	struct addrinfo hints;	/*"getaddrinfo()" is used to configure a remote address.*/
	memset(&hints, 0, sizeof(hints)); 
	hints.ai_socktype = SOCK_STREAM;	/*We set "hints.ai_socktype = SOCK_STREAM" to tell "getaddrinfo()" that we want a TCP connection. 
	Remember that we could set "SOCK_DGRAM" to indicate a UDP connection.*/
	struct addrinfo *peer_address;		/*"getaddrinfo()" is given the host name and port as the first two arguments. 
	These are passed directly in from the coomand line. 
	If everyting goes well, then our remote address is in the "peer_address" variable.*/
	if (getaddrinfo(argv[1], argv[2], &hints, &peer_address))
	{
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO()); 
		return 1; 
	}

	/*After "getaddrinfo()" configures the remote address, we print it out. 
	 * This isn't necessary, but it is a good debugging measure. 
	 * We use "getnameinfo()" to convert the address back into a string, like below*/
	printf("Remote address is: "); 
	char address_buffer[100]; 
	char service_buffer[100]; 
	getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen, address_buffer, sizeof(address_buffer), service_buffer, sizeof(service_buffer), NI_NUMERICHOST); 
	printf("%s %s\n", address_buffer, service_buffer); 

	/*We can then create our socket:*/
	printf("Creating socket...\n"); 
	SOCKET socket_peer; 
	socket_peer = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol);		/*"peer_address" is used to set the proper socket family and protocols. 
	This keeps our program very flexible, as the "socket()" call creates an IPv4 or IPv6 socket as needed.*/
	if (!ISVALIDSOCKET(socket_peer))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO()); 
		return 1; 
	}

	/*After the socket has been created, we call "connect()" to establish a connection to the remote server*/
	printf("Connecting...\n"); 
	if (connect(socket_peer, peer_address->ai_addr, peer_address->ai_addrlen))		/*"connect()" takes three arguments: 
		0) the socket
		1) the remote address
		2) the remote address length. */
		/*"connect()" associates a socket with a remote address and initiates the TCP connection.*/
	{
		fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO()); 
		return 1; 
	}
	freeaddrinfo(peer_address);		/*we use "freeaddrinfo()" fucntion to free the memory for "peer_address". */

	/*If we've made it this far, then the TCP connection has been established to the remote server. We let the user know by printing a message and instructions on how to send data: like below*/
	printf("Connected.\n"); 
	printf("To send data, enter text followed by enter.\n"); 

	/*It is clear we cannot call "recv()" directly here. 
	 * If we did, it would block until data comes from the socket. 
	 * In the meantime, if our user enter data on the terminal, that input is ingnored. 
	 * Instead, we use "select()". 
	 * We begin our loop and set up the call to "select()", like below.*/
	while(1)
	{
		fd_set reads;		/*We declare a variable, "fd_set reads", to store our socket set.*/ 
		FD_ZERO(&reads);	/*We then zero it with "FD_ZERO()" and add our only socket, "socket_peer"*/ 
		FD_SET(socket_peer, &reads); 
#if !defined(_WIN32)
		FD_SET(0, &reads); 
#endif

		struct timeval timeout; 
		timeout.tv_sec = 0; 
		timeout.tv_usec = 100000; 

		if (select(socket_peer+1, &reads, 0, 0, &timeout) < 0)
		{
			fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO()); 
			return 1; 
		}
		/*After "select()" returns, we check to see whether our socket is set in "reads". 
		 * If it is, then we know to call "recv()" to read new data. 
		 * The new data is printed to the console with "printf()"*/

		if (FD_ISSET(socket_peer, &reads))
		{
			char read[4096]; 
			int bytes_received = recv(socket_peer, read, 4096, 0);
			if (bytes_received < 1)
			{
				printf("Coonection closed by peer.\n"); 
				break; 
			}
			printf("Received (%d bytes): %.*s", bytes_received, bytes_received, read);		/*We use the "%.*s" "printf()" format specifier, which prints a string of a specified length*/
		}

		/*After checking for new TCP data, we also need to check for terminal input: */
#if defined(_WIN32)
		if(_kbhit())
		{
			if(FD_ISSET(0, &reads))
			{
#endif
				char read[4096]; 
				if (!fgets(read, 4096, stdin)) 
					break; 
				printf("Sending: %s", read); 
				int bytes_sent = send(socket_peer, read, strlen(read), 0); 
				printf("Send %d bytes.\n", bytes_sent); 
		}

	}
	printf("Closing socket...\n"); 
	CLOSESOCKET(socket_peer); 

#if defined(_WIN32)
	WSACleanup(); 
#endif

	printf("Finished.\n"); 
	return 0; 
}
