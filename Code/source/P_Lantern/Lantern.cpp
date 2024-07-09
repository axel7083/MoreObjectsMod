#include "SM64DS_2.h"
#include "Actors/Custom/lantern.h"

namespace
{
    using clpsBlock = StaticCLPS_Block<
            { .camBehavID = CLPS::CA_NORMAL, .camGoesThru = true, }
    >;
}

SpawnInfo Lantern::spawnData =
        {
                []() -> ActorBase* { return new Lantern; },
                0x0034,
                0x0100,
                Actor::NO_RENDER_IF_OFF_SCREEN | Actor::GRABBABLE,
                0._f,
                1500._f,
                4096._f,
                4096._f
        };

SharedFilePtr Lantern::modelFile;
SharedFilePtr Lantern::clsnFile;

Lantern::Counter Lantern::lanternCounters[NUM_COUNTERS];

int Lantern::InitResources()
{
    Model::LoadFile(modelFile);
    model.SetFile(modelFile.BMD(), 1, -1);

    // Set properties
    eventID = param1 & 0xff;

    cout << "event id " << eventID;

    cylClsn.Init(this, 120._f,120._f, CylinderClsn::ENEMY | CylinderClsn::INTANGIBLE, CylinderClsn::HIT_BY_FIRE);

    MovingMeshCollider::LoadFile(clsnFile);
    clsn.SetFile(clsnFile.KCL(), clsnNextMat, 0.1_f, ang.y, clpsBlock::instance<>);

    this->onFire = false;

    if (eventID > NUM_EVENTS) {
        cout << "invalid event id";
        return 0;
    }

    RegisterEventID();

    // We have a shared counter between instances
    u8& lanternCounter = GetCounter().count;
    lanternCounter++;

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    return 1;
}

void Lantern::UpdateModelTransform() {
    model.mat4x3 = Matrix4x3::RotationY(ang.y);
    model.mat4x3.c3 = pos >> 3;
}

int Lantern::CleanupResources()
{
    clsn.DisableIfEnabled();

    modelFile.Release();
    clsnFile.Release();

    return 1;
}

int Lantern::Behavior()
{
    if (IsClsnInRange(0._f, 0._f))
    {
        if ((cylClsn.hitFlags & CylinderClsn::HIT_BY_FIRE) && !this->onFire) {
            cout << "on fire";
            Vector3 firePos = { pos.x, pos.y + 200._f, pos.z };
            Actor::Spawn(OBJ_RED_FIRE_ACTOR_ID, 0x1, firePos, &ang, areaID, -1);
            this->onFire = true;

            // remove 1 to the counter
            u8& flipSwitchCounter = GetCounter().count;
            flipSwitchCounter--;

            // if the counter is equal to 0 => all lantern are on fire
            if(flipSwitchCounter == 0) {
                Event::SetBit(eventID);
                Sound::Play("NCS_SE_SYS_SMALL_HAPPY"sfx, camSpacePos);
            }
        }
    }

    cylClsn.Clear();
    cylClsn.Update();

    UpdateClsnPosAndRot();

    UpdateModelTransform();
    return 1;
}

void Lantern::RegisterEventID()
{
    for (s32 i = 0; i < NUM_COUNTERS; i++)
    {
        if (lanternCounters[i].eventID == eventID)
            return;

        if (lanternCounters[i].eventID == 0xff)
        {
            lanternCounters[i].eventID = eventID;
            return;
        }
    }

    Crash();
}

Lantern::Counter& Lantern::GetCounter()
{
    for (s32 i = 0; i < NUM_COUNTERS; i++)
    {
        if (lanternCounters[i].eventID == eventID)
            return lanternCounters[i];
    }

    Crash();
}


int Lantern::Render()
{
    model.Render();

    return 1;
}

Lantern::Lantern() {}
Lantern::~Lantern() {}