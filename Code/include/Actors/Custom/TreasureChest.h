#ifndef TREASURE_CHEST_INCLUDED
#define TREASURE_CHEST_INCLUDED

#include "SM64DS_2.h"

//because Koopas and Treasure Chests are incompatible
struct TreasureChest : public Actor
{
    enum Animations
    {
        OPEN,

        NUM_ANIMS
    };

    ModelAnim modelAnim; //0x0d4
    MovingCylinderClsn cylClsn; //0x138
	
	unsigned state;
	uint16_t cooldown;
	uint8_t order;
	bool spawnStar;
	uint8_t starID;
	uint8_t trackedStarID;
	
	void UpdateModelTransform();
	void ChangeState(unsigned state);
	void CallState();
	
	void State0_Init();
	void State0_Main();
	void State1_Init();
	void State1_Main();
	void State2_Init();
	void State2_Main();
	
	static TreasureChest* Spawn();
	virtual int InitResources() override;
	virtual int CleanupResources() override;
	virtual int Behavior() override;
	virtual int Render() override;
	virtual ~TreasureChest() override;
	
	static SharedFilePtr modelFile;
	static SharedFilePtr animFiles[1];
	
	static SpawnInfo spawnData;
};

#endif