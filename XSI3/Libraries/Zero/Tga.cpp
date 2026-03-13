#include "ZeroTypes.h"
#include "ZeroFile.h"
#include "ZeroStack.h"
#include "ZeroDebug.h"
#include "ZeroPixels.h"
#include "ZeroGeoTypes.h"
#include "Tga.h"

namespace TGA_BITMAP 
{
	//
	//  TGA File Header
	//
	//set to byte packing...
#if (PLATFORM != PLATFORM_PS2)

#pragma pack(1) 

#endif
	struct TGAHEADER
	{                           
		unsigned char   IdLength;            // Image ID Field Length      
		unsigned char   CmapType;            // color Map Type             
		unsigned char   ImageType;           // Image Type                 
		unsigned short   CmapIndex;           // First Entry Index          
		unsigned short   CmapLength;          // color Map Length           
		unsigned char   CmapEntrySize;       // color Map Entry Size       
		unsigned short   X_Origin;            // X-origin of Image          
		unsigned short   Y_Origin;            // Y-origin of Image          
		unsigned short   ImageWidth;          // Image Width                
		unsigned short   ImageHeight;         // Image Height               
		unsigned char   PixelDepth;          // Pixel Depth                
		unsigned char   ImagDesc;            // Image Descriptor           
	public:
		short SwapEndian(short Val)
		{
			short tmp;
			tmp = (short)((Val & 0xff) << 8);
			tmp = (short)(tmp | (short)((Val & 0xff00) >> 8));
			return tmp;
		}

		int Read(ZeroFile &file)
		{
			int rv;
			rv = file.Read8(this, sizeof(TGAHEADER), 1);
			if (rv < 1)
				return rv;
#if (PLATFORM == PLATFORM_GAMECUBE)
			// sort out big endian mess
			CmapIndex = SwapEndian(CmapIndex);
			CmapLength = SwapEndian(CmapLength);
			X_Origin = SwapEndian(X_Origin);
			Y_Origin = SwapEndian(Y_Origin);
			ImageWidth = SwapEndian(ImageWidth);
			ImageHeight = SwapEndian(ImageHeight);
#endif
			return rv;
		}
	} ;
	//back to what it was before...
#if (PLATFORM != PLATFORM_PS2)
#pragma pack() 
#endif	
	
	static unsigned char GetByte(unsigned char **p)
	{
		unsigned char rval = **p;
		(*p)++;
		return(rval);
	}
	
	
	static short TgaDecodeScanLine(
		unsigned char *DecodedBuffer, 
		unsigned short LineLength,
		unsigned short PixelSize, 
		unsigned char **DpTga)
	{
		int   i;              // Loop counter                                 
		int   byteCount;      // Number of bytes written to the buffer        
		int   runCount;       // The pixel run count                          
		int   bufIndex;       // The index of DecodedBuffer                   
		int   bufMark;        // Index marker of DecodedBuffer                
		int   pixelCount;     // The number of pixels read from the scan line 
		
		bufIndex   = 0;         // Initialize buffer index  
		byteCount  = 0;         // Initialize byte count    
		pixelCount = 0;         // Initialize pixel counter 
		
		// Main decoding loop 
		while (pixelCount < LineLength) {
			// Get the pixel count 
			runCount = GetByte(DpTga);
			
			// Make sure writing this next run will not overflow the buffer 
			if (pixelCount + (runCount & 0x7f) + 1 > LineLength) {
				return -1;     // Pixel run will overflow buffer 
			}
			
			
			// If the run is encoded... 
			if (runCount & 0x80) {
				
				runCount &= ~0x80;              // Mask off the upper bit       
				
				bufMark = bufIndex;             // Save the start-of-run index  
				
				// Update total pixel count 
				pixelCount += (runCount + 1);
				
				// Update the buffer byte count  
				byteCount += ((runCount + 1) * PixelSize);
				
				// Write the first pixel of the run to the buffer 
				for (i = 0; i < PixelSize; i++)	{
					DecodedBuffer[bufIndex++] = GetByte(DpTga);
				}
				
				// Write remainder of pixel run to buffer 'runCount' times 
				while (runCount--) {
					for (i = 0; i < PixelSize; i++)	{
						DecodedBuffer[bufIndex++] = DecodedBuffer[bufMark + i];
					}
				}
			} else { 
				// ...the run is unencoded (raw)   
				// Update total pixel count 
				pixelCount += (runCount + 1);
				
				// Update the buffer byte count 
				byteCount  += ((runCount + 1) * PixelSize);
				
				// Write runCount pixels 
				do {
					for (i = 0; i < PixelSize; i++)	{
						DecodedBuffer[bufIndex++] = GetByte(DpTga);
					}
				} while (runCount--);
			}
		}
		
		return (short)(byteCount);
	}
	
	
	
