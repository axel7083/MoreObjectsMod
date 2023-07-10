#include "SM64DS_2.h"
#include "Actors/Custom/ShyGuy.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[SHY_GUY_OBJECT_ID] = SHY_GUY_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[SHY_GUY_ACTOR_ID] = &ShyGuy::spawnData;
	
	ShyGuy::modelFile.Construct("data/custom_obj/shy_guy/shy_guy.bmd"ov0);
	ShyGuy::animFiles[ShyGuy::WAIT]  .Construct("data/custom_obj/shy_guy/shy_guy_wait.bca"ov0);
	ShyGuy::animFiles[ShyGuy::WALK]  .Construct("data/custom_obj/shy_guy/shy_guy_walk.bca"ov0);
	ShyGuy::animFiles[ShyGuy::RUN]   .Construct("data/custom_obj/shy_guy/shy_guy_run.bca"ov0);
	ShyGuy::animFiles[ShyGuy::FREEZE].Construct("data/custom_obj/shy_guy/shy_guy_freeze.bca"ov0);
}

void cleanup()
{
	
}