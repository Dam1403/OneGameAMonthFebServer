

#include "stdafx.h"

#define DOGGIE_COUNT 2
//Get processor cores eventually

// Writer Reader Push
// Circle array
// if writer* == reader* no read
// 1024 blocks
//SOCKET RETURNS HOW MUCH WAS READ. 
//WRITE DATALENGTH INTO PACKET ON RECIEPT



std::thread doggies[32];
unsigned int num_doggies = 0;
bool closing = false;
unsigned long id_base = 0;
unsigned int INITIALD_MAX_THREADS = 32;




void init_dispatch()
{
	unsigned int conc = std::thread::hardware_concurrency();
	num_doggies = INITIALD_MAX_THREADS > conc ? conc : INITIALD_MAX_THREADS;
	srand(get_seed());
	id_base = rand() * rand() + rand();//RAND_MAX * 0x1FFFF = about max_int

	for (int i = 0; i < num_doggies; i++)
	{
		doggies[i] = std::thread(fetch, i + 1);
	}
	doggies[0].join();//Hold main thread hostage.
	//main thread can console command??

}

unsigned long get_seed() 
{
	return time(0);//Pull this from random.org. This makes id's guessable
}

void fetch(int tagid) 
{
	InitialDPacketIn in_pac;
	int dog_tag = tagid;
	printf("Dog%i: %s", tagid, "Woof!!!\n");
	while (!closing)
	{
		printf("Dog%i: %s", tagid, "Barks!!!\n");
		get_datagram(&in_pac);
		deal_with(&in_pac);
		
	}
}

void dispatch_close()
{
	closing = true;
	InitialDPacket end_pac;
	PacketHeader* no_act_pac = (PacketHeader*)&end_pac;
	no_act_pac->action = INITIALD_NO_OPERATION;
	no_act_pac->data_len = 0;


	//memcpy_s((void*)&end_pac, sizeof(InitialDPacket), (void*)&no_act_pac, sizeof(PacketHeader));

	for (int i = 0; i < num_doggies;i++)
	{
		cb_write((char*) &end_pac,sizeof(InitialDPacket));//release waiting doggie
		doggies[i].join();
	}
	circle_buff_close();
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

void strip_newlines(char* victim, int vic_len)
{
	for (int i = 0; i < vic_len; i++)
	{
		switch (victim[i])
		{
		case '\n':
			victim[i] = '\0';
			break;

		case '\r':
			victim[i] = '\0';
			break;


		}
	}
}

int get_next_id()
{
	id_base += rand();//no one cares about overflow :D
	return id_base;
}