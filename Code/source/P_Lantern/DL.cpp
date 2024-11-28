#include "SM64DS_2.h"
#include "Actors/Custom/Lantern.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[LANTERN_OBJECT_ID] = LANTERN_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[LANTERN_ACTOR_ID] = &Lantern::spawnData;

    Lantern::modelFile .Construct("data/custom_obj/lantern/lantern.bmd"ov0);
    Lantern::clsnFile  .Construct("data/custom_obj/lantern/lantern.kcl"ov0);
}

void cleanup()
{
	
}