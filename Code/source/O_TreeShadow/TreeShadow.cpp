#include "SM64DS_2.h"
#include "Actors/Custom/TreeShadow.h"

SpawnInfo TreeShadow::spawnData =
{
	[]() -> ActorBase* { return new TreeShadow; },
	0x0034,
	0x0100,
	Actor::NO_RENDER_IF_OFF_SCREEN,
	0._f,
	1500._f,
	4096._f,
	4096._f
};

void TreeShadow::DropShadow()
{
	shadowMat = Matrix4x3::RotationY(ang.y);
	shadowMat.c3 = pos >> 3;
	
	DropShadowRadHeight(shadow, shadowMat, 336._f, 311._f, 15);
}

s32 TreeShadow::InitResources()
{
	shadow.InitCylinder();
	
	return 1;
}

s32 TreeShadow::CleanupResources()
{
	return 1;
}

s32 TreeShadow::Behavior()
{
	DropShadow();
	
	return 1;
}

s32 TreeShadow::Render()
{
	return 1;
}

TreeShadow::TreeShadow() {}
TreeShadow::~TreeShadow() {}