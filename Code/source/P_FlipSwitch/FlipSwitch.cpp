#include "Actors/Custom/FlipSwitch.h"

namespace
{
    using clpsBlock = StaticCLPS_Block<
            {  }
    >;
}

SharedFilePtr FlipSwitch::modelFile;
SharedFilePtr FlipSwitch::clsnFile;
	
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
	Model::LoadFile(modelFile);
	model.SetFile(modelFile.BMD(), 1, -1);

	MovingMeshCollider::LoadFile(clsnFile);
	clsn.SetFile(clsnFile.KCL(), clsnNextMat, 1._f, ang.y, clpsBlock::instance<>);

    fsState = FS_INTERROGATION;
    dirtyMaterial = true;
    counter = 0;

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
        if(dirtyMaterial) {
            UpdateMaterial();
        }

        if(counter > 0)
            counter--;
    }
	return 1;
}

void FlipSwitch::UpdateMaterial() {
    switch(fsState) {
        case FS_INTERROGATION:
            model.data.materials->teximageParam = model.data.modelFile->textures[1].cmd2aPart1 | model.data.modelFile->materials[1].cmd2aPart2;
            model.data.materials->paletteInfo = model.data.modelFile->palettes[1].vramOffset >> 4;
            break;
        case FS_EXCLAMATION:
            model.data.materials->teximageParam = model.data.modelFile->textures[0].cmd2aPart1 | model.data.modelFile->materials[0].cmd2aPart2;
            model.data.materials->paletteInfo = model.data.modelFile->palettes[0].vramOffset >> 4;
            break;
        default:
            break;
    }
    dirtyMaterial = false;
}

int FlipSwitch::Render()
{
	model.Render(nullptr);
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

    if(counter == 1) {
        counter = 2;
        return;
    }

    counter = 2;
    switch (fsState) {
        case FS_INTERROGATION:
            fsState = FS_EXCLAMATION;
            break;
        case FS_EXCLAMATION:
            fsState = FS_INTERROGATION;
            break;
    }
    dirtyMaterial = true;
}

void FlipSwitch::AfterClsnCallback(MeshColliderBase& clsn, Actor& clsnActor, Actor& otherActor)
{
    ((FlipSwitch&)clsnActor).AfterClsn(otherActor);
}

FlipSwitch::~FlipSwitch() {}