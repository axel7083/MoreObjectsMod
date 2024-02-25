#ifndef FLIP_SWITCH_INCLUDED
#define FLIP_SWITCH_INCLUDED

#include "SM64DS_2.h"

struct FlipSwitch : public Platform
{
    enum FlipSwitchState : u8
    {
        FS_INTERROGATION,
        FS_EXCLAMATION,
        JS_VALIDATED,
    };

    int counter;

    bool dirtyMaterial;
    u8 fsState;

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