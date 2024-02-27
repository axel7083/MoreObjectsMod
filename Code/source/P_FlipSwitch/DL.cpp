#include "SM64DS_2.h"
#include "Actors/Custom/FlipSwitch.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[FLIP_SWITCH_OBJECT_ID] = FLIP_SWITCH_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[FLIP_SWITCH_ACTOR_ID] = &FlipSwitch::spawnData;

    FlipSwitch::modelFile .Construct("data/custom_obj/flip_switch/flip_switch_platform.bmd"ov0);
    FlipSwitch::clsnFile.Construct("data/custom_obj/galaxy_shrink_platform/galaxy_shrink_platform.kcl"ov0);
}

void cleanup()
{
	
}