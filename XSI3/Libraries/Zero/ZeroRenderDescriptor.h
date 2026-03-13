/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroRenderDescriptor__
#define __ZeroRenderDescriptor__

#include "ZeroUnknown.h"
#include "ZeroTypes.h"
#include "ZeroMatrix.h"
#include "ZeroGeoUtil.h"

class ZeroObject;

// primitive type
enum ZeroPrimitiveType
{
	ZERO_PRIMITIVE_NULL,
	ZERO_PRIMITIVE_POINTLIST,
	ZERO_PRIMITIVE_LINELIST,
	ZERO_PRIMITIVE_LINESTRIP,
	ZERO_PRIMITIVE_TRIANGLELIST,
	ZERO_PRIMITIVE_TRIANGLESTRIP,
	ZERO_PRIMITIVE_TRIANGLEFAN,
	ZERO_PRIMITIVE_GAMECUBEPOINTLIST,
	ZERO_PRIMITIVE_GAMECUBETRILIST,
	ZERO_PRIMITIVE_GAMECUBETRISTRIP,
	ZERO_PRIMITIVE_GAMECUBELINELIST
};

// vertex format flags
const unsigned int ZERO_FORMAT_RHW			= 0x0001;	// vertex contains rhw (pre-projected)
const unsigned int ZERO_FORMAT_WEIGHTS		= 0x0002;	// vertex contains weights (incompatible with RHW!)
const unsigned int ZERO_FORMAT_NORMAL		= 0x0004;	// vertex contains a normal (incompatible with RHW!
const unsigned int ZERO_FORMAT_PSIZE		= 0x0008;	// vertex contains point size
const unsigned int ZERO_FORMAT_DIFFUSE		= 0x0010;	// vertex contains diffuse color
const unsigned int ZERO_FORMAT_SPECULAR	= 0x0020;	// vertex contains specular color
const unsigned int ZERO_FORMAT_TEXMASK		= 0x0F00;	// vertex texture coordinate mask
const unsigned int ZERO_FORMAT_TEXSHIFT	= 8;			// vertex texture coordinate shift
const unsigned int ZERO_FORMAT_TEX1			= 0x0100;	// vertex contains 1 texture coordinate
const unsigned int ZERO_FORMAT_TEX2			= 0x0200;	// vertex contains 2 texture coordinates
const unsigned int ZERO_FORMAT_TEX3			= 0x0300;	// vertex contains 3 texture coordinates
const unsigned int ZERO_FORMAT_TEX4			= 0x0400;	// vertex contains 4 texture coordinates
const unsigned int ZERO_FORMAT_TEX5			= 0x0500;	// vertex contains 5 texture coordinates
const unsigned int ZERO_FORMAT_TEX6			= 0x0600;	// vertex contains 6 texture coordinates
const unsigned int ZERO_FORMAT_TEX7			= 0x0700;	// vertex contains 7 texture coordinates
const unsigned int ZERO_FORMAT_TEX8			= 0x0800;	// vertex contains 8 texture coordinates
#if( PLATFORM == PLATFORM_GAMECUBE )
// gamecube specific
const unsigned int ZERO_FORMAT_COMPRESSED_POS = 0x1000; // vertex position is fixed point and tex coords are fixed point
const unsigned int ZERO_FORMAT_COMPRESSED_TEX = 0x2000; // tex coords are fixed point
const unsigned int ZERO_FORMAT_NBT            = 0x4000; // for specifying normal, binormal, and tangent
const unsigned int ZERO_FORMAT_DIRECT         = 0x8000; // give vertex data directly to the gpu don't store in a buffer
const unsigned int ZERO_FORMAT_DISPLAYLIST    = 0x010000; // store the data in a display list
#elif ( PLATFORM == PLATFORM_PS2 )
// ps2 specific
const unsigned int ZERO_FORMAT_DISPLAYLIST    = 0x01000; // store the data in a display list
#endif

// render descriptor flags
const unsigned int ZERO_RENDERFLAG_CLIP					= 0x0001;	// clip the geometry
#if (PLATFORM == PLATFORM_PS2)
const unsigned int ZERO_RENDERFLAG_UNCOMPRESSED 		= 0x0002;	// use uncompressed vertices
#endif
const unsigned int ZERO_RENDERFLAG_IMMEDIATE    		= 0x0004;	// render immediately (no bucketing)
const unsigned int ZERO_RENDERFLAG_RCV_STATIC_SHADOWS	= 0x0008;	// Receive shadows from static objects
const unsigned int ZERO_RENDERFLAG_RCV_DYN_SHADOWS		= 0x0010;	// Receive shadows from dynamic objects
const unsigned int ZERO_RENDERFLAG_RCV_TERRAIN_LIGHT    = 0x0020;   // Receive light reflected from terrain
const unsigned int ZERO_RENDERFLAG_RCV_TERRAIN_SHADOW   = 0x0040;   // Receives static shadows from the prelit terrain
const unsigned int ZERO_RENDERFLAG_ADD_COLOR            = 0x0080;   // Add the camera color
const unsigned int ZERO_RENDERFLAG_MULT_COLOR           = 0x0100;   // multiply the camera color
const unsigned int ZERO_RENDERFLAG_ZDISABLE             = 0x0200;   // disables the z buffer


