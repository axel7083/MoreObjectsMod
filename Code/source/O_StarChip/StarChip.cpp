#include "SM64DS_2.h"
#include "Actors/Custom/StarChip.h"

namespace
{
	constexpr Fix12i RADIUS = 100._f;
	constexpr Fix12i HEIGHT = 100._f;
	constexpr s16 ROT_SPEED = 3.667_deg;
	
	constexpr Fix12i BASE_SHADOW_RADIUS = 80._f;
	constexpr Fix12i SHADOW_SCALE_X = 1.25_f;
	constexpr Fix12i SHADOW_SCALE_Z = 2._f;
}

SpawnInfo StarChip::spawnData =
{
	[]() -> ActorBase* { return new StarChip; },
	0x0030,
	0x0100,
	Actor::NO_RENDER_IF_OFF_SCREEN | Actor::UPDATE_DURING_CUTSCENE,
	100._f,
	200._f,
	4096._f,
	4096._f,
};

SharedFilePtr StarChip::modelFile;

StarChip::Counter StarChip::chipCounters[NUM_COUNTERS];

s32 StarChip::InitResources()
{
	Model::LoadFile(modelFile);
	model.SetFile(modelFile.BMD(), 1, -1);
	
	shadow.InitCylinder();
	
	UpdateModelTransform();
	
	cylClsn.Init(this, RADIUS, HEIGHT, CylinderClsn::INTANGIBLE | CylinderClsn::COLLECTABLE, CylinderClsn::HIT_BY_YOSHI_TONGUE);
	
	eventID = param1 & 0xff;
	
	if (eventID > NUM_EVENTS)
		return 0;
	
	RegisterEventID();
	
	RaycastGround raycaster;
	raycaster.SetObjAndPos(pos, nullptr);
	
	floorPosY = raycaster.DetectClsn() ? raycaster.clsnPosY : pos.y;
	
	return 1;
}

s32 StarChip::CleanupResources()
{
	modelFile.Release();
	return 1;
}

s32 StarChip::Behavior()
{
	ang.y += ROT_SPEED;
	
	UpdateModelTransform();
	DropShadow();
	
	if (IsInYoshiMouth())
	{
		cylClsn.Clear();
		return 1;
	}
	
	CheckCylClsn();
	
	cylClsn.Clear();
	cylClsn.Update();
	
	return 1;
}

s32 StarChip::Render()
{
	if (flags & Actor::IN_YOSHI_MOUTH)
		return 1;
	
	model.Render();
	
	return 1;
}

void StarChip::OnTurnIntoEgg(Player& player)
{
	Kill();
}

u32 StarChip::OnYoshiTryEat()
{
	return Actor::YE_SWALLOW;
}

void StarChip::UpdateModelTransform()
{
	model.mat4x3 = Matrix4x3::RotationY(ang.y);
	model.mat4x3.c3 = pos >> 3;
}

void StarChip::DropShadow()
{
	if (flags & Actor::IN_YOSHI_MOUTH)
		return;
	
	shadowMat = model.mat4x3;
	shadowMat.ApplyScale(SHADOW_SCALE_X, 1._f, SHADOW_SCALE_Z);
	shadowMat.c3.y = (pos.y + 10._f) >> 3;
	
	DropShadowRadHeight(shadow, shadowMat, BASE_SHADOW_RADIUS, 512._f, 15);
}

void StarChip::CheckCylClsn()
{
	if (cylClsn.otherObjID == 0)
		return;
	
	Actor* otherObj = Actor::FindWithID(cylClsn.otherObjID);
	
	if (otherObj == nullptr || otherObj->actorID != PLAYER_ACTOR_ID)
		return;
	
	if (!(cylClsn.hitFlags & CylinderClsn::HIT_BY_YOSHI_TONGUE))
		Kill();
}

void StarChip::Kill()
{
	u8& chipCounter = GetCounter().count;
	
	chipCounter++;
	
	Particle::System::NewSimple(0xd2, pos.x, pos.y + 20._f, pos.z);
	
	pos.y += 50._f;
	SpawnNumber(pos, chipCounter, false, 0, 0);
	
	Sound::Play("NCS_SE_STAR_CHIP"sfx, camSpacePos);
	
	if (chipCounter == 5)
	{
		Event::SetBit(eventID);
		chipCounter = 0;
	}
	
	KillAndTrackInDeathTable();
}

bool StarChip::IsInYoshiMouth()
{
	if (flags & (Actor::GOING_TO_YOSHI_MOUTH | Actor::IN_YOSHI_MOUTH | Actor::BEING_SPIT))
		return true;
	
	flags &= ~(Actor::GOING_TO_YOSHI_MOUTH | Actor::IN_YOSHI_MOUTH | Actor::BEING_SPIT);
	
	return false;
}

void StarChip::RegisterEventID()
{
	for (s32 i = 0; i < NUM_COUNTERS; i++)
	{
		if (chipCounters[i].eventID == eventID)
			return;
		
		if (chipCounters[i].eventID == 0xff)
		{
			chipCounters[i].eventID = eventID;
			return;
		}
	}
	
	Crash();
}

StarChip::Counter& StarChip::GetCounter()
{
	for (s32 i = 0; i < NUM_COUNTERS; i++)
	{
		if (chipCounters[i].eventID == eventID)
			return chipCounters[i];
	}
	
	Crash();
}

StarChip::StarChip() {}
StarChip::~StarChip() {}