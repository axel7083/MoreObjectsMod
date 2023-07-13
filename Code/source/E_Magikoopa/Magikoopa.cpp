#include "SM64DS_2.h"
#include "Actors/Custom/Magikoopa.h"

asm("hookInstruction = 0x02017bfc");
extern "C" u32 hookInstruction;

namespace
{
	using StateFunc = void(Magikoopa::*)();
	
	StateFunc states[Magikoopa::NUM_STATES]
	{
		&Magikoopa::St_Appear,
		&Magikoopa::St_Wave,
		&Magikoopa::St_Shoot,
		&Magikoopa::St_Poof,
		&Magikoopa::St_Teleport,
		&Magikoopa::St_Hurt,
		&Magikoopa::St_Wait,
		&Magikoopa::St_Defeat
	};
	
	constexpr Fix12i SCALES[Magikoopa::NUM_TYPES] = { 1._f, 4._f };
	
	constexpr Fix12i RADIUSES[Magikoopa::NUM_TYPES] = { 104._f, 4 * 88._f };
	constexpr Fix12i HEIGHTS [Magikoopa::NUM_TYPES] = { 144._f, 4 * 144._f };
	constexpr Fix12i VERT_ACCEL = 0._f;
	constexpr Fix12i TERM_VEL = Fix12i::min;
	constexpr Fix12i SPIT_DECEL = 1._f;
	
	constexpr Fix12i TP_PLAYER_DIST_TOL = 480._f;
	constexpr Fix12i TP_PLAYER_DIST_MAX = 3072._f;
	constexpr Fix12i BOSS_START_BATTLE_RADIUS = 3000._f;
	constexpr Fix12i BOSS_STOP_BATTLE_RADIUS = 5632._f;
	
	constexpr u16 waitMsgIDs  [Player::NUM_CHARACTERS] = { 0x0096, 0x0097, 0x0098, 0x0099 };
	constexpr u16 defeatMsgIDs[Player::NUM_CHARACTERS] = { 0x009a, 0x009b, 0x009c, 0x009d };
	constexpr Fix12i TALK_HEIGHT = 471._f;
	
	constexpr s32 DAMAGE = 1;
	constexpr Fix12i PLAYER_KNOCKBACK = 12._f;
	constexpr Vector3_16 KNOCKBACK_ROT = { 33.75_deg, 0._deg, 0._deg };
	constexpr s32 NUM__COINS_MAGIKOOPA = 3;
	constexpr Fix12i PLAYER_BOUNCE_INIT_VEL = 40._f;
	
	constexpr u16 APPEAR_TIME = 30;
	constexpr u16 WAVE_TIME = 32;
	constexpr u16 SHOOT_TIME = 12;
	constexpr u16 POOF_TIME = 30;
	constexpr u16 TELEPORT_TIME = 45;
	constexpr u16 HURT_TIME = 32;
	
	constexpr s32 WAND_BONE_ID = 5;
	constexpr Fix12i WAND_LENGTH = 85._f;
	
	constexpr s16 MOST_HORZ_SHOT_ANGLES[Magikoopa::NUM_TYPES] = { -22.5_deg, 22.5_deg };
	
	constexpr u32 SPRITE_ID = 0x2e; //Should be 2e.
	
	constexpr u32 CYL_CLSN_VULN_FLAGS =
		CylinderClsn::HIT_BY_SPIN_OR_GROUND_POUND |
		CylinderClsn::HIT_BY_PUNCH |
		CylinderClsn::HIT_BY_KICK |
		CylinderClsn::HIT_BY_BREAKDANCE |
		CylinderClsn::HIT_BY_SLIDE_KICK |
		CylinderClsn::HIT_BY_DIVE |
		CylinderClsn::HIT_BY_UNK_11 |
		CylinderClsn::HIT_BY_EGG |
		CylinderClsn::HIT_BY_EXPLOSION |
		CylinderClsn::HIT_BY_YOSHI_TONGUE |
		CylinderClsn::HIT_BY_REGURG_GOOMBA |
		CylinderClsn::HIT_BY_FIRE |
		CylinderClsn::HIT_BY_ENEMY |
		CylinderClsn::HIT_BY_INTERACT_WITH_PLAYER;
	
	
	u32 originalInstruction = 0x00000000;
	Magikoopa::SharedRes* ptrToRes = nullptr;
	
