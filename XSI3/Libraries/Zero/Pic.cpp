#include "ZeroTypes.h"
#include "ZeroFile.h"
#include "ZeroStack.h"
#include "ZeroPixels.h"
#include "ZeroDebug.h"
#include "ZeroGeoTypes.h"
#include "Pic.h"

enum picFieldEnum
{
  fieldNONE         = 0x00,
  fieldODD          = 0x01,
  fieldEVEN         = 0x02,
  fieldFULL         = 0x03,
};

enum picEncodeEnum
{
  encodeUNCOMPRESSED    = 0x00,
  encodeMIXEDRUNLEN     = 0x02,
};

enum picChannel
{
  channelRED            = 0x80,
  channelGREEN          = 0x40,
  channelBLUE           = 0x20,
  channelALPHA          = 0x10,
  channelRGB            = channelRED | channelGREEN | channelBLUE,
};


namespace PIC_BITMAP
{ 

	#pragma pack(push, 1)
	struct RGBA{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	} ;
	#pragma pack(pop)


	union FLOATSIGN
	{
		int i;
		float f;
	};

//	static void LoadUncompressUnchanned (ZeroFile &fp, DDPIXELFORMAT *pixelFormat,
//														unsigned char *Buffer, int strideInBytes,
//														int widthInPixels, int heightInPixels);
//	
//	static void LoadUncompressChanned (ZeroFile &fp, DDPIXELFORMAT *pixelFormat,
//													 unsigned char *Buffer, int strideInBytes,
//													 int widthInPixels, int heightInPixels);
//	
//	
//	static void LoadCompressChanned (ZeroFile &fp, DDPIXELFORMAT *pixelFormat,
//												  unsigned char *Buffer, int strideInBytes,
//												  int widthInPixels, int heightInPixels);
//	
//	static void LoadCompressUnchanned (ZeroFile &fp, DDPIXELFORMAT *pixelFormat,
//														  unsigned char *Buffer, int strideInBytes,
//														  int widthInPixels, int heightInPixels);
//	
//	
	
	
	#pragma pack(1) 
	struct PICFILEHEADER
	{
	  unsigned    magic;
	  float	     version;
	  char        comment[80];
	  char        id[4];
	  short       width;
	  short       height;
	  float       aspectRatio;
	  short       fields;
	  short       pad;
	
	  unsigned SwapEndian (unsigned Val)
	  {
		  unsigned tmp;
		  tmp = (Val & 0xff) << 24;
		  tmp += (Val & 0xff00) << 8;
		  tmp += (Val & 0xff0000) >> 8;
		  tmp += Val >> 24;
		  return tmp;
	  }
	
	  void SwapByteOrder ()
	  {
#if (PLATFORM != PLATFORM_GAMECUBE)
		  FLOATSIGN t;
		  magic = SwapEndian (magic);
	
		  t.f = version;
		  t.i = SwapEndian (t.i);
		  version = t.f;
	
		  width = (short) (SwapEndian (width) >> 16);
		  height = (short) (SwapEndian (height) >> 16);
	
	  	  t.f = aspectRatio;
		  t.i = SwapEndian (t.i);
		  aspectRatio = t.f;
	
		  fields = (short) (SwapEndian (fields) >> 16);
		  pad = (short) (SwapEndian (pad) >> 16);
#endif
	  }
	};
	#pragma pack() 
	
	#pragma pack(1) 
	
	struct PICCHANNELINFO
	{
		unsigned char chained;
		unsigned char size;
		unsigned char type;
		unsigned char channel;
	
		PICCHANNELINFO ()
		{
			chained = 0;
			size = 0;
			type = 0;
			channel = 0;
		}
	};
	#pragma pack() 
	
	
	// Size checking macro
	#define CHECK_SIZE(s) \
	if (size < s)         \
	{                     \
	  goto SizeError;     \
	}                     \
	else                  \
	{                     \
	  size -= s;          \
	}

	
	static void ReadNormalScan (
		ZeroFile &fp,
		int Length,
		RGBA *scanLine,
		bool anAlphaLine)
	{
		int n;
	
		for (n = 0; n < Length; n++) 
		{
			 if(!anAlphaLine)
			 {
				scanLine[n].r = (unsigned char)fp.Getc();
				scanLine[n].g = (unsigned char)fp.Getc();
				scanLine[n].b = (unsigned char)fp.Getc();
				scanLine[n].a = 255;
			 }
			 else
				scanLine[n].a = (unsigned char)fp.Getc();
		}
	}

	
	
