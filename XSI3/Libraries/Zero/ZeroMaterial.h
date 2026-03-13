/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroMaterial__
#define __ZeroMaterial__

#include "ZeroCamera.h"
#include "ZeroSurface.h"
#include "ZeroRenderState.h"
#include "ZeroSurfaceState.h"


struct ZeroMaterialDesc
{
	unsigned int	crc;
	unsigned int	attrib;
	int				nTextures;
	ZeroSurface		*pTextures[MAX_SURFACE_STAGES];

	ZeroMaterialDesc( unsigned int crcName, unsigned int Attrib=0, ZeroSurface *pTextures[]=0, int nTextures=0 )
	{
		crc = crcName;
		this->attrib = Attrib;
		this->nTextures = nTextures;
		for ( int n=0; n < nTextures; ++n )
			this->pTextures[n] = pTextures[n];
	}
	ZeroMaterialDesc( const char *sName, unsigned int Attrib=0, ZeroSurface *pTextures[]=0, int nTextures=0 )
	{
		crc = CalcStrID( sName );
		this->attrib = Attrib;
		this->nTextures = nTextures;
		for ( int n=0; n < nTextures; ++n )
			this->pTextures[n] = pTextures[n];
	}
};


class ZeroMaterial: public ZeroUnknown
{
	ZERO_RTTI(ZeroUnknown);

public:
	ZeroColorValue diffuse;
	ZeroColorValue specular;
	ZeroColorValue emissive;
	float power;

public:
	//salah: this is a flag that i use for the sorting, it's set to 1 for the characters for now, 
	//but i can use it for some other stuff later !!
	unsigned int solidPiece;

	DLL_DECLSPEC	ZeroMaterial(const ZeroMaterial &material); 
	DLL_DECLSPEC	ZeroMaterial(const char *name);
					ZeroMaterial( unsigned int crcName );
					ZeroMaterial(const ZeroMaterialDesc &Desc );

	static DLL_DECLSPEC ZeroMaterial* __cdecl UseDefaultMaterial (void);

#ifdef PURE_ABSTRACT_MATERIAL
	ZeroUnknown *			CreateClone(bool isSim) = 0;
	virtual unsigned GetPrio(ZeroCamera* aCamera, const ZeroVector3& aWorldPosit)
	{
		return 0;
	}
#else
	ZeroUnknown *			CreateClone(bool isSim)
	{
		ZeroAssert((isSim, "ZeroMaterial - sim clone requested\n"));
		return new (isSim) ZeroMaterial (*this);
	}
#endif

	unsigned				GetAttrib()										{ return attribFlags; }
	void					SetAttrib(unsigned attrib)						{ attribFlags = attrib; }
	
	ZeroRenderState *		GetRenderStates()								{ return renderStates; }
	ZeroSurface *			GetSurface(const int stage = 0)					{ return surfaceTexture[stage]; }
	ZeroSurfaceStage *		GetSurfaceStages(const int stage = 0)			{ return surfaceStages[stage]; }
	ZeroMatrix *			GetSurfaceMatrix(const int stage = 0)			{ return surfaceMatrix[stage]; }

	virtual void					AttachSurface( ZeroSurface *surfaceArg, const int stage = 0 )
	{
		AttachObject ((ZeroUnknown **)&surfaceTexture[stage], (ZeroUnknown *)surfaceArg);
	}

	DLL_DECLSPEC void				AttachRenderStates (ZeroRenderState *renderArg);
	DLL_DECLSPEC ZeroRenderState *	AllocateRenderStates (int count);
	DLL_DECLSPEC void				AttachSurfaceStages (ZeroSurfaceStage *surfaceArg, int stage = 0);
	DLL_DECLSPEC ZeroSurfaceStage *	AllocateSurfaceStages (int count, int stage = 0);
	DLL_DECLSPEC void				AttachSurfaceMatrix (ZeroMatrix *matrixArg, int stage = 0);
	DLL_DECLSPEC ZeroMatrix *		AllocateSurfaceMatrix (int stage = 0);


	virtual unsigned int	GetVertexShaderID()		{ return 0; }	// Return unique numbers if you override this!
	virtual unsigned int	GetPixelShaderID()		{ return 0; }	// Return unique numbers if you override this!
	virtual void SetGlow(bool glow) {}

	DLL_DECLSPEC virtual void SetRenderStates(ZeroCamera* camera);

	//may be called when the material is used for the first time inside a frame
	DLL_DECLSPEC virtual void SetGlobalRenderStates(ZeroCamera* camera)		{}

protected:
	DLL_DECLSPEC ~ZeroMaterial();

	ZeroRenderState		*renderStates;
	ZeroSurface			*surfaceTexture[MAX_SURFACE_STAGES];
	ZeroSurfaceStage	*surfaceStages[MAX_SURFACE_STAGES];
	ZeroMatrix			*surfaceMatrix[MAX_SURFACE_STAGES];
	unsigned			allocFlags;
	unsigned			attribFlags;
};


#endif
