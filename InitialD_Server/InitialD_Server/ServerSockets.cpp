
#include "stdafx.h"

#define LISTEN_PORT  "13337"
#define BROADCAST_PORT "13338"


SOCKET in = INVALID_SOCKET;
char in_buff[sizeof(InitialDPacket)];

SOCKET out = INVALID_SOCKET;
char out_buff[sizeof(InitialDPacket)];

SOCKET bc = INVALID_SOCKET;
char bc_buff[sizeof(InitialDPacket)];

WSAData wsa;
addrinfo hints;
addrinfo *results;





bool init_server_sockets() {


	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cleanup_exit();
	}
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE;

	init_in_socket();
	init_out_socket();
	init_broadcast_socket();


	return 0;

}

bool init_broadcast_socket()
{
	bc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (bc == INVALID_SOCKET) {
		cleanup_exit();
	}
	bool option = TRUE;
	int iResult = setsockopt(bc, SOL_SOCKET, SO_BROADCAST, (char*)&option, sizeof(option));
	if (iResult == SOCKET_ERROR) {
		cleanup_exit();
	}
	circle_buff_init(sizeof(InitialDPacket),);
}

bool init_out_socket()
{
	int iResult = getaddrinfo("0.0.0.0", "13338", &hints, &results);

	if (iResult != 0) {
		cleanup_exit();
	}
	out = socket(results->ai_family, results->ai_socktype, results->ai_protocol);

	iResult = bind(out, results->ai_addr, results->ai_addrlen);
	if (iResult != 0) {
		cleanup_exit();
	}
}


bool init_in_socket()
{
	int iResult = getaddrinfo(NULL, LISTEN_PORT, &hints, &results);

	if (iResult != 0) {
		cleanup_exit();
	}

	in = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (in == INVALID_SOCKET) {
		cleanup_exit();
	}

	iResult = bind(in, results->ai_addr, results->ai_addrlen);

	if (iResult != 0) {
		cleanup_exit();
	}
	return 0;
}

int broadcast(char* data, int data_len)
{
	sockaddr_in sender;

	sender.sin_family = AF_INET;
	sender.sin_port = htons(13338);
	sender.sin_addr.S_un.S_addr = INADDR_BROADCAST;

	int socklen = sizeof(sockaddr_in);
	int res = sendto(bc, "BBBBBBBBBBBBBBBB", 16, 0, (struct sockaddr*)&sender, socklen);
	if (res < 0)
	{
		printf("sendto failed with error: %d\n", WSAGetLastError());
		closesocket(in);
		WSACleanup();
		return res;
	}
}

void get_datagram() {

	while (1)
	{
		printf("Waiting for data...");
		fflush(stdout);
		int recv_len = 0;
		if ((recv_len = recvfrom(in, in_buff, sizeof(InitialDPacket), 0, (struct sockaddr *) &si_other, &in_datalen)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

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

void cleanup_exit() {
	printf("Failed. Error Code : %d", WSAGetLastError());
	WSACleanup();
	exit(EXIT_FAILURE);

}