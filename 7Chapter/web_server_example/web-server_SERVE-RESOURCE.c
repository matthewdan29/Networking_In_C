/*"serve_resource()" function sends a connected client a requested resource. 
 * The server expects all hosted files to be in a subdirectory called "publiuc"
*/

/*Our "serve_resource()" function takes as arguments as connected client and a requested resource path. */

void serve_resource(struct client_info *client, const char *path)
{
	printf("serve_resource %s %s\n", get_client_address(client), path); 

	/*Below code is used to redirect root requests and to prevent long or obviously malicious request*/
	if (strcmp(path, "/") == 0)
		path = "/index.html"; 
	if (strlen(path) > 100)
	{
		send_400(client); 
		return; 
	}

	if (strstr(path, ".."))
	{
		send_404(client); 
		return; 
	}

	/*NEXT, "sprintf()" converts the path to refer to files in the "public" directory.
	 * A text-array is reserved, "full_path", and then "sprintf()" is used to store the full path into it. */
	char full_path[128]; 
	sprintf(full_path, "public%s", path); 

	/*IMPORTANT: the following code converts slashes to backslashes on windows*/
#if defined(_WIN32)
	char *p = full_path; 
	while(*p)
	{
		if (*p == '/')
			*p = '\\'; 
		++p; 
	}
#endif 

	/*Next, the server can check whether the requested resource actually exits. 
	 * This done by using the "fopen()" function. 
	 * If "fopen()" fails, for any reason, then our server assues that the file does not exits.*/
	FILE *fp = fopen(full_path, "rb"); 

	if (!fp)
	{
		send_404(client); 
		return; 
	}

	/*if "fopen()" succeeds, thren we can use "fseek()" and "ftell()" to determine the requested filled size.*/
	fseek(fp, 0L, SEEK_END); 
	size_t cl = ftell(fp); 
	rewind(fp); 

	/*This is how to store in the variable "ct" b*/
	const char *ct = get_content_type(full_path); 

	/*Once the file has been located and we have its length and type, the server can begin sending the HTTP response.*/
#define BSIZE 1024
	char buffer[BSIZE]; 

	/*Once the buffer is reserved, the server prints relevant headers into it and then sends those headers to the client. 
	 * This is done using "sprintf()" and then "send()" in turn.*/
	sprintf(buffer, "HTTP/1.1 200 OK\r\n"); 
	send(client->socket, buffer, strlen(buffer), 0); 
	
	sprintf(buffer, "Connection: close\r\n"); 
	send(client->socket, buffer, strlen(buffer), 0); 

	sprintf(buffer, "Content-Length: %u\r\n", cl); 
	send(cleint->socket, buffer, strlen(buffer), 0); 

	sprintf(buffer, "Content-Type: %s\r\n", ct); 
	send(client->socket, buffer, strlen(buffer), 0); 

	sprintf(buffer, "\r\n")	; 
	send(client->socket, buffer, strlen(buffer), 0); 

	/*The server can now send the acutal file content. 
	 * This done by calling "fread()" repeatedly until the entire file is sent*/

	int r = fread(buffer, 1, BSIZE, fp); 
	while (r)
	{
		send(client->socket, buffer, r, 0); 
		r = fread(buffer, 1, BSIZE, fp); 
	}

	/*The function can finish by closing the file handle and using "drop_client()" to disconnect the client*/
	fclose(fp); 
	drop_client(client); 
}
