#pragma once

struct GalaxyShrinkPlatform : Platform
{
	bool shrinking;
	bool shrinked;
	bool isBig;
	Model frameModel;

	static SpawnInfo spawnData;
	static SharedFilePtr modelFile;
	static SharedFilePtr frameModelFile;
	static SharedFilePtr clsnFile;
	
	GalaxyShrinkPlatform();
	virtual s32 InitResources() override;
	virtual s32 CleanupResources() override;
	virtual s32 Behavior() override;
	virtual s32 Render() override;
	virtual ~GalaxyShrinkPlatform() override;
	
	void UpdateModelTransform();
	void Shrink();
	void Unshrink();
	void AfterClsn(Actor& otherActor);
	
	static void AfterClsnCallback(MeshColliderBase& clsn, Actor& clsnActor, Actor& otherActor);
};