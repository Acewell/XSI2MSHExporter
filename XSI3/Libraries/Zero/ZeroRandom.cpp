//********************************************************************
// Direct geometry Hardware independent library
//	By Julio Jerez
// VC: 6.0
//********************************************************************
#include "ZeroTypes.h"
#include "ZeroRandom.h"
#include "ZeroRecord.h"

const float ranScale = (1.0f / 0xffffffff);

static unsigned randSeed = 0;

void ZeroSeed (unsigned seed)
{
	ZeroValue::Record((int)seed, ZeroValue::EXTENDED); 
	randSeed = seed;
}

unsigned ZeroRand(void)
{
#ifdef _WIN32
	__asm {
		mov	eax, randSeed 
		mov	edx, 1664525UL	
		mul	edx
		add	eax, 1013904223UL
		mov	randSeed, eax 
	}
#else
	randSeed *= 1664525UL;
	randSeed += 1013904223UL;
#endif
	return randSeed;
}

float ZeroRandFloat(void)
{
	unsigned r = ZeroRand();
	ZeroValue::Record((int)r, ZeroValue::EXTENDED);
	return (r * ranScale);
}

float ZeroRandNormal( float mean, float std_dev )
{
	float x1, x2, w;

	do
	{
		x1 = 2.0f * ZeroRandFloat() - 1.0f;
		x2 = 2.0f * ZeroRandFloat() - 1.0f;
		w = x1 * x1 + x2 * x2;
	}
	while (w >= 1.f || w < 1E-30f);
	
	w = sqrtf( (-2.0f * logf(w)) / w );

	return x1 * w * std_dev + mean;
}
