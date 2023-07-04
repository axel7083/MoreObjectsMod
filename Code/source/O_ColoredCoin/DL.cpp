#include "SM64DS_2.h"
#include "Actors/Custom/ColoredCoin.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[SILVER_COIN_OBJECT_ID]  = SILVER_COIN_ACTOR_ID;
	OBJ_TO_ACTOR_ID_TABLE[COLORED_COIN_OBJECT_ID] = COLORED_COIN_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[SILVER_COIN_ACTOR_ID]  = &ColoredCoin::spawnData;
	ACTOR_SPAWN_TABLE[COLORED_COIN_ACTOR_ID] = &ColoredCoin::spawnData;
	
	ColoredCoin::modelFile.Construct("data/custom_obj/colored_coin/colored_coin.bmd"ov0);
}

void cleanup()
{
	
}