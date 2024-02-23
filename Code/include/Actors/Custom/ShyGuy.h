#pragma once

struct ShyGuy : Enemy
{
	enum Animations
	{
		WAIT,
		WALK,
		RUN,
		FREEZE,
		
		NUM_ANIMS,
	};
	
	enum States
	{
		ST_WAIT,
		ST_TURN,
		ST_CHASE,
		ST_STOP,
		ST_WARP,
	};
	
	MovingCylinderClsn cylClsn;
	WithMeshClsn wmClsn;
	ModelAnim modelAnim;
	ShadowModel shadow;
	MaterialChanger matChg;
	u8 state;
	u8 chaseCooldown;
	s16 targetAngle;
	Player* targetPlayer;
	Vector3 targetPos;
	bool offTrack;
	u8 nextPathNode;
	u8 numPathNodes;
	bool alarmed;
	bool backAndForth;
	bool reverse;
	bool customColor;
	PathPtr pathPtr;
	u32 color;
	
	static SpawnInfo spawnData;
	static SharedFilePtr modelFile;
	static SharedFilePtr animFiles[NUM_ANIMS];
	
	ShyGuy();
	virtual s32 InitResources() override;
	virtual s32 CleanupResources() override;
	virtual s32 Behavior() override;
	virtual s32 Render() override;
	virtual ~ShyGuy() override;
	virtual u32 OnYoshiTryEat() override;
	virtual void OnTurnIntoEgg(Player& player) override;
	virtual Fix12i OnAimedAtWithEgg() override;
	
	void UpdateModelTransform();
	void SetTargetPos();
	void Kill();
	void CheckCylClsn();
	Player* PlayerVisibleToThis(Player* player);
	bool KillIfTouchedBadSurface();
	s32 GetClosestPathNodeID();
	void AimAtClosestPathPt();
	void PlayMovingSoundEffect();
	
	void State0_Wait();
	void State1_Turn();
	void State2_Chase();
	void State3_Stop();
	void State4_Warp();
	
	static Fix12i FloorY(const Vector3& pos);
};