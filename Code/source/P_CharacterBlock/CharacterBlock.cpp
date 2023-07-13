#include "SM64DS_2.h"
#include "Actors/Custom/CharacterBlock.h"

namespace
{
	using clpsBlock = StaticCLPS_Block<
		{ .camBehavID = CLPS::CA_NORMAL, .camGoesThru = true, }
	>;
	
	constexpr Fix12i JIGGLE_UP_VEL      = 9.375_f;
	constexpr Fix12i JIGGLE_DOWN_VEL    = -6.25_f;
	constexpr Fix12i MAX_JIGGLE_UP_DIST = 32._f;
	
	constexpr SoundIDs CHAR_CHANGE_SOUNDS[Player::NUM_CHARACTERS] =
	{
		"NCS_SE_VO_CALL_NAME"sfx,
		"NCS_SE_VO_CALL_NAME_L"sfx,
		"NCS_SE_VO_CALL_NAME_W"sfx,
		"NCS_SE_VO_CALL_NAME_Y"sfx,
	};
}

SharedFilePtr CharacterBlock::modelFile;
SharedFilePtr CharacterBlock::transModelFile;
SharedFilePtr CharacterBlock::texSeqFile;
SharedFilePtr CharacterBlock::transTexSeqFile;
SharedFilePtr CharacterBlock::animFile;
SharedFilePtr CharacterBlock::clsnFile;
SharedFilePtr CharacterBlock::starsParticleFiles[NUM_BLOCK_TYPES];
SharedFilePtr CharacterBlock::dustParticleFiles[2][NUM_BLOCK_TYPES];
	
Particle::SysDef CharacterBlock::starsSysDefs[NUM_BLOCK_TYPES];
Particle::SysDef CharacterBlock::dustSysDefs[2][NUM_BLOCK_TYPES];

SpawnInfo CharacterBlock::spawnData =
{
	[]() -> ActorBase* { return new CharacterBlock; },
	0x0034,
	0x0100,
	Actor::NO_RENDER_IF_OFF_SCREEN,
	0._f,
	1500._f,
	4096._f,
	4096._f,
};

s32 CharacterBlock::InitResources()
{
	blockType = param1 & 0xf;
	needsUnlock = (param1 >> 4 & 0xf) != 1;
	startingCharacter = param1 >> 8 & 0xf; // this was changed from (1 2 3 0) to (0 1 2 3)
	healPlayer = (param1 >> 12 & 0xf) == 1;
	
	Model::LoadFile(modelFile);
	model.SetFile(modelFile.BMD(), 1, -1);
	
	Model::LoadFile(transModelFile);
	transModel.SetFile(transModelFile.BMD(), 1, -1);
	
	TextureSequence::LoadFile(texSeqFile);
	TextureSequence::Prepare(*modelFile.BMD(), *texSeqFile.BTP());
	texSeq.SetFile(*texSeqFile.BTP(), Animation::NO_LOOP, 1._f, blockType);
	
	TextureSequence::LoadFile(transTexSeqFile);
	TextureSequence::Prepare(*transModelFile.BMD(), *transTexSeqFile.BTP());
	transTexSeq.SetFile(*transTexSeqFile.BTP(), Animation::NO_LOOP, 1._f, blockType);
	
	Animation::LoadFile(animFile);
	transModel.SetAnim(animFile.BCA(), Animation::LOOP, 1._f, 0);
	
	MovingMeshCollider::LoadFile(clsnFile);
	clsn.SetFile(clsnFile.KCL(), clsnNextMat, 0.1_f, ang.y, clpsBlock::instance<>);
	
	starsSysDefs[blockType].LoadAndSetFile(starsParticleFiles[blockType]);
	
	for (s32 i = 0; i < NUM_DUST_PARTICLES; i++)
		dustSysDefs[i][blockType].LoadAndSetFile(dustParticleFiles[i][blockType]);
	
	shadow.InitCuboid();
	
	UpdateModelTransform();
	isUnlocked = CheckUnlock();
	
	if (isUnlocked)
		UpdateClsnPosAndRot();
	
	originalPos = pos;
	jiggleState = JS_NO_JIGGLE;
	
	return 1;
}

s32 CharacterBlock::CleanupResources()
{
	clsn.DisableIfEnabled();
	
	modelFile.Release();
	transModelFile.Release();
	
	texSeqFile.Release();
	transTexSeqFile.Release();
	
	animFile.Release();
	clsnFile.Release();
	
	starsSysDefs[blockType].Release(starsParticleFiles[blockType]);
	
	for (s32 i = 0; i < NUM_DUST_PARTICLES; i++)
		dustSysDefs[i][blockType].Release(dustParticleFiles[i][blockType]);
	
	return 1;
}