// forward declaration
class ZeroCamera;
class ZeroMaterial;

// strided data array definition
class ZeroStridedArray
{
public:
	union
	{
		float *ptr;

		#if (PLATFORM == PLATFORM_GAMECUBE)
		short *cptr;               // used with the compressed position flag
		#endif

		ZeroColor *color;
		unsigned int *data;
	};

	union {	
		#if (PLATFORM == PLATFORM_GAMECUBE)
		// the stride to use when the data is compressed on gamecube
		// this is a bit of a hack ideally all strides should be bytes
		// however since the stride is dwords we need a special stride
		// for compressed data since we could have fractional dwords
		// the cstride is in words (2 bytes) and stride is in dwords (4 bytes)
		int cstride;
		#endif
		
		int stride;	// in dwords
	};
	
	int size;	// in dwords
	int count;  // only used on GC

public:
	ZeroStridedArray() : ptr(NULL), stride(0), size(0), count(0)
	{
	}
};

// vertex record definition
class ZeroVertexRecord
{
public:
	unsigned int size;				// vertex size	in dwords
	unsigned int format;			// vertex format

	ZeroStridedArray position;		// vertex position
	ZeroStridedArray weights;		// vertex weights
	ZeroStridedArray normal;		// vertex normal
	ZeroStridedArray psize;			// vertex point size
	ZeroStridedArray diffuse;		// vertex diffuse color
	ZeroStridedArray specular;		// vertex specular color
	ZeroStridedArray uv[8];			// vertex texture coordinates

#if (PLATFORM == PLATFORM_PS2)
	// ps2-specific stuff
	ZeroVector3 boxMin;				// bounding box minimum
	ZeroVector3 boxMax;				// bounding box maximum
	ZeroVector3 boxScale;			// scale box from min..max to 0..65535
	ZeroStridedArray matrixPalette;	
	ZeroStridedArray outputVertex;	// output of the vertex skinning, this buffer will disapear when we'll have the second rev
	ZeroStridedArray outputNormal;	// output of the vertex skinning, this buffer will disapear when we'll have the second rev
	ZeroStridedArray outputVertex1;	// double buffering the vertexSkinnig.
	ZeroStridedArray outputNormal1;

	void (ZeroVertexRecord::*ps2AddVertexPos4)(const float x, const float y, const float z, const float w, bool draw);
	void (ZeroVertexRecord::*ps2AddVertexPos3)(const float x, const float y, const float z, bool draw);
	void (ZeroVertexRecord::*ps2AddVertexUV)(const float u, const float v);
	void (ZeroVertexRecord::*ps2AddVertexSecondUV)(const float u, const float v);

	inline void ps2AddVertexPosUncompressed4(const float x, const float y, const float z, const float w, bool draw);
	inline void ps2AddVertexPosCompressed4(const float x, const float y, const float z, const float w, bool draw);
	inline void ps2AddVertexPosUncompressed3(const float x, const float y, const float z, bool draw);
	inline void ps2AddVertexPosCompressed3(const float x, const float y, const float z, bool draw);
	inline void ps2AddVertexUVUncompressed2(const float u, const float v);
	inline void ps2AddVertexUVCompressed2(const float u, const float v);
	inline void ps2AddVertexSecondUVUncompressed2(const float u, const float v);
	inline void ps2AddVertexSecondUVCompressed2(const float u, const float v);

#elif (PLATFORM == PLATFORM_GAMECUBE)
	// gamecube fixed point compression
	int posFracBits;                // number of bits for the fixed point has to be [0..15]
	int texFracBits;

	// function pointer to handle vertex data more efficiently
	// Direct functions will call GX functions directly and use no buffer
	// where non-direct function will load the data into a buffer

	// stub functions (function pointers that called the real function)
	// the function pointers are used to make the functions more efficient so
	// we can do all the comparisons upfront in the lock function
	void (ZeroVertexRecord::*gcAddVertexPos4)(const float x, const float y, const float z, const float w);
	void (ZeroVertexRecord::*gcAddVertexPos3)(const float x, const float y, const float z);
	void (ZeroVertexRecord::*gcAddVertexNormal4)(const float x, const float y, const float z, const float w);
	void (ZeroVertexRecord::*gcAddVertexNormal3)(const float x, const float y, const float z);
	void (ZeroVertexRecord::*gcAddVertexNBT)(const float nx, const float ny, const float nz,
	 						const float bx, const float by, const float bz,
							const float tx, const float ty, const float tz);
	void (ZeroVertexRecord::*gcAddVertexUV)(const float u, const float v);
	void (ZeroVertexRecord::*gcAddVertexSecondUV)(const float u, const float v);
	void (ZeroVertexRecord::*gcAddVertexColor)(ZeroColor c);
	void (ZeroVertexRecord::*gcAddVertexSpecular)(const unsigned int s);
	void (ZeroVertexRecord::*gcAddVertexSpecularColor)(ZeroColor c);


