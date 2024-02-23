#pragma once

struct DoorBlocker : Platform
{	
	Vector3 center;
	u8 eventID;
	bool gone;
	bool camSet;
	
	static SpawnInfo spawnData;
	static SharedFilePtr modelFile;
	static SharedFilePtr clsnFile;
	
	DoorBlocker();
	virtual int InitResources() override;
	virtual int CleanupResources() override;
	virtual int Behavior() override;
	virtual int Render() override;
	virtual ~DoorBlocker() override;
	
	void UpdateModelTransform();
};