#include "ZeroTypes.h"
#include "ZeroSegment.h"
#include "ZeroMaterial.h"
#include "ZeroProceduralGeometry.h"


ZERO_RTTI_CONTRUCTOR(ZeroProceduralGeometry);							  

ZeroProceduralGeometry::ZeroProceduralGeometry()
	:ZeroGeometry("")
{
	sharedGeometry = NULL;
}


ZeroProceduralGeometry::ZeroProceduralGeometry(const ZeroGeometryData& data)
	:ZeroGeometry(data)
{
	sharedGeometry = NULL;
	if (data.dataGeometry) {
		if (data.dataGeometry->GetSegments()->GetCount()) {
			sharedGeometry = data.dataGeometry;
			sharedGeometry->AddRef();
		}
	}
}


ZeroProceduralGeometry::ZeroProceduralGeometry(const ZeroProceduralGeometry& clone)
	:ZeroGeometry(clone)
{
	sharedGeometry = clone.sharedGeometry;
	if (sharedGeometry) {
		sharedGeometry->AddRef();
	}
}


ZeroProceduralGeometry::~ZeroProceduralGeometry()
{
	if(sharedGeometry) {
		sharedGeometry->Release();
	}
}

ZeroUnknown *ZeroProceduralGeometry::CreateClone (bool isSim)
{
	return new (isSim) ZeroProceduralGeometry (*this);
}


void ZeroProceduralGeometry::CloneFixUp (const ZeroObject& myOwner, const ZeroObject& myClone)
{
	ZeroSegment* segment;
	ZeroMaterial* material;

	if (sharedGeometry && (sharedGeometry->GetSegments())) {
		// for each segment...
		ZeroSegmentList::Iterator iter(*(sharedGeometry->GetSegments()));
		for (iter.Begin(); iter; iter ++) {
			segment = *iter;
			material = segment->GetMaterial();
			if(material) {
				material->solidPiece = 1;
			}
		}
	}
}


void ZeroProceduralGeometry::Render(
		const ZeroObject *owner, 
		ZeroCamera *camera, 
		const ZeroMatrix& worldMatrix, 
		unsigned flags) const
{
	if(sharedGeometry) {
		sharedGeometry->Render(owner, camera, worldMatrix, flags);
	}
}


