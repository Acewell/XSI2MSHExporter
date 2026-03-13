#ifndef _ZEROIMPORTEXPORTNODE_
#define _ZEROIMPORTEXPORTNODE_


#include "Zero.h"
#include "ZeroData.h"
#include "MSH_ClothConfig.h"

class CZeroData;
class CMSHBlockModel;
class CMSHBlockModelSegment;

struct MSH_FLATVERTEX;


enum LIGHTING_MODE 
{
	NO_LIGHTS,
	FAST_LIGHT,				// apply ligght model to vertexx
	FULL_LIGHT,				// calculate vertex color by ray casting to a collision geometry
	FULL_LIGHT_MAPS,	   // calculate light maps textures ray casting to a collision geometry
};

class ZeroImportExportNode: public ZeroNode<ZeroImportExportNode>
{
	bool bDeformer;
    void *cgcModel;
    CMSHBlockModel* mshModel;
	 mutable int mark;	


	void SetCGCModelToMatrixPaletteIndex();
	 void SplitSegments();

	 void ApplyLightsMaps ();
	 void ApplyFullLights ();
	 void ApplyFastLights ();

	 void ApplyLights (LIGHTING_MODE mode = NO_LIGHTS);

    ZeroMatrix GetWorldScale() const;
	
	 void MarkDeformers(ZeroImportExportNode *root,CZeroData *aData);
    void MarkAllDeformers(ZeroImportExportNode *root,CZeroData *aData);

	void SortSkeleton(CMSHBlockSkeleton *skeleton, const CMSHBlockModel* const modelList[], int modelsCount);
	 void SortAnimation(CMSHBlockAnimation *animation, const CMSHBlockModel* const modelList[], int modelsCount);
    void SortSkinNodes(CMSHBlockModel **outList, int &index, ZeroHeap <ZeroImportExportNode*, LONGLONG>& sortModels);
	 void RemapSkinMatrices1(ZeroImportExportNode *root);
	 void RemapSkinMatrices2(ZeroImportExportNode *root);
    void RemapAllSkinMatrices1(ZeroImportExportNode *root);
	 void RemapAllSkinMatrices2(ZeroImportExportNode *root);
	 void EnumerateEnvelopMatrices (int& index);
	 void CreateEnvelopArrays ();
	 void CreateAllEnvelopArrays ();

	 void ExtractNonSkinPolygonsFromSkin();
	 void ExtractNonSkinPolygonsFromSkin(CMSHBlockModelSegment *skinSegment);
	 void ExtractNonSkinSegment(ZeroImportExportNode *skinNode, CMSHBlockModelSegment *skinSegment, ZeroPolyhedra& skin, int vCount, const MSH_FLATVERTEX vertex[], MSH_FLATVERTEX tmpVertex[]) const;
	 void ExtractAllNonSkinSegment(ZeroImportExportNode *skinNode,  CMSHBlockModelSegment *skinSegment, ZeroPolyhedra& skin, int vCount, const MSH_FLATVERTEX vertex[], MSH_FLATVERTEX tmpVertex[]) const;

/*
	//
	//
	//	CGF - the idea here was that objects pose-constrained to other objects would become children
	//			of the constraining object.  we don't want this in CW, because artists sometimes use 
	//			pose-constraints within the hierarchy to animate, and we want to keep the hierarchy unchanged.

	 void PromoteContrainedModels ();
*/

    void CompressAnimationKeyFrames (CZeroData *data);
    void SkeletonAndAnimationLeftToRight(CZeroData *data);
    void SkeletonLeftToRight(CZeroData *data);
    void AnimationLeftToRight(CZeroData *data);
    void CameraLeftToRight(CZeroData *data);
    void LightsLeftToRight(CZeroData *data);
    void ApplyScale(const ZeroMatrix& parentScaleMatrix);

    void CreateVertexIndexStripsAndIndexList (CMSHBlockModelSegment *segment);
	 void CreateVertexIndexStripsAndIndexList ();


    void RightToLeftHanded ();
    void TotalVertexCount(int& vertexCount)	const;
    void FlatVertexArray (const ZeroMatrix &worldMatrix, ZeroVector3 *vertex, int &vCount) const;
    void CalcGlobalBBox (CZeroData *data);
    void CalcLocalBBox();
    void CalcLocalsBBoxes();

	void CreateCollisionSphere(const ZeroSphere sph, MSH_ClothCollisionSphere *sphereData);
	void CreateCollisionCylinder(const ZeroSphere sph, MSH_ClothCollisionCylinder *cylinderData);
	void CreateCollisionCone(const ZeroSphere sph, MSH_ClothCollisionCone *coneData, unsigned int vCount, ZeroVector3 *pVertex);
	void ResolveCollisionObjects();
	void PrintHierarchy (ZeroFile &file,  char *indentation) const;
	bool HasAnimatedChild(CZeroData *data, ZeroImportExportNode *root);
	bool IsInSkeleton(CZeroData *data);
	bool MergeChildGeometries(CZeroData *data,ZeroImportExportNode *root);
	void MergeAllChildGeometries(CZeroData *data);
	void GetModelArray(CMSHBlockModel **array, int& count);
	void AsignClothCollisionToModelsAndRemoveClothCollisionGeometry(CZeroData *data);


	public:

    ZeroMatrix GetWorldMatrix(ZeroImportExportNode *root) const;
    ZeroMatrix GetMatrix() const;
	ZeroImportExportNode (CMSHBlockModel *mshModelArg, void *cgcModelArg);

	void SetCgcModel (void* model)
	{
     cgcModel = model;
	}

	void SetMark (int markArg)	const
	{
		mark = markArg;
	}

	int GetMark() const
	{
		return mark;
	}

	void *GetCgcModel() const;
	CMSHBlockModel* GetMshModel() const;

	ZeroImportExportNode* FindByMSHModel (const CMSHBlockModel *model);
	ZeroImportExportNode* FindByCgcModel (const void *model);


	void OnInportFile (CZeroData *data);
	void OnExportFile (CZeroData *data, bool bRemapSkinMatrices = true, bool bApplyLeftToRight = true, 
							 bool bApplyScale = true, bool bSortModels = true, bool bFlattenGeometries = false);
	void ApplyGlobalRotationOnTriggerBoxes(ZeroMatrix globalMatrix,ZeroVector3 startPos);
	void ApplyGlobalRotation(CZeroData *zeroData,ZeroMatrix globalMatrix);
	static void CalculateDoubleIKKeyFrames(const ZeroVector3 *bone1Rots, const ZeroVector3 *bone2Rots, const ZeroVector3 bone2Pos, const ZeroVector3 effPos, const CMSHBlockEndEffector *effector,int startFrame,int endFrame, CMSHBlockPositionFrame *outPosFrames, CMSHBlockRotationFrame *outRotFrames);
	static void CalculateSingleIKKeyFrames(const ZeroVector3 *boneRots, const ZeroVector3 effPos, int startFrame,int endFrame, CMSHBlockPositionFrame *outPosFrames, CMSHBlockRotationFrame *outRotFrames);


};


    
#endif
    
    
            