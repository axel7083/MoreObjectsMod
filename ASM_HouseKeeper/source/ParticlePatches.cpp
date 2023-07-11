#include "SM64DS_2.h"

using namespace Particle;

asm("SPA_FILE_PTR = 0x020230d8");

extern "C"
{
	// this was used as a fixed offset to the embedded SPA file
	// now its used as a pointer to the SPA file
	extern ROMEmbeddedFile* SPA_FILE_PTR;
	
	u16 SPA_OV_0_ID = "data/particle_data.spa"ov0;
	
	s32 GetDynamicParticleMemorySize()
	{
		bool inBowserArena = LEVEL_ID == 36 || LEVEL_ID == 38 || LEVEL_ID == 40;
		
		s32 size = 0x3c;
		size += (inBowserArena ? 0x40 : 0x28) * 0x78;
		size += (inBowserArena ? 0x140 : 0x100) * 0x44;
		size += SPA_FILE_PTR->numSysDefs * 0x20;
		size += SPA_FILE_PTR->numTexs * 0x14;
		
		MainInfo* info = &SPA_FILE_PTR->firstSysDef;
		s32 numEffects = 0;
		
		for (s32 i = 0; i < SPA_FILE_PTR->numSysDefs; i++)
		{
			u32 flags = info->flags;
			char* next = ((char*)info) + 0x38;
			
			if (flags & MainInfo::HAS_EFFECT_ACCELERATION)    numEffects++;
			if (flags & MainInfo::HAS_EFFECT_JITTER)          numEffects++;
			if (flags & MainInfo::HAS_EFFECT_CONVERGE)        numEffects++;
			if (flags & MainInfo::HAS_EFFECT_TURN)            numEffects++;
			if (flags & MainInfo::HAS_EFFECT_LIMIT_PLANE)     numEffects++;
			if (flags & MainInfo::HAS_EFFECT_RADIUS_CONVERGE) numEffects++;
			
			if (flags & MainInfo::HAS_SCALE_TRANS)            next += 0xc;
			if (flags & MainInfo::HAS_COLOR_TRANS)            next += 0xc;
			if (flags & MainInfo::HAS_ALPHA_TRANS)            next += 0x8;
			if (flags & MainInfo::HAS_TEX_SEQ)                next += 0xc;
			if (flags & MainInfo::HAS_GLITTER)                next += 0x14;
			if (flags & MainInfo::HAS_EFFECT_ACCELERATION)    next += 0x8;
			if (flags & MainInfo::HAS_EFFECT_JITTER)          next += 0x8;
			if (flags & MainInfo::HAS_EFFECT_CONVERGE)        next += 0x10;
			if (flags & MainInfo::HAS_EFFECT_TURN)            next += 0x4;
			if (flags & MainInfo::HAS_EFFECT_LIMIT_PLANE)     next += 0x8;
			if (flags & MainInfo::HAS_EFFECT_RADIUS_CONVERGE) next += 0x10;
			
			info = (MainInfo*)next;
		}
		
		size += numEffects * 8;
		
		return size;
	}
}

// calculate the reserved particle memory size dynamically instead of using hardcoded values
asm(R"(
nsub_02022f84:
	bl GetDynamicParticleMemorySize
	mov r6, r0
	b 0x02022f88
)");

// load the SPA file from the filesystem instead of using the hardcoded one
asm(R"(
@ these get called when loading a level
nsub_02022f44:
	sub r13, r13, #0x4    @ original instruction
	push {r0,r14}
	ldr r0, =SPA_OV_0_ID
	ldr r0, [r0]
	bl 0x0201816c         @ LoadFile()
	ldr r1, =SPA_FILE_PTR
	str r0, [r1]
	pop {r0,r14}
	b 0x02022f48

nsub_02022ff8:
	ldr r0, =SPA_FILE_PTR
	ldr r0, [r0]
	b 0x02022ffc

nsub_0202300c:
	ldr r0, =SPA_FILE_PTR
	ldr r0, [r0]
	b 0x02023010

nsub_02023018:
	ldr r6, =SPA_FILE_PTR
	ldr r6, [r6]
	b 0x0202301c

nsub_02023074:
	ldr r0, =SPA_FILE_PTR
	ldr r0, [r0]
	b 0x02023078

@ gets called when leaving a level
nsub_020231d0:
	ldr r1, =SPA_FILE_PTR
	push {r0, r2-r8, r14}
	ldr r0, [r1]
	ldr r1, [r1]
	push {r1}
	bl 0x0203c1b4         @ Memory::Deallocate
	pop {r0-r8, r14}
	b 0x020231d4
)");


// enable external SPT files
asm(R"(
@ enables loading particle textures outside of the SPA file

nsub_0204a408:
	ldrb r2, [r10, #0x26]
	b 0x0204a40c

nsub_0204a420: 
	ldrb r0, [r10, #0x27]
	b 0x0204a424

nsub_0204a490: 
	ldrb r2, [r10, #0x27]
	b 0x0204a494

nsub_0204a0f0: 
	ldrb r0, [r10, #0x27]
	b 0x0204a0f4

nsub_0204a154: 
	ldrb r0, [r10, #0x27]
	b 0x0204a158

nsub_0204a03c: 
	ldrb r0, [r10, #0x27]
	b 0x0204a040

nsub_0204a0b4: 
	ldrb r0, [r10, #0x27]
	b 0x0204a0b8
)");

// enable external SysDefs
asm(R"(
@ the code below is taken from an old sm256 demo using no$gba (credits to Josh)

nsub_02021d40:
    cmp r6, #0x02000000
    ldrge r4, [r6]
    ldrlt r4, [r0, r6, lsl #0x5]
    b 0x02021d44
    
nsub_02049e44:
    cmp r7, #0x02000000
    movge r1, r7
    addlt r1, r1, r7, lsl #0x5
    b 0x02049e48
    
nsub_02049e90 = 0x02049e98
    
nsub_02021cf0:
    cmp r2, #0x02000000
    ldrlt r2, [r3, r2, lsl #0x5]
    strlth r12, [r2, #0x28]
    b 0x02021cf8
)");