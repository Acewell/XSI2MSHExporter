#ifndef _SCENE2MSH_H
#define _SCENE2MSH_H

#include "MSH_ClothConfig.h"
#include "ZeroImportExportUserData.h"
#include "MSHBlockModel.h"
class CXSI2MSHProgressDialog;
class CZeroData;
class CMSHBlockConstraintList;
class CMSHBlockMaterial;
class CMSHBlockMaterialSet;
class CMSHBlockLight;

class ZeroSegment;
class ZeroSphere;
class ZeroImportExportNode;
class CScene2MSH;


const CLOTH_FIX_POINT = 0x01;
//const CLOTH_VERTICAL_BORDER = 0x02;
//const CLOTH_HORIZONTAL_BORDER = 0x04;



struct ObjClsAttrib
{
	unsigned objNameCRC;
	long clsIdx;
	unsigned attrib;
	unsigned txt1CRC;
	unsigned txt2CRC;
	unsigned txt3CRC;
	bool operator< (const ObjClsAttrib &key0) const
	{
		return ((objNameCRC < key0.objNameCRC) 
			|| ((objNameCRC == key0.objNameCRC) && (clsIdx < key0.clsIdx))
			|| ((objNameCRC == key0.objNameCRC) && (clsIdx == key0.clsIdx)&& (attrib < key0.attrib)) 
			|| ((objNameCRC == key0.objNameCRC) && (clsIdx == key0.clsIdx)&& (attrib == key0.attrib) && (txt1CRC < key0.txt1CRC))
			|| ((objNameCRC == key0.objNameCRC) && (clsIdx == key0.clsIdx)&& (attrib == key0.attrib) && (txt1CRC == key0.txt1CRC) && (txt2CRC < key0.txt2CRC))
			|| ((objNameCRC == key0.objNameCRC) && (clsIdx == key0.clsIdx)&& (attrib == key0.attrib) && (txt1CRC == key0.txt1CRC) && (txt2CRC == key0.txt2CRC) && (txt3CRC < key0.txt3CRC))); 
	}

	bool operator> (const ObjClsAttrib &key0) const
	{
		return ((objNameCRC > key0.objNameCRC) 
			|| ((objNameCRC == key0.objNameCRC) && (clsIdx > key0.clsIdx))
			|| ((objNameCRC == key0.objNameCRC) && (clsIdx == key0.clsIdx)&& (attrib > key0.attrib)) 
			|| ((objNameCRC == key0.objNameCRC) && (clsIdx == key0.clsIdx)&& (attrib == key0.attrib) && (txt1CRC > key0.txt1CRC))
			|| ((objNameCRC == key0.objNameCRC) && (clsIdx == key0.clsIdx)&& (attrib == key0.attrib) && (txt1CRC == key0.txt1CRC) && (txt2CRC > key0.txt2CRC))
			|| ((objNameCRC == key0.objNameCRC) && (clsIdx == key0.clsIdx)&& (attrib == key0.attrib) && (txt1CRC == key0.txt1CRC) && (txt2CRC == key0.txt2CRC) && (txt3CRC > key0.txt3CRC))
			); 
	}
};

class UniformPolygonList: public ZeroList<unsigned int>
{
public:
	int m_flatVertexCount;
	char m_materialName[256];
	
	UniformPolygonList(const char *matName)
		:ZeroList<unsigned int>(ZeroAllocator::GetGenericAllocator())
	{
		strcpy (m_materialName, matName);
		m_flatVertexCount = 0;
	}
	
	void AddPoly (unsigned int polyIndex, unsigned int numVertices)
	{
		Append (polyIndex);
		m_flatVertexCount += numVertices;
	}
	
};

class PolygonListByType: public ZeroTree<UniformPolygonList, unsigned>
{
public:
	int m_maxVertexCount;
	
	PolygonListByType ()
		:ZeroTree<UniformPolygonList, unsigned>(ZeroAllocator::GetGenericAllocator())
	{
		m_maxVertexCount = 0;
	}
	
