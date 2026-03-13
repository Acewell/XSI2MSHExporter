//********************************************************************
// Direct geometry Hardware independent library
//	By Julio Jerez
// VC: 6.0
//********************************************************************
#ifndef __ZeroSurface__
#define __ZeroSurface__

#include "ZeroTypes.h"
#include "ZeroGeoTypes.h"
#include "ZeroGeoUtil.h"
#include "ZeroUnknown.h"

const int MAX_SURFACE_STAGES			  = 8;
const unsigned SURFACE_FLAG_OPTIMIZE	  = 0x0001;
const unsigned SURFACE_FLAG_MIPMAPS		  = 0x0002;
const unsigned SURFACE_FLAG_BUMPMAP		  = 0x0004;
const unsigned SURFACE_FLAG_MIP_4		  = 0x0008;
const unsigned SURFACE_FLAG_I8            = 0x0010;
const unsigned SURFACE_FLAG_I8A8          = 0x0020;
const unsigned SURFACE_FLAG_TERRAIN		  = 0x0040;
const unsigned SURFACE_FLAG_NODATA		  = 0x0080;





enum SURFACE_HINT
{
	HINT_INTERFACE_BANK = 0,
	HINT_INTERFACE_SHELL = 0,	// front end textures
	HINT_TRANSPARENT_BANK = 1,
	HINT_OPAQUE_BANK = 2,
	HINT_SPRITE_BANK = 4,
	HINT_TERRAIN_BANK = 20, // was 0x200, now value fits in 8 bits used by priority scheme
};


struct SURFACE_CONTEXT
{
	int pixelFormatIndex;
	unsigned flags;
	unsigned bankHint;
	
	unsigned vramAddress;
	bool wrap;
	//for procedural textures
	void* textureAddress;
	void* paletteAddress;
	unsigned int width;
	unsigned int height;
	unsigned int priority;
	
	
	SURFACE_CONTEXT (int pixelIndex = 0,
		unsigned flagsArg = SURFACE_FLAG_OPTIMIZE | SURFACE_FLAG_MIPMAPS,unsigned hint = HINT_INTERFACE_BANK,bool _wrap = true)
		
		: pixelFormatIndex( pixelIndex )
		, flags( flagsArg )
		, bankHint( hint )
		, vramAddress( 0 )
		, wrap( _wrap )
		, textureAddress(NULL)
		, paletteAddress(NULL)
		, width(0)
		, height(0)
		,priority(0)
	{}
};


struct ZeroSurfaceDriverLoader 
{
	char *ext;
	bool (__cdecl *GetDim) (const char *name, int &widthInPixels, int &heightInPixels);
	bool (__cdecl *Load) (const char *fileName, 
		unsigned char *buffer, int strideInBytes,
		int &bitmapWidthInPixels, int &bitmapHeightInPixels,
		ZeroPixelFormat& pixelFormat, bool &transparent);
};


class ZeroSurface: public ZeroUnknown
{
	ZERO_RTTI(ZeroUnknown);

protected:
	int locksCount;
	int mipMapLevels;
	int widthInPixels;
	int heightInPixels;
	ZeroPixelFormat pixelFormat;
	bool transparent;
	
	
protected:
	//	DLL_DECLSPEC bool LoadBitmap (void* const buffer, int strideInBytes,	const ZeroPixelFormat& pixelFormat);
	DLL_DECLSPEC bool LoadBitmap (void* const buffer, int strideInBytes);
	
	DLL_DECLSPEC ~ZeroSurface ();
	
public:
	DLL_DECLSPEC ZeroSurface (const char *fileName);
	DLL_DECLSPEC ZeroSurface (int width, int height, const SURFACE_CONTEXT *context);
	DLL_DECLSPEC ZeroSurface (const char *fileName, const SURFACE_CONTEXT *context);
	
	bool IsTransparent()
	{
		return transparent;
	}
	
	void SetName (const char *name)
	{
		_ASSERTE (0);
		//		ZeroUnknown::SetName (name);
		//		if (surface) {
		//			surface->Release();
		//			surface = NULL;
		//		}
	}
	
	unsigned GetCRC() const
	{
		return GetNameCRC();
	}
	
	int GetWidth() const
	{
		return widthInPixels;
	}
	
	int GetHeight() const
	{
		return heightInPixels;
	}

	int GetMipMapLevels() const
	{
		return mipMapLevels;
	}
	
	virtual const ZeroPixelFormat& GetPixelFormat () const
	{
		return pixelFormat;
	}
	virtual void* GetData()
	{
		return NULL;
	}
	
	virtual void* Lock (unsigned level)
	{
		return NULL;
	}
	virtual void Unlock (unsigned level)
	{
	}
	
	virtual void SetPixel (int x, int y, ZeroColor color)
	{
	}
	virtual ZeroColor GetPixel (int x, int y) const
	{
		return ZeroColor (0, 0, 0, 0);
	}
	
	virtual bool LoadBitmap ()
	{
		return true;
	}
	virtual void CreateMipMaps()
	{
	} 

};

#endif
