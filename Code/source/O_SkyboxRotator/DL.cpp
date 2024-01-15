#include "SM64DS_2.h"
#include "Actors/Custom/SkyboxRotator.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[SKYBOX_ROTATOR_OBJECT_ID] = SKYBOX_ROTATOR_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[SKYBOX_ROTATOR_ACTOR_ID] = &SkyboxRotator::spawnData;
}

void cleanup()
{
	
}