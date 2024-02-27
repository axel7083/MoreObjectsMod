#include "Actors/Custom/FlipSwitch.h"

namespace
{
    using clpsBlock = StaticCLPS_Block<
            {  }
    >;

    constexpr Fix12i REGULAR_SCALE = 1._f;
    constexpr Fix12i BIG_SCALE = 2.0_f;

    constexpr Vector3 BIG_FRAME_SCALE = { BIG_SCALE, BIG_SCALE, BIG_SCALE };
}

SharedFilePtr FlipSwitch::modelFile;
SharedFilePtr FlipSwitch::clsnFile;

FlipSwitch::Counter FlipSwitch::flipSwitchCounters[NUM_COUNTERS];

SpawnInfo FlipSwitch::spawnData =
{
    []() -> ActorBase* { return new FlipSwitch; },
    0x0030,
    0x0100,
    Actor::NO_RENDER_IF_OFF_SCREEN,
    0._f,
    1500._f,
    4096._f,
    4096._f
};

FlipSwitch* FlipSwitch::Spawn()
{
	return new FlipSwitch;
}

int FlipSwitch::InitResources()
{
    // Loading the platform that change material when walk on it
	Model::LoadFile(modelFile);
	model.SetFile(modelFile.BMD(), 1, -1);

    // Set properties
    eventID = param1 & 0xff;
    isBig = true;
    fsState = FS_INTERROGATION;
    dirtyMaterial = true;
    counter = 0;
    clsnPlayer = nullptr;

    // Loading the collider (reusing the collider of GalaxyShrink)
	MovingMeshCollider::LoadFile(clsnFile);
    clsn.SetFile(clsnFile.KCL(), clsnNextMat, isBig ? BIG_SCALE : REGULAR_SCALE, ang.y, clpsBlock::instance<>);

    if (eventID > NUM_EVENTS)
        return 0;

    RegisterEventID();

    // We have a shared counter between instances
    u8& flipSwitchCounter = GetCounter().count;
    flipSwitchCounter++;

    clsn.beforeClsnCallback = &MeshColliderBase::UpdatePosWithTransform;
    clsn.afterClsnCallback = &FlipSwitch::AfterClsnCallback;

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
	return 1;
}

int FlipSwitch::CleanupResources()
{
	if(clsn.IsEnabled())
		clsn.Disable();
	
	modelFile.Release();
	clsnFile.Release();
	
	return 1;
}

int FlipSwitch::Behavior()
{
    if (IsClsnInRange(0._f, 0._f)) {
        UpdateClsnPosAndRot();

        if(GetCounter().count == 0) {
            fsState = FS_VALIDATED;
            dirtyMaterial = true;
        }

        if(dirtyMaterial) {
            UpdateMaterial();
        }

        if (clsnPlayer != nullptr && clsnPlayer->IsState(*(Player::State*)Player::ST_JUMP)) {
            return 1;
        }

        if(counter > 0)
            counter--;
    }
	return 1;
}

void FlipSwitch::UpdateMaterial() {
    switch(fsState) {
        case FS_INTERROGATION:
            model.data.materials->teximageParam = model.data.modelFile->textures[0].cmd2aPart1 | model.data.modelFile->materials[0].cmd2aPart2;
            model.data.materials->paletteInfo = model.data.modelFile->palettes[0].vramOffset >> 4;
            break;
        case FS_EXCLAMATION:
            model.data.materials->teximageParam = model.data.modelFile->textures[1].cmd2aPart1 | model.data.modelFile->materials[1].cmd2aPart2;
            model.data.materials->paletteInfo = model.data.modelFile->palettes[1].vramOffset >> 4;
            break;
        case FS_VALIDATED:
            model.data.materials->teximageParam = model.data.modelFile->textures[2].cmd2aPart1 | model.data.modelFile->materials[2].cmd2aPart2;
            model.data.materials->paletteInfo = model.data.modelFile->palettes[2].vramOffset >> 4;
            break;
        default:
            break;
    }
    dirtyMaterial = false;
}

int FlipSwitch::Render()
{
    // Render the models in the appropriate sizes
	model.Render(isBig ? &BIG_FRAME_SCALE : nullptr);
	return 1;
}

void FlipSwitch::AfterClsn(Actor& otherActor)
{
    if (otherActor.actorID != PLAYER_ACTOR_ID)
        return;

    Player& player = (Player&)otherActor;

    // Do not count if the player is edge graping
    if (player.IsState(*(Player::State*)Player::ST_LEDGE_GRAB))
        return;

    clsnPlayer = &player;

    if(counter > 0) {
        counter = 2;
        return;
    }

    counter = 2;
    u8& flipSwitchCounter = GetCounter().count;

    switch (fsState) {
        case FS_INTERROGATION:
            fsState = FS_EXCLAMATION;
            flipSwitchCounter--;
            break;
        case FS_EXCLAMATION:
            fsState = FS_INTERROGATION;
            flipSwitchCounter++;
            break;
        default:
            return;
    }

    if(flipSwitchCounter == 0) {
        Event::SetBit(eventID);
    }

    dirtyMaterial = true;
}

void FlipSwitch::AfterClsnCallback(MeshColliderBase& clsn, Actor& clsnActor, Actor& otherActor)
{
    ((FlipSwitch&)clsnActor).AfterClsn(otherActor);
}

void FlipSwitch::RegisterEventID()
{
    for (s32 i = 0; i < NUM_COUNTERS; i++)
    {
        if (flipSwitchCounters[i].eventID == eventID)
            return;

        if (flipSwitchCounters[i].eventID == 0xff)
        {
            flipSwitchCounters[i].eventID = eventID;
            return;
        }
    }

    Crash();
}

FlipSwitch::Counter& FlipSwitch::GetCounter()
{
    for (s32 i = 0; i < NUM_COUNTERS; i++)
    {
        if (flipSwitchCounters[i].eventID == eventID)
            return flipSwitchCounters[i];
    }

    Crash();
}

FlipSwitch::~FlipSwitch() {}