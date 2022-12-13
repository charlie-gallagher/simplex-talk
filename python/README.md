# Simplex-talk in Python
I want to try to do a Simplex program in Python, to view the socket interface
from a different perspective than C.


The module `socket` provides a similar interface to C for dealing with sockets.
Some of the modules in the namespace are already familiar:

```
socket() -- create a new socket object
socketpair() -- create a pair of new socket objects [*]
fromfd() -- create a socket object from an open file descriptor [*]
send_fds() -- Send file descriptor to the socket.
recv_fds() -- Recieve file descriptors from the socket.
fromshare() -- create a socket object from data received from socket.share() [*]
gethostname() -- return the current hostname
gethostbyname() -- map a hostname to its IP number
gethostbyaddr() -- map an IP number or hostname to DNS info
getservbyname() -- map a service name and a protocol name to a port number
getprotobyname() -- map a protocol name (e.g. 'tcp') to a number
ntohs(), ntohl() -- convert 16, 32 bit int from network to host byte order
htons(), htonl() -- convert 16, 32 bit int from host to network byte order
inet_aton() -- convert IP addr string (123.45.67.89) to 32-bit packed format
inet_ntoa() -- convert 32-bit packed format IP to string (123.45.67.89)
socket.getdefaulttimeout() -- get the default timeout value
socket.setdefaulttimeout() -- set the default timeout value
create_connection() -- connects to an address, with an optional timeout and
                       optional source address.
```

An asterisk indicates not available on all platforms.

It also exports constants for the protocols that are the same as in C:

```
AF_INET
AF_UNIX
SOCK_STREAM
SOCK_DGRAM
SOCK_RAW
```

# Active and Passive Sockets
## Active open a socket for TCP
Create a socket with `socket()`, which has the following prototype:

```py
socket(family, type, proto)

# TCP
socket(family=AF_INET, type=SOCK_STREAM, proto=0)
```

These actually default to a TCP connection, so to create a socket for TCP all
you need to do is `socket()`.

The methods to the object returned by this function are the typical follow-up
functions:

```
_accept() -- accept connection, returning new socket fd and client address
bind(addr) -- bind the socket to a local address
close() -- close the socket
connect(addr) -- connect the socket to a remote address
connect_ex(addr) -- connect, return an error code instead of an exception
dup() -- return a new socket fd duplicated from fileno()
fileno() -- return underlying file descriptor
getpeername() -- return remote address [*]
getsockname() -- return local address
getsockopt(level, optname[, buflen]) -- get socket options
gettimeout() -- return timeout or None
listen([n]) -- start listening for incoming connections
recv(buflen[, flags]) -- receive data
recv_into(buffer[, nbytes[, flags]]) -- receive data (into a buffer)
recvfrom(buflen[, flags]) -- receive data and sender's address
recvfrom_into(buffer[, nbytes, [, flags])
  -- receive data and sender's address (into a buffer)
sendall(data[, flags]) -- send all data
send(data[, flags]) -- send data, may not send all of it
sendto(data[, flags], addr) -- send data to a given address
setblocking(bool) -- set or clear the blocking I/O flag
getblocking() -- return True if socket is blocking, False if non-blocking
setsockopt(level, optname, value[, optlen]) -- set socket options
settimeout(None | float) -- set or clear the timeout
shutdown(how) -- shut down traffic in one or both directions
```

An active open is used by a client to connect to a server. The socket is
created and bound to the network with `bind()`. Then it can attempt a remote
connection with `connect()` or `connect_ex()` (the latter returns an error code
instead of issuing an exception).

Data is exchanged with `send()` in the case of TCP, since this is a
connection-oriented protocol. If it was a datagram protocol like UDP, you would
use `send_to()`. Data is received with `recv()`.

## Passive open socket for TCP
This is similar, but instead of `connect()`ing, the socket is set up to listen
on the designated (presumably well-known and accessible on some network) port.
The basic flow is to create the socket as above with `socket()`, bind it to a
local address (`bind()`), and begin to `listen()` for incoming connections with
some given maximum number of connections that can queue.

In C, new connections are `accept`ed. This pulls the first connection request
off of the FIFO (or is it LIFO?) and assigns it its own socket and file
descriptor, through which the server can interact with the connection on a
dedicated port.

In Python's case, `_accept()` is prefixed with an underscore, so that should
mean it's not intended to be part of the public API. But it's also in this
documentation.

Ah okay, the class I was looking at the documentation for was `_socket.socket`,
but there's another sublass that adds methods for `accept()`ing, duplicating the
socket, sending a file efficiently in binary mode (as opposed to the
string-oriented `send()`).

## Convenience functions
It turns out there are convenience functions for the type of standard interface
I'm trying to create. For creating a server:

```py
with create_server(('', 8000)) as server:
     while True:
         conn, addr = server.accept()
         # handle new connection
```

This is nice because it automatically cleans up when the `with` statement ends,
like with non-special files. There's also `create_connection()`, which connects
to an address and returns a socket. Neither of these are methods, they are
functions exported by the module.

## Other functions
`getaddrinfo()` resolves the host and port into address info entities.

When host is an empty string, it is interpreted to mean 'localhost'.

`gethostbyname()` has a companion C function. It returns the IP address of the
host given the host's name (e.g. `google.com`).

`getservbyname()` returns the port number for a service, so presumably sending
`getservbyname('postgresql')` returns 5432.

`htons` is for converting a 16-bit integer from host to network byte order.


## Execution
The boring way to execute this is to use `create_server()` and
`create_connection()`. Let's do these first, and then re-implement them myself.

There's some encode/decode business you have to deal with before you can
transmit data over the network. You must `str.encode()` to UTF-8 before sending,
and `data.decode()` when receiving. I guess this standardizes the byte stream,
so you're sure what you're sending/receivig across the network.




---

Charlie Gallagher, December 2022
