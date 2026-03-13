/****************************************************************************
*                                                                           *
*     Implementation of Attenuation Maps                                    *
*     Author : Adam Batters                                                 *
*                                                                           *
****************************************************************************/
#include "ZeroTypes.h"
#include "ZeroTextureLight.h"
#include "ZeroSurfaceState.h"
#include "ZeroCameraSphere.h"
#include "ZeroCamera.h"
#include "ZeroList.h"

ZERO_RTTI_CONTRUCTOR(ZeroTxLight);

#if (PLATFORM != PLATFORM_PS2)
bool ZeroTxLight::s_enable = true;
#endif

ZeroVisList<ZeroTxLightShape *>& ZeroTxLightShape::GetShapeList()
{
	static ZeroVisList<ZeroTxLightShape *> shapelist;

	return shapelist;
}

void ZeroTxLightShape::InitTxLightShapes(ZeroVideo *aVideo)
{
	ZeroList<ZeroTxLightShape *>::Iterator iter(ZeroTxLightShape::GetShapeList());
	for (iter.Begin(); iter; iter ++) 
	{
		ZeroTxLightShape *tls = (*iter);
		tls->Init(aVideo);
	}
}

ZeroTxLight *ZeroTxLightShape::Build(int aUniqueId, ZeroUnknown &data)
{
	ZeroList<ZeroTxLightShape *>::Iterator iter(ZeroTxLightShape::GetShapeList());
	for (iter.Begin(); iter; iter ++) 
	{
		ZeroTxLightShape *tli = (*iter);

		if (tli->mUniqueId == aUniqueId)
			return tli->Build(data);
	}
	return NULL;
}


ZeroTxLightShape::ZeroTxLightShape(int aUniqueId)
{
	mUniqueId = aUniqueId;
#ifdef _DEBUG
	ZeroList<ZeroTxLightShape *>::Iterator iter(ZeroTxLightShape::GetShapeList());
	for (iter.Begin(); iter; iter ++) 
	{
		ZeroTxLightShape *tli = (*iter);
		if (tli->mUniqueId == aUniqueId)
			ZeroAssert((0u, "Two Texture Light Shapes share the same Id"));
	}
#endif
	GetShapeList().Append(this);
}

/****************************************************************************
*                                                                           *
*        Texture Light                                                      *
*                                                                           *
****************************************************************************/
ZeroTxLight::ZeroTxLight(ZeroTxLightShape *aShape)
	: ZeroObject("texture_light")
{
	mShape  = aShape;
	enabled = false;
	visible = false;
	active  = false;
}

ZeroTxLight::~ZeroTxLight()
{
	SetEnable(false);
	SetVisible(false);
	SetActive(false);
}

void ZeroTxLight::InitTxLights(ZeroVideo *aVideo)
{
	ZeroTxLightShape::InitTxLightShapes(aVideo);

	// force some globals to be constructed
	GetCameraList();
	GetEnabledList();
	GetActiveList();
}

/****************************************************************************
*                                                                           *
*        Active and Enabled List stuff                                      *
*                                                                           *
****************************************************************************/

ZeroVisList<ZeroTxLight *>& ZeroTxLight::GetEnabledList()
{
	static ZeroVisList<ZeroTxLight *> enabledlist; // this is a list of enabled lights !!
	return enabledlist;
}

ZeroVisList<ZeroTxLight *>& ZeroTxLight::GetCameraList()
{
	static ZeroVisList<ZeroTxLight *> cameralist; // this is a list of lights near to the camera !!
	return cameralist;
}

ZeroVisList<ZeroTxLight *>& ZeroTxLight::GetActiveList()
{
	static ZeroVisList<ZeroTxLight *> activelist;  // this is a list of active texture lights !!
	return activelist;
}

void ZeroTxLight::SetEnable(bool aEnable)
{
	if (enabled && !aEnable)
	{
		SetVisible(false);
		SetActive(false);
		GetEnabledList().Remove(this);
	}
	else if (!enabled && aEnable)
	{
		GetEnabledList().Append(this);
	}
	enabled = aEnable;
}

bool ZeroTxLight::GetEnable(void)
{
	return enabled;
}

void ZeroTxLight::SetActive(bool aActive)
{
	if (active && !aActive)
		GetActiveList().Remove(this);
	else if (!active && aActive)
		GetActiveList().Append(this);
	active = aActive;
}

void ZeroTxLight::SetVisible(bool aVisible)
{
	if (visible && !aVisible)
	{
		SetActive(false);
		GetCameraList().Remove(this);
	}
	else if (!visible && aVisible)
		GetCameraList().Append(this);
	visible = aVisible;
}

#define LIGHT_DISTANCE 400.0f

static ZeroCamera *s_Camera = NULL;

void ZeroTxLight::SetCamera(const ZeroCamera *aCamera)
{
	ZeroCameraSphere sphere(identitySphere);
	ZeroVector3 cam_wld_pos = aCamera->GetWorldPosition(ZeroVector3(0,0,0));

//	GetCameraList().RemoveAll();

	ZeroList<ZeroTxLight*>::Iterator iter (ZeroTxLight::GetEnabledList());
	for (iter.Begin(); iter; iter ++) 
	{
		ZeroTxLight *light  = (*iter);

		ZeroVector3 lt_wld_pos = light->GetWorldPosition(ZeroVector3(0,0,0));
		if (MagnitudeFast(cam_wld_pos - lt_wld_pos) > LIGHT_DISTANCE)
		{
			light->SetVisible(false);
			continue;
		}

		ZeroVector4 dim(light->GetDimensions());
		sphere.size = light->GetWorldDirection(dim);
		sphere.size.w = dim.w;
		sphere.posit  = light->GetWorldPosition(ZeroVector3(0, 0, 0));

		light->SetVisible(sphere.VisibilityTest(aCamera) >= 0);
	}
	s_Camera = (ZeroCamera *)aCamera;
}

void ZeroTxLight::SetActive(const ZeroSphere *aObjectSphere)
{
	if (!aObjectSphere)
	{
		ZeroTxLight::GetActiveList().RemoveAll();
		return;
	}

//	GetActiveList().RemoveAll();
	ZeroList<ZeroTxLight*>::Iterator iter (ZeroTxLight::GetCameraList());
	for (iter.Begin(); iter; iter ++) 
	{
		ZeroTxLight *light = (*iter);
		ZeroVector3 scale = light->GetScale();

		scale.x = 0.5f / scale.x;
		scale.y = 0.5f / scale.y;
		scale.z = 1.0f / scale.z;

		float maxdist = scale.x;
		if (scale.y > maxdist)
			maxdist = scale.y;
		if (scale.z > maxdist)
			maxdist = scale.z;

		float dist = light->GetDistance(*aObjectSphere, s_Camera);
		if (dist < maxdist)
			light->SetActive(true);
		else
			light->SetActive(false);
	}
}