	inline void gcAddVertexPosDirect4(const float x, const float y, const float z, const float w);
	inline void gcAddVertexPosDirect3(const float x, const float y, const float z);
	inline void gcAddVertexPosDirectCompressed4(const float x, const float y, const float z, const float w);
	inline void gcAddVertexPosDirectCompressed3(const float x, const float y, const float z);
	inline void gcAddVertexNormalDirect4(const float x, const float y, const float z, const float w);
	inline void gcAddVertexNormalDirect3(const float x, const float y, const float z);
	inline void gcAddVertexNBTDirect(const float nx, const float ny, const float nz,
	 						   		 const float bx, const float by, const float bz,
							   		 const float tx, const float ty, const float tz);
	inline void gcAddVertexUVDirect2(const float u, const float v);
	inline void gcAddVertexUVDirectCompressed2(const float u, const float v);
	inline void gcAddVertexSecondUVDirect2(const float u, const float v);
	inline void gcAddVertexSecondUVDirectCompressed2(const float u, const float v);
	inline void gcAddVertexColorDirect(ZeroColor c);
	inline void gcAddVertexSpecularDirect(const unsigned int s);
	inline void gcAddVertexSpecularColorDirect(ZeroColor c);

	inline void gcAddVertexPosBuffered4(const float x, const float y, const float z, const float w);
	inline void gcAddVertexPosBuffered3(const float x, const float y, const float z);
	inline void gcAddVertexNormalBuffered4(const float x, const float y, const float z, const float w);
	inline void gcAddVertexNormalBuffered3(const float x, const float y, const float z);
	inline void gcAddVertexNBTBuffered(const float nx, const float ny, const float nz,
	 						   		   const float bx, const float by, const float bz,
							   		   const float tx, const float ty, const float tz);
	inline void gcAddVertexUVBuffered2(const float u, const float v);
	inline void gcAddVertexSecondUVBuffered2(const float u, const float v);
	inline void gcAddVertexColorBuffered(ZeroColor c);
	inline void gcAddVertexSpecularBuffered(const unsigned int s);
	inline void gcAddVertexSpecularColorBuffered(ZeroColor c);


#endif
	
public:
	ZeroVertexRecord ()
	{
		size = 0;
		format = 0;
		
#if (PLATFORM == PLATFORM_PS2)
		memset(&boxMin, 0, sizeof(boxMin));
		memset(&boxMax, 0, sizeof(boxMax));
		memset(&boxScale, 0, sizeof(boxScale));
#elif (PLATFORM == PLATFORM_GAMECUBE)
		posFracBits = 0;
		texFracBits = 0;
#endif
	}

#if (PLATFORM == PLATFORM_PS2)
  // Specular stuff was giving me trouble. Disabled for now. NM 3/4/02
	enum { HAVE_SPECULAR = 0};
#else
	enum { HAVE_SPECULAR = 1};
#endif

	// Vertex builder function, moves on in array. DrawTriInStrip is a
	// PS2-only flag to draw that triangle as part of the strip
	inline void AddVertexPos(const float x, const float y, const bool DrawTriInStrip, const float z = 0.0f, const float w = 1.0f) 
	{
#if (PLATFORM == PLATFORM_PS2)

		(this->*ps2AddVertexPos4)(x,y,z,w,DrawTriInStrip);

#elif (PLATFORM == PLATFORM_GAMECUBE)
		
		(this->*gcAddVertexPos4)(x,y,z,w);

#else
		position.ptr[0] = x;
		position.ptr[1] = y;
		position.ptr[2] = z;
		position.ptr[3] = w;

		position.ptr += position.stride;

#endif
	}

	// Vertex builder function, moves on in array. DrawTriInStrip is a
	// PS2-only flag to draw that triangle as part of the strip
#if (PLATFORM == PLATFORM_PS2)
	inline void AddVertexPos(const float x, const float y, const float z, const bool DrawTriInStrip=true)
#else
	inline void AddVertexPos(const float x, const float y, const float z, const bool = false) 
#endif
	{
#if (PLATFORM == PLATFORM_PS2)

		(this->*ps2AddVertexPos3)(x,y,z,DrawTriInStrip);

#elif (PLATFORM == PLATFORM_GAMECUBE)
		
		(this->*gcAddVertexPos3)(x,y,z);

#else
		position.ptr[0] = x;
		position.ptr[1] = y;
		position.ptr[2] = z;

		position.ptr += position.stride;

#endif
	}

	// Vertex builder function, moves on in array. DrawTriInStrip is a
	// PS2-only flag to draw that triangle as part of the strip
#if (PLATFORM == PLATFORM_PS2)
	inline void AddVertexPos(const float x, const float y, const float z, const float w, const bool DrawTriInStrip=true)
#else
	inline void AddVertexPos(const float x, const float y, const float z, const float w, const bool = false) 
#endif
	{
#if (PLATFORM == PLATFORM_PS2)

		(this->*ps2AddVertexPos4)(x,y,z,w,DrawTriInStrip);

#elif (PLATFORM == PLATFORM_GAMECUBE)
		
		(this->*gcAddVertexPos4)(x,y,z,w);

#else
		position.ptr[0] = x;
		position.ptr[1] = y;
		position.ptr[2] = z;
		position.ptr[3] = w;

		position.ptr += position.stride;

#endif
	}


