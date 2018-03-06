// InitialD_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define LISTEN_PORT  "13337"
#define BROADCAST_PORT "13338"


#define MAX_IN_SIZE 2048

SOCKET in = INVALID_SOCKET;
SOCKET out = INVALID_SOCKET;

WSADATA wsa;


SOCKET broadcast_socket = INVALID_SOCKET;
char broadcast_buff[];
int broadcast_len;

struct sockaddr_in si_other;
struct sockaddr_in server;

addrinfo *results = NULL;

char in_buff[MAX_IN_SIZE];
int bufflen;
int in_datalen;


int init_socket();
void get_datagram();
void cleanup_exit();
int broadcast(char* data, int data_len);
int send_out(char* buff, int bufflen, struct sockaddr* out_addr, int out_datalen);

void write_random() 
{
	int write = 0;
	while (true)
	{
		int rando = rand() % 300;
		printf("Writer Sleeping for %i milliseconds\n", rando);
		cb_write((char*)&write, sizeof(int));
		printf("Wrote: %i\n", write);
		write++;
		Sleep(rando);
	}

}


void read_random(const char* name)
{
	int red;
	while (true)
	{
		int rando = rand() % 1000;
		printf("%s Sleeping for %i milliseconds\n",name, rando);
		cb_read((void*)&red, sizeof(int));
		printf("%s Read: %i\n",name , red);
		Sleep(rando);
	}

}


int main()
{
	srand(time(NULL));
	circle_buff_init(10,sizeof(int));
	int eight = 8;
	std::thread writer(write_random);
	Sleep(545);
	std::thread reader(read_random,"Reader1");
	Sleep(545);
	std::thread reader2(read_random,"Reader2");
	Sleep(545);
	std::thread reader3(read_random, "Reader3");
	Sleep(545);
	std::thread reader4(read_random, "Reader4");

	writer.join();
	reader.join();
	reader2.join();

	getchar();


	circle_buff_close();
}

/*
int main()
{
	init_socket();
	int res;
	struct sockaddr_in sender;
	int sender_len = sizeof(struct sockaddr_in);
	do
	{
		printf("listening\n");
		res = recvfrom(in, in_buff, MAX_IN_SIZE, 0,(sockaddr* )&sender, &sender_len);
		if (res < 0) 
		{
			printf("recvfrom failed with error: %d\n", WSAGetLastError());
			closesocket(in);
			WSACleanup();
			return res;
		}
		in_buff[res] = '\0';
		printf("Recieved: %s\n",in_buff);
		res = sendto(in, "BBBBBBBBBBBBBBBB", 16, 0, (struct sockaddr*)&sender, sender_len);
		if (res < 0)
		{
			printf("sendto failed with error: %d\n", WSAGetLastError());
			closesocket(in);
			WSACleanup();
			return res;
		}



	} while (res > 0);
	getchar();
	closesocket(in);
	WSACleanup();
	printf("Done");
	
    return 0;
}
*/


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

	int iResult = getaddrinfo(NULL,LISTEN_PORT, &hints, &results);

	if (iResult != 0) {

		cleanup_exit();
	}

	in = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (in == INVALID_SOCKET) {
		cleanup_exit();
	}

	broadcast_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (broadcast_socket == INVALID_SOCKET) {
		cleanup_exit();
	}
	bool option = TRUE;
	iResult = setsockopt(broadcast_socket,SOL_SOCKET,SO_BROADCAST,(char*)&option,sizeof(option));
	printf("Sockets created.\n");
	if (iResult == SOCKET_ERROR) {
		cleanup_exit();
	}
	iResult = bind(in,results->ai_addr,results->ai_addrlen);

	if (iResult != 0) {
		cleanup_exit();
	}

	char cc[1] = { 'd' };
	broadcast(cc, 0);
	return 0;

}


int broadcast(char* data, int data_len) 
{
	sockaddr_in sender;

	sender.sin_family = AF_INET;
	sender.sin_port = htons(13338);
	sender.sin_addr.S_un.S_addr = INADDR_BROADCAST;

	int socklen = sizeof(sockaddr_in);
	int res = sendto(broadcast_socket, "BBBBBBBBBBBBBBBB", 16, 0, (struct sockaddr*)&sender, socklen);
	if (res < 0)
	{
		printf("sendto failed with error: %d\n", WSAGetLastError());
		closesocket(in);
		WSACleanup();
		return res;
	}
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

		if ((bufflen = recvfrom(in, in_buff, MAX_IN_SIZE, 0, (struct sockaddr *) &si_other, &in_datalen)) == SOCKET_ERROR)
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



