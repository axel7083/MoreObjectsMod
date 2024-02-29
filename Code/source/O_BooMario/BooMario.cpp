#include "SM64DS_2.h"
#include "Actors/Custom/BooMario.h"

namespace
{
	using StateFunc = void(BooMario::*)();

	constexpr Fix12i RADIUS = 120._f;
	constexpr Fix12i HEIGHT = 150._f;
	constexpr Fix12i TALK_RADIUS = 240._f;
	constexpr Fix12i TALK_HEIGHT = 112._f;
	constexpr s16 TURN_SPEED = 11.25_deg;
	
	constexpr u16 NO_TALK_MSG_ID = 0x000c;
};

SharedFilePtr BooMario::modelFile;
SharedFilePtr BooMario::animFile;

SpawnInfo BooMario::spawnData =
{
	[]() -> ActorBase* { return new BooMario; },
	0x003b,
	0x00aa,
	Actor::NO_BEHAVIOR_IF_OFF_SCREEN | Actor::NO_RENDER_IF_OFF_SCREEN,
	50._f,
	70._f,
	4096._f,
	4096._f
};

s32 BooMario::InitResources()
{
	Model::LoadFile(modelFile);
	Animation::LoadFile(animFile);
	
	modelAnim.SetFile(modelFile.BMD(), 1, -1);
	modelAnim.SetAnim(animFile.BCA(), Animation::LOOP, 1._f, 0);

	UpdateModelTransform();

	return 1;
}

s32 BooMario::CleanupResources()
{
	modelFile.Release();
	animFile.Release();
	
	return 1;
}

s32 BooMario::Behavior()
{
	modelAnim.Advance();
	UpdateModelTransform();
	
	return 1;
}

s32 BooMario::Render()
{
	modelAnim.Render();
	
	return 1;
}

void BooMario::UpdateModelTransform()
{
    Player* player = ClosestPlayer();

    modelAnim.mat4x3 = Matrix4x3::RotationY(player->ang.y);
    modelAnim.mat4x3.c3 = pos >> 3;

    player->visibilityCounter = 2;
    pos.x = player->pos.x;
    pos.y = player->pos.y + 50._f;
    pos.z = player->pos.z;

}

BooMario::BooMario() {}
BooMario::~BooMario() {}