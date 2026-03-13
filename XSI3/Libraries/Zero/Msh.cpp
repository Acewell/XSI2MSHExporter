// ****************************************************************************
//
//
// **************************************************************************
#include "ZeroTypes.h"
#include "ZeroCrc.h"
#include "ZeroFile.h"
#include "ZeroStack.h"
#include "ZeroCache.h"
#include "ZeroLight.h"
#include "ZeroMatrix.h"
#include "ZeroEntity.h"
#include "ZeroCamera.h"
#include "ZeroVector3.h"
#include "ZeroGeoUtil.h"
#include "ZeroSegment.h"
#include "ZeroSurface.h"
#include "ZeroGeometry.h"
#include "ZeroMaterial.h"
#include "ZeroQuaternion.h"
#include "ZeroMeshContext.h"
#include "ZeroMeshGeometry.h"
#include "ZeroSkinGeometry.h"
#include "ZeroClothGeometry.h"
#include "ZeroTransformConstraintGeometry.h"

#include "Msh.h"
#include "ZeroDebug.h"



namespace MSH
{
	// Prototypes
	static void ParseMATD(
					ZeroFile &file, 
					unsigned size, 
					ZeroMaterial *&material, 
					const ZeroMeshContext *context);

	// block header
	struct BlockHeader
	{
		unsigned id;
		unsigned size;
	};
	

#if ((PLATFORM == PLATFORM_X86_WIN32) || (PLATFORM == PLATFORM_XBOX))
	static const int GEOMETRY_MAX_FACES		= 1024 * 16;
	static const int GEOMETRY_MAX_VERTEX	= 1024 * 16;
#else
	static const int GEOMETRY_MAX_FACES		= 1024 * 4;
	static const int GEOMETRY_MAX_VERTEX	= 1024 * 4;
#endif

	static bool mHasShadowVolumes=false;
	
	// block header stack
	static BlockHeader *headerPtr;
	static BlockHeader *headerStack;
	
	// point light data
	struct PointLightData
	{
		ZeroColorValue color;
		ZeroVector3 position;
		float startFalloff;
		float endFalloff;
	};

#ifdef _DEBUG
static unsigned baseAddress = 0;
static unsigned leadingAddress = 0;
static unsigned maxSize = 0;
#endif

	const int WORK_BUFFER_NAX_SIZE = 1024 * 128;

	template <class T> void SetBuffer (T *&output, void *&buffer, int elemnts)
	{
		char *ptr; 
		unsigned sizeInBytes;

#ifdef _DEBUG
		if (!baseAddress) {
			baseAddress	= (int) buffer;
		}
#endif

		output = (T*) buffer;
		sizeInBytes	= (sizeof (T) * elemnts + 15) & ~15;
		ptr = (char*) buffer;
		ptr += sizeInBytes;
		buffer = (void*)ptr;


#ifdef _DEBUG
		unsigned newBound;
		newBound	= (int)ptr;
		if (newBound > leadingAddress) {
			leadingAddress = newBound;
		}

		unsigned newSize;
		newSize = leadingAddress -	baseAddress;
		if (newSize > maxSize) {
			maxSize = newSize;
//			ZeroTrace (("msh loader max memory used %d bytes\n", maxSize));
		}
		ZeroAssert ((maxSize < WORK_BUFFER_NAX_SIZE, "msh loader need more than %d bytes to work", WORK_BUFFER_NAX_SIZE)); 
#endif
	}


	
	// infinite light data
	struct InfiniteLightData
	{
		ZeroColorValue color;
		ZeroVector3 direction;
	};
	
	// spot light data
	struct SpotLightData
	{
		ZeroColorValue color;
		ZeroVector3 position;
		ZeroVector3 interest;
		float startFalloff;
		float endFalloff;
		float coneAngle;
		float spreadAngle;
	};
	

	// material cache
	class MaterialCache : public ZeroVisTree<ZeroMaterial *, unsigned>
	{
	public:
		MaterialCache(void)
			: ZeroVisTree<ZeroMaterial *, unsigned>()
		{
		}
		
		~MaterialCache()
		{
			Iterator iter(*this);
			for (iter.Begin(); iter; iter++) {
				(*iter)->info->Release();
			}
		}
	};
	
	
	// SAVE A MESH TO AN OBJECT HIERARCHY FILE FORMAT (VERSION 1)
	//
	bool __cdecl Save (
		const char *nameArg, 
		ZeroObject *meshArg)
	{
		return true;
	}
	
	
	
	//
	// PARSE A GENERIC BLOCK
	//
	static void ParseNULL(ZeroFile &file, unsigned size)
	{
		// skip data
		file.SeekCur(size);
	}
		
	
	//
	// PARSE A NAME BLOCK
	//
	static void ParseNAME(ZeroFile &file, unsigned size, unsigned &value)
	{
		// allocate space for the name
		char nameBuf[256];
				
		// read the name
		file.Read8(nameBuf, 1, size);
		
		// return the CRC of the name
		strlwr(nameBuf);
				
		value = CalcStrID(nameBuf);
	}
	
