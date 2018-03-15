

#include "stdafx.h"

#define DOGGIE_COUNT 2
//Get processor cores eventually

// Writer Reader Push
// Circle array
// if writer* == reader* no read
// 1024 blocks
//SOCKET RETURNS HOW MUCH WAS READ. 
//WRITE DATALENGTH INTO PACKET ON RECIEPT



std::thread* doggies;
int num_doggies = 0;
bool closing = false;
unsigned long id_base = 0;


void dispatch_init()
{

	num_doggies = std::thread::hardware_concurrency();
	doggies = (std::thread *)malloc(num_doggies * sizeof(std::thread));
	id_base = rand()*0x20004;//RAND_MAX * 0x20004 = max_int

}

void fetch() 
{
	InitialDPacketIn in_pac;
	while (!closing)
	{
		cb_read((void*)&in_pac,sizeof(InitialDPacketIn));
		deal_with(&in_pac);
	}
}

void dispatch_close()
{
	closing = true;
	PacketHeader no_act_pac;
	no_act_pac.action = INITIALD_NO_OPERATION;
	no_act_pac.data_len = 0;

	InitialDPacket end_pac;
	memcpy_s((void*)&end_pac, sizeof(InitialDPacket), (void*)&no_act_pac, sizeof(PacketHeader));

	for (int i = 0; i < num_doggies;i++)
	{
		cb_write((char*) &end_pac,sizeof(InitialDPacket));//release waiting doggie
		doggies[i].join();
	}
	circle_buff_close();
	free(doggies);
}

bool deal_with(InitialDPacketIn* packet)
{
	PacketHeader* header = (PacketHeader*)packet;
	switch (header->action)
	{
		case INITIALD_NO_OPERATION:
			return true;
		
		case INITIALD_SERVER_LOCATE:
			return initiald_server_locate(packet);
	}
}



int get_next_id()
{
	id_base += rand();//no one cares about overflow :D
	return id_base;
}