#include "ZeroTypes.h"
#include "ZeroStack.h"
#include "ZeroObject.h"
#include "ZeroSegment.h"
#include "ZeroMaterial.h"
#include "ZeroSkinGeometry.h"


ZERO_RTTI_CONTRUCTOR(ZeroSkinGeometry);							  


static void SetMatrixPalette (ZeroCamera* camera, const ZeroRenderItem& context)
{
	ZeroSkinGeometry *geo;
	geo = (ZeroSkinGeometry *) context.owner->GetGeometry();
	const ZeroMatrix *matrix;
	matrix = geo->GetMatrixPalette();
	camera->SetWorldMatrix (geo->GetMatrixPaletteCount(), &matrix);
}

class ZeroBasePoseMatrixArray: public ZeroUnknown
{
	public:
	ZeroMatrix *basePose;

	ZeroBasePoseMatrixArray (int size) 
		:ZeroUnknown ()
	{
		basePose	= ZeroNew (ZeroMatrix[size]);
	}

	~ZeroBasePoseMatrixArray	()
	{
		delete[] basePose;
	}
};


ZeroSkinGeometry::ZeroSkinGeometry(const ZeroGeometryData& data)
	:ZeroTransformContraintGeometry(data)
{
	mPaletteCount = 0;
	mEnvelops = NULL;
	basePoseArray = NULL;
   mMatrixPalette = NULL;
	sharedGeometry->SetSegmentShaderFunction (SetMatrixPalette);

	if (data.envelopsCount) {
	   mPaletteCount = data.envelopsCount;
		mMatrixPalette = ZeroNew (ZeroMatrix[mPaletteCount]);

	   mEnvelops = ZeroNew (ZeroTransformContraintGeometry*[mPaletteCount]);
		memcpy (&mEnvelops[0], data.envelopsMap, mPaletteCount * sizeof (ZeroGeometry*)); 

		basePoseArray = new (false) ZeroBasePoseMatrixArray (data.envelopsCount);
	}
}

	
ZeroSkinGeometry::ZeroSkinGeometry(const ZeroSkinGeometry& clone)
	:ZeroTransformContraintGeometry(clone)
{
   mPaletteCount = 0;
	mEnvelops = NULL;
   mMatrixPalette = NULL;
	basePoseArray = NULL;
	if (clone.basePoseArray) {
		basePoseArray = clone.basePoseArray;
		basePoseArray->AddRef();
	}
	sharedGeometry->SetSegmentShaderFunction (SetMatrixPalette);
}


ZeroSkinGeometry::~ZeroSkinGeometry()
{
	if (basePoseArray) {
		basePoseArray->Release();
	}

	if (mMatrixPalette) {
		delete[] mMatrixPalette;
	}

	if (mEnvelops) {
		delete[] mEnvelops;
	}
}

void ZeroSkinGeometry::Scale (float scale)
{
	int i;
	ZeroMatrix *basePoses;

	if (basePoseArray) {
		basePoses = basePoseArray->basePose;
		for (i = 0; i < mPaletteCount; i ++) {
			basePoses[i].posit = basePoses[i].posit.Scale (scale);
		}
	}
   ZeroTransformContraintGeometry::Scale (scale); 
}


ZeroUnknown *ZeroSkinGeometry::CreateClone (bool isSim)
{
	return new (isSim) ZeroSkinGeometry (*this);
}

void ZeroSkinGeometry::CloneFixUp (const ZeroObject& myOwner, const ZeroObject& myCloneObj)
{
	int index;
	const ZeroObject *myRoot;
	const ZeroObject *cloneRoot;

	myRoot = myOwner.GetRoot();
	cloneRoot = myCloneObj.GetRoot();

	index = 0;
	ZeroTransformContraintGeometry::CloneFixUp (myOwner, myCloneObj);
	const ZeroSkinGeometry& myClone = *((ZeroSkinGeometry*) myCloneObj.GetGeometry());

   mPaletteCount = myClone.mPaletteCount;
   mMatrixPalette = ZeroNew (ZeroMatrix[mPaletteCount]);

   mEnvelops = ZeroNew (ZeroTransformContraintGeometry*[mPaletteCount]);
	memset (mEnvelops, 0, sizeof (ZeroTransformContraintGeometry*) * mPaletteCount);
	CloneCopyEnvlops (myRoot, cloneRoot, index, myClone.mEnvelops);
}


void ZeroSkinGeometry::CloneCopyEnvlops (
	const ZeroObject* myOwner, 
	const ZeroObject* myClone, 
	int& index,
	const ZeroTransformContraintGeometry* const cloneEnvelopsArray[])
{
	const ZeroObject* myOwnerPtr; 
	const ZeroObject* myClonePtr; 
	const ZeroGeometry *cloneGeometry;

	cloneGeometry = myClone->GetGeometry();

	if (index <	mPaletteCount) {
		if (cloneGeometry == cloneEnvelopsArray[index]) {
			mEnvelops[index] = (ZeroTransformContraintGeometry*) myOwner->GetGeometry();
			index	++;
		}
	}

	_ASSERTE (index <= mPaletteCount);
	myOwnerPtr = myOwner->GetChild();
	myClonePtr = myClone->GetChild();
	while (myClonePtr) {
		CloneCopyEnvlops (myOwnerPtr, myClonePtr, index, cloneEnvelopsArray);
		myOwnerPtr = myOwnerPtr->GetSibling();
		myClonePtr = myClonePtr->GetSibling();
	}
}



void ZeroSkinGeometry::RemapEnvelops(ZeroObject *skinObject)
{
	int i;
	int index;
	ZeroObject *root;
	ZeroObject *envObj;
	ZeroMatrix *basePoses;
	ZeroTransformContraintGeometry *env;
	ZeroStack<const ZeroObject*> envelops(2048);

	if (!mEnvelops) {
		return;
	}

	root = skinObject->GetRoot();

	index	= EnumerateByType(root, &envelops[0], ZeroTransformContraintGeometry::GetRttiTypeID());

	basePoses = basePoseArray->basePose;
	for (i = 0; i < mPaletteCount; i ++) {
		int envelopNumber;
		envelopNumber = (int) mEnvelops[i];

		envObj = (ZeroObject*) envelops[envelopNumber];
		env = (ZeroTransformContraintGeometry *) envObj->GetGeometry();
		_ASSERTE (env->IsTypeID(ZeroTransformContraintGeometry::GetRttiTypeID()));

		mEnvelops[i] = env;
		basePoses[i] = envObj->GetWorldMatrix(skinObject).Inverse();
	}
}


void ZeroSkinGeometry::Render (
		const ZeroObject *owner, 
		ZeroCamera *camera, 
		const ZeroMatrix &worldMatrix, 
		unsigned flags) const
{
	int i;
	int clipFlag;
	ZeroMatrix *basePoses;
	
   // Test geometry sphere
	clipFlag = sharedGeometry->TrivialRejection (camera, worldMatrix, flags);
	if (clipFlag < 0) {
		return;
	}

	//	now, set all the matrix palette entries that effect this skin
	basePoses = basePoseArray->basePose;
	for (i = 0; i < mPaletteCount; i ++) {
		ZeroMatrixMultiply(mMatrixPalette[i], basePoses[i], mEnvelops[i]->GetWorldMatrix()); 
	}

	// bypass ZeroTransformContraintGeometry	render function
	ZeroProceduralGeometry::Render (owner,  camera, worldMatrix, flags);
}


