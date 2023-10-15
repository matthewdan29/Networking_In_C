#include "chap04.h"

int main()
{
#if defined(_WIN32)
	WSADATA d; 
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		fprintf(stderr, "Failed to initialize.\n"); 
		return 1; 
	}
#endif 
	/*Below is how the remote address is configured:*/
	printf("Configuring remote address...\n"); 
	struct addrinfo hints; 
	memset(&hints, 0, sizeof(hints)); 
	hints.ai_socktype = SOCK_DGRAM; 	/*Notice the hard code to make suer its UDP protocal and not TCP*/
	struct addrinfo *peer_address; 
	if (getaddrinfo("127.0.0.1", "8080", &hints, &peer_address))	/*Notice the hard code for making sure its a local computer connection*/
	{
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO()); 
		return 1; 
	}

	/*We can print the configured address using "getnameinfo()".
	 * The call to "getnameinfo()" is the same as in the UDP server, udp_recvfrom.c*/
	printf("Remote address is: "); 
	char address_buffer[100]; 
	char service_buffer[100]; 
	getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen, address_buffer, sizeof(address_buffer), service_buffer, sizeof(service_buffer), NI_NUMERICHOST | NI_NUMERICSERV); 
	printf("%s %s\n", address_buffer, service_buffer); 

	/*Now that we've stored the remote address, we are ready to create our socket with a call to "socket()". 
	 * We pass in fields from "peer_address" to create the appropriate socket type. The code is below */
	printf("Creating socket...\n"); 
	SOCKET socket_peer; 
	socket_peer = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol); 
	if (!ISVALIDSOCKET(socket_peer))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO()); 
		return 1; 
	}
	
	/*Once the socket has been created, we can go straight to sending data with "sendto()". 
	 * There is no need to call "connect()". 
	 * Here is the code ot send "Hello world" to our UDP server: */
	const char *message = "Hello World"; 
	printf("Sending: %s\n", message); 
	int bytes_sent = sendto(socket_peer, message, strlen(message), 0, peer_address->ai_addr, peer_address->ai_addrlen);	/*"sendto()" is much like "send()" except that we need to pass in an address as the last parameter.*/ 
	printf("Sent %d bytes.\n", bytes_sent); 

	/*We end our example program by freeing the memory for "peer_address", closing the socket, cleaning up winsock, and finishing "main()" as follows*/

	freeaddrinfo(peer_address); 
	CLOSESOCKET(socket_peer); 
#if defined(_WIN32)
	WSACleanup(); 
#endif 

	printf("Finished.\n"); 
	return 0; 
}
