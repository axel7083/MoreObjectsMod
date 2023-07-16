#include "SM64DS_2.h"
#include "Actors/Custom/TreeShadow.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[TREE_SHADOW_OBJECT_ID] = TREE_SHADOW_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[TREE_SHADOW_ACTOR_ID] = &TreeShadow::spawnData;
}

void cleanup()
{
	
}