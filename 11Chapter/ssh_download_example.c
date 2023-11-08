/*After the SSH session is started and the user is authenticated, this prompts the user for the remote filename using the method below*/
printf("Remote file to download: "); 
char filename[128]; 
fgets(filename, sizeof(filename), stdin); 
filename[strlen(filename)-1] = 0; 

/*A new SCP session is initialized by calling the "libssh" library function "ssh_scp_new()"*/
ssh_scp scp = ssh_scp_new(ssh, SSH_SCP_READ, filename); 
if (!scp)
{
	fprintf(stderr, "ssch_scp_new() failed.\n%s\n", ssh_get_error(ssh)); 
	return 1; 
}

/*After the SCP session is created successfully, "ssh_scp_init()" must be called to initialize the SCP channel. */
if (ssh_scp_init(scp) != SSH_OK)
{
	fprintf(stderr, "ssh_scp_init() failed.\n%s\n", ssh_get_error(ssh)); 
	return 1; 
}

/*"ssh_scp_pull_request()" must be called to begin the file download. 
 * This function returns "SSH_SCP_REQUEST_NEWFILE" to indicate that the remote host is going to begin sending a new file. */

if (ssh_scp_pull_request(scp) != SSH_SCP_REQUEST_NEWFILE)
{
	fprintf(stderr, "ssh_scp_pull_request() failed.\n%s\n", ssh_get_error(ssh)); 
	return 1; 
}

/*"libssh" provides some methods we can use to retrieve the remote filename, filse size, and permissions.*/
int fsize = ssh_scp_request_get_size(scp); 
char *fname = strdup(ssh_scp_request_get_filename(scp)); 
int fpermission = ssh+scp_request_get_permission(scp); 

printf("Downloading file %s (%d bytes, permission 0%o\n", fname, fsize, fpermission); 
free(fname); 

/*Once the file size is known, we can allocate space to store it in memory. 
 * This is done using "mallac()" */
char *buffer = malloc(fsize); 
if (!buffer)
{
	fprintf(stderr, "malloc() failed\n"); 
	return 1; 
}

/*Our program then accepts the new file request with "ssh_scp_accept_request()" and downloads the file with "ssh_scp_read()"*/
ssh_scp_accept_request(scp); 
if (ssh_scp_read(scp, buffer, fsize) == SSH_ERROR)
{
	fprintf(stderr, "ssh_scp_read() failed.\n%s\n", ssh_get_error(ssh)); 
	return 1; 
}

/*below method prints out the files contents and frees the allocated memory*/
printf("Received %s:\n", filename); 
printf("%.*s\n", fsize, buffer); 
free(buffer); 

/*Another call to "ssh_scp_pull_request()" should return "SSH_SCP_REQEST_EOF". 
 * below checks for that */
if (ssh_scp_pull_request(scp) != SSH_SCP_REQUEST_EOF)
{
	fprintf(stderr, "ssh_scp_pull_request() unexpected.\n%s\n", ssh_get_error(ssh)); 
	return 1; 
}

/*After the file is received, "ssh_scp_close()" and "ssh_scp_free()" should be used to free resources */

ssh_scp_close(scp); 
ssh_scp_free(scp); 

