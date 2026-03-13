/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroRenderState__
#define __ZeroRenderState__



#if (PLATFORM == PLATFORM_XBOX)

// These enums were based on the PC version of DX and the values are invalid
// on XBOX. The PC defines all of these operations to be abstract and converted
// by the driver whereas on Xbox these values are in hardware format and no
// processing or conversion of these values takes place.
// To save the Graphics library having to interpret and process these abstract codes
// adding unnecessary overhead we just redefined these Zero constants to be
// what the XBox hardware expects.

enum ZeroBlendMode
{
    ZERO_BLEND_ZERO                 = 0,
    ZERO_BLEND_ONE                  = 1,
    ZERO_BLEND_SRCCOLOR             = 0x300,
    ZERO_BLEND_INVSRCCOLOR          = 0x301,
    ZERO_BLEND_SRCALPHA             = 0x302,
    ZERO_BLEND_INVSRCALPHA          = 0x303,
    ZERO_BLEND_DESTALPHA            = 0x304,
    ZERO_BLEND_INVDESTALPHA         = 0x305,
    ZERO_BLEND_DESTCOLOR            = 0x306,
    ZERO_BLEND_INVDESTCOLOR         = 0x307,
    ZERO_BLEND_SRCALPHASAT          = 0x308,
    ZERO_BLEND_CONSTANTCOLOR        = 0x8001,
    ZERO_BLEND_INVCONSTANTCOLOR     = 0x8002,
    ZERO_BLEND_CONSTANTALPHA        = 0x8003,
    ZERO_BLEND_INVCONSTANTALPHA     = 0x8004,
};

enum ZeroCull
{
	ZERO_CULLMODE_NONE              = 0,
	ZERO_CULLMODE_CW                = 0x900, 
	ZERO_CULLMODE_CCW               = 0x901
};

enum ZeroFillMode 
{ 
    ZERO_FILL_POINT     = 0x1b00, 
    ZERO_FILL_WIREFRAME = 0x1b01, 
    ZERO_FILL_SOLID     = 0x1b02 
};

enum ZeroShadeMode 
{ 
    ZERO_SHADE_FLAT         = 0x1d00, 
    ZERO_SHADE_GOURAUD      = 0x1d01
}; 

enum ZeroCompareFunction
{
    ZERO_COMPARE_NEVER               = 0x200,
    ZERO_COMPARE_LESS                = 0x201,
    ZERO_COMPARE_EQUAL               = 0x202,
    ZERO_COMPARE_LESSEQUAL           = 0x203,
    ZERO_COMPARE_GREATER             = 0x204,
    ZERO_COMPARE_NOTEQUAL            = 0x205,
    ZERO_COMPARE_GREATEREQUAL        = 0x206,
    ZERO_COMPARE_ALWAYS              = 0x207
};

enum ZeroStencilOp {
    ZERO_STENCILOP_KEEP           = 0x1e00,
    ZERO_STENCILOP_ZERO           = 0,
    ZERO_STENCILOP_REPLACE        = 0x1e01,
    ZERO_STENCILOP_INCRSAT        = 0x1e02,
    ZERO_STENCILOP_DECRSAT        = 0x1e03,
    ZERO_STENCILOP_INVERT         = 0x150a,
    ZERO_STENCILOP_INCR           = 0x8507,
    ZERO_STENCILOP_DECR           = 0x8508,
}; 


enum ZeroFogMode
{
    ZERO_FOG_NONE                = 0,
    ZERO_FOG_EXP                 = 1,
    ZERO_FOG_EXP2                = 2,
    ZERO_FOG_LINEAR              = 3,
};

enum ZeroDiffuseMaterialColorSource
 {
    ZERO_MATERIAL    = 0, 
    ZERO_DIFFUSE     = 1,       // color 1 oD0
    ZERO_SPECULAR    = 2        // color 2 oD1
}; 


enum ZeroVertexBlend 
{
    ZERO_VBLEND_DISABLE  = 0, 
    ZERO_VBLEND_1WEIGHT  = 1, 
    ZERO_VBLEND_2WEIGHTS = 3, 
    ZERO_VBLEND_3WEIGHTS = 5,

}; 


#else

enum ZeroBlendMode
{
   ZERO_BLEND_ZERO = 1, 
	ZERO_BLEND_ONE = 2, 
	ZERO_BLEND_SRCCOLOR = 3, 
	ZERO_BLEND_INVSRCCOLOR = 4, 
	ZERO_BLEND_SRCALPHA = 5, 
	ZERO_BLEND_INVSRCALPHA = 6, 
	ZERO_BLEND_DESTALPHA = 7, 
	ZERO_BLEND_INVDESTALPHA = 8, 
	ZERO_BLEND_DESTCOLOR = 9, 
	ZERO_BLEND_INVDESTCOLOR = 10, 
	ZERO_BLEND_SRCALPHASAT = 11, 
	ZERO_BLEND_BOTHSRCALPHA = 12, 
	ZERO_BLEND_BOTHINVSRCALPHA = 13
};

