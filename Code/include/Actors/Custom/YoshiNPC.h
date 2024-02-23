#pragma once

struct YoshiNPC : Actor
{
	enum States
	{
		ST_WAIT,
		ST_TALK,
		
		NUM_STATES,
	};
	
	ModelAnim modelAnim;
	MovingCylinderClsn cylClsn;
	ShadowModel shadow;
	Player* listener;
	u16 messages[2];
	u8 state;
	u8 eventID;
	bool shouldTalk;
	
	static SpawnInfo spawnData;
	static SharedFilePtr modelFile;
	static SharedFilePtr animFile;
	
	YoshiNPC();
	virtual s32 InitResources() override;
	virtual s32 CleanupResources() override;
	virtual s32 Behavior() override;
	virtual s32 Render() override;
	virtual ~YoshiNPC() override;
	
	void UpdateModelTransform();
	
	void St_Wait();
	void St_Talk();
};