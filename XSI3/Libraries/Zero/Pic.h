#ifndef __PIC_INCLUDE__
#define __PIC_INCLUDE__

#include "ZeroTypes.h"

class ZeroPixelFormat;

namespace PIC_BITMAP 
{
	DLL_DECLSPEC bool __cdecl GetDimension (const char *fileName, int &widthInPixels, int &HeightInPixels);

	DLL_DECLSPEC bool __cdecl Load (const char *fileName, 
							 unsigned char *buffer, int strideInBytes,
							 int &bitmapWidthInPixels, int &bitmapHeightInPixels,
						    ZeroPixelFormat& pixelFormat,bool &transparent);


	DLL_DECLSPEC int __cdecl Write (const char *fileName,                             
							 unsigned char *buffer, int strideInBytes,         
							 int bitmapWidthInPixels, int bitmapHeightInPixels,
							 const ZeroPixelFormat& pixelFormat);              

}

#endif
