//********************************************************************
// Direct geometry Hardware independent library
//	By Julio Jerez
// VC: 6.0
//********************************************************************
#include "ZeroTypes.h"
#include "ZeroSurface.h"
#include "ZeroResource.h"
#include "ZeroDebug.h"

#include "Bmp.h"
#include "Pic.h"
#include "Gct.h"

ZERO_RTTI_CONTRUCTOR(ZeroSurface);

#if (PLATFORM == PLATFORM_PS2)
#include "Tga_ps2.h"
#else
#include "Tga.h"
#endif

namespace InternalSurface
{
	class SurfaceLoader: public ResourceLoader<ZeroSurfaceDriverLoader>
	{
	public:
		
		static SurfaceLoader *loader;
		
		SurfaceLoader()
			:ResourceLoader<ZeroSurfaceDriverLoader>()
		{
			SetDefualtFormats ();
		}
		
		friend void Startup();
		
		void SetDefualtFormats ()
		{
			int i;
			int count;
			static ZeroSurfaceDriverLoader bitmapExt[] =
			{
#if (PLATFORM == PLATFORM_GAMECUBE)
				{".gct", GCT_BITMAP::GetDimension, GCT_BITMAP::Load},
#else
				{".tga", TGA_BITMAP::GetDimension, TGA_BITMAP::Load},
				{".pic", PIC_BITMAP::GetDimension, PIC_BITMAP::Load},
#endif
			};
			
			count = sizeof (bitmapExt) / sizeof (ZeroSurfaceDriverLoader);
			for (i = 0; i < count; i ++) {
				AddResourceLoader (bitmapExt[i]);
			}
		}
	};
	
	SurfaceLoader *GetLoader()
	{
		ZeroStartup::InitEngine();
		static SurfaceLoader loader;
		return &loader;
	}

	void Startup()
	{
		GetLoader();
	}
}



ZeroSurface::ZeroSurface (const char *fileName)
	:ZeroUnknown (fileName)
{
	locksCount = 0;
	
	mipMapLevels = 0;
	widthInPixels = 0;
	heightInPixels = 0;
	memset (&pixelFormat, 0, sizeof (pixelFormat));
	transparent = false;
}


ZeroSurface::ZeroSurface (
	const char *fileName,
	const SURFACE_CONTEXT *context)
	:ZeroUnknown (fileName)
{
	char *ext;
	ZeroSurfaceDriverLoader *loader;
	
	locksCount = 0;
	
	mipMapLevels = 0;
	widthInPixels = 0;
	heightInPixels = 0;
	memset (&pixelFormat, 0, sizeof (pixelFormat));
	transparent = false;

	ext = strrchr (fileName, '.');
	if (!ext) {
		ZeroTrace (("texture %s has no extension\n", fileName));
#ifdef _WIN32
		throw "No extension";
#endif
	}
	
	loader = InternalSurface::GetLoader()->FindLoader (ext);
	if (!loader) {
		ZeroTrace (("texture %s has no loader\n", fileName));
#ifdef _WIN32
		throw "Texture loader not found";
#endif
	}
	
	if (loader->GetDim (fileName, widthInPixels, heightInPixels) == false) {
		ZeroTrace (("texture %s not found\n", fileName));
#ifdef _WIN32
		throw	"texture file not found";
#endif
	}

#ifndef RECTANGULAR_TEXTURES
	if (widthInPixels != heightInPixels) {
		ZeroTrace (("texture %s is not square\n", fileName));
#ifdef _WIN32
		throw	"texture is not square";
#endif
	}
#endif

	if ( ( (widthInPixels - 1) & widthInPixels ) || ( (heightInPixels - 1) & heightInPixels ) )	 {
		ZeroTrace (("texture %s is not power of two\n", fileName));
#ifdef _WIN32
		throw	"texture is not power fo two";
#endif
	}
}


ZeroSurface::ZeroSurface (
	int widthArg,
	int heightArg,
	const SURFACE_CONTEXT *context)
	: ZeroUnknown ()
{
	locksCount = 0;
	
	widthInPixels = widthArg;
	heightInPixels = heightArg;

	memset (&pixelFormat, 0, sizeof (pixelFormat));
	
	mipMapLevels = 0;

#ifndef RECTANGULAR_TEXTURES
	if (widthInPixels	!= heightInPixels) {
		ZeroTrace (("texture is not square\n"));
#ifdef _WIN32
		throw	"texture is not square";
#endif
	}
#endif

#ifndef ALLOW_ANY_TEXTURE_SIZE
	if ((widthInPixels - 1) & widthInPixels)	 {
		ZeroTrace (("texture width is not power of two\n"));
#ifdef _WIN32
		throw "texture is not power of two";
#endif
	}
	if ((heightInPixels - 1) & heightInPixels)	 {
		ZeroTrace (("texture height is not power of two\n"));
#ifdef _WIN32
		throw "texture is not power of two";
#endif
	}
#endif
}

ZeroSurface::~ZeroSurface ()
{
}

bool ZeroSurface::LoadBitmap(
	void* const buffer, 
	int strideInBytes)
{
	ZeroSurfaceDriverLoader *loader;
	
#if (PLATFORM ==PLATFORM_PS2)
	//in the ps2 version, all the files are tga (at least for now)
	const char *name1 = GetName();
	char name[64];
	strcpy(name,name1);
	int len = strlen(name);
	name[len-1] = 'a';
	name[len-2] = 'g';
	name[len-3] = 't';
#else
	const char *name = GetName();
#endif
	const char *ext = strrchr (name, '.');
	if (!ext) {
		return false;
	}
							 
	loader = InternalSurface::GetLoader()->FindLoader (ext);

	if (!loader) {
		return false;
	}

	return loader->Load (name, (unsigned char*)buffer, strideInBytes, widthInPixels, heightInPixels, pixelFormat,transparent);
}
