//********************************************************************
// Direct geometry Hardware independent library
//	By Julio Jerez
// VC: 6.0
//********************************************************************
#ifndef __ZeroPixels__
#define __ZeroPixels__

inline void GetPixelMaskAndScale (
	unsigned int mask,
	unsigned int &shift,
	unsigned int &scale)
{
   int m;
   int i;
   for (shift = 0, m = mask; !(m & 1) && (shift < 32); m >>= 1) shift ++;
   for (scale = 0, i = 0x80; !(i & m) && (scale < 8); i >>=1) scale ++;
}

inline unsigned PackRGBA (
		unsigned rMask, 
		unsigned gMask, 
		unsigned bMask, 
		unsigned aMask,
		unsigned char red, 
		unsigned char green, 
		unsigned char blue, 
		unsigned char alpha)
{
	unsigned red_shift, red_scale;
	unsigned green_shift, green_scale;
	unsigned blue_shift, blue_scale;
	unsigned alpha_shift, alpha_scale;
	
	GetPixelMaskAndScale (rMask, red_shift, red_scale);
	GetPixelMaskAndScale (gMask, green_shift, green_scale);
	GetPixelMaskAndScale (bMask, blue_shift, blue_scale);
	GetPixelMaskAndScale (aMask, alpha_shift, alpha_scale);
	
	red >>= red_scale;
	green >>= green_scale;
	blue >>= blue_scale;
	alpha >>= alpha_scale;

	return 
		(red << red_shift) + 
		(green << green_shift) + 
		(blue << blue_shift) + 
		(alpha << alpha_shift);
}

#endif
