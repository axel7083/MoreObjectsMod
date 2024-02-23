#include "SM64DS_2.h"

using namespace Particle;

void SysDef::LoadAndSetFile(SharedFilePtr& filePtr)
{
	filePtr.LoadFile();
	
	char* ptrToFile = filePtr.filePtr;
	
	if (filePtr.filePtr == nullptr)
		Crash();
	
	if (filePtr.numRefs != 1)
		return;
	
	SysDef& sysDef = *this;
	u32 sprOffset = 0;
	
	sysDef.info = (MainInfo*)(ptrToFile + sprOffset);
	sprOffset += sizeof(MainInfo);
	
	u32 flags = sysDef.info->flags;
	
	if (flags & MainInfo::HAS_SCALE_TRANS)
	{
		sysDef.scaleTrans = (ScaleTransition*)(ptrToFile + sprOffset);
		sprOffset += sizeof(ScaleTransition);
	}
	else
	{
		sysDef.scaleTrans = nullptr;
	}
	
	if (flags & MainInfo::HAS_COLOR_TRANS)
	{
		sysDef.colorTrans = (ColorTransition*)(ptrToFile + sprOffset);
		sprOffset += sizeof(ColorTransition);
	}
	else
	{
		sysDef.colorTrans = nullptr;
	}
	
	if (flags & MainInfo::HAS_ALPHA_TRANS)
	{
		sysDef.alphaTrans = (AlphaTransition*)(ptrToFile + sprOffset);
		sprOffset += sizeof(AlphaTransition);
	}
	else
	{
		sysDef.alphaTrans = nullptr;
	}
	
	if (flags & MainInfo::HAS_TEX_SEQ)
	{
		sysDef.texSeq = (TexSeq*)(ptrToFile + sprOffset);
		sprOffset += sizeof(TexSeq);
	}
	else
	{
		sysDef.texSeq = nullptr;
	}
	
	if (flags & MainInfo::HAS_GLITTER)
	{
		sysDef.glitter = (Glitter*)(ptrToFile + sprOffset);
		sprOffset += sizeof(Glitter);
	}
	else
	{
		sysDef.glitter = nullptr;
	}
	
	u16 numEffects = 0;
	if (flags & MainInfo::HAS_EFFECT_ACCELERATION)    numEffects++;
	if (flags & MainInfo::HAS_EFFECT_JITTER)          numEffects++;
	if (flags & MainInfo::HAS_EFFECT_CONVERGE)        numEffects++;
	if (flags & MainInfo::HAS_EFFECT_TURN)            numEffects++;
	if (flags & MainInfo::HAS_EFFECT_LIMIT_PLANE)     numEffects++;
	if (flags & MainInfo::HAS_EFFECT_RADIUS_CONVERGE) numEffects++;
	
	sysDef.numEffects = numEffects;
	
	if (sysDef.numEffects == 0)
	{
		sysDef.effects = nullptr;
	}
	else
	{
		sysDef.effects = new Effect[sysDef.numEffects];
		Effect* effect = sysDef.effects;
		
		if (flags & MainInfo::HAS_EFFECT_ACCELERATION)
		{
			effect->data = (EffectData*)(ptrToFile + sprOffset);
			sprOffset += sizeof(Acceleration);
			effect->func = &Acceleration::Func;
			effect++;
		}
		if (flags & MainInfo::HAS_EFFECT_JITTER)
		{
			effect->data = (EffectData*)(ptrToFile + sprOffset);
			sprOffset += sizeof(Jitter);
			effect->func = &Jitter::Func;
			effect++;
		}
		if (flags & MainInfo::HAS_EFFECT_CONVERGE)
		{
			effect->data = (EffectData*)(ptrToFile + sprOffset);
			sprOffset += sizeof(Converge);
			effect->func = &Converge::Func;
			effect++;
		}
		if (flags & MainInfo::HAS_EFFECT_TURN)
		{
			effect->data = (EffectData*)(ptrToFile + sprOffset);
			sprOffset += sizeof(Turn);
			effect->func = &Turn::Func;
			effect++;
		}
		if (flags & MainInfo::HAS_EFFECT_LIMIT_PLANE)
		{
			effect->data = (EffectData*)(ptrToFile + sprOffset);
			sprOffset += sizeof(LimitPlane);
			effect->func = &LimitPlane::Func;
			effect++;
		}
		if (flags & MainInfo::HAS_EFFECT_RADIUS_CONVERGE)
		{
			effect->data = (EffectData*)(ptrToFile + sprOffset);
			sprOffset += sizeof(RadiusConverge);
			effect->func = &RadiusConverge::Func;
		}
	}
}

void SysDef::Release(SharedFilePtr& filePtr)
{
	filePtr.Release();
	
	if (filePtr.numRefs == 0 && effects != nullptr)
		delete effects;
}