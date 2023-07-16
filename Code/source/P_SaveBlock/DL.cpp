#include "SM64DS_2.h"
#include "Actors/Custom/SaveBlock.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[SAVE_BLOCK_OBJECT_ID] = SAVE_BLOCK_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[SAVE_BLOCK_ACTOR_ID] = &SaveBlock::spawnData;
	
	SaveBlock::modelFile .Construct("data/custom_obj/save_block/save_block.bmd"ov0);
	SaveBlock::texSeqFile.Construct("data/custom_obj/save_block/save_block.btp"ov0);
	SaveBlock::clsnFile  .Construct("data/custom_obj/save_block/save_block.kcl"ov0);
}

void cleanup()
{
	
}