	static void ReadCompressedScan (
		ZeroFile &fp,
		int Length,
		RGBA *scanLine,
		bool anAlphaLine)
	{
		int j;
		int n;
		int count;
		unsigned char c;
		RGBA Color;
	
		count = 0;
		for (n = 0; n < Length; n += count) 
		{
			c = (unsigned char)fp.Getc();
			if (c < 128)	
			{
				 count = c + 1;
				 for (j = 0; j < count; j++) 
				 {
					 if(!anAlphaLine)
					 {
						scanLine[n + j].r = (unsigned char)fp.Getc();
						scanLine[n + j].g = (unsigned char)fp.Getc();
						scanLine[n + j].b = (unsigned char)fp.Getc();
						scanLine[n + j].a = 255;
					 }
					 else
						scanLine[n + j].a = (unsigned char)fp.Getc();
				 }
			} 
			else 
			{
				if (c == 128)	
				{
					count = (unsigned char)fp.Getc() * 256 + (unsigned char)fp.Getc();
				} 
				else 
				{
					count = c - 127;
				}
	
				 if(!anAlphaLine)
				 {
					Color.r = (unsigned char)fp.Getc();
					Color.g = (unsigned char)fp.Getc();
					Color.b = (unsigned char)fp.Getc();
					Color.a = 255;
				 }
				 else
					Color.a = (unsigned char)fp.Getc();
				for (j = 0; j < count; j++) 
				{
				 if(!anAlphaLine)
				 {
					scanLine[n + j] = Color;
				 }
				 else
				 {
					 scanLine[n + j].a = Color.a;
				 }
				}
			}
		}
		if (n != Length)
		{
			ZeroTrace(("Pic.cpp: n != Length"));
		}
	}	
	
	
	static void LoadData(
		ZeroFile &fp,
		const ZeroPixelFormat& pixelFormat,
		unsigned char *BufferBytes, 
		int strideInBytes,
		int widthInPixels, 
		int heightInPixels,
		PICCHANNELINFO* aChanRGB,
		PICCHANNELINFO* aChanAlpha)
	{
		int i;
		int j;
		int Index;
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
		unsigned int red_shift;
		unsigned int green_shift;
		unsigned int blue_shift;
		unsigned int alpha_shift;
		unsigned int red_scale;
		unsigned int green_scale;
		unsigned int blue_scale;
		unsigned int alpha_scale;
		ZeroStack<RGBA> scanLinePool (widthInPixels); 
		RGBA* scanLine;	

		scanLine	= &scanLinePool[0];
	
//		GetPixelMaskAndScale (pixelFormat->dwRBitMask, red_shift, red_scale);
//		GetPixelMaskAndScale (pixelFormat->dwGBitMask, green_shift, green_scale);
//		GetPixelMaskAndScale (pixelFormat->dwBBitMask, blue_shift, blue_scale);
//		GetPixelMaskAndScale (pixelFormat->dwRGBAlphaBitMask, alpha_shift, alpha_scale);
//		strideInBytes = strideInBytes / (pixelFormat->dwRGBBitCount / 8);

		GetPixelMaskAndScale (pixelFormat.red.mask, red_shift, red_scale);
		GetPixelMaskAndScale (pixelFormat.green.mask, green_shift, green_scale);
		GetPixelMaskAndScale (pixelFormat.blue.mask, blue_shift, blue_scale);
		GetPixelMaskAndScale (pixelFormat.alpha.mask, alpha_shift, alpha_scale);
		strideInBytes = strideInBytes / (pixelFormat.bitCount / 8);

		if (pixelFormat.bitCount == 16) {

			unsigned short *Buffer;
			Buffer = (unsigned short *)BufferBytes;
			for (j = 0; j < heightInPixels; j ++) 
			{
				if(aChanRGB->type == encodeMIXEDRUNLEN)
					ReadCompressedScan(fp, heightInPixels, scanLine, false);
				else
					ReadNormalScan(fp, heightInPixels, scanLine, false);

				//Read the alpha channel
				if(aChanAlpha)
				{
					if(aChanAlpha->type == encodeMIXEDRUNLEN)
						ReadCompressedScan(fp, heightInPixels, scanLine, true);
					else
						ReadNormalScan(fp, heightInPixels, scanLine, true);
				}
	
				Index = strideInBytes * (heightInPixels - j - 1);
				for (i = 0; i < widthInPixels; i ++) {
					r = (unsigned char)(scanLine[i].r >> red_scale);
					g = (unsigned char)(scanLine[i].g >> green_scale);
					b = (unsigned char)(scanLine[i].b >> blue_scale);
					a = (unsigned char)(scanLine[i].a >> alpha_scale);
					Buffer[Index++] = (unsigned short)
						((r << red_shift) + (g << green_shift) + (b << blue_shift) + (a << alpha_shift));
				}
			}

		} else if (pixelFormat.bitCount == 32) {

			unsigned *Buffer;
			Buffer = (unsigned*) BufferBytes;
			for (j = 0; j < heightInPixels; j ++) {

				if(aChanRGB->type == encodeMIXEDRUNLEN)
					ReadCompressedScan(fp, widthInPixels, scanLine, false);
				else
					ReadNormalScan(fp, widthInPixels, scanLine, false);

				//Read the alpha channel
				if(aChanAlpha)
				{
					if(aChanAlpha->type == encodeMIXEDRUNLEN)
						ReadCompressedScan(fp, widthInPixels, scanLine, true);
					else
						ReadNormalScan(fp, widthInPixels, scanLine, true);
				}
	
				Index = strideInBytes * (heightInPixels - j - 1);
				for (i = 0; i < widthInPixels; i ++) {
					r = (unsigned char)(scanLine[i].r >> red_scale);
					g = (unsigned char)(scanLine[i].g >> green_scale);
					b = (unsigned char)(scanLine[i].b >> blue_scale);
					a = (unsigned char)(scanLine[i].a >> alpha_scale);
					Buffer[Index++] = (unsigned)
						((r << red_shift) + (g << green_shift) + (b << blue_shift) + (a << alpha_shift));
				}
			}

		} else {
			_ASSERTE (0);
		}

	}
	
/*	
	void LoadUncompressUnchanned (
		ZeroFile &fp,
		const ZeroPixelFormat& pixelFormat,
		unsigned char *Buffer, 
		int strideInBytes,
		int widthInPixels, 
		int heightInPixels)
	{
		_ASSERTE (0);
	}
	
	void LoadUncompressChanned (
		ZeroFile &fp,
		const ZeroPixelFormat& pixelFormat,
		unsigned char *Buffer, 
		int strideInBytes,
		int widthInPixels, 
		int heightInPixels)
	{
		_ASSERTE (0);
	}
	
	
	static void LoadCompressChanned (
		ZeroFile &fp,
		const ZeroPixelFormat& pixelFormat,
		unsigned char *Buffer, 
		int strideInBytes,
		int widthInPixels, 
		int heightInPixels)
	{
//		_ASSERTE (0);
		ZeroTrace ("!!!!!!!!!!!! no compress .pic file support\n");
	}
*/

