#include "chap10.h"

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

	/*The OpenSSL library is then initialized*/
	SSL_library_init(); 
	OpenSSL_add_all_algorithms(); 
	SSL_load_error_strings(); 

	/*An SSL context object must be created for our server. 
	 * This is done using a call to "SSL_CTX_new()". */
	SSL_CTX *ctx = SSL_CTX_new(TLS_server_method()); 
	if (!ctx)
	{
		fprintf(stderr, "SSL_CTX_new() failed.\n"); 
		return 1; 
	}

	/*Once the SSL context has been created, we can associate our server's certificate with it. 
	 * Below sets the SSL conetxt to use our certificate*/
	if (!SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) || !SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM))
	{
		fprintf(stderr, "SSL_CTX_use_certificate_fail() failed.\n"); 
		ERR_print_errors_fp(stderr); 
		return 1; 
	}

	/*Once the SSL context is configured with the proper certificate, the program creates a listening TCP socket the normal way. */
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
	if (!ISVALIDSOCKET(socket_listen))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO()); 
		return 1; 
	}

	/*The socket created by the preceding code is bound to the listening address with "bind()". 
	 * The "listen()" function is used to set the socket in a listening state*/
	printf("Binding socket to local address...\n"); 
	if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO()); 
		return 1; 
	}
	freeaddrinfo(bind_address); 


	printf("Listening...\n"); 
	if (listen(socket_listen, 10) < 0)
	{
		fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO()); 
		return 1; 
	}

	/*The server uses a "while" loop to accept multiple connections.
	 * It proves convenient for testing purposes to be able to handle multiple connections serially. 
	 * "while" loop begins by using "accpet()" to wait for new connections*/
	while(1)
	{
		printf("Waiting for connection...\n"); 
		struct sockaddr_storage client_address; 
		socklen_t client_len = sizeof(client_address); 
		SOCKET socket_client = accept(socket_listen, (struct sockaddr*) &client_address, &client_len); 
		if (!ISVALIDSOCKET(socket_client))
		{
			fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO()); 
			return 1; 
		}

		/*Once the connection is accpeted, we use "getnameinfo()" to print out the client's address. 
		 * This is good for debugging*/
		printf("Client is connected..."); 
		char address_buffer[100]; 
		getnameinfo((struct sockaddr*)&client_address, client_len, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST); 
		printf("%s\n", address_buffer); 

		/*Once the TCP connection is established, and "SSL" object needs to be created. 
		 * This is done with a call to "SSL_new()"*/
		SSL *ssl = SSL_new(ctx); 
		if (!ctx)
		{
			fprintf(stderr, "SSL_new() failed.\n"); 
			return 1; 
		}

		/*The SSL object is associated with the open socket by a call to "SSL_set_fd()". 
		 * Then, a TLS/SSL connection can be initialized with a call to "SSL_accept()"*/
		SSL_set_fd(ssl, socket_client); 
		if (SSL_accept(ssl) <= 0)
		{
			fprintf(stderr, "SSL_accpet() failed.\n"); 
			ERR_print_errors_fp(stderr); 

			SSL_shutdown(ssl); 
			CLOSESOCKET(socket_client); 
			SSL_free(ssl); 

			continue; 
		}

		printf("SSL connection using %s\n", SSL_get_cipher(ssl)); 

		/*Once the TCP and TLS/SSL connections are fully open, we use "SSL_read()" to receive the clients's request. 
		 * The program ignores the content of the this request. 
		 * Now "SSL_read" wait on and read the clients reques*/
		printf("Reading request...\n"); 
		char request[1024]; 
		int bytes_received = SSL_read(ssl, request, 1024); 
		printf("Received %d bytes.\n", bytes_received); 

		/*The following code uses "SSL_write" to transmit the HTTP headers to the client: */
		printf("Sending response...\n"); 
		const char *response = 
			"HTTP/1.1 200 OK\r\n"
			"Connection: close\r\n"
			"Content-Type: text/plain\r\n\r\n"
			"Local time is: "; 
		int bytes_sent = SSL_write(ssl, response, strlen(response)); 
		printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(response)); 

		/*The "time()" and "ctime()" functions are then used to format the current time. 
		 * Once the time is formatted in "time_msg", it is also sent to the client using "SSL_write()". */
		time_t timer; 
		time(&timer); 
		char *time_msg = ctime(&timer); 
		bytes_sent = SSL_write(ssl, time_msg, strlen(time_msg)); 
		printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(time_msg)); 

		/*After the data is transmitted to the client, the connection is closed, and the loop repeats.*/
		printf("Closing connection..\n"); 
		SSL_shutdown(ssl); 
		CLOSESOCKET(socket_client); 
		SSL_free(ssl); 
	}

	/*If the loop terminates, it would be useful to close the listening socket and clean up the SSL contxt*/
	printf("Closing listening socket...\n"); 
	CLOSESOCKET(socket_listen); 
	SSL_CTX_free(ctx); 

#if defined(_WIN32)
	WSACleanup(); 
#endif 

	printf("Finished.\n"); 
	
	return 0; 
}