	// Vertex builder function, moves on in array. DrawTriInStrip is a
	// PS2-only flag to draw that triangle as part of the strip
	// This is a PS2-optimized function that takes platform-native ints.
#if (PLATFORM == PLATFORM_PS2)
	inline void AddVertexPos(const int x, const int y, const int z, const bool DrawTriInStrip=true)
	{
		*((int64 *) position.ptr)= zenPOSITION_CREATE_HEX( x, y, z, DrawTriInStrip);

		position.ptr += position.stride;
	}
#endif

#if (PLATFORM != PLATFORM_PS2)
	// Vertex builder function, moves on in array.
	inline void AddVertexNormal(const float x, const float y) 
	{

#if (PLATFORM == PLATFORM_GAMECUBE)
		
		(this->*gcAddVertexNormal4)(x,y,0.0f,1.0f);

#else

		normal.ptr += normal.stride;

#endif
	}
#endif


#if (PLATFORM != PLATFORM_PS2)
	// Vertex builder function, moves on in array.
	inline void AddVertexNormal(const float x, const float y, const float z, const float = 0.0f, const float = 0.0f, const float = 0.0f,const float = 0.0f, const float = 0.0f, const float = 0.0f) 
	{
#if (PLATFORM == PLATFORM_GAMECUBE)

		(this->*gcAddVertexNormal3)(x,y,z);

#else

		normal.ptr += normal.stride;

#endif
	}
#endif


#if (PLATFORM != PLATFORM_PS2)
	// Vertex builder function, moves on in array.
	inline void AddVertexNormal(const float x, const float y, const float z, const float w, const float = 0.0f, const float = 0.0f, const float = 0.0f,const float = 0.0f, const float = 0.0f, const float = 0.0f) 
	{
#if (PLATFORM == PLATFORM_GAMECUBE)

		(this->*gcAddVertexNormal4)(x,y,z,w);

#else

		normal.ptr += normal.stride;

#endif
	}
#endif


// special normal, binormal, tangent vertex build function for gamecube only
#if (PLATFORM == PLATFORM_GAMECUBE)
	// Vertex builder function, moves on in array.
	inline void AddVertexNBT(const float nx, const float ny, const float nz,
								const float bx, const float by, const float bz,
								const float tx, const float ty, const float tz) 
	{

		(this->*gcAddVertexNBT)(nx,ny,nz,bx,by,bz,tx,ty,tz);

	}
#endif


	// Vertex builder helper function, moves on in array
	inline void AddVertexColor(ZeroColor c)
	{
#if (PLATFORM == PLATFORM_PS2)
		unsigned char b = (unsigned char) (c.r>>1);
		unsigned char g = (unsigned char) (c.g>>1);
		unsigned char r = (unsigned char) (c.b>>1);
		unsigned char a = (unsigned char) (c.a>>1 );

		unsigned int color = ( (r << 16) | (g << 8 ) | (b << 0 ) | (a << 24) );
		*((unsigned int*)diffuse.ptr) = color;
		
		diffuse.ptr += diffuse.stride;
		
#elif (PLATFORM == PLATFORM_GAMECUBE)

		(this->*gcAddVertexColor)(c);
#else

		diffuse.color[0] = c;
		
		diffuse.ptr += diffuse.stride;

#endif

	}

	// Vertex builder helper function, moves on in array. This is for
	// the first UV set
	inline void AddVertexUV(const float u, const float v)
	{
#if (PLATFORM == PLATFORM_PS2)

		(this->*ps2AddVertexUV)(u,v);

#elif (PLATFORM == PLATFORM_GAMECUBE)

		(this->*gcAddVertexUV)(u,v);
		
#else
		uv[0].ptr[0] = u;
		uv[0].ptr[1] = v;

		uv[0].ptr += uv[0].stride;
#endif
	}

	// Vertex builder helper function, moves on in array. This is for
	// the second UV set
	inline void AddVertexSecondUV(const float u, const float v)
	{
#if (PLATFORM == PLATFORM_PS2)

		(this->*ps2AddVertexSecondUV)(u,v);

#elif (PLATFORM == PLATFORM_GAMECUBE)

		(this->*gcAddVertexSecondUV)(u,v);
		
#else
		uv[1].ptr[0] = u;
		uv[1].ptr[1] = v;
		uv[1].ptr += uv[0].stride;
#endif
	}
	
	// Vertex builder helper function, moves on in array
	inline void AddVertexSpecular(const unsigned int s)
	{
#if (PLATFORM == PLATFORM_PS2)
		// Uh, we don't have specular right now on PS2. NM 3/6/02

#elif (PLATFORM == PLATFORM_GAMECUBE)

		(this->*gcAddVertexSpecular)(s);
		
#else
		specular.color[0] = s;
		specular.color += specular.stride;
#endif
	}

