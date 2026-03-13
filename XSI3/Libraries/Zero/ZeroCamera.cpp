//********************************************************************
// Direct geometry Hardware independent library
//	By Julio Jerez
// VC: 6.0
//********************************************************************
#include "ZeroTypes.h"
#include "ZeroCamera.h"
#include "ZeroMaterial.h"
#include "ZeroSegment.h"
#include "ZeroVideo.h"
#include "ZeroTextureLight.h"

#if (PLATFORM == PLATFORM_PS2)
// Doing this fixes a rounding error that makes fullscreen wipes
// (e.g. the start of Geo3 with its right-to-left wipe) not cover the
// full screen. If this is the same as other platforms, we get the
// bottom row and rightmost column of pixels unaffected by the wipe.
// - NM 10/1/02
const float VIEWPORT_EDGE_TOLERANCE	= -0.01f;
#else
const float VIEWPORT_EDGE_TOLERANCE	= 0.01f;
#endif

unsigned ZeroCamera::mCurPriority = 0xffffffff;

int ZeroCamera::primitivesDrawn = 0;
ZeroVideo *ZeroCamera::mVideo = NULL;

ZeroColorValue ZeroCamera::mRenderColor(1,1,1,1);
ZeroColorValue gMaterialColorMultiplier(1,1,1,1);
ZeroColor ZeroCamera::mTerrainColor(0,0,0,255);

class ZeroRenderItemPurge: public ZeroVisList<ZeroRenderItem>
{
	public:
	ZeroRenderItemPurge (void)
		: ZeroVisList<ZeroRenderItem> ()
	{
		AllocFirst();
	}

	~ZeroRenderItemPurge ()
	{
		ZeroTrace(("render items %d\n", GetCount()));
	}

	void AllocFirst(void)
	{
		for (int i = 0; i < 512; i ++) {
			Append();
		}
	}

	ZeroListNode *AllocNext(void)
	{
		ZeroListNode *node = Append();
		for (int i = 1; i < 32; i ++) {
			Append();
		}
		return node;
	}

	void Purge(void)
	{
		while (GetCount() > 512)
		{
			Remove(GetLast());
		}
	}
};

static ZeroRenderItemPurge &GetPurgeList()
{
	static ZeroRenderItemPurge list;
	return list;
}

static ZeroRenderItemPurge::Iterator purgeIter(GetPurgeList());

void ZeroCamera::PurgeAll(void)
{
	ZeroRenderItemPurge &items = GetPurgeList();
	items.Purge();
}

ZeroRenderItem& ZeroCamera::GetRenderItem(unsigned priority)
{
	ZeroRenderItem* item;

	if (!purgeIter) {
		ZeroRenderItemPurge& list = GetPurgeList();
		purgeIter.Set(list.AllocNext());
	}

	item = &(*purgeIter);

	GetHeapSort().Push (item,  priority);

	item->mColor = mRenderColor;

	purgeIter ++;
	return *item;
}


ZeroCamera::HeapSort& ZeroCamera::GetHeapSort ()
{
	static HeapSort sortArray;
	return sortArray;
}

ZeroCamera::ZeroCamera (const ZeroCamera &clone)
	:ZeroObject (clone)
{

	memset (&viewVolume, 0, sizeof (viewVolume));
	memset (&viewFrustrum, 0, sizeof (viewFrustrum));
	memset (&viewPyramid, 0, sizeof (viewPyramid));
	memset (&viewport, 0, sizeof (viewport));
	memset (&guardBandViewVolume, 0, sizeof (guardBandViewVolume));
	
	supportGuardBandClipping = false;

#if (SHADOW_TYPE != NO_SHADOWS)
	m_isShadowCamera = false;		// Will get overridden by shadow camera subclasses
	m_receiveShadows = false;

	m_nShadowCams = 0;
	for (int i = 0; i < MAX_SHADOW_CAMS; i++)
		m_shadowCams[i] = NULL;
#endif

	// init clipping rectangle;
	viewport = clone.viewport;

	
	SetZoom (viewport.fov, viewport.nearPlane, viewport.farPlane, viewport.zoomFactor); 
	
	UpdatePosition (0);
}


