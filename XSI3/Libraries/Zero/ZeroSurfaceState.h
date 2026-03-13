/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroSurfaceState__
#define __ZeroSurfaceState__



#if (PLATFORM == PLATFORM_XBOX)

enum ZeroTextureMagFilter
{
	ZERO_MAGFILTER_POINT         = 1,    // nearest
	ZERO_MAGFILTER_LINEAR        = 2,    // linear interpolation
	ZERO_MAGFILTER_ANISOTROPIC   = 3,    // anisotropic
	ZERO_MAGFILTER_FLATCUBIC	 = 5,    // not supported on XBOX
	ZERO_MAGFILTER_GAUSSIANCUBIC = 5     // different cubic kernel
};

enum ZeroTextureMinFilter
{
	ZERO_MINFILTER_POINT         = 1,    // nearest
	ZERO_MINFILTER_LINEAR        = 2,    // linear interpolation
	ZERO_MINFILTER_ANISOTROPIC   = 3     // anisotropic
};

enum ZeroTextureMipFilter
{
    ZERO_MIPFILTER_NONE          = 0,    // mipmapping disabled (use MAG filter)
	ZERO_MIPFILTER_POINT         = 1,    // nearest
	ZERO_MIPFILTER_LINEAR        = 2     // linear interpolation
};

enum ZeroTextureSource
{
	ZERO_TEXTURESOURCE_DIFFUSE         = 0x00000000,  // select diffuse color
	ZERO_TEXTURESOURCE_CURRENT         = 0x00000001,  // select result of previous stage
	ZERO_TEXTURESOURCE_TEXTURE         = 0x00000002,  // select texture color
	ZERO_TEXTURESOURCE_TFACTOR         = 0x00000003,  // select RENDERSTATE_TEXTUREFACTOR
	ZERO_TEXTURESOURCE_SPECULAR        = 0x00000004,  // select specular color
    ZERO_TEXTURESOURCE_TEMP            = 0x00000005,  // select temporary register color (read/write) ###xbox###
	ZERO_TEXTURESOURCE_COMPLEMENT      = 0x00000010,  // take 1.0 - x
	ZERO_TEXTURESOURCE_ALPHAREPLICATE  = 0x00000020   // replicate alpha to color components
};

enum ZeroTextureAddress
{
    ZERO_TEXTUREADDRESS_WRAP           = 1,
	ZERO_TEXTUREADDRESS_MIRROR         = 2,
	ZERO_TEXTUREADDRESS_CLAMP          = 3,
	ZERO_TEXTUREADDRESS_BORDER         = 4,
    ZERO_TEXTUREADDRESS_CLAMPTOEDGE    = 5          //###XBOX###
};

enum ZeroTextureOperation
{
	// Control
	ZERO_TEXTUREOP_DISABLE    = 1,      // disables stage
	ZERO_TEXTUREOP_SELECTARG1 = 2,      // the default
	ZERO_TEXTUREOP_SELECTARG2 = 3,


	// Modulate
	ZERO_TEXTUREOP_MODULATE   = 4,      // multiply args together
	ZERO_TEXTUREOP_MODULATE2X = 5,      // multiply and  1 bit
	ZERO_TEXTUREOP_MODULATE4X = 6,      // multiply and  2 bits
	
	// Add
	ZERO_TEXTUREOP_ADD          =  7,   // add arguments together
	ZERO_TEXTUREOP_ADDSIGNED    =  8,   // add with -0.5 bias
	ZERO_TEXTUREOP_ADDSIGNED2X  =  9,   // as above but left  1 bit
	ZERO_TEXTUREOP_SUBTRACT     = 10,   // Arg1 - Arg2, with no saturation
	ZERO_TEXTUREOP_ADDSMOOTH    = 11,   // Arg1 + Arg2 - Arg1*Arg2 = Arg1 + (1-Arg1)*Arg2

	// Linear alpha blend: Arg1*(Alpha) + Arg2*(1-Alpha)
	ZERO_TEXTUREOP_BLENDDIFFUSEALPHA    = 12, // iterated alpha
	ZERO_TEXTUREOP_BLENDTEXTUREALPHA    = 14, // texture alpha
	ZERO_TEXTUREOP_BLENDFACTORALPHA     = 15, // alpha from D3DRENDERSTATE_TEXTUREFACTOR
	
