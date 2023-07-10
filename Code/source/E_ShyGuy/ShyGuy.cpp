#include "SM64DS_2.h"
#include "Actors/Custom/ShyGuy.h"

namespace
{
	using StateFunc = void(ShyGuy::*)();
	
	const StateFunc stateFuncs[]
	{
		&ShyGuy::State0_Wait,
		&ShyGuy::State1_Turn,
		&ShyGuy::State2_Chase,
		&ShyGuy::State3_Stop,
		&ShyGuy::State4_Warp
	};
	
	char material0Name[] = "mat_body-material";
	char material1Name[] = "color-material";
	
	u8 matVals[] = { 0x1f, 0x1d, 0x1a, 0x18, 0x15, 0x13, 0x10, 0x00 };
	
	BMA_File::MaterialProperties matProps[] =
	{
		{
			0xffff, 0,
			&material0Name[0],
			{ 0x01, false, 0x0000 },   { 0x01, false, 0x0000 },   { 0x01, false, 0x0000 },
			{ 0x01, false, 0x0006 },   { 0x01, false, 0x0006 },   { 0x01, false, 0x0006 }, 
			{ 0x01, false, 0x0007 },   { 0x01, false, 0x0007 },   { 0x01, false, 0x0007 },
			{ 0x01, false, 0x0007 },   { 0x01, false, 0x0007 },   { 0x01, false, 0x0007 },
			{ 0x01, true , 0x0000 },
		},
		{
			0xffff, 0,
			&material1Name[0],
			{ 0x01, false, 0x0000 },   { 0x01, false, 0x0000 },   { 0x01, false, 0x0000 },
			{ 0x01, false, 0x0006 },   { 0x01, false, 0x0006 },   { 0x01, false, 0x0006 }, 
			{ 0x01, false, 0x0007 },   { 0x01, false, 0x0007 },   { 0x01, false, 0x0007 },
			{ 0x01, false, 0x0007 },   { 0x01, false, 0x0007 },   { 0x01, false, 0x0007 },
			{ 0x01, true , 0x0000 },
		},
	};
	
	BMA_File matAnimFile = { 7, 0x0000, &matVals[0], 2, &matProps[0] };
	
	constexpr Vector3 FORWARD_VEC = { 0._f, 0._f, 1._f };
	
	constexpr Fix12i RADIUS = 80._f;
	constexpr Fix12i HEIGHT = 160._f;
	constexpr Fix12i VERT_ACCEL = -2._f;
	constexpr Fix12i TERM_VEL = -50._f;
	
	constexpr s32 DAMAGE = 1;
	constexpr Fix12i PLAYER_KNOCKBACK = 12._f;
	constexpr Vector3_16 KNOCKBACK_ROT = { 33.75_deg, 0._deg, 0._deg };
	constexpr s32 NUM_COINS_TO_DROP = 2;
	constexpr Fix12i PLAYER_BOUNCE_INIT_VEL = 40._f;
	
	constexpr u16 WAIT_TIME = 45;
	constexpr s16 TURN_SPEED = 5.625_deg;
	constexpr Fix12i SIGHT_COS_RADIUS = 0.8659668_f; // cos(30 degrees)
	constexpr Fix12i SIGHT_DIST = 1200._f;
	
	constexpr Fix12i WALK_SPEED = 8._f;
	constexpr Fix12i WARP_SPEED = 32._f;
	constexpr Fix12i TARGET_POS_TOLERANCE = 8._f;
	constexpr Fix12i TARGET_POS_WARP_TOL = 32._f;
	constexpr Fix12i CLIFF_TOLERANCE = 50._f;
	constexpr Fix12i HEIGHT_TOLERANCE = HEIGHT >> 1;
	
	constexpr Fix12i CHASE_SPEED = 16._f;
	constexpr Fix12i CHASE_ACCEL = 0.5_f;
	constexpr s16 CHASE_TURN_SPEED = 14.065_deg;
	constexpr Fix12i QUIT_CHASE_DIST = 1200._f;
	constexpr u8 CHASE_COOLDOWN = 30;
	
	constexpr u8 GIVE_UP_TIMER = 150;
	
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
}

