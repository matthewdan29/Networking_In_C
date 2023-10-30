/*"get_client()" function accepts a "SOCKET" and searches through the linked list of connected clients to return the relevant "client_info" for the "SOCKET". 
 * If no matthcing "client_info" is found in the linked list, then a new "client_info" is allocated and added to the linked list. */
/*if you read through my algo repo this whole operation for a critcal part of the web should VERY VERY VERY simalar to the point you can talk through the code.*/

struct client_info *get_client(SOCKET s)
{
	struct client_info *ci = clients; 

	while(ci)
	{
		if (ci->socket == s)
			break; 
		ci = ci->next; 
	}

	if (ci)
		return ci; 

	/*Next is the search operation for the linked list. 
	 * the "head" is stored in "ci" and it walks throught the list. 
	 * If "head" is the socket we are looking for it breaks. 
	 * Or it keeps the search on.*/
	struct client_info *n = (struct client_info*) calloc(1, sizeof(struct client_info)); 

	if (!n)
	{
		fprintf(stderr, "Out of memory.\n"); 
		exit(1); 
	}

	n->address_length = sizeof(n->address); 
	n->next = clients; 
	clients = n; 
	return n; 
}

/*The "calloc()" function is used to allocate memory for a new "client_info" structure. 
 * "calloc()" function also zeroes-out the data structure, which is useful in this case. 
 * The cod then chekcs that the memory allocation succeeded, and it prints an error message if it fails. */
