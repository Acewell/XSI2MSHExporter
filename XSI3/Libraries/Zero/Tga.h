#ifndef _TGA_H
#define _TGA_H   1

#include "ZeroTypes.h"

class ZeroPixelFormat;

namespace TGA_BITMAP 
{
	DLL_DECLSPEC bool __cdecl GetDimension (const char *fileName, int &widthInPixels, int &HeightInPixels);

	DLL_DECLSPEC bool __cdecl Load (const char *fileName, 
							 unsigned char *buffer, int strideInBytes,
							 int &bitmapWidthInPixels, int &bitmapHeightInPixels,
						    ZeroPixelFormat& pixelFormat,bool &transparent);
	DLL_DECLSPEC bool __cdecl LoadRawData (const char *fileName, 
							 unsigned char *buffer, int strideInBytes,
							 int bitmapWidthInPixels, int bitmapHeightInPixels,
						    const ZeroPixelFormat& pixelFormat,bool &transparent,int sizeOfCustomData, unsigned char *customData);


	DLL_DECLSPEC int __cdecl Write (const char *fileName,                             
							 unsigned char *buffer, int strideInBytes,         
							 int bitmapWidthInPixels, int bitmapHeightInPixels,
							 const ZeroPixelFormat& pixelFormat);              
	DLL_DECLSPEC int __cdecl WriteRawData (const char *fileName,                             
							 unsigned char *buffer, int strideInBytes,         
							 int bitmapWidthInPixels, int bitmapHeightInPixels,
							 const ZeroPixelFormat& pixelFormat,int sizeOfCustomData, unsigned char *customData);              
}


#endif  
