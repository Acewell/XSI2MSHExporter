/****************************************************************************
*
*  File Name  : vehicle.cpp
*  Visual C++ 4.0 base by Julio Jerez
*
****************************************************************************/
#ifndef __InfinityMass__
#define __InfinityMass__


#include "ZeroTypes.h"
#include "ZeroDebug.h"
#include "ZeroTransform.h"

class ZeroInfiniteMassPhysics: public ZeroTransform
{
protected:
	// copy constructor
	DLL_DECLSPEC ZeroInfiniteMassPhysics (const ZeroInfiniteMassPhysics &transform, ZeroObject *ownerArg);
	
public:
	// constructor
	DLL_DECLSPEC ZeroInfiniteMassPhysics (ZeroObject *owner);

	// clone the object
	ZeroTransform *CreateClone(ZeroObject *ownerArg, bool isSim)
	{
		return new (isSim) ZeroInfiniteMassPhysics (*this, ownerArg);
	}
	
	void ImpulsiveFrictionConstants(
		float& normalRestitution, 
		float& tangentRestitution) 
	{
		// between 0.0 and 1.0
		normalRestitution = 0.2f;
		// between 0.0 and 1.0
		tangentRestitution = 1.0f;
	}
};

#endif

