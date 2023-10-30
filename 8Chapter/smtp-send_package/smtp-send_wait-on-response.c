/*It is useful to have a function that waits until a particular response code is received over the network. 
 * We implement "wait_on_response()"*/

void wait_on_response(SOCKET server, int expecting)
{
	char response[MAXRESSPONSE+1]; 
	char *p = response; 
	char *end = response + MAXRESPONSE; 

	int code = 0; 

	/*A "response" buffer varable is reserved for storing the SMTP server's response . 
	 * A pointer 'p' is set to the begining of the buffer; 'p' will be incremented to the end of the received data, but it starts at "response" since no data has been received yet. 
	 * An "end" pointer varable is set to the end of the buffer,  which is useful to endsure we do not attempt to write past the buffer end. */

	do 
	{
		int bytes_received = recv(server, p, end - p, 0); 
		if (bytes_received < 1)
		{
			fprintf(stderr, "Connection drooped.\n"); 
			exit(1); 
		}
		
		p += bytes_received; 
		*p = 0; 

		if (p == end)
		{
			fprintf(stderr, "Server response too large:\n"); 
			fprintf(stderr, "%s", response); 
			exit(1); 
		}

		code = parse_response(response); 
	}while (code == 0); 

	/*The beginning of the preceding loop uses "recv()" to receive data from the SMTP server. 
	 * The receive data is written at point 'p' in the "response" array. 
	 * We are careful to use "end" ro make sure received data isn't written past the end of "response". */

	/*After the loop terminates, the "wait_on_response()" function checks that the received SMTP response code is as expected. 
	 * If so the received data is printed to the screen, and the function returns. */
	if (code != expecting)
	{
		fprintf(stderr, "Error from server:\n"); 
		fprintf(stderr, "%s", response); 
		exit(1); 
	}

	printf("S: %s", response); 
}
