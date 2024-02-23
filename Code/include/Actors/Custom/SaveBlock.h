#pragma once

struct SaveBlock : Platform
{
	enum JiggleState : u8
	{
		JS_NO_JIGGLE,
		JS_JIGGLE_UP,
		JS_JIGGLE_DOWN,
		JS_JIGGLE_STOP,
	};
	
	TextureSequence texSeq;
	ShadowModel shadow;
	Matrix4x3 shadowMat;
	Vector3 originalPos;
	Fix12i floorPosY;
	u8 jiggleState;
	
	static SpawnInfo spawnData;
	static SharedFilePtr modelFile;
	static SharedFilePtr texSeqFile;
	static SharedFilePtr clsnFile;
	
	SaveBlock();
	virtual int InitResources() override;
	virtual int CleanupResources() override;
	virtual int Behavior() override;
	virtual int Render() override;
	virtual ~SaveBlock() override;
	virtual void OnHitFromUnderneath(Actor& attacker) override;
	
	void DropShadow();
	Fix12i GetFloorPosY();
	void Jiggle();
};