	// Vertex builder helper function, moves on in array
	inline void AddVertexSpecular(ZeroColor c)
	{
#if (PLATFORM == PLATFORM_PS2)
		// Uh, we don't have specular right now on PS2. NM 3/6/02

#elif (PLATFORM == PLATFORM_GAMECUBE)

		(this->*gcAddVertexSpecularColor)(c);
#else

		specular.color[0] = c;		
		specular.ptr += specular.stride;

#endif

	}

	
	// Required PS2 function-- must put the vertex data back to the start
	// of the data (start==write cursor). Not needed on GC/PC
	inline void RewindPointersBy(const int NumVertices)
	{
#if (PLATFORM == PLATFORM_PS2)
		position.ptr -= NumVertices*position.stride;
		if(diffuse.ptr != NULL)
			diffuse.ptr -= NumVertices*diffuse.stride;
		if(uv[0].ptr != NULL)
			uv[0].ptr -= NumVertices*uv[0].stride;
		if(uv[1].ptr != NULL)
			uv[1].ptr -= NumVertices*uv[1].stride;
#else
		// Nothing needed to do here due to separation of write and start
		// pointers
#endif
	}


};




class ZeroRenderDescriptorCooky
{
	protected:
	virtual ~ZeroRenderDescriptorCooky()
	{
	}

	public:
	virtual void Release()
	{
	}
};


// render descriptor definition
class ZeroRenderDescriptor 
{

	ZeroRenderDescriptorCooky *cooky;

public:
	// you fill in all this stuff...
	ZeroCamera *camera;				// owner camera

	unsigned int format;			// zero vertex format (use ZERO_FORMAT_*)
	unsigned int flags;				// zero rendering flags	

	ZeroMaterial *material;			// rendering material
	ZeroPrimitiveType primitive;	// zero primitive type (use ZERO_PRIMITIVE_*)

	
	// ZeroCamera::Lock() fills in all this stuff...
	// (no touchie after calling ZeroCamera::Unlock()!)

	int vertexCount;						// vertex count
	ZeroVertexRecord vertexRecord;	// vertex record

	int indexCount;						// index count
	unsigned short *indexArray;		// index array


public:
	// constructor
	ZeroRenderDescriptor (ZeroCamera *cameraArg) 
		:vertexRecord()
	{
		format = 0;
		flags = 0;
		camera = cameraArg;
		primitive = ZERO_PRIMITIVE_NULL;
		material = NULL;
		vertexCount = 0;
		indexCount = 0;
		indexArray = NULL;
		cooky = NULL;
	}

	~ZeroRenderDescriptor()
	{
		if (cooky) {
			cooky->Release();
		}
	}


	void SetPlatformCooky(ZeroRenderDescriptorCooky *data)
	{
		cooky = data;
	}

	ZeroRenderDescriptorCooky* GetPlatformCooky() const
	{
		return cooky;
	}

};


#if (PLATFORM == PLATFORM_PS2)

inline void ZeroVertexRecord::ps2AddVertexPosUncompressed4(const float x, const float y, const float z, const float w, bool draw)
{
	position.ptr[0] = x;
	position.ptr[1] = y;
	position.ptr[2] = z;
	position.data[3] = draw;

	position.ptr += position.stride;
}

inline void ZeroVertexRecord::ps2AddVertexPosCompressed4(const float x, const float y, const float z, const float w, bool draw)
{
	*((int64 *) position.ptr)= zenPOSITION_CREATE_HEX( x, y, z, draw);

	position.ptr += position.stride;
}

inline void ZeroVertexRecord::ps2AddVertexPosUncompressed3(const float x, const float y, const float z, bool draw)
{
	position.ptr[0] = x;
	position.ptr[1] = y;
	position.ptr[2] = z;

	position.ptr += position.stride;
}

inline void ZeroVertexRecord::ps2AddVertexPosCompressed3(const float x, const float y, const float z, bool draw)
{
	const int nx = (int)((x - boxMin.x) * boxScale.x);
	const int ny = (int)((y - boxMin.y) * boxScale.y);
	const int nz = (int)((z - boxMin.z) * boxScale.z);

	*((int64 *) position.ptr)= zenPOSITION_CREATE_HEX( nx, ny, nz, draw);

	position.ptr += position.stride;
}

inline void ZeroVertexRecord::ps2AddVertexUVUncompressed2(const float u, const float v)
{
	uv[0].ptr[0] = u;
	uv[0].ptr[1] = v;

	uv[0].ptr += uv[0].stride;
}

inline void ZeroVertexRecord::ps2AddVertexUVCompressed2(const float u, const float v)
{
	*((unsigned int*)uv[0].ptr)= zenTEXCOORD_CREATE(u,v);

	uv[0].ptr += uv[0].stride;
}

inline void ZeroVertexRecord::ps2AddVertexSecondUVUncompressed2(const float u, const float v)
{
	uv[1].ptr[0] = u;
	uv[1].ptr[1] = v;

	uv[1].ptr += uv[1].stride;
}

