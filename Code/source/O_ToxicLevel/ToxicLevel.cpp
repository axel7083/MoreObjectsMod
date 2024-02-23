#include "SM64DS_2.h"
#include "Actors/Custom/ToxicLevel.h"

SpawnInfo ToxicLevel::spawnData =
{
	[]() -> ActorBase* { return new ToxicLevel; },
	0x0004,
	0x0007,
	0,
	0._f,
	Fix12i::max,
	Fix12i::max,
	Fix12i::max,
};

s32 ToxicLevel::InitResources()
{
	frameForDamage = param1;
	toxicCounter = 0;
	
	return 1;
}

s32 ToxicLevel::CleanupResources()
{
	return 1;
}

s32 ToxicLevel::Behavior()
{						
	if (GAME_PAUSED != 0)
		return 1;
	
	if (toxicCounter >= frameForDamage)
	{
		if (HEALTH_ARR[0] > 0x100)
			HEALTH_ARR[0] -= 0x100;
		else
			PLAYER_ARR[0]->Hurt(PLAYER_ARR[0]->pos, 1, 12._f, 1, 0, 1);
		
		toxicCounter = 0;
	}
	else if (PLAYER_ARR[0]->GetTalkState() != Player::TK_TALKING)
	{
		toxicCounter++;
	}
	
	return 1;
}

s32 ToxicLevel::Render()
{
	return 1;
}

ToxicLevel::ToxicLevel() {}
ToxicLevel::~ToxicLevel() {}