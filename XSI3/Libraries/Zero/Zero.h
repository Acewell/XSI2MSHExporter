/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __Zero__
#define __Zero__

// core stuff
#include "ZeroTypes.h"	// Should be included first (for ZeroPlatform.h, ApplicationSpecific.h, etc)  /Smedis

#include "ZeroCrc.h"
#include "ZeroFsm.h"
#include "ZeroRef.h"
#include "ZeroAABB.h"
#include "ZeroLine.h"
#include "ZeroTime.h"
#include "ZeroNode.h"
#include "ZeroSets.h"
#include "ZeroHeap.h"
#include "ZeroTree.h"
#include "ZeroList.h"
#include "ZeroArray.h"
#include "ZeroQueue.h"
#include "ZeroGraph.h"
#include "ZeroDebug.h"
#include "ZeroStack.h"
#include "ZeroCache.h"
#include "ZeroPlane.h"
#include "ZeroSpline.h"
#include "ZeroMemory.h"
#include "ZeroInputs.h"
#include "ZeroRandom.h"
#include "ZeroMatrix.h"
#include "ZeroSphere.h"
#include "ZeroRecord.h"
#include "ZeroObject.h"
#include "ZeroEntity.h"
#include "ZeroFilters.h"
#include "ZeroUnknown.h"
#include "ZeroVector3.h"
#include "ZeroVector4.h"
#include "ZeroInterval.h"
#include "ZeroGeometry.h"
#include "ZeroResource.h"
#include "ZeroPolyhedra.h"
#include "ZeroTransform.h"
#include "ZeroCollision.h"
#include "ZeroRigidBody.h"
#include "ZeroMathTypes.h"
#include "ZeroPolyhedra.h"
#include "ZeroPolySoups.h"
//#include "ZeroPathFinder.h"
#include "ZeroQuaternion.h"
#include "ZeroVirtualMem.h"
#include "ZeroMeshContext.h"
#include "ZeroBspCollision.h"
#include "ZeroPolyCollision.h"
#include "ZeroClothGeometry.h"
#include "ZeroIntersections.h"
#include "ZeroCullingSphere.h"
#include "ZeroVectorQuantize.h"
#include "ZeroSphereCollision.h"
#include "ZeroProceduralGeometry.h"
#include "ZeroInfinityMassPhysics.h"
#include "ZeroTransformConstraintGeometry.h"

// graphic stuff
#include "Bmp.h"
#include "Pic.h"
#if (PLATFORM == PLATFORM_PS2)
#include "Tga_ps2.h"
#else
#include "Tga.h"
#endif
#include "Msh.h"
#include "ZeroVideo.h"
#include "ZeroLight.h"
#include "ZeroCamera.h"
#include "ZeroPixels.h"
#include "ZeroGeoUtil.h"
#include "ZeroSegment.h"
#include "ZeroSurface.h"
#include "ZeroGeoTypes.h"
#include "ZeroMaterial.h"
#include "ZeroRenderState.h"
#include "ZeroSurfaceState.h"
#include "ZeroCameraSphere.h"
#include "ZeroMeshGeometry.h"
#include "ZeroRenderDescriptor.h"
#include "ZeroTextureLight.h"

#endif
