/*After the ssh session is open and the user is authenticated, a channel can be opend. 
 * A new channel is opened by calling the "ssh_channel_new()" function. */
ssh_channel channel = ssh_channel_new(ssh); 
if (!channel)
{
	fprintf(stderr, "ssh_channel_new() failed.\n"); 
	return 0; 
}

/*The SSH protocol implements many types of channels. 
 * The session channel type is used for exectuing remote commands and tranferring files. 
 * We can request a session channel by using the "ssh_channel_open_session()" function.*/
if (ssh_channel_open_session(channel) != SSH_OK)
{
	fprintf(stderr, "ssh_channel_open_session() failed.\n"); 
	return 0; 
}

/*Once the session channel is open, you can issue a command to run with the "ssh_channel_request_exec()" function. 
 *Below "fgets()" to prompt the user for a command and "ssh_channel_request_exec()" to send the command to the remote host: */
printf("Remote command to execute: "); 
char command[128]; 
fgets(command, sizeof(command), stdin); 
command[strlen(command)-1] = 0; 

if (ssh_channel_request_exec(channel, command) != SSH_OK)
{
	fprintf(stderr, "ssh_channel_open_session() failed.\n"); 
	return 1; 
}

/*Once the command has been sent, the program uses "ssh_channel_read()" to receive the command output. */
char output[1024]; 
int bytes_received; 
while ((bytes_received = ssh_channel_read(channel, output, sizeof(output), 0)))
{
	if (bytes_received < 0)
	{
		fprintf(stderr, "ssh_channel_read() failed.\n"); 
		return 1; 
	}

	printf("%.*s", bytes_received, output); 
}

/*After the entire output from the command has been received, the client should send an end of file (EOF) over the channel, close the channel, and free the channel resources.*/

ssh_channel_send_eof(channel); 
ssh_channel_close(channel); 
ssh_channel_free(channel); 

/*be sure to call "ssh_free" and "ssh_disconnect()"*/
