#ifndef FLIP_SWITCH_INCLUDED
#define FLIP_SWITCH_INCLUDED

#include "SM64DS_2.h"

struct FlipSwitch : public Platform
{
    struct Counter
    {
        u8 count = 0;
        u8 eventID = 0xff;
    };

    enum FlipSwitchState : u8
    {
        FS_INTERROGATION,
        FS_EXCLAMATION,
        FS_VALIDATED,
    };

    bool isBig;

    // used to keep the state in memory
    // 0: player has not walk on it
    // 1: player has walk on it
    // 2: player is walking on it or is jumping after walking on it
    int counter;

    // When the state change we set this variable to true
    // meaning we should update the material used
    bool dirtyMaterial;

    // We keep the pointer to the player who just walked on it
    Player* clsnPlayer;

    // The state (interrogation, exclamation, validated
    u8 fsState;

    // The event to trigger when all flip switch are in EXCLAMATION
    u8 eventID;

    // static members to share information between instances
    static constexpr s32 NUM_COUNTERS = 1;
    static Counter flipSwitchCounters[NUM_COUNTERS];

    void RegisterEventID();
    Counter& GetCounter();

    static FlipSwitch* Spawn();
    virtual int InitResources() override;
    virtual int CleanupResources() override;
    virtual int Behavior() override;
    virtual int Render() override;
    virtual ~FlipSwitch();

    static SharedFilePtr modelFile;
    static SharedFilePtr clsnFile;

    static SpawnInfo spawnData;

    void AfterClsn(Actor& otherActor);
    void UpdateMaterial();
    static void BeforeClsnCallback(MeshColliderBase& clsn, Actor& clsnActor, Actor& otherActor);
    static void AfterClsnCallback(MeshColliderBase& clsn, Actor& clsnActor, Actor& otherActor);
};

#endif