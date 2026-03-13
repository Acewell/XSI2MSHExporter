#ifndef MSHBLOCK_MODEL_H
#define MSHBLOCK_MODEL_H

#include "MSHBlock.h"
#include "Zero.h"
#include "MSH_ClothConfig.h"
#include "ZeroData.h"

struct vect3
{
	float fX;
	float fY;
	float fZ;
};

struct vect4
{
	float fX;
	float fY;
	float fZ;
	float fW;
};



#pragma warning (push)
#pragma warning (disable: 4201)
struct rgba
{
	union {
		unsigned val;
		struct {
        	unsigned char cR;
        	unsigned char cG;
        	unsigned char cB;
        	unsigned char cA;
		};
	};
};
#pragma warning (pop)

struct uv
{
	float fU;
	float fV;
};


#define MAX_WEIGHT_COUNT 8
struct MSH_FLOAT_VERTEX
{
	float x;
	float y;
	float z;
	float nx;
	float ny;
	float nz;
	uv uvs[MAX_TEXTURES_PER_MSH_MATERIAL];
	float weight[MAX_TEXTURES_PER_MSH_MATERIAL];
};

struct MSH_FLATVERTEX
{
	MSH_FLOAT_VERTEX vertex;
	unsigned color;
	unsigned pnLtColor;
	unsigned clothVertexFlags;
	unsigned weightId[MAX_WEIGHT_COUNT];
	unsigned attibute;
	static const int MAX_USER_CLOTH = 50;
	int userClothNumbers[MAX_USER_CLOTH];
	int numUserCloth;
};


/*
CMSHBlockPolygonList

  Class for .msh file data blocks with 'NDXL' header
*/
class CMSHBlockTriangleList: public CMSHBlock
{
public:
	//	Number of polygons 
	int m_nTriangleCount;
	unsigned short *m_nTriangleIndices;
	
	CMSHBlockTriangleList (BlockHeader *pHdr,CZeroData *pData, int triangleCount, unsigned *indexList);
	~CMSHBlockTriangleList ();
	
	virtual void Read(FILE *fp);
	virtual void Write(FILE *fp);
	virtual void WriteText(FILE *fp);
	
};


class CMSHBlockTriangleStripList: public CMSHBlock
{
public:

	// for debug tristrip output info, by Mike Z
	static int maxStripLength, minStripLength, avgStripLength, numStrips;
	static char stripLengthOutput[5000];
	//	Number of polygons 
	int m_nIndexCount;
	unsigned short *m_nStripIndices;
	
	CMSHBlockTriangleStripList (BlockHeader *pHdr,CZeroData *pData, int triangleCount, unsigned *indexList);
	~CMSHBlockTriangleStripList ();
	
	virtual void Read(FILE *fp);
	virtual void Write(FILE *fp);
	virtual void WriteText(FILE *fp);
};



class MSHBLOCKPolygon: public ZeroVisList<int>
{
public:
	MSHBLOCKPolygon ()
		:ZeroVisList<int> ()
	{
	}
	
	void SetIndex(unsigned short index, unsigned short indexValue)
	{
		int i;
		while (index >= GetCount()) {
			Append (indexValue);
		}
		
		Iterator iter (*this);
		for (i = 0, iter.Begin(); i != index; iter ++, i ++); 
		(*iter) = indexValue;
	}
	
	void FlipWinding()
	{
		int i;
		ZeroStack<int> index (GetCount());
		
		Iterator iter (*this);
		
		i = 0;
		for (iter.End(); iter; iter --) {
			index[i] = *iter;
			i ++;
		}
		
		i = 0;
		for (iter.Begin(); iter; iter ++) {
			*iter = index[i];
			i ++;
		}
	}
	
};

/*
CMSHBlockPolygonList

  Class for .msh file data blocks with 'NDXL' header
*/
class CMSHBlockPolygonList: public CMSHBlock, public ZeroVisTree<MSHBLOCKPolygon, int>
{
public:
	CMSHBlockPolygonList(BlockHeader *pHdr,CZeroData *pData);
	CMSHBlockPolygonList(const CMSHBlockPolygonList& clone)
		:CMSHBlock (clone), ZeroVisTree<MSHBLOCKPolygon, int>()
	{
	}
	