	bool __cdecl GetDimension (
		const char *File_Name, 
		int &Width, 
		int &Height)
	{
		PICFILEHEADER head;
	
	   ZeroFile fp (File_Name, "rb");
		if (!fp) {
			return false;
		}
	
	   if (fp.Read8(&head, sizeof(PICFILEHEADER), 1) < 1) {
	        return false;
	   }
	
	   head.SwapByteOrder();
	
		Width = head.width;
		Height = head.height;
	
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



	bool __cdecl Load (
		const char *File_Name, 
		unsigned char *Buffer, 
		int strideInBytes,
		int &widthInPixels, 
		int &heightInPixels,
		ZeroPixelFormat& pixelFormat,bool &transparent)
	{
	   PICFILEHEADER head;
		PICCHANNELINFO chanRGB;
		PICCHANNELINFO chanAlpha;
		transparent = false;
	
	   ZeroFile fp (File_Name, "rb");
		if (!fp) {
			return false;
		}
	
	   if (fp.Read8(&head, sizeof(PICFILEHEADER), 1) < 1) {
	        return false;
	   }
	
	   head.SwapByteOrder();
	
	   if (head.magic != 0x5380f634)	{
			return false;
		}
		if (strncmp(head.id, "PICT", 4)) {
			return false;
		}
	
		_ASSERTE (widthInPixels == head.width);
		_ASSERTE (heightInPixels == head.height);
	
		if (fp.Read8(&chanRGB, sizeof(PICCHANNELINFO), 1) < 1) 
		{
	  		return false;
		}
	
		if (chanRGB.size != 8) 
		{
			return false;
		}

		
		if(chanRGB.channel != channelRGB) 
			return false;
	
		if(chanRGB.chained) //Check for alpha channel
		{
			transparent = true;
			if (fp.Read8(&chanAlpha, sizeof(PICCHANNELINFO), 1) < 1) 
			{
		  		return false;
			}
	//	   bitspp += chanAlpha.size;
	//    alphabytes = chanAlpha.size / 8;
		}
	
		LoadData(fp, pixelFormat, Buffer, strideInBytes, widthInPixels, heightInPixels, &chanRGB, chanRGB.chained ? &chanAlpha : NULL);

/*		if(chanRGB.type == encodeMIXEDRUNLEN) 
		{
			if (chanRGB.chained) 
			{
				LoadCompressChanned (fp, pixelFormat, Buffer, strideInBytes, widthInPixels, heightInPixels);
			} 
			else 
			{
				LoadCompressUnchanned (fp, pixelFormat, Buffer, strideInBytes, widthInPixels, heightInPixels);
			}
		} 
		else 
		{
			if (chanRGB.chained) 
			{
				LoadUncompressChanned (fp, pixelFormat, Buffer, strideInBytes, widthInPixels, heightInPixels);
			} 
			else 
			{
				LoadUncompressUnchanned (fp, pixelFormat, Buffer, strideInBytes,	widthInPixels, heightInPixels);
			}
		}*/
	
		return true;
	}
}	
												