SpawnInfo ShyGuy::spawnData =
{
	[]() -> ActorBase* { return new ShyGuy; },
	0x016b,
	0x0018,
	Actor::NO_RENDER_IF_OFF_SCREEN | Actor::UNK_02 | Actor::AIMABLE_BY_EGG,
	50._f,
	70._f,
	4096._f,
	4096._f
};

SharedFilePtr ShyGuy::modelFile;
SharedFilePtr ShyGuy::animFiles[NUM_ANIMS];

s32 ShyGuy::InitResources()
{
	Model::LoadFile(modelFile);
	
	for (s32 i = 0; i < NUM_ANIMS; i++)
		Animation::LoadFile(animFiles[i]);
	
	if (!modelAnim.SetFile(modelFile.BMD(), 1, -1))
		return 0;
	
	if (!shadow.InitCylinder())
		return 0;
	
	modelAnim.SetAnim(animFiles[WAIT].BCA(), Animation::LOOP, 1._f, 0);
	
	MaterialChanger::Prepare(*modelFile.BMD(), matAnimFile);
	matChg.SetFile(matAnimFile, Animation::NO_LOOP, 1._f, 0);
		
	cylClsn.Init(this, RADIUS, HEIGHT, CylinderClsn::ENEMY, CYL_CLSN_VULN_FLAGS);
	
	wmClsn.Init(this, RADIUS, HEIGHT >> 1, nullptr, nullptr);
	
	UpdateModelTransform();
	
	scale.x = scale.y = scale.z = 1._f;
	coinType = Enemy::CN_YELLOW;
	numCoinsMinus1 = NUM_COINS_TO_DROP - 1;
	
	targetAngle = ang.y;
	vertAccel = VERT_ACCEL;
	termVel = TERM_VEL;
	
	state = ST_WAIT;
	chaseCooldown = 0;
	targetPlayer = nullptr;
	
	offTrack = false;
	backAndForth = (ang.x & 0xf) > 0;
	customColor = ((ang.x >> 4) & 0xf) > 0;

	if (param1 != 0xffff)
	{
		reverse = false;
		nextPathNode = 1;
		
		pathPtr.FromID(param1);
		numPathNodes = pathPtr.NumNodes();
		pathPtr.GetNode(pos, 0);
		
		SetTargetPos();
	}
	else
	{
		targetPos = Vector3{ pos.x, FloorY(pos), pos.z };
	}
	
	color = (u16)ang.z | 0x8000 | (ang.z >>  0 & 0x1f) >> 1 << 16
	                            | (ang.z >>  5 & 0x1f) >> 1 << 21
	                            | (ang.z >> 10 & 0x1f) >> 1 << 26;
	
	ang.x = ang.z = 0._deg;
	
	return 1;
}

s32 ShyGuy::CleanupResources()
{
	modelFile.Release();
	
	for (s32 i = 0; i < NUM_ANIMS; i++)
		animFiles[i].Release();
	
	return 1;
}

s32 ShyGuy::Behavior()
{
	if (defeatMethod != Enemy::DF_NOT)
	{
		s32 res = UpdateKillByInvincibleChar(wmClsn, modelAnim, 3);
		
		if (res == 2) //finished kill
		{
			Kill();
		}
		else if (res == 0) //not yet
		{
			UpdateDeath(wmClsn);
			UpdateModelTransform();
		}
		
		return 1;
	}
	
	s32 eatState = UpdateYoshiEat(wmClsn);
	if (eatState == Enemy::UY_NOT || isBeingSpit)
	{
		if (KillIfTouchedBadSurface())
			return 1;
	}
	
	if (eatState != Enemy::UY_NOT)
	{
		cylClsn.Clear();
		if (isBeingSpit && spitTimer == 0)
			cylClsn.Update();
		
		UpdateModelTransform();
		
		if (isBeingSpit && wmClsn.JustHitGround())
		{
			isBeingSpit = false;
			state = ST_STOP;
		}
		
		return 1;
	}
	
	s32 prevState = state;
	
	(this->*stateFuncs[state])();
	
	stateTimer++;
	if (state != prevState)
		stateTimer = 0;
	
	bool cooledDown = DecIfAbove0_Byte(chaseCooldown) == 0;
	
	if ((state == ST_WAIT || state == ST_TURN) && cooledDown && !(flags & Actor::OFF_SCREEN))
	{
		targetPlayer = PlayerVisibleToThis(nullptr);
		
		if (targetPlayer)
			state = ST_CHASE;
	}
	
	if (state != ST_TURN)
		alarmed = false;
	
	UpdatePos(nullptr);
	UpdateModelTransform();
	
	if (state == ST_WARP)
	{
		cylClsn.Clear();
		return 1;
	}
	
	matChg.currFrame.ApproachLinear(0._f, -1._f);
	
	UpdateWMClsn(wmClsn, 2);
	CheckCylClsn();
	
	cylClsn.Clear();
	if (defeatMethod == Enemy::DF_NOT)
		cylClsn.Update();
	
	MakeVanishLuigiWork(cylClsn);
	PlayMovingSoundEffect();
	
	modelAnim.Advance();
	
	return 1;
}

