#include "SM64DS_2.h"
#include "Actors/Custom/PeachNPC.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[PEACH_NPC_OBJECT_ID] = PEACH_NPC_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[PEACH_NPC_ACTOR_ID] = &PeachNPC::spawnData;

    PeachNPC::modelFile.Construct("data/custom_obj/peach_npc/peach_low.bmd"ov0);
    PeachNPC::animFile .Construct("data/custom_obj/peach_npc/peach_wait.bca"ov0);
}

void cleanup()
{
	
}