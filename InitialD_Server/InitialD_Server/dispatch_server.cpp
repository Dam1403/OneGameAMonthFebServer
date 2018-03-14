#include "stdafx.h" 

bool initiald_server_locate(InitialDPacketIn* packet)
{
	sockaddr_in* inaddr = &packet->sender;
	ServerLocate* locate_in = (ServerLocate*)&(packet->packet.data[sizeof(PacketHeader)]);//better way to do this....
	
	InitialDPacket pack_out; 
	ServerLocateResponse* response = (ServerLocate*)&pack_out;
	response.uid = get_next_id();

	send_out()
}
bool initiald_server_join(InitialDPacketIn* packet)
{
	return false;
}
bool initiald_server_leave(InitialDPacketIn* packet)
{
	return false;
}

bool initiald_server_list(InitialDPacketIn* packet)
{
	return false;
}
bool initiald_server_member_info(InitialDPacketIn* packet)
{
	return false;
}