	void AddPoly(CScene2MSH *aConverter, CZeroData *aData,ObjClsAttrib *key, unsigned int polyIndex, unsigned int numVertices);
};

/*
note: can't use (X3DObject *) to identify a model because when you QueryInterface, you might get a different pointer
*/
class Constraints: public ZeroTree<unsigned, unsigned>
{
	public:

   Constraints ()
       :ZeroTree<unsigned, unsigned>(ZeroAllocator::GetGenericAllocator())
	{
   }

	void AddConstraint (X3DObject *model);
   void AddAllConstraints (X3DObject *pObject);
   bool IsConstraint (unsigned modelCRC)
   {
      return Find (modelCRC) ? true : false; 
   }
};

class ModelByCRC: public ZeroTree<X3DObject *,unsigned>
{
public:
	X3DObject *mTorsoModel;
    void AddModel(X3DObject *model);

    ModelByCRC ()
        :ZeroTree<X3DObject *, unsigned>(ZeroAllocator::GetGenericAllocator())
    {
		mTorsoModel=NULL;
    }
    void AddAllModels (X3DObject *pObject)
    {
		X3DObjectCollection *pChildren=NULL;
		AddModel(pObject);

		pObject->get_Children(&pChildren);
		if(pChildren)
		{
			VARIANT varIdx;
			varIdx.vt=VT_I4;
			X3DObject *pChild=NULL;
			long nChildren=0;
			pChildren->get_Count(&nChildren);
			for(long idx=0;idx<nChildren;idx++)
			{
				varIdx.lVal = idx;
				pChildren->get_Item(varIdx,&pChild);
				AddAllModels(pChild);
			}
		}
    }
};

class BoneMatrixByCRC: public ZeroTree<ZeroMatrix,unsigned>
{
public:
    BoneMatrixByCRC ()
        :ZeroTree<ZeroMatrix, unsigned>(ZeroAllocator::GetGenericAllocator())
    {
    }
};

class MaterialByNameCRC: public ZeroTree<CMSHBlockMaterial *, unsigned>
{
public:
	
	MaterialByNameCRC ()
		:ZeroTree<CMSHBlockMaterial *, unsigned>(ZeroAllocator::GetGenericAllocator())
	{
	}
};

class MaterialByObjClsAttrib: public ZeroTree<unsigned, ObjClsAttrib>
{
public:
	
	MaterialByObjClsAttrib ()
		:ZeroTree<unsigned, ObjClsAttrib>(ZeroAllocator::GetGenericAllocator())
	{
	}
};

class CScene2MSH//: public Constraints
{
    static CScene2MSH *mMe;
    CZeroData *mZeroData;
    ZeroImportExportNode *mDatabase;


protected:
	bool m_bWarningsOn;
	static OLECHAR FAR *sBSTRInit;
	MaterialByNameCRC m_MaterialByNameTree;

	MaterialByObjClsAttrib m_MaterialByObjClsAttrib;
	Constraints m_NewMaterialFromOldMaterial;
	
	CXSI2MSHProgressDialog *m_ProgDlg;
	float m_StartFrame,m_EndFrame,m_CurrentFrame,m_FrameRate;
	Application *m_App;
	ModelByCRC m_AllModels;
	ModelByCRC m_Deformers;
	BoneMatrixByCRC m_BoneMatrices;
	BoneMatrixByCRC m_OldWorldMatrices;
	BoneMatrixByCRC m_NewWorldMatrices;
	BoneMatrixByCRC m_CorrectionMatrices;
	MshPlatform m_Platform;
	int m_TextureFormat;

	int m_CurrentID;
	BOOL	mExportTextures,
			mExportAnimations,
			mCameraAnimations,
			mClipLoopedAnimations,
			mExportSelectedOnly,
			mExportLights,
			mExportSIInfo,
			mBaseposeFromCurrentFrame,
			mFilterPoseCNSAnims,
			mFlattenGeometry,
			mExportFKAnimation,
			mSkipBasepose,
			mExportLeftHanded
			;
	char *m_sDestinationPath;

	char *StripPath(char *aFilename);

