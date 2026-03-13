#include "Zero.h"
#include "ZeroImportExportNode.h"

#include "ZeroData.h"
#include "MSHBlockModel.h"
#include "MSHBlockCamera.h"
#include "MSHBlockFogData.h"
#include "MSHBlockFileInfo.h"
#include "MSHBlockAmbience.h"
#include "MSHBlockSpotLight.h"
#include "MSHBlockSceneInfo.h"
#include "MSHBlockPointLight.h"
#include "MSHBlockAnimations.h"
#include "MSHBlockMaterialSet.h"
#include "MSHBlockConstraintList.h"
#include "MSHBlockInfiniteLight.h"


//#define EXPORT_HARD_SKIN


//const MIN_FREE_VERTEX_SEPARATION = 16;
const MIN_FREE_VERTEX_SEPARATION = 32;


#define EXPORT_CLOTH_COLLISION_AS_GEOMETRY


ZeroImportExportNode::ZeroImportExportNode (CMSHBlockModel *mshModelArg, void *cgcModelArg)
	:ZeroNode<ZeroImportExportNode>(false)
{
	bDeformer=false;
	mark = 0;
	cgcModel = cgcModelArg;
	mshModel = mshModelArg;
}


void ZeroImportExportNode::PrintHierarchy (ZeroFile &file, char *indentation) const
{
	const char* name = "root";
	if (GetMshModel()) {
		name = GetMshModel()->GetName();
	}
	
	file.Print ("%s%s\n", indentation, name);
	ZeroBaseNode::PrintHierarchy (file, indentation);
}



void ZeroImportExportNode::OnInportFile (CZeroData *data)
{
	// do not change the order of these calls
	RightToLeftHanded ();
	CameraLeftToRight(data);
	LightsLeftToRight(data);
	SkeletonAndAnimationLeftToRight (data);
}



void ZeroImportExportNode::OnExportFile (CZeroData *data,bool bRemapSkinMatrices, bool bApplyLeftToRight, bool bApplyScale, bool bSortModels, bool bFlattenGeometries)
{
	int modelCount;
   CMSHBlockModel **mshModelArray;


	// do not change the order of these calls
	if(bApplyLeftToRight)
	{
		SkeletonAndAnimationLeftToRight (data);
	}

	CompressAnimationKeyFrames (data);
	
	
	if(bApplyScale)
	{
		ApplyScale(identityMatrix);
	}
	if(bApplyLeftToRight)
	{
		RightToLeftHanded ();
	}


	if(bApplyLeftToRight)
	{
		CameraLeftToRight(data);
		LightsLeftToRight(data);
	}

/*
	// contranint objects are coverted to childens
	PromoteContrainedModels ();
*/
	// hard skinned polygons are converted to solid segments
	//ExtractNonSkinPolygonsFromSkin(); -- REMOVED for SpecWarrior --DY


	if(bFlattenGeometries)
	{
		if(!bRemapSkinMatrices)
		{
			//
			//	models are already marked skin_node and envelop_node 
			//   - need to change all CGCModel values of bones to match matrixpaletteindex
			//		then set bRemapSkinMatrices = true
			SetCGCModelToMatrixPaletteIndex();
			MarkAllDeformers(this,data);
			bRemapSkinMatrices=true;
		}
		else
		{
			int index;
			//	1. mark models as envelop_nodes
			RemapAllSkinMatrices1(this);
			//  2. set matrixpaletteindex for all envelope_nodes
			index = 0;
			EnumerateEnvelopMatrices (index);

			//  3. remap bone indices for all skins' boneweight pairs
			RemapAllSkinMatrices2(this);

			CreateAllEnvelopArrays ();
			SetCGCModelToMatrixPaletteIndex();
			MarkAllDeformers(this,data);
		}

		MergeAllChildGeometries(data);
		data->RemoveOrphans();
	}
	// splite large segments 
	SplitSegments();
	CreateVertexIndexStripsAndIndexList();

	ResolveCollisionObjects();
	AsignClothCollisionToModelsAndRemoveClothCollisionGeometry(data);
	

	CalcLocalsBBoxes();
	CalcGlobalBBox(data);
						  
   if(bSortModels)
   {
	   mshModelArray = data->GetModels(modelCount);
		modelCount = 0;
		ZeroHeap <ZeroImportExportNode*, LONGLONG> sortModels (512, 0xfffffffffffffff);
		SortSkinNodes(mshModelArray, modelCount, sortModels);
		if(data->GetSkeleton())
		{
			SortSkeleton (data->GetSkeleton(), mshModelArray, modelCount);
		}
		if(data->GetAnimation())
		{
			SortAnimation (data->GetAnimation(), mshModelArray, modelCount);
		}
   }

	//	the converter tool does not remap skin matrices
	if(bRemapSkinMatrices) {
		int index;
		//	1. mark models as envelop_nodes
		RemapAllSkinMatrices1(this);
		//  2. set matrixpaletteindex for all envelope_nodes
		index = 0;
		EnumerateEnvelopMatrices (index);

		//  3. remap bone indices for all skins' boneweight pairs
		RemapAllSkinMatrices2(this);

		CreateAllEnvelopArrays ();
	}

}


void ZeroImportExportNode::SetCGCModelToMatrixPaletteIndex()
{
	ZeroImportExportNode *node;
	for (node = GetFirst(); node; node = node->GetNext()) 
	{
		if(node->mshModel)
		{
			node->SetCgcModel((void *)node->mshModel->GetMatrixPaletteIndex());
		}
	}
}

void ZeroImportExportNode::SplitSegments()
{
	ZeroImportExportNode *node;
	for (node = GetFirst(); node; node = node->GetNext()) {
		if (node->mshModel) {
			node->mshModel->SpliteSegments();
		}
	}
}


ZeroImportExportNode* ZeroImportExportNode::FindByCgcModel (const void *model)
{
	ZeroImportExportNode *node;
	for (node = GetFirst(); node; node = node->GetNext()) {
		if (node->cgcModel == model) {
			return node;
		}
	}
	return NULL;
}

ZeroImportExportNode* ZeroImportExportNode::FindByMSHModel (const CMSHBlockModel *model)
{
	ZeroImportExportNode *node;
	for (node = GetFirst(); node; node = node->GetNext()) {
		if (node->mshModel == model) {
			return node;
		}
	}
	return NULL;
}



void ZeroImportExportNode::ApplyGlobalRotationOnTriggerBoxes(ZeroMatrix globalMatrix,ZeroVector3 startPos)
{
	CMSHBlockModel *model = GetMshModel();
	if(model)
	{
		model->RotateTriggerBox(globalMatrix,startPos);
	}
	ZeroImportExportNode *child;
	for(child=GetChild();child;child=child->GetSibling())
	{
		child->ApplyGlobalRotationOnTriggerBoxes(globalMatrix,startPos);
	}
}

void ZeroImportExportNode::ApplyGlobalRotation(CZeroData *zeroData,ZeroMatrix globalMatrix)
{
	CMSHBlockModel *model = GetMshModel();
	ZeroVector3 p;
	ZeroQuaternion q;


	if(model)
	{
		model->GetTrans (p.x, p.y, p.z);
		model->GetQuatRot (q.x, q.y, q.z, q.s);
		ZeroMatrix preRootMatrix (q, p);

		p = globalMatrix.TransformVector (p);
		if(strcmp(model->GetName(),"torso_eff__h")==0)
		{
			ApplyGlobalRotationOnTriggerBoxes(globalMatrix,p);
		}

		ZeroMatrix postRootMatrix (q, p);
		model->SetTranslation (p.x, p.y, p.z);
		

		int count;
		vect3	*points;
		int segmentCount;

		globalMatrix = preRootMatrix * globalMatrix * postRootMatrix.Inverse(); 
		CMSHBlockModelSegment **segments	= model->GetSegments(segmentCount);
		for (int i = 0; i < segmentCount; i ++) 
		{
			ZeroMatrix rotNormals (globalMatrix);
			rotNormals.posit = ZeroVector3 (0, 0, 0);

			points = segments[i]->GetPositions(count);
			rotNormals.TransformVectors (&points[0].fX, sizeof (vect3), &points[0].fX, sizeof (vect3),	count);

			points = segments[i]->GetNormals(count);
			rotNormals.TransformVectors (&points[0].fX, sizeof (vect3), &points[0].fX, sizeof (vect3),	count);
		}
	}
	else
	{
		//
		//	if this is the NULL root node, then apply global rotation to the camera and lights
		//
		int i,nLights;
		CMSHBlockInfiniteLight **pInfLights=zeroData->GetInfiniteLights(nLights);
		for(i=0;i<nLights;i++)
		{
			pInfLights[i]->GetDirection(p.x,p.y,p.z);
			p=globalMatrix.TransformVector(p);
			pInfLights[i]->SetDirection(p.x,p.y,p.z);
		}

		CMSHBlockSpotLight **pSpotLights=zeroData->GetSpotLights(nLights);
		for(i=0;i<nLights;i++)
		{
			pSpotLights[i]->GetPosition(p.x,p.y,p.z);
			p=globalMatrix.TransformVector(p);
			pSpotLights[i]->SetPosition(p.x,p.y,p.z);

			pSpotLights[i]->GetInterest(p.x,p.y,p.z);
			p=globalMatrix.TransformVector(p);
			pSpotLights[i]->SetInterest(p.x,p.y,p.z);
		}

		CMSHBlockPointLight **pPointLights=zeroData->GetPointLights(nLights);
		for(i=0;i<nLights;i++)
		{
			pPointLights[i]->GetPosition(p.x,p.y,p.z);
			p=globalMatrix.TransformVector(p);
			pPointLights[i]->SetPosition(p.x,p.y,p.z);
		}

		CMSHBlockCamera *pCamera=zeroData->GetCamera();
		if(pCamera)
		{
			pCamera->GetPosition(p.x,p.y,p.z);
			p=globalMatrix.TransformVector(p);
			pCamera->SetPosition(p.x,p.y,p.z);

			pCamera->GetInterest(p.x,p.y,p.z);
			p=globalMatrix.TransformVector(p);
			pCamera->SetInterest(p.x,p.y,p.z);
		}
	}

	ZeroImportExportNode *child;
	for(child=GetChild();child;child=child->GetSibling())
	{
		child->ApplyGlobalRotation(zeroData,globalMatrix);
	}
}

void *ZeroImportExportNode::GetCgcModel() const
{
	return cgcModel;
}