	//--------------------
	//	File read/write methods
	virtual void Read(FILE *fp);
	virtual void Write(FILE *fp);
	virtual void WriteText(FILE *fp);
	
	
	int GetTriangleCount();
	
	//--------------------
	//	Set methods
	void SetNumPolys(int nPolyCount) {}
	void SetNumIndices(int polyIndex, unsigned short nIndices) {};
	void SetIndex(int polyIndex, unsigned short vertex, unsigned short indexValue);
	void AddPoly(unsigned short nIndices, unsigned short *pIndices);
	
	void FlipWinding()
	{
		Iterator iter (*this);
		for (iter.Begin(); iter; iter ++) {
			(*iter)->info.FlipWinding();
		}
	}
	
	
	// return number  of indices in array indexOut
	int TriangleList (unsigned indexOut[], const vect3 vertex[]);

	// return number  of indices in array indexOut
	int TriangleStrip (unsigned indexOut[], const vect3 vertex[]);
};




class CMSHBlockVertexWeightsList: public ZeroVisList<ZeroMatrixWeightPair>
{
public:
	CMSHBlockVertexWeightsList();
	
	void Read(FILE *fp);
	void Write(FILE *fp);
	void WriteText(FILE *fp);
};


class CMSHBlockSegmentEnvelop: public CMSHBlock, public ZeroVisTree<CMSHBlockVertexWeightsList, int>
{
	public:
	
	CMSHBlockSegmentEnvelop(BlockHeader *pHdr,CZeroData *pData);
	
	CMSHBlockSegmentEnvelop(const CMSHBlockSegmentEnvelop& envelop)
		:CMSHBlock (envelop), ZeroVisTree<CMSHBlockVertexWeightsList, int> ()
	{
	}
	
	void AddVertexWeight (unsigned vertexIndex, unsigned boneID, float weight);
	
	//--------------------
	//	File read/write methods
	virtual void Read(FILE *fp);
	virtual void Write(FILE *fp);
	virtual void WriteText(FILE *fp);
};




struct CMSHShadowVolume
{
	int edgeCount;
	int vertexCount;
	vect3	*vertexArray;
	ZeroMshShadowEdge	*edgeList;


	CMSHShadowVolume (int vertexCountArg, int edgeCountArg)
	{
		edgeCount = edgeCountArg;
		vertexCount = vertexCountArg;

		edgeList = new ZeroMshShadowEdge[edgeCountArg];
		vertexArray = new vect3[vertexCountArg];

	}

	CMSHShadowVolume (const CMSHShadowVolume& shadow)
	{
		_ASSERTE (0);
	}

	~CMSHShadowVolume ()
	{
		if (edgeList) {
			delete[] edgeList;
		}
		if (vertexArray) {
			delete[] vertexArray;
		}
	}
};


struct ClothCollisionOwners: public ZeroVisTree<unsigned, unsigned>
{
	ClothCollisionOwners()
		:ZeroVisTree<unsigned, unsigned>()
	{
	}
};



struct CMSHClothParametricCollision: public ZeroClothCollisionVolume
{
	const CMSHBlockModel* parent;
};

struct CMSHClothParametricCollisionList: public ZeroVisList<CMSHClothParametricCollision>
{
	CMSHClothParametricCollisionList ()
		:ZeroVisList<CMSHClothParametricCollision>()
	{

	}

	void WriteData(FILE *file, const void *pData, int nSize) const
	{
		fwrite(pData, nSize, 1, file);
	}


	void Write (FILE *file) const;
	void WriteText (FILE *file) const;
	void AddParametricClothCollsion (CMSHBlockModel* collisonModel, const ZeroMatrix& matrix, const CMSHBlockModel* parentModel);

};


/*
CMSHBlockModelSegment

  Class for .msh file data blocks with 'SEGM' header
*/
class CMSHBlockModelSegment: public CMSHBlock
{
	friend class ZeroImportExportNode;

	int maxFaceCount;
	int maxVetexCount;


