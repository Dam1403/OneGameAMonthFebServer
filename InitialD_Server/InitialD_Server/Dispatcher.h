#pragma once

void dispatch_init();
void fetch();
void dispatch_close();
bool deal_with(InitialDPacketIn* packet);
int get_next_id();


bool initiald_server_locate(InitialDPacketIn* packet);
bool initiald_server_join(InitialDPacketIn* packet);
bool initiald_server_leave(InitialDPacketIn* packet);
bool initiald_server_list(InitialDPacketIn* packet);
bool initiald_server_member_info(InitialDPacketIn* packet);