#pragma once



bool init_server_sockets();
bool init_in_socket();
bool init_out_socket();
bool init_broadcast_socket();
void cleanup_exit();
void get_datagram(_Out_ InitialDPacketIn* pack_in);
int broadcast(char* data, int data_len);
int send_out(char* buff, int bufflen, struct sockaddr* out_addr, int out_datalen);
int initiald_send_packet(int action, void* action_struct, int struct_len, sockaddr_in inaddr);