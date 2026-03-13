/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"
#include "ZeroMaterial.h"

static const unsigned ALLOCFLAG_RENDERSTATE = 0x80000000;
static inline unsigned ALLOCFLAG_SURFACESTAGE(int stage) { return 1U<<stage; }
static inline unsigned ALLOCFLAG_SURFACEMATRIX(int stage) { return 1U<<(stage+MAX_SURFACE_STAGES); }

ZERO_RTTI_CONTRUCTOR(ZeroMaterial);

#ifndef PURE_ABSTRACT_MATERIAL
namespace InternalMaterial
{
	static ZeroMaterial *defaultMaterial;

	class DefaultMaterial
	{

		DefaultMaterial ()
		{
			defaultMaterial = new (false) ZeroMaterial ("mat_default");

			static ZeroRenderState renderStates[] =
			{
				ZeroRenderState (ZERO_RENDERSTATE_LIGHTING, TRUE),
				ZeroRenderState (ZERO_RENDERSTATE_FILLMODE, ZERO_FILL_SOLID),
				ZeroRenderState (ZERO_RENDERSTATE_SHADEMODE, ZERO_SHADE_GOURAUD),
				ZeroRenderState (ZERO_RENDERSTATE_ALPHABLENDENABLE, FALSE),
				ZeroRenderState (ZERO_RENDERSTATE_NORMALIZENORMALS, FALSE),
				ZeroRenderState (ZERO_RENDERSTATE_SPECULARENABLE, FALSE),
				ZeroRenderState (ZERO_RENDERSTATE_DIFFUSEMATERIALSOURCE, ZERO_MATERIAL),
				ZeroRenderState (ZERO_RENDERSTATE_SPECULARMATERIALSOURCE, ZERO_MATERIAL),
				ZeroRenderState (ZERO_RENDERSTATE_EMISSIVEMATERIALSOURCE, ZERO_MATERIAL),
				ZeroRenderState (ZERO_RENDERSTATE_END,  0)
			};
			defaultMaterial->AttachRenderStates(renderStates);
		}
		public:

		~DefaultMaterial ()
		{
			defaultMaterial->Release();
		}

		friend void Startup();
	};

	void Startup()
	{
		static DefaultMaterial material;
	}
}
#endif


ZeroMaterial::ZeroMaterial (const char *name)
:	ZeroUnknown(name),
	diffuse(1, 1, 1, 1),
	specular(0, 0, 0, 1),
	emissive(0, 0, 0, 0),
	power(0)
{
	renderStates = NULL;
	memset(surfaceTexture, 0, sizeof(surfaceTexture));
	memset(surfaceStages, 0, sizeof(surfaceStages));
	memset(surfaceMatrix, 0, sizeof(surfaceMatrix));
	allocFlags = 0;
	attribFlags = 0;
	solidPiece = 0;
}


ZeroMaterial::ZeroMaterial (const ZeroMaterial &material) 
:	ZeroUnknown()
{
	_ASSERTE (0);
}


ZeroMaterial::ZeroMaterial( const ZeroMaterialDesc &Desc )
:	ZeroUnknown( Desc.crc ),
	diffuse(1, 1, 1, 1),
	specular(0, 0, 0, 1),
	emissive(0, 0, 0, 0),
	power(0)
{
	int n;
	
	attribFlags = Desc.attrib;
	for ( n=0; n < Desc.nTextures; ++n )
	{
		surfaceTexture[n] = Desc.pTextures[n];
		if ( surfaceTexture[n] )
			surfaceTexture[n]->AddRef();
	}
	for ( ; n < MAX_SURFACE_STAGES; ++n )
		surfaceTexture[n] = 0;
	renderStates = NULL;
	memset(surfaceStages, 0, sizeof(surfaceStages));
	memset(surfaceMatrix, 0, sizeof(surfaceMatrix));
	allocFlags = 0;
	solidPiece = 0;
}