	int userFlags;
	
protected:
	CMSHBlockModel *m_pMyModel;

	CMSHShadowVolume *m_shadow;

	//	Name of material to apply to this segment
	char *m_sMaterialName;
	bool m_HasColorBase;
	bool m_HasLightColorBase;
	
	//--------------------
	//	POSITION
	int m_nPositions;
	vect3	 *m_v3Positions;

	
	//--------------------
	//	NORMAL
	//	array of normals
	int m_nNormals;
	vect3 *m_v3Normals;
	
	
	//--------------------
	//	COLOR
	//	array of vertex colors - used when vert colors vary within a segment
	int m_nColors;
	rgba *m_rgbaColors;
	//	vertex color base value - used when vert colors are the same within a segment
	rgba m_rgbaColorBase;
	
	//--------------------
	//	UV
	//	array of UV coordinates 
	int m_nUVCoordinates[MAX_TEXTURES_PER_MSH_MATERIAL];
	uv *m_UVCoordinates[MAX_TEXTURES_PER_MSH_MATERIAL];

	//--------------------
	//	LIGHT COLOR
	//	vertex light colors
	int m_nLightColors;
	rgba *m_rgbaLightColors;
	//	vertex light color base value
	rgba m_rgbaLightColorBase;
	
	//--------------------
	//	ATTRIBUTE
	//	array of attribute values - used when vert attributes vary within a segment
	int m_nAttributes;
	unsigned int *m_pAttributes;
	//	attribute base value - used when vert attributes are the same within a segment
	unsigned int m_AttributeBase;
	
	//--------------------
	//	POLYGONS
	//	list of polygons and their indices
	mutable CMSHBlockPolygonList m_pPolygonList;
	
	//--------------------
	//	TrianglesList
	//	list of triangles and their indices
	CMSHBlockTriangleList *m_pTriangleList;


	//--------------------
	//	TrianglesStrip
	//	list of triangles and their indices
	CMSHBlockTriangleStripList *m_pTriangleStrips;


	CMSHBlockTriangleList *CreateNewTriangleList(int triangleCount, unsigned int *pIndices);
	CMSHBlockTriangleStripList *CreateNewTriangleStrip(int indexCount, unsigned int *pIndices);

   CMSHBlockSegmentEnvelop envelop;
	

	
	void CalculateLightColorBase();
public:
	CMSHBlockModelSegment(BlockHeader *pHdr,CZeroData *pData,CMSHBlockModel *myModel);
	CMSHBlockModelSegment(const CMSHBlockModelSegment& me, const ZeroPolyhedra& polyhedra,CMSHBlockModel *myModel);
	
	
	virtual ~CMSHBlockModelSegment();
	
	//--------------------
	//	File read/write methods
	virtual void Read(FILE *file);
	virtual void Write(FILE *file);
	virtual void WriteText(FILE *fp);
	
