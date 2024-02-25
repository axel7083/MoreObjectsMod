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
    materialApplied = false;

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
        UpdateMaterial();
    }
	return 1;
}

void FlipSwitch::UpdateMaterial() {
    if(!materialApplied) {
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
        materialApplied = true;
    }

}

int FlipSwitch::Render()
{
	model.Render(nullptr);
	return 1;
}

void FlipSwitch::AfterClsn(Actor& otherActor)
{
    if(fsState == FS_INTERROGATION) {
        fsState = FS_EXCLAMATION;
        materialApplied = false;
    }
}

void FlipSwitch::AfterClsnCallback(MeshColliderBase& clsn, Actor& clsnActor, Actor& otherActor)
{
    ((FlipSwitch&)clsnActor).AfterClsn(otherActor);
}

FlipSwitch::~FlipSwitch() {}