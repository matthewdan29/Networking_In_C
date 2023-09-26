/*This address list local network addresses */

/*To start we include the necessary system headers: like below*/

#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>

/*Next, we enter the "main()" function*/

int main()
{
	struct ifaddrs *addresses;	/*"addresses" variable is declared, which stores the addresses. */

	if(getifaddrs(&addresses) == -1)	/*A call to the "getifaddrs()" function allocates memory and fills in a linked list of address. */
	{
		printf("getifaddrs call failed\n"); 
		return -1; 
	}

	/*Next, we use a new pointer, "address", to walk through the linked list of addresses. 
	 * After considering each address, we set "address = address->ifa_next" to get the next address. We stop the loop when "address == 0", which happens at the end of the linked list: */

	struct ifaddrs *address = addresses; 
	while(address)
	{
		if(address->ifa_addr == NULL){
			address = address->ifa_next;
			continue; /*This was missing from the pdf of the but but was found in the github section. if you run the code without your going to get a segfault.*/
		}
		int family = address->ifa_addr->sa_family; 
		if (family == AF_INET || family == AF_INET6)
		{
			printf("%s\t", address->ifa_name); 
			printf("%s\t", family == AF_INET ? "IPv4" : "IPv6"); 

			char ap[100];		/*We define "ap", to store textual address*/
			const int family_size = family == AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6); 
			getnameinfo(address->ifa_addr, family_size, ap, sizeof(ap), 0, 0, NI_NUMERICHOST);		/*A call to the "getnameinfo()" function fills in this buffer, which we can then print. */
			printf("\t%s\n", ap); 
		}
		address = address->ifa_next; 
	}

	/*Finally, we free the memory allocated by "getifaddrs()" and we have finished*/

	freeifaddrs(addresses); 
	return 0; 
}


