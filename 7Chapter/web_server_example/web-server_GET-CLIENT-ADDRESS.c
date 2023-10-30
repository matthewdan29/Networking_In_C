/*this function coverts a given clients IP address into text. */
const char *get_client_address(struct client_info *ci)
{
	static char address_buffer[100]; 
	getnameinfo((struct sockaddr*)&ci->address, ci->address_length, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST); 	
	return address_buffer; 
}

/*this method is not re-entrant-safe. it has global access.  */
