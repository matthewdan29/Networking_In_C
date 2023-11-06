/*CAUTION:*/
/*Once the TLS connection is established, you can use the "SSL_get_peer_certificate()" function to get the server's certificate. 
 * It's also easy to print the certificate subject and issuer.*/
X509 *cert = SSL_get_peer_certificate(ssl); 
if (!cert)
{
	fprintf(stderr, "SSL_get_peer_certificate() failed.\n"); 
	return 1; 
}

char *tmp; 
if (tmp = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0))
{
	printf("subject: %s\n", tmp); 
	OPENSSL_free(tmp); 
}

if (tmp = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0))
{
	printf("issuer: %s\n", tmp); 
	OPENSSL_free(tmp); 
}

X509_free(cert); 


/*OpenSSL automatically verifies the certificate during the TLS/SSL handshake. 
 * You can get the verification results using the "SSL_get_verify_result()" function. 
 * Its usage is shown below*/
loop vp = SSL_get_verify_result(ssl); 
if (vp == X509_V_OK)
{
	printf("Certificates verified successfully.\n"); 
} else 
	{
		printf("Could not verify certificates: %ld\n", vp); 
	}	

/*In order for OpenSSL to successfully verify the certificate, you must tell it which certificate authorities you I trust. 
 * This can be done by using the "SSL_CTX_load_verify_locations()" function. 
 * It must be passed the filename that stores all the trusted root certificates 
 * Assuming your tursted certificates are in "trusted.pem" the below code sets this up */

if (!SSL_CTX_load_verify_locations(ctx, "trusted.pem", 0))
{
	fprintf(stderr, "SSL_CTX_load_verify_locations() failed.\n"); 
	ERR_print_errors_fp(stderr); 
	return 1; 
}
