#include "chap06.h"

/*First, define a constant, "TIMEOUT". 
 * Later in our program, if an HTTP response is taking more than "TIMEOUT" seconds to complete, then our program abandons the request.*/
#define TIMEOUT 5.0

void parse_url(char *url, char **hostname, char **port, char** path)
{
	printf("URL: %s\n", url); 

	char *p; 
	p = strstr(url, "://"); 

	char *protocol = 0; 
	if (p)
	{
		protocol = url; 
		*p = 0; 
		p += 3; 
	}else 
	{
		p = url; 
	}

	if (protocol)
	{
		if (strcmp(protocol, "http"))
		{
			fprintf(stderr, "unknown protocol '%s'. Only 'http' is supported.\n", protocol);
			exit(1); 
		}
	}
	*hostname = p; 
	while (*p && *p != ':' && *p != '/' && *p != '#')
		++p; 
	*port = "80"; 
	if (*p == ':')
	{
		*p++ = 0; 
		*port = p; 
	}
	while (*p && *p != '/' && *p != '#')
		++p; 
	*path = p; 
	if (*p == '/')
	{
		*path = p + 1; 
	}
	*p = 0; 

	while(*p && *p != '#')
		++p; 
	if (*p == '#')
		*p = 0; 

	printf("hostname: %s\n", *hostname); 
	printf("port: %s\n", *port); 
	printf("path: %s\n", *path); 
}

/*Next, the helper function "send_request" is written*/

/*"send_request()" works by first defining a character buffer in which to store the HTTP request. 
 * It then uses the "sprintf()" function to write to the buffer until the HTTP request is complete. 
 * The HTTP request ends with a blank line. 
 * This blank line tells the server that the entire request header has been received.*/
void send_request(SOCKET s, char *hostname, char *port, char *path)
{
	char buffer[2048]; 

	sprintf(buffer, "GET /%s HTTP/1.1\r\n", path); 
	sprintf(buffer + strlen(buffer), "Host: %s:%s\r\n", hostname, port); 
	sprintf(buffer + strlen(buffer), "Connection: close\r\n"); 
	sprintf(buffer + strlen(buffer), "User-Agent: honpws web_get 1.0\r\n"); 
	sprintf(buffer + strlen(buffer), "\r\n"); 
	
	send(s, buffer, strlen(buffer), 0); 
	printf("Sent Headers:\n%s", buffer); 
}

/*"connect_to_host()", "getaddrinfo()" is used to resolve the hostname. 
 * "getnameinfo()" is then used to print out the server IP address for debugging purposes.*/
SOCKET connect_to_host(char *hostname, char *port)
{
	printf("Configuring remote address...\n"); 
	struct addrinfo hints; 
	memset(&hints, 0, sizeof(hints)); 
	hints.ai_socktype = SOCK_STREAM;	/*REMEMBER this is for TCP*/
	struct addrinfo *peer_address; 
	if (getaddrinfo(hostname, port, &hints, &peer_address))
	{
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO()); 
		exit(1); 
	}

	printf("Remote address is: "); 
	char address_buffer[100]; 
	char service_buffer[100]; 
	getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen, address_buffer, sizeof(address_buffer), service_buffer, sizeof(service_buffer), NI_NUMERICHOST); 
	printf("%s %s\n", address_buffer, service_buffer); 

	/*Now for "connect_to_host()", a new socket is created with "socket()" and a TCP connection is established with "connect()". 
	 * If everything goes well, the function returns the created socket.*/
	printf("Creating socket...\n"); 
	SOCKET server; 
	server = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol); 
	if (!ISVALIDSOCKET(server))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO()); 
		exit(1); 
	}

	printf("Connecting...\n"); 
	if (connect(server, peer_address->ai_addr, peer_address->ai_addrlen))
	{
		fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO()); 
		exit(1); 
	}
	freeaddrinfo(peer_address); 

	printf("Connected.\n\n"); 
	return server; 
}

