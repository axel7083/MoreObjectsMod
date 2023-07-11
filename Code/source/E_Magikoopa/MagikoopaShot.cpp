#include "SM64DS_2.h"
#include "Actors/Custom/Magikoopa.h"

namespace
{
	constexpr Fix12i SHOT_SPEEDS[Magikoopa::NUM_TYPES] = { 8._f, 32._f };
	constexpr u16 SHOT_MAX_OFFSCREEN_TIME = 45;
}

SpawnInfo MagikoopaShot::spawnData =
{
	[]() -> ActorBase* { return new MagikoopaShot; },
	0x016c,
	0x0018,
	Actor::NO_RENDER_IF_OFF_SCREEN | Actor::UNK_02,
	50._f,
	70._f,
	4096._f,
	4096._f,
};

s32 MagikoopaShot::InitResources()
{
	resourceRefCount = 0;
	
	direction = Vector3_16f{Fix12s(ang.x, as_raw), Fix12s(ang.y, as_raw), Fix12s(ang.z, as_raw)};
	ang.x = ang.z = 0._deg;
	ang.y = cstd::atan2(direction.x, direction.z);
	
	cylClsn.Init(this, SHOT_RADIUS, SHOT_RADIUS * 2, CylinderClsn::ENEMY, 0);
	
	wmClsn.Init(this, SHOT_RADIUS, SHOT_RADIUS, nullptr, nullptr);
	wmClsn.SetFlag_2();
	
	stateTimer = 0;
	
	return 1;
}

s32 MagikoopaShot::CleanupResources()
{
	if (res->numRefs == 1)
		delete res;
	else
		res->numRefs--;
	
	Magikoopa::particleSysDefs[res->type].UnloadFile(Magikoopa::particleResFiles[res->type]);
	
	return 1;
}

s32 MagikoopaShot::Behavior()
{
	UpdatePosWithOnlySpeed(nullptr);
	UpdateWMClsn(wmClsn, 0);
	
	particleID = Particle::System::New(particleID, (u32)&Magikoopa::particleSysDefs[res->type], pos.x, pos.y + SHOT_RADIUS, pos.z, &direction, nullptr);
	
	Actor* otherObj = Actor::FindWithID(cylClsn.otherObjID);
	
	if (wmClsn.IsOnGround() || wmClsn.IsOnWall() || /*ADD CHECK FOR CEILING*/
		(otherObj != nullptr && otherObj->actorID != MAGIKOOPA_ACTOR_ID && otherObj->actorID != KAMELLA_ACTOR_ID))
	{
		res->TrackRes();
		
		Actor* newActor = nullptr;
		
		if (res->type == Magikoopa::MAGIKOOPA)
		{
			newActor = Actor::Spawn(res->spawnActorID, res->spawnActorParams, pos, &ang, areaID, -1);
		}
		else if (shotState == 0)
		{
			Vector3_16 fireAngle = { 22.5_deg, ang.y, 0._deg };
			
			for (s32 i = 0; i < numFireToSpawn; ++i)
			{
				newActor = Actor::Spawn(FIREBALL_ACTOR_ID, 0x0003, pos, &fireAngle, areaID, -1);
				
				if (newActor != nullptr)
				{
					newActor->horzSpeed = 30._f;
					*(Fix12i*)((char*)newActor + 0x35c) = 10._f; // timer, change this when struct is documented
				}
				
				fireAngle.y += 0x10000 / numFireToSpawn;
			}
		}
		else
		{
			newActor = Actor::Spawn(GOOMBA_ACTOR_ID, 0xffff, pos, &ang, areaID, -1);
		}
		
		res->StopTracking();
		res->res.ProcessAdditions();
		
		if (newActor != nullptr && param1 >= 0)
			res->shotUniqueIDs[param1] = newActor->uniqueID;
		
		Vector3 dustPos = { pos.x, pos.y + SHOT_RADIUS, pos.z };
		
		PoofDustAt(dustPos);
		MarkForDestruction();
	}
	
	if (flags & Actor::OFF_SCREEN)
	{
		stateTimer++;
		if (stateTimer >= SHOT_MAX_OFFSCREEN_TIME)
			MarkForDestruction();
	}
	else
	{
		stateTimer = 0;
	}
	
	cylClsn.Clear();
	cylClsn.Update();
	
	MakeVanishLuigiWork(cylClsn);
	
	return 1;
}

s32 MagikoopaShot::Render()
{
	return 1;
}

void MagikoopaShot::SetMagikoopa(Magikoopa& magikoopa)
{
	res = magikoopa.res;
	shotState = magikoopa.shotState;
	numFireToSpawn = 4 - magikoopa.health;
	res->numRefs++;
	
	speed.x = (Fix12i)direction.x * SHOT_SPEEDS[res->type];
	speed.y = (Fix12i)direction.y * SHOT_SPEEDS[res->type];
	speed.z = (Fix12i)direction.z * SHOT_SPEEDS[res->type];
	
	Magikoopa::particleSysDefs[res->type].LoadAndSetFile(Magikoopa::particleResFiles[res->type]);
}

MagikoopaShot::MagikoopaShot() {}
MagikoopaShot::~MagikoopaShot() {}