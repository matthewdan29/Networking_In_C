/*Here is a complete program that includes the needed socket API headers for each platform and properly initializes Winsock on Windows: */

#if defined(_WIN32)
#ifndef _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")	/*"#pragma comment(lib, "ws2_32.lib")" tells the Microsoft Visual C compiler to link your program against the winsock library, "ws2_32.lib"*/

#else 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#endif

#include <stdio.h>

int main()
{
#if defined(_WIN32)
	WSADATA d; 
	if (WSAStartup(MAKEWORD(2, 2), &d))	/*We called "WSAStartup()" on windows to initialize Winsock. The "MAKEWORD" macro allows us to request Winsock version 2.2 */
	{
		fprintf(stderr, "Failed to initialize.\n"); 
		return 1; 
	}
#endif

	printf("Ready to use socket API.\n"); 

#if defined(_WIN32)
	WSACleanup(); 		/*"WSACleanup()" is called if we're compliling for Winsock on windows. This function allows the Windows operating system to do additional cleanup. */
#endif

	return 0; 
}
