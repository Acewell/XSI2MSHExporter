/****************************************************************************
*
*  Visual C++ 4.0 base by Julio Jerez
*
****************************************************************************/
#ifndef __ZeroCamera__
#define __ZeroCamera__


#include "ZeroHeap.h"
#include "ZeroObject.h"
#include "ZeroGeoTypes.h"

const int SORT_MASK_IN_BITS = 24;
const int MAX_TRANS_SEGMENTS = 1024;

const float MIN_CAMERA_ZOOM = 0.25f;
const float MAX_CAMERA_ZOOM = 8.0f;
const float MIN_CAMERA_FOV	= 0.0001f;	// Prevent mathematical errors, but otherwise don't limit
const float MAX_CAMERA_FOV	= 0.999f * PI;

class ZeroSegment;
class ZeroSurface;
class ZeroRenderItem;
class ZeroRenderState;
class ZeroSurfaceStage;
class ZeroRenderDescriptor; 

typedef void (*ZeroSegmentShaderCallback) (ZeroCamera* camera, const ZeroRenderItem& item);
typedef void (*ZeroSegmentShaderCustomParametersCallback) (ZeroCamera* camera, ZeroRenderItem& item);


class ZeroRenderItem
{
	public:
	ZeroMatrix worldMatrix;
	ZeroSegment *segment;
	const ZeroObject *owner;
	ZeroSegmentShaderCallback fnt;
	unsigned appParameter;
	ZeroColorValue mColor;
	ZeroColor mTerrainColor;
	unsigned int flags;		// For passing to render descriptor
	
	ZeroRenderItem() : flags(0)
	{
	}

	~ZeroRenderItem()
	{
	}
};






class ZeroCamera: public ZeroObject
{
//	ZERORTTI(ZeroCamera);
	friend class ZeroVideo;

public:
	class HeapSort: public ZeroHeap<ZeroRenderItem*, unsigned>
	{
		public:
		HeapSort	()
			:ZeroHeap<ZeroRenderItem*, unsigned> (1024 * 4, 0xffffffff)
		{
		}
	};

	ZeroRenderItem &	GetRenderItem(unsigned priority);
	static ZeroVideo *	GetVideo()			{ return mVideo; }

	static ZeroColorValue mRenderColor;
	static ZeroColor mTerrainColor;        // color of the terrain under the object being rendered

private:
	static ZeroVideo *mVideo;
	HeapSort& GetHeapSort ();

	static void		PurgeAll(void);
	static void		SetVideo (ZeroVideo *video); 

protected:
	ZeroMatrix		viewMatrix;          // Camera Transformation ZeroMatrix
	ZeroMatrix		projMatrix;
	ZeroPlane		viewVolume [6];
	ZeroVector3		viewPyramid [5];
	ZeroVector3		viewFrustrum [8];
	ZeroPlane		guardBandViewVolume [6];
	ZeroVector3		boxMin;
	ZeroVector3		boxMax;
	static int		primitivesDrawn;
	static unsigned	mCurPriority;
	float			pixelAspect;
	float			aspectRatio;
	bool			supportGuardBandClipping;

#if (SHADOW_TYPE == PROJECTED_SHADOWS)
    #define MAX_SHADOW_CAMS 25
#elif (SHADOW_TYPE == Z_BUFFER_SHADOWS)
    #define MAX_SHADOW_CAMS 2
#endif

#if (SHADOW_TYPE != NO_SHADOWS)
	// Note: m_receiveShadows is used differently for the different shadow camera types;
	// for z-buffer shadows, it just enables the drawing of shadows. For projected shadows,
	// it indicates the object being drawn should receive static shadows (but not others)
	int				m_nShadowCams;					// Number of shadow cameras for shadow overlay
	ZeroCamera*		m_shadowCams[MAX_SHADOW_CAMS];	// Ptrs to associated shadow cameras for non-shadow camera
	bool			m_isShadowCamera : 1;			// True if this is a shadow camera
	bool			m_staticShadow : 1;				// True if this shadow camera contains the shadow of a static object
	bool			m_receiveShadows : 1;			// True if we should receive shadows
#endif

	DLL_DECLSPEC ZeroCamera(const ZeroCamera &clone);
	DLL_DECLSPEC ZeroCamera(int vpWidth, int vpHeight, float nearPlane, float farPlane, float fov, float zoomFactor);
	DLL_DECLSPEC ~ZeroCamera();

	DLL_DECLSPEC ZeroUnknown *	CreateClone(bool isSim);
	DLL_DECLSPEC void			CloneFixUp(const ZeroBaseNode &clone);

	DLL_DECLSPEC virtual void	SetFrustrum();

