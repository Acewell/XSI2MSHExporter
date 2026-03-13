/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroMeshContext__
#define __ZeroMeshContext__

#include "ZeroTypes.h"

class ZeroFile;
class ZeroLight;
class ZeroVideo; 
class ZeroObject;
class ZeroSurface;
class ZeroMaterial;
class ZeroSegmenData;
class ZeroMeshContext;
class ZeroGeometryData;
class ZeroLightDescriptor;


class ZEROMESH_DRIVER 
{
	public:
	char *ext;
	ZeroObject * (__cdecl *Load)(const char *Name, const ZeroMeshContext *Context);
	bool (__cdecl *Save) (const char *Name, ZeroObject *Obj);
};

// mesh context
class ZeroMeshContext
{
	public:
	// constructor
	ZeroMeshContext ()
	{
	}

	// destructor
	virtual ~ZeroMeshContext () 
	{
	}

	// get video manager
	virtual ZeroVideo *VideoManager(void) const
	{
		return NULL;
	}

	// get main camera
	virtual ZeroCamera *MainCamera(void) const
	{
		return NULL;
	}

	virtual void OnFileLoaded(ZeroObject *mesh) const;

	// should add geometry?
//	virtual bool AddGeometry (const char *geoName) const
//	{
//		return true;
//	}

	// get a surface
	virtual ZeroSurface *GetSurface (const char *name) const
	{
		return NULL;
	}

	// get a material
	virtual ZeroMaterial *GetMaterial (const char *name,
		const char *tex[], const unsigned attrib) const
	{
		return NULL;
	}

	// get a light
	virtual ZeroLight *GetLight (const ZeroLightDescriptor &desc) const
	{
		return NULL;
	}

	// get a camera
	virtual ZeroCamera *GetCamera (const float nearPlane,
								   const float farPlane,
								   const float fov,
								   const float zoom) const
	{
		return NULL;
	}

	virtual void ParseUnknownBlock (ZeroFile &file, unsigned size);

	virtual ZeroObject *GetObject (const ZeroObjectData& data) const;
	virtual ZeroGeometry *GetGeometry (const ZeroGeometryData& geometryData) const;
	virtual ZeroSegment *GetSegment (const ZeroSegmenData& segmentData) const;

	virtual void ParseExtraBlocks(ZeroFile &file, int &size, ZeroObject *obj)
	{
		// parse application specific blocks after the basic mesh
	}

};



#endif
