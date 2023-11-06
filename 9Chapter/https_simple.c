#include "chap09.h"

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
	/*Next, initialize the OpenSSL library*/
	SSL_library_init(); 
	OpenSSL_add_all_algorithms(); 
	SSL_load_error_strings(); 

	/*We can also create an OpenSSL context. 
	 * This is done by calling "SSL_CTX_new()"*/
	SSL_CTX *ctx = SSL_CTX_new(TLS_client_method()); 
	if (!ctx)
	{
		fprintf(stderr, "SSL_CTX_new() failed.\n"); 
		return 1; 
	}

	/*if (!SSL_CTX_load_verify_locations(ctx, "trusted.pem", 0))*/
	/*{*/
	/*	fprintf(stderr, "SSL_CTX_load_verify_locations() failed.\n"); */
		/*ERR_print_errors_fp(stderr); */
		/*return 1; */
	/*}*/

	/*Our program then checks that a hostname and port number was passed in on the command line*/
	if (argc < 3)
	{
		fprintf(stderr, "usage: https_simple hostname port\n"); 
		return 1; 
	}

	char *hostname = argv[1]; 
	char *port = argv[2]; 

	/*Next, configure the remote address for the socket connection.*/
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

	printf("Remote address is:"); 
	char address_buffer[100]; 
	char service_buffer[100]; 
	getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen, address_buffer, sizeof(address_buffer), service_buffer, sizeof(service_buffer), NI_NUMERICHOST); 	
	printf("%s %s\n", address_buffer, service_buffer); 

	/*We continue to create the socket using a call to "socket()", and we connect it using the "connect()" function*/
	printf("Creating socket...\n"); 
	SOCKET server; 
	server = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol); 
	if (!ISVALIDSOCKET(server))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO()); 
		exit(1); 
	}

	printf("Connectiong...\n"); 
	if (connect(server, peer_address->ai_addr, peer_address->ai_addrlen))
	{
		fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO()); 
		exit(1); 
	}

	freeaddrinfo(peer_address); 
	printf("Connected.\n\n"); 

	/*At this point, a TCP connection has been established. 
	 * We are going to use OpenSSL to initiate a TLS/SSL connection over our TCP connection. 
	 * Below creates new SSL object, sets the hostname for SNI, and initiates the TLS/SSL handshake.*/
	SSL *ssl = SSL_new(ctx); 
	if (!ctx)
	{
		fprintf(stderr, "SSL_new() failed.\n"); 
		return 1; 
	}

	if (!SSL_set_tlsext_host_name(ssl, hostname))
	{
		fprintf(stderr, "SSL_set_tlsext_host_name() failed.\n"); 
		ERR_print_errors_fp(stderr); 
		return 1; 
	}

	SSL_set_fd(ssl, server); 
	if (SSL_connect(ssl) == -1)
	{
		fprintf(stderr, "SSL_connect() failed.\n"); 
		ERR_print_errors_fp(stderr); 
		return 1; 
	}

	/*Next, we can print the selected cipher*/
	printf("SSL/TLS using %s\n", SSL_get_cipher(ssl)); 

	/*Next, look at the servers certificate*/
	X509 *cert = SSL_get_peer_certificate(ssl); 
	if (!cert)
	{
		fprintf(stderr, "SSL_get_peer_certificate() failed.\n"); 
		return 1; 
	}

	char *tmp; 
	if ((tmp = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0)))
	{
		printf("subject: %s\n", tmp); 
		OPENSSL_free(tmp); 
	}

	if ((tmp = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0)))
	{
		printf("issuer: %s\n", tmp); 
		OPENSSL_free(tmp); 
	}

	X509_free(cert); 

	/*We can then send our HTTPS request. 
	 * This request is the same as if we were using plain HTTP. 
	 * We begin by formatting the request into a buffer and then sending it over the encrypted connection using "SSL_write()"*/
	char buffer[2049]; 

	sprintf(buffer, "GET / HTTP/1.1\r\n"); 
	sprintf(buffer + strlen(buffer), "Host: %s:%s\r\n", hostname, port); 
	sprintf(buffer + strlen(buffer), "Connection: close\r\n"); 
	sprintf(buffer + strlen(buffer), "User-Agent: https_simple\r\n"); 
	sprintf(buffer + strlen(buffer), "\r\n"); 

	SSL_write(ssl, buffer, strlen(buffer)); 
	printf("Sent Headers:\n%s", buffer); 

	/*Our client now simply waits for data from the server until the connection is closed. 
	 * This is accomplished by using "SSL_read()" in a loop. 
	 * Below code receives the HTTPS response*/
	while(1)
	{
		int bytes_received = SSL_read(ssl, buffer, sizeof(buffer)); 
		if (bytes_received < 1)
		{
			printf("\nConnection closed by peer.\n"); 
			break; 
		}

		printf("Received (%d bytes): '%.*s'\n", bytes_received, bytes_received, buffer); 
	}

	/*Now, we only have to shut down the TLS/SSL connection, close the socket, and clean up. */
	printf("\nClosing socket...\n"); 
	SSL_shutdown(ssl); 
	CLOSESOCKET(server); 
	SSL_free(ssl); 
	SSL_CTX_free(ctx); 

#if defined(_WIN32)
	WSACleanup(); 
#endif 

	printf("Finished.\n"); 
	return 0; 
}
