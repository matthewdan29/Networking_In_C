/*"parse_response()" takes in a null-terminated response string and returns the parsed response code. 
 * If no code is found or the response isn't complete, then 0 is returned instead.*/

int parse_response(const char *response)
{
	const char *k = response; 
	if (!k[0] || !k[1] || !k[2])
		return 0; 
	for (; k[3]; ++k)
	{
		if (k == response || k[-1] == '\n')
		{
			if (isdigit(k[0]) && isdigit(k[1]) && isdigit(k[2]))
			{
				if (k[3] != '-')
				{
					if (strstr(k, "\r\n"))
					{
						return strtol(k, 0, 10); 
					}
				}
			}
		}
	}
	return 0; 
}


