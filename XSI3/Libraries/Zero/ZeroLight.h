//********************************************************************
// Direct geometry Hardware independent library
//	By Julio Jerez
// VC: 6.0
//********************************************************************
#ifndef __ZeroLight__
#define __ZeroLight__

#include "ZeroObject.h"
#include "ZeroGeoUtil.h"

#define LIGHT_CAST_SHADOW 1

class ZeroVideo;

enum LightType
{
	SpotLight = 0,
	PointLight = 1,
	DirectionalLight = 2,
	MovingSpotLight = 3
};

class ZeroLightDescriptor
{

public:
	LightType type;
	float range;
	float innerCone;
	float outerCone;
	
	ZeroColorValue ambient;
	ZeroColorValue diffuse;
	ZeroColorValue specular;
	float dvAttenuation0;
	float dvAttenuation1;
	float dvAttenuation2;

	unsigned flags;
	unsigned shadowColor;
  bool     bCastShadow;
};

class ZeroLight: public ZeroObject
{
//	ZERORTTI(ZeroLight);

protected:
	ZeroLightDescriptor desc;
	
	DLL_DECLSPEC ZeroLight (const ZeroLightDescriptor &desc);
	DLL_DECLSPEC ZeroLight (const ZeroLight &light);
	DLL_DECLSPEC ~ZeroLight ();

	DLL_DECLSPEC ZeroUnknown *CreateClone (bool isSim);
	DLL_DECLSPEC void CloneFixUp (const ZeroBaseNode &clone);

public:
	
	virtual void SetProperties()
	{
	}
	
	void SetProperties (const ZeroLightDescriptor &descArg)
	{
		desc = descArg;
		SetProperties();
	}

	const ZeroLightDescriptor &GetProperties(void) const
	{
		return desc;
	}

	virtual void SetEnable (bool enable)
	{
	}

	virtual bool GetEnable (void)
	{
		return false;
	}

	virtual void SetActive (bool active)
	{
	}

	virtual void SetActive (ZeroVideo* video,bool active)
	{
	}
	
	virtual bool GetActive (void)
	{
		return false;
	}
	
	virtual LightType GetType(void) {

		return desc.type;
	}
	
	virtual ZeroVector3 GetDirection(void) {
	
		return ZeroVector3(0,0,0);
	}
};

#endif


