/*The last argurment taken by "select()" allows us to specify a timeout. 
 * it expects a pointer to "struct timeval". 
 * The "timeval" structure is declared as follows */

struct timeval
{
	long tv_sec; 
	long tv_usec; 
}

/*"tv_sec" holds the number of seconds, and "tv_usec" holds the number of microseconds. 
 * if we want "select()" to wait a maximim of 1.5 seconds, we can call it like below*/

struct timeval timeout; 
timeout.tv_sec = 1; 
timout.tv_usec = 500000; 
select(max_socket+1, &copy, 0, 0, &timeout); 

/*"select()" can also be used to monitor for writeable sockets, and sockets with exceptions. 
 * We can check for all three conditions with one call: example below*/

select(max_socket+1, &ready_to_read, &ready_to_write, &excepted, &timeout); 

/*ON success, "select()" itself returns the number of socket decriptors contained in the three diecriptors sets it monitored. 
 * The return value is zero if it timed out before any sockets were readable/writeable/excepted. 
 * "select()" returns "-1" to indicate and error. */

		/*Iterating throught an "fd_set"*/
/*We can iterate throuth and "fd_set" using a simple "for" loop. 
 * Essentially, we start at 1, since all socket descriptors are positive numbers, and we coutinue through to the largest known socket descriptor in the set.
 * For each possible socket descriptor, we simply use "FD_ISSET()" to check if it is in the set, 
 * if we wanted to call "CLOSESOKET()" for every socket in the "fd_set master", we could do it like below */

SOCKET i; 
for(i = 1; i <= max_socket; ++i)
{
	if(FD_ISSET(i, &master))
	{
		CLOSESOCKET(i); 
	}
}/*THis should remind you of a brute-force search method. Its very fast and unless you data mine and see a for loop is holding your operation back, GO for IT.*/
