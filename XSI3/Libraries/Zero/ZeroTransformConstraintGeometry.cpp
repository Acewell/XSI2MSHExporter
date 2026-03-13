#include "ZeroTypes.h"
#include "ZeroStack.h"
#include "ZeroObject.h"
#include "ZeroTransformConstraintGeometry.h"


ZERO_RTTI_CONTRUCTOR(ZeroTransformContraintGeometry);							  


ZeroTransformContraintGeometry::ZeroTransformContraintGeometry()
	:ZeroProceduralGeometry()
{
}

ZeroTransformContraintGeometry::ZeroTransformContraintGeometry(const ZeroGeometryData& data)
	:ZeroProceduralGeometry(data)
{
}


ZeroTransformContraintGeometry::ZeroTransformContraintGeometry(const ZeroTransformContraintGeometry& clone)
	:ZeroProceduralGeometry(clone)
{
	worldMatrix = clone.worldMatrix;
}


ZeroTransformContraintGeometry::~ZeroTransformContraintGeometry()
{
}


ZeroUnknown *ZeroTransformContraintGeometry::CreateClone (bool isSim)
{
	return new (isSim) ZeroTransformContraintGeometry (*this);
}


void ZeroTransformContraintGeometry::Render(
		const ZeroObject *owner, 
		ZeroCamera *camera, 
		const ZeroMatrix& worldMatrixArg, 
		unsigned flags) const
{
	worldMatrix = worldMatrixArg;
	ZeroProceduralGeometry::Render (owner, camera, worldMatrix, flags);
}