	void GetTorsoPositionAndOrientation(float aFrameNumber, vect3 &aPos, vect3 &aFront);
	unsigned int GetModelCRC(X3DObject *pModel);
	bool GetBasePose(X3DObject *pModel, ZeroMatrix &matrix, VARIANT &sclX, VARIANT &sclY, VARIANT &sclZ,ZeroImportExportNode *pParentNode);
	void GetSIObjectName(IDispatch *aDisp, char *aName, int aNameSize);
	void GetSIObjectType(IDispatch *aDisp, char *aType, int aTypeSize);
	void GetPropertyByType(IDispatch *aObj, char *aPropType,Property **aProp,int idx=0);
	void GetGlobalPosition(IDispatch *aDisp,vect3 &vPos);
	void SetParameter(IDispatch *aObj, char *aParamName, float aFrameNumber, VARIANT *aValue);
	bool GetParameter(IDispatch *aObj, char *aParamName, float aFrameNumber, VARIANT *aValue);
	void DumpParameters(IDispatch *aObj, char *objName);

	void SetBaseLightInfo(CMSHBlockLight *aMSHLight, Light *aLight, OGLLight *aOGLLight);
	void CreateLight(CZeroData *pData,Light *pLight);
	void CreatePointLight(CZeroData *pData, Light *aLight, OGLLight *aOGLLight);
	void CreateSpotLight(CZeroData *pData, Light *aLight, OGLLight *aOGLLight);
	void CreateInfiniteLight(CZeroData *pData, Light *aLight, OGLLight *aOGLLight);

	void CreateZeroFileInfo(CZeroData *pData,Project *pProject);
	void CreateZeroSceneInfo(CZeroData *pData,Scene *pScene);
	void CreateZeroAmbience(CZeroData *pData,Scene *pScene);
	void CreateZeroFogData(CZeroData *pData,Scene *pScene);
	void CreateCamera(CZeroData *pData,Camera *pCamera);
	
	void ConvertTextureFile(char *srcPath, char *dstDir, CMSHBlockMaterial *pMSHMat);
	unsigned int GetMaterialTexture(X3DObject *aObject,char *aPropName, int idx);
	unsigned int GetMaterialAttribute(X3DObject *aObject,char *aClusterName);
	void AddATRBtoDefaultMaterials(X3DObject *aObject, CZeroData *aData,Material *aMat,PropertyCollection *aProperties,ObjClsAttrib *aKey);
	void AddTextureToMaterial(IDispatch *aDisp,CMSHBlockMaterial *aMat,Application *pApp);
	void AddTexturesToMaterial(IDispatch *aDisp,CMSHBlockMaterial *aMat,Application *pApp);
	void GetUniqueMaterialName(const char *aName,char *aUniqueName,const int nameSize);
	void AddMaterialNameToCRCList(char *sName,CMSHBlockMaterial *pMat);
	void CreateMSHMaterialFromXSIMaterial(CMSHBlockMaterialSet *aMSHMatSet, CMSHBlockMaterial *aMSHMaterial, Material *aMaterial);
	void RemoveRedundantMaterials();
	void AddMaterialsFromObject(CZeroData *aData, X3DObject *aObject,bool branchSelected);
	void CreateMaterials(CZeroData *pData,Scene *pScene);

	void CreateSkinBoundingBoxes(CZeroData *pData);

	void AlignBonesChainRoot(ZeroImportExportNode *node);
	void CreateAnimation(CMSHBlockEndEffector *effector);
	ZeroVector4 GetEulerFromPitchYawRollMatrix(const ZeroMatrix& matrix);
	void CreateDoubleIKAnimation(ZeroImportExportNode* node);
	void CreateSingleIKAnimation(ZeroImportExportNode* node);
	void CreateFreeJointAnimation(ZeroImportExportNode* node);
	void CreateAnimations();

	void SetClothData(CMSHBlockModel *aMSHModel,X3DObject *aModel);
	bool DetermineCollisionVolume(const char *name, X3DObject *pModel, ClothCollision &volume);
	void SetClothCollisionData(CMSHBlockModel *aMSHModel,X3DObject *aModel, Scene *aScene);
	void deprecatedSetClothCollisionData(CMSHBlockModel *aMSHModel,X3DObject *aModel);

