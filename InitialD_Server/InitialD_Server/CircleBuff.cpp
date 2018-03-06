#include "stdafx.h"



HANDLE rw_lock;
char* CircleBuffer;
int buff_step;
int circle_buff_len;

volatile int write_loc = 0;
int read_loc = 0;
bool full = false;


const int DOGGIES_WHOA = -1;





void circle_buff_init(int size,int step)
{
	circle_buff_len = size * step;
	CircleBuffer = (char*)malloc(circle_buff_len);
	if (!CircleBuffer) 
	{
		printf("CircleBuff Init Failure");
		exit(EXIT_FAILURE);
	}
	buff_step = step;
	write_loc, read_loc = 0;



}

int cb_write(char* data, int len)
{
	if (len > buff_step)
	{
		printf("Write Length greater than step size");
		return 0;
	}

	//get read_loc
	if (full) increment_read();//for smooth overwrites
	
	memcpy((void*)&CircleBuffer[write_loc],(void*)data,len);
	increment_write();
	WakeByAddressSingle((PVOID)&write_loc);
	//release read lock

}



void increment_write()
{
	write_loc += buff_step;

	if (write_loc == circle_buff_len) write_loc = 0;

	if (write_loc == read_loc) {
		full = true;
	}

}


int cb_read(_Out_ void* dst, int dst_len)
{

	if (dst_len > buff_step)
	{
		printf("Read Length greater than buffer size");
		return 0;
	}

	//get read_loc
	if (read_loc == write_loc && !full)
	{
		//release read lock
		WaitOnAddress(&write_loc,(PVOID)DOGGIES_WHOA,sizeof(int),INFINITE);
	}


	memcpy((void*)dst, (void*)&CircleBuffer[read_loc], dst_len);
	increment_read();
	//release read lock

}

void increment_read() 
{
	read_loc += buff_step;
	if (read_loc == circle_buff_len) read_loc = 0;

	
}
void circle_buff_close() 
{
	if (CircleBuffer) 
	{
		free(CircleBuffer);
	}

}







