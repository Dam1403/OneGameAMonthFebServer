#include "stdafx.h" 

char server_name[INITIALD_MAX_SERVER_NAME_LENGTH] = "InitialDServer1";


bool initiald_server_locate(InitialDPacketIn* packet)
{
	sockaddr_in inaddr = packet->sender;
	ServerLocate* locate_in = (ServerLocate*)((&packet->packet.data[0]) + sizeof(PacketHeader));
	strip_newlines(locate_in->name, sizeof(locate_in->name));
	char ipstr[32] = {0};
	printf("Recieved SERVER_LOCATE from: %s at ip_address %s\n", locate_in->name, inet_ntop(AF_INET,&inaddr.sin_addr,ipstr,32));

	ServerLocateResponse response;
	memcpy_s(
		&response.server_name,
		INITIALD_MAX_SERVER_NAME_LENGTH,
		&server_name,
		INITIALD_MAX_SERVER_NAME_LENGTH);

	printf("Responding %s\n", response.server_name);

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