	// Linear alpha blend with pre-multiplied arg1 input: Arg1 + Arg2*(1-Alpha)
	ZERO_TEXTUREOP_BLENDTEXTUREALPHAPM  = 16, // texture alpha
	ZERO_TEXTUREOP_BLENDCURRENTALPHA    = 13, // by alpha of current color
	
	// Specular mapping
	ZERO_TEXTUREOP_PREMODULATE            = 17,     // modulate with next texture before use
	ZERO_TEXTUREOP_MODULATEALPHA_ADDCOLOR = 18,     // Arg1.RGB + Arg1.A*Arg2.RGB
	// COLOROP only
	ZERO_TEXTUREOP_MODULATECOLOR_ADDALPHA = 19,     // Arg1.RGB*Arg2.RGB + Arg1.A
	// COLOROP only
	ZERO_TEXTUREOP_MODULATEINVALPHA_ADDCOLOR = 20,  // (1-Arg1.A)*Arg2.RGB + Arg1.RGB
	// COLOROP only
	ZERO_TEXTUREOP_MODULATEINVCOLOR_ADDALPHA = 21,  // (1-Arg1.RGB)*Arg2.RGB + Arg1.A
	// COLOROP only
	
	// Bump mapping
	ZERO_TEXTUREOP_BUMPENVMAP           = 25, // per pixel env map perturbation
	ZERO_TEXTUREOP_BUMPENVMAPLUMINANCE  = 26, // with luminance channel
	
	// This can do either diffuse or specular bump mapping with correct input.
	// Performs the function (Arg1.R*Arg2.R + Arg1.G*Arg2.G + Arg1.B*Arg2.B)
	// where each component has been scaled and offset to make it signed.
	// The result is replicated into all four (including alpha) channels.
	// This is a valid COLOROP only.
	ZERO_TEXTUREOP_DOTPRODUCT3          = 22,

    // Triadic ops ###XBOX###
    ZERO_TEXTUREOP_MULTIPLYADD          = 23, // Arg0 + Arg1*Arg2
    ZERO_TEXTUREOP_LERP                 = 24  // (Arg0)*Arg1 + (1-Arg0)*Arg2
};

enum ZeroTextureTransformFlags
{
    //Texture coordinates are passed directly to the rasterizer
	ZERO_TEXTRANSF_DISABLE         =   0,

	//The rasterizer should expect 1-D texture coordinates
    ZERO_TEXTRANSF_COUNT1          =   1,

	//The rasterizer should expect 2-D texture coordinates
    ZERO_TEXTRANSF_COUNT2          =   2,

	//The rasterizer should expect 3-D texture coordinates
    ZERO_TEXTRANSF_COUNT3          =   3,

	//The rasterizer should expect 4-D texture coordinates
    ZERO_TEXTRANSF_COUNT4          =   4,

	//The texture coordinates are all divided by the last element 
	//before being passed to the rasterizer. For example, if this 
	//flag is specified with the ZERO_TEXTRANSF_COUNT3 flag, the first
	//and second texture coordinates is divided by the third coordinate 
	//before being passed to the rasterizer
    ZERO_TEXTRANSF_PROJECTED       = 256,

    //Forces this enumeration to compile to 32 bits in size. Not used
	ZERO_TEXTRANSF_FORCE_DWORD     = 0x7fffffff
};

enum ZeroTci
{
	//Use the specified texture coordinates contained within the vertex format. This value resolves to zero. 
	ZERO_TCI_PASSTHRU                          =   0x00000000,

	//Use the vertex normal, transformed to camera space, as the input texture coordinates for this stage's texture transformation. 
	ZERO_TCI_CAMERASPACENORMAL                 =   0x00010000,

	//Use the vertex position, transformed to camera space, as the input texture coordinates for this stage's texture transformation. 
	ZERO_TCI_CAMERASPACEPOSITION               =   0x00020000,

	//Use the reflection vector, transformed to camera space, as the input texture coordinate for this stage's texture transformation. The reflection vector is computed from the input vertex position and normal vector. 
	ZERO_TCI_CAMERASPACEREFLECTIONVECTOR       =   0x00030000
};


