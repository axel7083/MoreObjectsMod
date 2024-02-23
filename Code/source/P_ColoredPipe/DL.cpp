#include "SM64DS_2.h"
#include "Actors/Custom/ColoredPipe.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[COLORED_PIPE_OBJECT_ID] = COLORED_PIPE_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[COLORED_PIPE_ACTOR_ID] = &ColoredPipe::spawnData;

	ColoredPipe::modelFile.Construct("data/custom_obj/colored_pipe/obj_dokan.bmd"ov0);
	ColoredPipe::clsnFile .Construct("data/custom_obj/colored_pipe/obj_dokan.kcl"ov0);
}

void cleanup()
{
	
}