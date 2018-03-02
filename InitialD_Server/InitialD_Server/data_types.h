//SERVER!!!

#pragma once
#define ACTION_DIE 1
#define ACTION_SPAWN 2
#define ACTION_POSITION 3
#define ACTION_DAMAGE 4
#define ACTION_PROJECTILE_SPAWN 5
#define ACTION_PROJECTILE_DESPAWN 6
#define ACTION_PROJECTILE_POSITION 7
#define ACTION_PROJECTILE_DAMAGE 8




#define TYPE_PLAYER 0
#define TYPE_ACTION 1
#define TYPE_PROJECTILE 2




#define MAX_PACKET_LENGTH 1024
#define INBUFF_COUNT 48
#define DOGGIE_COUNT 1

//Make these bitable able for speed. 


//UDP 

//PlayerStats



typedef struct PacketHeader {
	char protocol[9] = "InitialD";
	char vers1[7] = "0.0.1a";
	unsigned short action;
	unsigned short data_len;
};

typedef struct LocateServer {
	char name[32];
}LocateServer;


typedef struct InitialDPacket {
	char data[MAX_PACKET_LENGTH];
};
//24 bytes
typedef struct {
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




