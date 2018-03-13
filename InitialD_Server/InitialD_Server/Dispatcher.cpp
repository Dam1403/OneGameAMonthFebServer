

#include "stdafx.h"


// Writer Reader Push
// Circle array
// if writer* == reader* no read
// 1024 blocks
//SOCKET RETURNS HOW MUCH WAS READ. 
//WRITE DATALENGTH INTO PACKET ON RECIEPT


SOCKET in = INVALID_SOCKET;
char INPacketBuffer[MAX_PACKET_LENGTH];


std::thread doggies[DOGGIE_COUNT];

void dispatch_init()
{

}

void doghouse() 
{
	while (true)
	{
	}
}


void dispatcher();