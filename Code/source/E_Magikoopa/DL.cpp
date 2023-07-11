#include "SM64DS_2.h"
#include "Actors/Custom/Magikoopa.h"

asm("instructions = 0x02017bf8");
extern "C" u32 instructions[];

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[MAGIKOOPA_OBJECT_ID] = MAGIKOOPA_ACTOR_ID;
	OBJ_TO_ACTOR_ID_TABLE[KAMELLA_OBJECT_ID]   = KAMELLA_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[MAGIKOOPA_ACTOR_ID]      = &Magikoopa::spawnData[Magikoopa::MAGIKOOPA];
	ACTOR_SPAWN_TABLE[KAMELLA_ACTOR_ID]        = &Magikoopa::spawnData[Magikoopa::KAMELLA];
	ACTOR_SPAWN_TABLE[MAGIKOOPA_SHOT_ACTOR_ID] = &MagikoopaShot::spawnData;
	
	Magikoopa::modelFiles[Magikoopa::MAGIKOOPA].Construct("data/custom_obj/magikoopa/magikoopa.bmd"ov0);
	Magikoopa::modelFiles[Magikoopa::KAMELLA]  .Construct("data/custom_obj/magikoopa/kamella.bmd"ov0);
	
	Magikoopa::animFiles[Magikoopa::APPEAR].Construct("data/custom_obj/magikoopa/magikoopa_appear.bca"ov0);
	Magikoopa::animFiles[Magikoopa::WAVE]  .Construct("data/custom_obj/magikoopa/magikoopa_wave.bca"ov0);
	Magikoopa::animFiles[Magikoopa::SHOOT] .Construct("data/custom_obj/magikoopa/magikoopa_shoot.bca"ov0);
	Magikoopa::animFiles[Magikoopa::POOF]  .Construct("data/custom_obj/magikoopa/magikoopa_poof.bca"ov0);
	Magikoopa::animFiles[Magikoopa::WAIT]  .Construct("data/custom_obj/magikoopa/magikoopa_wait.bca"ov0);
	Magikoopa::animFiles[Magikoopa::HURT]  .Construct("data/custom_obj/magikoopa/magikoopa_hurt.bca"ov0);
	Magikoopa::animFiles[Magikoopa::DEFEAT].Construct("data/custom_obj/magikoopa/magikoopa_defeat.bca"ov0);
	
	Magikoopa::particleResFiles[Magikoopa::MAGIKOOPA].Construct("data/custom_obj/magikoopa/magikoopa_magic.spr"ov0);
	Magikoopa::particleResFiles[Magikoopa::KAMELLA]  .Construct("data/custom_obj/magikoopa/kamella_magic.spr"ov0);
	
	// enables the dynamic hook at 0x02017bfc, has no side effects
	instructions[0] = 0xe1a00004;
	instructions[1] = 0xe1a00000; // nop because this is where the dynamic hook will be
	instructions[2] = 0xe5d40002;
	instructions[3] = 0xe35000ff;
	instructions[4] = 0x23a00000;
	instructions[5] = 0x35d40002;
	instructions[6] = 0x32800001;
	instructions[7] = 0x35c40002;
	instructions[8] = 0x35940004;
	instructions[9] = 0xe8bd8010;
	
	CP15::FlushAndInvalidateDataCache((u32)&instructions, 10 * 4);
	CP15::InvalidateInstructionCache((u32)&instructions, 10 * 4);
}

void cleanup()
{
	Particle::Manager::UnloadNewTexs();
}