	//
	// PARSE A PARENT BLOCK
	//
	static unsigned ParsePARN(ZeroFile &file, unsigned size)
	{
		unsigned name = 0;
		
		// while there is size left...
		while (size)
		{
			// read a block header
			BlockHeader header = { 0, 0 };
			file.Read32(&header, sizeof(BlockHeader), 1);
			_ASSERTE(header.id);
			
			// deduct size
			size -= sizeof(BlockHeader);
			size -= header.size;
			
			// parse the block
			switch (header.id)
			{
			case 'EMAN':
				{
					// get the mesh name
					ParseNAME(file, header.size, name);
					break;
				}
				
			default:
				{
					// skip unknown block
					ParseNULL(file, header.size);
					break;
				}
			}
		}
		
		return name;
	}
	
	
	
	
	//
	// PARSE A BOUNDING BOX BLOCK
	//
	static void ParseBBOX(ZeroFile &file, unsigned size, ZeroQuaternion &rotation, ZeroVector3 &position, ZeroVector4 &dimension)
	{
		// read the rotation
		file.Read32(&rotation, sizeof(rotation), 1);
		
		// read the position
		file.Read32(&position, sizeof(position), 1);
		
		// read the dimension
		file.Read32(&dimension, sizeof(dimension), 1);
	}
	
	
	//
	// PARSE A SCENE INFO BLOCK
	//
	static unsigned ParseSINF(ZeroFile &file, unsigned size, ZeroObject *root)
	{
		unsigned sceneNameCRC;
		char sceneName[256];
		
		sceneNameCRC = 0;
		
		// while there is size left...
		while (size)
		{
			// read a block header
			BlockHeader header = { 0, 0 };
			file.Read32(&header, sizeof(BlockHeader), 1);
			_ASSERTE(header.id);
			
			// deduct size
			size -= sizeof(BlockHeader);
			size -= header.size;
			
			// parse the block
			switch (header.id)
			{
			case 'EMAN':
				{
					// parse name block
					file.Read8(sceneName, header.size, 1);
					sceneNameCRC = CalcStrID(sceneName);
					break;
				}
				
			case 'BBOX': // 'BBOX' is wrong, but older MSH files may have this tag backwards
			case 'XOBB':
				{
					ZeroQuaternion rotation;
					ZeroVector3 position;
					ZeroVector4 dimension;
					ParseBBOX(file, header.size, rotation, position, dimension);
					root->SetObjectRotation(rotation);
					root->SetObjectPosition(position);
					root->SetDimensions(dimension);
					break;
				}
				
			default:
				{
					// skip unrecognized block
					ParseNULL(file, header.size);
					break;
				}
			}
		}
		return sceneNameCRC;
	}
	
												 
//
// PARSE A MATERIAL BLOCK
//
void ParseMATD(
					ZeroFile &file, 
					unsigned size, 
					ZeroMaterial *&material, 
					const ZeroMeshContext *context)
{
	// clear the material data
	int i;
	unsigned name = 0;
	unsigned textureCRCs[MAX_TEXTURES_PER_MSH_MATERIAL];
	for(i=0;i<MAX_TEXTURES_PER_MSH_MATERIAL;i++)	{
		textureCRCs[i]=0;
	}

//	unsigned texture0 = 0;
//	unsigned texture1 = 0;
	unsigned attrib = 0;
	ZeroColorValue diffuse(1, 1, 1, 1);
	ZeroColorValue specular(0, 0, 0, 1);
	ZeroColorValue emissive(0, 0, 0, 0);
	float power(0);
	
	// while there is size left...
	while (size)
	{
		// read a block header
		BlockHeader header = { 0, 0 };
		file.Read32(&header, sizeof(BlockHeader), 1);
		_ASSERTE(header.id);
		
		// deduct size
		size -= sizeof(BlockHeader);
		size -= header.size;
		
		// parse the block
		switch (header.id)
		{
		case 'EMAN':
			{
				// get the material name crc
				ParseNAME(file, header.size, name);
				break;
			}
			
		case 'ATAD':
			{
				if (header.size == sizeof(ZeroColorValue) * 3 + sizeof(float))
				{
					// get the material data
					file.Read32(&diffuse, sizeof(ZeroColorValue), 1);
					file.Read32(&specular, sizeof(ZeroColorValue), 1);
					file.Read32(&emissive, sizeof(ZeroColorValue), 1);
					file.Read32(&power, sizeof(float), 1);
				}
				else
				{
					// skip the material data
					ParseNULL(file, header.size);
				}
				break;
			}
			
		case 'DXET':	// [COMPATIBILITY]
		case 'D0XT':
			{
				// get the texture 0 name
				ParseNAME(file, header.size, textureCRCs[0]);
			}
			break;
			
		case 'D1XT':
		case 'D2XT':
		case 'D3XT':
		case 'D4XT':
		case 'D5XT':
		case 'D6XT':
		case 'D7XT':
		case 'D8XT':
		case 'D9XT':
			{
				unsigned idx=((0x00FF0000&header.id)>>16)-48;
				// get the texture 1 name
				ParseNAME(file, header.size, textureCRCs[idx]);
				break;
			}
			
		case 'BRTA':
			{
				// get the material attrib
				file.Read32(&attrib, header.size, 1);
				break;
			}
			
		default:
			{
				// skip unrecognized block
				ParseNULL(file, header.size);
				break;
			}
		}
	}
	

	// create a new material
	const char *textureNames[MAX_TEXTURES_PER_MSH_MATERIAL];
	for(i=0;i<MAX_TEXTURES_PER_MSH_MATERIAL;i++)	{
		textureNames[i]=InvertCRC(textureCRCs[i]);
	}

	material = context->GetMaterial(InvertCRC(name), textureNames,	attrib);
	
	// set material values
	material->diffuse = diffuse;
	material->specular = specular;
#ifdef USE_EMISSIVE_COLOR
	material->emissive = emissive;
#endif
	material->power = power;
}

					
//
// PARSE A MATERIAL LIST BLOCK
//
static void ParseMATL(
							 ZeroFile &file, 
							 unsigned size, 
							 const ZeroMeshContext *context,
							 MaterialCache& materialCache)
{
	// read the entry count
	unsigned count;
	file.Read32(&count, sizeof(unsigned), 1);
	
	// deduct size
	size -= sizeof(unsigned);
	
	// clear the index
	unsigned index = 0;
	
	// while there is size left...
	while (size)
	{
		// read a block header
		BlockHeader header = { 0, 0 };
		file.Read32(&header, sizeof(BlockHeader), 1);
		_ASSERTE(header.id);
		
		// deduct size
		size -= sizeof(BlockHeader);
		size -= header.size;
		
		// parse the block
		switch (header.id)
		{
		case 'DTAM':
			{
				// parse material block
				ZeroMaterial *material = NULL;
				ParseMATD(file, header.size, material, context);
				materialCache.Insert(material, index);
				break;
			}
			
		default:
			{
				// skip unrecognized block
				ParseNULL(file, header.size);
				break;
			}
		}
		
		// advance the index
		index++;
	}
}

//
// PARSE AN INFINITE LIGHT BLOCK
//
static void ParseLGTI(ZeroFile &file, unsigned size, const ZeroMeshContext *context, ZeroObject *root)
{
	// clear the infinite light data
	unsigned name = 0;
	InfiniteLightData data;
	memset(&data, 0, sizeof(data));
	
	// while there is size left...
	while (size)
	{
		// read a block header
		BlockHeader header = { 0, 0 };
		file.Read32(&header, sizeof(BlockHeader), 1);
		_ASSERTE(header.id);
		
		// deduct size
		size -= sizeof(BlockHeader);
		size -= header.size;
		
		// parse the block
		switch (header.id)
		{
		case 'EMAN':
			{
				// parse name block
				ParseNAME(file, header.size, name);
			}
			break;
			
		case 'ATAD':
			{
				// parse data block
				file.Read32(&data, header.size, 1);
			}
			break;
			
		default:
			{
				// skip unrecognized block
				ParseNULL(file, header.size);
			}
			break;
		}
	}
#if (PLATFORM != PLATFORM_PS2)	
	// create the light descriptor
	ZeroLightDescriptor desc;
	desc.type = DirectionalLight;
	desc.range = 1e10;
	desc.innerCone = PIBY2;
	desc.outerCone = PIBY2;
	desc.ambient = ZeroColorValue(0, 0, 0, 0);
	desc.diffuse = data.color;
	desc.specular = data.color;
	desc.dvAttenuation0 = 1.0f;
	desc.dvAttenuation1 = 0.0f;
	desc.dvAttenuation2 = 0.0f;
	desc.flags = 0;
	
	// create the light matrix
	ZeroMatrix matrix(DirectionMatrix
		(data.direction, ZeroVector3(0, 1, 0), ZeroVector3(0, 0, 0)));
	
	// create a light
	ZeroLight *light = context->GetLight(desc);
	
	// set the light matrix
	light->SetObjectMatrix(matrix);
	
	// attach to the root
	light->Attach(root);
	
	// update the light properties
	light->SetProperties();
	
	// release the light
	light->Release();
#endif
}


//
// PARSE A POINT LIGHT BLOCK
//
static void ParseLGTP(ZeroFile &file, unsigned size, const ZeroMeshContext *context, ZeroObject *root)
{
	// clear the point light data
	unsigned name = 0;
	PointLightData data;
	memset(&data, 0, sizeof(data));
	
	// while there is size left...
	while (size)
	{
		// read a block header
		BlockHeader header = { 0, 0 };
		file.Read32(&header, sizeof(BlockHeader), 1);
		_ASSERTE(header.id);
		
		// deduct size
		size -= sizeof(BlockHeader);
		size -= header.size;
		
		// parse the block
		switch (header.id)
		{
		case 'EMAN':
			{
				// parse name block
				ParseNAME(file, header.size, name);
				break;
			}
			
		case 'ATAD':
			{
				// parse data block
				file.Read32(&data, header.size, 1);
				break;
			}
			
		default:
			{
				// skip unrecognized block
				ParseNULL(file, header.size);
			}
			break;
		}
	}
	
#if (PLATFORM != PLATFORM_PS2)		
	// create the light descriptor
	ZeroLightDescriptor desc;
	desc.type = PointLight;
	desc.range = data.endFalloff;
	desc.innerCone = PIBY2;
	desc.outerCone = PIBY2;
	desc.ambient = ZeroColorValue(0, 0, 0, 0);
	desc.diffuse = data.color;
	desc.specular = data.color;
	desc.dvAttenuation0 = 1.0f;
	desc.dvAttenuation1 = 0.0f;
	desc.dvAttenuation2 = 9.0f / (desc.range * desc.range);
	desc.flags = 0;
	
	// create the light matrix
	ZeroMatrix matrix(identityMatrix);
	matrix.posit = data.position;
	
	// create a light
	ZeroLight *light = context->GetLight(desc);
	
	// set the light matrix
	light->SetObjectMatrix(matrix);
	
	// attach to the root
	light->Attach(root);
	
	// update the light properties
	light->SetProperties();
	
	// release the light
	light->Release();
#endif
}


//
// PARSE A SPOT LIGHT BLOCK
//
static void ParseLGTS(ZeroFile &file, unsigned size, const ZeroMeshContext *context, ZeroObject *root)
{
	// clear the spot light data
	unsigned name = 0;
	SpotLightData data;
	memset(&data, 0, sizeof(data));
	
	// while there is size left...
	while (size)
	{
		// read a block header
		BlockHeader header = { 0, 0 };
		file.Read32(&header, sizeof(BlockHeader), 1);
		_ASSERTE(header.id);
		
		// deduct size
		size -= sizeof(BlockHeader);
		size -= header.size;
		
		// parse the block
		switch (header.id)
		{
		case 'EMAN':
			{
				// parse name block
				ParseNAME(file, header.size, name);
			}
			break;
			
		case 'ATAD':
			{
				// parse data block
				file.Read32(&data, header.size, 1);
			}
			break;
			
		default:
			{
				// skip unrecognized block
				ParseNULL(file, header.size);
			}
			break;
		}
	}

#if (PLATFORM != PLATFORM_PS2)	
	// create the light descriptor
	ZeroLightDescriptor desc;
	desc.type = SpotLight;
	desc.range = data.endFalloff;
	desc.innerCone = data.coneAngle;
	desc.outerCone = data.spreadAngle;
	desc.ambient = ZeroColorValue(0, 0, 0, 0);
	desc.diffuse = data.color;
	desc.specular = data.color;
	desc.dvAttenuation0 = 1.0f;
	desc.dvAttenuation1 = 0.0f;
	desc.dvAttenuation2 = 9.0f / (desc.range * desc.range);
	desc.flags = 0;
	
	// create the light matrix
	ZeroMatrix matrix(DirectionMatrix
		(data.interest - data.position, ZeroVector3(0, 1, 0), data.position));
	
	// create a light
	ZeroLight *light = context->GetLight(desc);
	
	// set the light matrix
	light->SetObjectMatrix(matrix);
	
	// attach to the root
	light->Attach(root);
	
	// update the light properties
	light->SetProperties();
	
	// release the light
	light->Release();
#endif
}

//
// PARSE A TRANSFORM MATRIX BLOCK
//
static void ParseTRAN(ZeroFile &file, unsigned size, ZeroQuaternion &rotation, ZeroVector3 &position)
{
	// read the scale
	ZeroVector3 scale;
	file.Read32(&scale, sizeof(scale), 1);
	
	// read the rotation
	file.Read32(&rotation, sizeof(rotation), 1);
	
	// read the translation
	file.Read32(&position, sizeof(position), 1);
}


//
// PARSE A POSITION LIST BLOCK
//static int ParsePOSL(ZeroFile &file, unsigned size, unsigned &count, float *list)
//{
//	int vertexSize;
//
//	file.Read32(&count, sizeof(unsigned), 1);
//
//	size -= sizeof (int);
//
//	vertexSize = size / count;
//	ZeroAssert (((vertexSize * count) == size,"Trying to load a PS2 file on Win32"));
//
//	file.Read32(list, vertexSize, count);
//	return vertexSize;
//}


static void ParseWeights(
	ZeroFile &file, 
	unsigned size, 
	unsigned &count, 
	ZeroWeight *list)
{
	unsigned i;
	// read the count
	file.Read32(&count, sizeof(unsigned), 1);


	for (i = 0; i < count; i ++) {
		ZeroMatrixWeightPair weight (0, 0);
		
    	file.Read32(&weight, sizeof(ZeroMatrixWeightPair), 1);
		list[i].w1 = weight.weight;
		list[i].boneIndex[0] = (unsigned char)weight.boneID;
		
		file.Read32(&weight, sizeof(ZeroMatrixWeightPair), 1);
		list[i].w2 = weight.weight;
		list[i].boneIndex[1] = (unsigned char)weight.boneID;
		
		file.Read32(&weight, sizeof(ZeroMatrixWeightPair), 1);
		list[i].w3 = weight.weight;
		list[i].boneIndex[2] = (unsigned char)weight.boneID;

		file.Read32(&weight, sizeof(ZeroMatrixWeightPair), 1);
		list[i].boneIndex[3] = (unsigned char)weight.boneID;
	}
}


//
// PARSE A NORMAL LIST BLOCK
//
static void ParseNRML(ZeroFile &file, unsigned size, unsigned &count, ZeroVector3 *list)
{
	// read the count
	file.Read32(&count, sizeof(unsigned), 1);
	
	// read the list
	file.Read32(list, sizeof(ZeroVector3), count);
}


//
// PARSE A COLOR BASE BLOCK
//
static void ParseCLRB(ZeroFile &file, unsigned size, unsigned count, ZeroColor *list)
{
#if (PLATFORM != PLATFORM_GAMECUBE)
	ZeroColor color;
	file.Read32(&color, sizeof(ZeroColor), 1);
#else
	// on Gamecube we need to use the int cast to ZeroColor to get
	// the proper swizzling of the color bits.	
	u32 color;
	file.Read32(&color, sizeof(u32), 1);
#endif
	
	// fill the list
	for (unsigned i = 0; i < count; i++)
	{
		list[i] = color;
	}
}


//
// PARSE A COLOR LIST BLOCK
//
static void ParseCLRL(ZeroFile &file, unsigned size, unsigned &count, ZeroColor *list)
{
	// read the count
	file.Read32(&count, sizeof(unsigned), 1);
	
	// read the list
#if (PLATFORM != PLATFORM_GAMECUBE)
	file.Read32(list, sizeof(ZeroColor), count);
#else
	// on Gamecube we need to use the int cast to ZeroColor to get
	// the proper swizzling of the color bits.
	unsigned color;
	for (unsigned i = 0; i < count; i++)
	{
		file.Read32(&color, sizeof(unsigned), 1);
		list[i] = color;
	}
#endif
}


//
// PARSE A UV PAIR LIST BLOCK
//
static void ParseTXCL(ZeroFile &file, unsigned size, unsigned &count, ZeroUV *list)
{
	// read the count
	file.Read32(&count, sizeof(unsigned), 1);
	
	// read the list
	file.Read32(list, sizeof(ZeroUV), count);
}


//
// PARSE A FACE INDEX LIST
//
//static void ParseNDXT(ZeroFile &file, unsigned size, unsigned &count, unsigned short *list)
//{
//	// read the count
//	file.Read32(&count, sizeof(unsigned), 1);
//	count *= 3;
//	
//	// read the list
//	file.Read16(list, sizeof(unsigned short), count);
//
//	// realign the read pointer
//	file.SeekCur (size - count * sizeof(unsigned short) - sizeof (unsigned));
//}

//static void ParseSTRP(ZeroFile &file, unsigned size, unsigned &count, unsigned short *list)
//{
//	// read the count
//	file.Read32(&count, sizeof(unsigned), 1);
//	
//	// read the list
//	file.Read16(list, sizeof(unsigned short), count);
//
//	// realign the read pointer
//	file.SeekCur (size - count * sizeof(unsigned short) - sizeof (unsigned));
//}



#ifdef _DEBUG
static int ConverPs2VertexDataToPcVertexData (ZeroVertexRecord& vertexRecord, int vertexCount, unsigned short indexArray[])
{
	int i;
	int i0;
	int i1;
	int indexCount;
	float *ptr;
	float *ptr1;
	bool clock;


	i0 = 0;
	i1 = 0;

	indexCount = 0;
	ptr = vertexRecord.position.ptr;
	clock	= true;

	for (i =0 ; i < vertexCount; i ++)	{
		int flag;
		flag =*( (int*) (&ptr[3]));

		if (flag & 0x8000) {
			clock	= true;
		} else {
			if (clock) {
			  indexArray[indexCount + 0] = (unsigned short)i0;
			  indexArray[indexCount + 1] = (unsigned short)i1;
			  indexArray[indexCount + 2] = (unsigned short)i;
			} else {
			  indexArray[indexCount + 0] = (unsigned short)i0;
			  indexArray[indexCount + 2] = (unsigned short)i1;
			  indexArray[indexCount + 1] = (unsigned short)i;
			}
			indexCount += 3;
			clock	= !clock;
		}

		i0 = i1;
		i1 = i;

		ptr += vertexRecord.position.stride;
	}


	ptr = vertexRecord.position.ptr;
	ptr1 = vertexRecord.position.ptr;
	for (i =0 ; i < vertexCount; i ++)	{
		memcpy (ptr, ptr1, sizeof (ZeroVector3));
		ptr += 3;
		ptr1 += vertexRecord.position.stride;
	}


	vertexRecord.size -= vertexRecord.position.size;
	vertexRecord.size	+= 3;

	vertexRecord.position.size = 3;
	vertexRecord.position.stride = 3;


	return indexCount;
}
#endif


//
// PARSE A SEGMENT BLOCK
//
static void ParseSEGM(
	ZeroFile &file, 
	unsigned size, 
	const ZeroMeshContext *context,
	const MaterialCache& materialCache,
	ZeroGeometry *geometry, 
	ZeroMshModelTypes modelType,
	unsigned hiddenSegments, 
	void *workBuffer)
{
	int newPos;
	int filePos;
	int diffuseSize;
	int specularSize;
	ZeroSegment *segment;
	ZeroMaterial *materialPtr;

	float *vertexArray;
	ZeroWeight* weightArray;
	ZeroVector3* normalArray; 
	ZeroColor* colorArray;
	ZeroColor* lightArray;
	unsigned short* indexArray;
	ZeroUV* uvArrays[MAX_TEXTURES_PER_MSH_MATERIAL];


	vertexArray	= NULL;
	weightArray = NULL;
	normalArray = NULL;
	colorArray = NULL;
	lightArray = NULL;
	indexArray = NULL;

	materialPtr = NULL;
	diffuseSize = 0;
	specularSize = 0;

	ZeroSegmenData segmentData;
	ZeroVertexRecord& vertexRecord = segmentData.vertexRecord;
	const char *textureNames[MAX_TEXTURES_PER_MSH_MATERIAL];
	// while there is size left...
	while (size) {
		// read a block header
		BlockHeader header = { 0, 0 };
		file.Read32(&header, sizeof(BlockHeader), 1);
		_ASSERTE(header.id);
		
		// deduct size
		size -= sizeof(BlockHeader);
		size -= header.size;
		
		filePos = file.Tell();
		// parse the block
		switch (header.id) {
			case 'WDHS':
			{
				// parse shadow volume
				unsigned vertexCount;
				unsigned indexCount;


				file.Read32(&vertexCount, sizeof(unsigned), 1);
				SetBuffer (vertexArray, workBuffer, vertexCount * sizeof (ZeroVector3) / sizeof (float));
				file.Read32(&vertexArray[0], sizeof (ZeroVector3), vertexCount);

				segmentData.vertexCount = vertexCount;
				vertexRecord.position.ptr = &vertexArray[0];
				vertexRecord.position.stride = sizeof (ZeroVector3) / sizeof (float);
				vertexRecord.position.size = sizeof (ZeroVector3) / sizeof (float);
				vertexRecord.size += sizeof (ZeroVector3) / sizeof (float);

				// get the edge index list
				file.Read32(&indexCount, sizeof(unsigned), 1);
				SetBuffer (indexArray, workBuffer, indexCount * sizeof(ZeroMshShadowEdge) / sizeof(short));
				file.Read16(indexArray, sizeof(ZeroMshShadowEdge), indexCount);


				segmentData.primitiveType = ZERO_PRIMITIVE_TRIANGLESTRIP;
				segmentData.indexCount = indexCount * sizeof (ZeroMshShadowEdge) / sizeof (unsigned short);
				segmentData.indexArray = &indexArray[0];
				break;
			}

			case 'ITAM':
			{
				// parse the material data
				unsigned index = 0;
				file.Read32(&index, header.size, 1);
				MaterialCache::ZeroTreeNode *node = materialCache.Find(index);
				if (node)
				{
					materialPtr = node->info;
					materialPtr->AddRef();
				}
				break;
			}
			
			case 'LSOP':
			{
				// get the vertex position list
				int vertexSize;
				unsigned bytes;
				unsigned vertexCount;

				bytes	= header.size - sizeof (int);

				file.Read32(&vertexCount, sizeof(unsigned), 1);
				vertexSize = bytes / vertexCount;
				ZeroAssert (((vertexSize * vertexCount) == bytes, "Trying to load a PS2 file on Win32"));

				SetBuffer (vertexArray, workBuffer, vertexCount * vertexSize / sizeof (float));
				file.Read32(&vertexArray[0], vertexSize, vertexCount);

				segmentData.vertexCount = vertexCount;
				vertexRecord.position.ptr = &vertexArray[0];
				vertexRecord.position.stride = vertexSize / sizeof (float);
				vertexRecord.position.size =vertexSize / sizeof (float);
				vertexRecord.size += vertexSize / sizeof (float);
				break;
			}
			
			case 'THGW':
			{
				unsigned vertexCount;
				vertexCount	= segmentData.vertexCount;

				SetBuffer (weightArray, workBuffer, vertexCount);

				ParseWeights(file, header.size, vertexCount, &weightArray[0]);
				vertexRecord.format |= ZERO_FORMAT_WEIGHTS;
				vertexRecord.weights.ptr = (float*)&weightArray[0];
				vertexRecord.weights.stride = sizeof (ZeroWeight) / sizeof (float);
				vertexRecord.weights.size = sizeof (ZeroWeight) / sizeof (float);
				vertexRecord.size += sizeof (ZeroWeight) / sizeof (float);
				break;
			}
			
			
			case 'LMRN':
			{
				unsigned vertexCount;
				vertexCount	= segmentData.vertexCount;
				// get the vertex normal list

				SetBuffer (normalArray, workBuffer, vertexCount);

				ParseNRML(file, header.size, vertexCount, &normalArray[0]);

				vertexRecord.format |= ZERO_FORMAT_NORMAL;
				vertexRecord.normal.ptr = (float*)&normalArray[0];
				vertexRecord.normal.stride = sizeof (ZeroVector3) / sizeof (float);
				vertexRecord.normal.size = sizeof (ZeroVector3) / sizeof (float);
				vertexRecord.size += sizeof (ZeroVector3) / sizeof (float);
				break;
			}
			
			case 'BRLC':
			{
				unsigned vertexCount;
				vertexCount	= segmentData.vertexCount;
				// get the vertex color list

				if (!colorArray) {
					SetBuffer (colorArray, workBuffer, vertexCount);
				}

				if (!lightArray) {
					SetBuffer (lightArray, workBuffer, vertexCount);
				}


				ParseCLRB(file, header.size, vertexCount, &colorArray[0]);
				if (!(vertexRecord.format & ZERO_FORMAT_DIFFUSE))
				{
					memset(&lightArray[0], 0xFF, vertexCount * sizeof(ZeroColor));
				}

				vertexRecord.format |= ZERO_FORMAT_DIFFUSE;
				vertexRecord.diffuse.ptr = (float*)&colorArray[0];
				vertexRecord.diffuse.stride = sizeof (ZeroColor) / sizeof (float);
				vertexRecord.diffuse.size = sizeof (ZeroColor) / sizeof (float);
				diffuseSize = sizeof (ZeroColor) / sizeof (float);

				break;
			}
			
			case 'LRLC':
			{
				unsigned vertexCount;
				vertexCount	= segmentData.vertexCount;
				// get the vertex color list

				if (!colorArray) {
					SetBuffer (colorArray, workBuffer, vertexCount);
				}

				if (!lightArray) {
					SetBuffer (lightArray, workBuffer, vertexCount);
				}

				ParseCLRL(file, header.size, vertexCount, &colorArray[0]);
				if (!(vertexRecord.format & ZERO_FORMAT_DIFFUSE))
				{
					memset(&lightArray[0], 0xFF, vertexCount * sizeof(ZeroColor));
				}

				vertexRecord.format |= ZERO_FORMAT_DIFFUSE;
				vertexRecord.diffuse.ptr = (float*)&colorArray[0];
				vertexRecord.diffuse.stride = sizeof (ZeroColor) / sizeof (float);
				vertexRecord.diffuse.size = sizeof (ZeroColor) / sizeof (float);
				diffuseSize = sizeof (ZeroColor) / sizeof (float);

				break;
			}
			
			case 'BTLV':
			{
				unsigned vertexCount;
				vertexCount	= segmentData.vertexCount;
				// get the vertex color list

				if (!colorArray) {
					SetBuffer (colorArray, workBuffer, vertexCount);
				}

				if (!lightArray) {
					SetBuffer (lightArray, workBuffer, vertexCount);
				}


				ParseCLRB(file, header.size, vertexCount, &lightArray[0]);
				if (!(vertexRecord.format & ZERO_FORMAT_DIFFUSE))
				{
					memset(&colorArray[0], 0xFF, vertexCount * sizeof(ZeroColor));
				}
				vertexRecord.format |= ZERO_FORMAT_DIFFUSE;
				diffuseSize = sizeof (ZeroColor) / sizeof (float);
				
				// if there is a material...
				if (materialPtr)
				{
					for(int i=0;i<MAX_TEXTURES_PER_MSH_MATERIAL;i++)
						textureNames[i]=materialPtr->GetSurface(i) ? materialPtr->GetSurface(i)->GetName() : NULL;

					// replace the material
					ZeroMaterial *materialNew = context->GetMaterial(
						materialPtr->GetName(), 
						textureNames,
						materialPtr->GetAttrib() | 1);
					materialNew->diffuse = materialPtr->diffuse;
					materialNew->specular = materialPtr->specular;
					materialNew->emissive = materialPtr->emissive;
					materialNew->power = materialPtr->power;
					materialPtr->Release();
					materialPtr = materialNew;
					
				}

				vertexRecord.diffuse.ptr = (float*)&lightArray[0];
				vertexRecord.diffuse.stride = sizeof (ZeroColor) / sizeof (float);
				vertexRecord.diffuse.size = sizeof (ZeroColor) / sizeof (float);
				break;
			}
			
			case 'LTLV':
			{
				unsigned vertexCount;
				vertexCount	= segmentData.vertexCount;
				// get the vertex color list

				if (!colorArray) {
					SetBuffer (colorArray, workBuffer, vertexCount);
				}

				if (!lightArray) {
					SetBuffer (lightArray, workBuffer, vertexCount);
				}

				ParseCLRL(file, header.size, vertexCount, &lightArray[0]);
				if (!(vertexRecord.format & ZERO_FORMAT_DIFFUSE))
				{
					memset(&colorArray[0], 0xFF, vertexCount * sizeof(ZeroColor));
				}
				vertexRecord.format  |= ZERO_FORMAT_DIFFUSE;
				diffuseSize = sizeof (ZeroColor) / sizeof (float);
				
				// if there is a material...
				if (materialPtr)
				{
					for(int i=0;i<MAX_TEXTURES_PER_MSH_MATERIAL;i++)
						textureNames[i]=materialPtr->GetSurface(i) ? materialPtr->GetSurface(i)->GetName() : NULL;

					ZeroMaterial *materialNew = context->GetMaterial(materialPtr->GetName(), 
						 textureNames,
						 materialPtr->GetAttrib() | 1);
					materialNew->diffuse = materialPtr->diffuse;
					materialNew->specular = materialPtr->specular;
					materialNew->emissive = materialPtr->emissive;
					materialNew->power = materialPtr->power;
					materialPtr->Release();
					materialPtr = materialNew;
					
				}

				vertexRecord.diffuse.ptr = (float*)&lightArray[0];
				vertexRecord.diffuse.stride = sizeof (ZeroColor) / sizeof (float);
				vertexRecord.diffuse.size = sizeof (ZeroColor) / sizeof (float);
				break;
			}
			
			case 'L9VU':
			case 'L8VU':
			case 'L7VU':
			case 'L6VU':
			case 'L5VU':
			case 'L4VU':
			case 'L3VU':
			case 'L2VU':
			case 'L1VU':
			case 'L0VU':
			case 'L\x1fVU':	// L1VU
			case 'L\x20VU':	// L2VU
			case 'L\x21VU':	// L3VU
			case 'L\x22VU':	// L4VU
			case 'L\x23VU':	// L5VU
			case 'L\x24VU':	// L6VU
			case 'L\x25VU':	// L7VU
			case 'L\x26VU':	// L8VU
			case 'L\x27VU':	// L9VU
			{
				unsigned idx=((0x00FF0000&header.id)>>16);
				idx -= (idx < 48 ) ? 30 : '0';
				unsigned format=ZERO_FORMAT_TEX1+ZERO_FORMAT_TEX1*idx;

				unsigned vertexCount;
				vertexCount	= segmentData.vertexCount;
				for(unsigned int i=0;i<idx;i++)
				{
					//
					//	if there is no uv[i] list
					if((vertexRecord.format & ZERO_FORMAT_TEXMASK)<ZERO_FORMAT_TEX1+ZERO_FORMAT_TEX1*i)
					{
						// create the vertex uv list
						SetBuffer (uvArrays[i], workBuffer, vertexCount);
						memset(&uvArrays[i][0], 0x00, vertexCount * sizeof(ZeroUV));

						vertexRecord.uv[i].ptr = (float*)&uvArrays[i][0];
						vertexRecord.uv[i].stride = sizeof (ZeroUV) / sizeof (float);
						vertexRecord.uv[i].size = sizeof (ZeroUV) / sizeof (float);
						vertexRecord.size += sizeof (ZeroUV) / sizeof (float);
					}
				}

				// get the vertex uv 1 list
				SetBuffer (uvArrays[idx], workBuffer, vertexCount);
				ParseTXCL(file, header.size, vertexCount, &uvArrays[idx][0]);

				vertexRecord.format &= ~ZERO_FORMAT_TEXMASK;
				vertexRecord.format  |= format;
				vertexRecord.uv[idx].ptr = (float*)&uvArrays[idx][0];
				vertexRecord.uv[idx].stride = sizeof (ZeroUV) / sizeof (float);
				vertexRecord.uv[idx].size = sizeof (ZeroUV) / sizeof (float);
				vertexRecord.size += sizeof (ZeroUV) / sizeof (float);
				break;
			}
			
			case 'TXDN':
			case 'LXDN':	// backwards-compatibility
			{
				unsigned indexCount;
				unsigned faceCount;
				// get the face index list


				file.Read32(&faceCount, sizeof(unsigned), 1);
				SetBuffer (indexArray, workBuffer, faceCount * 3);

				indexCount = faceCount * 3;
//				ParseNDXT(file, header.size, indexCount, &indexArray[0]);
				file.Read16(indexArray, sizeof (short), indexCount);

				segmentData.primitiveType = ZERO_PRIMITIVE_TRIANGLELIST;
				segmentData.indexCount = indexCount;
				segmentData.indexArray = &indexArray[0];

				file.SeekCur (header.size - indexCount * sizeof(unsigned short) - sizeof (unsigned));
				break;
			}
			
			case 'PRTS':
			{
				unsigned indexCount;
				// get the face index list


				file.Read32(&indexCount, sizeof(unsigned), 1);

				SetBuffer (indexArray, workBuffer, indexCount);

//				ParseSTRP(file, header.size, indexCount, &indexArray[0]);

				// read the list
				file.Read16(indexArray, sizeof(unsigned short), indexCount);


				segmentData.primitiveType = ZERO_PRIMITIVE_TRIANGLESTRIP;
				segmentData.indexCount = indexCount;
				segmentData.indexArray = &indexArray[0];

				// realign the read pointer
				file.SeekCur (header.size - indexCount * sizeof(unsigned short) - sizeof (unsigned));
				break;
			}
			
			default:
			{
				// skip unrecognized block
//				if (segmentData.extraData) {
//					ParseNULL(file, header.size);
//				} else {
//					_ASSERTE (header.size < 2048);
//					segmentData.extraData = &extraDta[0];
//					file.Read8(&extraDta[0], 1, header.size);
//				}
				ParseNULL(file, header.size);

				break;
			}
			}
			newPos = file.Tell();
			_ASSERTE (((newPos - filePos) == (int)header.size));
		}

		// if there is actual segment data...
		if (segmentData.vertexCount > 0)
		{
			// old ps2 file don't not have primitive type, they asume triangle strip.
			// so if vertexRecord.primitiveType = set ZERO_PRIMITIVE_NULL asume aps2 file
			if (segmentData.primitiveType == ZERO_PRIMITIVE_NULL) {
				if (vertexRecord.position.stride == 4) {
					segmentData.primitiveType	= ZERO_PRIMITIVE_TRIANGLESTRIP;
				}
			}

			
			// if there is no uv 0 list,
			// there is a material,
			// and the material has a texture 0...
			if (!(vertexRecord.format & ZERO_FORMAT_TEXMASK) && materialPtr && materialPtr->GetSurface(0))
			{
				// create the vertex uv 0 list
				vertexRecord.format |= ZERO_FORMAT_TEX1;

				SetBuffer (uvArrays[0], workBuffer, segmentData.vertexCount);

				memset(&uvArrays[0][0], 0x00,  segmentData.vertexCount * sizeof(ZeroUV));

				vertexRecord.uv[0].ptr = (float*)&uvArrays[0][0];
				vertexRecord.uv[0].stride = sizeof (ZeroUV) / sizeof (float);
				vertexRecord.uv[0].size = sizeof (ZeroUV) / sizeof (float);
				vertexRecord.size += sizeof (ZeroUV) / sizeof (float);
			}
			
			vertexRecord.size += diffuseSize;
			vertexRecord.size += specularSize;

	//		ZeroAssert ((indexCount > 0, "Segment has no index list"));
	#ifdef _DEBUG
			// if the index count equal zero it must be trying to load a ps2 model to the pc
			if (!segmentData.indexCount) {
				segmentData.primitiveType = ZERO_PRIMITIVE_TRIANGLELIST;
				segmentData.indexArray = &indexArray[0];
			   segmentData.indexCount = ConverPs2VertexDataToPcVertexData (vertexRecord, segmentData.vertexCount, segmentData.indexArray);
			}
	#endif

			if (vertexRecord.format & ZERO_FORMAT_DIFFUSE) {
				// copy diffuse data
				for (int i = 0; i < segmentData.vertexCount; i++) {

					_ASSERTE (colorArray);
					_ASSERTE (lightArray);
					ZeroColor &color = vertexRecord.diffuse.color[i * vertexRecord.diffuse.stride];
					color.r = (unsigned char)(colorArray[i].r * lightArray[i].r / 255);
					color.g = (unsigned char)(colorArray[i].g * lightArray[i].g / 255);
					color.b = (unsigned char)(colorArray[i].b * lightArray[i].b / 255);
					color.a = (unsigned char)(colorArray[i].a * lightArray[i].a / 255);

				}
			}
			
			segmentData.modelType = modelType;
			segmentData.material = materialPtr;
			segmentData.bIsDynamicVB = false;
			segmentData.bIsDynamicContent = false;

			segment = context->GetSegment(segmentData);

			if (!segment->GetHiddenFlag())	// context->GetSegment() may want to force hide this particular segment
				segment->SetHiddenFlag( hiddenSegments & 0x01);

			// set the bounding sphere
			segment->SetSphere(geometry->GetSphere());

			//set the Shadow Properties
			segment->SetShadowProperties(mHasShadowVolumes);

			// attach the segment
			geometry->AddSegment(segment);
			
			// release the segment
			segment->Release();
		}

		if (materialPtr)
		{
			// release the material
			materialPtr->Release();
		}
	}
	