s32 ShyGuy::Render()
{
	if (flags & Actor::IN_YOSHI_MOUTH)
		return 1;
	
	matChg.Update(modelAnim.data);
	
	if (customColor)
		modelAnim.data.materials[1].difAmb = color; //custom color
	else if (pathPtr.ptr != nullptr)
		modelAnim.data.materials[1].difAmb = 0x0010801f; //red
	else
		modelAnim.data.materials[1].difAmb = 0x4100fe00; //blue
	
	modelAnim.Render(&scale);
	
	return 1;
}

u32 ShyGuy::OnYoshiTryEat()
{
	return state == ST_WARP ? Actor::YE_DONT_EAT : Actor::YE_KEEP_AND_CAN_MAKE_EGG;
}

void ShyGuy::OnTurnIntoEgg(Player& player)
{
	if (player.IsCollectingCap())
		GivePlayerCoins(player, NUM_COINS_TO_DROP, 0);
	else
		player.RegisterEggCoinCount(NUM_COINS_TO_DROP, false, false);
	
	MarkForDestruction();
}

Fix12i ShyGuy::OnAimedAtWithEgg()
{
	return HEIGHT / 2;
}

void ShyGuy::UpdateModelTransform()
{
	modelAnim.mat4x3 = Matrix4x3::RotationY(ang.y);
	modelAnim.mat4x3.c3 = pos >> 3;
	
	if (!(flags & Actor::IN_YOSHI_MOUTH))
		DropShadowRadHeight(shadow, modelAnim.mat4x3, RADIUS, wmClsn.IsOnGround() ? 30._f : 150._f, 15);
}

void ShyGuy::SetTargetPos()
{
	pathPtr.GetNode(targetPos, nextPathNode);
	
	targetPos.y = FloorY(targetPos);
	targetAngle = pos.HorzAngle(targetPos);
}

void ShyGuy::Kill()
{
	Sound::Play("NCS_SE_SCT_KRB_DOWN"sfx, camSpacePos);
	
	KillAndTrackInDeathTable();
}

void ShyGuy::CheckCylClsn()
{
	Actor* otherObj = Actor::FindWithID(cylClsn.otherObjID);
	if (!otherObj)
		return;
	
	u32 hitFlags = cylClsn.hitFlags;
	
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
			Kill();
		}
		
		return;
	}
	else if (hitFlags & CylinderClsn::HIT_BY_SPIN_OR_GROUND_POUND)
	{
		defeatMethod = Enemy::DF_SQUASHED;
		KillByAttack(*otherObj, wmClsn);
		
		scale.y = 0.3333_f;
		
		Sound::Play("NCS_SE_SCT_KRB_SMASH"sfx, camSpacePos);
		
		return;
	}
	
	if (otherObj->actorID != PLAYER_ACTOR_ID)
		return;
	
	Player* player = (Player*)otherObj;
	
	if (JumpedOnByPlayer(cylClsn, *player))
	{
		defeatMethod = Enemy::DF_SQUASHED;
		KillByAttack(*player, wmClsn);
		
		scale.y = 0.3333_f;
		
		Sound::Play("NCS_SE_SCT_KRB_SMASH"sfx, camSpacePos);
		
		player->Bounce(PLAYER_BOUNCE_INIT_VEL);
	}
	else if (hitFlags & CylinderClsn::HIT_BY_MEGA_CHAR)
	{
		SpawnMegaCharParticles(*player, nullptr);
		
		Sound::Play("NCS_SE_SCT_HIT_SMALL"sfx, camSpacePos);
		
		KillByInvincibleChar(KNOCKBACK_ROT, *player);
	}
	else if (player->IsOnShell() || player->isMetalWario)
	{
		KillByInvincibleChar(KNOCKBACK_ROT, *player);
	}
	else
	{
		player->Hurt(pos, DAMAGE, PLAYER_KNOCKBACK, 1, 0, 1);
		
		if (state == ST_CHASE)
		{
			state = ST_WAIT;
			chaseCooldown = CHASE_COOLDOWN;
		}
		else if ((state == ST_WAIT || state == ST_TURN) && chaseCooldown == 0)
		{
			state = ST_TURN;
			alarmed = true;
			targetAngle = pos.HorzAngle(player->pos);
		}
	}
	
}