	u32 onLoadFuncPtr = (u32)&Magikoopa::SharedRes::OnLoadFile;
}
	
SpawnInfo Magikoopa::spawnData[NUM_TYPES] =
{
	{
		[]() -> ActorBase* { return new Magikoopa; },
		0x00c6,
		0x0018,
		Actor::NO_RENDER_IF_OFF_SCREEN | Actor::UNK_02 | Actor::AIMABLE_BY_EGG,
		50._f,
		70._f,
		4096._f,
		4096._f,
	},
	{
		[]() -> ActorBase* { return new Magikoopa; },
		0x00c6,
		0x0018,
		Actor::NO_RENDER_IF_OFF_SCREEN | Actor::UNK_02 | Actor::AIMABLE_BY_EGG,
		200._f,
		280._f,
		BOSS_STOP_BATTLE_RADIUS,
		BOSS_STOP_BATTLE_RADIUS,
	},
};

SharedFilePtr Magikoopa::modelFiles[NUM_TYPES];
SharedFilePtr Magikoopa::animFiles[NUM_ANIMS];
SharedFilePtr Magikoopa::magicParticleFiles[NUM_TYPES];

Particle::SysDef Magikoopa::magicSysDefs[NUM_TYPES];

void Magikoopa::Resources::Add(SharedFilePtr& sf)
{
	for (s32 i = 0; i < Magikoopa::Resources::NUM_PER_CHUNK; i++)
	{
		if ((u32)&sf == files[i])
			return;
		
		if (files[i] == 0)
		{
			files[i] = (u32)&sf | 1;
			// can't do this here or the Model::SetFile function thinks the resource was already loaded and doesn't offset the offsets
			// files[i]->numRefs++;
			return;
		}
	}
	
	next = new Resources; //TEST THIS
	
	if (next == nullptr)
		Crash();
	
	next->Add(sf);
}

void Magikoopa::Resources::ProcessAdditions()
{
	for (s32 i = 0; i < Magikoopa::Resources::NUM_PER_CHUNK; i++)
	{
		if (files[i] & 1)
		{
			SharedFilePtr& file = *(SharedFilePtr*)(files[i] & ~1);
			if (file.numRefs > 0)
			{
				files[i] &= ~1;
				file.numRefs++;
			}
			else // the resources may have been deallocated because the object failed to spawn, don't reload them!
				files[i] = 0;
		}
	}
	
	if (next)
		next->ProcessAdditions();
}

Magikoopa::Resources::~Resources()
{
	for (s32 i = 0; i < Magikoopa::Resources::NUM_PER_CHUNK; i++)
	{
		if (files[i] != 0)
			((SharedFilePtr*)files[i])->Release();
	}
	
	if (next)
		delete next;
}

void Magikoopa::SharedRes::TrackRes()
{
	ptrToRes = this;
	originalInstruction = hookInstruction;
	hookInstruction = (onLoadFuncPtr - (u32)&hookInstruction) / 4 - 2 & 0x00ffffff | 0xeb000000;
	
	CP15::FlushAndInvalidateDataCache((u32)&hookInstruction, 4);
	CP15::InvalidateInstructionCache((u32)&hookInstruction, 4);
}
void Magikoopa::SharedRes::StopTracking()
{
	hookInstruction = originalInstruction;
	
	CP15::FlushAndInvalidateDataCache((u32)&hookInstruction, 4);
	CP15::InvalidateInstructionCache((u32)&hookInstruction, 4);
}

