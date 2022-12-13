default: all

CHAT_BASE = simplex-c-

all:
	gcc -o $(CHAT_BASE)client c/simplex_talk_client.c
	gcc -o $(CHAT_BASE)server c/simplex_talk_server.c
	gcc -o resolve_hostname c/resolve-hostname.c
	gcc -o addrinfo c/addrinfo.c

