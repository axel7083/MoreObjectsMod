#ifndef MEGA_BLOCK_INCLUDED
#define MEGA_BLOCK_INCLUDED

#include "SM64DS_2.h"

struct MegaBlock : public Platform
{
    ShadowModel shadow;
	Matrix4x3 shadowMat;
	
	uint8_t starID;
	
	void UpdateShadowMatrix();

	static MegaBlock* Spawn();
	virtual int InitResources() override;
	virtual int CleanupResources() override;
	virtual int Behavior() override;
	virtual int Render() override;
	virtual ~MegaBlock();
	virtual void OnHitByMegaChar(Player& megaChar) override;
	virtual void Kill() override;
	
	static SharedFilePtr modelFile;
	static SharedFilePtr clsnFile;

	static SpawnInfo spawnData;
};

#endif