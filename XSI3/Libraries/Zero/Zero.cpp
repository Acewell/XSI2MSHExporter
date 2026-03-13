/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"

#pragma warning (disable : 4073)	// initializers put in library initialization area
#pragma init_seg (lib)

//namespace InternalMemory
//{
//	extern void Startup();
//}

namespace InternalStack
{
	extern void Startup ();
}


namespace InternalCRC
{
	extern void Startup ();
}

//namespace InternalZeroUnknown
//{
//	extern void Startup();
//}

namespace InternalSurface
{
	extern void Startup();
}

namespace InternalMaterial
{
	extern void Startup();
}

//namespace InternalRenderer
//{
//	extern void Startup();
//}


namespace InternalMeshLoader
{
	extern void Startup();
}


namespace InternalVideo
{
	extern void Startup();
}

namespace InternalObject
{
	extern void Startup();
}

namespace InternalAABB
{
	extern void Startup();
}

namespace  ZeroStartup 
{
	static bool startup_called = false;

	class Startup
	{
		public:
		Startup ()
		{
			if (!startup_called)
			{
				startup_called = true;
#if (PLATFORM == PLATFORM_PS2)
				printf("Entering startup...\n");
#endif
				ZeroPlatformInit();
//				InternalMemory::Startup();
				InternalStack::Startup();
//				InternalZeroUnknown::Startup();
				InternalCRC::Startup();
				InternalSurface::Startup();
//				InternalRenderer::Startup();
#ifndef PURE_ABSTRACT_MATERIAL
				InternalMaterial::Startup();
#endif
				InternalMeshLoader::Startup();
				InternalVideo::Startup();
				InternalObject::Startup();
				InternalAABB::Startup();
			}
		}
	};

#ifdef USE_GROUND_ZERO_STATIC_LIBRAY
	void InitEngine()
	{
		static Startup startup;
	}

#else
	static Startup startup;
#endif

}