void Magikoopa::SharedRes::OnLoadFile(SharedFilePtr& file)
{
	ptrToRes->res.Add(file);
}

s32 Magikoopa::InitResources()
{
	res = new SharedRes;
	if (res == nullptr)
		return 0;
		
	res->type = actorID == KAMELLA_ACTOR_ID ? KAMELLA : MAGIKOOPA;
	shotState = 0;
	
	Model::LoadFile(modelFiles[res->type]);
	
	for (s32 i = 0; i < NUM_ANIMS; i++)
		Animation::LoadFile(animFiles[i]);
	
	magicSysDefs[res->type].LoadAndSetFile(magicParticleFiles[res->type]);
	
	if (!modelAnim.SetFile(modelFiles[res->type].BMD(), 1, -1))
		return 0;
	
	if (!shadow.InitCylinder())
		return 0;
	
	modelAnim.SetAnim(animFiles[res->type == KAMELLA ? WAIT : APPEAR].BCA(), Animation::NO_LOOP, 1._f, 0);
		
	cylClsn.Init(this, RADIUSES[res->type], HEIGHTS[res->type], CylinderClsn::ENEMY, CYL_CLSN_VULN_FLAGS);
	
	wmClsn.Init(this, RADIUSES[res->type], HEIGHTS[res->type] >> 1, nullptr, nullptr);
	wmClsn.SetFlag_2();
	
	scale.x = scale.y = scale.z = 1._f;
	
	UpdateModelTransform();
	
	coinType = Enemy::CN_YELLOW;
	numCoinsMinus1 = NUM__COINS_MAGIKOOPA - 1;
	
	vertAccel = VERT_ACCEL;
	termVel = TERM_VEL;
	
	eventID = (param1 >> 8) & 0xff;
	starID = (ang.x >> 12) & 0xf;
	pathPtr.FromID(param1 & 0xff);
	numPathPts = pathPtr.NumNodes();
	currPathPt = numPathPts;
	
	if (starID - 8 >= 0 && starID - 8 <= 6)
		LoadKeyModels(starID - 8);
	
	res->spawnActorID = ang.x & 0xfff;
	res->spawnActorParams = ang.z;
	
	ang.x = ang.z = 0._deg;
	originalPos = pos;
	
	// avoid the glitch where particles mess up if all the whatevers are killed before the Magikoopa spawns a whatever.
	// fire uses only particles, so it doesn't count.
	
	res->TrackRes();
	Actor* newActor;
	
	if (res->type == KAMELLA)
	{
		if ((newActor = Actor::Spawn(GOOMBA_ACTOR_ID, 0xffff, pos, nullptr, areaID, -1)))
			newActor->MarkForDestruction();
	}
	else
	{
		if ((newActor = Actor::Spawn(res->spawnActorID, res->spawnActorParams, pos,  nullptr, 0, -1)))
			newActor->MarkForDestruction();
	}
	
	res->StopTracking();
	res->res.ProcessAdditions();
	
	state = ST_TELEPORT;
	
	health = 3;
	battleStarted = false;
	invincible = true;
	
	return 1;
}

s32 Magikoopa::CleanupResources()
{
	modelFiles[res->type].Release();
	
	for (s32 i = 0; i < NUM_ANIMS; i++)
		animFiles[i].Release();
	
	magicSysDefs[res->type].Release(magicParticleFiles[res->type]);
	
	if (starID - 8 >= 0 && starID - 8 <= 6)
		UnloadKeyModels(starID - 8);
	
	if (res->numRefs == 1)
		delete res;
	else
		res->numRefs--;
	
	return 1;
}

