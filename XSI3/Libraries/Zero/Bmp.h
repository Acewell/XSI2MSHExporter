#ifndef __BMP_UNCOMPRESS__
#define __BMP_UNCOMPRESS__

#ifdef TOOL_DEVELOPMENT 

#include "ZeroTypes.h"

class ZeroPixelFormat;

namespace BMP_BITMAP 
{
	DLL_DECLSPEC bool __cdecl GetDimension (const char *fileName, int &widthInPixels, int &HeightInPixels);

	DLL_DECLSPEC bool __cdecl Load (const char *fileName, 
							 unsigned char *buffer, int strideInBytes,
							 int &bitmapWidthInPixels, int &bitmapHeightInPixels,
						     ZeroPixelFormat& pixelFormat, bool &transparent);

	DLL_DECLSPEC int __cdecl Write (const char *fileName,                             
							 unsigned char *buffer, int strideInBytes,         
							 int bitmapWidthInPixels, int bitmapHeightInPixels,
							 const ZeroPixelFormat& pixelFormat);              

}

#endif

#endif //#ifdef TOOL_DEVELOPMENT 
