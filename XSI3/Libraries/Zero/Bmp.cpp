#include "ZeroTypes.h"

#ifdef TOOL_DEVELOPMENT 

#include "ZeroFile.h"
#include "ZeroStack.h"
#include "ZeroPixels.h"
#include "ZeroGeoTypes.h"
#include "Bmp.h"

#if (PLATFORM != PLATFORM_X86_WIN32)

#define BI_RGB 0		// Verify in Windows correct value!!

struct RGBQUAD
{
	unsigned char rgbBlue;
	unsigned char rgbGreen;
	unsigned char rgbRed;
	unsigned char rgbReserved;
};

struct BITMAPFILEHEADER
{
	unsigned short bfType;
	unsigned int bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int bfOffBits;
};

struct BITMAPINFOHEADER
{
	unsigned int biSize;
	int biWidth;
	int biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
};

#endif

namespace BMP_BITMAP 
{
	#pragma pack(push, 1)
	struct RGB{
		unsigned char r;
		unsigned char g;
		unsigned char b;
	} ;
	#pragma pack(pop)


	static void ReadBmpHeader(ZeroFile &File, BITMAPFILEHEADER &bmfh);
	static void ReadBmpInfo(ZeroFile &File, BITMAPINFOHEADER &bmih);

	bool __cdecl GetDimension (
		const char *File_Name, 
		int &Width, 
		int &Height)
	{
		BITMAPFILEHEADER bmfh;
		BITMAPINFOHEADER bmih;
		
		ZeroFile File (File_Name, "rb");
		if (!File) {
			_ASSERTE (File);
			return false;
		}

		ReadBmpHeader(File, bmfh);
		ReadBmpInfo(File, bmih);		
		
		Width = bmih.biWidth;
		Height = bmih.biHeight;
		
		return true;
	}
	
	
	bool __cdecl Load (
		const char *fileName, 
		unsigned char *bufferBytes, 
		int strideInBytes,
		int &widthInPixels, 
		int &heightInPixels,
		ZeroPixelFormat& pixelFormat,bool &transparent)
	{
		int i;
		int j;
		int k;
		int Index;
		int padding;
		int bytesPerPixels; 
		unsigned int red_shift, red_scale;
		unsigned int green_shift, green_scale;
		unsigned int blue_shift, blue_scale;
		unsigned int alpha_shift, alpha_scale;
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
		unsigned char junk [32];
		BITMAPFILEHEADER bmfh;
		BITMAPINFOHEADER bmih;
		RGBQUAD bmiColors[256];
		ZeroStack<RGB> Scan_Line(4096); 

		transparent = false;
	
		ZeroFile File (fileName, "rb");
		if (!File) {
			_ASSERTE (File);
			return false;
		}
	
		ReadBmpHeader(File, bmfh);
		ReadBmpInfo(File, bmih);		
	
		if (bmih.biCompression != BI_RGB) {
			_ASSERTE	(bmih.biCompression == BI_RGB);
			return false;
		}
	
		// Determine the red, green and blue masks' shift and scale.
//		ddpfPixelFormat = (DDPIXELFORMAT *)pixelFormat;
//		GetPixelMaskAndScale (ddpfPixelFormat->dwRBitMask, red_shift, red_scale);
//		GetPixelMaskAndScale (ddpfPixelFormat->dwGBitMask, green_shift, green_scale);
//		GetPixelMaskAndScale (ddpfPixelFormat->dwBBitMask, blue_shift, blue_scale);
//		GetPixelMaskAndScale (ddpfPixelFormat->dwRGBAlphaBitMask, alpha_shift, alpha_scale);

		GetPixelMaskAndScale (pixelFormat.red.mask, red_shift, red_scale);
		GetPixelMaskAndScale (pixelFormat.green.mask, green_shift, green_scale);
		GetPixelMaskAndScale (pixelFormat.blue.mask, blue_shift, blue_scale);
		GetPixelMaskAndScale (pixelFormat.alpha.mask, alpha_shift, alpha_scale);


		bytesPerPixels = pixelFormat.bitCount / 8;
		if (bytesPerPixels == 2) {
			// load 16 bit texture
			unsigned short D3DPal[256];
			unsigned short *Buffer;

			Buffer = (unsigned short*)bufferBytes;
			strideInBytes /= 2;

			if (bmih.biBitCount == 24) {
				// load a 24 bit map
			
				padding = (4 - ((3 * widthInPixels) & 3)) & 3;
				for (i = heightInPixels - 1; i >= 0; i--) {
					File.Read8 (&Scan_Line[0], 3, widthInPixels);
					File.Read8 (junk, 1, padding);
					Index = strideInBytes * i;
					for (j = 0; j < widthInPixels; j ++) {
						b = (unsigned char)(Scan_Line[j].r >> red_scale);
						g = (unsigned char)(Scan_Line[j].g >> green_scale);
						r = (unsigned char)(Scan_Line[j].b >> blue_scale);
						a = (unsigned char)(255 >> alpha_scale);
						Buffer[Index++] = (unsigned short)
							((r << red_shift) + (g << green_shift) + (b << blue_shift) + (a << alpha_shift));
					}
				}
			} else if (bmih.biBitCount == 8) {
				memset (bmiColors, 0, sizeof (bmiColors));
				if (bmih.biClrUsed) {
					File.Read32 (bmiColors, bmih.biClrUsed, sizeof(RGBQUAD));
				} else	{
					File.Read32 (bmiColors, 256, sizeof(RGBQUAD));
				}
			
				for (i = 0; i < 256; i ++) {
					b = (unsigned char)(bmiColors[i].rgbBlue >> red_scale);  
					g = (unsigned char)(bmiColors[i].rgbGreen >> green_scale);
					r = (unsigned char)(bmiColors[i].rgbRed >> blue_scale); 
					a = (unsigned char)(255 >> alpha_scale);
					D3DPal[i] = (unsigned short)
						((r << red_shift) + (g << green_shift) + (b << blue_shift) + (a << alpha_shift));
				}
			
				padding = (4 - (widthInPixels & 3)) & 3;
				for (i = heightInPixels - 1; i >= 0; i--) {
					File.Read8 (&Scan_Line[0], widthInPixels, 1);
					File.Read8 (junk,1, padding);
					Index = strideInBytes * i;
					for (j = 0; j < widthInPixels; j ++) {
						k = ((unsigned char *)&Scan_Line[0])[j];
						Buffer[Index++] = D3DPal[k];
					}
				}
			} 

		} else {
			unsigned *Buffer;
			unsigned D3DPal[256];

			Buffer = (unsigned*)bufferBytes;
			strideInBytes /= 4;
	
			if (bmih.biBitCount == 24) {
				// load a 24 bit map
			
				padding = (4 - ((3 * widthInPixels) & 3)) & 3;
				for (i = heightInPixels - 1; i >= 0; i--) {
					File.Read8 (&Scan_Line[0], widthInPixels, 3);
					File.Read8 (junk, 1, padding);
					Index = strideInBytes * i;
					for (j = 0; j < widthInPixels; j ++) {
						b = (unsigned char)(Scan_Line[j].r >> red_scale);
						g = (unsigned char)(Scan_Line[j].g >> green_scale);
						r = (unsigned char)(Scan_Line[j].b >> blue_scale);
						a = (unsigned char)(255 >> alpha_scale);
						Buffer[Index++] = (unsigned)
							((r << red_shift) + (g << green_shift) + (b << blue_shift) + (a << alpha_shift));
					}
				}
			
			} else if (bmih.biBitCount == 8) {
				memset (bmiColors, 0, sizeof (bmiColors));
				if (bmih.biClrUsed) {
					File.Read32 (bmiColors, bmih.biClrUsed, sizeof(RGBQUAD));
				} else	{
					File.Read32 (bmiColors, 256, sizeof(RGBQUAD));
				}
			
				for (i = 0; i < 256; i ++) {
					b = (unsigned char)(bmiColors[i].rgbBlue >> red_scale);  
					g = (unsigned char)(bmiColors[i].rgbGreen >> green_scale);
					r = (unsigned char)(bmiColors[i].rgbRed >> blue_scale); 
					a = (unsigned char)(255 >> alpha_scale);
					D3DPal[i] = (unsigned)
						((r << red_shift) + (g << green_shift) + (b << blue_shift) + (a << alpha_shift));
				}
			
				padding = (4 - (widthInPixels & 3)) & 3;
				for (i = heightInPixels - 1; i >= 0; i--) {
					File.Read8 (&Scan_Line[0], widthInPixels, 1);
					File.Read8 (junk,1, padding);
					Index = strideInBytes * i;
					for (j = 0; j < widthInPixels; j ++) {
						k = ((unsigned char *)&Scan_Line[0])[j];
						Buffer[Index++] = D3DPal[k];
					}
				}
			}
		}
			
	
		return true;
	}

