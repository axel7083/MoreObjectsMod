#pragma once

struct LaunchStar : Actor
{
	enum Animations
	{
		WAIT,
		LAUNCH,
		
		NUM_ANIMS,
	};
	
	ModelAnim modelAnim;
	MovingCylinderClsn cylClsn;
	Vector3 camPos;
	Vector3 camLookAt;
	Fix12i launchSpeed;
	PathPtr pathPtr;
	u8 eventID;
	u16 spawnTimer;
	
	static SpawnInfo spawnData;
	static SharedFilePtr modelFile;
	static SharedFilePtr animFiles[NUM_ANIMS];
	
	LaunchStar();
	virtual s32 InitResources() override;
	virtual s32 CleanupResources() override;
	virtual s32 Behavior() override;
	virtual s32 Render() override;
	virtual ~LaunchStar() override;
	
	void UpdateModelTransform();
};

// these were part of the Player struct
struct LsPlayerExtension
{
	BezierPathIter bzIt;
	LaunchStar* lsPtr;
	Vector3_16 lsDiffAng;
	Vector3_16 lsInitAng;
	Vector3 lsPos;
	Vector3 lsInitPos;
	u32 particleID;
	u16 launchTimer;
};

extern LsPlayerExtension LS_PLAYER_EXTENSIONS[4];