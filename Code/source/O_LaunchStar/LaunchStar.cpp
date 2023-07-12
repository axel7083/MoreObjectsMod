#include "SM64DS_2.h"
#include "Actors/Custom/LaunchStar.h"

SpawnInfo LaunchStar::spawnData =
{
	[]() -> ActorBase* { return new LaunchStar; },
	0x0168,
	0x00aa,
	Actor::NO_BEHAVIOR_IF_OFF_SCREEN | Actor::NO_RENDER_IF_OFF_SCREEN | Actor::UPDATE_DURING_STAR_CUTSCENE | Actor::UPDATE_DURING_CUTSCENE,
	0._f,
	1536._f,
	4096._f,
	4096._f,
};

SharedFilePtr LaunchStar::modelFile;
SharedFilePtr LaunchStar::animFiles[NUM_ANIMS];

s32 LaunchStar::InitResources()
{
	Model::LoadFile(modelFile);
	modelAnim.SetFile(modelFile.BMD(), 1, -1);
	
	for (s32 i = 0; i < NUM_ANIMS; i++)
		Animation::LoadFile(animFiles[i]);
	
	modelAnim.SetAnim(animFiles[WAIT].BCA(), Animation::LOOP, 1._f, 0);
	
	cylClsn.Init(this, 160._f, 320._f, CylinderClsn::INTANGIBLE | CylinderClsn::COLLECTABLE, CylinderClsn::HIT_BY_YOSHI_TONGUE);
	
	launchSpeed = Fix12i(ang.x);
	eventID = (param1 >> 8) & 0xff;
	
	// if the event is set, do not show the cutscene
	if (eventID >= NUM_EVENTS || Event::GetBit(eventID))
		eventID = 0xff;
	
	spawnTimer = 0;
	
	pathPtr.FromID(param1 & 0xff);
	pos = pathPtr.GetNode(0);
	Vector3 target = pathPtr.GetNode(1);
	
	ang.x = (ang.z == 1 ? 0._deg : 90._deg - target.VertAngle(pos));
	ang.y = (ang.z == 1 ? 0._deg : pos.HorzAngle(target));
	ang.z = 0._deg;
	
	UpdateModelTransform();
	
	pos.y -= 160._f;
	
	return 1;
}

s32 LaunchStar::CleanupResources()
{
	modelFile.Release();
	
	for (s32 i = 0; i < NUM_ANIMS; i++)
		animFiles[i].Release();
	
	return 1;
}

s32 LaunchStar::Behavior()
{
	if (eventID != 0xff)
	{
		if (!Event::GetBit(eventID))
			return 1;
		
		spawnTimer++;
		
		Camera& camera = *CAMERA;
		
		if (spawnTimer == 30)
		{
			camPos = camera.pos;
			camLookAt = camera.lookAt;
		}
		
		if (spawnTimer == 90)
		{
			eventID = 0xff;
			
			NEXT_ACTOR_UPDATE_FLAGS &= ~Actor::UPDATE_DURING_STAR_CUTSCENE;
			
			camera.SetLookAt(camLookAt);
			camera.SetPos(camPos);
		}
		else if (spawnTimer >= 30)
		{
			if (spawnTimer == 45)
				Sound::PlayMsgSound(41, 0x40, 0x7f, 107._f, false);
			
			NEXT_ACTOR_UPDATE_FLAGS |= Actor::UPDATE_DURING_STAR_CUTSCENE;
			
			Vector3 newCamPos = pos;
			newCamPos.x += Sin(ang.y) * 850._f;
			newCamPos.y += 600._f;
			newCamPos.z += Cos(ang.y) * 850._f;
			
			camera.SetLookAt(pos);
			camera.SetPos(newCamPos);
		}
	}
		
	Player* closestPlayer = ClosestPlayer();
	
	if (closestPlayer != nullptr)
	{
		Player& player = *closestPlayer;
		LsPlayerExtension& ext = LS_PLAYER_EXTENSIONS[player.playerID];
		
		if (player.uniqueID == cylClsn.otherObjID && (INPUT_ARR[0].buttonsPressed & Input::A) &&
		   (!player.IsState(Player::ST_LAUNCH_STAR) || ext.lsPtr != this))
		{
			ext.lsPtr = this;
			player.ChangeState(Player::ST_LAUNCH_STAR);
		}
	}
	
	if (modelAnim.file == animFiles[LAUNCH].BCA() && modelAnim.Finished())
		modelAnim.SetAnim(animFiles[WAIT].BCA(), Animation::LOOP, 1._f, 0);
	
	modelAnim.Advance();
	
	cylClsn.Clear();
	cylClsn.Update();
	
	return 1;
}

s32 LaunchStar::Render()
{
	if (eventID != 0xff && spawnTimer < 30)
		return 1;
	
	modelAnim.Render();
	
	return 1;
}

void LaunchStar::UpdateModelTransform()
{
	modelAnim.mat4x3 = Matrix4x3::RotationZXY(ang.x, ang.y, ang.z);
	modelAnim.mat4x3.c3 = pos >> 3;
}

LaunchStar::LaunchStar() {}
LaunchStar::~LaunchStar() {}