s32 Magikoopa::Behavior()
{
	// loads it the first time this is called
	if (!Particle::Manager::LoadTex("data/custom_obj/magikoopa/magikoopa_magic.spt"ov0, SPRITE_ID))
		return 0;
	
	// will set isBeingSpit to false if the magikoopa runs into ground
	if (UpdateYoshiEat(wmClsn))
	{
		cylClsn.Clear();
		
		modelAnim.SetAnim(animFiles[POOF].BCA(), Animation::NO_LOOP, 1._f, 0);
		modelAnim.currFrame = 0._f;
		speed.y = 0.0003_f;
	
		if (isBeingSpit && spitTimer == 0)
		{
			horzSpeed.ApproachLinear(0._f, SPIT_DECEL);
			cylClsn.Update();
		}
		
		if (horzSpeed == 0._f || !isBeingSpit)
		{
			horzSpeed = 0._f;
			isBeingSpit = false;
			state = ST_POOF;
			stateTimer = 0;
		}
		
		UpdateModelTransform();
		
		return 1;
	}
	
	if (defeatMethod != Enemy::DF_NOT)
	{
		s32 res = UpdateKillByInvincibleChar(wmClsn, modelAnim, 3);
		
		if (res == 2) //finished kill
		{
			KillMagikoopa();
		}
		else if (res == 0) //not yet
		{
			UpdateDeath(wmClsn);
			UpdateModelTransform();
		}
		
		return 1;
	}
	
	u8 prevState = state;
	
	(this->*states[state])();
	
	stateTimer++;
	if (state != prevState)
		stateTimer = 0;
	
	Player* player = ClosestPlayer();
	
	if (res->type == KAMELLA && battleStarted && (!player || originalPos.Dist(player->pos) >= BOSS_STOP_BATTLE_RADIUS) && health != 0)
	{
		state = ST_TELEPORT;
		stateTimer = 0;
		battleStarted = false;
		invincible = true;
		health = 3;
		shotState = 0;
		currPathPt = numPathPts;
		
		Sound::StopLoadedMusic_Layer3();
	}
	
	if (state == ST_TELEPORT)
	{
		if (stateTimer >= TELEPORT_TIME)
		{
			if (currPathPt < numPathPts)
			{
				nextPathPt = (u32)RandomInt() % (numPathPts - 1);
				if (nextPathPt >= currPathPt)
					nextPathPt++;
			}
			else
			{
				nextPathPt = 0;
			}
			
			pos = pathPtr.GetNode(nextPathPt);
		}
		return 1;
	}
	
	UpdatePos(nullptr);
	UpdateModelTransform();
	
	Vector3 wandTip = GetWandTipPos();
	particleID = Particle::System::New(particleID, magicSysDefs[res->type], wandTip.x, wandTip.y, wandTip.z, nullptr, nullptr);
	
	HandleClsn(); //must be done before clearing collision, of course
	
	cylClsn.Clear();
	if (defeatMethod == Enemy::DF_NOT)
		cylClsn.Update();
	
	MakeVanishLuigiWork(cylClsn);
	
	modelAnim.Advance();
	
	return 1;
}

s32 Magikoopa::Render()
{
	if (state == ST_TELEPORT || (flags & Actor::IN_YOSHI_MOUTH))
		return 1;
		
	modelAnim.Render(&scale);
	return 1;
}

void Magikoopa::OnPendingDestroy()
{
	if (eventID < NUM_EVENTS)
		Event::SetBit(eventID);
}

u32 Magikoopa::OnYoshiTryEat()
{
	return state == ST_TELEPORT || res->type == KAMELLA ? Actor::YE_DONT_EAT : Actor::YE_KEEP_AND_CAN_MAKE_EGG;
}

void Magikoopa::OnTurnIntoEgg(Player& player)
{
	if (player.IsCollectingCap())
		GivePlayerCoins(player, NUM__COINS_MAGIKOOPA, 0);
	else
		player.RegisterEggCoinCount(NUM__COINS_MAGIKOOPA, false, false);
	
	KillMagikoopa();
}

Fix12i Magikoopa::OnAimedAtWithEgg()
{
	return HEIGHTS[res->type] >> 1;
}

