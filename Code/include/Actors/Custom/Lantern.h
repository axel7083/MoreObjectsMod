#pragma once

#include "SM64DS_2.h"

struct Lantern : Platform
{
    struct Counter
    {
        u8 count = 0;
        u8 eventID = 0xff;
    };

    MovingCylinderClsn cylClsn;
    bool onFire;

    // The event to trigger when all lantern are on fire
    u8 eventID;

    static constexpr s32 NUM_COUNTERS = 2; // Allowing 2 counter, meaning supporting two sets of lanterns with their own events for each
    static Counter lanternCounters[NUM_COUNTERS];

    void RegisterEventID();
    Counter& GetCounter();

    static SpawnInfo spawnData;
    static SharedFilePtr modelFile;
    static SharedFilePtr clsnFile;

    Lantern();
    virtual int InitResources() override;
    virtual int CleanupResources() override;
    virtual int Behavior() override;
    virtual int Render() override;
    virtual ~Lantern() override;

    void UpdateModelTransform();
};