enum ZeroTransformStateType 
{
    ZERO_TS_TEXTURE0      = 0,
    ZERO_TS_TEXTURE1      = 1,
    ZERO_TS_TEXTURE2      = 2,
    ZERO_TS_TEXTURE3      = 3,
    ZERO_TS_TEXTURE4      = 4,
    ZERO_TS_TEXTURE5      = 5,
    ZERO_TS_TEXTURE6      = 6,
    ZERO_TS_TEXTURE7      = 7
};

enum ZeroSurfaceStageTypes
{
	ZERO_TEXTURESTAGE_END					=  0xffffffff,
	ZERO_TEXTURESTAGE_COLOROP				= 12,
	ZERO_TEXTURESTAGE_COLORARG1				= 14,
	ZERO_TEXTURESTAGE_COLORARG2				= 15,
	ZERO_TEXTURESTAGE_ALPHAOP				= 16,
	ZERO_TEXTURESTAGE_ALPHAARG1				= 18,
	ZERO_TEXTURESTAGE_ALPHAARG2				= 19,
	ZERO_TEXTURESTAGE_BUMPENVMAT00			= 22,
	ZERO_TEXTURESTAGE_BUMPENVMAT01			= 23,
	ZERO_TEXTURESTAGE_BUMPENVMAT10			= 24,
	ZERO_TEXTURESTAGE_BUMPENVMAT11			= 25,
	ZERO_TEXTURESTAGE_TEXCOORDINDEX			= 28,
	ZERO_TEXTURESTAGE_ADDRESSU				= 0,
	ZERO_TEXTURESTAGE_ADDRESSV				= 1,
	ZERO_TEXTURESTAGE_ADDRESSW				= 2,
	ZERO_TEXTURESTAGE_BORDERCOLOR			= 29,
	ZERO_TEXTURESTAGE_MAGFILTER				= 3,
	ZERO_TEXTURESTAGE_MINFILTER				= 4,
	ZERO_TEXTURESTAGE_MIPFILTER				= 5,
	ZERO_TEXTURESTAGE_MIPMAPLODBIAS			= 6,
	ZERO_TEXTURESTAGE_MAXMIPLEVEL			= 7,
	ZERO_TEXTURESTAGE_MAXANISOTROPY			= 8,
	ZERO_TEXTURESTAGE_BUMPENVLSCALE			= 26,
	ZERO_TEXTURESTAGE_BUMPENVLOFFSET		= 27,
	ZERO_TEXTURESTAGE_TEXTURETRANSFORMFLAGS	= 21
};


#else


enum ZeroTextureMagFilter
{
	ZERO_MAGFILTER_POINT         = 1,    // nearest
	ZERO_MAGFILTER_LINEAR        = 2,    // linear interpolation
	ZERO_MAGFILTER_ANISOTROPIC   = 3,    // anisotropic
	ZERO_MAGFILTER_FLATCUBIC	 = 4,    // cubic
	ZERO_MAGFILTER_GAUSSIANCUBIC = 5    // different cubic kernel
};

enum ZeroTextureMinFilter
{
	ZERO_MINFILTER_POINT         = 1,    // nearest
	ZERO_MINFILTER_LINEAR        = 2,    // linear interpolation
	ZERO_MINFILTER_ANISOTROPIC   = 3     // anisotropic
};

enum ZeroTextureMipFilter
{
    ZERO_MIPFILTER_NONE          = 0,    // mipmapping disabled (use MAG filter)
	ZERO_MIPFILTER_POINT         = 1,    // nearest
	ZERO_MIPFILTER_LINEAR        = 2     // linear interpolation
};

enum ZeroTextureSource
{
	ZERO_TEXTURESOURCE_DIFFUSE         = 0x00000000,  // select diffuse color
	ZERO_TEXTURESOURCE_CURRENT         = 0x00000001,  // select result of previous stage
	ZERO_TEXTURESOURCE_TEXTURE         = 0x00000002,  // select texture color
	ZERO_TEXTURESOURCE_TFACTOR         = 0x00000003,  // select RENDERSTATE_TEXTUREFACTOR
	ZERO_TEXTURESOURCE_SPECULAR        = 0x00000004,  // select specular color
	ZERO_TEXTURESOURCE_COMPLEMENT      = 0x00000010,  // take 1.0 - x
	ZERO_TEXTURESOURCE_ALPHAREPLICATE  = 0x00000020   // replicate alpha to color components
};

