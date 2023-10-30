/*"drop_client()" function searches through our linked list of client and removes a given client. */

void drop_client(struct client_info *client)
{
	CLOSESOCKET(client->socket); 		/*To clean client connection*/

	struct client_info **p = &clients; 

	while(*p)
	{
		if (*p == client)
		{
			*p = client->next; 
			free(client); 
			return; 
		}
		p = *&(*p)->next; 
	}

	fprintf(stderr, "drop_client not found.\n"); 
	exit(1); 
}