	//
	// PARSE A GAMECUBE SEGMENT BLOCK
	//
	static void ParseGSEG(ZeroFile &file, unsigned size, const ZeroMeshContext *context,
	                      const MaterialCache& materialCache, ZeroGeometry *geometry, ZeroMshModelTypes modelType,
	                      unsigned hiddenSegments, void *workBuffer)
	{
#if (PLATFORM != PLATFORM_GAMECUBE)
		// Trying to read gamecube segment on non-gamecube platform
		_ASSERTE( 0 );
#endif
		int newPos;
		int filePos;
		int diffuseSize;
		ZeroSegment *segment;
		ZeroMaterial *materialPtr;

		float *vertexArray;
		ZeroWeight* weightArray;
		ZeroVector3* normalArray; 
		ZeroColor* colorArray;
		unsigned short* indexArray;
		ZeroUV* uvArrays[MAX_TEXTURES_PER_MSH_MATERIAL];

		vertexArray	= NULL;
		weightArray = NULL;
		normalArray = NULL;
		colorArray = NULL;
		indexArray = NULL;

		materialPtr = NULL;
		diffuseSize = 0;

		ZeroSegmenData segmentData;
		ZeroVertexRecord& vertexRecord = segmentData.vertexRecord;
		
		// while there is size left...
		while (size) 
		{
			// read a block header
			BlockHeader header = { 0, 0 };
			file.Read32(&header, sizeof(BlockHeader), 1);
			_ASSERTE(header.id);
			
			// deduct size
			size -= sizeof(BlockHeader);
			size -= header.size;
			
			filePos = file.Tell();
			// parse the block
			switch (header.id) 
			{
				case 'WDHS':
				case 'LXDN':	// backwards-compatibility
				case 'BRLC':
				case 'BTLV':
				case 'LTLV':
				{
					// not supported
					// all these blocks should be stripped by GCAsset, in the case of
					// color blocks they are merged into CLRL
					_ASSERT( 0 );
					ParseNULL(file, header.size);
					break;
				}

				case 'ITAM':
				{
					// parse the material data
					unsigned index = 0;
					file.Read32(&index, header.size, 1);
					MaterialCache::ZeroTreeNode *node = materialCache.Find(index);
					if (node)
					{
						materialPtr = node->info;
						materialPtr->AddRef();
					}
					break;
				}
				
				case 'LSOP':
				{
					// get the vertex position list
					int vertexSize;
					unsigned bytes;
					unsigned vertexCount;

					bytes	= header.size - sizeof (int);

					file.Read32(&vertexCount, sizeof(unsigned), 1);
					vertexSize = bytes / vertexCount;
					ZeroAssert (((vertexSize * vertexCount) == bytes, "Trying to load a PS2 file on Win32"));

					SetBuffer (vertexArray, workBuffer, vertexCount * vertexSize / sizeof (float));
					file.Read32(&vertexArray[0], vertexSize, vertexCount);

					segmentData.vertexCount = vertexCount;
					vertexRecord.position.ptr = &vertexArray[0];
					vertexRecord.position.stride = vertexSize / sizeof (float);
					vertexRecord.position.size =vertexSize / sizeof (float);
					vertexRecord.position.count = vertexCount;
					vertexRecord.size += vertexSize / sizeof (float);
					break;
				}
				
				case 'THGW':
				{
					unsigned vertexCount;
					// read the count
					file.Read32(&vertexCount, sizeof(unsigned), 1);
					SetBuffer (weightArray, workBuffer, vertexCount);
					
					for (unsigned i = 0; i < vertexCount; i ++) 
					{
						ZeroMatrixWeightPair weight (0, 0);
						
				    	file.Read32(&weight, sizeof(ZeroMatrixWeightPair), 1);
						weightArray[i].w1 = weight.weight;
						weightArray[i].boneIndex[0] = (unsigned char)weight.boneID;
						
						file.Read32(&weight, sizeof(ZeroMatrixWeightPair), 1);
						weightArray[i].w2 = weight.weight;
						weightArray[i].boneIndex[1] = (unsigned char)weight.boneID;
						
						file.Read32(&weight, sizeof(ZeroMatrixWeightPair), 1);
						weightArray[i].w3 = weight.weight;
						weightArray[i].boneIndex[2] = (unsigned char)weight.boneID;

						file.Read32(&weight, sizeof(ZeroMatrixWeightPair), 1);
						weightArray[i].boneIndex[3] = (unsigned char)weight.boneID;
					}
					
					vertexRecord.format |= ZERO_FORMAT_WEIGHTS;
					vertexRecord.weights.ptr = (float*)&weightArray[0];
					vertexRecord.weights.stride = sizeof (ZeroWeight) / sizeof (float);
					vertexRecord.weights.size = sizeof (ZeroWeight) / sizeof (float);
					vertexRecord.weights.count = vertexCount;
					vertexRecord.size += sizeof(ZeroWeight) / sizeof (float);
					break;
				}				
				
				case 'LMRN':
				{
					// get the vertex normal list
					unsigned vertexCount;
					
					// read the count
					file.Read32(&vertexCount, sizeof(unsigned), 1);
					
					// read the list
					SetBuffer(normalArray, workBuffer, vertexCount);
					file.Read32(normalArray, sizeof(ZeroVector3), vertexCount);

					vertexRecord.format |= ZERO_FORMAT_NORMAL;
					vertexRecord.normal.ptr = (float*)&normalArray[0];
					vertexRecord.normal.stride = sizeof (ZeroVector3) / sizeof (float);
					vertexRecord.normal.size   = sizeof (ZeroVector3) / sizeof (float);
					vertexRecord.normal.count = vertexCount;
					vertexRecord.size += sizeof (ZeroVector3) / sizeof (float);
					break;
				}
								
				case 'LRLC':
				{
					// get the vertex color list
					unsigned vertexCount;
					
					// read the count
					file.Read32(&vertexCount, sizeof(unsigned), 1);
					
					SetBuffer(colorArray, workBuffer, vertexCount);
					// on Gamecube we need to use the int cast to ZeroColor to get
					// the proper swizzling of the color bits.
					unsigned color;
					for (unsigned i = 0; i < vertexCount; i++)
					{
						file.Read32(&color, sizeof(unsigned), 1);
						colorArray[i] = color;
					}
					vertexRecord.format |= ZERO_FORMAT_DIFFUSE;
					vertexRecord.diffuse.ptr = (float*)&colorArray[0];
					vertexRecord.diffuse.stride = sizeof (ZeroColor) / sizeof (float);
					vertexRecord.diffuse.size = sizeof (ZeroColor) / sizeof (float);
					vertexRecord.diffuse.count = vertexCount;
					vertexRecord.size += sizeof (ZeroColor) / sizeof (float);
					break;
				}
								
				case 'L9VU':
				case 'L8VU':
				case 'L7VU':
				case 'L6VU':
				case 'L5VU':
				case 'L4VU':
				case 'L3VU':
				case 'L2VU':
				case 'L1VU':
				case 'L0VU':
				case 'L\x1fVU':	// L1VU
				case 'L\x20VU':	// L2VU
				case 'L\x21VU':	// L3VU
				case 'L\x22VU':	// L4VU
				case 'L\x23VU':	// L5VU
				case 'L\x24VU':	// L6VU
				case 'L\x25VU':	// L7VU
				case 'L\x26VU':	// L8VU
				case 'L\x27VU':	// L9VU
				{
					unsigned idx=((0x00FF0000&header.id)>>16);
					idx -= (idx < 48 ) ? 30 : '0';
					unsigned format=ZERO_FORMAT_TEX1+ZERO_FORMAT_TEX1*idx;

					// get the vertex uv 1 list
					unsigned vertexCount;
					file.Read32(&vertexCount, sizeof(unsigned), 1);
					SetBuffer (uvArrays[idx], workBuffer, vertexCount);
					
					// read the list
					file.Read32(uvArrays[idx], sizeof(ZeroUV), vertexCount);

					if ((vertexRecord.format & ZERO_FORMAT_TEXMASK) < format)
					{
						vertexRecord.format &= ~ZERO_FORMAT_TEXMASK;
						vertexRecord.format  |= format;
					}
					vertexRecord.uv[idx].ptr = (float*)&uvArrays[idx][0];
					vertexRecord.uv[idx].stride = sizeof (ZeroUV) / sizeof (float);
					vertexRecord.uv[idx].size = sizeof (ZeroUV) / sizeof (float);
					vertexRecord.uv[idx].count = vertexCount;
					vertexRecord.size += sizeof (ZeroUV) / sizeof (float);
					break;
				 }
				
				// Here's where the GSEG block gets whacky... we have seperate index lists
				// for position, normal, color and texture co-ordinates
				case 'TXDN':
				{
					unsigned faceCount;
					unsigned indexCount;

					file.Read32(&faceCount, sizeof(unsigned), 1);
					SetBuffer (indexArray, workBuffer, faceCount * 3);

					indexCount = faceCount * 3;
					file.Read16(indexArray, sizeof(short), indexCount);

					segmentData.primitiveType = ZERO_PRIMITIVE_GAMECUBETRILIST;
					segmentData.indexCount = indexCount;
					segmentData.indexArray = &indexArray[0];

					file.SeekCur(header.size - indexCount * sizeof(unsigned short) - sizeof (unsigned));
					break;
				}
				
				case 'PRTS':
				{					
					// read the position index list
					unsigned indexCount;
					file.Read32(&indexCount, sizeof(unsigned), 1);

					// the index arrays appear one after each other and all go in indexArray
					SetBuffer(indexArray, workBuffer, indexCount);
					file.Read16(indexArray, sizeof(unsigned short), indexCount);
					
					segmentData.primitiveType = ZERO_PRIMITIVE_GAMECUBETRISTRIP;
					segmentData.indexCount = indexCount;
					segmentData.indexArray = &indexArray[0];

					// realign the read pointer
					file.SeekCur (header.size - indexCount * sizeof(unsigned short) - sizeof (unsigned));
					break;
				}
				
				default:
				{
					// skip unrecognized block
					ParseNULL(file, header.size);
					break;
				}
			}
			newPos = file.Tell();
			_ASSERTE (((newPos - filePos) == (int)header.size));
		}		
		// old ps2 file don't not have primitive type, they asume triangle strip.
		// so if vertexRecord.primitiveType = set ZERO_PRIMITIVE_NULL assume a ps2 file
		if (segmentData.primitiveType == ZERO_PRIMITIVE_NULL) 
			_ASSERT( 0 ); // if this really is happening I will fix it up in GCAsset
		
		// if there is no uv 0 list, there is a material, and the material has a texture 0...
		if (!(vertexRecord.format & ZERO_FORMAT_TEXMASK) && materialPtr && materialPtr->GetSurface(0))
			_ASSERT( 0 ); // if this really is happening I will fix it up in GCAsset

		if (!segmentData.indexCount) 
		{
			// if this really is happening I will fix it up in GCAsset		
			ZeroWarn((segmentData.indexCount, "segmentData.indexCount is 0"  ));
			return; 
		}

		segmentData.modelType = modelType;
		segmentData.material = materialPtr;
		segmentData.bIsDynamicVB = false;
		segmentData.bIsDynamicContent = false;

		segment = context->GetSegment(segmentData);

		// release the material
		if (materialPtr)
			materialPtr->Release();

		if ( segment->GetHiddenFlag() == false )	// context->GetSegment() may want to force hide this particular segment
			segment->SetHiddenFlag( hiddenSegments & 0x01);

		// set the bounding sphere
		segment->SetSphere(geometry->GetSphere());

		// attach the segment
		geometry->AddSegment(segment);
		
		// release the segment
		segment->Release();
	}

