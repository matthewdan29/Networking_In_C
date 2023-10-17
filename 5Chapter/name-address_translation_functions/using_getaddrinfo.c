/*The declaration for "getaddrinfo()" is shown in the following code.*/

int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res); 

/*	
 *		1) "*node" specifies a hostname or address as a string. 
 *		valid examples could be "example.com", "192.168.1.1", or "::1"
 *
 *		2) "*service" specifies a service or port number as a string. 
 *
 *		3) "*hints" is a pointer to a "struct addrinfo", which specifies options for selecting the address. 
 *		The "addrinfo" struct has the following fiels*/

struct addrinfo
{
	int	ai_flags; 
	int 	ai_family; 
	int 	ai_socktype; 
	int 	ai_protocol; 
	int 	ai_addrlen; 
	socklen_t	ai_addrlen; 
	struct sockaddr		*ai_addr; 
	char 	*ai_canonname; 
	struct addrinfo		*ai_next; 
}; 


		/*The call to "getaddrinfo()" looks at only four fields in "*hints"
		 * The rest of the structure should be zeroed-out. 
		 * The relevant fields are "ai_family", "ai_socktype", "ai_protocol", and "ai_flags": 
		 *
		 * 1) "ai_family" specifies the desire address family. 
		 * It can be "AF_INET" for IPv4, "AF_INET6" for IPv6 or "AF_UNSPEC" for any address family. 
		 * "AF_UNSPEC" is defined as 0
		 *
		 * 2) "ai_socktype" could be "SOCK_STREAM" for TCP, or "SOCK_DGRAM" for UDP. 
		 * Setting "ai_socktype" to 0 indicates that the address could be used for either. 
		 *
		 * 3) "ai_protocol" should be left to 0. 
		 * Strictly speacking, TCP isn't the only streaming protocol supported by the socket interface, and UDP isn't the only datagram protocol supoorted. 
		 * "ai_protocol" is used to disambiguate, but its not needed for our purposes. 
		 *
		 * 4) "ai_flags" specifies additional options about "getaddrinfo()" should work. 
		 * Multiple flages can be used by bitwise OR-ing them together. 
		 * In C, the bitwise OR operator uses the pipe symbol, '|'. 
		 * So, bitwise OR-ing two flags together would use the "(flag_one | flag_two)" code. */

		

		/*Common flags you may use for "ai_flags" field are:
		 *
		 * 1)"AI_NUMERICHOST" can be used to prevent name lookups. 
		 *
		 * 2) "AI_NUMERICSERV" can be used to only accept port numbers for the "service" argrument. 
		 *
		 * 3) "AI_ALL" can be used to request both an IPv4 and IPv6 address. 
		 *
		 * 4) "AI_ADDRCONFIG" forces "getaddrinfo()" to only return addresses that match the family type of a configured interface on the local machine. 
		 *
		 * 5) "AI_PASSIVE" can be used with "node = 0" to request the wildcard addres. */

		/*The final parameter to "getaddrinfo()", "res", is a pointer to a pointer to "struct addrinfo" and returns the addresss(es) found by "getaddrinfo()". */

/*Below is an example of using "getaddrinfo()" to find the address(es) for "example.com"*/

struct addrinfo hints; 
memset(&hints, 0, sizeof(hints)); 
hints.ai_flags = AI_ALL; 
	struct addrinfo *peer_address; 
if (getaddrinfo("example.com", 0, &hints, &peer_address))
{
	fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO()); 
	return 1; 
}


	/*We can loop through all the returned addresses with the following code: */

struct addrinfo *address = peer_address; 
do 
{
	/*work with address...*///ya u know a do loop

}	while ((address = address->ai_next)); 

/*When we've finished using "peer_address", we should free it with the following code*/

freeaddrinfo(peer_address); 

