#include "SM64DS_2.h"
#include "Actors/Custom/InvisibleWall.h"

namespace
{
	using clpsBlock = StaticCLPS_Block<
		{  }
	>;
}

SpawnInfo InvisibleWall::spawnData =
{
	[]() -> ActorBase* { return new InvisibleWall; },
	0x0000,
	0x0100,
	Actor::NO_BEHAVIOR_IF_OFF_SCREEN | Actor::NO_RENDER_IF_OFF_SCREEN,
	100._f,
	200._f,
	4096._f,
	4096._f,
};

SharedFilePtr InvisibleWall::clsnFile;

s32 InvisibleWall::InitResources()
{
	MovingMeshCollider::LoadFile(clsnFile);
	clsn.SetFile(clsnFile.KCL(), clsnNextMat, 1._f, ang.y, clpsBlock::instance<>);	
	clsn.beforeClsnCallback = &MeshColliderBase::UpdatePosWithTransform;
	
	UpdateClsnPosAndRot();
	
	return 1;
}

s32 InvisibleWall::CleanupResources()
{
	clsn.DisableIfEnabled();
	
	clsnFile.Release();
	
	return 1;
}

s32 InvisibleWall::Behavior()
{
	IsClsnInRange(0._f, 0._f);
	
	return 1;
}

s32 InvisibleWall::Render()
{
	return 1;
}

InvisibleWall::InvisibleWall() {}
InvisibleWall::~InvisibleWall() {}