CMSHBlockModel* ZeroImportExportNode::GetMshModel() const
{
	return mshModel;
}



ZeroMatrix ZeroImportExportNode::GetMatrix() const
{
	ZeroVector3 p;
	ZeroQuaternion q;
	
	mshModel->GetQuatRot(q.x, q.y, q.z, q.s);
	mshModel->GetTrans(p.x, p.y, p.z);
	
	return ZeroMatrix (q, p);
}

ZeroMatrix ZeroImportExportNode::GetWorldMatrix(ZeroImportExportNode *root) const
{
	const ZeroImportExportNode *obj;
	
	ZeroMatrix matrix (identityMatrix);
	for (obj = this; (obj != root) && (obj->GetMshModel()) ; obj = obj->GetParent()) {
		matrix = matrix * obj->GetMatrix();
	}
	return matrix;
}




void ZeroImportExportNode::CameraLeftToRight(CZeroData *data)
{
	float fX;
	float fY;
	float fZ;
	CMSHBlockCamera *camera;
	
	camera = data->GetCamera();
	
	if (camera) {
		camera->GetPosition(fX, fY, fZ);
		camera->SetPosition(fX, fY, -fZ);
		
		camera->GetInterest(fX, fY, fZ);
		camera->SetInterest(fX, fY, -fZ);


		ZeroVisList<CMSHBlockCameraAnimation> *anims=&(camera->GetAnimations());
		ZeroVisList<CMSHBlockCameraAnimation>::ZeroListNode *node=anims->GetFirst();
		while(node)
		{
			int nKeyframes=node->info.GetKeyframesCount();
			for(int i=0;i<nKeyframes;i++)
			{
				ZeroMatrix M(node->info[i].rotation,node->info[i].position);
				M=YawMatrix(PI)*M;

				M.posit.x=-M.posit.x;
				node->info[i].position=M.posit;

				ZeroQuaternion q(M);
				q.x=-q.x;
//				q.y=-q.y;
				q.s=-q.s;
				node->info[i].rotation=q;
			}
			node=node->GetNext();
		}
	}
}


void ZeroImportExportNode::LightsLeftToRight(CZeroData *data)
{
	int i;
	int nLights;
	float fX;
	float fY;
	float fZ;
	
	CMSHBlockSpotLight **m_ppSpotLights;
	CMSHBlockPointLight **m_ppPointLights;
	CMSHBlockInfiniteLight **m_ppInfiniteLights;
	
	m_ppSpotLights = data->GetSpotLights(nLights);
	for (i = 0; i < nLights; i ++) {
		m_ppSpotLights[i]->GetPosition(fX, fY, fZ);
		m_ppSpotLights[i]->SetPosition(fX, fY, -fZ);
		
		m_ppSpotLights[i]->GetInterest(fX, fY, fZ);
		m_ppSpotLights[i]->SetInterest(fX, fY, -fZ);
	}
	
	
	m_ppPointLights = data->GetPointLights(nLights);
	for (i = 0; i < nLights; i ++) {
		m_ppPointLights[i]->GetPosition(fX, fY, fZ);
		m_ppPointLights[i]->SetPosition(fX, fY, -fZ);
	}
	
	m_ppInfiniteLights = data->GetInfiniteLights(nLights);
	for (i = 0; i < nLights; i ++) {
		m_ppInfiniteLights[i]->GetDirection(fX, fY, fZ);
		m_ppInfiniteLights[i]->SetDirection(fX, fY, -fZ);
	}
}



void ZeroImportExportNode::ApplyScale(const ZeroMatrix& parentScaleMatrix)
{
	int i;
	int segmentsCount;
	ZeroImportExportNode* obj;
	CMSHBlockModelSegment **segments;
	
	ZeroMatrix scaleMatrix (parentScaleMatrix);
	if (mshModel) {
		ZeroVector3 posit;
		mshModel->GetTrans (posit.x, posit.y, posit.z);
		posit = parentScaleMatrix.RotateVector (posit);
		mshModel->SetTranslation (posit.x, posit.y, posit.z);
		
		scaleMatrix = identityMatrix;
		mshModel->GetScale(scaleMatrix.right.x, scaleMatrix.up.y, scaleMatrix.front.z);
		
		scaleMatrix = scaleMatrix * parentScaleMatrix;
		mshModel->SetScale(1.0, 1.0, 1.0);
		
		segments = mshModel->GetSegments(segmentsCount);
		for (i = 0; i < segmentsCount; i ++) {
			int vertecCount;
			vect3 *vertex;
			
			vertex = segments[i]->GetPositions(vertecCount);
			scaleMatrix.TransformVectors (vertex, sizeof (vect3), vertex, sizeof (vect3), vertecCount);
		}
	}
	
	for (obj = GetChild(); obj; obj = obj->GetSibling()) {
		obj->ApplyScale(scaleMatrix);
	}
}


ZeroMatrix ZeroImportExportNode::GetWorldScale() const
{
	ZeroImportExportNode* obj;
	ZeroMatrix scaleMatrix (identityMatrix);
	
	for (obj = (ZeroImportExportNode*)this; obj != NULL; obj = obj->GetParent()) {
		ZeroMatrix matrix (identityMatrix);
		if (obj->GetMshModel()) {
			obj->GetMshModel()->GetScale(matrix.right.x, matrix.up.y, matrix.front.z);
		}
		scaleMatrix = scaleMatrix * matrix;
	}
	
	return scaleMatrix;
}






void ZeroImportExportNode::RightToLeftHanded ()
{
	int i;
	int vertecCount;
	int segmentsCount;
	vect3 *vertex;
	vect3 *normal;
	ZeroImportExportNode* obj;
	CMSHBlockModelSegment **segments;
	
	if (mshModel) {
		ZeroVector3 posit;
		mshModel->GetTrans (posit.x, posit.y, posit.z);
		mshModel->SetTranslation (posit.x, posit.y, -posit.z);
		mshModel->LeftToRightTriggerBox();
		
		ZeroQuaternion q; 
		mshModel->GetQuatRot(q.x, q.y, q.z, q.s);
		mshModel->SetRotation(-q.x, -q.y, q.z, q.s);
		
		ZeroMatrix matrix (identityMatrix);
		matrix.front.z *= -1.0;
		
		segments = mshModel->GetSegments(segmentsCount);
		for (i = 0; i < segmentsCount; i ++) {
			vertex = segments[i]->GetPositions(vertecCount);
			normal = segments[i]->GetNormals(vertecCount);
			matrix.TransformVectors (vertex, sizeof (vect3), vertex, sizeof (vect3), vertecCount);
			matrix.TransformVectors (normal, sizeof (vect3), normal, sizeof (vect3), vertecCount);
			segments[i]->GetPolygonList()->FlipWinding();
		}
	}
	
	for (obj = GetChild(); obj; obj = obj->GetSibling()) {
		obj->RightToLeftHanded();
	}
}



void ZeroImportExportNode::TotalVertexCount(int& vertexCount)	const
{	
	int i;
	int vCount;
	int segmentsCount;
	ZeroImportExportNode *obj;
	CMSHBlockModelSegment **segments;
	
	if (mshModel) {
		segments = mshModel->GetSegments(segmentsCount);
		for (i = 0; i < segmentsCount; i ++) {
			segments[i]->GetPositions(vCount);
			vertexCount += vCount;
		}
	}
	
	for (obj = GetChild(); obj; obj = obj->GetSibling()) {
		obj->TotalVertexCount(vertexCount);
	} 
}

void ZeroImportExportNode::FlatVertexArray (
	const ZeroMatrix &worldMatrix, 
	ZeroVector3 *vertex, 
	int &vCount) const
{
	int i;
	int segmentsCount;
	ZeroImportExportNode *obj;
	CMSHBlockModelSegment **segments;
	
	ZeroMatrix matrix (worldMatrix);
	if (mshModel) {
		matrix = GetMatrix() * matrix;
		if (!mshModel->IsAnimTriggerCollision ()) {
			segments = mshModel->GetSegments(segmentsCount);
			for (i = 0; i < segmentsCount; i ++) {
				int vertexCount;
				vect3 *vertexPtr;
				vertexPtr = segments[i]->GetPositions(vertexCount);
				matrix.TransformVectors (vertex + vCount, sizeof (ZeroVector3), vertexPtr, sizeof (vect3), vertexCount);
				vCount += vertexCount;
			}
		}                        
	}
	
	for (obj = GetChild(); obj; obj = obj->GetSibling()) {
		obj->FlatVertexArray (matrix, vertex, vCount);
	}
}


void ZeroImportExportNode::CalcGlobalBBox (CZeroData *data)
{
	int vCount;
	int maxVertex;
	
	vCount = 0;
	maxVertex = 0;
	TotalVertexCount(maxVertex);
	
	ZeroStack<ZeroVector3> pool(maxVertex + 1024);
	FlatVertexArray (identityMatrix, &pool[0], vCount);
	
	if (!vCount) {
		return; 
	}
	
	ZeroVector3 min;
	ZeroVector3 max;
	GetMinMax (min, max, &pool[0].x, sizeof (ZeroVector3), vCount);
	
	
	ZeroVector3 pos (min + max);
	pos = pos.Scale (0.5f);
	
	ZeroVector4 size (max - min);
	size = size.Scale (0.5f);
	
	size.w = sqrtf (DotProduct (size, size));
	data->GetSceneInfo()->SetGlobalBBox(size.x, size.y, size.z, size.w, pos.x, pos.y, pos.z);
}


