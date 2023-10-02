/*The program functionality has been astablished not building the web server networking properties.*/

/*To begin with, include the needed headers:*/

#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNt 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#else 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#endif
/*To make it cross platform you include the headerfile to switch on and off depending on if it is Windows or Unix/Linux/MacOS */

/*Below are defined macros, which abstract out some of the difference between the Berkeley socket ans Winsock API's*/

#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())

#else 
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#endif

/*needed also is a couple of standard C headers*/

#include <stdio.h>
#include <string.h>
#include <time.h>

/*Now, to begin the "main()" function. The first thing the "main()" function will do is initialize Winscok if we are compiling on Windows: check below*/

int main()
{
#if defined(_WIN32)
	WSADATA d; 
	if (WSAStartup(MAKEWORD(2,2), &d))
	{
		fprint(stderr, "Failed to initialize.\n"); 
		return 1; 
	}
#endif 

	/*We much now figure out the local address that our web server should bind to:*/
	printf("Configuring local address...\n"); 
	struct addrinfo hints;		 
	memset(&hints, 0, sizeof(hints)); 
	hints.ai_family = AF_INET; 
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_flags = AI_PASSIVE; 

	struct addrinfo *bind_address; 
	getaddrinfo(0, "8080", &hints, &bind_address); /*We use "getaddrinfo()" to fill in a "struct addrinfo" structure with the needed information.*/

	/*Now that we've figured out our local address info, we can create the socet:*/

	printf("Creating socket...\n"); 
	SOCKET socket_listen; /*We define "socket_listen" as a "SOCKET" type. 
	"SOCKET" in a WindowsOS has a macro defining it as "int" on other platforms.
	We call the "socket()" function to generate the actual socket.*/
		socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol); /*"socket()" takes 3 parameters:
		1) the socket family, 
		2) the socket type, 
		3) the socket protocol,
	The reason we used "getaddrinfo()" before calling "socket()" is that we can now pass in parts of "bind_address" as the arguments to "socket()"*/

		/*Next, we should check that the call to "socket()" was successful:*/
		if (!ISVALIDSOCKET(socket_listen))	/*using the "ISVALIDSOCKET()" macro you can check if "socket_listen" is valid.*/
		{
			fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO()); 
			return 1; 
		}

		/*After the socket has been created successfully, we can call "bind()" to associate it with our address from "getaddrinfo()"*/

		printf("Binding socket to local address...\n"); 
		if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))	/*"bind()" fails if the port we are binding to is already in use. 
		Either close the program using that port or change your program to use a different port.*/
		{
			fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO()); 
			return 1; 
		}

		freeaddrinfo(bind_address); /*After binding the address we can call "freeaddrinfo()" function to release the address memory*/

		/*Next, once the socket has been created and bound to a local address, we can cause it to start listening for connections with the "listen()" function:*/
		printf("Listening...\n"); 
		if (listen(socket_listen, 10) < 0)	/*the argurment 10 in "listen()" tells how many connections it is allowed to queue up. */
		{
			fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO()); 
			return 1; 
		}

		/*After the socket has begun listening for connections, we can accept any incoming connection with the "accept()" function*/

		printf("Waiting for connection...\n"); 
		struct sockaddr_storage client_address; 
		socklen_t client_len = sizeof(client_address);	/*You must declare a new "struct sockaddr_storage" variable to store the address info for the connecting client, befor calling "accept()"*/
		SOCKET socket_client = accept(socket_listen, (struct sockaddr*) &client_address, &client_len); 	/*"accept()" has a list of functions.
		1) when its called it will block your program until a new connection is made. 
		Your program will sleep until a connection is made to the listening socket. 
		When the new connection is made, "accept()" will create a new socket for it. 
		Your orgiginal socket continues to listen for new connections, but the new socket returned by "accept()" can be used to send and receive data over the newly established connection, 
		"accecpt()" also fill in address info of the client that connected.*/
		if (!ISVALIDSOCKET(socket_client))	/*We check for errors by detecting if "client_socket" is valid socket. done the same way as was for "socket()"*/
		{
			fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO());
			return 1; 
		}

		/*At this point, a TCP connection has been established to a remote client. we can print the client's address to the console: below
		 * This step is completely optional, but it is good practice to log network connections somewhere (unless you running a VPN service)*/

		printf("Client is connected... "); 
		char address_buffer[100]; 
		getnameinfo((struct sockaddr*)&client_address, client_len, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST); 
		printf("%s\n", address_buffer); 

		/*As we are programming a web server, we expect the client(for example a web browser) to send us an HTTP request. 
		 * We read this request using the "recv()" function: below*/

		printf("Reading request...\n"); 
		char request[1024]; 
		int bytes_received = recv(socket_client, request, 1024, 0); 	/*"recv()" is called with the client's socket, the request buffer, and the request buffer size. 
		"recv()" returns the number of bytes that are received. 
		If nothing has been received yet, "recv()" blocks until it has something. 
		If the connection is terminated by the client, "recv()" returns 0 or -1, depending on the circumstance. */
		printf("Received %d bytes.\n", bytes_received); 

		/*If you want to print the browser's request to the console, you can do it like the method below. */

		printf("%.*s", bytes_received, request);	/*"%.*s" tells "printf()" that we want to print a specific number of characters - "bytes_received". */

		/*Now that the web browser has sent its request, you can send our response back: like below*/

		/*printf("Sending response...\n"); const char *response = "HTTP/1.1 200 OK\r\n" "Connection: close\r\n"	"Content-Type: text/plain\r\n\r\n" "Local time is: "; */

	       const char *response = 
	       "HTTP/1.1 200 OK\r\n"
		"Connection: close\r\n"
 		"Content-type: text/plain\r\n\r\n"
		"Local time is: "; 		

		int bytes_sent = send(socket_client, response, strlen(response), 0);		/*We send data to the client using the "send()" function. This function takes the client's socket, a pointer to the data to be sent, and the length of the data to send. */

		printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(response)); 

		/*After the HTTP header and the beginning of our message is sent, we can send the actual time. 
		 * We get the local time the same way we did in "time_console.c" and we wend it using "send()"*/

		time_t timer; 
		time(&timer); 
		char *time_msg = ctime(&timer); 
		bytes_sent = send(socket_client, time_msg, strlen(time_msg), 0);
		printf("Sent %d of %d bytes,\n", bytes_sent, (int)strlen(time_msg)); 

		/*We must then close the client connection to indicate to the browser that we've sent all of our data: */

		printf("Closing connection...\n"); 
		CLOSESOCKET(socket_client); /*If we dont close the connection, the browser will just wait for more data until it times out.*/

		/*At this point we could call "accept()" on "socket_listen" to accept additional connections.
		 * For this case, we will instead close the listening socket and terminate the program: */

		printf("Closing listening socket...\n"); 
		CLOSESOCKET(socket_listen); 

#if defined(_WIN32)
		WSACleanup(); 
#endif 

		printf("Finished.\n"); 

		return 0; 

		
}

