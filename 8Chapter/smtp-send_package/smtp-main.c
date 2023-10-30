int main()
{
#if defined(_WIN32)
	WSADATA d; 
	if (WSAStartup(MAKEWORD(2,2), &d))
	{
		fprintf(stderr, "Failed to initialize.\n"); 
		return 1; 
	}
#endif

	/*Our program can proceed by prompting the user for an SMTP hostname. 
	 * This hostname is stored in "hostname", and our "connect_to_host()" function is used to open a connection. */
	char hostname[MAXINPUT]; 
	get_input("mail server: ", hostname); 

	printf("Connecting to host: %s:25\n", hostname); 

	SOCKET server = connect_to_host(hostname, "25"); 

	/*After the connection is established, our SMTP client must not issue any commands until the server responds with a 220 code. 
	 * We use "wait_on_response()" to wait for*/
	wait_on_response(server, 220); 

	/*Once the server is ready to receive commands, we must issue the "HELO" command. 
	 * The following code sends the "HELO" command and waits for a 150 response code: */
	send_format(server, "HELO HONPWC\r\n"); 
	wait_on_response(server, 250); 

	/*Our program then prompts the user for the sending and receiving addresses and issues the appropriate SMTP commands. 
	 * This is done using "get_input()" to prompt the user, "send_format()" to issue the SMTP commands and "wait_on_response()" to receive the SMTP server's response*/
	char sender[MAXINPUT]; 
	get_input("from: ", sender); 
	send_format(server, "MAIL FROM:<%s>\r\n", sender); 
	wait_on_response(server, 250); 
	
	char recipient[MAXINPUT]; 
	get_input("to: ", recipient); 
	send_format(server, "RCPT TO:<%s>\r\n", recipient); 
	wait_on_response(server, 250); 

	/*After the sender and receiver are specified, the next step in the SMTP is to issue the "DATA" command. 
	 * The "DATA" command instructs the server to listen for actual email.*/
	send_format(server, "DATA\r\n"); 
	wait_on_response(server, 354); 

	/*Our client program then prompts the user for an email subjectline. 
	 * After the subject line is specified, it can send the email header: FO"FROM", "TO", and "Subject". */
	char subject[MAXINPUT]; 
	get_input("subject: ", subject); 

	send_format(server, "From:<%s>\r\n", sender); 
	send_format(server, "To:<%s>\r\n", recipient); 
	send_format(server, "Subject:%s\r\n", subject); 

	/*It is also useful to add a data header. Emails use a special format for dates. 
	 * We can make use of the "strftime()" function to format the date properly. */
	time_t timer; 
	time(&timer); 

	struct tm *timeinfo; 
	timeinfo = gmtime(&timer); 

	char date[128]; 
	strftime(date, 128, "%a, %d %b %Y %H:&M:%S +0000", timeinfo); 

	send_format(server, "Date:%s\r\n", date); 

	/*After the email headers are sent, the email body is delineated by a blank line.*/
	send_format(server, "\r\n"); 

	/*The following code sends user input to the server until a single period is inputted. */
	printf("Enter your email text, end with \".\" on a line by itself.\n"); 

	while(1)
	{
		char body[MAXINPUT]; 
		get_input("> ", body); 
		send_format(server, "%s\r\n", body); 
		if (strcmp(body, ".") == 0)
		{
			break; 
		}
	}

	wait_on_response(server, 250); 

	send_format(server, "QUIT\r\n"); 
	wait_on_response(server, 221); 

	/*last closing the socket, cleaning up and exiting the program */

	printf("\nClosing socket...\n"); 
	CLOSESOCKET(server); 

#if defined(_WIN32)
	WSACleanup(); 
#endif 

	printf("Finished.\n"); 
	return 0; 
}
