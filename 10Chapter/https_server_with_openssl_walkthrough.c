/*Before OpenSSL can be used, it must be initialized. 
 * Below initializes the OpenSSL library, loads the requesite encryption algorithms, and loads useful error strings*/
SSL_library_init(); 
OpenSSL_add_all_algorithms(); 
SSL_load_error_strings(); 

/*The server also needs to create an SSL context object. 
 * This object works as a sort of factory from wich we can create TLS/SSL connections
 * The below code creates "SSL_CTX" object*/
SSL_CTX *ctx = SSL_CTX_new(TLS_server_method()); 
if (!ctx)
{
	fprintf(stderr, "SSL_CTX_new() failed.\n"); 
	return 1; 
}

/*After the "SSL_CTX" object is created, we can set it to use our self-signed certificate and key. */
if (!SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) || !SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM))
{
	fprintf(stderr, "SSL_CTX_use_certificate_file() failed.\n"); 
	ERR_print_errors_fp(stderr); 
	return 1; 
}

/*After a new TCP connection is established, we use the socket returned by "accept()" to create our TLS/SSL socket*/
SSL *ssl = SSL_new(ctx); 
if (!ctx)
{
	fprintf(stderr, "SSL_new() failed.\n"); 
	return 1;
}

/*The "SSL" object is then linked to our TCP socket using "SSL_set_fd()". 
 * The "SSL_accept()" function is called to establish the TLS/SSL connection. */
SSL_set_fd(ssl, socket_client); 
if (SSL_accept(ssl) <= 0)
{
	fprintf(stderr, "SSL_accept() failed.\n"); 
	ERR_print_errors_fp(stderr); 
	return 1; 
}

printf("SSL connection using %s\n", SSL_get_cipher(ssl)); 

/*Once the TLS connection is established, data can be sent and received using "SSL_write()" and "SSL_read()". 
 * These functions replace the "send()" and "recv()" functions used with TCP sockets. 
 *
 * When the connection is finished, it is important to free resources like the example below */
SSL_shutdown(ssl); 
CLOSESOCKET(socket_client); 
SSL_free(ssl); 

/*When your program is finished accepting new connections, you should also free the SSL context object. like below*/
SSL_CTX_free(ctx); 