enum ZeroCull
{
	ZERO_CULLMODE_NONE  = 1,
	ZERO_CULLMODE_CW = 2, 
	ZERO_CULLMODE_CCW = 3
};

enum ZeroFillMode 
{ 
    ZERO_FILL_POINT     = 1, 
    ZERO_FILL_WIREFRAME = 2, 
    ZERO_FILL_SOLID     = 3 
};

enum ZeroShadeMode 
{ 
    ZERO_SHADE_FLAT         = 1, 
    ZERO_SHADE_GOURAUD      = 2, 
    ZERO_SHADE_PHONG        = 3
}; 

enum ZeroCompareFunction
{
    ZERO_COMPARE_NEVER               = 1,
    ZERO_COMPARE_LESS                = 2,
    ZERO_COMPARE_EQUAL               = 3,
    ZERO_COMPARE_LESSEQUAL           = 4,
    ZERO_COMPARE_GREATER             = 5,
    ZERO_COMPARE_NOTEQUAL            = 6,
    ZERO_COMPARE_GREATEREQUAL        = 7,
    ZERO_COMPARE_ALWAYS              = 8
};

enum ZeroStencilOp {
    ZERO_STENCILOP_KEEP           = 1,
    ZERO_STENCILOP_ZERO           = 2,
    ZERO_STENCILOP_REPLACE        = 3,
    ZERO_STENCILOP_INCRSAT        = 4,
    ZERO_STENCILOP_DECRSAT        = 5,
    ZERO_STENCILOP_INVERT         = 6,
    ZERO_STENCILOP_INCR           = 7,
    ZERO_STENCILOP_DECR           = 8
}; 


enum ZeroFogMode
{
    ZERO_FOG_NONE                = 0,
    ZERO_FOG_EXP                 = 1,
    ZERO_FOG_EXP2                = 2,
    ZERO_FOG_LINEAR              = 3,
	ZERO_FOG_REVEXP              = 4,
	ZERO_FOG_REVEXP2             = 5
};

enum ZeroDiffuseMaterialColorSource
 {
    ZERO_MATERIAL    = 0, 
    ZERO_DIFFUSE     = 1, 
    ZERO_SPECULAR    = 2
}; 


enum ZeroVertexBlend 
{
    ZERO_VBLEND_DISABLE  = 0, 
    ZERO_VBLEND_1WEIGHT  = 1, 
    ZERO_VBLEND_2WEIGHTS = 2, 
    ZERO_VBLEND_3WEIGHTS = 3,
	ZERO_VBLEND_TWEENING = 255,
    ZERO_VBLEND_0WEIGHTS = 256

}; 

#endif




enum ZeroRenderStatesTypes
{	
	ZERO_RENDERSTATE_END								= 0,
   ZERO_RENDERSTATE_CULLMODE           		,    // ZERO_CULL
   ZERO_RENDERSTATE_ANTIALIAS          		,    // ZERO_ANTIALIASMODE
   //ZERO_RENDERSTATE_TEXTUREPERSPECTIVE 		,    // TRUE for perspective correction
   ZERO_RENDERSTATE_ZENABLE            		,    // ZERO_ZBUFFERTYPE (or TRUE/FALSE for legacy)
   ZERO_RENDERSTATE_FILLMODE           		,    // ZERO_FILL_MODE       
   ZERO_RENDERSTATE_SHADEMODE          		,    // ZERO_SHADEMODE
   ZERO_RENDERSTATE_LINEPATTERN        		,   // ZERO_LINEPATTERN
   ZERO_RENDERSTATE_ZWRITEENABLE       		,   // TRUE to enable z writes
   ZERO_RENDERSTATE_ALPHATESTENABLE    		,   // TRUE to enable alpha tests
   ZERO_RENDERSTATE_LASTPIXEL          		,   // TRUE for last-pixel on lines
   ZERO_RENDERSTATE_SRCBLEND           		,   // ZERO_BLEND
   ZERO_RENDERSTATE_DESTBLEND          		,   // ZERO_BLEND
   ZERO_RENDERSTATE_ZFUNC              		,   // ZERO_CMPFUNC
   ZERO_RENDERSTATE_ALPHAREF           		,   // ZERO_FIXED
   ZERO_RENDERSTATE_ALPHAFUNC          		,   // ZERO_CMPFUNC
   ZERO_RENDERSTATE_DITHERENABLE       		,   // TRUE to enable dithering
   ZERO_RENDERSTATE_ALPHABLENDENABLE   		,   // TRUE to enable alpha blending
   ZERO_RENDERSTATE_FOGENABLE          		,   // TRUE to enable fog blending
   ZERO_RENDERSTATE_SPECULARENABLE     		,   // TRUE to enable specular
   ZERO_RENDERSTATE_ZVISIBLE           		,   // TRUE to enable z checking
 //  ZERO_RENDERSTATE_STIPPLEDALPHA      		,   // TRUE to enable stippled alpha (RGB device only)
   ZERO_RENDERSTATE_FOGCOLOR           		,   // ZERO_COLOR
   ZERO_RENDERSTATE_FOGTABLEMODE       		,   // ZERO_FOGMODE
   ZERO_RENDERSTATE_FOGSTART           		,   // Fog start (for both vertex and pixel fog)
   ZERO_RENDERSTATE_FOGEND             		,   // Fog end     
   ZERO_RENDERSTATE_FOGDENSITY         		,   // Fog density 
//	ZERO_RENDERSTATE_FOGTABLESTART 				,
//	ZERO_RENDERSTATE_FOGTABLEEND 	   			,
//	ZERO_RENDERSTATE_FOGTABLEDENSITY 			,