void ZeroImportExportNode::CalcLocalBBox()
{
	int i;
	int j;
	int vertexCount;
	int segmentsCount;
	CMSHBlockModelSegment **segments;
	
	vertexCount = 0;
	
	segments = mshModel->GetSegments(segmentsCount);
	for (i = 0; i < segmentsCount; i ++) {
		int count;
		segments[i]->GetPositions(count);
		vertexCount += count;
	}
	
	ZeroPolyhedra polyhedra;
	ZeroStack<ZeroVector3> vertexPool (vertexCount);

	
	polyhedra.BeginFace();
	vertexCount = 0;
	for (i = 0; i < segmentsCount; i ++) {
		int i0;
		int i1;
		int i2;
		int vCount;
		int fCount;
		vect3* ptr;
		unsigned short *Index;
		
		ptr = segments[i]->GetPositions(vCount);
		for (j = 0; j < vCount; j ++) {
			vertexPool[j + vertexCount].x = ptr[j].fX;
			vertexPool[j + vertexCount].y = ptr[j].fY;
			vertexPool[j + vertexCount].z = ptr[j].fZ;
		}
		
		fCount = segments[i]->GetTriangleList()->m_nTriangleCount;
		Index = segments[i]->GetTriangleList()->m_nTriangleIndices;
		
		for (j = 0; j < fCount; j ++) {
			i0 = Index[j * 3 + 0] + vertexCount; 
			i1 = Index[j * 3 + 1] + vertexCount; 
			i2 = Index[j * 3 + 2] + vertexCount;
			polyhedra.AddFace (i0, i1, i2);
		}
		vertexCount += vCount;
	}
	polyhedra.EndFace();
	
	ZeroSphere sph(polyhedra.CalculateSphere (&vertexPool[0].x, sizeof (ZeroVector3)));
	
	ZeroQuaternion R (sph);
	ZeroVector3 T (sph.posit);
	ZeroVector4 D (sph.size);
	
	mshModel->SetBBoxRotation(R.x,R.y,R.z,R.s);
	mshModel->SetBBoxTranslation(T.x,T.y,T.z);
	mshModel->SetBBoxDimension(D.x,D.y,D.z,D.w);
}


void ZeroImportExportNode::CreateCollisionSphere(const ZeroSphere sph, MSH_ClothCollisionSphere *sphereData)
{
	sphereData->center_x=sph.posit.x;
	sphereData->center_y=sph.posit.y;
	sphereData->center_z=sph.posit.z;
	sphereData->radius=sph.size.x;
	if(sph.size.y>sphereData->radius)
		sphereData->radius=sph.size.y;
	if(sph.size.z>sphereData->radius)
		sphereData->radius=sph.size.z;
}

void ZeroImportExportNode::CreateCollisionCylinder(const ZeroSphere sph, MSH_ClothCollisionCylinder *cylinderData)
{
	float xz,xy,yz;
	
	ZeroVector4 center = ZeroVector4(sph.posit);
	xy=fabsf(sph.size.x-sph.size.y);
	xz=fabsf(sph.size.x-sph.size.z);
	yz=fabsf(sph.size.y-sph.size.z);
	if((xy<xz) && (xy<yz))
	{
		cylinderData->height = 2*(sph.size.z);
		cylinderData->radius = sph.size.x;
		cylinderData->dir_x=sph.front.x;
		cylinderData->dir_y=sph.front.y;
		cylinderData->dir_z=sph.front.z;
	}
	else
	{
		if((xz<xy) && (xz<yz))
		{
			cylinderData->radius = sph.size.x;
			cylinderData->height = 2*(sph.size.y);
			cylinderData->dir_x=sph.up.x;
			cylinderData->dir_y=sph.up.y;
			cylinderData->dir_z=sph.up.z;
		}
		else
		{
			cylinderData->radius = sph.size.y;
			cylinderData->height = 2*(sph.size.x);
			cylinderData->dir_x=sph.right.x;
			cylinderData->dir_y=sph.right.y;
			cylinderData->dir_z=sph.right.z;
		}
	}
	
	cylinderData->top_x=center.x-0.5f*cylinderData->dir_x*cylinderData->height;
	cylinderData->top_y=center.y-0.5f*cylinderData->dir_y*cylinderData->height;
	cylinderData->top_z=center.z-0.5f*cylinderData->dir_z*cylinderData->height;
}

void ZeroImportExportNode::CreateCollisionCone(const ZeroSphere sph, MSH_ClothCollisionCone *coneData, unsigned int vCount, ZeroVector3 *pVertex)
{
	float xz,xy,yz;
	
	ZeroVector4 center = ZeroVector4(sph.posit);
	xy=fabsf(sph.size.x-sph.size.y);
	xz=fabsf(sph.size.x-sph.size.z);
	yz=fabsf(sph.size.y-sph.size.z);
	if((xy<xz) && (xy<yz))
	{
		coneData->height = 2*(sph.size.z);
		coneData->radius1 = sph.size.x;
		coneData->dir_x=sph.front.x;
		coneData->dir_y=sph.front.y;
		coneData->dir_z=sph.front.z;
	}
	else
	{
		if((xz<xy) && (xz<yz))
		{
			coneData->radius1 = sph.size.x;
			coneData->height = 2*(sph.size.y);
			coneData->dir_x=sph.up.x;
			coneData->dir_y=sph.up.y;
			coneData->dir_z=sph.up.z;
		}
		else
		{
			coneData->radius1 = sph.size.y;
			coneData->height = 2*(sph.size.x);
			coneData->dir_x=sph.right.x;
			coneData->dir_y=sph.right.y;
			coneData->dir_z=sph.right.z;
		}
	}
	
	coneData->top_x = center.x-0.5f*coneData->dir_x*coneData->height;
	coneData->top_y = center.y-0.5f*coneData->dir_y*coneData->height;
	coneData->top_z = center.z-0.5f*coneData->dir_z*coneData->height;
	coneData->radius2=0.0;
	
	if(vCount)
	{
		ZeroVector3 tmpTop(coneData->top_x,coneData->top_y,coneData->top_z);
		ZeroVector3 Dir(coneData->dir_x,coneData->dir_y,coneData->dir_z);
		float tmpTest0=0.01f;
		float tmpTest1=0.1f;
		float tmpTest2=0.1f;
		float tmpTest3=0.1f;
		float tmpTest4=0.01f;
		for(unsigned int i=0;i<vCount;i++)
		{
			ZeroVector3 vtx=pVertex[i];
			vtx-=tmpTop;
			
			float r=Magnitude(vtx);
			if(fabsf(r)>tmpTest0)
			{
				if(fabsf(DotProduct(Dir,vtx))<tmpTest1)
				{
					tmpTest0 = fabsf(r);
					tmpTest1 = fabsf(DotProduct(Dir,vtx));
					if(fabsf(r-coneData->radius1)<tmpTest2)
					{
						tmpTest2 = fabsf(r-coneData->radius1);
						ZeroVector3 tmpBottom(center.x+0.5f*coneData->dir_x*coneData->height,
							center.y+0.5f*coneData->dir_y*coneData->height,
							center.z+0.5f*coneData->dir_z*coneData->height);
						for(unsigned int j=0;j<vCount;j++)
						{
							ZeroVector3 vtx2=pVertex[j];
							vtx2-=tmpBottom;
							float r2=Magnitude(vtx2);
							if(fabsf(r2)>tmpTest4)
							{
								if(fabsf(DotProduct(Dir,vtx2))<tmpTest3)
								{
									tmpTest4 = fabsf(r2);
									tmpTest3 = fabsf(DotProduct(Dir,vtx2));
									if(coneData->radius2<r2)
										coneData->radius2=r2;
								}
							}
						}
						break;
					}
					else
					{
						if(coneData->radius2<r)
							coneData->radius2=r;
						coneData->top_x = center.x+0.5f*coneData->dir_x*coneData->height;
						coneData->top_y = center.y+0.5f*coneData->dir_y*coneData->height;
						coneData->top_z = center.z+0.5f*coneData->dir_z*coneData->height;
						coneData->dir_x=-coneData->dir_x;
						coneData->dir_y=-coneData->dir_y;
						coneData->dir_z=-coneData->dir_z;
					}
				}
			}
		}
	}
}


void ZeroImportExportNode::CalcLocalsBBoxes()
{
	ZeroImportExportNode *obj;
	
	if (mshModel) {
		int count;
		if ( (mshModel->GetModelType() != Shadow_Volume) && ( mshModel->GetSegments(count))) {
			CalcLocalBBox();
		} else {
        	mshModel->SetBBoxRotation(0, 0, 0, 1.0);
			mshModel->SetBBoxTranslation(0, 0, 0);
        	mshModel->SetBBoxDimension(0, 0, 0, 0);
		}
	}
	
	for (obj = GetChild(); obj; obj = obj->GetSibling()) {
		obj->CalcLocalsBBoxes();
	}
}




void ZeroImportExportNode::SkeletonAndAnimationLeftToRight(CZeroData *data)
{
	if (data->GetSkeleton()) {
		SkeletonLeftToRight(data);
		AnimationLeftToRight(data);
	}
}

void ZeroImportExportNode::SkeletonLeftToRight(CZeroData *data)
{
	//    ZeroImportExportNode *node;
	//    CMSHBlockSkeleton::Iterator iter (*data->GetSkeleton());
	//    for (iter.Begin(); iter; iter ++) {
	//
	//        CMSHBlockSkeletonJoint *joint;
	//
	//        joint = *iter;
	//        do {
	//            node = FindByMSHModel (joint->GetModel());
	//            ZeroMatrix scaleMatrix (node->GetParent()->GetWorldScale());
	//            joint->LeftToRight(scaleMatrix);
	//            joint = joint->GetBone();
	//        } while (joint); 
	//    }
}


void ZeroImportExportNode::AnimationLeftToRight(CZeroData *data)
{
	int i;
	CMSHBlockModel *model;
	ZeroImportExportNode *node;
	CMSHBlockNodeAnimation* anim;
	CMSHBlockPositionFrame *posframes;
	CMSHBlockRotationFrame *rotframes;
	
	
	ZeroMatrix mat(identityMatrix);
	mat.front.z *= -1;
	
//	ZeroMatrix mat1(identityMatrix);
//	mat1.up.y *= -1;
//	mat1.right.x *= -1;
	
	if (data->GetAnimation()) {
		
		CMSHBlockAnimation::Iterator iter (*data->GetAnimation());
		for (iter.Begin(); iter; iter ++) {
			anim = &(*iter);
			
			model = anim->GetModel();
			
			node = FindByMSHModel (model);

			posframes = &anim->mPositions[0];
			rotframes = &anim->mRotations[0];

#if 0
			ZeroMatrix scaleMatrix (node->GetWorldScale());
			scaleMatrix.TransformVectors (&posframes->position, sizeof (CMSHBlockPositionFrame), &posframes->position, sizeof (CMSHBlockPositionFrame), anim->GetNumPositions());
			mat.TransformVectors (&posframes->position, sizeof (CMSHBlockPositionFrame), &posframes->position, sizeof (CMSHBlockPositionFrame), anim->GetNumPositions());
#else
			ZeroMatrix matrix (node->GetWorldScale() * mat);
			matrix.TransformVectors (&posframes->position, sizeof (CMSHBlockPositionFrame), 
											 &posframes->position, sizeof (CMSHBlockPositionFrame), 
											 anim->GetNumPositions());

#endif

			for (i = 0; i < anim->GetNumRotations(); i ++) 
			{
				rotframes[i].rotation.x *= -1.0;
				rotframes[i].rotation.y *= -1.0;
			}
		}
	}
}



