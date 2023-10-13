/*The "select()" function has several useful features.
 * Given a set of sockets, it can be used to block until any of the sockets in that set is ready to be read from. 
 * It can also be configured to return if a socket is ready to be written to or if a socket has an error. 
 * Additionally, you can configure "select()" to return after a specified time if none of these events take place. */

/*Below is the "select()" function prototype.*/
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout); 

/*Before calling "select()", you must first add you sockets into an "fd_set". 
 * If you have 3 sockets, "socket_listen", "socket_a", and "socket_b", we add them to an "fd_set", like this below: */

fd_set our_sockets; 
FD_ZERO(&our_sockets); 
FD_SET(socket_listen, &our_sockets); 
FD_SET(socket_a, &our_sockets); 
FD_SET(socket_b, &our_sockets); 

/*"select()" also requires that you pass a number that's larger than the largest socket descriptor we are going to monitor. 
 * You store the largest socket descriptor in a variable, like below*/

SOCKET max_socket; 
max_socket = socket_listen; 
if (socket_a > max_socket)
	max_socket = socket_a; 
if (socket_b > max_socket)
	max_socet = socket_b;

/*When we call "select()", it modifies our "fd_set" of sockets to indicate which sockets are ready. 
 * We want to copy our socket set before calling it. 
 * We can copy an "fd_set" with a simple assignment like below and then call "select()" like below*/

fd_set copy; 
copy = our_sockets; 

select(max_socket+1, &copy, 0, 0, 0); 

/*This call blocks until at leastone of the sockets is ready to be read from.
 * When "select()" returns, "copy" is modified so that it only contains the sockets that are ready to be read from. 
 * We can check which sockets are still in "copy" using "FD_ISSET()", like below*/

if (FD_ISSET(socket_listen, &copy))
{
	/*socket_listen has a new connection*/
	accept(socket_listen...)
}

if (FD_ISSET(socket_a, &copy))
{
	/*socket_a is ready to be read from*/
	recv(socket_a...)
}

if (FD_ISSET(socket_b, &copy))
{
	/*socket_b is ready to be read from*/
	recv(socket_b...)
}

/*We passed our "fd_set" as the second argurment to "select()". 
 * If we wanted to monitor an "fd_set" for writability instead of readablility, we would pass our "fd_set" as the third argument to "select()". 
 * We can monitor a set of sockets for exceptions by passing it as the fourth argument to "select()"*/
