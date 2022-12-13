#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_PORT 8000
#define MAX_PENDING 5
#define MAX_LINE 256

void print_server_header(void);

int main() 
{
	struct sockaddr_in sin;
	char buf[MAX_LINE];
	int len;
	int s, new_s;

	print_server_header();

	/* build address data structure */
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(SERVER_PORT);

	/* Setup passive open */
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("simplex-talk: socket");
		exit(1);
	}
	if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
		perror("simplex-talk: bind");
		exit(1);
	}
	listen(s, MAX_PENDING);

	/* Wait for connection, then receieve and print text */
	while (1) {
		// I believe `accept` is a blocking function. It takes the
		// next connection from the listening sockert's request queue
		if ((new_s = accept(s, (struct sockaddr *)&sin, (socklen_t *)&len)) < 0) {
			perror("simplex-talk: accept");
			exit(1);
		}
		while ((len = recv(new_s, buf, sizeof(buf), 0)) != 0)
			fputs(buf, stdout);
		close(new_s);
		printf("Reached the end of the while loop!\n");
	}
}


void print_server_header(void) {
	printf("Simplex-server C edition\n");
	printf("\nInstructions:\n");
	printf(" - Wait for connections\n");
	printf(" - Close the server with ^C\n");
}