void ZeroImportExportNode::CompressAnimationKeyFrames (CZeroData *data)
{
	if (data->GetAnimation()) {
		data->GetAnimation()->CompressKeyFrames();
	}
}







void ZeroImportExportNode::ExtractNonSkinSegment(
	ZeroImportExportNode *skinNode, 
	CMSHBlockModelSegment *skinSegment,
	ZeroPolyhedra& skin,
	int vCount,
	const MSH_FLATVERTEX vertex[],
	MSH_FLATVERTEX tmpVertex[]) const
{
	int n;
	int mark;
	int index;
	int freePoly;
	bool hasUVs[MAX_TEXTURES_PER_MSH_MATERIAL];
//	bool hasUV1;
	bool hasNormal;
	bool hasColor;
	bool hasLight;
	bool hasWeight;
	bool addPolys;
	ZeroEdge *ptr;
	ZeroEdge *edge;
	unsigned boneID;
	ZeroPolyhedra singleMesh;
	CMSHBlockModelSegment *segment;


	freePoly = 0;
	singleMesh.BeginFace ();
	boneID = (unsigned) cgcModel;

	mark = skin.IncLRU();
	ZeroPolyhedra::Iterator iter(skin);

	for (iter.Begin(); iter; iter ++) {
		edge = &(*iter)->info;


		if (edge->incidentFace < 0) {
			continue;
		}

		if (edge->mark == mark) {
			continue;
		}

		ptr = edge;
		do {
			ptr->mark = mark;
			ptr = ptr->twin->next;
		} while (ptr != edge); 

		addPolys	= true;
		ptr = edge;
		do {
			if (ptr->incidentFace > 0) {
				ZeroEdge *polyEdge;
				polyEdge	= ptr;
				do {
					index = polyEdge->incidentVertex;
					const MSH_FLATVERTEX& point = vertex[index];

					if (point.weightId[0] != boneID) {
						addPolys	= false;
						break;
					}

					if (point.vertex.weight[0]	< 0.995f) {
						addPolys	= false;
						break;
					}

					polyEdge	= polyEdge->next;
				} while (polyEdge	!= ptr);
			}

			ptr = ptr->twin->next;
		} while (addPolys && (ptr != edge)); 

		if (addPolys) {
			int indexCount;
			int index [2048];

			freePoly ++;
			ptr = edge;
			do {
				if (ptr->incidentFace > 0) {
					ZeroEdge *polyEdge;

					indexCount = 0;
					polyEdge	= ptr;
					do {
						index [indexCount] = polyEdge->incidentVertex;
						indexCount ++;
						polyEdge	= polyEdge->next;
					} while (polyEdge	!= ptr);
					singleMesh.AddFace (indexCount, index);
				}
				ptr = ptr->twin->next;
			} while (addPolys && (ptr != edge)); 
		}
	} 

	singleMesh.EndFace();

	if (freePoly > MIN_FREE_VERTEX_SEPARATION) {
		mark = singleMesh.IncLRU();
		ZeroPolyhedra::Iterator iter(singleMesh);
		for (iter.Begin(); iter; iter ++) {
			edge = &(*iter)->info;

			if (edge->incidentFace < 0) {
				continue;
			}

			if (edge->mark == mark) {
				continue;
			}

			ptr = edge;
			do {
				ptr->mark = mark;
				ptr = ptr->next;
			} while (ptr != edge); 


			ptr = edge;
			do {
				ZeroEdge *face;
				face = skin.FindEdge(ptr->incidentVertex, ptr->twin->incidentVertex); 
				if (face) {
					skin.DeleteFace (face);
					break;
				}

				ptr = ptr->next;
			} while (ptr != edge); 
		}

		ZeroPolyhedraDescriptor desc(&singleMesh);
		singleMesh.PackVertex (desc, tmpVertex, (void*)vertex, sizeof (MSH_FLATVERTEX));

		for (index = 0; index < desc.vertexCount + 1; index ++) {
			memset (&tmpVertex[index].weightId[0], 0, sizeof (tmpVertex[index].weightId));
			memset (&tmpVertex[index].vertex.weight[0], 0, sizeof (tmpVertex[index].vertex.weight));
		}

//		segment = mshModel->AddModelSegment();
		segment = mshModel->CreateAddSegment();
		mshModel->SetHidden(skinSegment->GetMyModel()->GetHidden());
		mshModel->SetDynamicallyLit(skinSegment->GetMyModel()->GetDynamicallyLit());
		mshModel->SetRetainNormals(skinSegment->GetMyModel()->GetRetainNormals());
		mshModel->SetRenderAfterShadows(skinSegment->GetMyModel()->GetRenderAfterShadows());
		mshModel->SetLeaveChildGeometry(skinSegment->GetMyModel()->GetLeaveChildGeometry());
		mshModel->SetPS2Optimize(skinSegment->GetMyModel()->GetPS2Optimize());

		segment->SetMaterialName (skinSegment->GetMaterialName());


		hasNormal = skinSegment->GetNormals(n) ? true : false;
		hasColor = skinSegment->GetColors(n) ? true : false;;
		hasLight = skinSegment->GetLightColors(n) ? true : false;
		hasWeight = false;
		for(int i=0;i<MAX_TEXTURES_PER_MSH_MATERIAL;i++)
			hasUVs[i] = skinSegment->GetUVCoords(n, i) ? true : false;
//		hasUV1 = skinSegment->GetUVCoords(n, 1) ? true : false;


		// transform vertex array to its local space
		
		ZeroMatrix matrix (skinNode->GetWorldMatrix(NULL)*(GetWorldMatrix(NULL).Inverse()));
		matrix.TransformVectors (&tmpVertex[0].vertex.x,	sizeof (MSH_FLATVERTEX), &tmpVertex[0].vertex.x, sizeof (MSH_FLATVERTEX), desc.vertexCount + 1);

		matrix.posit = ZeroVector4 (0.0, 0.0, 0.0, 1.0);
		matrix.TransformVectors (&tmpVertex[0].vertex.nx,	sizeof (MSH_FLATVERTEX), &tmpVertex[0].vertex.nx, sizeof (MSH_FLATVERTEX), desc.vertexCount + 1);
		segment->SetVertexData (singleMesh, tmpVertex, hasNormal, hasColor, hasLight, hasWeight, hasUVs);   
	}
}



void ZeroImportExportNode::ExtractAllNonSkinSegment(
	ZeroImportExportNode *skinNode,
	CMSHBlockModelSegment *skinSegment,
	ZeroPolyhedra& skin,
	int vCount,
	const MSH_FLATVERTEX vertex[],
	MSH_FLATVERTEX tmpVertex[]) const
{
	ZeroImportExportNode* obj;

	_ASSERTE (mshModel);
	ExtractNonSkinSegment(skinNode, skinSegment, skin, vCount, vertex, tmpVertex);

	for (obj = GetChild(); obj; obj = obj->GetSibling()) {
		obj->ExtractAllNonSkinSegment(skinNode, skinSegment, skin, vCount, vertex, tmpVertex);
	}
}

void ZeroImportExportNode::ExtractNonSkinPolygonsFromSkin(CMSHBlockModelSegment *skinSegment)
{
	int n;
	int index;
	int prevCount;
	int positionCount;
	bool hasNormal;
	bool hasColor;
	bool hasLight;
	bool hasWeight;
	bool hasUVs[MAX_TEXTURES_PER_MSH_MATERIAL];
//	bool hasUV1;

	vect3	*positions;
	ZeroPolyhedra polyhedra;
	ZeroStack<int> polyIndex(2044);

	CMSHBlockPolygonList::Iterator polyIter(*skinSegment->GetPolygonList());

	polyhedra.BeginFace();
	for (polyIter.Begin(); polyIter; polyIter	++) {
		MSHBLOCKPolygon& polygon = (*polyIter)->info;
		MSHBLOCKPolygon::Iterator indexIterator (polygon);
		index = 0;
		for (indexIterator.Begin(); indexIterator; indexIterator ++) {
			polyIndex[index] = *indexIterator;
			index ++;
		}
		polyhedra.AddFace (index, &polyIndex[0]);
	}
	polyhedra.EndFace();


	ZeroPolyhedraDescriptor desc(&polyhedra);
	positions = skinSegment->GetPositions(positionCount);
	ZeroStack<MSH_FLATVERTEX> vertex(desc.vertexCount + 10);
	ZeroStack<MSH_FLATVERTEX> tmpVertex(desc.vertexCount + 10);
	skinSegment->CopyFlatVertex (&vertex[0]);

	#ifdef EXPORT_HARD_SKIN
	for (n = 0; n < positionCount; n ++) {
		int j;
		vertex[n].vertex.weight[0] = 1.0f;
		for (j = 1; j < MAX_WEIGHT_COUNT; j ++) {
			vertex[n].vertex.weight[j] = 0.0f;
			vertex[n].weightId[j] = 0;
		}
	}
	#endif


	prevCount = polyhedra.GetCount();
	ExtractAllNonSkinSegment(this, skinSegment, polyhedra, positionCount, &vertex[0], &tmpVertex[0]);


	if (prevCount > (int)polyhedra.GetCount()) {
		hasNormal = skinSegment->GetNormals(n) ? true : false;
		hasColor = skinSegment->GetColors(n) ? true : false;;
		hasLight = skinSegment->GetLightColors(n) ? true : false;
		hasWeight = true;
		for(int i=0;i<MAX_TEXTURES_PER_MSH_MATERIAL;i++) {
			hasUVs[i] = skinSegment->GetUVCoords(n, i) ? true : false;
		}
//		hasUV1 = skinSegment->GetUVCoords(n, 1) ? true : false;

		ZeroPolyhedraDescriptor desc1(&polyhedra);
		polyhedra.PackVertex (desc1, (void*)&tmpVertex[0], (void*)&vertex[0], sizeof (MSH_FLATVERTEX));
		skinSegment->SetVertexData (polyhedra, &tmpVertex[0], hasNormal, hasColor, hasLight, hasWeight, hasUVs);   
	}

}



