/*Our server code uses a series of "if" statements to determine the proper media type based only on the requested files's extension. 
 * This is not the best but is a common one. 
 * Dont go labeling HTML files with inbedded scripts lol*/
/*This solve's a encoding problem for the content type requested by the HTML*/

/*The "get_content_type()" function works by matching the filename extension to a list of known extensions. 
 * This is done by using the "strrch()" function to find the last dot "(.)" in the filename. 
 * If a dot is found, then "strcmp()" is used to check for a match on each extension. 
 * When a match is found, the proper media type is reuturned. 
 * Otherwise, the default of "application/octet-stream" is returned instead. */

const char *get_constent_type(const char* path)
{
	const char *last_dot = strrchr(path, '.'); 
	if (last_dot)
	{
		if (strcmp(last_dot, ".css") == 0)
			return "text/css"; 
		if (strcmp(last_dot, ".csv") == 0)
			return "text/csv"; 
		if (strcmp(last_dot, ".gif") == 0)
			return "image/gif"; 
		if (strcmp(last_dot, ".html") == 0)
			return "text/html"; 
		if (strcmp(last_dot, ".ico") == 0)
			return "image/x-con"; 
		if (strcmp(last_dot, ".jpeg") == 0)
			return "image/jpeg"; 
		if (strcmp(last_dot, ".jpg") == 0)
			return "image/jpeg"; 
		if (strcmp(last_dot, ".js") == 0)
			return "application/javascript"; 
		if (strcmp(last_dot, ".json") == 0)
			return "application/json"; 
		if (strcmp(last_dot, ".png") == 0)
			return "image/png"; 
		if (strcmp(last_dot, "pdf") == 0)
			return "application/pdf"; 
		if (strcmp(last_dot, "svg") == 0)
			return "image/svg+xml"; 
		if (strcmp(last_dot, ".txt") == 0)
			return "text/plain"; 
	}
	return "application/octect-stream"; 
}
