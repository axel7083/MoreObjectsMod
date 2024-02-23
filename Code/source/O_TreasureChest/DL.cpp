#include "SM64DS_2.h"
#include "Actors/Custom/TreasureChest.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[TREASURE_CHEST_DL_OBJECT_ID] = TREASURE_CHEST_ACTOR_ID;
	ACTOR_SPAWN_TABLE[TREASURE_CHEST_DL_ACTOR_ID] = &TreasureChest::spawnData;

    TreasureChest::modelFile .Construct("data/custom_obj/treasure_chest/t_box.bmd"ov0);
    TreasureChest::animFiles[TreasureChest::OPEN]  .Construct("data/custom_obj/treasure_chest/t_box_open.bca"ov0);
}

void cleanup()
{
	
}