void ZeroImportExportNode::ExtractNonSkinPolygonsFromSkin()
{
	int i;
	int segmentCount;
	ZeroImportExportNode* obj;
	CMSHBlockModelSegment **segments;


	if (mshModel) {
		if (mshModel->GetModelType() == Skin_Node) {
			segments	= mshModel->GetSegments(segmentCount);
			for (i = 0 ; i < segmentCount; i ++) {
				ExtractNonSkinPolygonsFromSkin(segments[i]);
			}
			
			for(i=segmentCount-1;i>=0;i--)
			{
				if(!segments[i]->GetPolygonList()->GetCount())
				{
					mshModel->RemoveSegment(segments[i]);
				}
			}
			segments	= mshModel->GetSegments(segmentCount);
			if(!segmentCount)
			{
				mshModel->SetModelType(Null_Node);
			}
		}
	}

	for (obj = GetChild(); obj; obj = obj->GetSibling()) {
		obj->ExtractNonSkinPolygonsFromSkin();
	}
}





void ZeroImportExportNode::SortSkinNodes (
	CMSHBlockModel **outList, int &index,
	ZeroHeap <ZeroImportExportNode*, LONGLONG>& sortModels)
{
	ZeroImportExportNode *node;

	if (mshModel) {
		outList[index] = mshModel;
		index	++;

		sortModels.Flush ();
		for (node = GetChild(); node; node = GetChild()) {
			LONGLONG code;
			LONGLONG lowCode;
			LONGLONG highCode;
			node->AddRef();
			node->Detach();

			lowCode = CalcLowerCRC (node->GetMshModel()->GetName());

			highCode	= 1;
			switch (node->GetMshModel()->GetModelType()) {
				case Cloth_Node:
					highCode	<<= 33;
					break;

				case Static_Node:
					highCode	<<= 34;
					break;

				case Envelop_Node:
					highCode	<<= 35;
					break;

				case Null_Node:
					highCode	<<= 36;
					break;

				case Child_Skin_Node:
					highCode	<<= 37;
					break;

				case Skin_Node:
					highCode	<<= 38;
					break;

				default:
					highCode	<<= 32;

			}
			

			code = lowCode + highCode;
			sortModels.Push (node, code);
		}

		while (sortModels.GetCount()) {
			node = sortModels[0];
			node->Attach (this);
			node->Release();
			sortModels.Pop();
		}
	}
	
	for (node = GetChild(); node; node = node->GetSibling()) {
		node->SortSkinNodes(outList, index, sortModels);
	}
}


void ZeroImportExportNode::SortSkeleton(
	CMSHBlockSkeleton *skeleton,
	const CMSHBlockModel* const modelList[], 
	int modelsCount)
{
	int i;
	int j;
	int effCount;
   CMSHBlockModel* model;
   const CMSHBlockModel* srcModel;

	ZeroStack<CMSHBlockSkeleton::ZeroListNode*> effectors(1024);

	effCount	= 0;
	CMSHBlockSkeleton::ZeroListNode *node;
	for (node = skeleton->GetFirst(); node; node = skeleton->GetFirst()) {
		node->AddRef();
		effectors[effCount] = node;
		effCount ++;
		skeleton->Remove(node);
	}

	for (i = 0; i < modelsCount; i ++) {
		srcModel = modelList[i];

		for (j = 0; j < effCount; j ++) {
			node = effectors[j];
		   model = node->info.GetModel();
			if (model == srcModel) {
				skeleton->Append (node);
				node ->Release();
				break;
			}
		}
	}
}


void ZeroImportExportNode::SortAnimation(
	CMSHBlockAnimation *animation,
	const CMSHBlockModel* const modelList[], 
	int modelsCount)
{

	int i;
	int j;
	int effCount;
   CMSHBlockModel* model;
   const CMSHBlockModel* srcModel;

	ZeroStack<CMSHBlockAnimation::ZeroListNode*> effectors(1024);

	effCount	= 0;
	CMSHBlockAnimation::ZeroListNode *node;
	for (node = animation->GetFirst(); node; node = animation->GetFirst()) {
		node->AddRef();
		effectors[effCount] = node;
		effCount ++;
		animation->Remove(node);
	}

	for (i = 0; i < modelsCount; i ++) {
		srcModel = modelList[i];
		for (j = 0; j < effCount; j ++) {
			node = effectors[j];
		   model = node->info.GetModel();
			if (model == srcModel) {
				animation->Append (node);
				node ->Release();
				break;
			}
		}
	}

}


void ZeroImportExportNode::CalculateSingleIKKeyFrames(const ZeroVector3 *boneRots, const ZeroVector3 effPos, int startFrame,int endFrame, CMSHBlockPositionFrame *outPosFrames, CMSHBlockRotationFrame *outRotFrames)
{
	int i=0;
	for (int frameNum=startFrame;frameNum<=endFrame;frameNum++)
	{
		ZeroMatrix yawMatrix = YawMatrix(boneRots[i].y);
		ZeroMatrix rollMatrix = RollMatrix(boneRots[i].z);
		ZeroMatrix pitchMatrix = PitchMatrix(boneRots[i].x);

		ZeroMatrix yawRollMatrix (yawMatrix*rollMatrix);
		ZeroVector4 p (yawRollMatrix.TransformVector (effPos));

		outPosFrames[i].key = frameNum;
		outRotFrames[i].key = frameNum;

		outPosFrames[i].position = p;
		outRotFrames[i].rotation = ZeroQuaternion(pitchMatrix);

		i ++;
	}
}

void ZeroImportExportNode::CalculateDoubleIKKeyFrames(const ZeroVector3 *bone1Rots, const ZeroVector3 *bone2Rots, const ZeroVector3 bone2Pos, const ZeroVector3 effPos, const CMSHBlockEndEffector *effector,int startFrame,int endFrame, CMSHBlockPositionFrame *outPosFrames, CMSHBlockRotationFrame *outRotFrames)
{
	float l1;
	float l2;
	float arg;
	float den;
	float yaw;
	float roll;
	float pitch;
	float dist2;
	float tmp1_0;
	float tmp1_1;
	float bone_x;
	float bone_y;
	float distl1l2;
	float distl2l1;
	float cosTheta1;
	float sinTheta1;
	float cosTheta2;
	float sinTheta2;
	float bone1_Roll;
	float bone2_Roll;
	float constrainSign;

	constrainSign = effector->GetConstrainSign();


	int i=0;
	for (int frameNum=startFrame;frameNum<=endFrame;frameNum++)
	{
		_ASSERTE (fabs (bone2Rots[i].x) < 1.0); 
		_ASSERTE (fabs (bone2Rots[i].y) < 1.0); 
		
		outPosFrames[i].key = frameNum;
		outRotFrames[i].key = frameNum;

		ZeroMatrix bone1YawMatrix (YawMatrix(bone1Rots[i].y));
		ZeroMatrix bone1RollMatrix (RollMatrix(bone1Rots[i].z));
		ZeroMatrix bone1PitchMatrix (PitchMatrix(bone1Rots[i].x));

		ZeroMatrix bone2RollMatrix (RollMatrix(bone2Rots[i].z), bone2Pos);

		ZeroMatrix bone1PitchYawRollMatrix (bone1PitchMatrix * bone1YawMatrix * bone1RollMatrix);
		ZeroMatrix chainMatrix (bone2RollMatrix *bone1PitchYawRollMatrix);
		ZeroVector4 p (chainMatrix.TransformVector (effPos));

		outPosFrames[i].position = p;

		roll = atan2f (p.y, p.x); 
		ZeroVector4 p1 (p.x * cosf (roll)  + p.y * sinf (roll), p.y * cosf (roll) - p.x * sinf (roll), p.z);
		_ASSERTE (fabs (p1.y) < 0.001);

		yaw = atan2f (-p1.z, p1.x);
		if (yaw > PI * 0.5f) 
		{
			yaw = yaw - PI;
		} else if (yaw < -PI * 0.5f)
		{
			yaw = yaw + PI;
		}
		ZeroVector4 p2 (p1.x * cosf (yaw) - p1.z * sinf (yaw), p1.y, p1.x * sinf (yaw) + p1.z * cosf (yaw));
		_ASSERTE (fabs (p2.z) < 0.001);


		l2 = effPos.x;
		l1 = bone2Pos.x;


		bone_x = p2.x;
		bone_y = 0;
		den = 2.0f * l1 * l2;
	
		dist2 = bone_x * bone_x + bone_y * bone_y;
	
		distl1l2 = l1 + l2;
		distl1l2 *= distl1l2;
	
		distl2l1 = l1 - l2;
		distl2l1 *= distl2l1;

		ZeroVector4 testBonePos (bone2Pos);
	
		if (dist2 >= (distl1l2 * 0.99999f)) 
		{
			bone2_Roll = 0;
			bone1_Roll = atan2f (bone_y, bone_x); 
			testBonePos.y = bone2Pos.x;
		} 
		else if (dist2 <= (distl2l1 * 0.99999f)) 
		{
			bone2_Roll = PI;
			bone1_Roll = atan2f (bone_y, bone_x); 
			testBonePos.y = bone2Pos.x;
		} 
		else 
		{
			cosTheta2 = bone_x * bone_x + bone_y * bone_y - l1 * l1 - l2 * l2;
			
			tmp1_1 = 0;
			tmp1_0 = 0;
			sinTheta1 = bone_y;
			cosTheta1 = bone_x;
		
			cosTheta2 /= den;
			arg = 1.0f - cosTheta2 * cosTheta2;
			if (arg < 0.0) 
			{
				arg = 0.0;
			}
			sinTheta2 = sqrtf (arg) * constrainSign;

			tmp1_1 = l2 * sinTheta2;
			tmp1_0 = l1 + l2 * cosTheta2;
			sinTheta1 = tmp1_0 * bone_y - tmp1_1 * bone_x;
			cosTheta1 = tmp1_0 * bone_x + tmp1_1 * bone_y;
			bone2_Roll = atan2f (sinTheta2, cosTheta2); 
			
			den = cosTheta1 * cosTheta1 + sinTheta1 * sinTheta1;
			_ASSERTE (den > 0);
			den = 1.0f / sqrtf (den);
			sinTheta1 *= den;
			cosTheta1 *= den;
			
			bone1_Roll = atan2f (sinTheta1, cosTheta1); 
		}


		ZeroMatrix rollIKMatrix2 (RollMatrix(bone2_Roll), bone2Pos);

		ZeroMatrix rollIKMatrix1 (RollMatrix(bone1_Roll));
		ZeroMatrix yawMatrix1  (YawMatrix(yaw));
		ZeroMatrix rollMatrix1 (RollMatrix(roll));

		ZeroMatrix rollYawRollMatrix (rollIKMatrix1 * yawMatrix1 * rollMatrix1);

		ZeroVector4 axis (rollIKMatrix2.TransformVector (effPos));
		axis = rollYawRollMatrix.TransformVector (axis);

		ZeroVector4 v1 (rollYawRollMatrix.TransformVector (testBonePos));
		ZeroVector4 v2	(bone1PitchYawRollMatrix.TransformVector (testBonePos));

		ZeroVector4 v0 (axis.Scale (DotProduct (v1, axis) / DotProduct (axis, axis)));


		ZeroVector4 v1v0 (v1 - v0);
		ZeroVector4 v2v0 (v2 - v0);
		pitch	= 0;
		dist2 = DotProduct (v1v0, v1v0);
		if (dist2 > 1.0e-8) 
		{
			float cosPitch;
			float sinPitch;
//			_ASSERTE (fabsf (DotProduct (v1v0, v1v0) - DotProduct (v2v0, v2v0)) < 0.01);

			ZeroVector4 dir (axis.Scale (1.0f / sqrtf (DotProduct (axis, axis))));

			cosPitch	= DotProduct (v1v0, v2v0) / DotProduct (v1v0, v1v0);
			sinPitch	= DotProduct (CrossProduct (v1v0, v2v0), dir) / DotProduct (v1v0, v1v0);

			pitch	= atan2f (sinPitch, cosPitch);
		}
		
		ZeroMatrix pitchMatrix (PitchMatrix(pitch));
		outRotFrames[i].rotation = ZeroQuaternion(pitchMatrix);
		i ++;
	}
}



