/*Simply using the "fork()" functions the multi process is easy to understands.
 * The "fork()" function splits the executing program into two separate processes. A multi-process TCP server may accept connnections like below.*/

while(1)
{
	socket_client = accept(socket_listen, &new_client, &new_client_length); 
	int pid = fork(); 
	if (pid == 0)
	{/*child process*/
		close(socket_listen); 
		recv(socket_client, ...); 
		send(socket_client, ...); 
		close(socket_client); 
		exit(0); 
	}
	/*parent process*/
	close(socket_client); 
}/*The program blocks on "accept()". 
When a new connection is established, the program calls "fork()" to split into two processes. 
The child process, where "pid == 0", only services this one connection. 
The child process can use "recv()" freely without worrying about blocking. 
The parent process simply calls "close()" on the new connection and returns to listening for more connections with "accept()"*/
