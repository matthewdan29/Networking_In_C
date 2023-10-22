/*This function parse a given URL.*/

void parse_url(char *url, char **hostname, char **port, char** path)
{
	printf("URL: %s\n", url); 

	/*The function next attempts to find "://" in the URL. 
	 * If found, it reads in the first part of the URL as a protocol. 
	 * This program only supports "HTTP"
	 * If the given protocol is not HTTP, then an eror is returned.*/

	char *p; 
	p = strstr(url, "://");		/*"strstr()" looking for the '://' in the string*/

	char *protocol = 0; 
	if (p)
	{
		protocol = url; 
		*p = 0; 
		p += 3; 
	} else 
	{
		p = url; 
	}

	if (protocol)
	{
		if(strcmp(protocol, "http"))
		{
			fprintf(stderr, "Unknown protocol '%s'. Only 'http' is supported.\n", protocol); 
			exit(1); 
		}
	}
	/*At this point in the code, 'p' points to the beginning of the hostname. 
	 * The code can save the hostname into the return cariable, "hostname"
	 * The code must then scan for the end of the hostname by looking for the first colon, slash, or hash.*/
	*hostname = p; 
	while (*p && *p != ':' && *p != '/' && *p != '#')
		++p; 

	/*Once 'p' has advanced to the end of the hostname, we must check whether a port number was found. 
	 * If a port number is found, our code returns it in the "port" variable; otherwise, a default port number of "80" is returned.*/
	*port = "80"; 
	if (*p == ':')
	{
		*p++ = 0; 
		*port = p; 
	}
	while (*p && *p != '/' && *p != '#')
		++p; 

	/*After the port number, 'p' points to the document path. 
	 * The function returns this part of the URL in the "path" variable
	 * The cod to set the "path" variable is as follows*/
	*path = p; 
	if (*p == '/')
	{
		*path = p + 1; 
	}
	*p = 0; 

	/*The code then attempts to find a hash, if it exits
	 * if the hash doesn't exitst, it is overwritten with a terminating null character. 
	 * The code that advances to the hash is as follows*/
	while (*p && *p != '#')
		++p; 
	if (*p == '#')
		*p = 0; 

	/*The code has now parsed out the hostname, port number, and document path
	 *It then prints out these values for debugging purposes and returns.*/
	printf("hostname: %s\n", *hostname); 
	printf("port: %s\n", *port); 
	printf("path: %s\n", *path); 
}
