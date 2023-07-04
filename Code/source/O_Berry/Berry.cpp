#include "SM64DS_2.h"
#include "Actors/Custom/Berry.h"

namespace
{	
	constexpr Fix12i RADIUS = 50._f;
	constexpr Fix12i COIN_SPEED = 11._f;
	
	const u8 coinPrizeTypes[] =
	{
		0, 0, 0, 0,
		0, 0, 0, 2,
		0, 2, 2, 2,
	};
};

SpawnInfo Berry::spawnData =
{
	[]() -> ActorBase* { return new Berry; },
	0x016d,
	0x00aa,
	Actor::NO_BEHAVIOR_IF_OFF_SCREEN | Actor::NO_RENDER_IF_OFF_SCREEN,
	50._f,
	96._f,
	4096._f,
	4096._f,
};

SharedFilePtr Berry::modelFile;
SharedFilePtr Berry::stemModelFile;

u32 Berry::berryCount;
u32 Berry::berryMaxCount;

s32 Berry::InitResources()
{
	Model::LoadFile(modelFile);
	model.SetFile(modelFile.BMD(), 1, -1);
	
	Model::LoadFile(stemModelFile);
	stem.SetFile(stemModelFile.BMD(), 1, -1);
	
	model.data.materials[0].difAmb = ((param1 & 0x7fff) << 16) | 0x8000;
	
	Model::LoadFile(COIN_YELLOW_POLY4_MODEL_PTR);
	Model::LoadFile(COIN_BLUE_POLY32_MODEL_PTR);
	Model::LoadFile(COIN_BLUE_POLY4_MODEL_PTR);
	
	cylClsn.Init(this, RADIUS, RADIUS * 2, CylinderClsn::ENEMY, CylinderClsn::HIT_BY_YOSHI_TONGUE);
	
	shadow.InitCylinder();
	
	UpdateModelTransform();
	
	origPos = pos;
	
	return 1;
}

s32 Berry::CleanupResources()
{
	modelFile.Release();
	stemModelFile.Release();
	
	berryCount = 0;
	berryMaxCount = 0;
	
	return 1;
}

s32 Berry::Behavior()
{
	if (killed)
		return 1;
	
	if (!groundFound) //not called during InitResources because not all the platforms may have initialized their resources yet
	{
		groundFound = true; 
		
		Vector3 spherePos = { pos.x, pos.y + RADIUS, pos.z };
		SphereClsn sphere;
		sphere.SetObjAndSphere(spherePos, RADIUS * 3 / 2, nullptr);
		
		Vector3 stemPos, stemNormal;
		if (sphere.DetectClsn())
		{
			Vector3 newSpherePos = sphere.pos + sphere.pushback;
			stemPos = newSpherePos - sphere.result.surfaceInfo.normal * sphere.radius;
			stemNormal = sphere.result.surfaceInfo.normal;
		}
		else
		{
			stemPos = pos;
			stemNormal = Vector3{ 0._f, 1._f, 0._f };
		}
		
		Vector3 zeros = { 0._f, 0._f, 0._f };
		
		s16 newAngX = zeros.VertAngle(stemNormal) + 90._deg;
		s16 newAngY = zeros.HorzAngle(stemNormal);
		
		Matrix4x3 mat1 = Matrix4x3::RotationZXY(newAngX, newAngY, 0._deg);
		Matrix4x3 mat2 = Matrix4x3::RotationY(ang.y);
		
		stem.mat4x3 = mat1(mat2);
		stem.mat4x3.c3 = stemPos >> 3;
				
		berryOffsetX = stemNormal.x * 25;
		berryOffsetZ = stemNormal.z * 25;
	}
	
	cylClsn.Clear();
	cylClsn.Update();
	
	UpdateModelTransform();
	
	return 1;
}

s32 Berry::Render()
{
	stem.Render();
	
	if (!killed && !(flags & Actor::IN_YOSHI_MOUTH))
		model.Render();
	
	return 1;
}

u32 Berry::OnYoshiTryEat()
{
	return Actor::YE_SWALLOW;
}

void Berry::OnTurnIntoEgg(Player& player)
{
	player.Heal(0x100);
	
	Kill();
}

void Berry::UpdateModelTransform()
{
	// model.mat4x3 = Matrix4x3::RotationY(ang.y);
	model.mat4x3.c3.x = (pos.x + berryOffsetX) >> 3;
	model.mat4x3.c3.y = pos.y >> 3;
	model.mat4x3.c3.z = (pos.z + berryOffsetZ) >> 3;
	
	DropShadowRadHeight(shadow, model.mat4x3, RADIUS * 2, 50._f, 15);
}

void Berry::Kill()
{
	berryCount++;
	
	Vector3 numberPos = { pos.x, pos.y + 160._f, pos.z };
	Actor::Spawn(RED_COIN_NUMBER_ACTOR_ID, berryCount, numberPos, nullptr, areaID, -1);
	
	berryCount &= 7;
	
	if (berryCount == 0)
	{
		berryMaxCount++;
		
		switch (berryMaxCount)
		{
			case 1:
			case 2:
			case 3:
				for (s32 i = 0; i < 4; i++)
				{
					Actor* coin = Actor::Spawn(COIN_ACTOR_ID + coinPrizeTypes[4 * (berryMaxCount - 1) + i], 0xf2, pos, nullptr, areaID, -1);
					
					if (coin != nullptr)
					{
						coin->motionAng = Vector3_16{ 0._deg,  (s16)(i * 90._deg), 0._deg };
						coin->horzSpeed = COIN_SPEED;
					}
				}
				break;
			default:
				Vector3 mushroomPos = {pos.x, pos.y + 5._f, pos.z};
				Actor::Spawn(ONE_UP_MUSHROOM_ACTOR_ID, 0, mushroomPos, nullptr, areaID, -1);
				break;
		}
	}
	
	pos = origPos;
	killed = true;
}

Berry::Berry() {}
Berry::~Berry() {}