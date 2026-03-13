/****************************************************************************
*                                                                           *
*            Texture Lights, implemented as Attenuation Maps                *
*                                                                           *
*     Author : Adam Batters                                                 *
*     Notes  : Only 1 texture light will be active on any one object in     *
*              any frame through the standard Zero Pipeline (Terrain may    *
*              be different) it will be the light that is closest to the    *
*              object as determined by it's implementations distance fn     *
*                                                                           *
****************************************************************************/
#ifndef __ZeroTextureLight__
#define __ZeroTextureLight__

#include "Zero.h"

// this allows the app to create it's own attenuation maps with wacky shapes
// there should only be one global instance of each shape created that builds
// your objects derived from ZeroTxLight
class ZeroTxLightShape
{
protected:
	int mUniqueId;

public:
	// list of shapes
	static ZeroVisList<ZeroTxLightShape *>& GetShapeList();
	// init all registered shape implementations
	static void InitTxLightShapes(ZeroVideo *aVideo);
	// build a light of an unknown shape
	static ZeroTxLight *Build(int aUniqueId, ZeroUnknown &data);

	// base constructor
	ZeroTxLightShape(int aUniqueId);

	// build a light of this shape
	virtual ZeroTxLight *Build(ZeroUnknown &data) = 0;

	// override this to setup the implementation
	virtual bool Init(ZeroVideo *aVideo) = 0;

	// get the Surfaces to perform light attenuation
	virtual ZeroSurface *GetAttenuationMap() = 0;
	virtual ZeroSurface *GetLightMap() = 0;
};

// this is the actual light object, derive from this and return it in your
// shapes build function all INSTANCE data should go in here 
class ZeroTxLight : public ZeroObject
{
	ZERO_RTTI(ZeroObject);

protected:
#if (PLATFORM != PLATFORM_PS2)
	static bool s_enable;
#endif
	bool enabled;
	bool visible;
	bool active;

	virtual ~ZeroTxLight();

	// no need for anyone but TxLight to call these
	virtual void SetActive(bool aActive);
	virtual void SetVisible(bool aActive);

public:
	ZeroTxLightShape         *mShape;

	static ZeroVisList<ZeroTxLight *>& GetActiveList();
	static ZeroVisList<ZeroTxLight *>& GetCameraList();
	static ZeroVisList<ZeroTxLight *>& GetEnabledList();
	static void InitTxLights(ZeroVideo *aVideo);

#if (PLATFORM != PLATFORM_PS2)
	static void Enable(bool enabled) { s_enable = enabled; }
	static bool IsEnabled() { return s_enable; }
#else
	static inline void Enable(bool enabled) { }
	static inline bool IsEnabled() { return false;}
#endif

	ZeroTxLight(ZeroTxLightShape *aShape);

	// returns the distance from a texture light to the objects sphere
	virtual float GetDistance(const ZeroSphere &aSphere, ZeroCamera *aCamera) = 0;
	// get the x, y and z scale for map
	virtual ZeroVector3 GetScale() = 0;
	// get color
	virtual ZeroColorValue GetColor() = 0;

	// goes through the enabled list and activates lights
	static void SetActive(const ZeroSphere *aObjectSphere);
	static void SetCamera(const ZeroCamera *aCamera);
	virtual void SetEnable (bool aEnable);
	virtual bool GetEnable (void);
};

#endif
