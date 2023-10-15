/*UDP is a connectionless protocol. 
 * Therefore, no connection is established before sending data. 
 * A UDP connection is never established. 
 * With UDP, data is simply sent and received. 
 * We can call "connect()" and then "send()", but the socket API provides an easier way for UDP sockets in the form of the "sendto()" function. */

sendto(udp_socket, data, data_length, 0, peer_address, peer_address_length); 

/*"connect()" on an UDP socket works a bit differently. 
 * All "connect()" does with a UDP socket is associate a remote address. 
 * While "connect()" on  a TCP socket involoves a handshake for sending packets over the network, "connect()" on a UDP socket only stores an address locally.*/
