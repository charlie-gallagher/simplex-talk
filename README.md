# Basic socket implementations
This repository contains two client/server messaging systems implemented _very
basically_ using sockets. The point is to have a clear reference for when I
forget how to implement a socket in C or Python. These are missing many features
that good socket-based programs should have, so for the love of computer
networking don't use these for anything other than simple demonstrations.

## C implementation
I took the C implementation from the book Computer Networks: A Systems Approach,
by Davie and Peterson. The edition of the book is old, and so the original
approach was also slightly out of date. I made the following adjustments:

- Replaced `bzero` with `memset` when zeroing out the "hints" struct. I think
  memset is clearer, since bzero requires you cast your struct pointer to a
`char *` type.
- Replaced `gethostbyname` with `getaddrinfo`, to implement a more flexible
  address resolution process.

## Python implementation
The `socket` module contains some utility functions to quickly set up and tear
down sockets and connections. I first implemented a client/server pair with
these (`socket.create_connection` and `socket.create_server`). Afterwards, I
reimplemented things with my own versions of these that explicitly create the
sockets, bind them to the network, and attempt a connection.

They're more verbose, but not in any nice way. All of the log messages go to
`stdout` for the most part, although maybe they should be redirected to
`stderr`. That's something I'm never quite sure about.

---

# Notes on C implementation
Actually, in preparation for writing the above, I need to learn a bit more about
the functions, their versions, etc.

`gethostbyname()` resolves a string host into a standard address. Actually, more
like an address object with the following format:

```c
struct  hostent {
             char    *h_name;        /* official name of host */
             char    **h_aliases;    /* alias list */
             int     h_addrtype;     /* host address type */
             int     h_length;       /* length of address */
             char    **h_addr_list;  /* list of addresses from name server */
     };
```

Notice that this includes an official name, list of aliases, and addresses. The
final `h_addr_list` contains the addresses in network byte order. A relevant
file is `/etc/hosts`, which contains some known host information.

The function is not recommended on Mac (BSD) and deprecated on Linux. Instead,
it's replaced by `getaddrinfo(3)`, which has an updated interface.

```c
int getaddrinfo(
    const char *hostname,
    const char *servname,
    const struct addrinfo *hints,
    struct addrinfo **res
);
```

`hostname` is a string to lookup. `servname` is the _service_ name, used to look
up the port number. It could also be a decimal port number. At least one must be
given, or else `NULL`. The `addrinfo` struct contains hints, restrictions on the
types of addresses being searched for in a standard format. Finally, `res` is a
pointer to a pointer to an `addrinfo` object. The result is actually returned as
a linked list, filled into `res` as a side-effect, while the `int` return value
is an error code.

The required includes are:

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
```

The `addrinfo` struct has this definition:

```c
struct addrinfo {
         int ai_flags;           /* input flags */
         int ai_family;          /* protocol family for socket */
         int ai_socktype;        /* socket type */
         int ai_protocol;        /* protocol for socket */
         socklen_t ai_addrlen;   /* length of socket-address */
         struct sockaddr *ai_addr; /* socket-address for socket */
         char *ai_canonname;     /* canonical name for service location */
         struct addrinfo *ai_next; /* pointer to next in list */
 };
```

For me, this is usually

```c
struct addrinfo myai {
    0,              /* Flags*/
    AF_INET,
    SOCK_STREAM,
    0,              /* Protocol */
    0,              /* Address length */
    0,              /* sockaddr address ai_addr */
    0,              /* Canonical name for service location */
    0               /* Pointer to next item in list */
}
```

Or alternatively, you can do less work and just zero-fill the whole struct and
then fill in the family and socket type afterwards.

```c
struct addrinfo myai;
memset(&myai, 0, sizeof(myai));
myai.ai_family = AF_INET;
myai.ai_socktype = SOCK_STREAM;
```

With everything zeroed out except the family and socket type, you can filter the
results of `getaddrinfo()` to just those addresses we're actually interested in
connecting with.

The whole procedure for finding a suitable host looks something like this. Say
you know you want to connect with TCP to localhost on port 8000. First, I'll
define the hints, and second I'll get a list of results. I have to loop through
the results via the linked list. Here is a small program that tries to connect
to `localhost:8000`, which gels well with the server program in this repo. Try
combining them to get `localhost:8000` to connect successfully.

```c
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
```

Note that this is heavily inspired by the example code in the MacOS
`getaddrinfo(3)` man page.

You could then use `getnameinfo(3)` to get more information about the host you
connected to. I implemented this more fully in `addrinfo.c`, but without
attempting to connect, so it's not a very dynamic program.




---

Charlie Gallagher, December 2022
