// InitialD_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"



#define MAX_IN_SIZE 2048


WSADATA wsa;


SOCKET broadcast_socket = INVALID_SOCKET;
char broadcast_buff[];
int broadcast_len;

struct sockaddr_in si_other;
struct sockaddr_in server;

addrinfo *results = NULL;

int bufflen;






int main()
{
	init_server_sockets();
}