inline void ZeroVertexRecord::ps2AddVertexSecondUVCompressed2(const float u, const float v)
{
	*((unsigned int*)uv[1].ptr)= zenTEXCOORD_CREATE(u,v);

	uv[1].ptr += uv[1].stride;
}

#elif (PLATFORM == PLATFORM_GAMECUBE)

// these are special functions used to create display list or
// render direct data with no storage 

inline void ZeroVertexRecord::gcAddVertexPosDirect4(const float x, const float y, const float z, const float w)
{
	GXPosition3f32(x,y,z);
}

inline void ZeroVertexRecord::gcAddVertexPosDirect3(const float x, const float y, const float z)
{
	GXPosition3f32(x,y,z);
}

inline void ZeroVertexRecord::gcAddVertexPosDirectCompressed4(const float x, const float y, const float z, const float w)
{
	float _x = x * (1 << posFracBits);
	float _y = y * (1 << posFracBits);
	float _z = z * (1 << posFracBits);
	
	s16 tempx;
	s16 tempy;
	s16 tempz;
	
	OSf32tos16(&_x, &tempx);
	OSf32tos16(&_y, &tempy);
	OSf32tos16(&_z, &tempz);

	GXPosition3s16(tempx, tempy, tempz);
}

inline void ZeroVertexRecord::gcAddVertexPosDirectCompressed3(const float x, const float y, const float z)
{
	float _x = x * (1 << posFracBits);
	float _y = y * (1 << posFracBits);
	float _z = z * (1 << posFracBits);

	s16 tempx;
	s16 tempy;
	s16 tempz;
	
	OSf32tos16(&_x, &tempx);
	OSf32tos16(&_y, &tempy);
	OSf32tos16(&_z, &tempz);

	GXPosition3s16(tempx, tempy, tempz);
}

inline void ZeroVertexRecord::gcAddVertexNormalDirect4(const float x, const float y, const float z, const float w)
{
	GXNormal3f32(x,y,z);
}

inline void ZeroVertexRecord::gcAddVertexNormalDirect3(const float x, const float y, const float z)
{
	GXNormal3f32(x,y,z);
}


inline void ZeroVertexRecord::gcAddVertexNBTDirect(const float nx, const float ny, const float nz,
 						   					       const float bx, const float by, const float bz,
						   				           const float tx, const float ty, const float tz)
{
	GXNormal3f32(nx,ny,nz);
	GXNormal3f32(bx,by,bz);
	GXNormal3f32(tx,ty,tz);
}


inline void ZeroVertexRecord::gcAddVertexUVDirect2(const float u, const float v)
{
	GXTexCoord2f32(u,v);
}

inline void ZeroVertexRecord::gcAddVertexUVDirectCompressed2(const float u, const float v)
{
	float _u = u * (1 << texFracBits);
	float _v = v * (1 << texFracBits);

	s16 tempu;
	s16 tempv;
	
	OSf32tos16(&_u, &tempu);
	OSf32tos16(&_v, &tempv);

	GXTexCoord2s16(tempu, tempv);
}

inline void ZeroVertexRecord::gcAddVertexSecondUVDirect2(const float u, const float v)
{
	GXTexCoord2f32(u,v);
}

inline void ZeroVertexRecord::gcAddVertexSecondUVDirectCompressed2(const float u, const float v)
{
	float _u = u * (1 << texFracBits);
	float _v = v * (1 << texFracBits);

	s16 tempu;
	s16 tempv;
	
	OSf32tos16(&_u, &tempu);
	OSf32tos16(&_v, &tempv);

	GXTexCoord2s16(tempu, tempv);
}

inline void ZeroVertexRecord::gcAddVertexColorDirect(ZeroColor c)
{
	GXColor4u8(c.r, c.g, c.b, c.a);
}

inline void ZeroVertexRecord::gcAddVertexSpecularDirect(const unsigned int s)
{
	GXColor1u32(s);
}

inline void ZeroVertexRecord::gcAddVertexSpecularColorDirect(ZeroColor c)
{
	GXColor4u8(c.r, c.g, c.b, c.a);
}



// Non-Direct functions so all vertex data into a buffer

inline void ZeroVertexRecord::gcAddVertexPosBuffered4(const float x, const float y, const float z, const float w)
{
	if( (format & ZERO_FORMAT_COMPRESSED_POS) || 
		(format & ZERO_FORMAT_COMPRESSED_TEX) ) {
			
		if( format & ZERO_FORMAT_COMPRESSED_POS ) {   // fixed point format
			float _x = x * (1 << posFracBits);
			float _y = y * (1 << posFracBits);
			float _z = z * (1 << posFracBits);
			float _w = w * (1 << posFracBits);
			
			OSf32tos16(&_x, &position.cptr[0]);
			OSf32tos16(&_y, &position.cptr[1]);
			OSf32tos16(&_z, &position.cptr[2]);
			OSf32tos16(&_w, &position.cptr[3]);
				
			position.cptr += position.cstride;
		}
		else {

			// can only access bytes or words because of alignment problems			
			position.cptr[0] = *((u16 *)&x);
			position.cptr[1] = *((u16 *)&x + 1);
			position.cptr[2] = *((u16 *)&y);
			position.cptr[3] = *((u16 *)&y + 1);
			position.cptr[4] = *((u16 *)&z);
			position.cptr[5] = *((u16 *)&z + 1);
			position.cptr[6] = *((u16 *)&w);
			position.cptr[7] = *((u16 *)&w + 1);
			
			position.cptr += position.cstride;
		
		}			
	}
	else {
		position.ptr[0] = x;
		position.ptr[1] = y;
		position.ptr[2] = z;
		position.ptr[3] = w;
		
		position.ptr += position.stride;

	}			


}

