/*The declaration for our name-printing function looks like the code below*/
const unsigned char *print_name(const unsigned char *msg, const unsigned char *p, const unsigned char *end)
{
	if (p + 2 > end)
	{
		fprintf(stderr, "End of message.\n"); 
		exit(1); 
	}
	
	/*We then check to see if 'p' points to a name pointer. 
	 * IFF, we interpret the pointer and call "print_name" recursively to print the name that is pointed to.*/
	if ((*p & 0xC0) == 0xC0)
	{
		const int k = ((*p & 0x3F) << 8) + p[1];	/*We use the statment that 'k' is being assigned to for checking a name pointer.
		We take the lower 6 bits of "*p" and all 8 bit of "p[1]" to indicate the pointer.*/ 
		p += 2; 
		printf(" (pointer %d) ", k); 
		print_name(msg, msg+k, end); 
		return p; 
		
		/*If the name is not a pointer, we simply print it one label at a time.*/
	} else 
	{
		const int len = *p++; 
		if (p + len + 1 > end)
		{
			fprintf(stderr, "End of message.\n"); 
			exit(1); 
		}
		printf("%*s", len, p); 
		p += len; 
		if (*p)
		{
			printf("."); 
			return print_name(msg, p, end); 
		} else 
		{
			return p+1; 
		}
	}

}

/*Using "print_name()" that we have just defined, we can now construct a function to print an entire DNS message to the screen. 
 * */
