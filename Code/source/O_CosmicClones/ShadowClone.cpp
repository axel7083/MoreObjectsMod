// Author: SplattyDS
#include "SM64DS_2.h"
#include "../../include/Actors/Custom/ShadowClone.h"

asm("marioBodyModelFile = 0x0210ec50");
asm("marioHeadModelFile = 0x0210e690");
extern "C" SharedFilePtr marioBodyModelFile;
extern "C" SharedFilePtr marioHeadModelFile;

asm("luigiBodyModelFile = 0x0210ebb8");
asm("luigiHeadModelFile = 0x0210eb20");
extern "C" SharedFilePtr luigiBodyModelFile;
extern "C" SharedFilePtr luigiHeadModelFile;

asm("warioBodyModelFile = 0x0210ec60");
asm("warioHeadModelFile = 0x0210e758");
extern "C" SharedFilePtr warioBodyModelFile;
extern "C" SharedFilePtr warioHeadModelFile;

asm("FUN_020e6b3c = 0x020e6b3c");
extern "C" void FUN_020e6b3c(Model& model);

asm("G3X_SetToonTable = 0x020555a4");
extern "C" void G3X_SetToonTable(u16* toonTable);

asm("GetShadowVars = 0x020e4374");
extern "C" void GetShadowVars(Player& player, Fix12i& shadowDepth, Fix12i& shadowRad);

namespace
{
	u16 TOON_COLORS[] =
	{
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x480a, 0x480a, 0x480a, 0x5c0d, 0x5c0d, 0x7c13, 0x7c13,
	};
	
	constexpr SharedFilePtr* BODY_MODEL_FILES[3] =
	{
		&marioBodyModelFile,
		&luigiBodyModelFile,
		&warioBodyModelFile,
	};
	
	constexpr SharedFilePtr* HEAD_MODEL_FILES[3] =
	{
		&marioHeadModelFile,
		&luigiHeadModelFile,
		&warioHeadModelFile,
	};
}



SpawnInfo ShadowClone::spawnData =
{
	[]() -> ActorBase* { return new ShadowClone; },
	0x00c0, // right after Player
	0x0013, // ^
	0,
	0._f,
	10000._f,
	10000._f,
	0._f,
};

s32 ShadowClone::InitResources()
{
	Model::LoadFile(*BODY_MODEL_FILES[SAVE_DATA.currentFile]);
	bodyModel.SetFile(BODY_MODEL_FILES[SAVE_DATA.currentFile]->BMD(), 1, -1);
	
	Model::LoadFile(*HEAD_MODEL_FILES[SAVE_DATA.currentFile]);
	headModel.SetFile(HEAD_MODEL_FILES[SAVE_DATA.currentFile]->BMD(), 1, -1);
	
	for (s32 i = 0; i < NUM_CLONES; i++)
	{
		cylClsns[i].Init(pos, 40._f, 150._f, CylinderClsn::ENEMY | CylinderClsn::INTANGIBLE, 0);
		shadows[i].InitCylinder();
	}
	
	bodyModel.SetPolygonMode(2);
	headModel.SetPolygonMode(2);
	
	FUN_020e6b3c(bodyModel);
	FUN_020e6b3c(headModel);
	
	G3X_SetToonTable(&TOON_COLORS[0]);
	
	disabledTimer = 120;
	areaID = -1;
    // Get the eventID
    eventID = param1 & 0xff;
	return 1;
}

s32 ShadowClone::CleanupResources()
{
	BODY_MODEL_FILES[SAVE_DATA.currentFile]->Release();
	HEAD_MODEL_FILES[SAVE_DATA.currentFile]->Release();
	
	for (s32 i = 0; i < TOTAL_FRAMES; i++)
	{
		ShadowCloneFrame& frame = frames[i];
		
		if (frame.loaded)
			GetPlayerAnimation(frame.animID).Release();
	}
	
	return 1;
}

