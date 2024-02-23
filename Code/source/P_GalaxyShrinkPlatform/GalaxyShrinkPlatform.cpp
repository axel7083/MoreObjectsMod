#include "SM64DS_2.h"
#include "Actors/Custom/GalaxyShrinkPlatform.h"

namespace
{
	using clpsBlock = StaticCLPS_Block<
		{ .camBehavID = CLPS::CA_NORMAL, }
	>;
	
	constexpr Fix12i REGULAR_SCALE = 1._f;
	constexpr Fix12i BIG_SCALE = 1.5_f;
	
	constexpr Fix12i SHRINK_SPEED = 0.025_f;
	
	constexpr Vector3 BIG_FRAME_SCALE = { BIG_SCALE, BIG_SCALE, BIG_SCALE };
}

SpawnInfo GalaxyShrinkPlatform::spawnData =
{
	[]() -> ActorBase* { return new GalaxyShrinkPlatform; },
	0x0030,
	0x0100,
	Actor::NO_RENDER_IF_OFF_SCREEN,
	0._f,
	1500._f,
	4096._f,
	4096._f
};

SharedFilePtr GalaxyShrinkPlatform::modelFile;
SharedFilePtr GalaxyShrinkPlatform::frameModelFile;
SharedFilePtr GalaxyShrinkPlatform::clsnFile;

s32 GalaxyShrinkPlatform::InitResources()
{
	Model::LoadFile(modelFile);
	model.SetFile(modelFile.BMD(), 1, -1);
	
	Model::LoadFile(frameModelFile);
	frameModel.SetFile(frameModelFile.BMD(), 1, -1);
	
	isBig = param1 == 1;
	
	MovingMeshCollider::LoadFile(clsnFile);
	clsn.SetFile(clsnFile.KCL(), clsnNextMat, isBig ? BIG_SCALE : REGULAR_SCALE, ang.y, clpsBlock::instance<>);
	
	clsn.beforeClsnCallback = &MeshColliderBase::UpdatePosWithTransform;	
	clsn.afterClsnCallback = &GalaxyShrinkPlatform::AfterClsnCallback;
	
	Unshrink();
	UpdateModelTransform();
	UpdateClsnPosAndRot();
	
	return 1;
}

s32 GalaxyShrinkPlatform::CleanupResources()
{
	clsn.DisableIfEnabled();
	
	modelFile.Release();
	clsnFile.Release();
	frameModelFile.Release();
	
	return 1;
}

s32 GalaxyShrinkPlatform::Behavior()
{
	if (IsClsnInRange(0._f, 0._f))
	{
		UpdateClsnPosAndRot();

		if (shrinking)
			Shrink();
		
		if (shrinked)
			clsn.DisableIfEnabled();
	}
	else if (shrinked)
	{
		Unshrink();
	}
	
	return 1;
}

s32 GalaxyShrinkPlatform::Render()
{
	if (scale.x != 0._f)
		model.Render(&scale);
	
	frameModel.Render(isBig ? &BIG_FRAME_SCALE : nullptr);
	
	return 1;
}

void GalaxyShrinkPlatform::UpdateModelTransform()
{
	model.mat4x3 = Matrix4x3::RotationY(ang.y);
	model.mat4x3.c3 = pos >> 3;
	
	frameModel.mat4x3 = model.mat4x3;
}

void GalaxyShrinkPlatform::Shrink()
{
	if (scale.x <= 0._f)
	{
		scale.x = scale.z = 0._f;
		
		SmallPoofDust();
		
		shrinking = false;
		shrinked = true;
		
		return;
	}
	
	scale.x -= SHRINK_SPEED;
	scale.z = scale.x;
}

void GalaxyShrinkPlatform::Unshrink()
{
	shrinked = false;
	scale.x = scale.z = isBig ? BIG_SCALE : REGULAR_SCALE;
	
	clsn.Enable(this);
}

void GalaxyShrinkPlatform::AfterClsn(Actor& otherActor)
{
	if (otherActor.actorID == PLAYER_ACTOR_ID)
	{
		shrinking = true;
		
		Sound::Play("NCS_SE_GALAXY_SHRINKING_PLATFORM"sfx, camSpacePos);
	}
}

void GalaxyShrinkPlatform::AfterClsnCallback(MeshColliderBase& clsn, Actor& clsnActor, Actor& otherActor)
{
	((GalaxyShrinkPlatform&)clsnActor).AfterClsn(otherActor);
}

GalaxyShrinkPlatform::GalaxyShrinkPlatform() {}
GalaxyShrinkPlatform::~GalaxyShrinkPlatform() {}