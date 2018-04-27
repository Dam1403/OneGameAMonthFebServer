//SERVER!!!

#pragma once
#define INITIALD_NO_OPERATION 0


#define INITIALD_ACTION_DIE 1
#define DACTION_SPAWN 2
#define DACTION_POSITION 3
#define DACTION_DAMAGE 4
#define DACTION_PROJECTILE_SPAWN 5
#define DACTION_PROJECTILE_DESPAWN 6
#define DACTION_PROJECTILE_POSITION 7
#define DACTION_PROJECTILE_DAMAGE 8


#define INITIALD_SERVER_LOCATE 16
#define INITIALD_SERVER_JOIN 17
#define INITIALD_SERVER_LEAVE 18
#define INITIALD_SERVER_LIST 19
#define INITIALD_SERVER_MEMBER_INFO 20



#define TYPE_PLAYER 0
#define TYPE_ACTION 1
#define TYPE_PROJECTILE 2




#define MAX_PACKET_LENGTH 1024
#define INBUFF_COUNT 48


#define INITIALD_MAX_SERVER_NAME_LENGTH 32
#define INITIALD_MAX_USER_NAME_LENGTH 32

//UDP 

//PlayerStats


typedef struct PacketHeader {
	char protocol[9] = "InitialD";
	char vers1[7] = "0.0.1a";
	unsigned short action;
	unsigned short data_len;
}PacketHeader;



typedef struct ServerLocate {
	char name[INITIALD_MAX_USER_NAME_LENGTH];
}LocateServer;

typedef struct ServerLocateResponse
{
	char server_name[INITIALD_MAX_SERVER_NAME_LENGTH];
} ServerLocateResponse;


//HANDLE DUPLICATES
typedef struct ServerJoin {
	char username[INITIALD_MAX_USER_NAME_LENGTH];
}ServerJoin;

typedef struct ServerJoinResponse
{
	unsigned long uid;
}ServerJoinResponse;

typedef struct ServerList
{
	unsigned long uid;
};
typedef struct ServerListResponse
{
	char names[MAX_PACKET_LENGTH/INITIALD_MAX_USER_NAME_LENGTH][INITIALD_MAX_USER_NAME_LENGTH];
	//NEXT????
};


typedef struct InitialDPacket {
	char data[MAX_PACKET_LENGTH];
} InitialDPacket;


typedef struct InitialDPacketIn
{
	InitialDPacket packet;
	sockaddr_in sender;
}InitialDPacketIn;
//24 bytes
typedef struct SixDof{
	float pos_x;
	float pos_y;
	float pos_z;
	float rot_x;
	float rot_y;
	float rot_z;
} SixDof;

typedef struct{
	int type;
	SixDof location;
	int health;
	int weapon_heat;
	int defense_power;
} Player;

struct Action {
	int type;
	int player_id;
	int action_type;//because enum size is suggested to be an int, but some comps may optomize to smaller values
	int action_args_length;// check max length to prevent overflow
	unsigned char action_args[256];
	
};

typedef struct{
	int type;
	int id;
	SixDof location;
} Projectile;






