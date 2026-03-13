//********************************************************************
// Direct geometry Hardware independent library
//	By Julio Jerez
// VC: 6.0
//********************************************************************
#ifndef __ZeroVideo__
#define __ZeroVideo__

#include "ZeroTypes.h"
#include "ZeroList.h"
#include "Msh.h"
#include "ZeroSphere.h"
#include "ZeroGeoTypes.h"
#include "ZeroSegment.h"
#include "ZeroRenderDescriptor.h"


//enum ZeroPrimitiveType;
class ZeroVideo;
class ZeroLight;
class ZeroCamera;
class ZeroSurface;
class ZeroSegment;
class ZeroMaterial;
struct ZeroMaterialDesc;
class ZeroParticleCache;
class ZeroSpriteCache;
class ZeroRenderState;
class ZeroPixelFormat;
class ZeroLightDescriptor;
class ZeroRenderDescriptor;
class ZeroTxLight;
struct SURFACE_CONTEXT;


// what is this shit	here
class ZeroTerrain;


#define	ZBUFFER_16 				1
#define	ZBUFFER_24				2
#define	ZBUFFER_32 				4
#define	STENCIL_ZBUFFER_24	8

struct ZeroVideoMode
{
	int width;
	int height;
	int zbuffer;
	int bitdepth;
	int windowed;
	unsigned reserve1;
	unsigned reserve2;
	unsigned reserve3;
	unsigned reserve4;
};
#ifdef WIN32
class ZeroVideoModeList: public ZeroVisList<ZeroVideoMode>
{
	public:
	ZeroVideoModeList()	: ZeroVisList<ZeroVideoMode>()				{}
};
#endif


class ZeroVideoConstructor : public ZeroUnknown
{
protected:
	int crc;
	
public:
	DLL_DECLSPEC ZeroVideoConstructor (const char *name, const void* ptr = NULL);
	DLL_DECLSPEC ~ZeroVideoConstructor();

	DLL_DECLSPEC virtual ZeroVideo* CreateVideo (HWND win)		{ return NULL; }
};


class ZeroVideo: public ZeroUnknown
{
protected:
	HWND win;
#ifdef WIN32
	ZeroVideoModeList videoModesList;
#endif
	int		opaqueIndex;
	int		translucentIndex;

	virtual ~ZeroVideo()	{}

	ZeroVideo (HWND window);
	
public:

	static DLL_DECLSPEC ZeroVideo* __cdecl CreateVideo (const char *deviceName, HWND win);
	
	virtual void					SetShellEnvironment()		{}
	virtual void					UploadAllTextures()			{}
	virtual void					ExitShellEnvironment()		{}

	HWND							GetWindowHandle()			{ return win; }

#ifdef WIN32
	const ZeroVideoModeList&		GetVideoModes() const		{ return videoModesList; }
#endif

	int								GetOpaqueIndex()			{ return opaqueIndex; }
	int								GetTranslucentIndex()		{ return translucentIndex; }

	virtual void					SetVideoMode (const ZeroVideoMode& newMode) = 0;
	virtual const ZeroVideoMode &	GetVideoMode (void) = 0;
	virtual ZeroLight*				CreateLight (const ZeroLightDescriptor &desc) = 0;
	virtual int						GetAllLights (ZeroLight **lightList, int maxCount) = 0;
	virtual int						GetEnabledLights (ZeroLight **lightList, int maxCount) = 0;
	virtual int						GetActiveLights (ZeroLight **lightList, int maxCount) = 0;
	virtual void					SetActiveLights (bool active) = 0;
	virtual void					SetActiveLights (const ZeroSphere &sphere) = 0;
	
	virtual ZeroCamera*				CreateCamera (float nearPlane, float farPlane, float fov = 90.0f * DEG2RAD, float zoomFactor = 1.0f) = 0;
	virtual ZeroSurface*			UseSurface (const char *name, const SURFACE_CONTEXT& context) = 0; 
	virtual ZeroSurface*			MakeSurface (int width, int height, const SURFACE_CONTEXT& context) = 0;

	virtual void					RefreshAllSurfaces (void)		{}	//Should be implemented for each platform! ( = 0 at a later point)

	virtual ZeroSegment *		CreateSegment( const ZeroSegmenData& segmentData ) = 0;

#ifdef PURE_ABSTRACT_MATERIAL
	virtual ZeroMaterial *		CreateMaterial( const ZeroMaterialDesc &Desc ) = 0;
#else
	virtual ZeroMaterial *		CreateMaterial( const ZeroMaterialDesc &Desc  )	{ ZeroAssert((0u, "BAD")); return 0; }		// = 0 at a later point  /Smedis
#endif

	virtual ZeroParticleCache *	CreateParticleCache(const char *textureName, ZeroRenderBlendMode aBlendMode) = 0;
	virtual ZeroSpriteCache *	CreateSpriteCache(const char *textureName, float spriteWidth, float spriteHeight, float spriteScale) = 0;

	virtual void			Flush()						{}	//Should be implemented for each platform! ( = 0 at a later point)
							
						
	virtual void			SetRenderStates(ZeroRenderState * states) = 0;
												
	virtual void			Blit () = 0;
	virtual void			ClearZBuffer () = 0;
	virtual void			BlitFill (ZeroColor color) = 0; 

//#if (PLATFORM == PLATFORM_PS2 )
//	virtual ZeroTerrain* CreateTerrain (char *filename) {}
//	virtual void BeginFrame(ZeroCamera* camera) = 0;
//	virtual void EndFrame(ZeroCamera* camera) = 0;
//	virtual ZeroCamera* CreateSecondCamera (float nearPlane, float farPlane, float fov = 90.0f * DEG2RAD, float zoomFactor = 1.0f) {};

	virtual void			EndFrame(ZeroCamera * cam)					{}
	virtual void			BeginFrame(ZeroCamera * cam)				{}
	virtual void			BeginScene(ZeroCamera * cam)				{}
	virtual void			EndScene(ZeroCamera * cam)					{}

	virtual void			MoveWindow ()				{}
	virtual void			ClearStencilBuffer()		{}
	virtual void			SetClientRectangle ()		{}
	virtual int				GetPixelsFormats (ZeroPixelFormat* pixelsFormats, int maxCount)		{ return 0; }

	virtual void			OutputText (int x, int y, ZeroColor color, char const *str) const	{}

	virtual ZeroTxLight*	CreateTxLight(int aShape, ZeroUnknown &aData);
	virtual ZeroTerrain*	CreateTerrain (char *filename)										{ return NULL; }

	virtual void BeginBatch(const char *name) {}
	virtual void EndBatch(void) {}
	
	virtual void Preset(void) {}
	virtual void Reset(void) {}

	virtual void SetBrightness(float brightness) {}
	virtual void CPUGPUSync(void) {}

};


#endif