Player* ShyGuy::PlayerVisibleToThis(Player* player)
{
	if (!player)
		player = ClosestPlayer();
	
	if (!player)
		return nullptr;
	
	Vector3 eyePos = { pos.x, pos.y + (HEIGHT >> 1), pos.z };
	Vector3 playerPos = { player->pos.x, player->pos.y + 72._f, player->pos.z };
	
	Vector3 toPlayer = playerPos - eyePos;
	Fix12i dist = toPlayer.Len();
	
	if (dist > SIGHT_DIST)
		return nullptr;
	
	Matrix4x3 rotationMat = Matrix4x3::RotationY(ang.y);
	Vector3 forward = rotationMat(FORWARD_VEC);
	
	if (toPlayer.Dot(forward) < dist * SIGHT_COS_RADIUS)
		return nullptr;
	
	RaycastLine raycaster;
	raycaster.SetObjAndLine(eyePos, playerPos, nullptr);
	
	return !raycaster.DetectClsn() ? player : nullptr;
}

bool ShyGuy::KillIfTouchedBadSurface()
{
	if (wmClsn.IsOnGround())
		return false;
	
	CLPS& clps = wmClsn.sphere.floorResult.surfaceInfo.clps;
	s32 behav = clps.behaviorID;
	
	bool isQuicksand = clps.textureID == CLPS::TX_SAND &&
		(behav == CLPS::BH_LOW_JUMPS || behav == CLPS::BH_SLOW_SHALLOW_QUICKSAND ||
		 behav == CLPS::BH_SLOW_DEEP_QUICKSAND || behav == CLPS::BH_INSTANT_QUICKSAND);
	
	if (isQuicksand || clps.isWater || behav == CLPS::BH_WIND_GUST ||
		behav == CLPS::BH_LAVA || behav == CLPS::BH_DEATH || behav == CLPS::BH_DEATH_2)
	{
		SpawnCoin();
		Kill();
		return true;
	}
	
	return false;
}

s32 ShyGuy::GetClosestPathNodeID()
{
	Fix12i closestDist = Fix12i::max;
	s32 closestPt = 0;
	Vector3 pathNode;
	
	for (s32 i = 0; i < numPathNodes; ++i)
	{
		pathPtr.GetNode(pathNode, i);
		
		Fix12i dist = pos.Dist(pathNode);
		
		if (dist < closestDist)
		{
			closestDist = dist;
			closestPt = i;
		}
	}
	
	return closestPt;
}

void ShyGuy::AimAtClosestPathPt()
{
	if (pathPtr.ptr != nullptr)
	{
		nextPathNode = GetClosestPathNodeID();
		SetTargetPos();
	}
}

void ShyGuy::PlayMovingSoundEffect()
{
	if (modelAnim.file != animFiles[WALK].BCA() && modelAnim.file != animFiles[RUN].BCA())
		return;
	
	u32 numFrames = modelAnim.GetNumFrames();
	u32 currFrame = modelAnim.GetCurrFrame();
	
	if (currFrame == 0 || currFrame == numFrames / 2)
		Sound::Play("NCS_SE_SCT_KRB_WALK"sfx, camSpacePos);
}