ZeroCamera::ZeroCamera (
	int vpWidth, 
	int vpHeight, 
	float nearPlane, 
	float farPlane, 
	float fov, 
	float zoomFactor)
	:ZeroObject ("camera"), viewMatrix(), boxMin (0, 0, 0), boxMax(0, 0, 0)
{
	GetHeapSort();

	memset (&viewVolume, 0, sizeof (viewVolume));
	memset (&viewFrustrum, 0, sizeof (viewFrustrum));
	memset (&viewPyramid, 0, sizeof (viewPyramid));
	memset (&viewport, 0, sizeof (viewport));
	memset (&guardBandViewVolume, 0, sizeof (guardBandViewVolume));
	
	supportGuardBandClipping = false;
	aspectRatio = 0.75f;
	pixelAspect = aspectRatio * vpWidth / vpHeight;

#if (SHADOW_TYPE != NO_SHADOWS)
	m_isShadowCamera = false;		// Will get overridden by shadow camera subclasses
	m_receiveShadows = false;

	m_nShadowCams = 0;
	for (int i = 0; i < MAX_SHADOW_CAMS; i++)
		m_shadowCams[i] = NULL;
#endif

	// init clipping rectangle;
	viewport.pane = ZeroRect (0, 0, vpWidth - 1, vpHeight - 1);
	
#if (PLATFORM == PLATFORM_PS2)
	// compensate for scissor region
	viewport.origx   = (float)   (viewport.pane.x1 - viewport.pane.x0 + 1) / 2.0f;
	viewport.origy   = (float)   (viewport.pane.y1 - viewport.pane.y0 + 1) / 2.0f;
#else
	viewport.origx   = (float)   (viewport.pane.x1 + viewport.pane.x0 + 1) / 2.0f;
	viewport.origy   = (float)   (viewport.pane.y1 + viewport.pane.y0 + 1) / 2.0f;
#endif
	viewport.width2  = (float)   (viewport.pane.x1 - viewport.pane.x0 + 1) / 2.0f - VIEWPORT_EDGE_TOLERANCE;
#ifdef _WIN32
	viewport.height2 = (float) -((viewport.pane.y1 - viewport.pane.y0 + 1) / 2.0f - VIEWPORT_EDGE_TOLERANCE);
#else
	viewport.height2 = (float)   (viewport.pane.y1 - viewport.pane.y0 + 1) / 2.0f - VIEWPORT_EDGE_TOLERANCE;
#endif
	
	SetZoom (fov, nearPlane, farPlane, zoomFactor); 
	
	UpdatePosition (0);
}

ZeroCamera::~ZeroCamera ()
{
}

void ZeroCamera::FreeAll()
{
	PurgeAll();
}

void ZeroCamera::SetAspectRatio(float aAspectRatio)
{
	aspectRatio=aAspectRatio;
	pixelAspect = aspectRatio * (viewport.pane.x1-viewport.pane.x0+1) / (viewport.pane.y1-viewport.pane.y0+1);
	SetFrustrum();
}

ZeroUnknown *ZeroCamera::CreateClone (bool isSim)
{
	ZeroAssert ((!IsSim(), "ZeroCamera - sim clone requested\n"));
	return new (isSim) ZeroCamera (*this);
}

void ZeroCamera::CloneFixUp (const ZeroBaseNode &clone)
{
	UpdatePosition(0);
}


void ZeroCamera::SetVideo (ZeroVideo *video)
{
	mVideo = video;
}


void ZeroCamera::AddPrimitivesDrawn(int primitives)
{
	primitivesDrawn += primitives;
}

int ZeroCamera::GetPrimitivesDrawn() const
{
	return primitivesDrawn;
}



void ZeroCamera::SetZoom (
	float fov,
	float nearPlane,
	float farPlane,
	float zoom)
{
	zoom = fabsf (zoom);
	if (zoom < MIN_CAMERA_ZOOM) {
		zoom = MIN_CAMERA_ZOOM;
	}
	if (zoom > MAX_CAMERA_ZOOM) {
		zoom = MAX_CAMERA_ZOOM;
	}
	
	farPlane = fabsf (farPlane);
	nearPlane = fabsf (nearPlane);
	
	// The View angle is constrained to [MIN_fov < Angle < MAX_fov];
	fov = fabsf (fov);
	if (fov < MIN_CAMERA_FOV) {
		fov = MIN_CAMERA_FOV;
	}
	if (fov > MAX_CAMERA_FOV) {
		fov = MAX_CAMERA_FOV;
	}
	
	viewport.fov = fov;
	viewport.farPlane = farPlane;
	viewport.nearPlane = nearPlane;
	viewport.zoomFactor = zoom;
	
#if 1
	SetViewport (viewport.pane.x0, viewport.pane.y0, viewport.pane.x1, viewport.pane.y1);
#else
	int x0 = (int) (viewport.origx - viewport.width2);
	int x1 = (int) (viewport.origx + viewport.width2);
	int y0 = (int) (viewport.origy - viewport.height2);
	int y1 = (int) (viewport.origy + viewport.height2);
	
	SetViewport (x0, y0, x1, y1);
#endif
}