	//--------------------
	//	Get methods
	char *GetMaterialName() const;
	vect3 *GetPositions(int &nPos) const;
	vect3 *GetNormals(int &nNorms) const;
	rgba *GetColors(int &nColors) const;
	rgba *GetLightColors(int &nColors) const;
	unsigned int *GetAttributes(int &nAttributes) const;
	uv *GetUVCoords(int &nUV,int idx=0) const;
	CMSHBlockPolygonList *GetPolygonList() const;
	CMSHBlockTriangleList *GetTriangleList() const;
	CMSHBlockTriangleStripList *GetTriangleStrip() const;
	CMSHBlockModel *GetMyModel(){return m_pMyModel;};
	CMSHShadowVolume *GetShadowVolume(){return m_shadow;};

	
	//	bool HasBBox();
	//	void GetBBoxRotation(float &fX,float &fY, float &fZ, float &fW);
	//	void GetBBoxTranslation(float &fX,float &fY, float &fZ);
	//	void GetBBoxDimension(float &fX,float &fY, float &fZ, float &fW);
	bool HasColorBase();
	void GetColorBase(unsigned char &cR,unsigned char &cG,unsigned char &cB,unsigned char &cA);
	bool HasLightColorBase();
	void GetLightColorBase(unsigned char &cR,unsigned char &cG,unsigned char &cB,unsigned char &cA);
	unsigned int GetAttributeBase();
	
	
	//--------------------
	//	Set methods
	void ConvertLongToRGBA(unsigned int val, rgba *pCol);
	unsigned int ConvertRGBAToLong(rgba *pCol);
	void SetMaterialName(const char *sMaterialName);
	void SetNumPositions(int nPositions);
	void SetPosition(int index, float fX, float fY, float fZ);
	void SetNumNormals(int nNormals);
	void SetNormal(int index, float fX, float fY, float fZ);
	void SetNumColors(int nColors);
	void SetColor(int index, unsigned int col);
	void SetColorBase(unsigned char cR, unsigned char cG, unsigned char cB, unsigned char cA);
	void SetNumLightColors(int nColors);
	void SetLightColor(int index, unsigned char cR, unsigned char cG, unsigned char cB, unsigned char cA);
	void SetLightColorBase(unsigned char cR, unsigned char cG, unsigned char cB, unsigned char cA);
	void SetNumUVs(int nUVs,int idx=0);
	void SetUV(int index, float fU, float fV,int idx=0);
	void SetNumAttributes(int nAttributes);
	void SetAttribute(int index, unsigned int Attribute);
	void SetAttributeBase(unsigned int val);

	CMSHShadowVolume *CreateEmptyShadowVolume(int vertCount,int edgeCount);
	void SetUserFlags (int flags)
	{
		userFlags = flags;
	}

	int GetUserFlags () const
	{
		return userFlags;
	}


	void SetParent (CMSHBlockModel *parent)
	{
		m_pMyModel = parent;
	}


	void CreateClothVertexList(); 
	void CreateVertexIndexStripsAndIndexList ();




	
	void AddVertexWeight (unsigned vertexIndex, unsigned boneID, float weight);
	CMSHBlockSegmentEnvelop& GetVertexWeightsEnvelop()
	{
		return envelop;
	}


	void	CreateShadowVolume();
	int CopyFlatVertex (MSH_FLATVERTEX array[]);

	void SetVertexData (ZeroPolyhedra& mesh, const MSH_FLATVERTEX array[], 
							  bool hasNormal,	bool hasColor,	bool hasLight,	bool hasWeight, bool hasUVs[]);

	
	void SeparateFaces(int color, ZeroPolyhedra& polyhedra, ZeroEdge *start, ZeroPolyhedra& output,
							 int* vertexMark, int& vertexCountOut, int& faceCountOut);
	
	void Splite(CMSHBlockModelSegment** segA, CMSHBlockModelSegment** segB);
	void Splite(ZeroVisList<CMSHBlockModelSegment*>& segmentListOut);
	void Merge(CMSHBlockModelSegment* aInSegment);

	CZeroData *GetData()
	{
		return m_pData;
	}
};




//CMSHBlockModel
//  Class for .msh file data blocks with 'MODL' header
  //class CMSHBlockConstraintList;
class CMSHBlockModel: public CMSHBlock
{
  protected:
	  //	char *m_sPrefix;
	char *m_sName;
	
	bool isHidden;
	bool isDynamicallyLit;
	bool isRetainNormals;
	bool isRenderAfterShadows;
	bool isLeaveChildGeometry;
	bool isPS2Optimize;
	ZeroMshModelTypes modelType;

/*
	unsigned constraintParent;
*/
	
	int matrixPaletteIndex;
	
	//	char *m_sParentPrefix;
	char *m_sParentName;
	
	//--------------------
	//	TRANSFORMATION MATRIX
	float m_fScaleX, m_fScaleY, m_fScaleZ;
	float m_fTransX, m_fTransY, m_fTransZ;
	float m_fQuatRotX, m_fQuatRotY, m_fQuatRotZ, m_fQuatRotW;
	

	int m_envelopsCount;
	int* m_envelopArray;
	
	//	Model Segments
	int m_nModelSegments;
	CMSHBlockModelSegment **m_ppModelSegments;
	
	
	//--------------------
	//	cloth info
	MSH_ClothConfig m_ClothData;

