#include "stdafx.h" 

bool initiald_server_locate(InitialDPacketIn* packet)
{
	sockaddr_in inaddr = packet->sender;
	ServerLocate* locate_in = (ServerLocate*)(&packet->packet + sizeof(PacketHeader));

	ServerLocateResponse response;
	response.uid = get_next_id();

	return initiald_send_packet(INITIALD_SERVER_LOCATE,(void*)&response, sizeof(response),inaddr);
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
