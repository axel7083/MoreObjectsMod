#include "SM64DS_2.h"
#include "Actors/Custom/NoteBlock.h"

namespace
{
	using clpsBlock = StaticCLPS_Block<
		{ .camBehavID = CLPS::CA_NORMAL, }
	>;
	
	constexpr Fix12i REGULAR_SCALE = 1._f;
	constexpr Fix12i BIG_SCALE     = 1.5_f;
	
	constexpr Fix12i SHADOW_OFFSET_Y   = -20._f;
	constexpr Fix12i SHADOW_SCALE_Y    = 336._f;
	constexpr Fix12i BASE_SHADOW_SCALE = 96._f;
	
	constexpr Fix12i JIGGLE_DOWN_VEL      = -9.375_f;
	constexpr Fix12i JIGGLE_UP_VEL        = 6.25_f;
	constexpr Fix12i MIN_JIGGLE_DOWN_DIST = 32._f;
	
	constexpr Fix12i REGULAR_BOUNCE_VEL = 55._f;
}

SpawnInfo NoteBlock::spawnData =
{
	[]() -> ActorBase* { return new NoteBlock; },
	0x0034,
	0x0100,
	Actor::NO_RENDER_IF_OFF_SCREEN,
	0._f,
	1500._f,
	4096._f,
	4096._f
};

SharedFilePtr NoteBlock::modelFile;
SharedFilePtr NoteBlock::clsnFile;

s32 NoteBlock::InitResources()
{
	boostBounceVel = (Fix12i)(param1 & 0xffff);
	scale.x = scale.y = scale.z = ang.x == 1 ? BIG_SCALE : REGULAR_SCALE;
	ang.x = 0._deg;
	originalPos = pos;
	
	Model::LoadFile(modelFile);
	model.SetFile(modelFile.BMD(), 1, -1);

	MovingMeshCollider::LoadFile(clsnFile);
	clsn.SetFile(clsnFile.KCL(), clsnNextMat, scale.x, ang.y, clpsBlock::instance<>);
	clsn.beforeClsnCallback = &MeshColliderBase::UpdatePosWithTransform;
	clsn.afterClsnCallback = &NoteBlock::AfterClsnCallback;
	
	shadow.InitCuboid();
	
	UpdateModelPosAndRotY();
	UpdateClsnPosAndRot();
	
	return 1;
}

s32 NoteBlock::CleanupResources()
{
	clsn.DisableIfEnabled();
	
	modelFile.Release();
	clsnFile.Release();
	
	return 1;
}

s32 NoteBlock::Behavior()
{
	UpdateModelPosAndRotY();
	DropShadow();
	
	if (IsClsnInRange(0._f, 0._f))
	{
		Jiggle();
		
		if (clsnPlayer != nullptr)
			LaunchPlayer();
		
		UpdateClsnPosAndRot();
	}
	
	clsnPlayer = nullptr;
	
	return 1;
}

s32 NoteBlock::Render()
{
	model.Render(&scale);
	
	return 1;
}

void NoteBlock::DropShadow()
{
	shadowMat = model.mat4x3;
	shadowMat.c3.y += SHADOW_OFFSET_Y;
	
	DropShadowScaleXYZ(shadow, shadowMat, BASE_SHADOW_SCALE * scale.x, SHADOW_SCALE_Y, BASE_SHADOW_SCALE * scale.x, 15);
}

void NoteBlock::Jiggle()
{
	switch (jiggleState)
	{
		case JS_NO_JIGGLE:
			break;
			
		case JS_JIGGLE_DOWN:
			pos.y += JIGGLE_DOWN_VEL;
			
			if (pos.y < originalPos.y - MIN_JIGGLE_DOWN_DIST)
				jiggleState = JS_JIGGLE_UP;
			
			break;
			
		case JS_JIGGLE_UP:
			pos.y += JIGGLE_UP_VEL;
			
			if (pos.y > originalPos.y)
				jiggleState = JS_JIGGLE_STOP;
			
			break;
			
		case JS_JIGGLE_STOP:
			pos.y = originalPos.y;
			jiggleState = JS_NO_JIGGLE;
			
			break;
	}
}

void NoteBlock::LaunchPlayer()
{
	// because the player can't bounce when ground pounding
	if (clsnPlayer->IsState(*(Player::State*)Player::ST_GROUND_POUND))
		clsnPlayer->ChangeState(*(Player::State*)Player::ST_FALL);
	
	bool boost = INPUT_ARR[clsnPlayer->playerID].buttonsHeld & Input::B;
	
	clsnPlayer->Bounce(boost ? boostBounceVel : REGULAR_BOUNCE_VEL);
	
	Sound::Play("NCS_SE_NOTEBLOCK"sfx, camSpacePos);
}

void NoteBlock::AfterClsn(Actor& otherActor)
{
	if (otherActor.actorID != PLAYER_ACTOR_ID)
		return;
	
	Player& player = (Player&)otherActor;
	
	if (player.IsState(*(Player::State*)Player::ST_LEDGE_GRAB))
		return;
	
	clsnPlayer = &player;
	jiggleState = JS_JIGGLE_DOWN;
}

void NoteBlock::AfterClsnCallback(MeshColliderBase& clsn, Actor& clsnActor, Actor& otherActor)
{
	((NoteBlock&)clsnActor).AfterClsn(otherActor);
}

NoteBlock::NoteBlock() {}
NoteBlock::~NoteBlock() {}