	int m_nClothFixedPoints;
	int *m_pClothFixedIndices;
	char **m_FixedPointBoneName;
	int m_nUserCloth;
	int *m_pUserClothFirst;
	int *m_pUserClothSecond;

	int m_ClothParticlesCount;
	vect3	 *m_ClothParticles;

	int m_TriangleCount;
	int* m_ClothTriagleList;

	int  m_ClothParticleToVertexIndexMapCount;
	int* m_ClothParticleToVertexIndexMap;
	uv*  m_ClothUV;
	
	int  m_StretchPairsCount;
	ZeroBendEdge* m_softVertexStretchPairs;

	int			  m_BendPairsCount;
	ZeroBendEdge*  m_softVertexBendPairs;

	int			  m_CrossPairsCount;
	ZeroBendEdge*  m_softVertexCrossPairs;

  
	bool m_bClothEnabled;
	//--------------------
	//	cloth collision info
	ZeroVisList<ClothCollision> m_clothCollisionVolumes;


/************** Old Colth Collision Structures ***************/
	CMSHClothParametricCollisionList	clothCollisionList;

	MSH_ClothCollisionConfig m_CollData;
	MSH_ClothCollisionSphere m_CollSphereData;
	MSH_ClothCollisionCylinder m_CollCylinderData;
	MSH_ClothCollisionCone m_CollConeData;
	ClothCollisionOwners clothCollisionOwners;

	//--------------------
	//	animation trigger collision info
	char *m_sCollTriggeredAnimation;
	MSH_AnimTriggerCollisionConfig m_AnimTriggerCollData;
	MSH_ClothCollisionSphere m_AnimTriggerCollSphereData;
	MSH_ClothCollisionCylinder m_AnimTriggerCollCylinderData;
	MSH_ClothCollisionCone m_AnimTriggerCollConeData;
	vect3 m_TorsoPosAtHit, m_TorsoFrontAtHit;
/*************************************************************/
	
	//--------------------
	//	BOUNDING BOX
	//	m_HasBBox == true  --> Bounding box has been initialized
	bool m_HasBBox;
	float m_fBBoxRX,m_fBBoxRY,m_fBBoxRZ,m_fBBoxRW;
	float m_fBBoxTX,m_fBBoxTY,m_fBBoxTZ;
	float m_fBBoxDX,m_fBBoxDY,m_fBBoxDZ,m_fBBoxDW;

	

	
	void WriteCloth (FILE *file);
	void WriteClothText (FILE *file);

	void ParseCloth(FILE *file, BlockHeader *pHdr);

	  
  public:
	  CMSHBlockModel(BlockHeader *pHdr,CZeroData *pData);
	  virtual ~CMSHBlockModel();
	  
	  //--------------------
	  //	File read/write methods
	  virtual void Read(FILE *file);
	  virtual void Write(FILE *file);
	  virtual void WriteText(FILE *fp);
	  
	  //--------------------
	  //	Get methods
	  char *GetPrefix();
	  char *GetName();
	  char *GetParentPrefix();
	  char *GetParentName();
/*
	  unsigned GetConstraint ();
*/
	  void GetScale(float &fX, float &fY, float &fZ);
	  void GetQuatRot(float &fX, float &fY, float &fZ, float &fW);
	  void GetTrans(float &fX, float &fY, float &fZ);
	  
	  CMSHBlockModelSegment **GetSegments(int &nSegments);
	  void GetPreferredRotation(float &fX, float &fY, float &fZ);
	  char *GetRootName();
	  int *GetEnvelopArray(int &nMatrices);
	  
	  bool HasBBox();
	  void GetBBoxRotation(float &fX,float &fY, float &fZ, float &fW);
	  void GetBBoxTranslation(float &fX,float &fY, float &fZ);
	  void GetBBoxDimension(float &fX,float &fY, float &fZ, float &fW);
	  MSH_ClothConfig *GetClothData();
	  int *GetClothFixedPoints(int &nIndices);

