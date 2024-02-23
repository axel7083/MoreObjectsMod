#pragma once

struct StarChip : Actor
{
	struct Counter
	{
		u8 count = 0;
		u8 eventID = 0xff;
	};
	
	static constexpr s32 NUM_COUNTERS = 1;
	
	Model model;
	MovingCylinderClsn cylClsn;
	ShadowModel shadow;
	Matrix4x3 shadowMat;
	Fix12i floorPosY;
	u8 eventID;
	bool killable;
	
	static SpawnInfo spawnData;
	static SharedFilePtr modelFile;
	
	static Counter chipCounters[NUM_COUNTERS];
	
	StarChip();
	virtual int InitResources() override;
	virtual int CleanupResources() override;
	virtual int Behavior() override;
	virtual int Render() override;
	virtual ~StarChip() override;
	virtual u32 OnYoshiTryEat() override;
	virtual void OnTurnIntoEgg(Player& player) override;
	
	void UpdateModelTransform();
	void DropShadow();
	void CheckCylClsn();
	void Kill();
	bool IsInYoshiMouth();
	
	void RegisterEventID();
	Counter& GetCounter();
};