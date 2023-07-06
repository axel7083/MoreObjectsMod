#include "SM64DS_2.h"
#include "Actors/Custom/RideableYoshi.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[RIDEABLE_YOSHI_OBJECT_ID] = RIDEABLE_YOSHI_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[RIDEABLE_YOSHI_ACTOR_ID] = &RideableYoshi::spawnData;
	
	RideableYoshi::ridingAnimFile.Construct("data/player/ride_yoshi.bca"ov0);
}

void cleanup()
{
	
}