void print_dns_message(const char *message, int msg_length)
{
	/*"print_dns_message()" takes a pointer to the start of the message, and an int data type indicates the messages's length*/
	if (msg_length < 12)	/*making sure the message length is protocol standard*/
	{
		fprintf(stderr, "Message is too short to be valid.\n"); 
		exit(1); 
	}

	/*We then copy the "message" pointer into a new variable, "msg". 
	 * We define "msg" as an "unsigned char" pointer, which makes certain calculations easier to work with.*/
	const unsigned char *msg = (const unsigned char *)message; 

	/*If you want to print out the entire raw DNS message, you can do that with the method below*/
	int i; 
	for (i = 0; i < msg_length; ++i)
	{
		unsigned char r = msg[i]; 
		printf("%02d: %02X %03d '%c'\n", i, r, r, r); 
	}
	printf("\n"); /*run this code*/

	/*The message ID can be printed very easily. 
	 * The message ID is simply the first two bytes of the message.*/
	printf("ID = %0X %0x\n", msg[0], msg[1]); 

	/*Next, we get the "QR" bit from the message header. 
	 * This bit is the most significant bit of "msg[2]". 
	 * We use the bitmask "0x80" to see whether it is set. 
	 * If it is, we know that the message is a response; otherwise it's a query. 
	 * the follow method below reads "QR" and prints a corresponding message*/
	const int qr = (msg[2] & 0x80) >> 7; 
	printf("QR = %d %s\n", qr, qr ? "response" : "query"); 

	/*Below is the method for "OPCODE", "AA", "TC", and "RD" fields and its much the same way as "QR".*/
	const int opcode = (msg[2] & 0x78) >> 3; 
	printf("OPCODE = %d", opcode); 
	switch(opcode)
	{
		case 0: printf("standard\n"); 
			break; 
		case 1: printf("reverse\n"); 
			break; 
		case 2: printf("status\n"); 
			break; 
		default: printf("?\n"); 
			 break; 
	}

	const int aa = (msg[2] & 0x04) >> 2; 
	printf("AA = %d %s\n", aa, aa ? "authoritative" : ""); 

	const int tc = (msg[2] & 0x02) >> 1; 
	printf("TC = %d %s\n", tc, tc ? "message truncated" : ""); 

	const int rd = (msg[2] & 0x01); 
	printf("RD = %d %s\n", rd, rd ? "recursion desired" : ""); 

	/*Next, we can read in "RCODE" for response-type messages. 
	 * Since "RCODE" can have several different values, we use a "switch" statement to print them. */
	if (qr)
	{
		const int recode = msg[3] & 0x07; 
		printf("RCODE = %d", rcode); 
		switch(rcode)
		{
			case 0: printf("success\n"); 
				break; 
			case 1: printf("format error\n"); 
				break; 
			case 2: printf("server failure\n"); 
				break; 
			case 3: printf("name error\n"); 
				break; 
			case 4: printf("not implemented\n"); 
				break; 
			case 5: printf("refused\n"); 
				break; 
			default: printf("?\n"); 
				 break;
		}
		if (rcode != 0)
			return; 
	}

	/*The next four fields in the header are the question count, the answer count, the name server count, and the additional count. */
	const int qdcount = (msg[4] << 8) + msg[5]; 
	const int ancount = (msg[6] << 8) + msg[7]; 
	const int nscount = (msg[8] << 8) + msg[9]; 
	const int arcount = (msg[10] << 8) + msg[11]; 

	printf("QDCOUNT = %d\n", qdcount); 
	printf("ANCOUNT = %d\n", ancount); 
	printf("NSCOUNT = %d\n", nscount); 
	printf("ARCOUNT = %d\n", arcount); 

	/*Before reading the rest of the message, we define two new variables as follows:*/
	const unsigned char *p = msg + 12; 
	const unsigned char *end = msg + msg_length; 

	/*We read and print each question in the DNS message with the following code: */
	if (qdcount)
	{
		int i; 
		for (i = 0; i < qdcount; ++i)
		{
			if (p >= end)
			{
				fprintf(stderr, "End of message.\n"); 
				exit(1); 
			}
			printf("Query %2d\n", i + 1); 
			printf(" name: "); 

			p = print_name(msg, p, end); 
			printf("\n"); 

			if (p + 4 > end)
			{
				fprintf(stderr, "End of message.\n"); 
				exit(1); 
			}

			const int type = (p[0] << 8) + p[1]; 
			printf(" type: %d\n", type); 
			p += 2; 

			const int qclass = (p[0] << 8) + p[1]; 
			printf(" class: %d\n", qclass); 
			p += 2; 
		}
	}

	/*Next, we print out the answer, authority, and additional sections which is more difficult. */
	if (ancount || nscount || arcount)
	{
		int i; 
		for (i = 0; i < ancount + nscount + arcount; ++i)
		{
			if (p >= end)
			{
				fprintf(stderr, "End of message.\n"); 
				exit(1); 
			}

			printf("Answer %2d\n", i + 1); 
			printf(" name: "); 

			p = print_name(msg, p, end); 
			printf("\n"); 

			if (p + 10 > end)
			{
				fprintf(stderr, "End of message.\n"); 
				exit(1); 
			}

			const int type = (p[0] << 8) + p[1]; 
			printf(" type: %d\n", type); 
			p += 2; 

			const int qclass = (p[0] << 8) + p[1]; 
			printf(" class: %d\n", qclass); 
			p += 2; 

			/*We then expect to find a 16 bit field and a 16 bit data length field. 
			 * The TTL field tells us how many seconds we are allowed to cache an answer for. 
			 * The data length field tells us how any bytes of additional data are included for the answer.*/
			const unsigned int ttl = (p[0] << 24) + (p[1] << 16) + (p[2] << 8) + p[3]; 
			printf(" ttl: %u\n", ttl); 
			p += 4; 

			const int rdlen = (p[0] << 8) + p[1]; 
			printf(" rdlen: %d\n", rdlen); 
			p += 2; 

			/*Before we can read in the data of the "rdlen" length, check that we won't read past the end of the message.*/
			if (p + rdlen > end)
			{
				fprintf(stderr, "End of message.\n"); 
				exit(1); 
			}

			/*We can then tyr to interpret the answer data. 
			 * Each record type stores different data. 
			 * We need to write code to display each type. 
			 * We limit this to the "A", "MX", "TXT", and "CNAME", records*/
			if (rdlen == 4 && type == 1)
			{	/*a Record*/
				printf("Address "); 
				printf("%d.%d.%d.%d\n", p[0], p[1], p[2], p[3]);
			} else if (type == 15 && rdlen > 3)
			{/*MX Record*/
				const int preference = (p[0] << 8) + p[1]; 
				printf(" pref: %d\n", preference); 
				printf("MX: "); 
				print_name(msg, p+2, end); 
				printf("\n"); 
			} else if (rdlen == 16 && type == 28)
			{	/*AAA record*/
				printf("Address "); 
				int j; 
				for (j = 0; j < rdlen; j += 2)
				{
					printf("%02x%02x", p[j], p[j+1]); 
					if (j + 2 < rdlen)
						printf(":"); 
				}
				printf("\n"); 
			} else if (type == 5)
			{
				printf("CNAME: "); 
				print_name(msg, p, end); 
				printf("\n"); 
			}
			p += rdlen; 
		}
	}
	if (p != end)
	{
		printf("There is some unread data left over.\n"); 
	}
	printf("\n"); 
}

