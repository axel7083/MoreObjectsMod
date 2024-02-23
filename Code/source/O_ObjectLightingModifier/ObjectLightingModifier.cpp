#include "SM64DS_2.h"
#include "Actors/Custom/ObjectLightingModifier.h"

SpawnInfo ObjectLightingModifier::spawnData =
{
	[]() -> ActorBase* { return new ObjectLightingModifier; },
	0x0000,
	0x0100,
	Actor::NO_RENDER_IF_OFF_SCREEN,
	100._f,
	200._f,
	4096._f,
	4096._f,
};

s32 ObjectLightingModifier::InitResources()
{
	GXFIFO::SetLightColor((u8)((param1 & 0xff00) >> 8), (u8)(param1 & 0xff), (u8)((ang.x & 0xff00) >> 8), (u8)(ang.x & 0xff));
	
	return 1;
}

s32 ObjectLightingModifier::CleanupResources()
{
	GXFIFO::SetLightColor(0, 0xff, 0xff, 0xff);
	
	return 1;
}

s32 ObjectLightingModifier::Behavior()
{
	return 1;
}

s32 ObjectLightingModifier::Render()
{
	return 1;
}

ObjectLightingModifier::ObjectLightingModifier() {}
ObjectLightingModifier::~ObjectLightingModifier() {}