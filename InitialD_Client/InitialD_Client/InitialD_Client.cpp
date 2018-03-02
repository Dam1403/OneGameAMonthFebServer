// InitialD_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define LISTEN_PORT 13337
#define BROADCAST_PORT 13338
#define IP_STR_LEN 16 
#define MAX_IN_SIZE 2048
#define MAX_INTERFACES 30

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
void get_interface_broadcast_ips(_Out_ IN_ADDR** ips, _Out_ int* ip_count);
int init_broadcast_socket();
int broadcast(char data[], int data_len);
ULONG get_network_broadcast_addr(ULONG dwAddr, ULONG mask);


int main()
{
	//init_socket();

	init_broadcast_socket();
	PacketHeader header;
	header.action = LOCATE_SERVER;
	header.data_len = sizeof(LocateServer);

	LocateServer ls;
	
	printf("Name: ");
	fgets(ls.name, sizeof(ls.name), stdin);
	printf("\n");

	const int out_size = sizeof(header) + sizeof(ls);

	char out_buff[out_size];

	memset(out_buff, '\0', sizeof(out_buff));

	memcpy(out_buff, &header, sizeof(header));
	memcpy(&out_buff[sizeof(header)], &ls, sizeof(ls));
	broadcast(out_buff,out_size);

	getchar();
	closesocket(broadcast_socket);
	WSACleanup();
	printf("Done");
	getchar();
	
    return 0;
}


/*
int main() 
{

	int count = 0;


	IN_ADDR* ips;
	get_interface_broadcast_ips(&ips, &count);

	int str_index = 0;
	for (int i = 0; i < count; i++) 
	{
		printf("%lu\n", (ULONG)ips[i].S_un.S_addr);
		
	}



	free(ips);
	getchar();
}

*/

int init_broadcast_socket() 
{
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cleanup_exit();
	}
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

int broadcast(char data[], int data_len)
{
	int count = 0;
	IN_ADDR* ips;
	get_interface_broadcast_ips(&ips, &count);

	int str_index = 0;

	sockaddr_in sender;
	int socklen = sizeof(sockaddr_in);

	sender.sin_family = AF_INET;
	sender.sin_port = htons(BROADCAST_PORT);

	for (int i = 0; i < count;i++) 
	{


		sender.sin_addr = ips[i];//wont work on linux
		int res = sendto(broadcast_socket, data, data_len, 0, (sockaddr*)&sender, socklen);
		if (res < 0)
		{
			printf("sendto failed with error: %d\n", WSAGetLastError());
			closesocket(in);
			WSACleanup();
			return res;
		}

	}



	free(ips);
}

void findLANServer() 
{
	
}

void get_interface_broadcast_ips( _Out_ IN_ADDR** ips, _Out_ int* ip_count ) 
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
		addresses = (MIB_IPADDRTABLE*)malloc(size);
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



	int entries = addresses->dwNumEntries;
	IN_ADDR* output = (IN_ADDR*)malloc(entries * sizeof(IN_ADDR));
	memset(output, '\0', entries);

	IN_ADDR in_addr;


	ULONG localhost = 16777343;
	for (int i = 0; i < addresses->dwNumEntries; i++)
	{
		ULONG dwAddr = addresses->table[i].dwAddr;
		ULONG dwMask = addresses->table[i].dwMask;
		in_addr.S_un.S_addr = get_network_broadcast_addr(dwAddr, dwMask);
		if (dwAddr == localhost) { in_addr.S_un.S_addr = localhost;}


		output[i] = in_addr;
		
	}

	if (addresses) {
		free(addresses);
		addresses = NULL;
	}


	*ips = output;
	*ip_count = entries;


	

}

ULONG get_network_broadcast_addr(ULONG dwAddr, ULONG mask)
{
	//00 11 11 11 mask
	ULONG antimask = ~mask;

	//11 00 00 00 antimask
	//10 10 10 10 dwAddr


	return (dwAddr & mask) ^ antimask;

	//11 10 10 10 return
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

