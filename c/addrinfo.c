/* Get address info
 *
 * This program compiles and forms a standalone application.
 * It was written for `sockets`. 
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char *argv[])
{
	/* First, I need a hints struct to reduce what is acceptable.
	 * I want to set ai_family to AF_INET, and use anything else
	 * probably. 
	 */
	struct addrinfo hint;
	/* rp is result pointer, used to loop through results */
	struct addrinfo *result, *rp;

	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;


	/* Let's make sure we have the right number of arguments
	 *
	 * I'm expecting a host and a port
	 */
	if (argc < 3) {
		fprintf(stderr, "Usage: %s host port\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Populates 'result' with linked list of results */
	if (getaddrinfo(argv[1], argv[2], &hint, &result) != 0) {
		fprintf(stderr, "Error getting address information\n");
		exit(EXIT_FAILURE);
	}

	printf("Socket types: \n");
	printf("  %d Stream\n  %d Datagram\n  %d Raw\n", SOCK_STREAM, SOCK_DGRAM, SOCK_RAW);
	printf("Family: \n");
	printf("  %d Internet\n\n", PF_INET);

	/* Loop through results and report whether the resolution was successful */
	char host[NI_MAXHOST], port[NI_MAXSERV]; /* NI_MAXHOST and NI_MAXSERV defined in netdb.h */
	struct sockaddr *current_address = result->ai_addr;
	socklen_t addresslen = sizeof(*current_address);
	int s;
	int current_socktype, current_family;

	int i = 0;
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		current_address = rp->ai_addr;
		current_socktype = rp->ai_socktype;
		current_family = rp->ai_family;
		s = getnameinfo(
			current_address,
			addresslen,
			host,
			NI_MAXHOST,
			port,
			NI_MAXSERV,
			NI_NUMERICSERV | NI_NUMERICHOST
		);


		if (s == 0) 
			printf("%2d: %s:%s (fam: %d, type: %d)\n", i, host, port, current_family, current_socktype);
		else {
			fprintf(stderr, "Address resolution error: %s\n", gai_strerror(s));
			continue;
		}
		i++;
	}

	printf("getaddrinfo(3) found %d results\n", i);
	freeaddrinfo(result);



	return 0;
}
