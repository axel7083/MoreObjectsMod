#include "SM64DS_2.h"
#include "Actors/Custom/ToxicLevel.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[TOXIC_LEVEL_OBJECT_ID] = TOXIC_LEVEL_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[TOXIC_LEVEL_ACTOR_ID] = &ToxicLevel::spawnData;
}

void cleanup()
{
	
}