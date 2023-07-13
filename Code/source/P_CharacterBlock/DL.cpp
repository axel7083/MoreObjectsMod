#include "SM64DS_2.h"
#include "Actors/Custom/CharacterBlock.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[CHARACTER_BLOCK_OBJECT_ID] = CHARACTER_BLOCK_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[CHARACTER_BLOCK_ACTOR_ID] = &CharacterBlock::spawnData;
	
	CharacterBlock::modelFile      .Construct("data/custom_obj/character_block/character_block.bmd"ov0);
	CharacterBlock::transModelFile .Construct("data/custom_obj/character_block/character_block_trans.bmd"ov0);
	CharacterBlock::texSeqFile     .Construct("data/custom_obj/character_block/character_block.btp"ov0);
	CharacterBlock::transTexSeqFile.Construct("data/custom_obj/character_block/character_block_trans.btp"ov0);
	CharacterBlock::animFile       .Construct("data/custom_obj/character_block/character_block_trans.bca"ov0);
	CharacterBlock::clsnFile       .Construct("data/custom_obj/character_block/character_block.kcl"ov0);
	CharacterBlock::starsParticleFiles[CharacterBlock::BT_MARIO].Construct("data/custom_obj/character_block/stars_mario.spd"ov0);
	CharacterBlock::starsParticleFiles[CharacterBlock::BT_LUIGI].Construct("data/custom_obj/character_block/stars_luigi.spd"ov0);
	CharacterBlock::starsParticleFiles[CharacterBlock::BT_WARIO].Construct("data/custom_obj/character_block/stars_wario.spd"ov0);
	CharacterBlock::starsParticleFiles[CharacterBlock::BT_YOSHI].Construct("data/custom_obj/character_block/stars_yoshi.spd"ov0);
	CharacterBlock::dustParticleFiles[0][CharacterBlock::BT_MARIO].Construct("data/custom_obj/character_block/dust_1_mario.spd"ov0);
	CharacterBlock::dustParticleFiles[0][CharacterBlock::BT_LUIGI].Construct("data/custom_obj/character_block/dust_1_luigi.spd"ov0);
	CharacterBlock::dustParticleFiles[0][CharacterBlock::BT_WARIO].Construct("data/custom_obj/character_block/dust_1_wario.spd"ov0);
	CharacterBlock::dustParticleFiles[0][CharacterBlock::BT_YOSHI].Construct("data/custom_obj/character_block/dust_1_yoshi.spd"ov0);
	CharacterBlock::dustParticleFiles[1][CharacterBlock::BT_MARIO].Construct("data/custom_obj/character_block/dust_2_mario.spd"ov0);
	CharacterBlock::dustParticleFiles[1][CharacterBlock::BT_LUIGI].Construct("data/custom_obj/character_block/dust_2_luigi.spd"ov0);
	CharacterBlock::dustParticleFiles[1][CharacterBlock::BT_WARIO].Construct("data/custom_obj/character_block/dust_2_wario.spd"ov0);
	CharacterBlock::dustParticleFiles[1][CharacterBlock::BT_YOSHI].Construct("data/custom_obj/character_block/dust_2_yoshi.spd"ov0);
}

void cleanup()
{
	
}