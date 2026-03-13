/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroTypes__
#define __ZeroTypes__

#include "ZeroPlatform.h"


// Smedis was here.
// You have to create an (at least empty) file with the same name in your Application!
// Make sure the compiler can find the file by adding the folder in Tools|Options|Directories, or in similar way.
// The purpose of the file is to create application-dependant defines that changes the behavior of the Zero library.
#include "ApplicationSpecific.h"




#if (COMPILER == VISUAL_C)
	#pragma warning (disable : 4100) //unreferenced formal parameter
	#pragma warning (disable : 4710) //function not inlined
	#pragma warning (disable : 4711) //function '$E1' selected for automatic inline expansion

	#ifdef _DEBUG
		#pragma warning (disable : 4127)	//conditional expression is constant
	#endif
#endif


#if (PLATFORM == PLATFORM_X86_WIN32)
	#include <windows.h>
	#include <crtdbg.h>
#endif

#if (PLATFORM == PLATFORM_XBOX)
	#define D3DCOMPILE_PUREDEVICE 1
	#undef  D3DCOMPILE_NOTINLINE
	#include <xtl.h>
	#include <crtdbg.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <float.H>

#if (PLATFORM == PLATFORM_X86_WIN32)
	#include <io.h> 
	#include <direct.h> 
	// Must include sys/stat.h before we muck with wchar_t's type.
	#include <sys/stat.h>
#endif



//#define USE_NEW_PHYSICS_MANAGER

#ifdef USE_GROUND_ZERO_STATIC_LIBRAY
	// compile static library
	#define DLL_DECLSPEC

#else
	// compile dynamic library
	#ifdef GROUND_ZERO_DLL
		#define DLL_DECLSPEC 			__declspec(dllexport)
	#else
		#define DLL_DECLSPEC 			__declspec(dllimport)
	#endif
#endif


#if ((PLATFORM == PLATFORM_X86_WIN32) || (PLATFORM == PLATFORM_XBOX))
	// win32/intel specifics

	#ifdef LONGLONG
		#undef LONGLONG
	#endif

	#ifdef LONGLONG
		#undef Naked
	#endif

	#define Naked __declspec (naked)
	#define LONGLONG __int64
	#define u__int64 unsigned __int64

	#define MAKEID(a, b, c, d) (((unsigned int)(a) << 0) | ((unsigned int)(b) << 8) | ((unsigned int)(c) << 16) | ((unsigned int)(d) << 24))

	typedef int					int32;
	typedef unsigned int		uint32;
	typedef short				int16;
	typedef unsigned short	uint16;
	typedef char				int8;
	typedef unsigned char	uint8;
	typedef wchar_t cw_wchar_t;

	// Drop in a small compiler error to help catch misuse of wchar_t --
	// that is incompatible w/ gcc 2.95.3 on PS2 (upgrading is NOT doable
	// now 6/11/02). So, everyone should use cw_wchar_t instead.
	#ifndef XSI_TOOLS
		#ifdef wchar_t
			#undef wchar_t
		#endif
		#define wchar_t DONT_USE_WCHAR_T___USE_CW_WCHAR_T_FOR_PS2_COMPATABILITY
	#endif

