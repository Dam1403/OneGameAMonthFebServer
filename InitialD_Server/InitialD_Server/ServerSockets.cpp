
#include "stdafx.h"

#define BROADCAST_PORT  "13337"
#define LISTEN_PORT "13338"


SOCKET in = INVALID_SOCKET;
char in_buff[sizeof(InitialDPacket)];

SOCKET out = INVALID_SOCKET;
char out_buff[sizeof(InitialDPacket)];

SOCKET bc = INVALID_SOCKET;
char bc_buff[sizeof(InitialDPacket)];

WSAData wsa;
addrinfo hints;
addrinfo *results;



InitialDPacket initiald_pack_out;
PacketHeader* initiald_pack_head = (PacketHeader*)&initiald_pack_out;
void* initiald_pack_data = (&initiald_pack_out.data[0]) + sizeof(PacketHeader);
int initiald_size_nohead = sizeof(InitialDPacket) - sizeof(PacketHeader);

bool init_server_sockets() {
	PacketHeader ph_schema;

	strncpy_s(
		initiald_pack_head->protocol,
		sizeof(initiald_pack_head->protocol),
		ph_schema.protocol,
		sizeof(ph_schema.protocol));
	strncpy_s(
		initiald_pack_head->vers1, 
		sizeof(initiald_pack_head->vers1), 
		ph_schema.vers1, 
		sizeof(ph_schema.vers1));

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

	return true;
}

bool init_out_socket()
{
	size_t iResult = getaddrinfo("0.0.0.0", BROADCAST_PORT, &hints, &results);

	if (iResult != 0) {
		cleanup_exit();
	}
	out = socket(results->ai_family, results->ai_socktype, results->ai_protocol);

	iResult = bind(out, results->ai_addr, results->ai_addrlen);
	if (iResult != 0) {
		cleanup_exit();
	}

	return true;
}


bool init_in_socket()
{
	size_t iResult = getaddrinfo(NULL, LISTEN_PORT, &hints, &results);

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
	sender.sin_port = std::atoi(BROADCAST_PORT);
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

void get_datagram(_Out_ InitialDPacketIn* pack_in) {


	int in_len = sizeof(sockaddr_in);
	fflush(stdout);

	int recv_len = 0;
	if ((recv_len = recvfrom(in, (char*)&pack_in->packet, sizeof(InitialDPacket), 0, (sockaddr *)&pack_in->sender, &in_len)) == SOCKET_ERROR)
	{
		printf("recvfrom() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
		

}
int initiald_send_packet(int action,void* action_struct,int struct_len,sockaddr_in inaddr)
{
	initiald_pack_head->action = action;//HEADERS MISSING!!!
	initiald_pack_head->data_len = struct_len;

	memcpy_s(initiald_pack_data,initiald_size_nohead,action_struct,struct_len);


	if (sendto(out, (char*)&initiald_pack_out, sizeof(InitialDPacket), 0, (sockaddr*)&inaddr, sizeof(sockaddr_in)) == SOCKET_ERROR)
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