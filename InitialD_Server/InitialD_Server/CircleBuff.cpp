#include "stdafx.h"
//This is cool but useless. recv from can be called from multiple threads. and the os will handle the sychronization.
//make the doggies fetch directly from the socket.

std::mutex rw_mutex;
char* CircleBuffer;
int buff_step;
int circle_buff_len;

volatile int write_loc = 0;
int read_loc = 0;
bool full = false;

FILE* fp;
const int DOGGIES_WHOA = -1;





void init_circle_buff(int count,int step)
{
	circle_buff_len = count * step;
	CircleBuffer = (char*)malloc(circle_buff_len);
	if (!CircleBuffer) 
	{
		printf("CircleBuff Init Failure");
		exit(EXIT_FAILURE);
	}
	buff_step = step;
	write_loc, read_loc = 0;
	fopen_s(&fp,"Logs.txt","w");
	fflush(fp);
}

int cb_write(char* data, int len)
{
	if (len > buff_step)
	{
		printf("Write Length greater than step size");
		return 0;
	}

	//get read_loc
	rw_mutex.lock();
	if (full) increment_read();//for smooth overwrites
	
	memcpy((void*)&CircleBuffer[write_loc],(void*)data,len);
	WakeByAddressSingle((PVOID)&write_loc);
	fprintf(fp,"Wrote %i\n",*((int*)&CircleBuffer[write_loc]));
	increment_write();
	rw_mutex.unlock();
	//release read lock
	return len;

}

void increment_write()
{
	write_loc += buff_step;
	if (write_loc == circle_buff_len) write_loc = 0;

	if (full)  read_loc = write_loc;

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
	rw_mutex.lock();
	while (read_loc == write_loc && !full)
	{
		//release read lock
		rw_mutex.unlock();
		WaitOnAddress(&write_loc,(PVOID)&write_loc,sizeof(int),INFINITE);
		rw_mutex.lock();
	}
	memcpy((void*)dst, (void*)&CircleBuffer[read_loc], dst_len);
	increment_read();
	rw_mutex.unlock();
	return buff_step;
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