	bool __cdecl GetDimension (
		const char *fname, 
		int &Width, 
		int &Height)
	{
		TGAHEADER head;
		
		ZeroFile fp (fname, "rb");
		
		if (!fp) {
//			_ASSERTE (fp);
			ZeroTrace (("targa file \"%s\" missing\n", fname));
			return false;
		}
		
		// I dont know why this got taken out, but I NEED it.  
		// ZeroFile::Read is not supported on big endian platforms.
#if (PLATFORM == PLATFORM_GAMECUBE)
		if (head.Read(fp) < 1) {
			printf("Error reading file \"%s\"\n", fname);
			return false;
		}
#else
		if (fp.Read(&head, sizeof(TGAHEADER), 1) < 1) {
			//        printf("Error reading file \"%s\"\n", fname);
			return false;
		}
#endif	
		Width = head.ImageWidth;
		Height = head.ImageHeight;
		
		return true;
	}
	
	bool __cdecl Load (
		const char *fileName, 
		unsigned char *buffer, 
		int strideInBytes,
		int &widthInPixels, 
		int &heightInPixels,
		ZeroPixelFormat& pixelFormat,bool &transparent)
	{
		int bpp;
		int fsize;
		int datasize;
		int bytesPerPixels; 
		unsigned int red_shift, red_scale;
		unsigned int green_shift, green_scale;
		unsigned int blue_shift, blue_scale;
		unsigned int alpha_shift, alpha_scale;
		unsigned char r,g,b,a;
		bool hasPalette;

		TGAHEADER head;
		transparent = false;
		
		ZeroFile fp (fileName, "rb");
		if (!fp) {
			_ASSERTE (fp);
			return false;
		}
		
		
		if (head.Read(fp) < 1) {
			return false;
		}
		
		// support for 8 bit palettized is not very good
		hasPalette = false;

		if (head.PixelDepth == 24) {
			bpp = 3;
		} else if (head.PixelDepth == 32) {
			bpp = 4;
			transparent = true;
		} else if (head.PixelDepth == 8) {
			bpp = 4;
			transparent = true;
			hasPalette = true;
		} else {
			return false;
		}
		
		widthInPixels = head.ImageWidth;
		heightInPixels = head.ImageHeight;
		
		fsize = head.ImageWidth * head.ImageHeight * bpp;
		
		if (head.IdLength > 0) {
			fp.SeekCur(head.IdLength);
		}
		
		datasize = fp.Size() - fp.Tell();
		
		ZeroStack<unsigned char> tmpbuf (fsize); 
		ZeroStack<unsigned char> tmpbuf1 (fsize); 
		
		if (head.ImageType == 1) {
			if (!hasPalette)
				return false;
			unsigned char palette[4*256];
			if (fp.Read8(palette, head.CmapLength * 4, 1) < 1)
				return false;
			int count = (widthInPixels * heightInPixels);
			if (fp.Read8(&tmpbuf[0], count, 1) < 1)
				return false;
			unsigned char *src = &tmpbuf[0];
			unsigned char *dst = &tmpbuf1[0];
			while (--count)
			{
				unsigned int index = *src++;
				unsigned char *entry = palette + index * 4;
				*dst++ = entry[0];	// blue
				*dst++ = entry[1];	// green
				*dst++ = entry[2];	// red
				*dst++ = entry[3];	// alpha
			}
		} else if (head.ImageType == 2) {
			if (fp.Read8(&tmpbuf1[0], fsize, 1) < 1) {
				return false;
			}
			
		} else if (head.ImageType == 10) {
			// handle compressed images 
			int i, byteCount;
			unsigned char *dp, *tp;
			
			if (fp.Read8(&tmpbuf[0], datasize, 1) < 1) {
				return NULL;
			}
			
			dp = buffer;
			tp = &tmpbuf[0];
			for (i=0; i < head.ImageHeight; i++) {
				byteCount = TgaDecodeScanLine(dp, head.ImageWidth, (unsigned short)bpp, &tp);
				if (byteCount < 0) {
					return NULL;
				}
				dp += byteCount;
			}
		}

		GetPixelMaskAndScale (pixelFormat.red.mask, red_shift, red_scale);
		GetPixelMaskAndScale (pixelFormat.green.mask, green_shift, green_scale);
		GetPixelMaskAndScale (pixelFormat.blue.mask, blue_shift, blue_scale);
		GetPixelMaskAndScale (pixelFormat.alpha.mask, alpha_shift, alpha_scale);
		
		bytesPerPixels = pixelFormat.bitCount / 8;

		if (bytesPerPixels == 2) {
			if (bpp==4) {
				unsigned char *fp = (unsigned char *)&tmpbuf1[0];
				
				int count = (widthInPixels * heightInPixels);
				int y = heightInPixels-1;
				int sx = 0;
				unsigned short *tp = (unsigned short int *)(buffer + y * strideInBytes);
				
				while(count--) {
					b = (unsigned char)((*fp++) >> blue_scale);
					g = (unsigned char)((*fp++) >> green_scale);
					r = (unsigned char)((*fp++) >> red_scale);
					a = (unsigned char)((*fp++) >> alpha_scale);
					*tp = (unsigned short) ((r << red_shift) + (g << green_shift) + (b << blue_shift)+ (a << alpha_shift));                     
					
					tp++;
					sx++;
					if (sx >= widthInPixels) {
						sx=0;
						y--;
						tp = (unsigned short*)(buffer + y * strideInBytes);
					}
				}
			} else if (bpp==3) {
				unsigned char *fp = (unsigned char *)&tmpbuf1[0];
				int count = (widthInPixels * heightInPixels);
				int y = heightInPixels-1;
				int sx = 0;
				unsigned short *tp = (unsigned short int *)(buffer + y * strideInBytes);
				
				while(count--) {
					b = (unsigned char)((*fp++) >> blue_scale);
					g = (unsigned char)((*fp++) >> green_scale);
					r = (unsigned char)((*fp++) >> red_scale);
					a = (unsigned char)(255 >> alpha_scale);
					*tp = (unsigned short) ((r << red_shift) + (g << green_shift) + (b << blue_shift) + (a << alpha_shift));
					
					tp++;
					sx++;
					if (sx >= widthInPixels) {
						sx=0;
						y--;
						tp = (unsigned short*)(buffer + y * strideInBytes);
					}
				}
			}
		} else {
			
			if (bpp==4) {
				unsigned char *fp = (unsigned char *)&tmpbuf1[0];
				
				int count = (widthInPixels * heightInPixels);
				int y = heightInPixels-1;
				int sx = 0;
				unsigned *tp = (unsigned*)(buffer + y * strideInBytes);
				
				while(count--) {
					b = (unsigned char)((*fp++) >> blue_scale);
					g = (unsigned char)((*fp++) >> green_scale);
					r = (unsigned char)((*fp++) >> red_scale);
					a = (unsigned char)((*fp++) >> alpha_scale);
					*tp = (unsigned) ((r << red_shift) + (g << green_shift) + (b << blue_shift)+ (a << alpha_shift));                     
					
					tp++;
					sx++;
					if (sx >= widthInPixels) {
						sx=0;
						y--;
						tp = (unsigned*)(buffer + y * strideInBytes);
					}
				}
			} else if (bpp==3) {
				unsigned char *fp = (unsigned char *)&tmpbuf1[0];
				int count = widthInPixels * heightInPixels;
				
				int y = heightInPixels-1;
				int sx = 0;
				unsigned *tp = (unsigned *)(buffer + y * strideInBytes);
				
				while(count--) {
					b = (unsigned char)((*fp++) >> blue_scale);
					g = (unsigned char)((*fp++) >> green_scale);
					r = (unsigned char)((*fp++) >> red_scale);
					a = (unsigned char)(255 >> alpha_scale);
					*tp = (r << red_shift) + (g << green_shift) + (b << blue_shift) + (a << alpha_shift);
					
					tp++;
					sx++;
					if (sx >= widthInPixels) {
						sx=0;
						y--;
						tp = (unsigned *)(buffer + y * strideInBytes);
					}
				}
			}
		}
		
		return true;
	}
	
	
	bool __cdecl LoadRawData (
		const char *fileName, 
		unsigned char *buffer, 
		int strideInBytes,
		int widthInPixels, 
		int heightInPixels,
		const ZeroPixelFormat& pixelFormat,bool &transparent,int szCustomData,unsigned char *customData)
	{
//		int bpp;
		int fsize;
		int datasize;
		int bytesPerPixels; 

		TGAHEADER head;
		transparent = false;
		
		ZeroFile fp (fileName, "rb");
		if (!fp) {
			_ASSERTE (fp);
			return false;
		}
		
		
		if (head.Read(fp) < 1) {
			return false;
		}
		
		if (head.PixelDepth!=pixelFormat.bitCount)
			return false;

		widthInPixels = head.ImageWidth;
		heightInPixels = head.ImageHeight;
		
		fsize = head.ImageWidth * head.ImageHeight * head.PixelDepth/8;
		
		if (head.IdLength > 0) {
			if(szCustomData == head.IdLength)
				fp.Read8(customData,szCustomData,1);
			else
				fp.SeekCur(head.IdLength);
		}
		
		datasize = fp.Size() - fp.Tell();
		
		ZeroStack<unsigned char> tmpbuf (fsize); 
		ZeroStack<unsigned char> tmpbuf1 (fsize); 
		
		if (head.ImageType == 2) {
			if (fp.Read8(&tmpbuf1[0], fsize, 1) < 1) {
				return false;
			}
			
		} else if (head.ImageType == 10) {
			// handle compressed images 
			int i, byteCount;
			unsigned char *dp, *tp;
			
			if (fp.Read8(&tmpbuf[0], datasize, 1) < 1) {
				return NULL;
			}
			
			dp = buffer;
			tp = &tmpbuf[0];
			for (i=0; i < head.ImageHeight; i++) {
				byteCount = TgaDecodeScanLine(dp, (unsigned short)head.ImageWidth, (unsigned short)(head.PixelDepth/8), &tp);
				if (byteCount < 0) {
					return NULL;
				}
				dp += byteCount;
			}
		}

		
		bytesPerPixels = pixelFormat.bitCount / 8;

		unsigned char*inData = (unsigned char*)&tmpbuf1[0];
		int count = (widthInPixels * heightInPixels);
		int y;
		if(head.ImagDesc & 0x20)
			y=0;
		else
			y = heightInPixels-1;
		int sx = 0;
		unsigned char *outData = (unsigned char*)(buffer + y * strideInBytes);
		if(head.ImagDesc & 0x10)
			outData += strideInBytes-1;
		
		while(count--) 
		{
			memcpy(outData,inData,bytesPerPixels);
			
			if(head.ImagDesc & 0x10)
				outData-=bytesPerPixels;
			else
				outData+=bytesPerPixels;
			inData+=bytesPerPixels;
			sx++;
			if (sx >= widthInPixels) 
			{
				sx=0;
				if(head.ImagDesc & 0x20)
					y++;
				else
					y--;
				outData = (unsigned char*)(buffer + y * strideInBytes);
				if(head.ImagDesc & 0x10)
					outData += strideInBytes-1;
			}
		}
		
		return true;
	}

	
	int __cdecl Write (
		const char *fileName,
		unsigned char *buffer, 
		int strideInBytes,         
		int widthInPixels, 
		int heightInPixels,
		const ZeroPixelFormat& pixelFormat)              
	{
		int i;
		int j;
		TGAHEADER head;
		unsigned color;
		unsigned char r,g,b,a;
		unsigned int red_shift, red_scale;
		unsigned int green_shift, green_scale;
		unsigned int blue_shift, blue_scale;
		unsigned int alpha_shift, alpha_scale;
	
		ZeroFile fp (fileName, "wb");
		
		if (!fp) {
			_ASSERTE (fp);
			return 0;
		}
		
		head.IdLength = 0;
		head.CmapType = 0;
		head.ImageType = 2;
		head.CmapIndex = 0;
		head.CmapLength = 0;
		head.CmapEntrySize = 0;
		head.X_Origin = 0;
		head.Y_Origin = 0;
		head.ImageWidth = (unsigned short)widthInPixels;
		head.ImageHeight = (unsigned short)heightInPixels;
		head.PixelDepth = 32;
		head.ImagDesc = 0;
		
		if (fp.Write(&head, sizeof(TGAHEADER), 1) < 1) {
			return 0;
		}

		ZeroStack<unsigned> scanLine (widthInPixels);

		GetPixelMaskAndScale (pixelFormat.red.mask, red_shift, red_scale);
		GetPixelMaskAndScale (pixelFormat.green.mask, green_shift, green_scale);
		GetPixelMaskAndScale (pixelFormat.blue.mask, blue_shift, blue_scale);
		GetPixelMaskAndScale (pixelFormat.alpha.mask, alpha_shift, alpha_scale);

		buffer += strideInBytes * heightInPixels;
		for (i = 0; i < heightInPixels; i ++) {
			buffer -= strideInBytes;
			if (pixelFormat.bitCount == 16) {
				unsigned short* ptr;

				ptr = (unsigned short*) buffer;
				for (j = 0; j < widthInPixels; j ++) {
					color = ptr[j];
					a = (unsigned char)((color & pixelFormat.alpha.mask) >> alpha_shift);
					r = (unsigned char)((color & pixelFormat.red.mask) >> red_shift);
					g = (unsigned char)((color & pixelFormat.green.mask) >> green_shift);
					b = (unsigned char)((color & pixelFormat.blue.mask) >> blue_shift);

					a <<= alpha_scale;
					r <<= red_scale;
					g <<= green_scale;
					b <<= blue_scale;
	
					color = (a << 24) + (r << 16) + (g << 8) + b;   
					scanLine[j]	= color;
				}
			} else if (pixelFormat.bitCount == 32) {
				unsigned* ptr;
				ptr = (unsigned*) buffer;
				for (j = 0; j < widthInPixels; j ++) {
					color = ptr[j];
					a = (unsigned char)((color & pixelFormat.alpha.mask) >> alpha_shift);
					r = (unsigned char)((color & pixelFormat.red.mask) >> red_shift);
					g = (unsigned char)((color & pixelFormat.green.mask) >> green_shift);
					b = (unsigned char)((color & pixelFormat.blue.mask) >> blue_shift);
					color = (a << 24) + (r << 16) + (g << 8) + b;   
					scanLine[j]	= color;
				}
			}
			fp.Write(&scanLine[0], 4, widthInPixels);
		}
		
		return 1;

	}
	