enum ZeroTextureAddress
{
    ZERO_TEXTUREADDRESS_WRAP           = 1,
	ZERO_TEXTUREADDRESS_MIRROR         = 2,
	ZERO_TEXTUREADDRESS_CLAMP          = 3,
	ZERO_TEXTUREADDRESS_BORDER         = 4
};

enum ZeroTextureOperation
{
	// Control
	ZERO_TEXTUREOP_DISABLE    = 1,      // disables stage
	ZERO_TEXTUREOP_SELECTARG1 = 2,      // the default
	ZERO_TEXTUREOP_SELECTARG2 = 3,
	
	// Modulate
	ZERO_TEXTUREOP_MODULATE   = 4,      // multiply args together
	ZERO_TEXTUREOP_MODULATE2X = 5,      // multiply and  1 bit
	ZERO_TEXTUREOP_MODULATE4X = 6,      // multiply and  2 bits
	
	// Add
	ZERO_TEXTUREOP_ADD          =  7,   // add arguments together
	ZERO_TEXTUREOP_ADDSIGNED    =  8,   // add with -0.5 bias
	ZERO_TEXTUREOP_ADDSIGNED2X  =  9,   // as above but left  1 bit
	ZERO_TEXTUREOP_SUBTRACT     = 10,   // Arg1 - Arg2, with no saturation
	ZERO_TEXTUREOP_ADDSMOOTH    = 11,   // Arg1 + Arg2 - Arg1*Arg2 = Arg1 + (1-Arg1)*Arg2
	
	// Linear alpha blend: Arg1*(Alpha) + Arg2*(1-Alpha)
	ZERO_TEXTUREOP_BLENDDIFFUSEALPHA    = 12, // iterated alpha
	ZERO_TEXTUREOP_BLENDTEXTUREALPHA    = 13, // texture alpha
	ZERO_TEXTUREOP_BLENDFACTORALPHA     = 14, // alpha from D3DRENDERSTATE_TEXTUREFACTOR
	
	// Linear alpha blend with pre-multiplied arg1 input: Arg1 + Arg2*(1-Alpha)
	ZERO_TEXTUREOP_BLENDTEXTUREALPHAPM  = 15, // texture alpha
	ZERO_TEXTUREOP_BLENDCURRENTALPHA    = 16, // by alpha of current color
	
	// Specular mapping
	ZERO_TEXTUREOP_PREMODULATE            = 17,     // modulate with next texture before use
	ZERO_TEXTUREOP_MODULATEALPHA_ADDCOLOR = 18,     // Arg1.RGB + Arg1.A*Arg2.RGB
	// COLOROP only
	ZERO_TEXTUREOP_MODULATECOLOR_ADDALPHA = 19,     // Arg1.RGB*Arg2.RGB + Arg1.A
	// COLOROP only
	ZERO_TEXTUREOP_MODULATEINVALPHA_ADDCOLOR = 20,  // (1-Arg1.A)*Arg2.RGB + Arg1.RGB
	// COLOROP only
	ZERO_TEXTUREOP_MODULATEINVCOLOR_ADDALPHA = 21,  // (1-Arg1.RGB)*Arg2.RGB + Arg1.A
	// COLOROP only
	
	// Bump mapping
	ZERO_TEXTUREOP_BUMPENVMAP           = 22, // per pixel env map perturbation
	ZERO_TEXTUREOP_BUMPENVMAPLUMINANCE  = 23, // with luminance channel
	
	// This can do either diffuse or specular bump mapping with correct input.
	// Performs the function (Arg1.R*Arg2.R + Arg1.G*Arg2.G + Arg1.B*Arg2.B)
	// where each component has been scaled and offset to make it signed.
	// The result is replicated into all four (including alpha) channels.
	// This is a valid COLOROP only.
	ZERO_TEXTUREOP_DOTPRODUCT3          = 24
};

enum ZeroTextureTransformFlags
{
    //Texture coordinates are passed directly to the rasterizer
	ZERO_TEXTRANSF_DISABLE         =   0,

	//The rasterizer should expect 1-D texture coordinates
    ZERO_TEXTRANSF_COUNT1          =   1,

	//The rasterizer should expect 2-D texture coordinates
    ZERO_TEXTRANSF_COUNT2          =   2,

	//The rasterizer should expect 3-D texture coordinates
    ZERO_TEXTRANSF_COUNT3          =   3,

