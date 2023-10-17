/*To perform a simple DNS query, we would put an arbitrary number into "ID", set the "RD" bit to 1, and set "QDCOUNT" to 1. 
 * We would then add a question after the header. 
 * That data would be sent as UDP packet to port 53. of a DNS server. 
 * */

char dns_query[] = 
{
	0xAB, 0xCD, 		/*ID*/
	0x01, 0x00, 		/*Recursion*/
	0x00, 0x01, 		/*QDCOUNT*/
	0x00, 0x00, 		/*ANCOUNT*/
	0x00, 0x00, 		/*NSCOUNT*/
	0x00, 0x00, 		/*ARCOUNT*/	
	7, 'e', 'x', 'a', 'm', 'p', 'l', 'e', 
	3, 'c', 'o', 'm', 
	0,			/*end of name*/
	0x00, 0x01, 		/*QTYPE = A*/
	0x00, 0x01		/*QCLASS*/
}; 

/*This data could be sent as is to a DNS server over port 53*/