inline void ZeroVertexRecord::gcAddVertexPosBuffered3(const float x, const float y, const float z)
{
	if( (format & ZERO_FORMAT_COMPRESSED_POS) || 
		(format & ZERO_FORMAT_COMPRESSED_TEX) ) {

		if( format & ZERO_FORMAT_COMPRESSED_POS ) {   // fixed point format
			float _x = x * (1 << posFracBits);
			float _y = y * (1 << posFracBits);
			float _z = z * (1 << posFracBits);
			
			OSf32tos16(&_x, &position.cptr[0]);
			OSf32tos16(&_y, &position.cptr[1]);
			OSf32tos16(&_z, &position.cptr[2]);

			position.cptr += position.cstride;

		}
		else {
			// can only access bytes or words because of alignment problems			
			position.cptr[0] = *((u16 *)&x);
			position.cptr[1] = *((u16 *)&x + 1);
			position.cptr[2] = *((u16 *)&y);
			position.cptr[3] = *((u16 *)&y + 1);
			position.cptr[4] = *((u16 *)&z);
			position.cptr[5] = *((u16 *)&z + 1);
			
			position.cptr += position.cstride;
		
		}
	}
	else {
		position.ptr[0] = x;
		position.ptr[1] = y;
		position.ptr[2] = z;

		position.ptr += position.stride;

	}			

}


inline void ZeroVertexRecord::gcAddVertexNormalBuffered4(const float x, const float y, const float z, const float w)
{
		if( (format & ZERO_FORMAT_COMPRESSED_POS) || 
			(format & ZERO_FORMAT_COMPRESSED_TEX) ) {
			
			// can only access bytes or words because of alignment problems			
			normal.cptr[0] = *((u16 *)&x);
			normal.cptr[1] = *((u16 *)&x + 1);
			normal.cptr[2] = *((u16 *)&y);
			normal.cptr[3] = *((u16 *)&y + 1);
			normal.cptr[4] = *((u16 *)&z);
			normal.cptr[5] = *((u16 *)&z + 1);
			normal.cptr[6] = *((u16 *)&w);
			normal.cptr[7] = *((u16 *)&w + 1);
			
			normal.cptr += normal.cstride;
		}
		else {

			normal.ptr[0] = x;
			normal.ptr[1] = y;
			normal.ptr[2] = z;
			normal.ptr[3] = w;

			normal.ptr += normal.stride;
		}
}

inline void ZeroVertexRecord::gcAddVertexNormalBuffered3(const float x, const float y, const float z)
{
	if( (format & ZERO_FORMAT_COMPRESSED_POS) || 
		(format & ZERO_FORMAT_COMPRESSED_TEX) ) {

		// can only access bytes or words because of alignment problems			
		normal.cptr[0] = *((u16 *)&x);
		normal.cptr[1] = *((u16 *)&x + 1);
		normal.cptr[2] = *((u16 *)&y);
		normal.cptr[3] = *((u16 *)&y + 1);
		normal.cptr[4] = *((u16 *)&z);
		normal.cptr[5] = *((u16 *)&z + 1);

		normal.cptr += normal.cstride;
	}
	else {

		normal.ptr[0] = x;
		normal.ptr[1] = y;
		normal.ptr[2] = z;

		normal.ptr += normal.stride;
	}

}


inline void ZeroVertexRecord::gcAddVertexNBTBuffered(const float nx, const float ny, const float nz,
 						                     		 const float bx, const float by, const float bz,
						                     		 const float tx, const float ty, const float tz) 
{

	if( (format & ZERO_FORMAT_COMPRESSED_POS) || 
		(format & ZERO_FORMAT_COMPRESSED_TEX) ) {

		// can only access bytes or words because of alignment problems			
		normal.cptr[0] = *((u16 *)&nx);
		normal.cptr[1] = *((u16 *)&nx + 1);
		normal.cptr[2] = *((u16 *)&ny);
		normal.cptr[3] = *((u16 *)&ny + 1);
		normal.cptr[4] = *((u16 *)&nz);
		normal.cptr[5] = *((u16 *)&nz + 1);

		normal.cptr[6] = *((u16 *)&bx);
		normal.cptr[7] = *((u16 *)&bx + 1);
		normal.cptr[8] = *((u16 *)&by);
		normal.cptr[9] = *((u16 *)&by + 1);
		normal.cptr[10] = *((u16 *)&bz);
		normal.cptr[11] = *((u16 *)&bz + 1);

		normal.cptr[12] = *((u16 *)&tx);
		normal.cptr[13] = *((u16 *)&tx + 1);
		normal.cptr[14] = *((u16 *)&ty);
		normal.cptr[15] = *((u16 *)&ty + 1);
		normal.cptr[16] = *((u16 *)&tz);
		normal.cptr[17] = *((u16 *)&tz + 1);

		normal.cptr += normal.cstride;
	}
	else {

		normal.ptr[0] = nx;
		normal.ptr[1] = ny;
		normal.ptr[2] = nz;
		
		normal.ptr[3] = bx;
		normal.ptr[4] = by;
		normal.ptr[5] = bz;

		normal.ptr[6] = tx;
		normal.ptr[7] = ty;
		normal.ptr[8] = tz;

		normal.ptr += normal.stride;
	}
}

