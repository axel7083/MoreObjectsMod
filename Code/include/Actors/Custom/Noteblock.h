#pragma once

struct NoteBlock : Platform
{
	enum JiggleState : u8
	{
		JS_NO_JIGGLE,
		JS_JIGGLE_DOWN,
		JS_JIGGLE_UP,
		JS_JIGGLE_STOP,
	};
	
	ShadowModel shadow;
	Matrix4x3 shadowMat;
	Vector3 originalPos;
	Player* clsnPlayer;
	Fix12i boostBounceVel;
	u8 jiggleState;

	static SpawnInfo spawnData;
	static SharedFilePtr modelFile;
	static SharedFilePtr clsnFile;

	NoteBlock();
	virtual s32 InitResources() override;
	virtual s32 CleanupResources() override;
	// virtual bool BeforeBehavior() override;
	virtual s32 Behavior() override;
	virtual s32 Render() override;
	virtual ~NoteBlock() override;
	
	void DropShadow();
	void Jiggle();
	void LaunchPlayer();
	void AfterClsn(Actor& otherActor);
	
	static void AfterClsnCallback(MeshColliderBase& clsn, Actor& clsnActor, Actor& otherActor);
};