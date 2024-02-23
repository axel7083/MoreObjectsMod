#include "SM64DS_2.h"
#include "Actors/Custom/GalaxyShrinkPlatform.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[GALAXY_SHRINK_PLATFORM_OBJECT_ID] = GALAXY_SHRINK_PLATFORM_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[GALAXY_SHRINK_PLATFORM_ACTOR_ID] = &GalaxyShrinkPlatform::spawnData;
	
	GalaxyShrinkPlatform::modelFile     .Construct("data/custom_obj/galaxy_shrink_platform/galaxy_shrink_platform.bmd"ov0);
	GalaxyShrinkPlatform::frameModelFile.Construct("data/custom_obj/galaxy_shrink_platform/galaxy_shrink_platform_frame.bmd"ov0);
	
	GalaxyShrinkPlatform::clsnFile.Construct("data/custom_obj/galaxy_shrink_platform/galaxy_shrink_platform.kcl"ov0);
}

void cleanup()
{
	
}