	  void GetClothCollision(MSH_ClothCollisionSphere **collSphere,MSH_ClothCollisionCylinder **collCylinder,MSH_ClothCollisionCone **collCone);
	  void GetAnimTriggerCollision(MSH_ClothCollisionSphere **collSphere,MSH_ClothCollisionCylinder **collCylinder,MSH_ClothCollisionCone **collCone);
	  char *GetCollTriggeredAnimation();
	  int GetAnimTriggerLateStart();
	  float GetAnimTriggerTime();
	  int GetAnimTriggerHitFrameMin();
	  int GetAnimTriggerHitFrameMax();
	  float GetAnimTriggerToleranceMin();
	  float GetAnimTriggerToleranceMax();
	  float GetAnimTriggerRecoveryTime();


	  
	  //--------------------
	  //	Set methods
	  void SetPrefix(char *sPrefix);
	  void SetName(char *sName);

/*
	  void SetConstraint	(char* name);
*/
	  void SetParentPrefix(char *sParentPrefix);
	  void SetParentName(char *sParentName);
	  void SetScale(float fX, float fY, float fZ);
	  void SetRotation(float fX, float fY, float fZ, float fW);
	  void SetTranslation(float fX, float fY, float fZ);
	  void AddSegment(CMSHBlockModelSegment *segment);
	  CMSHBlockModelSegment *CreateSegment();
	  CMSHBlockModelSegment *CreateAddSegment();
	  void RemoveSegment(CMSHBlockModelSegment *pSegment);
	  
	  void SetBBoxRotation(float fX,float fY, float fZ, float fW);
	  void SetBBoxTranslation(float fX,float fY, float fZ);
	  void SetBBoxDimension(float fX,float fY, float fZ, float fW);
	  void SetClothData(const MSH_ClothConfig &newData);
	  void SetClothFixedPoints(int nIndices, const int pIndices[]);	
	  void SetUserCloth(int nIndices, const int pIndicesFirst[], const int pIndicesSecond[]);	
	  void CreateClothVertexList(const CMSHBlockModelSegment& segment); 
	  void AddClothCollisionVolume(const ClothCollision &volume)  {	m_clothCollisionVolumes.Append(volume); }

	  int IsFixed(int index);
	  char* GetBoneNameFromPaletteIndex(int index);
	  void SetFixedPointsWeights();

	  void SetCollisionSphere();
	  void SetCollisionCylinder();
	  void SetCollisionCone();

	  void AddClothCollisionOwner (const char *name)
	  {
		  const char *ptr;
		  unsigned crc;

		  ptr = strrchr (name, '.');
		  if (ptr) {
			  ptr ++;
		  } else {
			  ptr = name;
		  }

		  crc = CalcStrCRC(ptr);
		  clothCollisionOwners.Insert (crc, crc);
	  }


	  bool IsCollsionOwmer (const CMSHBlockModel *owner) const
	  {
  		  unsigned crc;
		  crc = CalcStrCRC(owner->m_sName);
		  return clothCollisionOwners.Find (crc) ? true : false;
	  }


	  void SetAnimTriggerCollisionSphere(MSH_ClothCollisionSphere *collSphere, char *sName, int iLateStart, float fTime, int frameMin, int frameMax, float toleranceMin, float toleranceMax, float recoveryTime, vect3 torsoPos, vect3 torsoFront);
	  void SetAnimTriggerCollisionCylinder(MSH_ClothCollisionCylinder *collCylinder, char *sName, int iLateStart, float fTime, int frameMin, int frameMax, float toleranceMin, float toleranceMax, float recoveryTime, vect3 torsoPos, vect3 torsoFront);
	  void SetAnimTriggerCollisionCone(MSH_ClothCollisionCone *collCone, char *sName, int iLateStart, float fTime, int frameMin, int frameMax, float toleranceMin, float toleranceMax, float recoveryTime, vect3 torsoPos, vect3 torsoFront);
	  void RotateTriggerBox(ZeroMatrix &matrix,ZeroVector3 startPos);
	  void LeftToRightTriggerBox();


	  void AddParametricClothCollsion (CMSHBlockModel* collisonModel, const ZeroMatrix& matrix, const CMSHBlockModel* parentModel)
	  {
		  clothCollisionList.AddParametricClothCollsion (collisonModel, matrix, parentModel);
	  }