void Magikoopa::UpdateModelTransform()
{
	modelAnim.mat4x3 = Matrix4x3::RotationY(ang.y);
	modelAnim.mat4x3.ApplyScale(SCALES[res->type], SCALES[res->type], SCALES[res->type]);
	modelAnim.mat4x3.c3 = pos >> 3;
	
	if (!(flags & Actor::IN_YOSHI_MOUTH))
		DropShadowRadHeight(shadow, modelAnim.mat4x3, RADIUSES[0], 800._f / SCALES[res->type], 15);
}

void Magikoopa::KillMagikoopa()
{
	//The coins have already spawned.
	KillAndTrackInDeathTable();
}

void Magikoopa::HandleClsn()
{
	if (cylClsn.otherObjID == 0)
		return;
	
	Actor* otherObj = Actor::FindWithID(cylClsn.otherObjID);
	
	if (otherObj == nullptr)
		return;
	
	u32 hitFlags = cylClsn.hitFlags;
	
	if (res->type == KAMELLA)
	{
		if ((hitFlags & CylinderClsn::HIT_BY_EGG) && !invincible)
		{
			health--;
			state = ST_HURT;
			stateTimer = 0;
			invincible = true;
			return;
		}
		
		if (otherObj->actorID != PLAYER_ACTOR_ID)
			return;
	
		Player& player = *(Player*)otherObj;
		
		if ((hitFlags & CylinderClsn::HIT_BY_PLAYER) && state != ST_HURT)
			player.Hurt(pos, DAMAGE, PLAYER_KNOCKBACK, 1, 0, 1);
		
		return;
	}
	
	if (hitFlags & (CylinderClsn::HIT_BY_PUNCH      | CylinderClsn::HIT_BY_KICK |
	                CylinderClsn::HIT_BY_BREAKDANCE | CylinderClsn::HIT_BY_SLIDE_KICK |
					CylinderClsn::HIT_BY_DIVE       | CylinderClsn::HIT_BY_EGG |
					CylinderClsn::HIT_BY_EXPLOSION  | CylinderClsn::HIT_BY_REGURG_GOOMBA |
					CylinderClsn::HIT_BY_FIRE))
	{
		if (otherObj->actorID == PLAYER_ACTOR_ID)
		{
			KillByInvincibleChar(KNOCKBACK_ROT, *(Player*)otherObj);
		}
		else
		{
			SpawnCoin();
			KillMagikoopa();
		}
		
		return;
	}
	else if (hitFlags & CylinderClsn::HIT_BY_SPIN_OR_GROUND_POUND)
	{
		defeatMethod = Enemy::DF_SQUASHED;
		KillByAttack(*otherObj, wmClsn);
		
		scale.y = 0.3333_f;
		
		return;
	}
	
	if (otherObj->actorID != PLAYER_ACTOR_ID)
		return;
	
	Player& player = *(Player*)otherObj;
	
	if (JumpedOnByPlayer(cylClsn, player))
	{
		defeatMethod = Enemy::DF_SQUASHED;
		KillByAttack(player, wmClsn);
		
		scale.y = 0.3333_f;
		
		player.Bounce(PLAYER_BOUNCE_INIT_VEL);
	}
	else if (hitFlags & CylinderClsn::HIT_BY_MEGA_CHAR)
	{
		SpawnMegaCharParticles(player, nullptr);
		
		Sound::Play("NCS_SE_SCT_HIT_SMALL"sfx, camSpacePos);
		
		KillByInvincibleChar(KNOCKBACK_ROT, player);
	}
	else if (player.IsOnShell() || player.isMetalWario)
	{
		KillByInvincibleChar(KNOCKBACK_ROT, player);
	}
	else if (hitFlags & CylinderClsn::HIT_BY_PLAYER)
	{
		player.Hurt(pos, DAMAGE, PLAYER_KNOCKBACK, 1, 0, 1);
	}
}