void ZeroCamera::SetViewport (
	int x0,
	int y0,
	int x1,
	int y1)
{
	int tmp;
	
	if (x1 < x0) {
		tmp = x1;
		x1 = x0;
		x0 = tmp;
	}
	
	if (y1 < y0) {
		tmp = y1;
		y1 = y0;
		y0 = tmp;
	}
	
	ZeroRect pane (x0, y0, x1, y1);
	
	// dstanfill -- this clip doesn't make sense to me (why does the new vieport have to be
	// inside the old one?) and was breaking things, so I took it out.
//	if (viewport.pane.Clip (pane) < 0 ) {
//		return;
//	}
	
	SetBufferPane (pane);
	
#if (PLATFORM == PLATFORM_PS2)
	// compensate for scissor region
	viewport.origx   = (float)   (pane.x1 - pane.x0 + 1) / 2.0f;
	viewport.origy   = (float)   (pane.y1 - pane.y0 + 1) / 2.0f;
#else
	viewport.origx   = (float)   (pane.x1 + pane.x0 + 1) / 2.0f;
	viewport.origy   = (float)   (pane.y1 + pane.y0 + 1) / 2.0f;
#endif
	viewport.width2  = (float)   (pane.x1 - pane.x0 + 1) / 2.0f - VIEWPORT_EDGE_TOLERANCE;
	viewport.height2 = (float) -((pane.y1 - pane.y0 + 1) / 2.0f - VIEWPORT_EDGE_TOLERANCE);
	
	SetFrustrum();
}


void ZeroCamera::SetFrustrum ()
{
	float cotAng;
	float invAspect;
	float farPlane;
	float nearPlane;

	cotAng = cosf(viewport.fov * 0.5f) / sinf (viewport.fov * 0.5f);
	invAspect = -viewport.width2 / viewport.height2 / pixelAspect;
	
	viewport.homScaley = cotAng * viewport.zoomFactor;
	viewport.homScalex = viewport.homScaley / invAspect;
	
	farPlane = viewport.farPlane;
	nearPlane = viewport.nearPlane;

#if (PLATFORM != PLATFORM_PS2)	
	viewport.homScalez0 = farPlane / (farPlane - nearPlane);
	viewport.homScalez1 = farPlane * nearPlane / (nearPlane - farPlane);
#else
	// the z values for the ps2 range from -1(nearplane) to 1(farplane)
	viewport.homScalez0 = ( nearPlane + farPlane ) / ( nearPlane - farPlane );
	viewport.homScalez1 = -2.0f * nearPlane * farPlane / ( nearPlane - farPlane );
#endif
	
	projMatrix.right.x = viewport.homScalex;
	projMatrix.up.y	   = viewport.homScaley;
	projMatrix.front.z = viewport.homScalez0;
	projMatrix.front.w = 1.0f;
	projMatrix.posit.z = viewport.homScalez1;
	projMatrix.posit.w = 0.0f;
	
	viewFrustrum[0].x = -nearPlane / viewport.homScalex;
	viewFrustrum[0].y = -nearPlane / viewport.homScaley;
	viewFrustrum[0].z =  nearPlane;
	
	viewFrustrum[1].x =  nearPlane / viewport.homScalex;
	viewFrustrum[1].y = -nearPlane / viewport.homScaley;
	viewFrustrum[1].z =  nearPlane;                      
	
	viewFrustrum[2].x =  nearPlane / viewport.homScalex;
	viewFrustrum[2].y =  nearPlane / viewport.homScaley;
	viewFrustrum[2].z =  nearPlane;                       
	
	viewFrustrum[3].x = -nearPlane / viewport.homScalex;
	viewFrustrum[3].y =  nearPlane / viewport.homScaley;
	viewFrustrum[3].z =  nearPlane;                       
	
	viewFrustrum[4].x = -farPlane / viewport.homScalex;
	viewFrustrum[4].y = -farPlane / viewport.homScaley;
	viewFrustrum[4].z =  farPlane;
	
	viewFrustrum[5].x =  farPlane / viewport.homScalex;
	viewFrustrum[5].y = -farPlane / viewport.homScaley;
	viewFrustrum[5].z =  farPlane;                      
	
	viewFrustrum[6].x =  farPlane / viewport.homScalex;
	viewFrustrum[6].y =  farPlane / viewport.homScaley;
	viewFrustrum[6].z =  farPlane;                       
	
	viewFrustrum[7].x = -farPlane / viewport.homScalex;
	viewFrustrum[7].y =  farPlane / viewport.homScaley;
	viewFrustrum[7].z =  farPlane;                       
	
	ZeroVector3 orig (0.0f, 0.0f, 0.0f);
	viewVolume[0] = Normalize (ZeroPlane (orig, viewFrustrum[7], viewFrustrum[4]));
	viewVolume[1] = Normalize (ZeroPlane (orig, viewFrustrum[5], viewFrustrum[6]));
	viewVolume[2] = Normalize (ZeroPlane (orig, viewFrustrum[4], viewFrustrum[5]));
	viewVolume[3] = Normalize (ZeroPlane (orig, viewFrustrum[6], viewFrustrum[7]));
	viewVolume[4] = Normalize (ZeroPlane (viewFrustrum[4], viewFrustrum[6], viewFrustrum[5]));
	viewVolume[5] = Normalize (ZeroPlane (viewFrustrum[0], viewFrustrum[1], viewFrustrum[2]));

	SetProjectionMatrix ();
}


