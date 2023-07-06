#include "SM64DS_2.h"
#include "Actors/Custom/RideableYoshi.h"

asm("yoshiWaitAnimFile = 0x0210ec30");
asm("runAnimFile = 0x0210ea28");
asm("yoshiHeadEatAnimFile = 0x0210eb88");

extern "C"
{
	extern SharedFilePtr yoshiWaitAnimFile;
	extern SharedFilePtr runAnimFile;
	extern SharedFilePtr yoshiHeadEatAnimFile;
}

namespace
{
	SharedFilePtr* animFiles[] =
	{
		&yoshiWaitAnimFile,
		&RideableYoshi::ridingAnimFile,
		&runAnimFile,
	};
	
	constexpr s32 SPIN_BONE_ID = 8;
	constexpr s32 FACE_1_BONE_ID = 15;
	
	constexpr Fix12i RADIUS = 99._f;
	constexpr Fix12i HEIGHT = 112._f;
	
	constexpr Fix12i HORZ_SPEED = 20._f;
	constexpr Fix12i VERT_ACCEL = -2._f;
	constexpr Fix12i TERM_VEL = -50._f;
	
	constexpr u8 COOLDOWN_TIME = 30;
	constexpr u16 RUN_TIME = 180;
};

SpawnInfo RideableYoshi::spawnData =
{
	[]() -> ActorBase* { return new RideableYoshi; },
	0x0171,
	0x00aa,
	Actor::NO_RENDER_IF_OFF_SCREEN | Actor::UNK_02,
	50._f,
	70._f,
	4096._f,
	4096._f
};

SharedFilePtr RideableYoshi::ridingAnimFile;

RideableYoshi::State RideableYoshi::ST_WAIT = { &RideableYoshi::St_Wait_Init, &RideableYoshi::St_Wait_Main };
RideableYoshi::State RideableYoshi::ST_RIDE = { &RideableYoshi::St_Ride_Init, &RideableYoshi::St_Ride_Main };
RideableYoshi::State RideableYoshi::ST_RUN  = { &RideableYoshi::St_Run_Init,  &RideableYoshi::St_Run_Main };

s32 RideableYoshi::InitResources()
{
	headModel = new ModelAnim;
	
	if (headModel == nullptr)
		return 0;
	
	// the player should load his stuff first, so the SharedFilePtr's should be there before now.
	Animation::LoadFile(yoshiHeadEatAnimFile);
	
	for (s32 i = 0; i < NUM_ANIMS; ++i)
		Animation::LoadFile(*animFiles[i]);
	
	rider = ClosestPlayer();
	
	modelAnim.SetFile(rider->bodyModels[Player::CH_YOSHI]->data.modelFile, 1, -1);
	modelAnim.SetAnim(animFiles[WAIT]->BCA(), Animation::LOOP, 1._f, 0);
	
	headModel->SetFile(rider->headModels[Player::CH_YOSHI]->data.modelFile, 1, -1);
	((ModelAnim*)headModel)->SetAnim(yoshiHeadEatAnimFile.BCA(), Animation::NO_LOOP, 1._f, 0);
	
	cylClsn.Init(this, RADIUS, HEIGHT, CylinderClsn::VERTICAL_PUSHBACK_ONLY | CylinderClsn::ENEMY | CylinderClsn::SIGN, 0);
	
	wmClsn.Init(this, RADIUS, RADIUS, nullptr, nullptr);
	
	shadow.InitCylinder();
	
	origPos = pos;
	
	UpdateModelTransform();
	ChangeState(&ST_WAIT);
	
	vertAccel = VERT_ACCEL;
	termVel = TERM_VEL;
	
	return 1;
}

s32 RideableYoshi::CleanupResources()
{
	delete headModel;
	
	yoshiHeadEatAnimFile.Release();
	
	for (s32 i = 0; i < NUM_ANIMS; ++i)
		animFiles[i]->Release();
	
	return 1;
}

s32 RideableYoshi::Behavior()
{
	modelAnim.Advance();
	
	(this->*state->main)();
	
	UpdateModelTransform();
	DropShadow();
	
	return 1;
}

s32 RideableYoshi::Render()
{
	if (riding && rider->isMega)
		return 1;
	
	modelAnim.Render();
	headModel->Render();
	
	return 1;
}

void RideableYoshi::UpdateModelTransform()
{
	if (riding)
	{
		modelAnim.mat4x3 = rider->bodyModels[rider->param1]->mat4x3(rider->bodyModels[rider->param1]->data.transforms[SPIN_BONE_ID]);
	}
	else
	{
		modelAnim.mat4x3 = Matrix4x3::RotationY(ang.y);
		modelAnim.mat4x3.c3 = pos >> 3;
	}
	
	headModel->mat4x3 = modelAnim.mat4x3(modelAnim.data.transforms[FACE_1_BONE_ID]);
}

void RideableYoshi::DropShadow()
{
	shadowMat = modelAnim.mat4x3;
	shadowMat.c3.y += 20._f >> 3;
	
	DropShadowRadHeight(shadow, shadowMat, RADIUS, 55._f, 15); // radius and height are Yoshi the Player.
}

void RideableYoshi::CheckCylClsn()
{
	if (DecIfAbove0_Byte(cooldown) != 0 || cylClsn.otherObjID == 0)
		return;
	
	Actor* actor = Actor::FindWithID(cylClsn.otherObjID);
	
	if (actor == nullptr || actor->actorID != PLAYER_ACTOR_ID)
		return;
	
	Player& player = *(Player*)actor;
	
	if (player.param1 != Player::CH_YOSHI && JumpedOnByPlayer(cylClsn, player))
	{
		rider = &player;
		ChangeState(&ST_RIDE);
	}
}

