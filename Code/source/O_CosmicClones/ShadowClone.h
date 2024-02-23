// Author: SplattyDS
#pragma once

struct ShadowCloneFrame
{
	u32 animID;
	Fix12i animFrame;
	bool loaded;
	
	// Matrix4x3 transformMat;
	Vector3_16 ang;
	Vector3 pos;
	Vector3 transformVec;
	
	Fix12i shadowHeight;
	Fix12i shadowRad;
	Fix12i shadowDepth;
};

struct ShadowClone : Actor
{
	static constexpr s32 NUM_CLONES = 3;
	static constexpr s32 FRAMES_BETWEEN_CLONES = 20;
	static constexpr s32 TOTAL_FRAMES = NUM_CLONES * FRAMES_BETWEEN_CLONES;
	
	ModelAnim bodyModel;
	Model headModel;
	ShadowModel shadows[NUM_CLONES];
	Matrix4x3 shadowMats[NUM_CLONES];
	CylinderClsnWithPos cylClsns[NUM_CLONES];
	ShadowCloneFrame frames[TOTAL_FRAMES];
	u32 lastFrame; // lastFrame is rendered, then replaced (this is the frame of the LAST clone)
	u16 disabledTimer;
	bool active[NUM_CLONES];
	s32 activationFrame;
	u32 spawnParticleID;
	u32 moveParticleIDs[NUM_CLONES];
	
	static SpawnInfo spawnData;
	
	virtual s32 InitResources() override;
	virtual s32 CleanupResources() override;
	virtual s32 Behavior() override;
	virtual s32 Render() override;
	
	void ReplaceLastFrame();
	void CheckClsnWithPlayer(CylinderClsnWithPos& cylClsn);
	void DropShadow(ShadowCloneFrame& frame, s32 shadowID);
	
	static SharedFilePtr& GetPlayerAnimation(u32 animID);
};