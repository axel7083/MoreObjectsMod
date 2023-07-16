#include "SM64DS_2.h"
#include "Actors/Custom/DoorBlocker.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[DOOR_BLOCKER_OBJECT_ID] = DOOR_BLOCKER_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[DOOR_BLOCKER_ACTOR_ID] = &DoorBlocker::spawnData;
	
	DoorBlocker::modelFile.Construct("data/custom_obj/door_blocker/door_blocker.bmd"ov0);
	DoorBlocker::clsnFile .Construct("data/custom_obj/door_blocker/door_blocker.kcl"ov0);
}

void cleanup()
{
	
}