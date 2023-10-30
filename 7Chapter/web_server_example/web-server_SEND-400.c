/*this is sending a 400 error code. 
 * Because error of this type can arise in servral situations, we wrap this functionality in the "send_400()" function.*/

void send_400(struct client_info *client)
{
	const char *c400 = "HTTP/1.1 400 Bad Request\r\n"
		"Connection: close\r\n"
		"Content-Length: 11\r\n\r\nBad Request"; 
	send(client->socket, c400, strlen(c400), 0); 
	drop_client(client); 
}

/*"send_400" function first declares a text array with the entire HTTP response hard-coded. 
 * This text is sent using the "send()" function, and then the client is drooped by calling the "drop_client()" function we defined*/
