#pragma once

struct BooMario : Actor
{

	ModelAnim modelAnim;

	static SpawnInfo spawnData;
	static SharedFilePtr modelFile;
	static SharedFilePtr animFile;

    BooMario();
	virtual s32 InitResources() override;
	virtual s32 CleanupResources() override;
	virtual s32 Behavior() override;
	virtual s32 Render() override;
	virtual ~BooMario() override;
	
	void UpdateModelTransform();
};