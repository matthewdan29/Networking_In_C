/*"send_format()" takes a socket, a formatting string, and the additional arguments to send. 
 * You can think of "send_format()" as being very similar to "printf()". 
 * The differecne is that "send_format()" delivers the formatted text over the network instead of printing to the screen. */

void send_format(SOCKET server, const char *text, ...)
{
	char buffer[1024]; 
	va_list args; 
	va_start(args, text); 
	vsprintf(buffer, text, args);	/*"vsprintf()" is used to format the text into the buffer.*/ 
	va_end(args); 

	send(server, buffer, strlen(buffer), 0); 

	printf("C: %s", buffer); 
}
