#include "SM64DS_2.h"
#include "Actors/Custom/SaveBlock.h"

namespace
{
	using clpsBlock = StaticCLPS_Block<
		{ .camBehavID = CLPS::CA_NORMAL, .camGoesThru = true, }
	>;
	
	constexpr Fix12i JIGGLE_UP_VEL      = 9.375_f;
	constexpr Fix12i JIGGLE_DOWN_VEL    = -6.25_f;
	constexpr Fix12i MAX_JIGGLE_UP_DIST = 32._f;
}

SpawnInfo SaveBlock::spawnData =
{
	[]() -> ActorBase* { return new SaveBlock; },
	0x0034,
	0x0100,
	Actor::NO_RENDER_IF_OFF_SCREEN,
	0._f,
	1500._f,
	4096._f,
	4096._f
};

SharedFilePtr SaveBlock::modelFile;
SharedFilePtr SaveBlock::texSeqFile;
SharedFilePtr SaveBlock::clsnFile;

int SaveBlock::InitResources()
{
	Model::LoadFile(modelFile);
	model.SetFile(modelFile.BMD(), 1, -1);
	
	TextureSequence::LoadFile(texSeqFile);
	TextureSequence::Prepare(*modelFile.BMD(), *texSeqFile.BTP());
	texSeq.SetFile(*texSeqFile.BTP(), Animation::LOOP, 16._f, 1);
	
	MovingMeshCollider::LoadFile(clsnFile);
	clsn.SetFile(clsnFile.KCL(), clsnNextMat, 0.1_f, ang.y, clpsBlock::instance<>);
	
	shadow.InitCuboid();
	
	UpdateModelPosAndRotY();
	UpdateClsnPosAndRot();
	
	jiggleState = JS_NO_JIGGLE;
	originalPos = pos;
	
	return 1;
}

int SaveBlock::CleanupResources()
{
	clsn.DisableIfEnabled();
	
	modelFile.Release();
	texSeqFile.Release();
	clsnFile.Release();
	
	return 1;
}

int SaveBlock::Behavior()
{
	if (IsClsnInRange(0._f, 0._f))
	{
		Jiggle();
		UpdateClsnPosAndRot();
	}
	
	UpdateModelPosAndRotY();
	DropShadow();
	
	texSeq.Advance();
	
	return 1;
}

int SaveBlock::Render()
{
	texSeq.Update(model.data);
	
	model.Render();
	
	return 1;
}

void SaveBlock::OnHitFromUnderneath(Actor& attacker)
{
	if (attacker.actorID != PLAYER_ACTOR_ID)
		return;
	
	Sound::Play("NCS_SE_SYS_QUIT_WITH_SAVE"sfx, camSpacePos);
	
	SaveData::SaveCurrentFile();
	
	Particle::System::NewSimple(0x9, pos.x, pos.y, pos.z);
	Particle::System::NewSimple(0x9, pos.x, pos.y, pos.z);
	
	jiggleState = JS_JIGGLE_UP;
}

void SaveBlock::DropShadow()
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
	shadowMat.c3.y = (pos.y - 32._f) >> 3;
	shadowMat.c3.z = pos.z >> 3;
	
	DropShadowScaleXYZ(shadow, shadowMat, shadowScale, shadowDepth + 20._f, shadowScale, 15);
}

Fix12i SaveBlock::GetFloorPosY()
{
	Vector3 raycastPos = { pos.x, pos.y + 40._f, pos.z };
	RaycastGround raycaster;
	
	raycaster.SetObjAndPos(raycastPos, nullptr);
	raycaster.unk4c = 1000._f;
	
	return raycaster.DetectClsn() ? raycaster.clsnPosY : pos.y;
}

void SaveBlock::Jiggle()
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

SaveBlock::SaveBlock() {}
SaveBlock::~SaveBlock() {}