	  void RemoveGeometry();
	  
	  virtual void WriteAnimTriggerCollisionObject(FILE *file);
	  
	  void SetModelType (ZeroMshModelTypes type)
	  {
		  modelType = type;
	  }
	  
	  ZeroMshModelTypes GetModelType ()
	  {
		  return modelType;
	  }
	  
	  bool IsAnimTriggerCollision ()
	  {
		  return m_AnimTriggerCollData.config.bCollisionEnabled ? true: false;
	  }

	  
	  bool GetPS2Optimize () const
	  {
		  return isPS2Optimize;
	  }
	  
	  void SetPS2Optimize (bool flag)
	  {
		  isPS2Optimize = flag;
	  }
	  
	  bool GetDynamicallyLit () const
	  {
		  return isDynamicallyLit;
	  }
	  
	  void SetDynamicallyLit (bool flag)
	  {
		  isDynamicallyLit = flag;
	  }
	  
	  bool GetLeaveChildGeometry () const
	  {
		  return isLeaveChildGeometry;
	  }
	  
	  void SetLeaveChildGeometry (bool flag)
	  {
		  isLeaveChildGeometry = flag;
	  }
	  
	  bool GetRetainNormals () const
	  {
		  return isRetainNormals;
	  }
	  
	  void SetRetainNormals (bool flag)
	  {
		  isRetainNormals = flag;
	  }
	  
	  bool GetHidden () const
	  {
		  return isHidden;
	  }
	  
	  void SetHidden (bool flag)
	  {
		  isHidden = flag;
	  }
	  
	  bool GetRenderAfterShadows () const
	  {
		  return isRenderAfterShadows;
	  }
	  
	  void SetRenderAfterShadows (bool flag)
	  {
		  isRenderAfterShadows = flag;
	  }
	  
	  void SetMatrixPaletteIndex (int index)
	  {
		  matrixPaletteIndex = index;
	  }

	  void SetEnvelopArray(int nIndices, const int pIndices[]) ;	

	  
	  int GetMatrixPaletteIndex () const
	  {
		  return matrixPaletteIndex;
	  }

	  void EnableCloth()
	  {
		m_bClothEnabled = true;
	  }

	  bool IsClothEnabled() const
	  {
		return m_bClothEnabled;
	  }
	  
	  void GetArrayWeightModels (CMSHBlockModel **models);
	  
	  void SpliteSegments();

	  enum SWCI_CollisionType
	  {
		PST_SPHERE,
		PST_ELLIPSOID,
		PST_CYLINDER,
		PST_MESH,
		PST_BOX,
	  };

  protected:

	  bool m_bIsSWCICollisionObject;
	  SWCI_CollisionType m_SWCICollisionType;
	  float m_fSWCI_Radius;
	  float m_fSWCI_Height;
	  float m_fSWCI_Length;

  public:

	  void SetSWCICollisionObject(bool bCollision)
	  {
	      m_bIsSWCICollisionObject = bCollision;
	  }
	  bool GetSWCICollisionObject(void) const
	  {
		  return m_bIsSWCICollisionObject;
	  }
	  void SetSWCICollisionType(SWCI_CollisionType coltype)
	  {
		  m_SWCICollisionType = coltype;
	  }
	  SWCI_CollisionType GetSWCICollisionType(void) const
	  {
		  return m_SWCICollisionType;
	  }
	  void SetSWCIRadius(float fRadius)
	  {
		  m_fSWCI_Radius = fRadius;
	  }
	  float GetSWCIRadius(void) const
	  {
		  return m_fSWCI_Radius;
	  }
	  void SetSWCIHeight(float fHeight)
	  {
		  m_fSWCI_Height = fHeight;
	  }
	  float GetSWCIHeight(void) const
	  {
		  return m_fSWCI_Height;
	  }
	  void SetSWCILength(float fLength)
	  {
		  m_fSWCI_Length = fLength;
	  }
	  float GetSWCILength(void) const
	  {
		  return m_fSWCI_Length;
	  }
	  
};
#endif