/* Simplex-talk client
 *
 * Inspired by: Computer Networks, Chapter 1.4.2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_PORT 8000
#define MAX_LINE 256

int main(int argc, char *argv[])
{
	FILE *fp;
	struct hostent *hp;
	struct sockaddr_in sin;
	char *host;
	char buf[MAX_LINE];
	int s;
	int len;

	if (argc == 2) {
		host = argv[1];
	} else {
		fprintf(stderr, "usage: simplex-talk <host>\n");
		exit(1);
	}

	/* Translate host name into peer's IP address */
	hp = gethostbyname(host);	// The manual lists this function as
					// obsolete, instead use getaddrinfo(3)
	if (!hp) {
		fprintf(stderr, "simplex-talk: unknown host: %s\n", host);
		exit(1);
	}

	/* Build address data structure */
	bzero((char *)&sin, sizeof(sin));  // bzero (strings.h) sets a string
					   // to all NULL values. Used here
					   // to set all data in sin to 0
	sin.sin_family = AF_INET;  // Always AF_INET
	bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length); // Copy byte sequence
	sin.sin_port = htons(SERVER_PORT);

	/* Active open */
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("simplex-talk: socket");
		exit(1);
	}
	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		perror("simplex-talk: connect");
		close(s);
		exit(1);
	}
	printf("Connected to %s\n", host);


	/* Main loop: get and send lines of text */
	while (fgets(buf, sizeof(buf), stdin)) {
		buf[MAX_LINE - 1] = '\0';
		len = strlen(buf) + 1;
		send(s, buf, len, 0);
	}

	return 0;
}
