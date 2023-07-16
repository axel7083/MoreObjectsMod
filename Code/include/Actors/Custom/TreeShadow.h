#pragma once

struct TreeShadow : Actor
{
	ShadowModel shadow;
	Matrix4x3 shadowMat;
	
	static SpawnInfo spawnData;
	
	TreeShadow();
	virtual s32 InitResources() override;
	virtual s32 CleanupResources() override;
	virtual s32 Behavior() override;
	virtual s32 Render() override;
	virtual ~TreeShadow() override;
	
	void DropShadow();
};