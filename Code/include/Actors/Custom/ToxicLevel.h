#pragma once

struct ToxicLevel : Actor
{
	u32 toxicCounter;
	u32 frameForDamage;
	
	static SpawnInfo spawnData;
	
	ToxicLevel();
	virtual s32 InitResources() override;
	virtual s32 CleanupResources() override;
	virtual s32 Behavior() override;
	virtual s32 Render() override;
	virtual ~ToxicLevel() override;
};