#include "SM64DS_2.h"
#include "Actors/Custom/BooMario.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[BOO_MARIO_OBJECT_ID] = BOO_MARIO_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[BOO_MARIO_ACTOR_ID] = &BooMario::spawnData;

    BooMario::modelFile.Construct("data/enemy/teresa/teresa.bmd"ov0);
    BooMario::animFile .Construct("data/enemy/teresa/teresa_wait.bca"ov0);
}

void cleanup()
{
	
}