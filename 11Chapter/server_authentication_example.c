/*ONce the SSH session is established, you can get the server's public key using the "ssh_get_server_publickey()" function. */
ssh_key key; 
if (ssh_get_server_publickey(ssh, &key) != SSH_OK) 
{
	fprintf(stderr, "ssh_get_server_publickey() failed.\n%s\n", ssh_get_error(ssh)); 
	return -1; 
}

/*It is often useful to obtain and display a hash of the server's SSH public key. 
 * Users can look at hashes and compare these to known keys. 
 * The "libssh" library provides the "ssh_get_publickey_hash()" function for this purpose. */

/*The following code prints out an "SHA1" hash of the public key obtained in above code*/
unsigned char *hash; 
size_t hash_len; 
if (ssh_get_publickey_hash(key, SSH_PUBLICKEY_HASH_SHA1, &hash, &hash_len) != SSH_OK)
{
	fprintf(stderr, "ssh_get_publickey_hash() failed.\n%s\n", ssh_get_error(ssh)); 
	return -1; 
}

printf("host public key hash:\n"); 
ssh_print_hash(SSH_PUBLICKEY_HASH_SHA1, hash, hash_len); 

/*"libssh" prints "SHA1" hashes using Base64. 
 * It also prepends the hash type first. 
 * */

/*When you've finished with the public key and hash, free thier resources*/
ssh_clean_pubkey_hash(&hash); 
ssh_key_free(key); 

/*"libssh" provides the "ssh_session_is_known_server()" function to determine whether a server's public key is known. */
enum ssh_known_hosts_e known = ssh_session_is_known_server(ssh); 
switch (known)
{
	case SSH_KNOWN_HOSTS_OK: 
		printf("Host known.\n");
		break; 
	case SSH_KNOWN_HOSTS_CHANGED: 
		printf("Host Changed.\n"); 
		break; 
	case SSH_KNOWN_HOSTS_OTHER: 
		printf("Host other.\n"); 
		break; 
	case SSH_KNOWN_HOSTS_UNKNOWN: 
		printf("Host Unknown.\n"); 
		break; 
	case SSH_KNOWN_HOSTS_NOT_FOUND: 
		printf("No host file.\n"); 
		break; 
	case SSH_KNOWN_HOSTS_ERROR: 
		printf("Host error. %s\n", ssh_get_error(ssh)); 
		return 1; 
	defaul: 
		printf("Error. Known: %d\n", known); 
		return 1; 
}	

/*If the user has verified that a host is to be trusted, use "ssh_session_update_known_hosts()" to allow "libssh" to save the servers public key hash. 
 * This allows "ssh_session_is_known_server()" to reutrn "SSH_KNOWN_HOST_OK" for the next connection.*/
/*The following methos illustrates prompting the user to trust a connection and using "ssh_session_update_known_hosts()"*/
if (known == SSH_KNOWN_HOSTS_CHANGED || known == SSH_KNOWN_HOSTS_OTHER || known == SSH_KNOWN_HOSTS_UNKNOWN || known == SSH_KNOWN_HOSTS_NOT_FOUND)
{
	printf("Do you want to accept and remember this host? Y/N\n"); 
	char answer[10]; 
	fgets(answer, sizeof(andser), stdin); 
	if (answer[0] != 'Y' && answer[0] != 'y')
	{
		return 0; 
	}

	ssh_session_update_known_hosts(ssh); 
}


