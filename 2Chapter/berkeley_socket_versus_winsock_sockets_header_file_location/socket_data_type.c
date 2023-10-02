/*In Windows, a socket handle can be anything. In UNIX, the "socket()" function returns an "int", whereas in Windows it returns a "SOCKET", "SOCKET" is a "typedef" for an "unsigned int" in the Winsock headers. As a workaround, you can find it useful to either "typedef in SOCKET" or "#define SOCKET int" on non-windows platforms. That way, you can store a socket descriptor as a "SOCKET" type on all platform: like below*/

#if !defined(_WIN32)
#define SOCKET int 
#endif

/*Below if a useful "#define" for a macro to indicate if a socket descriptor is valid or not. Because, Windows "socket()" returns "INVALID_SOCKET" if it fails
 * Unix "socket()" returns a negative number on failure. 
 * This is particularly problematic as the Windows "SOCKET" type is unsigned.*/

#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#else 
#define ISVALIDSOCKET(s) ((s) >= 0)
#endif

/*Below is to abstract out the difference with a macro of Unix systems and Windows operations systems methods of closing standard stockets.*/

#if defined(_WIN32)
#define CLOSESOCKET(s) closesocket(s)
#else 
#define CLOSESOCKET(s) close(s)
#endif

/*Below is abstracting the different method inwhich Windows and Unix operating system handles errors.*/

#if defined(_WIN32)
#define GETSOCKETERRNO() (WSAGetLastError())
#else 
#define GETSOCKETERRNO() (errno)
#endif 