	virtual void				SetBufferPane(const ZeroRect& rect)		{}
	virtual unsigned GetSolidMask();

public:
	ZeroViewport			viewport;

	virtual void			SetCommonRenderStates(unsigned priority, const ZeroRenderItem& item)	{ _ASSERTE (0); }
	virtual void			BeginShadows()	{ }
	virtual void			EndShadows()	{ }
	const ZeroMatrix &		GetViewMatrix() const													{ return viewMatrix; }
	const ZeroMatrix &		GetProjMatrix() const				{ return projMatrix; }
	const ZeroPlane *		GetGuadBandViewVolume() const		{ return supportGuardBandClipping ? guardBandViewVolume: NULL; }
	const ZeroPlane *		GetViewVolume() const				{ return viewVolume; }
	const ZeroVector3 *		GetViewPyramid() const				{ return viewPyramid; }
	const ZeroVector3 *		GetViewFrustrum() const				{ return viewFrustrum;}
	const ZeroViewport &	GetViewport() const					{ return viewport; }
	void					GetCameraBox (ZeroVector3 &min, ZeroVector3 &max) const	{ min = boxMin; max = boxMax; }
	float					GetAspectRatio()					{ return aspectRatio; }

#if (SHADOW_TYPE != NO_SHADOWS)
	DLL_DECLSPEC bool		IsShadowCamera(void) const {return m_isShadowCamera;}
	DLL_DECLSPEC bool		EnableShadows(bool enable) {bool e = m_receiveShadows; m_receiveShadows = enable; return e;}
	DLL_DECLSPEC bool		SetStaticShadow(bool stat) {bool s = m_staticShadow; m_staticShadow = stat; return s;}
	DLL_DECLSPEC bool		IsStaticShadow(void) const {return m_staticShadow;}
	DLL_DECLSPEC void		AddShadowCamera(ZeroCamera* c) { if (m_nShadowCams < MAX_SHADOW_CAMS) m_shadowCams[m_nShadowCams++] = c;}
	DLL_DECLSPEC void		ClearShadows(void) {m_nShadowCams = 0;}
#endif

	DLL_DECLSPEC void		AddPrimitivesDrawn (int primitives);
	DLL_DECLSPEC int		GetPrimitivesDrawn () const;

	DLL_DECLSPEC void		UpdatePosition (float timeStep);
#ifdef WIN32
	DLL_DECLSPEC void		SetZoom (float fov, float nearPlane, float farPlane, float zoomFactor); 
#else
	DLL_DECLSPEC virtual void	SetZoom (float fov, float nearPlane, float farPlane, float zoomFactor); 
#endif
	DLL_DECLSPEC virtual void	SetViewport (int x0 = -1, int y0 = -1, int x1 = 0xffff, int y1 = 0xffff);

	virtual void			FreeAll();

	virtual void			BeginScene();
	virtual void			EndScene();
	virtual void			DrawFog() {}


	void					SetAspectRatio(float aAspectRatio);
	virtual void			SetProjectionMatrix()											{}
	virtual void			SetViewMatrix()													{}
	virtual void			SetWorldMatrix (int count, const ZeroMatrix **matrices)			{}

	virtual void			Lock (ZeroRenderDescriptor &aDesc, bool aDynamic)				{}
	virtual void			Unlock (ZeroRenderDescriptor &aDesc)							{}
	virtual void			Render (const ZeroRenderDescriptor &desc)						{}

	virtual void			SetRenderStates (ZeroRenderState* renderStates)					{}
	virtual void			SetTextureStages (int surfaceStage, ZeroSurface* surface, ZeroSurfaceStage* surfaceStages, bool custom=false)	{}

	virtual void			SetTextureMatrix (int surfaceStage, const ZeroMatrix &matrix) const		{}

	virtual ZeroUnknown *	GetDynamicBuffer(const ZeroRenderDescriptor &desc)				{ return NULL; }
	virtual ZeroUnknown *	GetStaticBuffer(const ZeroRenderDescriptor &desc)				{ return NULL; }
	virtual ZeroUnknown *	CreateVertexBuffer (const ZeroRenderDescriptor &desc)			{ return NULL; }
	virtual void			AddToStaticVertexBuffer(const ZeroRenderDescriptor &desc,
													 unsigned* indexStart, unsigned* vertexStart)		{}

	virtual void RenderItem(const ZeroRenderItem& aItem, unsigned aPriority);
	virtual void Flush();
	virtual void FlushSolid();
	
	// returns whether or not a point (in world space) is inside the view frustrum of the camera
	virtual bool PointInFrustrum(ZeroVector3 &pt);
	
	virtual bool NeedScreenCapture(void) { return false; }
	
};


#endif