s32 ShadowClone::Behavior()
{
    if(eventID < NUM_EVENTS && Event::GetBit(eventID)) {
        cout << "Suppose to be off" << "\n";
        return 1;
    }

	ReplaceLastFrame();
	
	if (++lastFrame >= TOTAL_FRAMES)
		lastFrame = 0;
	
	if (disabledTimer != 0)
	{
		s32 frameID = lastFrame - (NUM_CLONES - 2) * FRAMES_BETWEEN_CLONES;
		if (frameID < 0)
			frameID += TOTAL_FRAMES;
		
		if (--disabledTimer == 0)
		{
			activationFrame = frameID + 1; // next frame
			if (frameID == TOTAL_FRAMES)
				activationFrame = 0;
		}
		
		pos = frames[frameID].pos;
		
		spawnParticleID = Particle::System::New(spawnParticleID, 0x141, pos.x, pos.y + 75._f, pos.z, nullptr, nullptr);
		
		return 1;
	}
	
	for (s32 i = 0; i < NUM_CLONES; i++)
	{
		s32 frameID = lastFrame - i * FRAMES_BETWEEN_CLONES;
		if (frameID < 0)
			frameID += TOTAL_FRAMES;
		
		if (!active[i] && frameID != activationFrame)
			continue;
		
		ShadowCloneFrame& frame = frames[frameID];
		Vector3& cylClsnPos = cylClsns[i].pos;
		
		if (frame.loaded)
		{
			cylClsnPos = frame.pos;
			
			CheckClsnWithPlayer(cylClsns[i]);
			
			cylClsns[i].Clear();
			cylClsns[i].Update();
		}
		
		if (!active[i])
		{
			// just spawned
			Particle::System::NewSimple(0x142, cylClsnPos.x, cylClsnPos.y + 75._f, cylClsnPos.z);
			active[i] = true;
		}
		
		DropShadow(frame, i);
		moveParticleIDs[i] = Particle::System::New(moveParticleIDs[i], 0x143, cylClsnPos.x, cylClsnPos.y + 75._f, cylClsnPos.z, nullptr, nullptr);
	}
	
	pos = cylClsns[NUM_CLONES - 1].pos;
	
	G3X_SetToonTable(&TOON_COLORS[0]);
	
	return 1;
}

s32 ShadowClone::Render()
{
    if(eventID < NUM_EVENTS && Event::GetBit(eventID)) {
        return 1;
    }

	for (s32 i = 0; i < NUM_CLONES; i++)
	{
		if (!active[i])
			continue;
		
		s32 frameID = lastFrame - i * FRAMES_BETWEEN_CLONES;
		if (frameID < 0)
			frameID += TOTAL_FRAMES;
		
		ShadowCloneFrame& frame = frames[frameID];
		
		if (frame.loaded)
		{
			Matrix4x3 transformMat = Matrix4x3::Translation(frame.transformVec.RotateYAndTranslate(frame.pos, frame.ang.y) >> 3);
			transformMat.RotateY(frame.ang.y);
			transformMat.RotateX(frame.ang.x);
			transformMat.RotateZ(frame.ang.z);
			
			BCA_File* animFile = GetPlayerAnimation(frame.animID).BCA();
			
			bodyModel.SetAnim(animFile, Animation::NO_LOOP, 1._f, (u32)(s32)frame.animFrame);
			bodyModel.currFrame = frame.animFrame;
			bodyModel.mat4x3 = transformMat;
			
			bodyModel.Render();
			
			headModel.mat4x3 = transformMat;
			headModel.data.transforms[0] = bodyModel.data.transforms[15];
			
			headModel.Render();
		}
	}
	
	return 1;
}

void ShadowClone::ReplaceLastFrame()
{
	ShadowCloneFrame& frame = frames[lastFrame];
	Player& player = *PLAYER_ARR[0];
	u32 bodyModelID = player.GetBodyModelID(player.renderedChar, false);
	
	if (frame.loaded)
		GetPlayerAnimation(frame.animID).Release();
	
	frame.animID = player.animID;
	frame.animFrame = player.bodyModels[bodyModelID]->currFrame;
	
	frame.ang = player.ang;
	frame.pos = player.pos;
	frame.transformVec = { 0._f, player.unk690 - player.unk68c, player.unk694 };
	
	// frame.transformMat = player.bodyModels[bodyModelID]->mat4x3;
	
	// 15._f should be -97._f when isBalloonMario but we don't render that
	frame.shadowHeight = player.unk690 + (player.pos.y - player.unk68c) + 15._f;
	GetShadowVars(player, frame.shadowDepth, frame.shadowRad);
	
	frame.loaded = true;
	
	Animation::LoadFile(GetPlayerAnimation(frame.animID));
}

void ShadowClone::CheckClsnWithPlayer(CylinderClsnWithPos& cylClsn)
{
	if (cylClsn.otherObjID == 0)
		return;
	
	Actor* otherObj = Actor::FindWithID(cylClsn.otherObjID);
	
	if (otherObj == nullptr || otherObj->actorID != PLAYER_ACTOR_ID)
		return;
	
	Player& player = *(Player*)otherObj;
	
	if (player.isMetalWario || player.isMega || player.isVanishLuigi || player.isBalloonMario || player.GetTalkState() != Player::TK_NOT || (player.actorInHands != nullptr && player.actorInHands->actorID == BOWSER_TAIL_ACTOR_ID))
		return;
	
	player.Hurt(pos, 2, 15._f, 1, 0, 1);
}

void ShadowClone::DropShadow(ShadowCloneFrame& frame, s32 shadowID)
{
	shadowMats[shadowID] = Matrix4x3::Translation(frame.pos.x >> 3, frame.shadowHeight >> 3, frame.pos.z >> 3);
	DropShadowRadHeight(shadows[shadowID], shadowMats[shadowID], frame.shadowRad, frame.shadowDepth, 15);
}

SharedFilePtr& ShadowClone::GetPlayerAnimation(u32 animID)
{
	return *Player::ANIM_PTRS[animID + SAVE_DATA.currentFile];
}