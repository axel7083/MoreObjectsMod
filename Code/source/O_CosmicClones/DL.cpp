// Author: SplattyDS
#include "SM64DS_2.h"
#include "../../include/Actors/Custom/ShadowClone.h"

void init()
{
    OBJ_TO_ACTOR_ID_TABLE[SHADOW_CLONE_OBJECT_ID] = SHADOW_CLONE_ACTOR_ID;
    ACTOR_SPAWN_TABLE[SHADOW_CLONE_ACTOR_ID] = &ShadowClone::spawnData;
}

void cleanup()
{

}