void ZeroImportExportNode::CreateVertexIndexStripsAndIndexList ()
{
	int i;
	int segmentsCount;
	ZeroImportExportNode* obj;
	CMSHBlockModelSegment **segments;
	
	if (mshModel) {
		segments = mshModel->GetSegments(segmentsCount);
		for (i = 0; i < segmentsCount; i ++) {
			CreateVertexIndexStripsAndIndexList (segments[i]);
		}
	}
	
	for (obj = GetChild(); obj; obj = obj->GetSibling()) {
		obj->CreateVertexIndexStripsAndIndexList ();
	}
}



void ZeroImportExportNode::CreateVertexIndexStripsAndIndexList (CMSHBlockModelSegment *segment)
{
	if(mshModel->IsClothEnabled())
		segment->CreateClothVertexList();
	else 
		if(mshModel->GetModelType() == Shadow_Volume)
			segment->CreateShadowVolume();
		else
			segment->CreateVertexIndexStripsAndIndexList ();
	
	/*switch (mshModel->GetModelType()) {
		case Cloth_Node:
		{
			segment->CreateClothVertexList();
			break;
		}
		case Shadow_Volume:
		{
 			segment->CreateShadowVolume();
			break;
		}
		default:
		{
			segment->CreateVertexIndexStripsAndIndexList ();
		}
	}*/
}




void ZeroImportExportNode::MarkDeformers(ZeroImportExportNode *root,CZeroData *aData)
{
	int i;
	int tmpCount;
	int segmentsCount;
	ZeroImportExportNode *node;
	
	CMSHBlockModelSegment *segment;
   CMSHBlockModelSegment **segments;

	ZeroStack<CMSHBlockModelSegment*> segmentArray(2048);

	segmentsCount = 0;
	segments = mshModel->GetSegments (tmpCount);
	for (i = 0; i < tmpCount; i ++) {
		segmentArray[segmentsCount] = segments[i];
		segmentsCount ++;
	}


	int nEnvelops;
	int *envelopArray;

	envelopArray=mshModel->GetEnvelopArray(nEnvelops);

	// mark all bones use by the skinning processes
	for (i = 0; i < segmentsCount; i ++) 
	{
		segment = segmentArray[i];
		
		CMSHBlockSegmentEnvelop::Iterator iter(segment->GetVertexWeightsEnvelop());
		for (iter.Begin(); iter; iter ++) {
			CMSHBlockVertexWeightsList::Iterator weightIter ((*iter)->info);
			for (weightIter.Begin(); weightIter; weightIter ++) 
			{
				ZeroMatrixWeightPair& boneWeightPair = *weightIter;
				
				_ASSERTE(boneWeightPair.boneID<nEnvelops);
				node = root->FindByCgcModel ((void*)envelopArray[boneWeightPair.boneID]);

/*
				ZeroImportExportNode *origNode=node;
				if(node!=this)
				{
					//
					//	want to promote weights to animated parent 
					//  -- just in case vertices are weighted to effectors
					bool bDone=false;
					while(node && node!=this && !bDone)
					{
						CMSHBlockModel *tmpModel=node->GetMshModel();
						if(tmpModel)
						{
							if(node->IsInSkeleton(aData))
							{
								bDone=true;
							}
							else if(tmpModel->GetModelType()==Skin_Node)
							{
								bDone=true;
								node=origNode;
							}
							else
							{
								node=node->GetParent();
							}
						}
						else
						{
							node=node->GetParent();
						}
					}
					if(!node)
					{
						node=origNode;
					}
				}
*/
				boneWeightPair.boneID=node->GetMshModel()->GetMatrixPaletteIndex();
				node->bDeformer=true;
			}
		}
	}
}


void ZeroImportExportNode::MarkAllDeformers(ZeroImportExportNode *root,CZeroData *aData)
{
	ZeroImportExportNode *node;
	
	if (mshModel) {
		if (mshModel->GetModelType() == Skin_Node) {
			MarkDeformers(root,aData);
		}
	}
	
	for (node = GetChild(); node; node = node->GetSibling()) {
		node->MarkAllDeformers(root,aData);
	}
}



void ZeroImportExportNode::RemapSkinMatrices1(ZeroImportExportNode *root)
{
	int i;
	int tmpCount;
	int segmentsCount;
	ZeroImportExportNode *node;
	
	CMSHBlockModelSegment *segment;
   CMSHBlockModelSegment **segments;

	ZeroStack<CMSHBlockModelSegment*> segmentArray(2048);

	segmentsCount = 0;
	segments = mshModel->GetSegments (tmpCount);
	for (i = 0; i < tmpCount; i ++) {
		segmentArray[segmentsCount] = segments[i];
		segmentsCount ++;
	}

	// mark all bones use by the skinning processes
	for (i = 0; i < segmentsCount; i ++) {
		segment = segmentArray[i];
		
		CMSHBlockSegmentEnvelop::Iterator iter(segment->GetVertexWeightsEnvelop());
		for (iter.Begin(); iter; iter ++) {
			CMSHBlockVertexWeightsList::Iterator weightIter ((*iter)->info);
			for (weightIter.Begin(); weightIter; weightIter ++) {
				ZeroMatrixWeightPair& boneWeightPair = *weightIter;
				
				node = root->FindByCgcModel ((void*)boneWeightPair.boneID);
				if (node->GetMshModel() != mshModel && boneWeightPair.boneID) {
					node->GetMshModel()->SetModelType(Envelop_Node);
				}
			}
		}
	}
}


void ZeroImportExportNode::RemapAllSkinMatrices1(ZeroImportExportNode *root)
{
	ZeroImportExportNode *node;
	
	if (mshModel) {
		if (mshModel->GetModelType() == Skin_Node) {
			RemapSkinMatrices1(root);
		}
	}
	
	for (node = GetChild(); node; node = node->GetSibling()) {
		node->RemapAllSkinMatrices1(root);
	}
}



void ZeroImportExportNode::RemapSkinMatrices2(ZeroImportExportNode *root)
{
	int i;
	int tmpCount;
	int segmentsCount;
	ZeroImportExportNode *node;
	
	CMSHBlockModelSegment *segment;
   CMSHBlockModelSegment **segments;

	ZeroStack<CMSHBlockModelSegment*> segmentArray(2048);

	segmentsCount = 0;
	segments = mshModel->GetSegments (tmpCount);
	for (i = 0; i < tmpCount; i ++) {
		segmentArray[segmentsCount] = segments[i];
		segmentsCount ++;
	}

	// remap all matrices used for skinning process
	for (i = 0; i < segmentsCount; i ++) {
		segment = segmentArray[i];
		
		CMSHBlockSegmentEnvelop::Iterator iter(segment->GetVertexWeightsEnvelop());
		for (iter.Begin(); iter; iter ++) {
			CMSHBlockVertexWeightsList::Iterator weightIter ((*iter)->info);
			for (weightIter.Begin(); weightIter; weightIter ++) {
				ZeroMatrixWeightPair& boneWeightPair = *weightIter;
				
				if(boneWeightPair.boneID)
				{
					node = root->FindByCgcModel ((void*)boneWeightPair.boneID);
					_ASSERTE (node);
					boneWeightPair.boneID = node->GetMshModel()->GetMatrixPaletteIndex();
				}
			}
		}
	}
}



void ZeroImportExportNode::RemapAllSkinMatrices2(ZeroImportExportNode *root)
{
	ZeroImportExportNode *node;
	
	if (mshModel) {
		if (mshModel->GetModelType() == Skin_Node) {
			RemapSkinMatrices2(root);
		}
	}
	
	for (node = GetChild(); node; node = node->GetSibling()) {
		node->RemapAllSkinMatrices2(root);
	}
}


void ZeroImportExportNode::EnumerateEnvelopMatrices (int& index)
{
	ZeroImportExportNode *obj;
 
	if (mshModel) {
		mshModel->SetMatrixPaletteIndex (index);
	}

	index ++;
	for (obj = GetChild(); obj; obj = obj->GetSibling()) {
		obj->EnumerateEnvelopMatrices (index);
	}
}

