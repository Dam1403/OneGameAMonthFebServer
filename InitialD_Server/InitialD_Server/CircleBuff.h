#pragma once


void circle_buff_init(int size, int step);
int cb_write(char* data, int len);
void increment_write();
int cb_read(void* dst, int dst_len);
void increment_read();
void circle_buff_close();