Vector3 Magikoopa::GetWandTipPos()
{
	Matrix4x3 wandMat = modelAnim.mat4x3(modelAnim.data.transforms[WAND_BONE_ID]);
	Vector3 tipOffset = { 0._f >> 3, WAND_LENGTH >> 3, 0._f >> 3 };
	
	return wandMat(tipOffset) << 3;
}

void Magikoopa::AttemptTalkStartIfNotStarted()
{
	Player* player = ClosestPlayer();
	
	if (player->StartTalk(*this, true))
	{
		Message::PrepareTalk();
		listener = player;
	}
}

void Magikoopa::Talk()
{
	if (!listener)
		return;
	
	s32 talkState = listener->GetTalkState();
	
	switch (talkState)
	{
		case Player::TK_NOT:
		{
			Message::EndTalk();
			listener = nullptr;
			
			if (state == ST_DEFEAT)
			{
				KillMagikoopa();
				
				Sound::StopLoadedMusic_Layer3();
				
				Vector3 starPos = { pos.x, pos.y + 250._f, pos.z };
				
				if (starID < 8)
					Actor::Spawn(POWER_STAR_ACTOR_ID, (0x0040 + starID), starPos, nullptr, 0, -1);
				else if (starID < 15)
					Actor::Spawn(KEY_ACTOR_ID, (0x0000 + starID - 8), starPos, nullptr, 0, -1);
			}
			else
			{
				state = ST_WAVE;
				Sound::LoadAndSetMusic_Layer3(45);
			}
			
			break;
		}
		case Player::TK_START:
		{
			u16 msgID = state == ST_DEFEAT ? defeatMsgIDs[listener->param1] : waitMsgIDs[listener->param1];
			Vector3 lookAt = { pos.x, pos.y + TALK_HEIGHT, pos.z };
			
			listener->ShowMessage(*this, msgID, lookAt, 0, 0);
			
			break;
		}
	}
}

void Magikoopa::St_Appear()
{
	modelAnim.SetAnim(animFiles[APPEAR].BCA(), Animation::NO_LOOP, 1._f, 0);
	
	if (res->type == MAGIKOOPA)
		battleStarted = true;
	
	// the boss should not be hit before talking
	if (battleStarted)
		invincible = false;
	else if (res->type == KAMELLA)
		AttemptTalkStartIfNotStarted();
	
	if (stateTimer == 0)
	{
		Player* player = ClosestPlayer();
		if (player)
			ang.y = pos.HorzAngle(player->pos);
	}
	
	if (stateTimer >= APPEAR_TIME - 1)
		state = (res->type == KAMELLA && !battleStarted) ? ST_WAIT : ST_WAVE;
}

void Magikoopa::St_Wave()
{
	modelAnim.SetAnim(animFiles[WAVE].BCA(), Animation::LOOP, 1._f, 0);
	
	battleStarted = true;
	invincible = false;
	
	if (stateTimer >= WAVE_TIME - 1)
		state = ST_SHOOT;
}