void ZeroImportExportNode::CreateEnvelopArrays ()
{

	int i;
	int id; 
	int paletteIndex;
	int segmentsCount;
	CMSHBlockModelSegment *segment;
	CMSHBlockModelSegment **segments;
	int envelopArray[256];

	ZeroVisTree<int, int> bonesTree;

	segments = mshModel->GetSegments (segmentsCount);
	for (i = 0; i < segmentsCount; i ++) {
		segment = segments[i];
		CMSHBlockSegmentEnvelop::Iterator iter(segment->GetVertexWeightsEnvelop());
		for (iter.Begin(); iter; iter ++) {
			CMSHBlockVertexWeightsList::Iterator weightIter ((*iter)->info);
			for (weightIter.Begin(); weightIter; weightIter ++) {
				ZeroMatrixWeightPair& boneWeightPair = *weightIter;
				id = (int)boneWeightPair.boneID;
//				if(id)
//				{
					bonesTree.Insert (id, id);
					boneWeightPair.boneID = -boneWeightPair.boneID;
//				}
//				else
//				{
//					int skinID=mshModel->GetMatrixPaletteIndex();
//					bonesTree.Insert (skinID, id);
//					boneWeightPair.boneID = -mshModel->GetMatrixPaletteIndex();
//				}
			}
		}
	}


	paletteIndex = 0;
	ZeroVisTree<int, int>::Iterator iter(bonesTree);
	for (iter.Begin(); iter; iter	++) {
		id = (*iter)->info;
		envelopArray[paletteIndex] = id;
		id	= -id;
		for (i = 0; i < segmentsCount; i ++) {
			segment = segments[i];
			CMSHBlockSegmentEnvelop::Iterator iter(segment->GetVertexWeightsEnvelop());
			for (iter.Begin(); iter; iter ++) {
				CMSHBlockVertexWeightsList::Iterator weightIter ((*iter)->info);
				for (weightIter.Begin(); weightIter; weightIter ++) {
					ZeroMatrixWeightPair& boneWeightPair = *weightIter;
					if (boneWeightPair.boneID == id) {
						boneWeightPair.boneID = paletteIndex;
					}
				}
			}
		}
		paletteIndex ++;
	}

	mshModel->SetEnvelopArray(paletteIndex, envelopArray);

}


void ZeroImportExportNode::CreateAllEnvelopArrays ()
{
	ZeroImportExportNode *obj;
	for (obj = GetFirst(); obj; obj = obj->GetNext()) {
		if (obj->GetMshModel()) {
			if (obj->GetMshModel()->GetModelType() == Skin_Node) {
				 obj->CreateEnvelopArrays ();
			}
		}
	}
}


/*
void ZeroImportExportNode::PromoteContrainedModels ()
{
	int i;
	int count;
	ZeroImportExportNode *node;
	ZeroImportExportNode *array[1024];

	count = 0;
	for (node = GetFirst(); node; node = node->GetNext()) {
		if (node->mshModel) {
			int constraint;
			constraint = node->mshModel->GetConstraint();
			if (constraint) {
				array[count] = node;
				count ++;
			}
		}
	}

	for (i = 0; i < count; i ++) {
		ZeroImportExportNode *me;
		ZeroImportExportNode *parent;

		me = array[i];
		const char* constraintName = InvertCRC (me->mshModel->GetConstraint());
		for (node = GetFirst(); node; node = node->GetNext()) {
			if (node->mshModel) {
				if (!strcmp (node->mshModel->GetName(), constraintName)) {
					parent = node;

					ZeroMatrix myMatrix (me->GetWorldMatrix(this));
					ZeroMatrix cnsmatrix (parent->GetWorldMatrix(this));
					ZeroMatrix matrix (myMatrix * cnsmatrix.Inverse());

					ZeroQuaternion q (matrix);


					me->mshModel->SetTranslation (matrix.posit.x, matrix.posit.y, matrix.posit.z);
					me->mshModel->SetRotation (q.x, q.y, q.z, q.s);

					me->mshModel->SetConstraint ("");
					me->mshModel->SetParentName(parent->mshModel->GetName());

					me->AddRef();

					me->Detach();

					me->Attach (parent);
					me->Release();
					break;

				}
			}
		}
	}
}
*/

bool ZeroImportExportNode::MergeChildGeometries(CZeroData *data, ZeroImportExportNode *root)
{
	bool bRetVal=false;
	bool bDetach=false;
	CMSHBlockModel *rootModel=root->GetMshModel();
	CMSHBlockModel *thisModel=GetMshModel();

	bool bRecurse=true;
	bool bSkeletonPiece=IsInSkeleton(data);
	if(thisModel)
	{
		if(bSkeletonPiece || strnicmp(thisModel->GetName(),"hp_",3)==0 || thisModel->GetLeaveChildGeometry() || bDeformer || thisModel->GetModelType()==Skin_Node|| thisModel->GetModelType()==Shadow_Volume)
		{
			bRecurse=false;
		}
	}

	ZeroImportExportNode *nextChild=GetChild();
	if(bRecurse)
	{
		while(nextChild)
		{
			if(nextChild->MergeChildGeometries(data, root))
				nextChild=GetChild();
			else
				nextChild=nextChild->GetSibling();
		}
	}

	if(rootModel && thisModel)
	{
		SetMark(1);
		if(thisModel!=rootModel)
		{
			if(bSkeletonPiece || strnicmp(thisModel->GetName(),"hp_",3)==0 || thisModel->GetLeaveChildGeometry() || bDeformer || thisModel->GetModelType()==Skin_Node|| thisModel->GetModelType()==Shadow_Volume|| rootModel->GetModelType()==Shadow_Volume)
			{
				if(strcmpi(thisModel->GetParentName(),rootModel->GetName())!=0)
				{
					ZeroMatrix mat(GetWorldMatrix(root));
/*
					if(bSkeletonPiece)
					{
						CMSHBlockAnimation *anim=data->GetAnimation();
						CMSHBlockNodeAnimation *nodeAnim=anim->GetNodeAnimation(thisModel->GetName());
						if(!nodeAnim->IsIKKeyFrames())
						{
							int keyframes=nodeAnim->GetKeyframesCount();
							ZeroMatrix fixMat=mat*(GetMatrix().Inverse());
							for(int i=0;i<keyframes;i++)
							{
								ZeroMatrix m((*nodeAnim)[i].rotation,(*nodeAnim)[i].position);
								m=fixMat*m;
								(*nodeAnim)[i].rotation=ZeroQuaternion(m);
								(*nodeAnim)[i].position=m.posit;
							}
						}
					}
*/
					ZeroQuaternion q (mat);


					thisModel->SetTranslation (mat.posit.x, mat.posit.y, mat.posit.z);
					thisModel->SetRotation (q.x, q.y, q.z, q.s);
					thisModel->SetParentName(rootModel->GetName());

					AddRef();
					Detach();
					Attach(root);
					Release();
					bRetVal=true;
				}
			}
			else
			{
				int i,nThisSegments;
				CMSHBlockModelSegment **ppThisSegments;
				ppThisSegments = thisModel->GetSegments(nThisSegments);


				if(!thisModel->GetHidden())
				{
					for(i=0;i<nThisSegments;i++)
					{
						ZeroMatrix mat(GetWorldMatrix(root));
						bool bMergedSegment=false;
						int j,nRootSegments;
						
						if(rootModel->GetHidden())
						{
							rootModel->RemoveGeometry();
							rootModel->SetHidden(false);
						}

						CMSHBlockModelSegment **ppRootSegments;
						ppRootSegments = rootModel->GetSegments(nRootSegments);
			
						char thisMaterial[64];
						strcpy(thisMaterial,ppThisSegments[i]->GetMaterialName());

						int nPositions;
						vect3 *pPositions;
						int nNorms;
						vect3 *pNorms;
						
						pPositions = ppThisSegments[i]->GetPositions(nPositions);
						mat.TransformVectors(pPositions,sizeof(vect3),pPositions,sizeof(vect3),nPositions);

						mat.posit = ZeroVector3 (0, 0, 0);
						pNorms = ppThisSegments[i]->GetNormals(nNorms);
						mat.TransformVectors(pNorms,sizeof(vect3),pNorms,sizeof(vect3),nNorms);

						for(j=0;j<nRootSegments;j++)
						{
							if(strcmp(thisMaterial,ppRootSegments[j]->GetMaterialName())==0)
							{
								ppRootSegments[j] ->Merge(ppThisSegments[i]);
								bMergedSegment = true;
								j=nRootSegments;
							}
						}

						if(!bMergedSegment)
						{
							ZeroPolyhedra polyhedra;
							polyhedra.BeginFace();
							CMSHBlockPolygonList::Iterator polyListIter (ppThisSegments[i]->m_pPolygonList);
							for (polyListIter.Begin(); polyListIter; polyListIter ++) 
							{
								int indexCount;
								MSHBLOCKPolygon& polygon = (*polyListIter)->info;
								ZeroStack<int> index(polygon.GetCount());
								
								indexCount = 0;
								MSHBLOCKPolygon::Iterator polyIter (polygon);
								for (polyIter.Begin(); polyIter; polyIter ++) {
									index[indexCount] = *polyIter;
									indexCount ++;
								}
								polyhedra.AddFace (indexCount, &index[0]);
								
							}
							polyhedra.EndFace();

							CMSHBlockModelSegment *segment;
							segment = rootModel->CreateAddSegment();
							segment->SetMaterialName (ppThisSegments[i]->GetMaterialName());


							int n;
							bool hasNormal = ppThisSegments[i]->GetNormals(n) ? true : false;
							bool hasColor = ppThisSegments[i]->GetColors(n) ? true : false;;
							bool hasLight = ppThisSegments[i]->GetLightColors(n) ? true : false;
							bool hasWeight = false;
							bool hasUVs[MAX_TEXTURES_PER_MSH_MATERIAL];
							
							unsigned char r,g,b,a;
							if(ppThisSegments[i]->HasColorBase())
							{
								ppThisSegments[i]->GetColorBase(r,g,b,a);
								segment->SetColorBase(r,g,b,a);
							}
							if(ppThisSegments[i]->HasLightColorBase())
							{
								ppThisSegments[i]->GetLightColorBase(r,g,b,a);
								segment->SetLightColorBase(r,g,b,a);
							}

							for(j=0;j<MAX_TEXTURES_PER_MSH_MATERIAL;j++)
								hasUVs[j] = ppThisSegments[i]->GetUVCoords(n, j) ? true : false;
							
							ZeroStack<MSH_FLATVERTEX> tmpVertex(nPositions);
							ppThisSegments[i]->CopyFlatVertex(&tmpVertex[0]);
							segment->SetVertexData(polyhedra, &tmpVertex[0], hasNormal, hasColor, hasLight, hasWeight, hasUVs);   
						}
					}
				}
				rootModel->SetModelType(Static_Node);
				thisModel->RemoveGeometry();
				data->RemoveModel(thisModel);
				mshModel = NULL;
				bDetach = true;
			}
		}
	}

	if(bDetach)
	{
		Detach();
		bRetVal=true;
	}

	return bRetVal;
}