/*With our helper functions out of the way, we can now begin to define the "main()".*/
int main(int argc, char *argv[])
{
#if defined(_WIN32)
	WSADATA d; 
	if (WSAStartup(MAKEWORD(2,2), &d))
	{
		fprintf(stderr, "Failed to initialize.\n"); 
		return 1; 
	}
#endif 

	if (argc < 2)
	{
		fprintf(stderr, "usage: web_get url\n"); 
		return 1; 
	}
	char *url = argv[1]; 

	/*We can then parse the URL into its hostname, port, and path parts*/
	char *hostname, *port, *path; 
	parse_url(url, &hostname, &port, &path); 

	/*The program then continues by establishing a connection to the target server and sending the HTTP request. 
	 * Making the functions "connect_to_host()" and "send_request()" makes the program modular.*/
	SOCKET server = connect_to_host(hostname, port); 
	send_request(server, hostname, port, path); 

	/*One feature of our web client is that it times out if a request takes too long to complete. 
	 * We store the start time in the "start_time" variable*/
	const clock_t start_time = clock(); 

	/*Ti is now necessary to define some more variables that can be used for bookkeeping while receiving and parsing the HTTP response.*/
#define RESPONSE_SIZE 8192
	char response[RESPONSE_SIZE+1];		/*this program max size of the HTTP response is the size of "RESPONSE_SIZE" for memory handling*/ 
	char *p = response, *q; 
	char *end = response + RESPONSE_SIZE; 
	char *body = 0; 	/*"body" is used to remember the beginning of the HTTP response body once received.*/

	enum {length, chunked, connection}; 
	int encoding = 0; 
	int remaining = 0; 

	/*We then start a loop to receive and process the HTTP response. 
	 * This loop first checks that it hasn't taken too much time and that we still have buffer space left to store the received data.*/
	while(1)
	{
		if ((clock() - start_time) / CLOCKS_PER_SEC > TIMEOUT)
		{
			fprintf(stderr, "timeout after %.2f seconds\n", TIMEOUT); 
			return 1; 
		}

		if (p == end)
		{
			fprintf(stderr, "out of buffer space\n"); 
			return 1; 
		}

		/*Next, include the code to receive data over TCP socket. 
		 * "select()" is used with a sort timeout.
		 * This allows us to periodically check that the request hasn't timed out.*/
		fd_set reads; 
		FD_ZERO(&reads); 
		FD_SET(server, &reads); 

		struct timeval timeout; 
		timeout.tv_sec = 0; 
		timeout.tv_usec = 200000; 

		if (select(server+1, &reads, 0, 0, &timeout) < 0)
		{
			fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO()); 
			return 1; 
		}

		/*When attempting to read new data, we may find that the socket was closed by the web server. 
		 * If this is the case, we check whether we were expecting a close connection to indicate the end of the transmission.
		 * That is the case if "encoding == connection". 
		 * If so, we print the HTTP body data that was received.*/
		if (FD_ISSET(server, &reads))
		{
			int bytes_received = recv(server, p, end - p, 0); 
			if (bytes_received < 1)
			{
				if (encoding == connection && body)
				{
					printf("%.*s", (int)(end - body), body);
				}
				printf("\nConnection closed by peer.\n"); 
				break; 
			}

			/*printf("Received (%d bytes): '%,*s'", bytes_received, bytes_received, p);*/
			p += bytes_received; 
			*p = 0; 

			/*The following codes fins the end of the HTTP header using "strstr()", and updates the "body" pointer to point to the beginning of the HTTP body:*/
			if (!body && (body = strstr(response, "\r\n\r\n")))
			{
				*body = 0; 
				body += 4;
				printf("Received Header:\n%s\n", response); 

				/*If neither "Content-Lenth" or "Transfer-Encoding: chucked" is found, then "endcoding = connection" is set to indicate that we consider the HTTP body received when connection is closed.*/
				q = strstr(response, "\nContent-Length: "); 
				if (q)
				{
					encoding = length; 
					q = strchr(q, ' '); 
					q += 1; 
					remaining = strtol(q, 0, 10); 
				} else 
				{
					q = strstr(response, "\nTransfer-Encoding: chucked"); 
					if (q)
					{
						encoding = chunked; 
						remaining = 0; 
					} else 
					{
						encoding = connection; 
					}
				}
				printf("\nReceived Body:\n"); 
			}
			/*If the HTTP body start has been identified, and "encoding  == length", then the program simply needs to wait unitl "remaining" bytes have been received.*/
			if (body)
			{
				if (encoding == length)
				{
					if (p - body >= remaining)
					{
						printf("%.*s", remaining, body);
						break; 
					}

					/*if "Transfer-Encoding: chucked" is used, then the receiving logic is a bit more complcated.*/
				} else if (encoding == chunked)
				{
					do {
						if (remaining == 0)
						{
							if ((q = strstr(body, "\r\n")))
							{
								remaining = strtol(body, 0, 16); 
								if (!remaining) goto finish; 
								body = q + 2; 
							} else 
							{
								break; 
							}
						}	
						if (remaining && p - body >= remaining)
						{
							printf("%.*s", remaining, body); 
							body += remaining +2; 
							remaining = 0; 
						}
					} while (!remaining); 
				}
			} /*if (body)*/

			/*At this point we shown all the logic to partse the HTTP response body. 
			 * We only need to end our loops, close the socket, and the program is finished. */
		}/*IF FDSET*/
	}/*end while (1)*/
finish:  
	printf("\nClosing socket...\n"); 
	CLOSESOCKET(server); 
#if defined(_WIN32)
	WSACleanup(); 
#endif 
	printf("Finished.\n"); 
	return 0; 
}
