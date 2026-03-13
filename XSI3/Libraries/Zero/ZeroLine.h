#ifndef  __ZeroLine__
#define  __ZeroLine__

#include "ZeroTypes.h"
#include "ZeroGeoTypes.h"

class ZeroCamera;

DLL_DECLSPEC void WorldLine (ZeroCamera *camera,
							 const ZeroColor color,
							 const ZeroVector3 &p0,
							 const ZeroVector3 &p1);
DLL_DECLSPEC void WorldCLine (ZeroCamera *camera,
							  const ZeroColor color0,
							  const ZeroColor color1,
							  const ZeroVector3 &p0,
							  const ZeroVector3 &p1);
DLL_DECLSPEC void ClippedLine (ZeroCamera *camera,
							   const ZeroColor color,
							   const ZeroPoint<float> &p0,
							   const ZeroPoint<float> &p1,
							   const float z);
DLL_DECLSPEC void GraphicLine (ZeroCamera *camera,
							   const ZeroColor color,
							   const ZeroPoint<float> &p0,
							   const ZeroPoint<float> &p1,
							   const float z);
DLL_DECLSPEC void GraphicCircle (ZeroCamera *camera,
								 const ZeroColor color,
								 const ZeroPoint<float> &p0,
								 const float radius,
								 const float z);
DLL_DECLSPEC void GraphicDiamond (ZeroCamera *camera,
								  const ZeroColor color,
								  const ZeroPoint<float> &p0,
								  const float radius,
								  const float z);
DLL_DECLSPEC void ClippedRectFilled (ZeroCamera *camera,
									 const ZeroColor color,
									 const ZeroArea<float> &rect,
									 const float z);
DLL_DECLSPEC void GraphicRectFilled (ZeroCamera *camera,
									 const ZeroColor color,
									 const ZeroArea<float> &rect,
									 const float z);
DLL_DECLSPEC void ClippedRectEmpty (ZeroCamera *camera,
									const ZeroColor color,
									const ZeroArea<float> &rect,
									const float z);
DLL_DECLSPEC void GraphicRectEmpty (ZeroCamera *camera,
									const ZeroColor color,
									const ZeroArea<float> &rect,
									const float z);

#endif
