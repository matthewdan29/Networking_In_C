/*This is for C programs to know what your local address is.*/

/*Here is a small 'C' program, "win_init.c", showing the initialization and cleanup of Winsock: below*/

#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")	/*If you are using Microsoft Visual C compiler, then "#pragma comment(lib, "ws2_32.lib")" tells Microsoft Vidsual C to link the executable with the Winsock library, "ws2_32.lib"*/

int main()
{
	WSADATA d; 

	if (WSAStartup(MAKEWORD(2,2), &d)) /*The "WSADATA" structure will be filled in by "WSAStartup()" with details about the Windows Sockets implementation.*/
	{
		printf("Failed to initialize.\n"); 
		return -1; 
	}

	WSACleanup(); 
	printf("ok.\n"); 
	return 0; 
}