/*We can now define our "main()" function to create DNS query, send it to a DNS server, and await a response.*/

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Usage:\n\tdns_query hostname type\n"); 
		printf("Example:\n\tdns_query example.com aaaa\n"); 
		exit(0); 
	}

	if (strlen(argv[1]) > 255)	/*Host names longer then 255 char are not allowed in the protocol*/
	{
		fprintf(stderr, "Hostname too long."); 
		exit(1); 
	}

	/*We then try to interpret the record type requested by the user. 
	 * We support the following options 'a' "aaa" "txt" "mx" and "any"*/
	unsigned char type; 
	if (strcmp(argv[2], "a") == 0)
	{
		type = 1; 
	} else if (strcmp(argv[2], "mx") == 0)
	{
		type = 15; 
	} else if (strcmp(argv[2], "txt") == 0)
	{
		type = 16; 
	}else if (strcmp(argv[2], "aaaa") == 0)
	{
		type = 28; 
	}else if (strcmp(argv[2], "any")  == 0)
	{
		type = 255; 
	}else 
	{
		fprintf(stderr, "Unknown type '%s'. Use a, aaaa, txt, mx, or any.", argv[2]); 
		exit(1); 
	}

	/*windows socket handling */
#if defined(_WIN32)
	WSADATA d; 
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		fprintf(stderr, "Failed to initialize.\n"); 
		return 1; 
	}
#endif 

	/*We are connecting to UDP port 53. 
	 * We use "getaddrinfo()" to set up the required structures for our socket*/
	printf("Configuring remote address...\n"); 
	struct addrinfo hints; 
	memset(&hints, 0, sizeof(hints)); 
	hints.ai_socktype = SOCK_DGRAM; 
	struct addrinfo *peer_address; 
	if (getaddrinfo("8.8.8.8", "53", &hints, &peer_address))
	{
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO()); 
		return 1; 
	}

	/*We then create our socket using the data returned from "getaddrinfo()" */
	printf("Creating socket...\n"); 
	SOCKET socket_peer; 
	socket_peer = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol); 
	if (!ISVALIDSOCKET(socket_peer))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO()); 
		return 1; 
	}

	/*Our program then constructs the data for the DNS query message. 
	 * The first 12 bytes compose the header and are known at compile time*/
	char query[1024] = {
		0xAB, 0xCD, 
		0x01, 0x00, 
		0x00, 0x00, 
		0x00, 0x00, 
		0x00, 0x00, 
		0x00, 0x00
	}; 

	/*Next we need to encode the users desired hostname into the query.*/
	char *p = query + 12; 
	char *h = argv[1]; 

	while(*h)
	{
		char *len = p; 
		p++; 
		if (h != argv[1])
			++h; 
		while(*h && *h != '.')
			*p++ = *h++; 
		*len = p - len -1; 
	}
	*p++ = 0; 

	/*Finally, outside the loop, we add a terminating 0 byte to finish the name section of the question.*/
	/*We then add the question type and question call to the query*/
	*p++ = 0x00; 
	*p++ = type; /*QTYPE*/
	*p++ = 0x00; 
	*p++ = 0x01; /*QCLASS*/

	/*We can then calculate the query size by comparing 'p' to the query begining.*/
	const int query_size = p - query; 

	/*Now, with the query message formed, and its length known, we can use "sendto()" to transmit the DNS query to the DNS server.*/
	int bytes_sent = sendto(socket_peer, query, query_size, 0, peer_address->ai_addr, peer_address->ai_addrlen); 
	printf("sent %d bytes.\n", bytes_sent); 

	/*For debugging purposes, we also display the query we sent*/
	print_dns_message(query, query_size); 

	/*The code to receive and display the DNS response is below*/
	char read[1024];
	int bytes_received = recvfrom(socket_peer, read, 1024, 0, 0, 0); 

	printf("Received %d bytes.\n", bytes_received); 
	print_dns_message(read, bytes_received); 
	printf("\n"); 

	/*We can finish our program by freeing the addresses from "getaddrinfo()" and cleaning up Winsock. */
	freeaddrinfo(peer_address); 
	CLOSESOCKET(socket_peer); 
#if defined(_WIN32)
	WSACleanup(); 
#endif 

	return 0; 
}
