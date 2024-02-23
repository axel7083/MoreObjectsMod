#include "Actors/Custom/ColoredPipe.h"

namespace
{
    using clpsBlock = StaticCLPS_Block<
            {  }
    >;
}

SharedFilePtr ColoredPipe::modelFile;
SharedFilePtr ColoredPipe::clsnFile;

SpawnInfo ColoredPipe::spawnData =
{
    []() -> ActorBase* { return new ColoredPipe; },
	0x0030,
	0x0100,
	0x00000002,
	0x00000000_f,
	0x005dc000_f,
	0x01000000_f,
	0x01000000_f
};

ColoredPipe* ColoredPipe::Spawn()
{
	return new ColoredPipe;
}

void ColoredPipe::UpdateModelTransform()
{
	model.mat4x3 = Matrix4x3::RotationY(ang.y);
	model.mat4x3.c3.x = pos.x >> 3;
	model.mat4x3.c3.y = pos.y >> 3;
	model.mat4x3.c3.z = pos.z >> 3;
	
	//Pipe rotation for parameters 2 & 3

    model.mat4x3 = model.mat4x3.RotateX(ang.x);
    model.mat4x3 = model.mat4x3.RotateZ(ang.z);
}

int ColoredPipe::InitResources()
{
	
	Model::LoadFile(modelFile);
	model.SetFile(modelFile.BMD(), 1, -1);
	model.data.materials[0].difAmb = 
		model.data.materials[1].difAmb = (param1 & 0x7fff) << 16 | 0x8000;

    MovingMeshCollider::LoadFile(clsnFile);
	clsn.SetFile(clsnFile.KCL(), clsnNextMat, 0x0163_f, ang.y, clpsBlock::instance<>); //0x0175_f
	
	clsn.beforeClsnCallback = (decltype(clsn.beforeClsnCallback))0x02039348;

	UpdateModelTransform();
	UpdateClsnPosAndRot();
	
	return 1;
}

int ColoredPipe::CleanupResources()
{
	clsn.Disable();
	modelFile.Release();
	clsnFile.Release();
	return 1;
}

int ColoredPipe::Behavior()
{
	UpdateModelTransform();
	if(IsClsnInRange(0_f, 0_f))
	{
		Player* player = ClosestPlayer();
		
		if(!player)
			return 1;
		
		//If the player jumped into the pipe
		/*if (player->pos.x < pos.x + 0x50000_f && player->pos.x > pos.x - 0x50000_f && player->pos.z < pos.z + 0x50000_f && player->pos.z > pos.z - 0x50000_f && player->pos.y < pos.y + 0x50000_f && player->pos.y > pos.y + 0x30000_f)
		{
			Sound::Play(2, 71, camSpacePos);
		}*/
		
		UpdateClsnPosAndRot();
	}
	
	return 1;
}

int ColoredPipe::Render()
{
	model.Render(nullptr);
	return 1;
}

ColoredPipe::~ColoredPipe() {}
