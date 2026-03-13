//********************************************************************
// Direct geometry Hardware independent library
//	By Julio Jerez
// VC: 6.0
//********************************************************************
#include "ZeroTypes.h"
#include "ZeroFilters.h"

float ZeroIIR2Filter::Update (float xn)
{
	float y;
	
	y = b0 * xn - a1 * my1 - a2 * my2;
	my2 = my1;
	my1 = y;
	return y;
}

ZeroLP2Filter::ZeroLP2Filter (float Damping,
							  float Period)
: ZeroIIR2Filter (0, 0, 0)
{
	
	int i;
	float y;
	
	Damping = 1 - Damping;
	if (Damping > 0.995f) Damping = 0.995f;
	if (Damping < 0.700f) Damping = 0.700f;
	if (Period < 4) Period = 4;
	
	b0 = 1;
	a1 = -2 * Damping * cosf (PI2 / Period);
	a2 = Damping * Damping;
	
	y = 0;
	for (i = 0 ; i < 200; i ++) {
		y = Update(1);
	}
	
	b0 = 1.0f / y;
	SetMemories ();
}



