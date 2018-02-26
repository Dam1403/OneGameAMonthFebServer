// InitialD_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define LISTEN_PORT 13337
#define BROADCAST_PORT 13338

#define MAX_IN_SIZE 2048

SOCKET in = INVALID_SOCKET;
SOCKET out = INVALID_SOCKET;

WSADATA wsa;

struct sockaddr_in si_other;
struct sockaddr_in server;
addrinfo *results = NULL;


int bufflen;
int in_datalen;
//Hello Broadcast
SOCKET broadcast_socket = INVALID_SOCKET;
char* hello_server;

int init_socket();
void get_datagram();
void cleanup_exit();
int send_out(char* buff, int bufflen, struct sockaddr* out_addr, int out_datalen);
void get_interface_ips(_Out_ char* ips, _Out_ int* ip_count);


/*
int main()
{
	init_socket();
	char buff[12] = "AAAAAAAAAAA";
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
*/

int main() 
{
	char* ips = NULL;
	int count = 0;
	get_interface_ips(ips, &count);
	getchar();
}
int init_broadcast_socket() 
{
	broadcast_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (broadcast_socket == INVALID_SOCKET) {
		cleanup_exit();
	}
	bool option = TRUE;
	int iResult = setsockopt(broadcast_socket, SOL_SOCKET, SO_BROADCAST, (char*)&option, sizeof(option));
	printf("Sockets created.\n");
	if (iResult == SOCKET_ERROR) {
		cleanup_exit();
	}
	return 0;
}

int init_socket() {

	addrinfo *ptr = NULL;

	addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cleanup_exit();
	}

	printf("Socket created.\n");
	int iResult = getaddrinfo("0.0.0.0", "13338", &hints, &results);

	if (iResult != 0) {

		cleanup_exit();
	}

	out = socket(results->ai_family, results->ai_socktype, results->ai_protocol);

	if (out == INVALID_SOCKET) {
		cleanup_exit();
	}

	bool option = TRUE;
	iResult = setsockopt(out, SOL_SOCKET, SO_BROADCAST, (char*)&option, sizeof(option));
	if (iResult != 0) {
		cleanup_exit();
	}

	iResult = bind(out, results->ai_addr, results->ai_addrlen);

	if (iResult != 0) {
		cleanup_exit();
	}

	get_datagram();
	return 0;

}


void cleanup_exit() {
	printf("Failed. Error Code : %d", WSAGetLastError());
	WSACleanup();
	exit(EXIT_FAILURE);

}
void get_datagram() {
	struct sockaddr_in sender;
	int sender_len = sizeof(struct sockaddr_in);

	printf("Waiting for data...");
	fflush(stdout);

	//clear the buffer by filling null, it might have previously received data
	char buff[MAX_IN_SIZE];
	//try to receive some data, this is a blocking call
	if ((bufflen = recv(out, buff, MAX_IN_SIZE, 0)) == SOCKET_ERROR)
	{
		printf("recvfrom() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	//print details of the client/peer and the data received
	//printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
	printf("Data: %s\n", buff);
}

int broadcast(char* data, int data_len)
{
	sockaddr_in sender;

	sender.sin_family = AF_INET;
	sender.sin_port = htons(BROADCAST_PORT);
	sender.sin_addr.S_un.S_addr = INADDR_BROADCAST;//wont work on linux

	if (strnlen_s(data, 64) != data_len) 
	{
		printf("broadcast strlen mismatch");
		exit(EXIT_FAILURE);
	}

	int socklen = sizeof(sockaddr_in);
	int res = sendto(broadcast_socket, data, data_len, 0, (struct sockaddr*)&sender, socklen);
	if (res < 0)
	{
		printf("sendto failed with error: %d\n", WSAGetLastError());
		closesocket(in);
		WSACleanup();
		return res;
	}
}

void findLANServer() 
{
	
}

void get_interface_ips( _Out_ char* ips, _Out_ int* ip_count ) 
{

	
	DWORD size = 0;
	PMIB_IPADDRTABLE addresses = (MIB_IPADDRTABLE*)malloc(sizeof(MIB_IPADDRTABLE));


	if (!addresses) 
	{
		printf("PMIB_IPADDRTABLE malloc failure");
		exit(EXIT_FAILURE);
	}

	if (GetIpAddrTable(addresses, &size, 0) == ERROR_INSUFFICIENT_BUFFER) 
	{
		free(addresses);
		addresses = (MIB_IPADDRTABLE*)malloc(sizeof(MIB_IPADDRTABLE) * size);
		if (!addresses) 
		{
			printf("PMIB_IPADDRTABLE malloc failure");
			exit(EXIT_FAILURE);
		}
	}
	
	int result = GetIpAddrTable(addresses, &size, 0);
	
	if(result != NO_ERROR)
	{
		printf("getipaddrtable failed with result %i",result);
		free(addresses);
		exit(EXIT_FAILURE);
	}


	//"XXX.XXX.XXX.XXX\0" length=15 + null turminator

	int out_len = (15 * size) + 1;
	char* output = (char*)malloc(out_len);
	memset(output, '\0', out_len);

	IN_ADDR in_addr;
	for (int i = 0; i < addresses->dwNumEntries; i++)
	{
		in_addr.S_un.S_addr = addresses->table[i].dwAddr;
		inet_ntop(AF_INET,&in_addr,output + (16*i),16);
	}

	if (addresses) {
		free(addresses);
		addresses = NULL;
	}

	printf(output);
	ips = output;
	*ip_count = size;


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