	//
	// PARSE A GEOMETRY BLOCK
	//
	static ZeroGeometry *ParseGEOM(
		ZeroFile &file, 
		unsigned size, 
		const ZeroMeshContext *context,
		unsigned nameCRC,
		const MaterialCache& materialCache,
		ZeroMshModelTypes modelType,
		unsigned hiddenSegments,
		void *workBuffer)
	{
		int newPos;
		int filePos;
		ZeroSphere sphere; 
		int envlopsCount;
		int envlopsMap[256]; 
		ZeroGeometry *geometry;
		ZeroClothData clothData;
		ZeroGeometryData geoData;
		ZeroGeometry *dummyGeometry;

		int* nailedPoints;
		int* indexTriangleList;
		int* particlesIndexMap;
		ZeroUV* clothVertexUV;
		ZeroVector3* particles;
		ZeroBendEdge* bendPairs;
		ZeroClothVertexEdge* clothVertexEdge;
		ZeroClothCollisionVolume *collisionVolumes;

		particles = NULL;
		bendPairs = NULL;
		nailedPoints = NULL;
		clothVertexUV = NULL;
		clothVertexEdge = NULL;
		collisionVolumes = NULL;
		indexTriangleList = NULL;
		particlesIndexMap = NULL;

		envlopsCount = 0;
		dummyGeometry = new (false) ZeroMeshGeometry(InvertCRC(nameCRC));

		geoData.modelType = modelType;
		geoData.dataGeometry = dummyGeometry;
		
		// while there is size left...
		while (size) {
			// read a block header
			BlockHeader header = { 0, 0 };
			file.Read32(&header, sizeof(BlockHeader), 1);
			_ASSERTE(header.id);
			
			// deduct size
			size -= sizeof(BlockHeader);
			size -= header.size;
			
			filePos = file.Tell();
			
			// parse the block
			switch (header.id)
			{
				case 'XOBB':
				{
					// get the segment's bounding sphere
					ZeroQuaternion rotation;
					ZeroVector3 position;
					ZeroVector4 dimension;
					ParseBBOX(file, header.size, rotation, position, dimension);
					sphere = ZeroSphere(rotation, position, dimension);
					dummyGeometry->SetSphere(sphere);
					break;
				}

				case 'LVNE':
				{
					file.Read32(&envlopsCount, sizeof(unsigned), 1);
					file.Read32(envlopsMap, sizeof (int), envlopsCount);

					geoData.envelopsCount = envlopsCount;
					geoData.envelopsMap = envlopsMap;
					break;
				}

				// non-gamecube segment
				case 'MGES':
				{
					// add a segment to the geometry
					ParseSEGM(file, header.size, context, materialCache, dummyGeometry, modelType, hiddenSegments, workBuffer);
					break;
				}

				// gamecube segment
				case 'GESG':
				{
					// add a segment to the geometry
					ParseGSEG(file, header.size, context, materialCache, dummyGeometry, modelType, hiddenSegments, workBuffer);
					break;
				}

				case 'HTLC':
				{

					file.Read32(&clothData.particlesCount, sizeof(unsigned), 1);
					SetBuffer (particles, workBuffer, clothData.particlesCount);
					file.Read32(&particles[0], sizeof (ZeroVector3), clothData.particlesCount);
					clothData.particlesPosit = &particles[0];

					file.Read32(&clothData.nailedPointsCount, sizeof(unsigned), 1);
					SetBuffer (nailedPoints, workBuffer, clothData.nailedPointsCount);
					file.Read32(&nailedPoints[0], sizeof (float), clothData.nailedPointsCount);
					clothData.nailedPoints = &nailedPoints[0];

					file.Read32(&clothData.indexTriangleCount, sizeof(unsigned), 1);
					SetBuffer (indexTriangleList, workBuffer, clothData.indexTriangleCount * 3);
					file.Read32(&indexTriangleList[0], 3 * sizeof (int), clothData.indexTriangleCount);
					clothData.indexTriangleList = &indexTriangleList[0];

					file.Read32(&clothData.particlesIndexMapCount, sizeof(unsigned), 1);
					SetBuffer (particlesIndexMap, workBuffer, clothData.particlesIndexMapCount);
					file.Read32(&particlesIndexMap[0], sizeof (int), clothData.particlesIndexMapCount);
					clothData.particlesIndexMap = &particlesIndexMap[0];

					file.Read32(&clothData.particlesIndexMapCount, sizeof(unsigned), 1);
					SetBuffer (clothVertexUV, workBuffer, clothData.particlesIndexMapCount);
					file.Read32(&clothVertexUV[0], sizeof (ZeroUV), clothData.particlesIndexMapCount);
					clothData.geometryUV = &clothVertexUV[0];

					file.Read32(&clothData.edgeCount, sizeof(unsigned), 1);
					SetBuffer (clothVertexEdge, workBuffer, clothData.edgeCount);
					file.Read32(&clothVertexEdge[0], sizeof (ZeroClothVertexEdge), clothData.edgeCount);
					clothData.clothVertexEdge = &clothVertexEdge[0];

					file.Read32(&clothData.bendPairsCount, sizeof(unsigned), 1);
					SetBuffer (bendPairs, workBuffer, clothData.edgeCount);
					file.Read32(&bendPairs[0], sizeof (ZeroBendEdge), clothData.bendPairsCount);
					clothData.bendPairs = &bendPairs[0];

					file.Read32(&clothData.collisionCount, sizeof(unsigned), 1);
					if (clothData.collisionCount) {
						SetBuffer (collisionVolumes, workBuffer, clothData.collisionCount);
						file.Read32(&collisionVolumes[0], sizeof (ZeroClothCollisionVolume), clothData.collisionCount);
						clothData.collisionVolumes = &collisionVolumes[0];
					}

					geoData.clothData = &clothData;


					break;
				}
				
				default:
				{
					// skip unrecognized block
					ParseNULL(file, header.size);
					break;
				}
			}
			
			newPos = file.Tell();
			_ASSERTE (((newPos - filePos) == (int)header.size));
		}

		if (geoData.modelType == Cloth_Node) {
			if (clothData.particlesCount == 0) {
				geoData.modelType = Static_Node;
			}
		}

		geometry = context->GetGeometry (geoData);
		geometry->SetSphere(sphere);

		dummyGeometry->Release();
		return geometry;
	}


	
	//
	// PARSE A MODEL BLOCK
	//
	static void ParseMODL(
		ZeroFile &file, 
		unsigned size,  
		const ZeroMeshContext *context,
  		const MaterialCache& materialCache,
		ZeroObject *root,
		void *workBuffer)
	{
      int newPos;
		int filePos;
		unsigned objectFlags;
		unsigned objectNameCRC;
		unsigned parentNameCRC;

		ZeroObject *object;
		ZeroObject *parent;
		ZeroGeometry *geometry;
		ZeroMshModelTypes modelType;


      object = NULL;
      geometry = NULL;
      objectNameCRC = 0;
      parentNameCRC = 0;
      objectFlags = 0;

      modelType = Null_Node;
	  ZeroQuaternion objectRotation(1, 0, 0, 0);
	  ZeroVector3 objectPosition(0, 0, 0);

		
		// while there is size left...
		while (size) {
			// read a block header
			BlockHeader header = { 0, 0 };
			file.Read32(&header.id, sizeof(BlockHeader), 1);
			_ASSERTE(header.id);
			
			// deduct size
			size -= sizeof(BlockHeader);
			size -= header.size;
			
			
			filePos = file.Tell();
			
			// parse the block
			switch (header.id) {
				case 'PYTM':
            {
            		// note ZeroMshModelTypes if forced to a dword
					file.Read32(&modelType, sizeof(ZeroMshModelTypes), 1);
					break;
            }
				
				case 'EMAN':
				{
					// get the mesh name
					ParseNAME(file, header.size, objectNameCRC);
					break;
				}
				
				case 'NRAP':
				{
					// get the mesh parent
					parentNameCRC = ParsePARN(file, header.size);
					break;
				}
				
				
				case 'TNRP':
				{
					// parent name
					ParseNAME(file, header.size, parentNameCRC);
					break;
				}
				
				
				case 'NART':
				{
					// get the mesh matrix
					ParseTRAN(file, header.size, objectRotation, objectPosition);
					break;
				}
				
				case 'SGLF':
            {
					file.Read32(&objectFlags, sizeof(unsigned), 1);
					break;
            }
				
				case 'MOEG':
				{
					geometry = ParseGEOM(file, header.size, context, objectNameCRC, materialCache, modelType, objectFlags, workBuffer); 
					break;
				}
				
				case 'XDNM':
				{
					// skip matrix palette index
					ParseNULL(file, header.size);
					break;
				}
				
				
				default:
				{
//					_ASSERTE (0);
					// skip unrecognized block
					ParseNULL(file, header.size);
					break;
				}
			}
			
         newPos = file.Tell();
         _ASSERTE (((newPos - filePos) == (int)header.size));
		}
		
		
      switch (modelType) {
			case Envelop_Node:
			{
				if (!geometry) {
					ZeroGeometryData data;
					data.modelType = modelType;
					geometry = context->GetGeometry (data);
				} else if (!geometry->IsTypeID (ZeroTransformContraintGeometry::GetRttiTypeID())) {
					ZeroGeometryData data;
					data.modelType = modelType;
					data.dataGeometry = geometry;
					geometry = context->GetGeometry (data);
				}
				break;
			}

			default:
			{
				break;
			}
			
      }
		
		
		ZeroObjectData data;
		data.modelType = modelType;
		data.nameCRC = objectNameCRC;
		object = context->GetObject (data);
      _ASSERTE (object);
		
      object->SetNameCRC (objectNameCRC);
	  object->SetObjectRotation(objectRotation);
	  object->SetObjectPosition(objectPosition);
		
      if (geometry) {
			object->AttachGeometry (geometry);
			geometry->Release();
      }
		
//		attach to hierachy
		parent = root->Find (parentNameCRC);
      _ASSERTE (parent);
		
      object->Attach (parent);
      object->Release();
	}
	
	
	
