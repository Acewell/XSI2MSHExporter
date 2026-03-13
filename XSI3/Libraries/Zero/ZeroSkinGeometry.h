/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroSkinGeometry__
#define __ZeroSkinGeometry__

#include "ZeroRtti.h"
#include "ZeroTypes.h"
#include "ZeroUnknown.h"
#include "ZeroMatrix.h"
#include "ZeroSphere.h"
#include "ZeroGeoTypes.h"
#include "ZeroTransformConstraintGeometry.h"


class ZeroBasePoseMatrixArray;

class ZeroSkinGeometry: public ZeroTransformContraintGeometry
{	
	protected:
   int mPaletteCount;
   ZeroMatrix *mMatrixPalette;
	ZeroBasePoseMatrixArray	*basePoseArray;
   ZeroTransformContraintGeometry** mEnvelops;

	ZERO_RTTI(ZeroTransformContraintGeometry);

	void CloneCopyEnvlops (const ZeroObject* myOwner, const ZeroObject* myClone, int& index, const ZeroTransformContraintGeometry* const mEnvelops[]);
	DLL_DECLSPEC ZeroSkinGeometry(const ZeroSkinGeometry& aGeometry);

	DLL_DECLSPEC ~ZeroSkinGeometry();
	virtual void CloneFixUp (const ZeroObject& myOwner, const ZeroObject& myClone);

	public:
	ZeroSkinGeometry(const ZeroGeometryData& data); 
	ZeroUnknown *CreateClone (bool isSim);

	void Scale (float scale);

	int GetMatrixPaletteCount() const
	{
		return mPaletteCount;
	}

	ZeroMatrix* GetMatrixPalette() const
	{
		return mMatrixPalette;
	}

	void RemapEnvelops(ZeroObject *owner); 
	virtual void Render (const ZeroObject *owner,  ZeroCamera *camera,  const ZeroMatrix &world, unsigned flags = 0) const;
};




#endif