	//The rasterizer should expect 4-D texture coordinates
    ZERO_TEXTRANSF_COUNT4          =   4,

	//The texture coordinates are all divided by the last element 
	//before being passed to the rasterizer. For example, if this 
	//flag is specified with the ZERO_TEXTRANSF_COUNT3 flag, the first
	//and second texture coordinates is divided by the third coordinate 
	//before being passed to the rasterizer
    ZERO_TEXTRANSF_PROJECTED       = 256,

    //Forces this enumeration to compile to 32 bits in size. Not used
	ZERO_TEXTRANSF_FORCE_DWORD     = 0x7fffffff
};

enum ZeroTci
{
	//Use the specified texture coordinates contained within the vertex format. This value resolves to zero. 
	ZERO_TCI_PASSTHRU                          =   0x00000000,

	//Use the vertex normal, transformed to camera space, as the input texture coordinates for this stage's texture transformation. 
	ZERO_TCI_CAMERASPACENORMAL                 =   0x00010000,

	//Use the vertex position, transformed to camera space, as the input texture coordinates for this stage's texture transformation. 
	ZERO_TCI_CAMERASPACEPOSITION               =   0x00020000,

	//Use the reflection vector, transformed to camera space, as the input texture coordinate for this stage's texture transformation. The reflection vector is computed from the input vertex position and normal vector. 
	ZERO_TCI_CAMERASPACEREFLECTIONVECTOR       =   0x00030000
};


enum ZeroTransformStateType 
{
    ZERO_TS_TEXTURE0      = 0,
    ZERO_TS_TEXTURE1      = 1,
    ZERO_TS_TEXTURE2      = 2,
    ZERO_TS_TEXTURE3      = 3,
    ZERO_TS_TEXTURE4      = 4,
    ZERO_TS_TEXTURE5      = 5,
    ZERO_TS_TEXTURE6      = 6,
    ZERO_TS_TEXTURE7      = 7
};

enum ZeroSurfaceStageTypes
{
	ZERO_TEXTURESTAGE_END					=  0,
	ZERO_TEXTURESTAGE_COLOROP				=  1,
	ZERO_TEXTURESTAGE_COLORARG1				=  2,
	ZERO_TEXTURESTAGE_COLORARG2				=  3,
	ZERO_TEXTURESTAGE_ALPHAOP				=  4,
	ZERO_TEXTURESTAGE_ALPHAARG1				=  5,
	ZERO_TEXTURESTAGE_ALPHAARG2				=  6,
	ZERO_TEXTURESTAGE_BUMPENVMAT00			=  7,
	ZERO_TEXTURESTAGE_BUMPENVMAT01			=  8,
	ZERO_TEXTURESTAGE_BUMPENVMAT10			=  9,
	ZERO_TEXTURESTAGE_BUMPENVMAT11			= 10,
	ZERO_TEXTURESTAGE_TEXCOORDINDEX			= 11,
	ZERO_TEXTURESTAGE_ADDRESS				= 12,
	ZERO_TEXTURESTAGE_ADDRESSU				= 13,
	ZERO_TEXTURESTAGE_ADDRESSV				= 14,
	ZERO_TEXTURESTAGE_BORDERCOLOR			= 15,
	ZERO_TEXTURESTAGE_MAGFILTER				= 16,
	ZERO_TEXTURESTAGE_MINFILTER				= 17,
	ZERO_TEXTURESTAGE_MIPFILTER				= 18,
	ZERO_TEXTURESTAGE_MIPMAPLODBIAS			= 19,
	ZERO_TEXTURESTAGE_MAXMIPLEVEL			= 20,
	ZERO_TEXTURESTAGE_MAXANISOTROPY			= 21,
	ZERO_TEXTURESTAGE_BUMPENVLSCALE			= 22,
	ZERO_TEXTURESTAGE_BUMPENVLOFFSET		= 23,
	ZERO_TEXTURESTAGE_TEXTURETRANSFORMFLAGS	= 24
};

#endif


class ZeroSurfaceStage
{
public:
	ZeroSurfaceStageTypes op;
	int value;
	
	ZeroSurfaceStage ()
	{
	}
	
	ZeroSurfaceStage (ZeroSurfaceStageTypes opArg, int valueArg)
	{
		op = opArg;
		value = valueArg;
	}
};

#endif
