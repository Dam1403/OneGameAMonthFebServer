#pragma once




void init_dispatch();
void fetch(int tagid);
void dispatch_close();
bool deal_with(InitialDPacketIn* packet);
int get_next_id();
void strip_newlines(char* victim, int vic_len);
unsigned long get_seed();



//dispatch_server.cpp!!!
bool initiald_server_locate(InitialDPacketIn* packet);
bool initiald_server_join(InitialDPacketIn* packet);
bool initiald_server_leave(InitialDPacketIn* packet);
bool initiald_server_list(InitialDPacketIn* packet);
bool initiald_server_member_info(InitialDPacketIn* packet);