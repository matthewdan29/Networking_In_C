/*"connect_to_host()" which attempts to open a TCP connection to a given hostname and port number.*/

/*First "getaddrinfo()" is used to resolve the hostname and "getnameinfo()" is then used to print the server IP address.*/
SOCKET connect_to_host(const char *hostname, const char *port)
{
	printf("Configuring remote address...\n"); 
	struct addrinfo hints; 
	memset(&hints, 0, sizeof(hints)); 
	hints.ai_socktype = SOCK_STREAM; 
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

	/*a socket is then created with "socket()"*/
	printf("Creating socket...\n"); 
	SOCKET server; 
	server = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol); 
	if (!ISVALIDSOCKET(server))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO()); 
		exit(1); 
	}

	/*Once the socket has been created, "connect()" is used to establish the connection. 
	 * The following code shows the use of "connect()" and the end of the "connect_to_host()" function*/
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