inline void ZeroVertexRecord::gcAddVertexUVBuffered2(const float u, const float v)
{
	if( (format & ZERO_FORMAT_COMPRESSED_POS) || 
		(format & ZERO_FORMAT_COMPRESSED_TEX) ) {

		if( format & ZERO_FORMAT_COMPRESSED_TEX ) {
			float _u = u * (1 << texFracBits);
			float _v = v * (1 << texFracBits);
			
			OSf32tos16(&_u, &uv[0].cptr[0]);
			OSf32tos16(&_v, &uv[0].cptr[1]);

			uv[0].cptr += uv[0].cstride;
					
		}
		else {

			// can only access bytes or words because of alignment problems
			uv[0].cptr[0] = *((u16 *)&u);
			uv[0].cptr[1] = *((u16 *)&u + 1);
			
			uv[0].cptr[2] = *((u16 *)&v);
			uv[0].cptr[3] = *((u16 *)&v + 1);
			
			uv[0].cptr += uv[0].cstride;
		}
	}
	else {

		uv[0].ptr[0] = u;
		uv[0].ptr[1] = v;

		uv[0].ptr += uv[0].stride;
	}
}

inline void ZeroVertexRecord::gcAddVertexSecondUVBuffered2(const float u, const float v)
{
	if( (format & ZERO_FORMAT_COMPRESSED_POS) || 
		(format & ZERO_FORMAT_COMPRESSED_TEX) ) {

		if( format & ZERO_FORMAT_COMPRESSED_TEX ) {
			float _u = u * (1 << texFracBits);
			float _v = v * (1 << texFracBits);
			
			OSf32tos16(&_u, &uv[1].cptr[0]);
			OSf32tos16(&_v, &uv[1].cptr[1]);

			uv[1].cptr += uv[1].cstride;
					
		}
		else {

			// can only access bytes or words because of alignment problems
			uv[1].cptr[0] = *((u16 *)&u);
			uv[1].cptr[1] = *((u16 *)&u + 1);
			
			uv[1].cptr[2] = *((u16 *)&v);
			uv[1].cptr[3] = *((u16 *)&v + 1);

			uv[1].cptr += uv[0].cstride;
		}
	}
	else {

		uv[1].ptr[0] = u;
		uv[1].ptr[1] = v;

		uv[1].ptr += uv[0].stride;
	}
}

inline void ZeroVertexRecord::gcAddVertexColorBuffered(ZeroColor c)
{
	if( (format & ZERO_FORMAT_COMPRESSED_POS) || 
		(format & ZERO_FORMAT_COMPRESSED_TEX) ) {

		// can only access bytes or words because of alignment problems
		diffuse.cptr[0] = *((u16 *)&c);
		diffuse.cptr[1] = *((u16 *)&c + 1);

		diffuse.cptr += diffuse.cstride;
	}
	else {

		diffuse.color[0] = c;

		diffuse.ptr += diffuse.stride;
	}
}

inline void ZeroVertexRecord::gcAddVertexSpecularBuffered(const unsigned int s)
{
	if( (format & ZERO_FORMAT_COMPRESSED_POS) || 
		(format & ZERO_FORMAT_COMPRESSED_TEX) ) {

		// can only access bytes or words because of alignment problems
		specular.cptr[0] = *((u16 *)&s);
		specular.cptr[1] = *((u16 *)&s + 1);

		specular.cptr += specular.cstride;
	}
	else {

		specular.color[0] = s;

		specular.ptr += specular.stride;
	}
}

inline void ZeroVertexRecord::gcAddVertexSpecularColorBuffered(ZeroColor c)
{
	if( (format & ZERO_FORMAT_COMPRESSED_POS) || 
		(format & ZERO_FORMAT_COMPRESSED_TEX) ) {

		// can only access bytes or words because of alignment problems
		specular.cptr[0] = *((u16 *)&c);
		specular.cptr[1] = *((u16 *)&c + 1);

		specular.cptr += specular.cstride;
	}
	else {

		specular.color[0] = c;

		specular.ptr += specular.stride;
	}

}

#endif


#endif