#else
		// other cpus here

	#if (PLATFORM == PLATFORM_GAMECUBE)
		typedef long				int32;
		typedef unsigned long	uint32;
		typedef short				int16;
		typedef unsigned short	uint16;
		typedef char				int8;
		typedef unsigned char	uint8;
		typedef wchar_t cw_wchar_t;

	#elif(PLATFORM == PLATFORM_PS2)
		typedef int					int32;
		typedef unsigned int		uint32;
		typedef short				int16;
		typedef unsigned short	uint16;
		typedef char				int8;
		typedef unsigned char	uint8;

		// gcc 2.95.3 doesn't set this the way the interface wants,
		// and gcc 2.96.0 is horribly broken. - NM 6/10/02
		typedef unsigned short cw_wchar_t;

		// Drop in a small compiler error to help catch misuse of wchar_t --
		// that is incompatible w/ gcc 2.95.3 on PS2 (upgrading is NOT doable
		// now 6/11/02). So, everyone should use cw_wchar_t instead.
	    #ifdef wchar_t
	        #undef wchar_t
		#endif
	    #define wchar_t DONT_USE_WCHAR_T___USE_CW_WCHAR_T_FOR_PS2_COMPATABILITY

	#endif

	#define HWND			void*
	#define HINSTANCE		int
	#define Naked 
	#define LONGLONG		long

	struct IntPart{
			int LowPart; 
			int  HighPart; 
	};

	struct LARGE_INTEGER { 
		union
		{
			IntPart intPart;
			long	QuadPart;
		};
	} ; 

	#define __cdecl
	//	#define LARGE_INTEGER	long


	#define _finite(x) finitef(x)
	#define OutputDebugString(a) printf(a)
	#define __int64			int64
	#define u__int64		uint64
	#define BOOL			bool

	// nintendo defines these in types.h, so we only redefine them on PS2
	#ifndef TRUE
		#define TRUE			true
	#endif
	#ifndef FALSE
		#define FALSE			false
	#endif

	#define GetModuleFileName(hMod, dst_str, dst_size)\
		sprintf(dst_str, "Line:%d, Module:%s", __LINE__, __FILE__);

	#define MAKEID(a, b, c, d) (((unsigned int)(a) << 0) | ((unsigned int)(b) << 8) | ((unsigned int)(c) << 16) | ((unsigned int)(d) << 24))

	#define _MAX_FNAME	1024
	#define _MAX_EXT	4
#endif

extern "C++"
{
	namespace ZeroStartup 
	{
	#ifdef USE_GROUND_ZERO_STATIC_LIBRAY
		extern void InitEngine();
	#else
		inline void InitEngine() {}
	#endif
	}
}



#define __ASM_MATH__
#define __USE_CPU_FOUND__

#ifdef _DEBUG
	//#define __ENABLE_SANITY_CHECK 
#endif

const float PI				   = (3.141592654f);
const float PI2				= (PI * 2.0f);
const float PIBY2			   = (PI * 0.5f);
const float DEG2RAD			= (PI / 180.0f);
const float RAD2DEG			= (180.0f / PI);
const float EPSILON			= (1e-4f);
const float INFINITE_MASS	= (1e10f);
const float HUGE_NUMBER		= (1e30f);

#define SIZEOF(a) (sizeof(a)/sizeof(a[0]))

#if (PLATFORM == PLATFORM_PS2)

	#define __tHEAP_NO_MALLOC__

	//#include <eekernel.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <math.h>
	//#include <libdev.h>
	//#include <eeregs.h>
	//#include <libgraph.h>
	//#include <libdma.h>
	//#include <sifdev.h>
	//#include <sifrpc.h>
	//#include <libpad.h>

	#include <math/ee/matrixfloat_t.h>
	#include <math/ee/matrixfloat_vu0_t.h>

	#include <core/ee/assert_t.h>
	#include <core/ee/types_t.h>
	#include <core/ee/timer_t.h>
	#include <core/ee/cpu_t.h>
	#include <core/ee/library_t.h>
	#include <core/ee/hashid_t.h>
	#include <core/ee/file_t.h>
	#include <core/ee/host_t.h>
	#include <core/ee/iop_t.h>
	#include <core/ee/heap_t.h>
	#include <core/ee/pad_t.h>
	/*
	#include <render/ee/texture_t.h>
	#include <render/ee/fog_t.h>
	#include <render/ee/framebuffer_t.h>
	#include <render/ee/dma_t.h>
	#include <render/ee/viewport_t.h>
	#include <render/ee/video_t.h>
	#include <render/ee/material_t.h>
	#include <render/ee/light_t.h>
	#include <render/ee/draw_t.h>
	#include <render/ee/render_t.h>
	#include <render/ee/rendermath_t.h>
	#include <render/ee/metafile_t.h>
	#include <render/ee/model_t.h>
	#include <render/ee/modelinstance_t.h>
	#include <render/ee/renderprofile_t.h>

	#include <font/ee/font_t.h>
	*/

	extern "C"
	{
	#include "zen/ee/zen.h"
	}

#endif

