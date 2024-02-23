#include "SM64DS_2.h"
#include "Actors/Custom/Berry.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[BERRY_OBJECT_ID] = BERRY_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[BERRY_ACTOR_ID] = &Berry::spawnData;
	
	Berry::modelFile    .Construct("data/custom_obj/berry/berry.bmd"ov0);
	Berry::stemModelFile.Construct("data/custom_obj/berry/berry_stem.bmd"ov0);
}

void cleanup()
{
	
}