s32 CharacterBlock::Behavior()
{
	isUnlocked = CheckUnlock();
	
	if (!isUnlocked)
	{
		clsn.DisableIfEnabled();
		
		transModel.Advance();
		
		UpdateModelTransform();
		DropShadow();
		
		return 1;
	}
	
	Jiggle();
	
	if (IsClsnInRange(0._f, 0._f))
		UpdateClsnPosAndRot();
	
	UpdateModelTransform();
	DropShadow();
	
	return 1;
}

s32 CharacterBlock::Render()
{
	if (isUnlocked)
	{
		texSeq.Update(model.data);
		
		model.Render();
	}
	else
	{
		transTexSeq.Update(transModel.data);
		
		transModel.Render();
	}
	
	return 1;
}

void CharacterBlock::OnHitFromUnderneath(Actor& attacker)
{
	if (SAVE_DATA.currentCharacter == blockType || attacker.actorID != PLAYER_ACTOR_ID)
		return;
	
	Player& player = (Player&)attacker;
	
	s16 prevHealth = HEALTH_ARR[player.playerID];
	
	player.ChangeState(*(Player::State*)Player::ST_FALL);
	player.SetRealCharacter(blockType);
	
	SAVE_DATA.currentCharacter = blockType;
	
	if (!healPlayer)
		HEALTH_ARR[player.playerID] = prevHealth;
	
	Sound::Play(CHAR_CHANGE_SOUNDS[blockType], camSpacePos);
	
	Particle::System::NewSimple(starsSysDefs[blockType], pos.x, pos.y, pos.z);
	
	for (s32 i = 0; i < NUM_DUST_PARTICLES; i++)
		Particle::System::NewSimple(dustSysDefs[i][blockType], player.pos.x, player.pos.y, player.pos.z);
	
	jiggleState = JS_JIGGLE_UP;
}

void CharacterBlock::UpdateModelTransform()
{
	model.mat4x3 = Matrix4x3::RotationY(ang.y);
	model.mat4x3.c3 = pos >> 3;
	
	if (needsUnlock)
		transModel.mat4x3 = model.mat4x3;
}

void CharacterBlock::DropShadow()
{
	if (!(flags & Actor::OFF_SCREEN) || CURRENT_GAMEMODE == 1)
		floorPosY = GetFloorPosY();
	
	Fix12i shadowDepth = pos.y - floorPosY;
	
	if (shadowDepth < 1._f)
		shadowDepth = 1._f;
	
	Fix12i shadowScale = 180._f - (shadowDepth * 0.09375_f);
	Fix12i range = shadowDepth + 532._f;
	
	if (shadowScale < 10._f)
		shadowScale = 10._f;
	
	rangeOffsetY = -((shadowDepth + 20._f) / 2);
	rangeAsr3 = (range / 2) >> 3;
	
	// shadowScale *= scale.x;
	
	shadowMat = Matrix4x3::RotationY(ang.y);
	shadowMat.c3.x = pos.x >> 3;
	shadowMat.c3.y = (pos.y - 0x20000_f) >> 3;
	shadowMat.c3.z = pos.z >> 3;
	
	DropShadowScaleXYZ(shadow, shadowMat, shadowScale, shadowDepth + 20._f, shadowScale, 15);
}

Fix12i CharacterBlock::GetFloorPosY()
{
	Vector3 raycastPos = { pos.x, pos.y + 40._f, pos.z };
	RaycastGround raycaster;
	
	raycaster.SetObjAndPos(raycastPos, nullptr);
	raycaster.unk4c = 1000._f;
	
	return raycaster.DetectClsn() ? raycaster.clsnPosY : pos.y;
}

void CharacterBlock::Jiggle()
{
	switch (jiggleState)
	{
		case JS_NO_JIGGLE:
		{
			break;
		}
		case JS_JIGGLE_UP:
		{
			pos.y += JIGGLE_UP_VEL;
			
			if (pos.y > originalPos.y + MAX_JIGGLE_UP_DIST)
				jiggleState = JS_JIGGLE_DOWN;
			
			break;
		}
		case JS_JIGGLE_DOWN:
		{
			pos.y += JIGGLE_DOWN_VEL;
			
			if (pos.y < originalPos.y)
				jiggleState = JS_JIGGLE_STOP;
			
			break;
		}
		case JS_JIGGLE_STOP:
		{
			pos.y = originalPos.y;
			jiggleState = JS_NO_JIGGLE;
			
			break;
		}
	}
}

bool CharacterBlock::CheckUnlock()
{
	if (needsUnlock && blockType != startingCharacter)
	{
		if (blockType == 3 && (SAVE_DATA.characterState & 1 << startingCharacter) == 0)
			return false;
		
		else if ((SAVE_DATA.characterState & 1 << blockType) == 0)
			return false;
	}
	
	return true;
}

CharacterBlock::CharacterBlock() {}
CharacterBlock::~CharacterBlock() {}