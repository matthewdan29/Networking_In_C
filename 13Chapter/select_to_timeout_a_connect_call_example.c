/*Step 1, setting the socket to non-blocking mode, can be accomplished with like below*/
#if defined(_WIN32)
	unsigned long nonblock = 1; 
	ioctlsocket(socket_peer, FIONBIO, &nonblock); 
#else 
	int flags; 
	flags = fcntl(socket_peer, F_GETFL, 0); 
	fcntl(socket_peer, F_SETFL, flags | O_NONBLOCK); 
#endif 

/*Step 2 and Step 3, the call to "connect()" is done normally. 
 * The only difference is that you should expect an error code of "EINPROGRESS" on Unix-based systems and "WSAEWOULDBLOCK" on windows. */

/*Step 4, the setup for "select()" is straightforward. 
 * The "select()" function is used in the same way as described in other DIR. 
 * The following code shows  on e way to use "select()" */
fd_set set; 
FD_ZERO(&set); 
FD_SET(socket_peer, &set); 

struct timeval timeout; 
timeout.tv_sec = 5; 
timeout.tv_usec = 0; 
select(socket_peer+1, 0, &set, 0, &timeout); 


/*look at the above code and notice the timeout was set to five meaning only 5 sec to stay alive on the network*/

/*Step 5, setting the socket back to non-blocking mode is accomplished with the below code*/
#if defined(_WIN32)
	nonblock = 0; 
	ioctlsocket(socket_peer, FIONBIO, &nonblock); 
#else 
	fcntl(socket_peer, F_SETFL, flags); 
#endif


/*Step 6, we are looking to see whether the call to "select()" timed out, returned early from an error, or returned early because our socket has successfully connected. */