	void SetObjectFlags(CMSHBlockModel *aMSHModel,X3DObject *aModel);
	float SolveConstraint (X3DObject *effector);
	bool IsMeshAnimated(X3DObject *pModel);
	void CreateSkeleton (ZeroImportExportNode *node);
	void CreateIKChain (ZeroImportExportNode *node);
	void WriteDataToMSHSegment(CMSHBlockModelSegment *pMSHSegment, UniformPolygonList& segmentPolyList,MSH_FLATVERTEX *flatVertexArray,PolygonFaceCollection *polys, int iDoUVs, bool bDoColors, bool bDoWeights,unsigned modelNameCRC, CMSHBlockModel *aMSHModel, CZeroData *pData);
	unsigned int GetNumVertices(PolygonFaceCollection *aPolys, VARIANT &varElem);

	void GetPolygonData(FacetCollection *aFacets, PolygonFaceCollection *aPolys, TriangleCollection *aTris, VARIANT &varElem, MSH_FLATVERTEX **pVertex, ClusterElementCollection *aUV0s,ClusterElementCollection *aUV1s,long *aPolyTriStart, bool &aDoColors, X3DObjectCollection *aDeformers, VARIANT *aWeights, ClusterElementCollection *aClothFixPointsCluster, 	ClusterElementCollection **aUserClothSprings, int numUserClothSprings, bool *aShowWeightsWarning);


	void SortPolygonsByMaterial(long nClusters, ClusterCollection *pClusters, PolygonFaceCollection *polys, PolygonListByType &polyLists, CZeroData *aData, X3DObject *aModel,ObjClsAttrib *aKey);
	int GetAnimationStartFrame(Scene *aScene,char *animName);
	void SetAnimTriggerCollision(CMSHBlockModel *aMSHModel,X3DObject *aModel,Scene *aScene);
	void CreateGeometry(Geometry *aGeometry,CZeroData *aData, CMSHBlockModel *aMSHModel, X3DObject *aModel,Scene *aScene,ObjClsAttrib *aKey);
	ZeroImportExportNode *CreateModel(CZeroData *pData, X3DObject *pModel, Geometry *pGeometry, X3DObject *pParent,ZeroImportExportNode *modelParentDataBase,Scene *aScene);
	void CreateHierarchy (CZeroData *pData, X3DObject *pModel,X3DObject *pParent,int level,ZeroImportExportNode* database,Scene *aScene,bool branchSelected);

	void CreateModels(CZeroData *pData, Scene *pScene, char *pathFiles);
	void CreateSkeletonAndAnimations(Scene *pScene);
	void RecursiveTransform(ZeroImportExportNode *node,ZeroMatrix *m,ZeroVector4 *v);
	void CorrectGlobalRotationForSkeleton(ZeroImportExportNode *node,ZeroMatrix oldWorldMatrix,ZeroMatrix newWorldMatrix,bool bForceRotation);


	void SetPoseConstraint (X3DObject *model, CMSHBlockModel *aMSHModel);



    public:
	CScene2MSH();
	virtual ~CScene2MSH();

	void SetDestinationPath(const char *aDstPath);
	void SetTextureFormat(int format);
	void SetPlatform(int platform);
	void SetWarningsOn(bool warnings);
	void SetFilters(BOOL aExportSelected, BOOL aExportTextures, BOOL aExportAnimations, BOOL aExportLights, BOOL aExportSIInfo,  BOOL aClipLoopedAnims, BOOL aBaseposeFromCurrentFrame, BOOL aFlattenGeometry, BOOL aCameraAnims, BOOL aExportFKAnimations, BOOL aSkipBasepose, BOOL aExportLeftHanded);
	void CreateZeroStructure(CZeroData *pData, Project *pProject, Scene *pScene, char *pathFiles,CXSI2MSHProgressDialog *progDlg);
	const char *GetNewMaterialName(CZeroData *pData, ObjClsAttrib *aKey);
};
#endif
