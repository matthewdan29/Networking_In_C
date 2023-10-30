/*"wait_on_clients()", blocks until an existing client sends data, or a new client attempts to connect. 
 * This function uses "select()" */

fd_set wait_on_clients(SOCKET server)
{
	fd_set reads; 
	FD_ZERO(&reads); 
	FD_SET(server, &reads); 
	SOCKET max_socket = server; 

	struct client_info *ci = clients; 

	while(ci)
	{
		FD_SET(ci->socket, &reads); 
		if (ci->socket > max_socket)
			max_socket = ci->socket; 
		ci = ci->next; 
	}

	if (select(max_socket+1, &reads, 0, 0, 0) < 0)
	{
		fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO()); 
		exit(1); 
	}

	return reads; 
}

/*"wait_on_clients()" function returns "reads" so that the caller can see which sokcet is ready. */
