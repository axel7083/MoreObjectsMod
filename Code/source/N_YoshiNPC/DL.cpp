#include "SM64DS_2.h"
#include "Actors/Custom/YoshiNPC.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[YOSHI_NPC_OBJECT_ID] = YOSHI_NPC_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[YOSHI_NPC_ACTOR_ID] = &YoshiNPC::spawnData;
	
	YoshiNPC::modelFile.Construct("data/custom_obj/yoshi_npc/yoshi_npc.bmd"ov0);
	YoshiNPC::animFile .Construct("data/custom_obj/yoshi_npc/yoshi_npc_wait.bca"ov0);
}

void cleanup()
{
	
}