#pragma once

struct CharacterBlock : Platform
{
	enum BlockTypes
	{
		BT_MARIO,
		BT_LUIGI,
		BT_WARIO,
		BT_YOSHI,
		
		NUM_BLOCK_TYPES,
	};
	
	enum JiggleState : u8
	{
		JS_NO_JIGGLE,
		JS_JIGGLE_UP,
		JS_JIGGLE_DOWN,
		JS_JIGGLE_STOP,
	};
	
	static constexpr s32 NUM_DUST_PARTICLES = 2;
	
	TextureSequence texSeq;
	TextureSequence transTexSeq;
	ModelAnim transModel;
	ShadowModel shadow;
	Matrix4x3 shadowMat;
	Vector3 originalPos;
	Fix12i floorPosY;
	u8 blockType;
	u32 myParticle;
	u32 startingCharacter;
	bool needsUnlock;
	bool isUnlocked;
	u8 jiggleState;
	bool healPlayer;
	
	static SpawnInfo spawnData;
	static SharedFilePtr modelFile;
	static SharedFilePtr transModelFile;
	static SharedFilePtr texSeqFile;
	static SharedFilePtr transTexSeqFile;
	static SharedFilePtr animFile;
	static SharedFilePtr clsnFile;
	static SharedFilePtr starsParticleFiles[NUM_BLOCK_TYPES];
	static SharedFilePtr dustParticleFiles[2][NUM_BLOCK_TYPES];
	
	static Particle::SysDef starsSysDefs[NUM_BLOCK_TYPES];
	static Particle::SysDef dustSysDefs[2][NUM_BLOCK_TYPES];
	
	CharacterBlock();
	virtual s32 InitResources() override;
	virtual s32 CleanupResources() override;
	virtual s32 Behavior() override;
	virtual s32 Render() override;
	virtual ~CharacterBlock() override;
	virtual void OnHitFromUnderneath(Actor& attacker) override;
	
	void UpdateModelTransform();
	void DropShadow();
	Fix12i GetFloorPosY();
	void Jiggle();
	void JumpedUnderBlock();
	bool CheckUnlock();
};