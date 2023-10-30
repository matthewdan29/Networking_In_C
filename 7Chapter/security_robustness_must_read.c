/*if your code doesn't carefully check for errors and unexpected conditions, then it will be vunerable to exploits.*/

/*Consider the follwoing code which receives data into a buffer until a space caracter is found*/

char buffer[1028] = {0}; 
char *p = buffer; 

while (!strstr(p, " "))
	p += recv(client, p, 1028, 0); 

/*it reserves 1028 bytes of buffer space and then uses "recv()" to write received data into that space. 
 * The "p" pointer is updated on each read to indicate where the next data should be written. 
 * The code then loops until the "strstr()" function detects a space character.*/

/*One problem with the preceding code is that "recv()" could write past the end of the allocated space for "buffer". 
 * This is because 1028 is passed to "recv()", even if some data has already been written.
 * If a network client can cause your code to write past theend of a buffer, then that client may be able to completely compromise your server. 
 * This is because both data and executable code are stored in your server's memory. 
 * A malicious client may be able to write executable code past the "buffer" array and cause your program to execute it. 
 * Even if the malicious code isn't executed, the client could still overwrite other important data in your server's memory.*/

/*The preceding code can be fixed by passing to "recv()" only the amount of buffer space remaining:*/

char buffer[1028] = {0}; 
char *p = buffer; 

while (!strstr(p, " "))
	p += recv(client, p, 1028 - (p - buffer), 0); 

/*Now, "recv()" is not able to write more than 1028. 
 * You may think that the memory errors are resolved, but you would still be wrong. 
 * Consider that "buffer" is completely full now, "strstr()" cannot find a space character or a null terminating character
 * In that case "strstr()" continues to read past the end of "buffer" into unallocated memory. */

/*To fix that issue allow "recv()" to write 1027 bytes total. 
 * This reserves one byte to remain as the null terminating character*/

char buffer[1028] = {0}; 
char *p = buffer; 

while (!strstr(p, " "))
	p += recv(client, p, 1027 - (p - buffer), 0); 
