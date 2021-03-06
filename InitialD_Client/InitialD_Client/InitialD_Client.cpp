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


InitialDPacket initiald_pack_out;
PacketHeader* initiald_pack_head = (PacketHeader*)&initiald_pack_out;
void* initiald_pack_out_body = (&initiald_pack_out.data[0]) + sizeof(PacketHeader);

InitialDPacket initiald_pack_in;
PacketHeader* initiald_pack_in_head = (PacketHeader*)&initiald_pack_in;
void* initiald_pack_in_body = (&initiald_pack_in.data[0]) + sizeof(PacketHeader);




int bufflen;
int in_datalen;
//Hello Broadcast
SOCKET broadcast_socket = INVALID_SOCKET;
char* hello_server;


int init_in_socket();
void get_datagram(char* buff, int buff_len);
void cleanup_exit();
void recieve_loop();
bool send_out(char* buff, int bufflen, struct sockaddr* out_addr, int out_datalen);
void get_interface_broadcast_ips(_Out_ IN_ADDR** ips, _Out_ int* ip_count);
int init_broadcast_socket();
int broadcast(char data[], int data_len);
ULONG get_network_broadcast_addr(ULONG dwAddr, ULONG mask);


int main()
{
	//init_socket();
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cleanup_exit();
	}
	init_broadcast_socket();
	//init_in_socket();
	std::thread catman(recieve_loop);
	PacketHeader header;
	header.action = INITIALD_SERVER_LOCATE;
	header.data_len = sizeof(LocateServer);

	LocateServer ls;
	
	printf("Name: ");
	fgets(ls.name, sizeof(ls.name), stdin);
	printf("\n");
	const int out_size = sizeof(header) + sizeof(ls);

	char out_buff[out_size];


	memset(out_buff, '\0', sizeof(initiald_pack_out));

	printf("Waiting for data...");
	while (true)
	{
		memcpy(out_buff, &header, sizeof(header));
		memcpy(&out_buff[sizeof(header)], &ls, sizeof(ls));
		printf("Broadcasting on All Interfaces\n");
		broadcast(out_buff, out_size);
		printf("Recieving\n");



		getchar();


	}

	closesocket(broadcast_socket);
	WSACleanup();
	printf("Done");
	getchar();
	
    return 0;
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
	addrinfo* info;//these need to be freed
	iResult = getaddrinfo("0.0.0.0","0",NULL,&info);
	if (iResult != 0)
	{
		cleanup_exit();
	}
	iResult = bind(broadcast_socket,info->ai_addr, sizeof(sockaddr));
	if (iResult == SOCKET_ERROR)
	{
		cleanup_exit();
	}
	return 0;
}

int init_in_socket() {



	
	addrinfo *ptr = NULL;
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE;

	printf("Socket created.\n");
	char port_str[12] = { 0 };
	_itoa_s(LISTEN_PORT, port_str, 12);
	int iResult = getaddrinfo("0.0.0.0",port_str, &hints, &results);

	if (iResult != 0) {

		cleanup_exit();
	}
	in = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (in == INVALID_SOCKET) {
		cleanup_exit();
	}

	iResult = bind(broadcast_socket, results->ai_addr, results->ai_addrlen);

	if (iResult != 0) {
		cleanup_exit();
	}
	return 0;

}


void cleanup_exit() {
	printf("Failed. Error Code : %d", WSAGetLastError());
	WSACleanup();
	exit(EXIT_FAILURE);

}

void recieve_loop() 
{
	char buff[sizeof(InitialDPacket)];
	while (true)
	{
		get_datagram(buff,sizeof(InitialDPacket));
	}
}


void get_datagram(char* buff, int buff_len) {
	struct sockaddr_in sender;
	int sender_len = sizeof(struct sockaddr_in);


	fflush(stdout);

	//clear the buffer by filling null, it might have previously received data

	//try to receive some data, this is a blocking call
	if ((bufflen = recv(broadcast_socket, (char*)&initiald_pack_in, sizeof(InitialDPacket), 0)) == SOCKET_ERROR)
	{
		printf("recvfrom() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	
	ServerLocateResponse* response = (ServerLocateResponse*)initiald_pack_in_body;
	printf("Recieved %s\n", response->server_name);
	//print details of the client/peer and the data received
	//printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));

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


	ULONG localhost = 16777343;// MAKE ENDIAN PROOF!!!!
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

bool send_out(char* buff, int bufflen, struct sockaddr* out_addr, int out_datalen) 
{
	//now reply the client with the same data
	if (sendto(out, buff, bufflen, 0, out_addr, out_datalen) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		return false;
	}
	return true;
}

