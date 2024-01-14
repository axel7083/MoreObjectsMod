#include "SM64DS_2.h"
#include "Actors/Custom/MegaBlock.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[MEGA_BLOCK_OBJECT_ID] = MEGA_BLOCK_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[MEGA_BLOCK_ACTOR_ID] = &MegaBlock::spawnData;

    MegaBlock::modelFile .Construct("data/custom_obj/mega_block/star_block.bmd"ov0);
    MegaBlock::clsnFile  .Construct("data/custom_obj/mega_block/star_block.kcl"ov0);
}

void cleanup()
{
	
}