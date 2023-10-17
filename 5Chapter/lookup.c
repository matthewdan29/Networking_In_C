/*To begin with, we need to include our required header file. 
 * We also define "AI_ALL" for systems that are missing it.*/
#include "chap05.h"

#ifndef AI_ALL
#define AI_ALL 0X0100
#endif

/*We can begin the "main()" function and check that the user passed in a hostname to lookup. 
 * If the user doesn't pass in a hostname, we print a helpful reminder.*/
int  main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Usage:\n\tlookup hostname\n"); 
		printf("Example:\n\tlookup example.com\n"); 
		exit(0); 
	}

	/*we need the following code to initialize winsock on windows*/
#if defined(_WIN32)
	WSADATA d; 
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		fprintf(stderr, "Failed to initialize.\n"); 
		return 	1; 
	}
#endif 

	/*We can then call "getaddrinfo()" to convert the hostname or address into a "struct addrinfo".*/
	printf("Resolving hostname '%s'\n", argv[1]);	/*This prints the host name or address that is passed in as the first command-line argument*/ 
	struct addrinfo hints; 
	memset(&hints, 0, sizeof(hints)); 
	hints.ai_flags = AI_ALL; 	/*We are setting the flag that specify that we want all available addresses of any type, including IPv4 and IPv6*/
	struct addrinfo *peer_address; 
	if (getaddrinfo(argv[1], 0, &hints, &peer_address))	/*If the user passed in an invalid address or a hostname for which no record could be found, then "getaddrinfo()" returns a non-zero value.*/
	{
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO()); 
		return 1; 
	}

	/*Next, "peer_address" holds the deired address(es), we can use "getnameinfo()" to convert them to text.*/
	printf("Remote address is:\n"); 
	struct addrinfo *address = peer_address; 	/*"peer_address" is now in the "*address" pointer. */
	do 
	{
		char address_buffer[100]; 	/*"address_buffer[]" is declared to store the text address, and we call "getnameinfo()" to fill in that address*/
		getnameinfo(address->ai_addr, address->ai_addrlen, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST); 	/*"NI_NUMERICHOST" indicates that we want it to put the ip address into "address_buffer" and not a  hostname*/
		printf("\t%s\n", address_buffer); 
	} while ((address = address->ai_next)); 

	/*After we've printed out address, we should use "freeaddrinfo()" to free the memory allocated by "getaddrinfo()". 
	 * We should also call the winsock cleanup function on windows. */

	freeaddrinfo(peer_address); 

#if defined(_WIN32)
	WSACleanup(); 
#endif 
	return 0; 
}
