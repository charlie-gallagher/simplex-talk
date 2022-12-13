#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
	struct addrinfo hint, *result, *rp;

	/* Partially fill the `hint` object */
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	
	/* Get the results
	 *
	 * int getaddrinfo(const char *hostname,
	 * 		   const char *servname,
	 * 	           const struct addrinfo *hints,
	 *                 struct addrinfo **res);
	 */
	if (getaddrinfo("localhost", "8000", &hint, &result) != 0) {
		fprintf(stderr, "getaddrinfo: failed\n");
	}
	
	/* Loop over the results
	 *
	 * The only way to test the results is to create a socket,
	 * connect to it, and see if there is an error. Then disconnect.
	 */
	int s = -1;
	const char *cause = NULL;
	for (rp = result; rp; rp = rp->ai_next) {
		s = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (s == -1) {
			cause = "socket";
			continue;
		}
		if (connect(s, rp->ai_addr, rp->ai_addrlen) != 0) {
			cause = "connect";
			close(s);
			s = -1;
			continue;
		}
		if (close(s) != 0) {
			fprintf(stderr, "Failed to close socket for some reason\n");
		}
		/* If we connected and close without problem, it was a success */
		printf("Successfully connected to localhost:8000\n");
		break;
	}
	if (s == -1) {
		fprintf(stderr, "Error with cause: %s\n", cause);
		exit(-1);
	}
	freeaddrinfo(result);
}
