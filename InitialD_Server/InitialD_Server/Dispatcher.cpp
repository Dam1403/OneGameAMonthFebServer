

#include "stdafx.h"


// Writer Reader Push
// Circle array
// if writer* == reader* no read
// 1024 blocks


char INPacketBuffer[MAX_PACKET_LENGTH * INBUFF_COUNT];
volatile int write_addr = 0;
const int DOGGIES_WHOA = -1;
int read_loc = 0;



HANDLE doggies[DOGGIE_COUNT];

void dispatch_init()
{

}

void doghouse() 
{
	while (true)
	{
		WaitOnAddress(&write_addr,(PVOID)&DOGGIES_WHOA, sizeof(int),INFINITE);

	}
}

void dispatcher();