// ASSERT macros
#ifdef _DEBUG
	inline void ZeroBreak(void)
	{
	#if ( (PLATFORM == PLATFORM_X86_WIN32) || (PLATFORM == PLATFORM_XBOX) )
			_asm {
				int 3
			}
	#elif (PLATFORM == PLATFORM_PS2)
		asm volatile
		(
			"break 0"
		);
	#elif (PLATFORM == PLATFORM_GAMECUBE)
		asm volatile
		(
			tw 0x1f,r0,r0
			mr r0,r0
		);
	#endif
	}
	
	
	char *__cdecl __GetAssertMessage__ ();
	void __cdecl __ExpandTraceMessage__ (char *fmt, ...);
	bool __cdecl __ExpandAssertMessage__ (const void* ptr, char *fmt, ...);
	bool __cdecl __ExpandAssertMessage__ (unsigned fCondition, char *fmt, ...);
	
	
	//DLL_DECLSPEC void __cdecl ZeroTrace (char *fmt, ...);
	
	#if (PLATFORM == PLATFORM_X86_WIN32)
		#define ZeroTrace(x)											\
		{																\
			__ExpandTraceMessage__ x;									\
			_RPTF1(_CRT_WARN, "%s", __GetAssertMessage__ ());			\
		}

		#define ZeroWarn(x)												\
		{																\
			if (__ExpandAssertMessage__ x) {							\
				_RPTF1( _CRT_ASSERT, "%s", __GetAssertMessage__ ());	\
			}															\
		}

		#define ZeroAssert(x)											\
		{																\
			if (__ExpandAssertMessage__ x) {							\
				_RPTF1( _CRT_ASSERT, "%s", __GetAssertMessage__ ());	\
			}															\
		}

	#elif (PLATFORM == PLATFORM_XBOX) 
		#define ZeroTrace(x)											\
		{																\
			__ExpandTraceMessage__ x;									\
			_RPTF1(_CRT_WARN, "%s", __GetAssertMessage__ ());			\
		}
		
		#define ZeroWarn(x)												\
		{																\
			if (__ExpandAssertMessage__ x) {							\
				_RPTF1( _CRT_ASSERT, "%s", __GetAssertMessage__ ());	\
			}															\
		}
		
		#define ZeroAssert(x)											\
		{																\
			if (__ExpandAssertMessage__ x) {							\
				_RPTF1( _CRT_ASSERT, "%s", __GetAssertMessage__ ());	\
			}															\
		}

	#elif (PLATFORM == PLATFORM_PS2) 
		#define ZeroTrace(x)											\
		{																\
			__ExpandTraceMessage__ x;									\
			printf ("%s\n", __GetAssertMessage__ ());					\
		}
		
		#define ZeroWarn(x)												\
		{																\
			if (__ExpandAssertMessage__ x) {							\
				printf ("%s\n", __GetAssertMessage__ ());				\
			}															\
		}

		#define ZeroAssert(x)											\
		{																\
			if (__ExpandAssertMessage__ x) {							\
				printf ("%s\n", __GetAssertMessage__ ());				\
				ZeroBreak();											\
			}															\
		}
		
		#define _ASSERT(exp) assert((exp))
		#define _ASSERTE(exp) assert((exp))
		

	#elif (PLATFORM == PLATFORM_GAMECUBE)
		#define ZeroTrace(x)											\
		{																\
			__ExpandTraceMessage__ x;									\
			OSReport ("%s(%d): %s",										\
				__FILE__, __LINE__, __GetAssertMessage__ ());			\
		}

		#define ZeroWarn(x)												\
		{																\
			if (__ExpandAssertMessage__ x) {							\
				OSReport ("%s(%d) Warning: %s\n",                       \
					 __FILE__, __LINE__, __GetAssertMessage__ ());		\
			}															\
		}

		#define ZeroAssert(x)											\
		{																\
			if (__ExpandAssertMessage__ x) {							\
				OSReport ("%s(%d) Warning: %s\n",                       \
					 __FILE__, __LINE__, __GetAssertMessage__ ());		\
				ZeroBreak();											\
			}															\
		}
		
		#define _ASSERT(x) 												\
		{																\
			if (!(x))														\
			{															\
				OSReport ("%s(%d) Warning: %s\n",                       \
					 __FILE__, __LINE__, __GetAssertMessage__ ());		\
				ZeroBreak();											\
			}															\
		}
		#define _ASSERTE(x) _ASSERT(x)
	#endif



#else

	#define ZeroBreak() {}
	#define ZeroTrace(x) {}
	#define ZeroWarn(x) {}
	#define ZeroAssert(x) {}
	#define _ASSERT(expr) ((void)0)
	#define _ASSERTE(expr) ((void)0)

#endif

#endif