void RideableYoshi::ChangeState(State* newState)
{
	state = newState;
	(this->*state->init)();
}

void RideableYoshi::StartRide(s32 charID)
{
	// remember to take care of the player-not-wearing-a-cap case
	riderChar = rider->param1;
	
	modelAnim.SetFile(rider->bodyModels[charID]->data.modelFile, 1, -1);
	modelAnim.SetAnim(animFiles[RIDE]->BCA(), Animation::LOOP, 1._f, 0);
	
	((ModelAnim*)headModel)->~ModelAnim();
	new(headModel) Model();
	
	headModel->SetFile(rider->headModels[charID]->data.modelFile, 1, -1);
	
	// if we don't do this, Luigi always has a vanish head
	if (riderChar == Player::CH_LUIGI)
		rider->texSeq268.Update(headModel->data);
	
	rider->pos = pos;
	
	rider->TurnOffToonShading(rider->prevHatChar);
	rider->TurnOffToonShading(rider->currHatChar);
	rider->TurnOffToonShading(Player::CH_YOSHI);
	
	ChangeCharacter(*rider, Player::CH_YOSHI);
	
	riding = true;
}

void RideableYoshi::EndRide(bool changeChar)
{
	modelAnim.SetFile(rider->bodyModels[Player::CH_YOSHI]->data.modelFile, 1, -1);
	modelAnim.SetAnim(animFiles[WAIT]->BCA(), Animation::LOOP, 1._f, 0);
	
	headModel->~Model();
	new(headModel) ModelAnim();
	
	headModel->SetFile(rider->headModels[Player::CH_YOSHI]->data.modelFile, 1, -1);
	((ModelAnim*)headModel)->SetAnim(yoshiHeadEatAnimFile.BCA(), Animation::NO_LOOP, 1._f, 0);
	
	Vector3 zeros = { 0._f, 0._f, 0._f };
	pos = rider->pos;
	rider->pos = modelAnim.mat4x3(zeros) << 3;
	
	if (changeChar)
		ChangeCharacter(*rider, riderChar);
	
	riding = false;
}

void RideableYoshi::St_Wait_Init()
{
	modelAnim.SetAnim(animFiles[WAIT]->BCA(), Animation::LOOP, 1._f, 0);
	
	cooldown = COOLDOWN_TIME;
	horzSpeed = 0._f;
}

void RideableYoshi::St_Wait_Main()
{
	UpdatePos(nullptr);
	
	CheckCylClsn();
	
	cylClsn.Clear();
	
	if (state != &ST_RIDE)
	{
		cylClsn.Update();
		wmClsn.UpdateDiscreteNoLava();
	}
}

void RideableYoshi::St_Ride_Init()
{
	StartRide(rider->param1);
}

void RideableYoshi::St_Ride_Main()
{
	pos = rider->pos;
	
	s32 newArea = 8;
	
	for (s32 i = 0; i < 8; ++i)
	{
		if (AREAS[i].showing)
		{
			if (newArea == 8)
				newArea = i;
			else
				newArea = -1;
		}
	}
	
	if (newArea != 8)
		areaID = newArea;
	
	s32 endRideState = ShouldEndRide(*rider);
	if (endRideState != ER_DO_NOT_END_RIDE)
	{
		EndRide(endRideState != ER_END_RIDE_NO_CHANGE_CHAR);
		ChangeState(endRideState == ER_END_RIDE_CHANGE_CHAR ? &ST_RUN : &ST_WAIT);
	}
}

void RideableYoshi::St_Run_Init()
{
	modelAnim.SetAnim(animFiles[RUN]->BCA(), Animation::LOOP, 0x1900_f, 0);
	
	cooldown = COOLDOWN_TIME;
	runTimer = RUN_TIME;
	motionAng.y = ang.y = rider->ang.y;
}

void RideableYoshi::St_Run_Main()
{
	horzSpeed = HORZ_SPEED;
	
	St_Wait_Main();
	
	if (state == &ST_RUN && DecIfAbove0_Short(runTimer) == 0)
	{
		Vector3 dustPos = { pos.x, pos.y + 80._f, pos.z };
		DisappearPoofDustAt(dustPos);
		PoofDustAt(dustPos);
		
		pos = origPos;
		
		ChangeState(&ST_WAIT);
	}
}

s32 RideableYoshi::ShouldEndRide(Player& player)
{
	if (player.isMega)
		return ER_DO_NOT_END_RIDE;
	
	switch ((u32)player.currState)
	{
		case Player::ST_HURT:
		case Player::ST_HURT_WATER:
		case Player::ST_ELECTROCUTE:
		case Player::ST_BURN_FIRE:
		case Player::ST_BURN_LAVA:
			return ER_END_RIDE_CHANGE_CHAR;
		case Player::ST_SWEEP_KICK:
			return ER_END_RIDE_CHANGE_CHAR_NO_RUN;
	}
	
	return player.param1 == Player::CH_YOSHI ? ER_DO_NOT_END_RIDE : ER_END_RIDE_NO_CHANGE_CHAR;
}

void RideableYoshi::ChangeCharacter(Player& player, u32 character)
{
	u32 bodyModelID = player.GetBodyModelID(character, false);
	u32 prevBodyModelID = player.GetBodyModelID(player.prevHatChar, false);
	SharedFilePtr* animFile = Player::ANIM_PTRS[player.animID + character];
	
	player.SetNewHatCharacter(character, 0, true);
	player.param1 = character;
	player.toonStateAndFlag = 0;
	player.bodyModels[bodyModelID]->Copy(*player.bodyModels[prevBodyModelID], animFile->BCA(), 0);
}

RideableYoshi::RideableYoshi() {}
RideableYoshi::~RideableYoshi() {}