	//
	// PARSE AN OBJECT HIERARCHY FILE (VERSION 1)
	//
	static void ParseMSH(
		ZeroFile &file, 
		unsigned size,
		ZeroObject *root,
		const ZeroMeshContext *context)
	{
		unsigned sceneNameCRC;
		MaterialCache materialCache;

		sceneNameCRC = 0;

		ZeroStack<char> workBuffer(WORK_BUFFER_NAX_SIZE);

		#ifdef _DEBUG
		baseAddress = 0;
		leadingAddress = 0;
		#endif


		// while there is size left...
		while (size) {
			// read a block header
			BlockHeader header = { 0, 0 };
			file.Read32(&header.id, sizeof(BlockHeader), 1);
			
			_ASSERTE(header.id);
			
			
			// deduct size
			size -= sizeof(BlockHeader);
			size -= header.size;
			
			// parse the block
			switch (header.id) {
				case 'FNIS':
				{
					// parse a scene info block
					sceneNameCRC = ParseSINF(file, header.size, root);
					break;
				}
					
				case 'LTAM':
				{
					ParseMATL(file, header.size, context, materialCache);
					break;
				}
					
				case 'TESM':	// [COMPATIBILITY]
				{
					_ASSERTE (0);
					// parse a material list
					//					ParseMSET(file, header.size, context, materialCache);
					ParseNULL(file, header.size);
					break;
				}
					
					
				case 'LDOM':
				{
					// parse a model block
					ParseMODL(file, header.size, context, materialCache, root, &workBuffer[0]);
					break;
				}
					
					
				case 'PTGL':
				{
					// parse a point light
					ParseLGTP(file, header.size, context, root);
					break;
				}
					
				case 'ITGL':
				{
					// parse an infinite light
					ParseLGTI(file, header.size, context, root);
					break;
				}
					
				case 'STGL':
				{
					// parse a spot light
					ParseLGTS(file, header.size, context, root);
					break;
				}
					
					
				case 'HTLC':
				{
					// parse a cloth block
					_ASSERTE (0);
					//					ParseCLTH(file, header.size);
					ParseNULL(file, header.size);
					break;
				}
					
					
				case 'RMAC':  // parse a camera block
				{
					
					//ParseCAMR(file, header.size);
					ParseNULL(file, header.size);
					break;
				}
					
					
					// remenber to delete this block
				case 'FNIF': // file info data
				case 'IBMA': // ambience data
				case 'DGOF': // fog data
				{
					ParseNULL(file, header.size);
					break;
				}
					
				default:
				{
					_ASSERTE (0);
					// skip unrecognized block
					ParseNULL(file, header.size);
				}
			}
      }
		  
      root->SetNameCRC (sceneNameCRC);
	}
	

	
	//
	// LOAD A MESH FROM AN OBJECT HIERARCHY FILE (VERSION 1)
	//
	ZeroObject * __cdecl Load (const char *nameArg, const ZeroMeshContext *context)
	{
		mHasShadowVolumes=false;

		int size;
		ZeroEntity *root;

		ZeroFile file(nameArg, "rb");
		if (!file) {
			return NULL;
		}
		
		// create root
		root = new (false) ZeroEntity();

		// read the block header
		BlockHeader header = {0, 0};
		file.Read32(&header, sizeof(BlockHeader), 1);
		if (header.id == 'RDEH') {
			
			size = header.size;
			
			// searcj for the msh block
			while (size) {
				// read a block header
				BlockHeader header = {0, 0};
				file.Read32(&header.id, sizeof(BlockHeader), 1);
				_ASSERTE(header.id);

				// deduct size
				size -= sizeof(BlockHeader);
				size -= header.size;
				
				// parse the block
				switch (header.id)
				{
					case '2HSM':
					{
						// parse current version 
						ParseMSH(file, header.size, root, context);
						goto mshFound;
					}
					
					case 'OVHS':
					{
						int shadows;
						// check to see if this model has shadow volumes
						file.Read32(&shadows, sizeof(shadows), 1);
						if(shadows)
							mHasShadowVolumes=true;
					}
					break;
					
					default:
					{
						ParseNULL(file, header.size);
						break;
						
					}
				}
			}
			
		} else if (header.id == '2HSM') {
			ParseMSH(file, header.size, root, context);
		} else {
			_ASSERTE (0);
		}
		
		mshFound:;

		if (!root->GetChild()) {
			root->Release();
			root = NULL;
			ZeroTrace ((0u, "msh file unknow version"));
		} 
		
		
		// root->DebugPrint ("xxx0.txt");
		if (root) {
			ZeroObject *obj;
			for (obj = root->GetFirst(); obj; obj = obj->GetNext()) {
				ZeroGeometry *geometry;
				geometry = obj->GetGeometry();
				if (geometry) {
					if (geometry->IsTypeID(ZeroSkinGeometry::GetRttiTypeID())) {
						ZeroSkinGeometry *skinGeo;
						skinGeo = (ZeroSkinGeometry *)geometry;
						skinGeo->RemapEnvelops(obj); 
					}

					if (geometry->IsTypeID(ZeroClothGeometry::GetRttiTypeID())) {
						ZeroClothGeometry *clothGeo;
						clothGeo	= (ZeroClothGeometry *)geometry;
						clothGeo->RemapCollisions(obj);
					}
				}
			}
		}
		
		if (root) {
			// does context really need to be const?
			// i dont see why the loader shouldnt be able to modify the context
			ZeroMeshContext *hackContext = (ZeroMeshContext *)context;
			hackContext->ParseExtraBlocks(file, size, root);
		}

		return root;
	}
}
