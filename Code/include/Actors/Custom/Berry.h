#pragma once

struct Berry : Actor
{
	Model model;
	Model stem;
	MovingCylinderClsn cylClsn;
	ShadowModel shadow;
	Vector3 origPos;
	Fix12i berryOffsetX;
	Fix12i berryOffsetZ;
	bool groundFound;
	bool killed;
	bool isOnFloor;
	
	static SpawnInfo spawnData;
	static SharedFilePtr modelFile;
	static SharedFilePtr stemModelFile;
	
	static u32 berryCount;
	static u32 berryMaxCount;
	
	Berry();
	virtual s32 InitResources() override;
	virtual s32 CleanupResources() override;
	virtual s32 Behavior() override;
	virtual s32 Render() override;
	virtual ~Berry() override;
	virtual u32 OnYoshiTryEat() override;
	virtual void OnTurnIntoEgg(Player& player) override;
	
	void UpdateModelTransform();
	void Kill();
};