void ZeroCamera::UpdatePosition (float timeStep)
{
	ZeroMatrix mat (GetWorldMatrix());
	
	viewPyramid[0] = mat.posit;
//	mat.Transform (&viewPyramid[1], &viewFrustrum[4], 4);

	mat.TransformVectors (&viewPyramid[1], sizeof(ZeroVector3), &viewFrustrum[4], sizeof(ZeroVector3), 4);
	viewMatrix = mat.Inverse();
	GetMinMax (boxMin, boxMax, &viewPyramid[0].x, sizeof (ZeroVector3), 5);
}


void ZeroCamera::BeginScene()
{
	mCurPriority = 0xffffffff;
	purgeIter.Begin();
	GetHeapSort().Flush();
	
	ZeroTxLight::SetCamera(this);
}


void ZeroCamera::EndScene()
{
	Flush();
}



void ZeroCamera::RenderItem(const ZeroRenderItem& aItem, unsigned aPriority)
{
	if(aPriority >> SORT_MASK_IN_BITS != mCurPriority >> SORT_MASK_IN_BITS) {
		mCurPriority = aPriority;
		SetCommonRenderStates(mCurPriority, aItem);
	}

	if(aItem.segment)
	{
		if (aItem.fnt) {
			aItem.fnt (this, aItem);
		}
		//salah: i moved this codes to the platform dependant segment, lights should be set by the 
		//segment not by the camera

		//ZeroSphere sphere(aItem.worldMatrix);
		//sphere.size = aItem.segment->GetSphere().size;
		//mVideo->SetActiveLights(sphere);
		
		//ZeroTxLight::SetActive(&sphere);

		gMaterialColorMultiplier = aItem.mColor;
		mTerrainColor = aItem.mTerrainColor;
		aItem.segment->Render(this, aItem);
		gMaterialColorMultiplier.r = gMaterialColorMultiplier.g = gMaterialColorMultiplier.b = gMaterialColorMultiplier.a = 1.0f;
	}
}



void ZeroCamera::Flush()
{
	mCurPriority = 0xffffffff;
	purgeIter.Begin();

	unsigned priority;

	HeapSort& heapSort = GetHeapSort ();
	while (heapSort.GetCount())  {
		priority = heapSort.Value();
		const ZeroRenderItem& item = *heapSort[0];
		RenderItem(item, priority);
		heapSort.Pop();
	}
}


#if (PLATFORM == PLATFORM_PS2)
void ZeroCamera::FlushSolid()
{
	unsigned priority;

	HeapSort& heapSort = GetHeapSort ();
	while (heapSort.GetCount())  
	{
		priority = heapSort.Value();

		//Break when not transparent stuff anymore
		if((priority & 0xE0000000) == 0x20000000)
			return;

		const ZeroRenderItem& item = *heapSort[0];
		

		RenderItem(item, priority);
		heapSort.Pop();
	}
}
#else
void ZeroCamera::FlushSolid()
{
	unsigned priority;
	unsigned solidMask = GetSolidMask();

	HeapSort& heapSort = GetHeapSort ();
	while (heapSort.GetCount())  
	{
		priority = heapSort.Value();

		//Break when not transparent stuff anymore
		if(!(priority & solidMask))
			return;

		const ZeroRenderItem& item = *heapSort[0];
		RenderItem(item, priority);
		heapSort.Pop();
	}
}
#endif

unsigned ZeroCamera::GetSolidMask()
{
	return 1 << 26;
}


// returns whether or not a point (in world space) is inside the view frustrum of the camera
bool ZeroCamera::PointInFrustrum(ZeroVector3 &pt)
{

	ZeroVector3 camPt = viewMatrix.TransformVector(pt);
	
	if ((camPt.z < viewport.nearPlane) ||
		(camPt.z > viewport.farPlane)  ||
		(PlaneDist(viewVolume[0], camPt) < 0.0f) ||
		(PlaneDist(viewVolume[1], camPt) < 0.0f) ||
		(PlaneDist(viewVolume[2], camPt) < 0.0f) ||
		(PlaneDist(viewVolume[3], camPt) < 0.0f))
	{
		return false;
	}
	
	return true;
	
}
