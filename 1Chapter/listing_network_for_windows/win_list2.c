/*To start, you need to define "_WIN32_WINNT"*/

/* The "_WIN32_WINNT" macro must be defined first so that the proper version of the Windows headers are included.*/
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

/*Next, we include the follwing progmas to tell Microsoft Visual C which libraries must be linked with the executable: below */

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

/*Next, we then enter the "main()" function and initialize Winsock 2.2 using "WSAStartup()" as described earlier. We check its return value to detect any errors*/

int main()
{
	WSADATA d; 
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		printf("Failed to initialize.\n"); 
		return -1; 
	}

	/*Next, we allocate memory for the adapter, and we request the adapters' addresses from Windows using the "GetAdapterAddresses()" function:*/

	DWORD asize = 20000;	/*"asize" variable will store the sixe of our adapters' address buffer.*/ 
	PIP_ADAPTER_ADDRESSES adapters; 
	do
	{
		adapters = (PIP_ADAPTER_ADDRESSES)malloc(asize);	/*"malloc" allocates space in for objects*/

		if (!adapters)
		{
			printf("Couldn't allocate %ld bytes for adapters.\n", asize); 
			WSACleanup(); 
			return -1; 
		}

		int r = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, 0, adapters, &asize);	/*First parameter, "AF_UNSPEC" tells Windows that we ant both IPv4 and IPv6 addresses.Second parameter, "GAA_FLAG_INCLUDE_PREFIX", is required to request a list of addresses. 
										Next parameter is reserved and shodl be passes in as '0' or "NULL". 
										Last parameter, we pass in our buffer, "Addapters", and a pointer to its size, "&asize"*/
		if (r == ERROR_BUFFER_OVERFLOW)
		{
			printf("GetAdaptersAddresses wants %ld bytes.\n", asize); 
			free(adapters); 
		}
		else if (r == ERROR_SUCCESS)
		{
			break; 
		}
		else 
		{
			printf("Error from GetadaptersAddress: %d\n", r); 
			free(adapters); 
			WSACleanup(); 
			return -1; 
		}
	}while(!adapters); 

	/*Our next step is to loop through this linked list and print information or for each adapter and address: below*/

	PIP_ADAPTER_ADDRESSES adapter = adapters;	/*The new variable, "adapter", which we use to walk through the linked list of adapters.*/
	while(adapter)
	{
		printf("\nAdapter name: %S\n", adapter->FriendlyName); 

		PIP_ADAPTER_UNICAST_ADDRESS address = adapter->FirstUnicastAddress;	/*This is the first address for each adapter*/
		while (address)
		{
			printf("\t%s", address->Address.lpSockaddr->sa_family == AF_INET ? "IPv4" : "IPv6");	/*The "address->Address.lpSockaddr->sa_family" variable stores the address family type.*/

			char ap[100]; 	/*We allocate a buffer, "ap", to store the text representation of the address*/

			getnameinfo(address->Address.lpSockaddr, address->Address.iSockaddrLength, ap, sizeof(ap), 0, 0, NI_NUMERICHOST); 
			printf("\t%s\n", ap);	/*The "getnameinfo()" function is called to convert the address into a standard notation adress.*/

			address = address->Next; 
		}
		adapter = adapter->Next;	/*At the end of each loop, we set "adapter = adapter->Next" to get the next adapter*/
	}

	/*last, finish the program by freeing the allocated memory and calling "WSACleanup()" */

	free(adapters); 
	WSACleanup(); 
	return 0; 
}