	int __cdecl Write (
		const char *fileName,                             
		unsigned char *buffer, 
		int strideInBytes,         
		int bitmapWidthInPixels, 
		int bitmapHeightInPixels,
		const ZeroPixelFormat& pixelFormat)              
	{
		return 0;
	}

	void ReadBmpHeader(ZeroFile &File, BITMAPFILEHEADER &bmfh)
	{
		File.Read16(&bmfh.bfType, sizeof(bmfh.bfType), 1);
		File.Read32(&bmfh.bfSize, sizeof(bmfh.bfSize), 1);
		File.Read16(&bmfh.bfReserved1, sizeof(bmfh.bfReserved1), 1);
		File.Read16(&bmfh.bfReserved1, sizeof(bmfh.bfReserved2), 1);
		File.Read32(&bmfh.bfOffBits, sizeof(bmfh.bfOffBits), 1);
	}

	void ReadBmpInfo(ZeroFile &File, BITMAPINFOHEADER &bmih)
	{
		File.Read32(&bmih.biSize, sizeof(bmih.biSize), 1);
		File.Read32(&bmih.biWidth, sizeof(bmih.biWidth), 1);
		File.Read32(&bmih.biHeight, sizeof(bmih.biHeight), 1);
		File.Read16(&bmih.biPlanes, sizeof(bmih.biPlanes), 1);
		File.Read16(&bmih.biBitCount, sizeof(bmih.biBitCount), 1);
		File.Read32(&bmih.biCompression, sizeof(bmih.biCompression), 1);
		File.Read32(&bmih.biSizeImage, sizeof(bmih.biSizeImage), 1);
		File.Read32(&bmih.biXPelsPerMeter, sizeof(bmih.biXPelsPerMeter), 1);
		File.Read32(&bmih.biYPelsPerMeter, sizeof(bmih.biYPelsPerMeter), 1);
		File.Read32(&bmih.biClrUsed, sizeof(bmih.biClrUsed), 1);
		File.Read32(&bmih.biClrImportant, sizeof(bmih.biClrImportant), 1);
	}
}

#endif //#ifdef TOOL_DEVELOPMENT 
