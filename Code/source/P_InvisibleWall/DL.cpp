#include "SM64DS_2.h"
#include "Actors/Custom/InvisibleWall.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[INVISIBLE_WALL_OBJECT_ID]  = INVISIBLE_WALL_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[INVISIBLE_WALL_ACTOR_ID]  = &InvisibleWall::spawnData;
	
	InvisibleWall::clsnFile.Construct("data/custom_obj/invisible_wall/invisible_wall.kcl"ov0);
}

void cleanup()
{
	
}