//CLIENT!!!!

#pragma once
#define LOCATE_SERVER 1001


typedef struct PacketHeader {
	char protocol[9] = "InitialD";
	char vers1[7] = "0.0.1a";
	unsigned short action;
	unsigned short data_len;
};


typedef struct LocateServer {
	char name[32];
}LocateServer;
