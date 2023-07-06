#pragma once

struct InvisibleWall : Platform
{
	static SpawnInfo spawnData;
	static SharedFilePtr clsnFile;
	
	InvisibleWall();
	virtual s32 InitResources() override;
	virtual s32 CleanupResources() override;
	virtual s32 Behavior() override;
	virtual s32 Render() override;
	virtual ~InvisibleWall();
};