bool ZeroImportExportNode::HasAnimatedChild(CZeroData *data, ZeroImportExportNode *root)
{
	CMSHBlockSkeleton *skeleton=data->GetSkeleton();
	CMSHBlockModel *nodeModel=GetMshModel();
	if(skeleton->FindEffector(nodeModel))
		return true;

	ZeroImportExportNode *nextChild=GetChild();
	while(nextChild)
	{
		if(nextChild->HasAnimatedChild(data, root))
			return true;
		nextChild=nextChild->GetSibling();
	}
	return false;
}

bool ZeroImportExportNode::IsInSkeleton(CZeroData *data)
{
	CMSHBlockSkeleton *skeleton=data->GetSkeleton();
	CMSHBlockModel *nodeModel=GetMshModel();
	if(!nodeModel)
		return false;

	CMSHBlockSkeleton::ZeroListNode *effNode=skeleton->GetFirst();
	while(effNode)
	{
		CMSHBlockModel *effModel=effNode->info.GetModel();
		if(nodeModel==effModel)
			return true;
		CMSHBlockModel *pParent;
		switch(effNode->info.GetBoneCount())
		{
		case 0:
		case 1:
			if(effModel->GetParentName())
			{
				pParent=data->FindModelByName(effModel->GetParentName());
				if(pParent==nodeModel)
					return true;
			}
			break;
		case 2:
			pParent=data->FindModelByName(effModel->GetParentName());
			if(pParent==nodeModel)
				return true;
			pParent=data->FindModelByName(pParent->GetParentName());
			if(pParent==nodeModel)
				return true;
			break;
		}
		effNode=effNode->GetNext();
	}

	return false;
}

void ZeroImportExportNode::MergeAllChildGeometries(CZeroData *data)
{
	CMSHBlockModel *rootModel=GetMshModel();

	ZeroImportExportNode *nextChild;

	if(rootModel)
	{
		nextChild=GetChild();
		while(nextChild)
		{
			//
			//	if no animations in this branch, then merge
			if(/*!nextChild->HasAnimatedChild(data, this) && */!nextChild->GetMark() && mshModel->GetModelType()!=Skin_Node)
			{
				nextChild->MergeChildGeometries(data, this);
				nextChild=GetChild();
			}
			else
			{
				nextChild=nextChild->GetSibling();
			}
		}
	}

	nextChild=GetChild();
	while(nextChild)
	{
		nextChild->MergeAllChildGeometries(data);
		nextChild=nextChild->GetSibling();
	}
}




void ZeroImportExportNode::ResolveCollisionObjects()
{
	int i;
	int j;
	int vertexCount;
	int segmentsCount;
	CMSHBlockModelSegment **segments;
	


	vertexCount = 0;
	
	MSH_ClothCollisionSphere *testSphereData1=NULL;
	MSH_ClothCollisionCylinder *testCylinderData1=NULL;
	MSH_ClothCollisionCone *testConeData1=NULL;
	MSH_ClothCollisionSphere *testSphereData2=NULL;
	MSH_ClothCollisionCylinder *testCylinderData2=NULL;
	MSH_ClothCollisionCone *testConeData2=NULL;
	
	if (mshModel) 
	{
		mshModel->GetClothCollision(&testSphereData1,&testCylinderData1,&testConeData1);
		mshModel->GetAnimTriggerCollision(&testSphereData2,&testCylinderData2,&testConeData2);
		if(testSphereData1||testCylinderData1||testConeData1||testSphereData2||testCylinderData2||testConeData2)
		{
			segments = mshModel->GetSegments(segmentsCount);
			if(segmentsCount)
			{
				for (i = 0; i < segmentsCount; i ++) {
					int count;
					segments[i]->GetPositions(count);
					vertexCount += count;
				}
				
				ZeroPolyhedra polyhedra;
				ZeroStack<ZeroVector3> vertexPool (vertexCount);
				
				polyhedra.BeginFace();
				vertexCount = 0;
				for (i = 0; i < segmentsCount; i ++) {
					int i0;
					int i1;
					int i2;
					int vCount;
					int fCount;
					vect3* ptr;
					unsigned short *Index;
					
					ptr = segments[i]->GetPositions(vCount);
					for (j = 0; j < vCount; j ++) {
						vertexPool[j + vertexCount].x = ptr[j].fX;
						vertexPool[j + vertexCount].y = ptr[j].fY;
						vertexPool[j + vertexCount].z = ptr[j].fZ;
					}
					
					fCount = segments[i]->GetTriangleList()->m_nTriangleCount;
					Index = segments[i]->GetTriangleList()->m_nTriangleIndices;
					
					for (j = 0; j < fCount; j ++) {
						i0 = Index[j * 3 + 0] + vertexCount; 
						i1 = Index[j * 3 + 1] + vertexCount; 
						i2 = Index[j * 3 + 2] + vertexCount;
						polyhedra.AddFace (i0, i1, i2);
					}
					vertexCount += vCount;
				}
				polyhedra.EndFace();
				
				ZeroSphere sph(polyhedra.CalculateSphere (&vertexPool[0].x, sizeof (ZeroVector3)));
				
				ZeroQuaternion R (sph);
				ZeroVector3 T (sph.posit);
				ZeroVector4 D (sph.size);

				MSH_ClothCollisionSphere *sphereData=NULL;
				MSH_ClothCollisionCylinder *cylinderData=NULL;
				MSH_ClothCollisionCone *coneData=NULL;
				
				mshModel->GetClothCollision(&sphereData,&cylinderData,&coneData);
				if(sphereData)
					CreateCollisionSphere(sph,sphereData);
				if(cylinderData)
					CreateCollisionCylinder(sph,cylinderData);
				if(coneData)
					CreateCollisionCone(sph,coneData,vertexCount,&vertexPool[0]);
				
				sphereData=NULL;
				cylinderData=NULL;
				coneData=NULL;
				
				mshModel->GetAnimTriggerCollision(&sphereData,&cylinderData,&coneData);
				if(sphereData)
					CreateCollisionSphere(sph,sphereData);
				if(cylinderData)
				{
//	local y-axis is now the axis of collision volume cylinders, 
//	and it points towards the bottom
//					CreateCollisionCylinder(sph,cylinderData);
					float minX,minY,minZ,maxX,maxY,maxZ;
					minX=minY=minZ=maxX=maxY=maxZ=0.0;
					for (i = 0; i < segmentsCount; i ++) 
					{
						int count;
						vect3 *ptr;
						ptr = segments[i]->GetPositions(count);
						for(int j=0;j<count;j++)
						{
							if(ptr[j].fX<minX)
								minX = ptr[j].fX;
							if(ptr[j].fY<minY)
								minY = ptr[j].fY;
							if(ptr[j].fZ<minZ)
								minZ = ptr[j].fZ;
							if(ptr[j].fX>maxX)
								maxX = ptr[j].fX;
							if(ptr[j].fY>maxY)
								maxY = ptr[j].fY;
							if(ptr[j].fZ>maxZ)
								maxZ = ptr[j].fZ;
						}
					}
					
					minX=fabsf(minX);
					minZ=fabsf(minZ);
					if(minX>maxX)
						maxX=minX;
					if(minZ>maxZ)
						maxZ=minZ;
					if(maxX>maxZ)
						cylinderData->radius = maxX;
					else
						cylinderData->radius = maxZ;
					cylinderData->height = maxY-minY;
					cylinderData->dir_x = 0.f;
					cylinderData->dir_y = 1.f;
					cylinderData->dir_z = 0.f;
					cylinderData->top_x = 0.f;
					cylinderData->top_y = minY;
					cylinderData->top_z = 0.f;
				}
				if(coneData)
					CreateCollisionCone(sph,coneData,vertexCount,&vertexPool[0]);

				if(sphereData || cylinderData || coneData)
				{
					mshModel->RemoveGeometry();
				}
			}
		}
	}

	ZeroImportExportNode *obj;
	for (obj = GetChild(); obj; obj = obj->GetSibling()) 
	{
		obj->ResolveCollisionObjects();
	}
}


void ZeroImportExportNode::GetModelArray(CMSHBlockModel **array, int& count)
{
	ZeroImportExportNode *node;
	if (mshModel) {
		array[count] = mshModel;
		count ++;
	}

	for (node = GetChild(); node; node = node->GetSibling()) {
		node->GetModelArray(array, count);
	}
}


void ZeroImportExportNode::AsignClothCollisionToModelsAndRemoveClothCollisionGeometry(CZeroData *data)
{
	int i;
	int j;
	int clothCount;
	int collisionCount;
	CMSHBlockModel *model;
	ZeroImportExportNode *node;
	ZeroImportExportNode *nodeCloth[256];
	ZeroImportExportNode *nodeCollisions[512];

	MSH_ClothCollisionCone *testConeData1;
	MSH_ClothCollisionSphere *testSphereData1;
	MSH_ClothCollisionCylinder *testCylinderData1;

	clothCount = 0;
	collisionCount	= 0;

	testConeData1=NULL;
	testSphereData1=NULL;
	testCylinderData1=NULL;
	for (node = GetFirst(); node; node = node->GetNext()) {
		model = node->GetMshModel();
		if (model) {
			if (model->IsClothEnabled()/*model->GetModelType() == Cloth_Node*/) {
				nodeCloth[clothCount] = node;
				clothCount ++;
			} else {
				model->GetClothCollision(&testSphereData1, &testCylinderData1, &testConeData1);

				if (testSphereData1 || testCylinderData1 || testConeData1) {
					nodeCollisions[collisionCount] = node;
					collisionCount ++;
				}
			}
		}
	}

	for (i = 0; i < clothCount; i ++) {
		model = nodeCloth[i]->GetMshModel();
		for (j = 0; j < collisionCount; j ++) {
			node = nodeCollisions[j];
			if (node->GetMshModel()->IsCollsionOwmer (model)) {
				ZeroMatrix matrix (node->GetMatrix());
				model->AddParametricClothCollsion (node->GetMshModel(), matrix, node->GetParent()->GetMshModel());
			}
		}
	}

#ifndef EXPORT_CLOTH_COLLISION_AS_GEOMETRY
	if (collisionCount) {
		int count;
		CMSHBlockModel *modelArray[1024];
		for (j = 0; j < collisionCount; j ++) {
			node = nodeCollisions[j];
			model = node->GetMshModel();
			node->Detach();
			delete model;
		}
		count = 0;
		GetModelArray(modelArray, count);
		data->PackModelArray (modelArray, count);
	}
#endif

}



