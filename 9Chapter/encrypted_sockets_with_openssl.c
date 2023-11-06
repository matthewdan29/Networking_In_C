/*The TLS provided by OpenSSL can be applied to any TCP socket.*/

/*It is important to initialize OpenSSL in you program*/
SSL_library_init(); 		/*Required to initialize the OpeenSSL library*/
OpenSSL_add_all_algorithms(); 	/*This causes OpenSSL to load all avalilable algroithms.*/
SSL_load_error_strings(); 	/*Causes OpenSSL to load error strings.*/


/*Once OpenSSL is initialized, we are ready to create an SSL context. 
 *"SSL_CTX_new()" function, which returns an "SSL_CTX" object. 
 You can think of this object as a sort of factory for SSL/TLS connections. 
 It holds the initial settings that you want to use for you connections.*/

/*Below creates the SSL context*/
SSL_CTX *ctx = SSL_CTX_new(TLS_client_method()); 	/*"TLS_client_method()" indicates that we want general-purpose, version-flexible TLS methods available.*/
if (!ctx)
{
	fprintf(stderr, "SSL_CTX_new() failed.\n");
	return 1; 
}

/*To secure a TCP connection, you must first have a TCP connection. 
 * This TCP connection should be established in the normal way. 
 * like below*/
getaddrinfo(hostname, port, hints, address); 
socket = socket(address, type, protocol); 
connect(socket, address, type); 

/*Once "connect()" has returned successfully, and a TCP connection is establisthed, you can use the below code to initiate a TLS connection*/
SSL *ssl = SSL_nex(ctx); 	/*"*ssl" object is used to track the new SSL/TLS connection*/
if (!ctx)
{
	fprintf(stderr, "SSL_new() failed.\n"); 
	return 1; 
}

if (!SSL_set_tlsext_host_name(ssl, hostname))
{
	fprintf(stderr, "SSL_set_tlsext_host_name() failed.\n"); 
	ERR_print_error_fp(stderr); 
	return 1; 
}

SSL_set_fd(ssl, socket); 	/*"SSL_set_fd" is used to initiate the new TLS/SS connection in our exitsting TCP*/
if (SSL_connect(ssl) == -1)
{
	fprintf(stderr, "SSL_connect() failed.\n"); 
	ERR_print_errors_fp(stderr); 
	return 1;
}

/*It is possible to see which cipher the TLS connection is using. look below*/
printf("SSL/TLS using %s\n", SSL_get_cipher(ssl)); 

/*Once the TLS connection is established, data can be sent and received using "SSL_write()" and "SSL_read()" respectively. 
 * They are just like "send()" and "recv()"*/

/*Below show transmitting a simple message over a TLS connection*/
char *data = "Hello World!"; 
int bytes_sent = SSL_write(ssl, data, strlen(data)); 

/*Receiving data is done with "SSL_read()"*/
char read[4096]; 
int bytes_received = SSL_read(ssl, read, 4096); 
printf("Received: %.*s\n", bytes_received, read); 

/*When the connection is finished, its important to free the used resources by calling "SSL_shutdown()" and "SSL_free(ssl)"*/
SSL_shutdown(ssl); 
CLOSESOCKET(socket);
SSL_free(ssl); 

/*When you're done with an SSL context you should also call "SSL_CTX_free()"*/
SSL_CTX_free(ctx); 


