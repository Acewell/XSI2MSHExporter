//********************************************************************
// Direct geometry Hardware independent library
//	By Julio Jerez
// VC: 6.0
//********************************************************************
#include "ZeroTypes.h"
#include "ZeroLight.h"

ZeroLight::ZeroLight (const ZeroLightDescriptor &desc)
: ZeroObject ("light"), desc(desc)
{
}

ZeroLight::ZeroLight (const ZeroLight &light)
: ZeroObject(light), desc(light.desc)
{
}

ZeroLight::~ZeroLight ()
{
}

ZeroUnknown *ZeroLight::CreateClone (bool isSim)
{
	//ZeroWarn ((isSim, "ZeroLight - sim clone requested\n"));
	return new (isSim) ZeroLight (*this);
}

void ZeroLight::CloneFixUp (const ZeroBaseNode &clone)
{
	//SetEnable(true);
	SetProperties();
}
