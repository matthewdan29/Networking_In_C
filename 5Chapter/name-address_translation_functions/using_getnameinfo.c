/*"getnameinfo()" can be used to convert an "addrinfo" structure back into a text format. 
 * It works with both IPv4 and IPv6. 
 * It also, optionally, converts a port number into a text format number or service name. */

/*The declaration for "getnameinfo()" can be seen in the following code: */
int getnameinfo(const struct sockaddr *addr, socklen_t addrlen, char *host, socklen_t hostlen, char *serv, socklen_t servlen, int flags); 

/*The first two parameters are passed in from the "ai_addr" and "ai_addrlen" fields of "struct addrinfo". 
 * The next two parameters, "host" and "hostlen", specify a character bufer and buffer length to store the hostname or IP address txt.
 *
 * The following two parameters, "host" and "hostlen", specify a character buffer and buffer length to store the hostname or IP address text. 
 *
 * The following parameters, "serv" and "servlen", specify the buffer and length to store the service name. 
 *
 * */

/*Flags can be a bitwise OR combination of the following: 
 *
 * 	
 * 		1)"NI_NAMEREQD" requires "getnameinfo()" to return a hostname and not an address. 
 * 		2) "NI_DGRAM" specifies that the service is UDP-based rather than TCP-based. 
 *
 * 		3) "NI_NUMERICHOST" requests that "getnameinfo()" returns the IP address and not a host name. 
 *
 * 		4) "NI_NUMERICSERV" request that "getnameinfo()" returns the port number and not a service name. */


		/*For example, we can use "getnameinfo()" as follows*/

char host[100]; 
char serv[100]; 
getnameinfo(address->ai_addr, address->ai_addrlen, host, sizeof(host), serv, sizeof(serv), 0); 
printf("%s %s\n", host, serv); 

/*In the preceding code, "getnameinfo()" attempts to perform a reverse DNS lookup. 
 * This works like the DNS queries we've done so far but in reverse.*/

/*If, instead of the hostname, we would like the IP address, we can modify our code to the following*/

char host[100]; 
char serv[100]; 
getnameinfo(address->ai_addr, address->ai_addrlen, host, sizeof(host), serv, sizeof(serv), NI_NUMERICHOST | NI_NUMERICSERV); 

printf("%s %s\n", host, serv); 

/*Using "NI_NUMERICHOST" generally runs much faster too, as it doesn't require "getnameinfo()" to send off any reverse DNS queries */
