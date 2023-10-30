/*Hey i'm trying my best to post information on how to learn networking I know the file name is very long but you will understand the content within the file*/

/*Consider a client that sends only the beginning of an HTTP request, followed by a delay, and then the remainder of the HTTP request. 
 * We cannot respond to that client until the entire HTTP request is reveived. 
 * At the same time, we do not wish to delay servicing other connected clients while waiting. 
 * For that reason, we need to buffer up received data for each client separately. 
 * Only once we've received an entire HTTP request from a client can we respond to that client.*/
			/*Important commet explaing the struct below*/
/*This "struct" allows us to store information about each connected client. 
 * A client's address is stored in the "address" field, the address length in "address_lenth", and the socket in the "socket" field. 
 * All of the data received from the client so far is sotred in the "request" array: "received" indicates the number of bytes stored in that array. 
 * The "next" field is a pointer that allows us to store "client_info" structures in a linked list.*/
#define MAX_REQUEST_SIZE 2047

struct client_info 
{
	socklen_t address_length; 
	struct sockaddr_storage address; 
	SOCKET socket; 
	char request[MAX_REQUEST_SIZE + 1]; 
	int received; 
	struct client_info *next; 	/*Notice that there is no "pre" meaning its one linked list
	I personally would of doubled it although it can cause problems being able to navagate quicker and having micro services constently checking the the "full list" and "3-n partion of the listed for security and maintane other plugin options"*/
};

/*To simplify our code, we store the root of our linked list in a global variable, "clients". */

static struct client_info *clients = 0; 
