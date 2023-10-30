/*We begin the "main()" function. It starts by initializing Winsock on Windows*/

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
	/*The server is listening on port 8080. 
	 * We use 8080 as our port number instead of HTTP standard port 80.*/

	/*The code to create the server socket is as follows*/
	SOCKET server = create_socket(0, "8080"); 

	/*If you want to accept connections from only the local system, and not out side use the following code (please remember this server isn't secure!)*/
	SOCKET server = create_socket("127.0.0.1", "8080"); 
	
	/*Now we create a infinte loop that waits on clients. 
	 * We call "wait_on_clients()" to wait until a new client connects or an old client sends new data*/
	while(1)
	{
		fd_set reads; 
		reads = wait_on_clients(server); 

		/*The "server" then detects whether a new "client" has connected. 
		 * This case is indicated by "server" being set in "fd_set reads". 
		 * We us the "FD_ISSET()" macro to detect this condition*/
		if (FD_ISSET(server, &reads))
		{
			/*Once a new client connection has been detected, "get_client()" is called with the argument -1; -1 is not a valid socket specifier, so "get_client()" creates a new "struct client_info". 
			 * This "struct client_info" is assigned to the "client" variable.*/
			struct client_info *client = get_client(-1); 

			/*The "accept()" socket function is used to accept the new connection and place the connected client address information into the respective "client" fields. 
			 * The new socket returned by "accept()" is stored in "client->socket". */
			client->socket = accept(server, (struct sockaddr*) &(client->address), &(client->address_length)); 
			
			if (!ISVALIDSOCKET(client->socket))
			{
				fprintf(stderr, "accetp() failed. (%d)\n", GETSOCKETERRNO()); 
				return 1; 
			}
			/*The client's address is printed using a call to "get_client_address()". 
			 * this is helpful for debugging*/
			printf("New connection from %s.\n", get_client_address(client)); 
		}

		/*The server must then handle the case where an already connected client is sending data. 
		 * First walk through the linked list of clients and use "FD_ISSET()" on each client to determine which clients have data available. 
		 * The linked list root is stored in the "clients" global variable.*/
		struct client_info *client = clients; 
		while(client)
		{
			struct client_info *next = client->next; 

			if (FD_ISSET(client->socket, &reads))
			{
				/*We then check that we have memory available to store more received data for "client". 
				 * If the client's buffer is already completely full, then we send a 400 error.*/
				if (MAX_REQUEST_SIZE == client->received)
				{
					send_400(client); 
					continue; 
				}

				/*Knowing that we have at least some memory left to store received data, we can use "recv()" to store the client's data. 
				 * The following code uses "recv()" to write new data into the client's buffer while being careful to not overflow that buffer*/
				int r = recv(client->socket, client->request + client->received, MAX_REQUEST_SIZE - client->received, 0); 

				/*A client that disconnects unexpectedly causes "recv()" to return a non-positive number. 
				 * We need to use "drop_client()" to clean up our memory allocated for that client*/
				if (r < 1)
				{
					printf("Unexpected disconnect from %s.\n", get_client_address(client)); 
					drop_client(client); 

					/*HTTP header and body is delineated by a blank line.("\r\n\r\n")
					 * The following code detects whether the HTTP header has been received*/
				} else 
				{
					client->received += r; 
					client->request[client->received] = 0; 

					char *q = strstr(client->request, "\r\n\r\n"); 
					if (q)
					{
						/*Our server only handles "GET" requests. 
						 * We also enforce that any valid path should start with a slash character; "strncmp()" is used to detect these two conditions in the following code.*/
						if (strncmp("GET /", client->request, 5))
						{
							send_400(client);
						}else 
						{
							char *path = client->request + 4; 
							char *end_path = strstr(path, " "); 
							if (!end_path)
							{
								send_400(client); 
							} else 
							{
								*end_path = 0; 
								serve_resource(client, path); 
							}
						}
					}/*if (q)*/

					/*The server is basically functional at this point. 
					 * Last, finish our loops and close out the "main()" function.*/
				}
			}

			client = next; 
		}
	}/*while(1)*/

	printf("\nClosing socket...\n"); 
	CLOSESOCKET(server); 

#if defined(_WIN32)
	WSACleanup(); 
#endif 

	printf("Finished.\n"); 
	return 0; 
}

/*great way to break the loop is the check "clients" (global verable) for 2 things 
 * if there is any connected 
 * if there was any new data send in a time limit or new socket connected
 *
 * It is useful to drop all connected clients with the line of code 
 * 			"while(clients) drop_client(clients);"*/