ZeroMaterial::~ZeroMaterial () 
{
	if (allocFlags & ALLOCFLAG_RENDERSTATE)
	{
		delete[] renderStates;
	}

	for (int stage = 0; stage < MAX_SURFACE_STAGES; stage++)
	{
		if (surfaceTexture[stage])
		{
			surfaceTexture[stage]->Release();
		}
		if (allocFlags & ALLOCFLAG_SURFACESTAGE(stage))
		{
			delete surfaceStages[stage];
		}
		if (allocFlags & ALLOCFLAG_SURFACEMATRIX(stage))
		{
			delete surfaceMatrix[stage];
		}
	}
}

void ZeroMaterial::AttachRenderStates (ZeroRenderState *renderArg)
{
	if (allocFlags & ALLOCFLAG_RENDERSTATE)
	{
		delete[] renderStates;
	}

	allocFlags &= ~ALLOCFLAG_RENDERSTATE;
	renderStates = renderArg;
}

ZeroRenderState *ZeroMaterial::AllocateRenderStates (int count)
{
	if (allocFlags & ALLOCFLAG_RENDERSTATE)
	{
		delete[] renderStates;
	}

	allocFlags |= ALLOCFLAG_RENDERSTATE;
	renderStates = ZeroNew (ZeroRenderState[count]);
	return renderStates;
}

void ZeroMaterial::AttachSurfaceStages (ZeroSurfaceStage *surfaceArg, int stage)
{
	if (allocFlags & ALLOCFLAG_SURFACESTAGE(stage))
	{
		delete[] surfaceStages[stage];
	}

	allocFlags &= ~ALLOCFLAG_SURFACESTAGE(stage);
	surfaceStages[stage] = surfaceArg;
}

ZeroSurfaceStage *ZeroMaterial::AllocateSurfaceStages (int count, int stage)
{
	if (allocFlags & ALLOCFLAG_SURFACESTAGE(stage))
	{
		delete[] surfaceStages[stage];
	}

	allocFlags |= ALLOCFLAG_SURFACESTAGE(stage);
	surfaceStages[stage] = ZeroNew (ZeroSurfaceStage[count]);
	return surfaceStages[stage];
}

void ZeroMaterial::AttachSurfaceMatrix (ZeroMatrix *matrixArg, int stage)
{
	if (allocFlags & ALLOCFLAG_SURFACEMATRIX(stage))
	{
		delete[] surfaceMatrix[stage];
	}

	allocFlags &= ~ALLOCFLAG_SURFACEMATRIX(stage);
	surfaceMatrix[stage] = matrixArg;
}

ZeroMatrix *ZeroMaterial::AllocateSurfaceMatrix (int stage)
{
	if (allocFlags & ALLOCFLAG_SURFACEMATRIX(stage))
	{
		delete[] surfaceMatrix[stage];
	}

	allocFlags |= ALLOCFLAG_SURFACEMATRIX(stage);
	surfaceMatrix[stage] = ZeroNew (ZeroMatrix);
	return surfaceMatrix[stage];
}

// we over-ride this for the PS2 in the ps2Material class
void ZeroMaterial::SetRenderStates(ZeroCamera* camera)
{
	if (renderStates)
	{
		camera->SetRenderStates (renderStates);
	}

	for (int stage = 0; stage < MAX_SURFACE_STAGES; stage++)
	{
		if (surfaceStages[stage])
		{
			camera->SetTextureStages (stage, surfaceTexture[stage], surfaceStages[stage]);

			if (surfaceMatrix[stage])
			{
				camera->SetTextureMatrix (stage, *surfaceMatrix[stage]);
			}
		}
		else
		{
			camera->SetTextureStages (stage, NULL, NULL);
			break;
		}
	}
}


#ifndef PURE_ABSTRACT_MATERIAL
ZeroMaterial* __cdecl ZeroMaterial::UseDefaultMaterial ()
{
	ZeroStartup::InitEngine();

	_ASSERTE (InternalMaterial::defaultMaterial);
	InternalMaterial::defaultMaterial->AddRef(); 
	return InternalMaterial::defaultMaterial;
}
#endif
