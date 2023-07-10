#pragma once

struct ColoredCoin : Actor
{
	Model model;
	MovingCylinderClsn cylClsn;
	ShadowModel shadow;
	Matrix4x3 shadowMat;
	Player* killer;
	Actor* spawnedStar;
	bool killable;
	bool fake;
	bool hurt;
	bool deathCoin;
	bool deathStarted;
	u8 health;
	u8 starID;
	s32 deathFrames;
	s32 frameCounter;
	s32 value;

	static SpawnInfo spawnData;
	static SharedFilePtr modelFile;
	
	ColoredCoin();
	virtual s32 InitResources() override;
	virtual s32 CleanupResources() override;
	virtual s32 Behavior() override;
	virtual s32 Render() override;
	virtual u32 OnYoshiTryEat() override;
	virtual void OnTurnIntoEgg(Player& player) override;
	virtual ~ColoredCoin() override;
	
	void UpdateModelTransform();
	void DropShadow();
	void CheckCylClsn();
	void Kill(Player& player);
	bool IsInYoshiMouth();
};