// InitialD_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define LISTEN_PORT 13337
#define MAX_IN_SIZE 2048

SOCKET in = INVALID_SOCKET;
SOCKET out = INVALID_SOCKET;
WSADATA wsa;

struct sockaddr_in si_other;
struct sockaddr_in server;
addrinfo *results = NULL;


int bufflen;
int in_datalen;


int init_socket();
void get_datagram();
void cleanup_exit();
int send_out(char* buff, int bufflen, struct sockaddr* out_addr, int out_datalen);

int main()
{
	init_socket();
	char buff[12] = { 'A' };
	int res = sendto(out, buff, 12, 0,results->ai_addr,results->ai_addrlen);
	printf("%i",res);
	if (res == SOCKET_ERROR) {
		closesocket(out);
		cleanup_exit();
	}
	getchar();
	closesocket(out);
	WSACleanup();
	printf("Done");
	
    return 0;
}



int init_socket() {

	addrinfo *ptr = NULL;

	addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cleanup_exit();
	}

	printf("Socket created.\n");

	int iResult = getaddrinfo("192.168.1.1", "13337", &hints, &results);

	if (iResult != 0) {

		cleanup_exit();
	}

	out = socket(results->ai_family, results->ai_socktype, results->ai_protocol);

	if (out == INVALID_SOCKET) {
		cleanup_exit();
	}
	/*
	iResult  = connect(out,results->ai_addr,(int)results->ai_addrlen);

	if (iResult == SOCKET_ERROR) {
		closesocket(out);
		out = INVALID_SOCKET;
		cleanup_exit();
	}
	*/

	return 0;

}


void cleanup_exit() {
	printf("Failed. Error Code : %d", WSAGetLastError());
	WSACleanup();
	exit(EXIT_FAILURE);

}
void get_datagram() {

	while (1)
	{
		printf("Waiting for data...");
		fflush(stdout);

		//clear the buffer by filling null, it might have previously received data
		char buff[MAX_IN_SIZE];
		//try to receive some data, this is a blocking call
		if ((bufflen = recvfrom(in, buff, MAX_IN_SIZE, 0, (struct sockaddr *) &si_other, &in_datalen)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		//print details of the client/peer and the data received
		//printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		printf("Data: %s\n", buff);
	}

}




int send_out(char* buff, int bufflen, struct sockaddr* out_addr, int out_datalen) 
{
	//now reply the client with the same data
	if (sendto(out, buff, bufflen, 0, out_addr, out_datalen) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	return 0;
}