void ShyGuy::State0_Wait()
{
	if (offTrack)
	{
		Vector3 targetDir = targetPos - pos;
		
		if ((wmClsn.IsOnWall() && targetDir.Dot(wallNormal) < 0._f) ||
			IsGoingOffCliff(wmClsn, 50._f, 0x1f49, 0, 1, CLIFF_TOLERANCE) ||
			(pos.HorzDist(targetPos) <= TARGET_POS_TOLERANCE && Abs(targetPos.y - pos.y) > HEIGHT_TOLERANCE))
		{
			state = ST_WARP;
			return;
		}
	}
		
	if (pos.HorzDist(targetPos) > TARGET_POS_TOLERANCE)
	{
		modelAnim.SetAnim(animFiles[WALK].BCA(), Animation::LOOP, 1._f, 0);
		horzSpeed = WALK_SPEED;
		motionAng.y = ang.y = pos.HorzAngle(targetPos);
	}
	else if (pathPtr.ptr != nullptr)
	{
		nextPathNode += reverse ? -1 : 1;
		if (backAndForth)
		{
			if (nextPathNode == numPathNodes - 1)
				reverse = true;
			else if (nextPathNode == 0)
				reverse = false;
		}
		else if (nextPathNode >= numPathNodes)
			nextPathNode = 0;
		
		SetTargetPos();
		state = ST_TURN;
	}
	else
	{
		modelAnim.SetAnim(animFiles[WAIT].BCA(), Animation::LOOP, 1._f, 0);
		horzSpeed = 0._f;
		if (stateTimer >= WAIT_TIME)
		{
			targetAngle += 0x4000;
			state = ST_TURN;
		}
	}
}

void ShyGuy::State1_Turn()
{
	horzSpeed = 0._f;
	modelAnim.SetAnim(animFiles[WAIT].BCA(), Animation::LOOP, 1._f, 0);
	if (ApproachLinear(ang.y, targetAngle, TURN_SPEED))
		state = alarmed ? ST_CHASE : ST_WAIT;
}

void ShyGuy::State2_Chase()
{
	modelAnim.SetAnim(animFiles[RUN].BCA(), Animation::LOOP, 1._f, 0);
	
	offTrack = true;
	
	if (flags & Actor::OFF_SCREEN)
	{
		state = ST_WAIT;
		chaseCooldown = CHASE_COOLDOWN;
		AimAtClosestPathPt();
	}
	if (pos.Dist(targetPlayer->pos) >= QUIT_CHASE_DIST)
		state = ST_STOP;
	
	targetAngle = pos.HorzAngle(targetPlayer->pos);
	
	horzSpeed.ApproachLinear(CHASE_SPEED, CHASE_ACCEL);
	
	ApproachLinear(ang.y, targetAngle, CHASE_TURN_SPEED);
	motionAng.y = ang.y;
}

void ShyGuy::State3_Stop()
{
	AimAtClosestPathPt();
	
	modelAnim.SetAnim(animFiles[FREEZE].BCA(), Animation::LOOP, 1._f, 0);
	
	offTrack = true;
	
	if (horzSpeed.ApproachLinear(0._f, CHASE_ACCEL))
	{
		state = ST_WAIT;
		chaseCooldown = CHASE_COOLDOWN;
	}
}

void ShyGuy::State4_Warp()
{
	if (stateTimer == 0)
	{
		vertAccel = 0._f;
		termVel = Fix12i::min;
		flags &= ~Actor::AIMABLE_BY_EGG;
	}
	
	if (pos.HorzDist(targetPos) > TARGET_POS_WARP_TOL)
	{
		s32 vertAngle = targetPos.VertAngle(pos);
		
		horzSpeed = WARP_SPEED * Cos(vertAngle);
		speed.y = WARP_SPEED * Sin(vertAngle);
		
		motionAng.y = ang.y = pos.HorzAngle(targetPos);
	}
	else
	{
		state = ST_WAIT;
		chaseCooldown = CHASE_COOLDOWN;
		
		speed.y = 0._f;
		vertAccel = VERT_ACCEL;
		termVel = TERM_VEL;
		flags |= Actor::AIMABLE_BY_EGG;
		
		offTrack = false;
	}
	
	matChg.currFrame.ApproachLinear(6._f, 1._f);
}

Fix12i ShyGuy::FloorY(const Vector3& pos)
{
	Vector3 raycastPos = { pos.x, pos.y + 20._f, pos.z };
	RaycastGround raycaster;
	
	raycaster.SetObjAndPos(raycastPos, nullptr);
	
	return raycaster.DetectClsn() ? raycaster.clsnPosY : pos.y;
}

ShyGuy::ShyGuy() {}
ShyGuy::~ShyGuy() {}