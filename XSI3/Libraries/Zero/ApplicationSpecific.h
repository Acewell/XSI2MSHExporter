#ifndef __APPLICATION_SPEFICIC_H_
#define __APPLICATION_SPEFICIC_H_

#include "ZeroPlatform.h"

#define MAX_TEXTURES_PER_MSH_MATERIAL 8

#define RECTANGULAR_TEXTURES
#define ALLOW_ANY_TEXTURE_SIZE
#ifdef ALLOW_ANY_TEXTURE_SIZE
#define LINEAR_TEXTURES
#endif
#define CUBE_TEXTURES
#define VOLUME_TEXTURES
#define DEPTHSTENCIL_TEXTURES

// One of these days maybe we can unify shadow handling?
// Note: SHADOWING_ENABLE is similar to SHADOW_TYPE==PROJECTED_SHADOWS,
// but is only implemented on Windows. Don't mix them!
#define NO_SHADOWS        0		// Don't draw shadows
#define Z_BUFFER_SHADOWS  1		// Use light distance comparison for shadows
#define PROJECTED_SHADOWS 2		// Use smoother object by object projected shadows



#if (PLATFORM == PLATFORM_X86_WIN32)
	#define SHADOWING_ENABLE
	#define SHADOW_TYPE NO_SHADOWS
#elif (PLATFORM == PLATFORM_GAMECUBE)
	#define SHADOW_TYPE PROJECTED_SHADOWS
#endif

#if (PLATFORM == PLATFORM_X86_WIN32)
	#define TOOL_DEVELOPMENT
	#define ENABLE_EDITOR
	#define ENABLE_MODELER
	#define ENABLE_PARTICLE_EDITOR
	#define ENABLE_ANIMVIEWER

	// we need to switch to the new mgr to support reset
	#define USE_NEW_MEMORY_MGR
	#ifdef _DEBUG
		// can't track allocs now that we have switched
		// #define ZERO_TRACK_MEMORY_ALLOCATION
	#endif

#elif (PLATFORM == PLATFORM_GAMECUBE)

  #ifdef _DEBUG
    //#define ZERO_TRACK_MEMORY_ALLOCATION
  #else
  	#define _FINAL
  #endif

#elif (PLATFORM == PLATFORM_PS2)

  // New memory mgr needs to be always on for PS2. 
  #define USE_NEW_MEMORY_MGR

#elif (PLATFORM == PLATFORM_XBOX)

	// reset depends on new mgr
	#define USE_NEW_MEMORY_MGR

  #ifdef _DEBUG
    //#define ZERO_TRACK_MEMORY_ALLOCATION
  #else
//  	#define _FINAL
  #endif

#endif

#ifdef USE_NEW_MEMORY_MGR
#ifdef ZERO_TRACK_MEMORY_ALLOCATION
  #error BadPath
  #ifdef _DEBUG
  	#ifdef WIN32
      // Can't define both ZERO_TRACK_MEMORY_ALLOCATION and
      // USE_NEW_MEMORY_MGR at once - NM 7/8/02
	  	#define ZERO_TRACK_MEMORY_ALLOCATION
  	#endif
  #endif
#endif
#endif


#if ((PLATFORM == PLATFORM_GAMECUBE) || (PLATFORM == PLATFORM_PS2) || (PLATFORM == PLATFORM_XBOX))
	#define PURE_ABSTRACT_MATERIAL
#endif

// Resolution code, added NM 3/6/02
#if (PLATFORM == PLATFORM_PS2)
  // Screen sizes, in both int & float and half sizes
  const float SCREEN_WIDTH_FLOAT = 512.0f;
  const int   SCREEN_WIDTH_INT = 512;
  const float SCREEN_HEIGHT_FLOAT = 448.0f;
  const int   SCREEN_HEIGHT_INT = 448;
  const float HALF_SCREEN_WIDTH_FLOAT = 256.0f;
  const int   HALF_SCREEN_WIDTH_INT = 256;
  const float HALF_SCREEN_HEIGHT_FLOAT = 224.0f;
  const int   HALF_SCREEN_HEIGHT_INT = 224;

#else
// PC/GC. Maybe we should split up things here-- it'd be neat to have
// the FE resolution be a variable on the PC, scaled up to match.

  // Screen sizes, in both int & float and half sizes
  const float SCREEN_WIDTH_FLOAT = 640.0f;
  const int   SCREEN_WIDTH_INT = 640;
  const float SCREEN_HEIGHT_FLOAT = 480.0f;
  const int   SCREEN_HEIGHT_INT = 480;
  const float HALF_SCREEN_WIDTH_FLOAT = 320.0f;
  const int   HALF_SCREEN_WIDTH_INT = 320;
  const float HALF_SCREEN_HEIGHT_FLOAT = 240.0f;
  const int   HALF_SCREEN_HEIGHT_INT = 240;

