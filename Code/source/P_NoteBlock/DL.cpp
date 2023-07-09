#include "SM64DS_2.h"
#include "Actors/Custom/NoteBlock.h"

void init()
{
	OBJ_TO_ACTOR_ID_TABLE[NOTE_BLOCK_OBJECT_ID] = NOTE_BLOCK_ACTOR_ID;
	
	ACTOR_SPAWN_TABLE[NOTE_BLOCK_ACTOR_ID] = &NoteBlock::spawnData;
	
	NoteBlock::modelFile.Construct("data/custom_obj/note_block/note_block.bmd"ov0);
	NoteBlock::clsnFile .Construct("data/custom_obj/note_block/note_block.kcl"ov0);
}

void cleanup()
{
	
}