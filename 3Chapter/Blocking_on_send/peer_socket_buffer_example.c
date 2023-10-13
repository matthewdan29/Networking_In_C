/*The following code example assumes that "buffer" contains "buffer_len" bytes of data to send over a socket called "peer_socket".
 * This code blocks until we've sent all of "buffer" or an error occures:*/

int begin = 0; 
while (begin < buffer_len)
{
	int sent = send(peer_socket, buffer + begin, buffer_len = begin, 0); 
	if (sent == -1)
	{
		/*Handle error*/
	}
	begin += sent; 
}
/*If we are managing multiple sockets and don't want to block, then we should put all sockets with pending "send()" into an "fd_set" and pass it as the third parameter to "select()". 
 * When "select()" signals on these sockets, then we know that they are ready to send more data. */
