#pragma once

constexpr Fix12i SHOT_RADIUS = 48._f;

struct Magikoopa : Enemy
{
	// this keeps track of all the resources what the Magikoopa spawns needs.
	// without this, particle glitches can happen.
	struct Resources
	{
		static constexpr s32 NUM_PER_CHUNK = 16;
		
		u32 files[NUM_PER_CHUNK];
		Resources* next;
		
		~Resources();
		
		void Add(SharedFilePtr& sf);
		void ProcessAdditions();
	};
	
	struct SharedRes
	{
		static constexpr s32 SHOT_LIMIT = 3;
		
		u32 numRefs = 1;
		u32 spawnActorID;
		u32 spawnActorParams;
		u8 type;
		u32 shotUniqueIDs[SHOT_LIMIT] = { 0xffffffff, 0xffffffff, 0xffffffff };
		Resources res;
		
		void TrackRes();
		void StopTracking();
		
		static void OnLoadFile(SharedFilePtr& file);
	};
	
	enum Animations
	{
		APPEAR,
		WAVE,
		SHOOT,
		POOF,
		WAIT,
		HURT,
		DEFEAT,
		
		NUM_ANIMS,
	};
	
	enum Types
	{
		MAGIKOOPA,
		KAMELLA,
		
		NUM_TYPES,
	};
	
	enum States
	{
		ST_APPEAR,
		ST_WAVE,
		ST_SHOOT,
		ST_POOF,
		ST_TELEPORT,
		ST_HURT,
		ST_WAIT,
		ST_DEFEAT,
		
		NUM_STATES,
	};
	
	MovingCylinderClsn cylClsn;
	WithMeshClsn wmClsn;
	ModelAnim modelAnim;
	ShadowModel shadow;
	Vector3 originalPos;
	PathPtr pathPtr;
	SharedRes* res;
	Player* listener;
	Fix12i horzDecel;
	u32 particleID;
	u8 state;
	u8 numPathPts;
	u8 currPathPt;
	u8 nextPathPt;
	u8 eventID;
	u8 starID;
	u8 resourceRefCount;
	u8 shotState;
	u8 health;
	bool invincible;
	bool battleStarted;
	
	static SpawnInfo spawnData[NUM_TYPES];
	static SharedFilePtr modelFiles[NUM_TYPES];
	static SharedFilePtr animFiles[NUM_ANIMS];
	static SharedFilePtr particleResFiles[NUM_TYPES];
	
	static Particle::SysDef particleSysDefs[NUM_TYPES];

	Magikoopa();
	virtual s32 InitResources() override;
	virtual s32 CleanupResources() override;
	virtual s32 Behavior() override;
	virtual s32 Render() override;
	virtual void OnPendingDestroy() override;
	virtual ~Magikoopa() override;
	virtual u32 OnYoshiTryEat() override;
	virtual void OnTurnIntoEgg(Player& player) override;
	virtual Fix12i OnAimedAtWithEgg() override; //returns egg height
	
	void UpdateModelTransform();
	void KillMagikoopa();
	void HandleClsn();
	Vector3 GetWandTipPos();
	
	void AttemptTalkStartIfNotStarted();
	void Talk();
	
	void St_Appear();
	void St_Wave();
	void St_Shoot();
	void St_Poof();
	void St_Teleport();
	void St_Hurt();
	void St_Wait();
	void St_Defeat();
};

struct MagikoopaShot : Enemy
{
	MovingCylinderClsn cylClsn;
	WithMeshClsn wmClsn;
	Vector3_16f direction;
	u8* resourceRefCount;
	Magikoopa::SharedRes* res;
	u32 particleID;
	u8 shotState;
	u8 numFireToSpawn;
	
	static SpawnInfo spawnData;
	
	MagikoopaShot();
	virtual s32 InitResources() override;
	virtual s32 CleanupResources() override;
	virtual s32 Behavior() override;
	virtual s32 Render() override;
	virtual ~MagikoopaShot() override;
	
	void SetMagikoopa(Magikoopa& magikoopa);
};