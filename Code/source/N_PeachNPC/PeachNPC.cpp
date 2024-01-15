#include "SM64DS_2.h"
#include "Actors/Custom/PeachNPC.h"

namespace
{
	using StateFunc = void(PeachNPC::*)();
	
	const StateFunc states[PeachNPC::NUM_STATES]
	{
		&PeachNPC::St_Wait,
		&PeachNPC::St_Talk,
	};
	
	constexpr Fix12i RADIUS = 120._f;
	constexpr Fix12i HEIGHT = 150._f;
	constexpr Fix12i TALK_RADIUS = 240._f;
	constexpr Fix12i TALK_HEIGHT = 112._f;
	constexpr s16 TURN_SPEED = 11.25_deg;
	
	constexpr u16 NO_TALK_MSG_ID = 0x000c;
};

SharedFilePtr PeachNPC::modelFile;
SharedFilePtr PeachNPC::animFile;

SpawnInfo PeachNPC::spawnData =
{
	[]() -> ActorBase* { return new PeachNPC; },
	0x003b,
	0x00aa,
	Actor::NO_BEHAVIOR_IF_OFF_SCREEN | Actor::NO_RENDER_IF_OFF_SCREEN,
	50._f,
	70._f,
	4096._f,
	4096._f
};

s32 PeachNPC::InitResources()
{
	Model::LoadFile(modelFile);
	Animation::LoadFile(animFile);
	
	modelAnim.SetFile(modelFile.BMD(), 1, -1);
	modelAnim.SetAnim(animFile.BCA(), Animation::LOOP, 1._f, 0);
	
	cylClsn.Init(this, RADIUS, HEIGHT, CylinderClsn::VERTICAL_PUSHBACK_ONLY | CylinderClsn::ENEMY | CylinderClsn::SIGN, 0);
	shadow.InitCylinder();
	
	Vector3 raycastPos = { pos.x, pos.y + 20._f, pos.z };
	RaycastGround raycaster;
	
	raycaster.SetObjAndPos(raycastPos, this);
	
	if (raycaster.DetectClsn())
		pos.y = raycaster.clsnPosY;
	
	UpdateModelTransform();
	
	state = ST_WAIT;
	listener = nullptr;
	
	messages[0] = param1 & 0xfff;
	messages[1] = ang.x  & 0xfff;
	eventID = (param1 >> 12 & 0xf) | (ang.x >> 8 & 0xf0);

	shouldTalk = ang.z & 0x8000;
	
	ang.x = ang.z = 0._deg;
	
	return 1;
}

s32 PeachNPC::CleanupResources()
{
	modelFile.Release();
	animFile.Release();
	
	return 1;
}

s32 PeachNPC::Behavior()
{
	modelAnim.Advance();
	
	(this->*states[state])();
	
	cylClsn.Clear();
	cylClsn.Update();
	
	UpdateModelTransform();
	
	return 1;
}

s32 PeachNPC::Render()
{
	modelAnim.Render();
	
	return 1;
}

void PeachNPC::UpdateModelTransform()
{
	modelAnim.mat4x3 = Matrix4x3::RotationY(ang.y);
	modelAnim.mat4x3.c3 = pos >> 3;
	
	DropShadowRadHeight(shadow, modelAnim.mat4x3, 99._f, 55._f, 15); //radius and height are (C) Yoshi the Player.
}

void PeachNPC::St_Wait()
{
	if (cylClsn.otherObjID == 0 || !(cylClsn.hitFlags & CylinderClsn::HIT_BY_PLAYER))
		return;
	
	Actor* otherObj = Actor::FindWithID(cylClsn.otherObjID);
	
	if (otherObj == nullptr || otherObj->actorID != PLAYER_ACTOR_ID)
		return;
	
	Player& player = *(Player*)otherObj;
	
	if (player.StartTalk(*this, false))
	{
		Message::PrepareTalk();
		
		state = ST_TALK;
		listener = &player;
	}
}
void PeachNPC::St_Talk()
{
	if (!ApproachLinear(ang.y, pos.HorzAngle(listener->pos), TURN_SPEED))
		return;
	
	s32 talkState = listener->GetTalkState();
	
	switch (talkState)
	{
		case Player::TK_START:
		{
			Vector3 lookAt = { pos.x, pos.y + TALK_HEIGHT, pos.z };
			
			listener->ShowMessage(*this, shouldTalk ? messages[Event::GetBit(eventID) ? 1 : 0] : NO_TALK_MSG_ID, lookAt, 0, 0);

            // todo: find a sound that peach can play
			// Sound::PlayCharVoice(Player::CH_YOSHI, 4, camSpacePos);
			
			break;
		}
		case Player::TK_NOT:
		{
			Message::EndTalk();
			
			state = ST_WAIT;
			
			break;
		}
	}
}

PeachNPC::PeachNPC() {}
PeachNPC::~PeachNPC() {}