void Magikoopa::St_Shoot()
{
	modelAnim.SetAnim(animFiles[SHOOT].BCA(), Animation::NO_LOOP, 1._f, 0);
	
	if (stateTimer == 2 && !(flags & Actor::OFF_SCREEN))
	{
		s32 shotID = -1;
		
		if (res->type == MAGIKOOPA || shotState != 0)
		{
			for (s32 i = 0; i < 3; i++)
			{
				if (!Actor::FindWithID(res->shotUniqueIDs[i]))
				{
					shotID = i;
					break;
				}
			}
		}
		
		if (shotID >= 0 || (res->type == KAMELLA && shotState == 0))
		{	
			Vector3 shotPos = GetWandTipPos();
			shotPos.y -= SHOT_RADIUS;
			
			Player* player = ClosestPlayer();
			
			// & 0xc000 because of that silly 0x8000 case
			s16 vertAng = -30._deg;
			if (player != nullptr)
				vertAng = (AngleDiff(pos.HorzAngle(player->pos), ang.y) & 0xc000) ? -90._deg : -cstd::abs(player->pos.VertAngle(shotPos));
			
			if (vertAng < -90._deg)
				vertAng = -90._deg;
			if (vertAng > MOST_HORZ_SHOT_ANGLES[res->type])
				vertAng = MOST_HORZ_SHOT_ANGLES[res->type];
			
			Vector3_16 shotDir = { (s16)(Sin(ang.y) * Cos(vertAng)).val,
							                         (Sin(vertAng)).val,
							       (s16)(Cos(ang.y) * Cos(vertAng)).val };
			
			MagikoopaShot* shot = (MagikoopaShot*)Actor::Spawn(MAGIKOOPA_SHOT_ACTOR_ID, shotID, shotPos, &shotDir, areaID, -1);
			
			if (shot != nullptr)
			{
				shot->SetMagikoopa(*this);
				
				if (shotID >= 0)
					res->shotUniqueIDs[shotID] = shot->uniqueID;
				
				shotState ^= 1;
			}
		}
	}
	
	if (stateTimer >= SHOOT_TIME - 1)
		state = ST_POOF;
}

void Magikoopa::St_Poof()
{
	modelAnim.SetAnim(animFiles[POOF].BCA(), Animation::NO_LOOP, 1._f, 0);
	
	if (stateTimer >= POOF_TIME - 1)
		state = ST_TELEPORT;
}

void Magikoopa::St_Teleport()
{
	Vector3 dustPos = { pos.x, pos.y + (HEIGHTS[res->type] >> 1), pos.z };
	
	if (stateTimer == 0)
	{
		if (battleStarted)
			DisappearPoofDustAt(dustPos);
		
		flags &= ~Actor::AIMABLE_BY_EGG;
	}
	
	if (stateTimer >= TELEPORT_TIME || !battleStarted)
	{
		stateTimer = TELEPORT_TIME; // prevent overflow
		
		Player* player = ClosestPlayer();
		
		if (res->type == MAGIKOOPA || battleStarted)
		{
			if ((flags & Actor::OFF_SCREEN) || (player != nullptr && (pos.Dist(player->pos) <= TP_PLAYER_DIST_TOL ||
				(res->type == MAGIKOOPA && pos.Dist(player->pos) > TP_PLAYER_DIST_MAX))))
			{
				return;
			}
		}
		else if (player == nullptr || originalPos.Dist(player->pos) > BOSS_START_BATTLE_RADIUS || player->currClsnState != 1)
			return;
		
		currPathPt = nextPathPt;
		
		PoofDustAt(dustPos);
		
		flags |= Actor::AIMABLE_BY_EGG;
		state = ST_APPEAR;
	}
}

void Magikoopa::St_Hurt()
{
	modelAnim.SetAnim(animFiles[HURT].BCA(), Animation::NO_LOOP, 1._f, 0);
	
	if (health == 0)
		AttemptTalkStartIfNotStarted();
	
	if (stateTimer >= HURT_TIME)
		state = health == 0 ? ST_DEFEAT : ST_POOF;
}

void Magikoopa::St_Wait()
{
	modelAnim.SetAnim(animFiles[WAIT].BCA(), Animation::LOOP, 1._f, 0);
	
	Talk();
	
	if (state == ST_WAIT)
		AttemptTalkStartIfNotStarted();
}

void Magikoopa::St_Defeat()
{
	modelAnim.SetAnim(animFiles[DEFEAT].BCA(), Animation::NO_LOOP, 1._f, 0);
	
	Talk();
	
	if (!shouldBeKilled)
		AttemptTalkStartIfNotStarted();
}

Magikoopa::Magikoopa() {}
Magikoopa::~Magikoopa() {}