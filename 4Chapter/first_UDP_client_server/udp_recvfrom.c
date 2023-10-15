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
	/*Next, we must configure the local address that our server listens on. 
	 * We use "getaddrinfo()" for this, as follows: below*/
	printf("Configuring local address...\n"); 
	struct addrinfo hints; 
	memset(&hints, 0, sizeof(hints)); 
	hints.ai_family = AF_INET; 		/*"AF_INET6" makes it ipv6*/
	hints.ai_socktype = SOCK_DGRAM;		/*UDP uses "sock_dram"*/
	hints.ai_flags = AI_PASSIVE; 

	struct addrinfo *bind_address; 
	getaddrinfo(0, "8080", &hints, &bind_address); 

	/*After we have our local address information, we can create the socket as follows: below*/
	printf("Creating socket...\n"); 
	SOCKET socket_listen; 
	socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol); 
	if(!ISVALIDSOCKET(socket_listen))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO()); 
		return 1; 
	}

	/*We must then bind the new socket to the local address that we got from "getaddrinfo()".*/
	printf("Binding socket to local address...\n"); 
	if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO()); 
		return 1; 
	}
	freeaddrinfo(bind_address); 

	/*Once the local address is bound, we can simply start to receive data.
	 * There is no need to call "listen()" or "accept()". 
	 * We listen for incoming data using "recvfrom()" as shown below*/
	struct sockaddr_storage client_address;		/*To store the client's address.*/
	socklen_t client_len = sizeof(client_address);	/*to hold the address size.*/ 
	char read[1024]; 
	int bytes_received = recvfrom(socket_listen, read, 1024, 0, (struct sockaddr*) &client_address, &client_len);		/*"recvfrom()" is used in a similar manner to "recv()", except that it returns the sender's address, as well as the received data. 
	You can think of "recvfrom()" as a combination of the TCP server "accept()" and "recv()". */

	/*Once we've received data, we can print it out. 
	 * Keep in mind that the data may not be null terminated. 
	 * It can be safely printed with the "%.*s printf()" format specifier, as shown below*/
	printf("Received (%d bytes): %.*s\n", bytes_received, bytes_received, read); 

	/*To print the sender's address and port number. 
	 * We can use the "getnameinfo()" function to convert this data into a printable string, as shown in the following code.*/
	printf("Remote address is: "); 
	char address_buffer[100]; 
	char service_buffer[100]; 
	getnameinfo(((struct sockaddr*)&client_address), client_len, address_buffer, sizeof(address_buffer), service_buffer, sizeof(service_buffer), NI_NUMERICHOST | NI_NUMERICSERV); 		/*The last argument to "getnameinfo()" tells "getnameinfo()" that we want both the client address and port number to be in numeric form. */

	/*In any case, if our server were to send data back, it would need to send it to the address and port stored in "client_address".
	 * This would be done by passing "client_address" to "sendto()"*/
	printf("%s %s\n", address_buffer, service_buffer); 

	/*Next, once the data has been received, we'll end our simple UDP server by closing the connection, cleaning up Winsock, and ending the program: */

	CLOSESOCKET(socket_listen); 
#if defined(_WIN32)
	WSACleanup(); 
#endif 

	printf("Finished.\n"); 
	return 0; 
}
