#include "SM64DS_2.h"
#include "Actors/Custom/LaunchStar.h"

LsPlayerExtension LS_PLAYER_EXTENSIONS[4];

Player::State Player::ST_LAUNCH_STAR = { &Player::St_LaunchStar_Init, &Player::St_LaunchStar_Main, &Player::St_LaunchStar_Cleanup };

bool Player::St_LaunchStar_Init()
{
	LsPlayerExtension& ext = LS_PLAYER_EXTENSIONS[playerID];
	
	horzSpeed = vertAccel = 0._f;
	ext.lsInitPos = pos;
	ext.launchTimer = 0;
	
	ext.lsPos = ext.lsPtr->pos; // launch star pos
	ext.lsPos.y += 160._f;      // correct y-pos of target launch star
	
	ext.lsInitAng = ang;
	ext.lsDiffAng.x = ext.lsPtr->ang.x - ang.x;
	ext.lsDiffAng.y = ext.lsPtr->ang.y - ang.y;
	ext.lsDiffAng.z = ext.lsPtr->ang.z - ang.z;
	
	SetAnim(95, Animation::NO_LOOP, 1._f, 0);
	
	Sound::Play("NCS_SE_SCT_HEART_TOUCH"sfx, camSpacePos);
	
	return true;
}

bool Player::St_LaunchStar_Main()
{
	LsPlayerExtension& ext = LS_PLAYER_EXTENSIONS[playerID];
	ModelAnim& lsModelAnim = ext.lsPtr->modelAnim;
	
	speed.y = 0._f;
	
	Fix12i t = Fix12i(ext.launchTimer) / 4;
	
	if (stateState == 1 || stateState == 2)
	{
		lsModelAnim.data.UpdateBones(lsModelAnim.file, (s32)lsModelAnim.GetCurrFrame());
		
		pos = lsModelAnim.mat4x3(lsModelAnim.data.bones[1].pos) << 3;
	}
	
	switch (stateState)
	{
		case 0:
			Vec3_Interp(pos, ext.lsInitPos, ext.lsPos, t);
			
			ang.x = (Fix12s(ext.lsDiffAng.x, as_raw) * t).val + ext.lsInitAng.x;
			ang.y = (Fix12s(ext.lsDiffAng.y, as_raw) * t).val + ext.lsInitAng.y;
			ang.z = (Fix12s(ext.lsDiffAng.z, as_raw) * t).val + ext.lsInitAng.z;
			
			ext.launchTimer++; //increment state timer
			
			if (ext.launchTimer > 4)
			{
				ext.lsPtr->modelAnim.SetAnim(LaunchStar::animFiles[LaunchStar::LAUNCH].BCA(), Animation::NO_LOOP, 1._f, 0);
				stateState = 1;
			}
			
			break;
		case 1:
			if (lsModelAnim.currFrame >= 20._f)
			{
				lsModelAnim.speed = ext.lsPtr->launchSpeed / 8;
				
				Sound::Play("NCS_SE_SCT_THO_LAUNCH"sfx, camSpacePos);
				Sound::Play("NCS_SE_SCT_BTN_WALK"sfx, camSpacePos);
				Sound::Play("NCS_SE_PT_GO_UP"sfx, camSpacePos);
				
				stateState = 2;
			}
			
			break;
		case 2:
			if (lsModelAnim.currFrame >= 47._f)
			{
				lsModelAnim.currFrame = 47._f;
				lsModelAnim.speed = 1._f;
				
				pos = ext.lsPos;
				
				ext.bzIt.pathPtr = ext.lsPtr->pathPtr;
				ext.bzIt.currSplineX3 = 0;
				ext.bzIt.tinyStep = 0.004_fs;
				ext.bzIt.step = ext.lsPtr->launchSpeed;
				ext.bzIt.currTime = 0_f;
				ext.bzIt.pos = ext.lsPos;
				
				Vector3_16f particleDir { 0._fs, 0._fs, 0._fs };
				ext.particleID = Particle::System::New(ext.particleID, 0x114, pos.x, pos.y, pos.z, &particleDir, nullptr);
				
				stateState = 3;
			}
			
			break;
		case 3:
			bool finished = !ext.bzIt.Advance();
			pos = ext.bzIt.pos;
			
			ang.y = motionAng.y = prevPos.HorzAngle(pos);
			ang.x = 90._deg - pos.VertAngle(prevPos);
			
			Vector3_16f particleDir { 0._fs, 0._fs, 0._fs };
			ext.particleID = Particle::System::New(ext.particleID, 0x114, pos.x, pos.y, pos.z, &particleDir, nullptr);
			
			if (finished)
			{
				Vector3 p2 = ext.bzIt.pathPtr.GetNode(ext.bzIt.currSplineX3 + 2);
				s16 ang = pos.VertAngle(p2);
				
				horzSpeed = Cos(ang) * ext.bzIt.step;
				speed.y   = Sin(ang) * ext.bzIt.step;
				
				ChangeState(*(Player::State*)Player::ST_FALL);
			}
			
			break;
	}
	
	prevPos = pos;
	
	return true;
}

bool Player::St_LaunchStar_Cleanup()
{
	switch ((u32)nextState)
	{
		case Player::ST_SWIM:
		case Player::ST_HURT:
		case Player::ST_HURT_WATER:
			return false;
		default:
			return true;
	}
}