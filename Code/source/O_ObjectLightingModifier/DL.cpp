#include "SM64DS_2.h"
#include "Actors/Custom/ObjectLightingModifier.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[OBJECT_LIGHTING_MODIFIER_OBJECT_ID] = OBJECT_LIGHTING_MODIFIER_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[OBJECT_LIGHTING_MODIFIER_ACTOR_ID] = &ObjectLightingModifier::spawnData;
}

void cleanup()
{
	
}