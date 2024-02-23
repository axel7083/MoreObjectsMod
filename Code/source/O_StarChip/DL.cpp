#include "SM64DS_2.h"
#include "Actors/Custom/StarChip.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[STAR_CHIP_OBJECT_ID] = STAR_CHIP_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[STAR_CHIP_ACTOR_ID] = &StarChip::spawnData;
	
	StarChip::modelFile.Construct("data/custom_obj/star_chip/star_chip.bmd"ov0);
}

void cleanup()
{
	
}