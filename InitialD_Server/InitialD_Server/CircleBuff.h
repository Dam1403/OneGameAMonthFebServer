#pragma once


void init_circle_buff(int count, int step);
int cb_write(char* data, int len);
void increment_write();
int cb_read(void* dst, int dst_len);
void increment_read();
void circle_buff_close();