#endif

#if (PLATFORM == PLATFORM_XBOX)

  #define LEFT_MARGIN 30
  #define RIGHT_MARGIN 30
  #define TOP_MARGIN 20
  #define BOTTOM_MARGIN 20

#elif (PLATFORM == PLATFORM_X86_WIN32)

  #define LEFT_MARGIN 30
  #define RIGHT_MARGIN 30
  #define TOP_MARGIN 20
  #define BOTTOM_MARGIN 20

#else
  #define LEFT_MARGIN 0
  #define RIGHT_MARGIN 0
  #define TOP_MARGIN 0
  #define BOTTOM_MARGIN 0

#endif

	const int MAX_PLAYERS = 8;

#if (PLATFORM == PLATFORM_PS2)
	// We don't have no stinkin' VM - NM 6/4/02
	#define DISABLE_ALL_VM 1

  // Note where the files are located.
  #if defined(CDROM) || defined(DVDROM)
    // files on cd
    #define TARGET_PS2 2
  #else
    // files on dev kit
    #define TARGET_PS2 1
  #endif
#elif (PLATFORM == PLATFORM_XBOX)
	// Don't need no stinkin' (and broken) VM for xbox - JLS 12/18/02
	#define DISABLE_ALL_VM		1
#else
	// GC, PC/DX8 builds do use VM
	#ifdef _DEBUG
		#define DISABLE_ALL_VM		1
	#else
		#define DISABLE_ALL_VM		0	
	#endif
#endif

#if ((PLATFORM == PLATFORM_X86_WIN32) || (PLATFORM == PLATFORM_XBOX))

// -------------------------------------------------------------------
// Warning levels tweaks, before we get in any headers. The code is
// designed to be compiled at warning level 4 (max).

// disable annoying warnings we can't do as much about

#pragma warning (disable : 4100) // unreferenced formal pararameter
#pragma warning (disable : 4115) // named type definition in parentheses (usually _RPC_ASYNC_STATE)
#pragma warning (disable : 4127) // conditional expression is constant -- NEEDED FOR _ASSERTE
#pragma warning (disable : 4189) // local variable is initialized but not referenced
#pragma warning (disable : 4201) // nonstandard extension used : nameless struct/union
#pragma warning (disable : 4206) // nonstandard extension used : translation unit is empty (file has no content)
#pragma warning (disable : 4505) // unreferenced local function has been removed
#pragma warning (disable : 4511) // copy constructor could not be generated
#pragma warning (disable : 4512) // assignment operator could not be generated
#pragma warning (disable : 4514) // unreferenced inline/local function has been removed
#pragma warning (disable : 4786) //'identifier' : identifier was truncated to 'number' characters in the debug information

#pragma warning (disable : 4291) // no matching operator delete found
#pragma warning (disable : 4800)

// I want these warnings in a release build, as it'll help identify
// where we should use more/less inlining...
#ifdef DEBUG
#pragma check_stack(on) 
#else
#pragma warning (default : 4710) // function 'foo' not inlined
//#pragma warning (error : 4711) // function 'foo' selected for automatic inline expansion
#endif

// And promote some warnings to errors about dangerous C code styles. 

#pragma warning (error : 4013) //'function' undefined; assuming extern returning int
#pragma warning (error : 4020) // too many actual parameters (i.e. different from declaration)
#pragma warning (error : 4024) // formal parameter N different from declaration
#pragma warning (error : 4028) // formal parameter N different from declaration (not quite C4024)
#pragma warning (error : 4033) // function must return a value
#pragma warning (error : 4087) // function : declared with 'void' parameter list
#pragma warning (error : 4133) //'type' : incompatible types - from 'type1' to 'type2'
#pragma warning (error : 4700) // local variable 'foo' used without having been initialized
#pragma warning (error : 4701) // local variable 'foo' may used without having been initialized
#pragma warning (error : 4715) // function: not all control paths return a value

// Promote a warning to an error to help the PS2 version avoid all doubles.
//#pragma warning (error : 4244) // conversion from 'double' to 'float', possible loss of data
// [Ack, can't do that yet, it's not just double -> floats that are trapped.]


// -------------------------------------------------------------------
// General, systemwide headers.
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#if PLATFORM == PLATFORM_XBOX
#include <xtl.h>
#include <xonline.h>
#include <vector>
#endif

// End of General, systemwide headers.
// -------------------------------------------------------------------

//#pragma warning ( push, 3)
//#pragma warning (pop)

#endif // PLATFORM_XBOX

#endif // ifndef __APPLICATION_SPEFICIC_H_
