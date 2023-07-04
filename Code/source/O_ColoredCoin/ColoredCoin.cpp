#include "SM64DS_2.h"
#include "Actors/Custom/ColoredCoin.h"

namespace
{
	constexpr Fix12i RADIUS = 100._f;
	constexpr Fix12i HEIGHT = 100._f;
	
	constexpr s16 ROT_SPEED = 16.875_deg;
}

SharedFilePtr ColoredCoin::modelFile;

SpawnInfo ColoredCoin::spawnData =
{
	[]() -> ActorBase* { return new ColoredCoin; },
	0x0030,
	0x0100,
	Actor::NO_RENDER_IF_OFF_SCREEN,
	100._f,
	200._f,
	4096._f,
	4096._f
};

s32 ColoredCoin::InitResources()
{
	if (actorID == SILVER_COIN_ACTOR_ID)
	{
		value = 1;
		health = 0;
		hurt = false;
		fake = false;
		deathCoin = false;
		deathFrames = 0;
		starID = 0;
	}
	else if (actorID == COLORED_COIN_ACTOR_ID)
	{
		value = ang.x & 0xff;
		health = (ang.x >> 8) & 0xf;
		health = health > 8 ? 8 : health;
		hurt = (ang.x >> 12 & 0xf) == 1;
		fake = value == 0;
		deathCoin = ang.z != 0;
		deathFrames = ang.z & 0x0fff;
		starID = ang.z & 0xf000;
	}
	
	frameCounter = 0;
	deathStarted = false;
	
	Model::LoadFile(modelFile);
	model.SetFile(modelFile.BMD(), 1, -1);
	
	if (actorID == COLORED_COIN_ACTOR_ID)
		model.data.materials[0].difAmb = (param1 & 0x7fff) << 16 | 0x8000;
	
	shadow.InitCylinder();
	
	UpdateModelTransform();
	
	shadowMat = model.mat4x3;
	shadowMat.c3.y -= 1.3125_f;
	
	cylClsn.Init(this, RADIUS, HEIGHT, CylinderClsn::INTANGIBLE | CylinderClsn::COLLECTABLE, CylinderClsn::HIT_BY_YOSHI_TONGUE);
	
	return 1;
}

s32 ColoredCoin::CleanupResources()
{
	modelFile.Release();
	
	return 1;
}

s32 ColoredCoin::Behavior()
{
	ang.y += ROT_SPEED;
	
	UpdateModelTransform();
	CheckCylClsn();
	
	cylClsn.Clear();
	if (!deathStarted)
		cylClsn.Update();
	
	if (deathStarted)
	{
		frameCounter++;
		
		if (killer->pos == spawnedStar->pos)
		{
			deathStarted = false;
			KillAndTrackInDeathTable();
		}
		else if (frameCounter == deathFrames)
		{
			if (HEALTH_ARR[0] >= 0x100)
				HEALTH_ARR[0] -= 0x100;
			
			if (HEALTH_ARR[0] == 0)
			{
				ClosestPlayer()->Hurt(pos, 1, 12._f, 1, 0, 1);
				deathStarted = false;
				KillAndTrackInDeathTable();
			}
			
			frameCounter = 0;
		}
	}
	
	return 1;
}

s32 ColoredCoin::Render()
{
	if (!deathStarted)
		model.Render(nullptr);
	
	return 1;
}

void ColoredCoin::UpdateModelTransform()
{
	model.mat4x3 = Matrix4x3::RotationY(ang.y);
	model.mat4x3.c3 = pos >> 3;
	
	if (!deathStarted)
		DropShadowRadHeight(shadow, shadowMat, RADIUS, 512._f, 15);
}

void ColoredCoin::CheckCylClsn()
{
	Actor* otherObj = Actor::FindWithID(cylClsn.otherObjID);
	
	if (otherObj == nullptr || otherObj->actorID != PLAYER_ACTOR_ID)
		return;
	
	Player& player = *(Player*)otherObj;
	
	if (!(cylClsn.hitFlags & CylinderClsn::HIT_BY_YOSHI_TONGUE))
		Kill(player);
}

void ColoredCoin::OnTurnIntoEgg(Player& player)
{
	Kill(player);
}

u32 ColoredCoin::OnYoshiTryEat()
{
	return Actor::YE_SWALLOW;
}

void ColoredCoin::Kill(Player& player)
{
	killer = &player;
	
	if (deathCoin)
	{
		deathStarted = true;
		spawnedStar = nullptr;
		
		cylClsn.Clear();
		
		Actor* starMarker = FindWithActorID(STAR_MARKER_ACTOR_ID, nullptr);
		
		while (starMarker != nullptr)
		{
			if (starMarker->param1 == 0x20 + starID && spawnedStar == nullptr)
				spawnedStar = Actor::Spawn(POWER_STAR_ACTOR_ID, 0x0040 + starID, starMarker->pos, nullptr, areaID, -1);
			
			starMarker = FindWithActorID(STAR_MARKER_ACTOR_ID, starMarker);
		}
		
		Particle::System::NewSimple(0xd2, pos.x, pos.y + 40._f, pos.z);
		Sound::Play("NCS_SE_SCT_COIN_OUTDOOR"sfx, camSpacePos);
		
		return;
	}
	
	KillAndTrackInDeathTable();
	
	if (fake)
	{
		Particle::System::NewSimple(0xb, pos.x, pos.y + 40._f, pos.z);
		Sound::Play("SE_EMY_TERESA"sfx, camSpacePos);
	}
	else
	{
		Particle::System::NewSimple(0xd2, pos.x, pos.y + 40._f, pos.z);
		Sound::Play("NCS_SE_SCT_COIN_OUTDOOR"sfx, camSpacePos);
	}
	
	if (hurt)
		player.Hurt(pos, health, 12._f, 1, 0, 1);
	else
		player.Heal(health << 8);
	
	GiveCoins(player.playerID, value);
}

ColoredCoin::ColoredCoin() {}
ColoredCoin::~ColoredCoin() {}