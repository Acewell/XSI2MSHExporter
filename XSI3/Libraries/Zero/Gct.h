#ifndef _gct_h_
#define _gct_h_

#include "ZeroTypes.h"

class ZeroPixelFormat;

namespace GCT_BITMAP 
{
	DLL_DECLSPEC bool __cdecl GetDimension (const char *fileName, int &widthInPixels, int &HeightInPixels);

	DLL_DECLSPEC bool __cdecl Load (const char *fileName, 
							 unsigned char *buffer, int strideInBytes,
							 int &bitmapWidthInPixels, int &bitmapHeightInPixels,
						     ZeroPixelFormat& pixelFormat,bool &transparent);
}

#endif