   ZERO_RENDERSTATE_EDGEANTIALIAS      		,   // TRUE to enable edge antialiasing
//   ZERO_RENDERSTATE_COLORKEYENABLE     		,   // TRUE to enable source colorkeyed textures
   ZERO_RENDERSTATE_ZBIAS              		,   // LONG Z bias
   ZERO_RENDERSTATE_RANGEFOGENABLE     		,   // Enables range-based fog
															
   ZERO_RENDERSTATE_STENCILENABLE      		,   // BOOL enable/disable stenciling
   ZERO_RENDERSTATE_STENCILFAIL        		,   // ZERO_STENCILOP to do if stencil test fails
   ZERO_RENDERSTATE_STENCILZFAIL       		,   // ZERO_STENCILOP to do if stencil test passes and Z test fails
   ZERO_RENDERSTATE_STENCILPASS        		,   // ZERO_STENCILOP to do if both stencil and Z tests pass
   ZERO_RENDERSTATE_STENCILFUNC        		,   // ZERO_CMPFUNC fn.  Stencil Test passes if ((ref & mask) stencilfn (stencil & mask)) is true
   ZERO_RENDERSTATE_STENCILREF         		,   // Reference value used in stencil test
   ZERO_RENDERSTATE_STENCILMASK        		,   // Mask value used in stencil test
   ZERO_RENDERSTATE_STENCILWRITEMASK   		,   // Write mask applied to values written to stencil buffer
															
   ZERO_RENDERSTATE_TEXTUREFACTOR				,   // ZERO_COLOR used for multi-texture blend
													
   ZERO_RENDERSTATE_CLIPPING						,
   ZERO_RENDERSTATE_LIGHTING						,
  // ZERO_RENDERSTATE_EXTENTS						,
   ZERO_RENDERSTATE_AMBIENT						,
   ZERO_RENDERSTATE_FOGVERTEXMODE				,
   ZERO_RENDERSTATE_COLORVERTEX					,
   ZERO_RENDERSTATE_LOCALVIEWER					,
   ZERO_RENDERSTATE_NORMALIZENORMALS			,
 //  ZERO_RENDERSTATE_COLORKEYBLENDENABLE		,
   ZERO_RENDERSTATE_DIFFUSEMATERIALSOURCE    ,
   ZERO_RENDERSTATE_SPECULARMATERIALSOURCE   ,
   ZERO_RENDERSTATE_AMBIENTMATERIALSOURCE    ,
   ZERO_RENDERSTATE_EMISSIVEMATERIALSOURCE   ,
   ZERO_RENDERSTATE_VERTEXBLEND              ,
   ZERO_RENDERSTATE_CLIPPLANEENABLE          ,

   ZERO_RENDERSTATE_POINTSPRITEENABLE	,
   ZERO_RENDERSTATE_POINTSCALEENABLE	,	
   ZERO_RENDERSTATE_POINTSIZE	,
   ZERO_RENDERSTATE_POINTSIZE_MIN,
   ZERO_RENDERSTATE_POINTSCALE_A	,
   ZERO_RENDERSTATE_POINTSCALE_B	,
   ZERO_RENDERSTATE_POINTSCALE_C	,

   ZERO_RENDERSTATE_SOFTWAREVERTEXPROCESSING, 
   
   ZERO_RENDERSTATE_INDEXEDVERTEXBLENDENABLE, 


	// Do not change this line
	ZERO_RENDERSTATE_COUNT
};

class ZeroRenderState
{
	public:
	ZeroRenderStatesTypes state; 
	int mode; 

	ZeroRenderState ()
	{
	}

	ZeroRenderState (ZeroRenderStatesTypes stateArg, int modeArg)
	{
		state = stateArg;
		mode = modeArg; 
	}

};


#endif
