/*"create_socket()" like all server, needs to create a listening socket to accept new connections. */

/*This function begins by using "getaddrinfo()" to find the listening address*/
SOCKET create_socket(const char* host, const char *port)
{
	printf("Configuring local address...\n"); 
	struct addrinfo hints; 
	memset(&hints, 0, sizeof(hints)); 
	hints.ai_family = AF_INET; 
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_flags = AI_PASSIVE; 

	struct addrinfo *bind_address; 
	getaddrinfo(host, port, &hints, &bind_address); 

	/*"create_socket()" then continues with creating a socket using "socket()", binding that socket to the listening address with "bind()", and having the socket enter a listening state with "listen()".*/
	/*this is the listen loop for socket thats been created and binded*/
	printf("Creating socket...\n"); 
	SOCKET socket_listen; 
	socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol); 
	if (!ISVALIDSOCKET(socket_listen))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO()); 
		exit(1); 
	}

	printf("Binding socket to local address...\n"); 
	if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO()); 
		exit(1); 
	}
	freeaddrinfo(bind_address); 

	printf("Listening...\n"); 
	if (listen(socket_listen, 10) < 0)
	{
		fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO()); 
		exit(1); 
	}

	return socket_listen; 
}
