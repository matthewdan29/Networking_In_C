#include <stdio.h>
#include <time.h>

/*below is: The local, console version of our time-telling program.*/

int main()
{
	time_t timer;	 
	time(&timer);			/*the function "time()" get the time*/

	printf("Local time is: %s", ctime(&timer)); 	/*"ctime()" converts the time into a string so it printable*/

	return 0; 
}
