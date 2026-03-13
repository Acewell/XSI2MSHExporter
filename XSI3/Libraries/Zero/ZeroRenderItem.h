#ifndef __ZeroRenderItem__
#define __ZeroRenderItem__

#include "ZeroTypes.h"
#include "ZeroUnknown.h"



class ZeroCamera;
class ZeroMaterial;
class ZeroObject;


class ZeroRenderItem: public ZeroUnknown 
{
public:

	// render the segment
	DLL_DECLSPEC virtual void Render(ZeroCamera *camera, const ZeroObject* aOwner = NULL, const float aAlpha = 1.0f) = 0;
	virtual unsigned int GetMaterialAttrib() const = 0;
};

#endif

