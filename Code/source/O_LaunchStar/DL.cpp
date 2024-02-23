#include "SM64DS_2.h"
#include "Actors/Custom/LaunchStar.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[LAUNCH_STAR_OBJECT_ID] = LAUNCH_STAR_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[LAUNCH_STAR_ACTOR_ID] = &LaunchStar::spawnData;
	
	LaunchStar::modelFile.Construct("data/custom_obj/launch_star/launch_star.bmd"ov0);
	LaunchStar::animFiles[LaunchStar::WAIT]  .Construct("data/custom_obj/launch_star/launch_star_wait.bca"ov0);
	LaunchStar::animFiles[LaunchStar::LAUNCH].Construct("data/custom_obj/launch_star/launch_star_launch.bca"ov0);
}

void cleanup()
{
	
}