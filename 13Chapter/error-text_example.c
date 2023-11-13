/*In a real-world program, you may want to display a text based error message in addition to the error code. 
 * We can build a simple function to return the last error message as a C string*/
const char *get_error_text()
{
#if defined(_WIN32)

	static char message[256] = {0}; 
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0, WSAGetLastError(), 0, message, 256, 0); 
	char *nl = strrchr(message, '\n'); 
	if (nl)
		*nl = 0; 
	return message;

#else 
	return strerror(errno);
#endif
}
/*The preceding function formats the error as text using "FormatMessage()" on windows and "strerror()" on other operating systems. */

/*This program calls "socket()" function with invalid parameters, and then uses "get_error_text()" to display the error message*/
printf("Calling socket() with invalid parameters.\n"); 
socket(0,0,0); 
printf("Last error was: %s\n", get_error_text()):

/*Note that error codes and descriptions vary greatly between operation systems.*/	
