#include "Actors/Custom/MegaBlock.h"

namespace
{
    using clpsBlock = StaticCLPS_Block<
            {  }
    >;
	
	char blocksRunOver = 0;
}

SharedFilePtr MegaBlock::modelFile;
SharedFilePtr MegaBlock::clsnFile;
	
SpawnInfo MegaBlock::spawnData =
{
    []() -> ActorBase* { return new MegaBlock; },
	0x002e,
	0x00c3,
    Actor::NO_RENDER_IF_OFF_SCREEN,
	0x000c8000_f,
	0x00320000_f,
	0x01000000_f,
	0x01000000_f
};

MegaBlock* MegaBlock::Spawn()
{
	return new MegaBlock;
}

void MegaBlock::UpdateShadowMatrix()
{
    shadowMat = Matrix4x3::RotationY(ang.y);
	model.mat4x3.c3.x = pos.x >> 3;
	model.mat4x3.c3.y = pos.y >> 3;
	model.mat4x3.c3.z = pos.z >> 3;
	//DropShadowRadHeight(shadow, model.mat4x3, 0x150000_f, 0x137000_f, 0xc);
}

int MegaBlock::InitResources()
{
	Model::LoadFile(modelFile);
	model.SetFile(modelFile.BMD(), 1, -1);

	UpdateModelPosAndRotY();
	UpdateClsnPosAndRot();
	UpdateShadowMatrix();
	
	MovingMeshCollider::LoadFile(clsnFile);
	clsn.SetFile(clsnFile.KCL(), clsnNextMat, 0x0D00_f, ang.y, clpsBlock::instance<>);
	
	Model::LoadFile(SILVER_NUMBER_MODEL_PTR);
	TextureSequence::LoadFile(SILVER_NUMBER_TEXSEQ_PTR);
	
	starID = (param1 & 0xf) > 7 ? 7 : (param1 & 0xf);
	
	return 1;
}

int MegaBlock::CleanupResources()
{
	if(clsn.IsEnabled())
		clsn.Disable();
	
	modelFile.Release();
	clsnFile.Release();
	
	SILVER_NUMBER_MODEL_PTR.Release();
	SILVER_NUMBER_TEXSEQ_PTR.Release();
	
	return 1;
}

int MegaBlock::Behavior()
{
	if(UpdateKillByMegaChar(0, 0, 0, 0x100000_f))
		return 1;
	
	clsn.rangeOffsetY = 0xc8000_f;
	clsn.range = 0x320000_f;
	
	//DropShadowScaleXYZ(shadow, shadowMat, 0x190000_f, 0x190000_f, 0x190000_f, 0xf);
	IsClsnInRangeOnScreen(0x600000_f, 0_f);
	
	return 1;
}

int MegaBlock::Render()
{
	model.Render(nullptr);
	return 1;
}

MegaBlock::~MegaBlock() {}

void MegaBlock::OnHitByMegaChar(Player& megaChar)
{
	megaChar.IncMegaKillCount();

    Sound::Play("NCS_SE_SCT_HIT_LARGE"sfx, camSpacePos);
	KillByMegaChar(megaChar);
}

void MegaBlock::Kill()
{
	Vector3 vec = pos;
	vec.y += 0xc8000_f;
	
	Particle::System::NewSimple(0x122, vec.x, vec.y, vec.z);
	Particle::System::NewSimple(0x123, vec.x, vec.y, vec.z);
	DisappearPoofDustAt(vec);

    Sound::Play("NCS_SE_SYS_STAR_APPEAR"sfx, camSpacePos);
    MarkForDestruction();
	
	++blocksRunOver;
	// Actor::Spawn(RED_COIN_NUMBER_ACTOR_ID, 0x10 | blocksRunOver, vec, nullptr, areaID, -1);
	
	if(blocksRunOver == 5)
	{
		Actor::Spawn(POWER_STAR_ACTOR_ID, 0x0040 + starID, vec, nullptr, areaID, -1);
	}
}