	int __cdecl WriteRawData (
		const char *fileName,
		unsigned char *buffer, 
		int strideInBytes,         
		int widthInPixels, 
		int heightInPixels,
		const ZeroPixelFormat& pixelFormat,
		int sizeOfCustomData, 
		unsigned char *customData)              
	{
		int i;
//		int j;
		TGAHEADER head;
	
		ZeroFile fp (fileName, "wb");
		
		if (!fp) {
			_ASSERTE (fp);
			return 0;
		}
		
		head.IdLength = (unsigned char)sizeOfCustomData;
		head.CmapType = 0;
		head.ImageType = 2;	
		head.CmapIndex = 0;
		head.CmapLength = 0;
		head.CmapEntrySize = 0;
		head.X_Origin = 0;
		head.Y_Origin = 0;
		head.ImageWidth = (unsigned short)widthInPixels;
		head.ImageHeight = (unsigned short)heightInPixels;
		head.PixelDepth = (unsigned char)pixelFormat.bitCount;
		head.ImagDesc = 0;
		
		if (fp.Write(&head, sizeof(TGAHEADER), 1) < 1) {
			return 0;
		}
		if(sizeOfCustomData)
			fp.Write(customData,sizeOfCustomData,1);

		buffer += strideInBytes * heightInPixels;
		for (i = 0; i < heightInPixels; i ++) 
		{
			buffer -= strideInBytes;
			fp.Write(buffer, 1, strideInBytes);
		}
		
		return 1;

	}
	
}

