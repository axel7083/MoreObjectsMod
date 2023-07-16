#include "SM64DS_2.h"
#include "Actors/Custom/DoorBlocker.h"

namespace
{
	using clpsBlock = StaticCLPS_Block<
		{  }
	>;
}

SpawnInfo DoorBlocker::spawnData =
{
	[]() -> ActorBase* { return new DoorBlocker; },
	0x016e,
	0x00cc,
	Actor::NO_RENDER_IF_OFF_SCREEN,
	125._f,
	300._f,
	4096._f,
	4096._f,
};

SharedFilePtr DoorBlocker::modelFile;
SharedFilePtr DoorBlocker::clsnFile;

int DoorBlocker::InitResources()
{
	Model::LoadFile(modelFile);
	model.SetFile(modelFile.BMD(), 1, -1);
	
	UpdateModelPosAndRotY();
	UpdateClsnPosAndRot();
	
	MovingMeshCollider::LoadFile(clsnFile);
	clsn.SetFile(clsnFile.KCL(), clsnNextMat, 1._f, ang.y, clpsBlock::instance<>);
	
	eventID = param1 & 0xff;
	gone = eventID < NUM_EVENTS && Event::GetBit(eventID);
	camSet = true;
	
	center = Vector3{ 80._f * Sin(ang.y), 125._f, 80._f * Cos(ang.y) };
	
	return 1;
}

int DoorBlocker::CleanupResources()
{
	clsn.Disable();
	
	modelFile.Release();
	clsnFile.Release();
	
	return 1;
}

int DoorBlocker::Behavior()
{
	if (eventID < NUM_EVENTS && Event::GetBit(eventID))
	{
		clsn.DisableIfEnabled();
		
		if (!gone)
		{
			gone = true;
			camSet = false;
			DisappearPoofDustAt(center);
		}
	}
	else
	{
		UpdateModelPosAndRotY();
		IsClsnInRange(0_f, 0_f);
		
		if (gone)
		{
			gone = false;
			camSet = false;
			PoofDustAt(center);
		}
	}
	
	return 1;
}

int DoorBlocker::Render()
{
	if (eventID >= NUM_EVENTS || !Event::GetBit(eventID))
		model.Render();
	
	return 1;
}

DoorBlocker::DoorBlocker() {}
DoorBlocker::~DoorBlocker() {}