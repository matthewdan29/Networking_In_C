#include "chap11.h"

int main(int argc, char *argv[]) 
{
	const char *hostname = 0; 
	int port = 22; 
	if (argc < 2)
	{
		fprintf(stderr, "Usage: ssh_connect hostname port\n"); 
		return 1; 
	}

	hostname = argv[1]; 
	if (argc > 2)
		port = atol(argv[2]); 
	
	/*Some internet criminals randomly scan IP address for SSH connections.
	 * For this reason this program unsure that you are able to ssh into differnt ports*/

	/*Once our program has obtained the hostname and connection port number, we continue by creating an SSH session object.  
	 * This is done with a call to "ssh_new()"*/
	ssh_session ssh = ssh_new(); 
	if (!ssh)
	{
		fprintf(stderr, "ssh_new() failed.\n"); 
		return 1; 
	}

	/*Once the SSH session is created, we need to specify some options before completing the connection. 
	 * The "ssh_options_set()" function is used to set options. 
	 * below shows how to set the remote hostname and port*/
	ssh_options_set(ssh, SSH_OPTIONS_HOST, hostname); 
	ssh_options_set(ssh, SSH_OPTIONS_PORT, &port); 

	/*"libssh" includes useful debugging tools. 
	 * By setting the "SSH_OPTIONS_LOG_VERBOSITY" option, we tell "libssh" to print almost everything it does. 
	 * Below shows "libssh" to log a lot of information about which actions it's taken */
	int verbosity = SSH_LOG_PROTOCOL; 
	ssh_options_set(ssh, SSH_OPTIONS_LOG_VERBOSITY, &verbosity); 

	/*Next, you can use "ssh_connect()" to initiate the SSH connection. */
	int ret = ssh_connect(ssh); 
	if (ret != SSH_OK)
	{
		fprintf(stderr, "ssh_connect() failed.\n", ssh_get_error(ssh)); 
		return -1; 
	}

	/*Next, the program prints out that the connection was successful*/
	printf("Connected to %s on port %d.\n", hostname, port); 

	/*The SSH protocol allows servers to send a message to clients upon connection. 
	 * This message is called the banner. 
	 * It is typically used to identify the server or provide short access rules. */
	printf("Banner:\n\%s\n", ssh_get_serverbanner(ssh)); 

	/*After that print out of the banner the program disconnects from the ssh connection.
	 * Although you could do more...*/
	ssh_disconnect(ssh); 
	ssh_free(ssh); 
	
	return 0; 
}

