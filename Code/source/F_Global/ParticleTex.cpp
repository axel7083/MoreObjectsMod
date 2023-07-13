#include "SM64DS_2.h"

using namespace Particle;

bool Manager::LoadTex(u32 ov0FileID, u32 newTexID)
{
	if (newTexID >= PARTICLE_SYS_TRACKER->manager->numTextures)
		Crash();
	
	TexDef& texDef = PARTICLE_SYS_TRACKER->manager->texDefArr[newTexID];
	if (texDef.texture != nullptr)
		return true;
	
	texDef.texture = (Texture*)SharedFilePtr{}.Construct(ov0FileID).Load();
	if (texDef.texture == nullptr)
		return false;
	
	texDef.flags  = texDef.texture->flags;
	texDef.width  = texDef.texture->Width();
	texDef.height = texDef.texture->Height();
	
	GX::BeginLoadTex();
	texDef.texVramOffset = Texture::AllocTexVram(texDef.texture->texelArrSize, texDef.texture->Format() == 5);
	GX::LoadTex(texDef.texture->TexelArr(), texDef.texVramOffset, texDef.texture->texelArrSize);
	GX::EndLoadTex();
	
	GX::BeginLoadTexPltt();
	texDef.palVramOffset = Texture::AllocPalVram(texDef.texture->palleteSize, texDef.texture->Format() == 2);
	GX::LoadTexPltt(texDef.texture->PalleteColArr(), texDef.palVramOffset, texDef.texture->palleteSize);
	GX::EndLoadTexPltt();
	
	return true;
}

void Manager::UnloadNewTexs()
{
	TexDef* texDefArr = PARTICLE_SYS_TRACKER->manager->texDefArr;
	s32 numTexDefs = PARTICLE_SYS_TRACKER->manager->numTextures;
	
	for (s32 i = PARTICLE_SYS_TRACKER->manager->numBuiltInTexs; i < numTexDefs; i++)
	{
		if (texDefArr[i].texture != nullptr)
			delete texDefArr[i].texture;
	}
}