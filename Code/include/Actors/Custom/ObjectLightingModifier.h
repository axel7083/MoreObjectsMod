#pragma once

struct ObjectLightingModifier : Actor
{
	static SpawnInfo spawnData;
	
	void UpdateModelTransform();
	
	ObjectLightingModifier();
	virtual s32 InitResources() override;
	virtual s32 CleanupResources() override;
	virtual s32 Behavior() override;
	virtual s32 Render() override;
	virtual ~ObjectLightingModifier() override;
};