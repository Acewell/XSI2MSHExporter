#ifndef __ZeroPlatform__
#define __ZeroPlatform__

#define PLATFORM_X86_WIN32	0
#define PLATFORM_PS2				1
#define PLATFORM_GAMECUBE		2
#define PLATFORM_XBOX				3

#define VISUAL_C     0
#define GNU_C        1
#define CODEWARRIOR  2

#ifndef PLATFORM
#if defined(WIN32)  //XBOX DEFINES WIN32
#if defined(_XBOX)
#define PLATFORM PLATFORM_XBOX
#define COMPILER VISUAL_C
#else
#define PLATFORM PLATFORM_X86_WIN32
#define COMPILER VISUAL_C
#endif
#elif defined(SN_TARGET_PS2)
#define PLATFORM PLATFORM_PS2
#define COMPILER GNU_C
#else
#define PLATFORM PLATFORM_GAMECUBE
#define COMPILER CODEWARRIOR
#endif
#endif

#if (PLATFORM!=PLATFORM_X86_WIN32)
#define GAME_DECLSPEC
#define DLL_DECLSPEC
#endif

#if (PLATFORM==PLATFORM_GAMECUBE)
#include <dolphin/os.h>
#include <dolphin/dvd.h>
#include <dolphin/pad.h>
#include <dolphin/gx.h>

// turns on anonymous structures
#pragma cpp_extensions on
// turns on long long
#pragma longlong on

#define USE_GROUND_ZERO_STATIC_LIBRAY
#define stricmp strcmpi
#define uint64  unsigned long long
#define int64   long long

#define	ENDIAN_DECLARE_INT32(val)       (((val >>24) & 0x000000ff) | \
                                         ((val >> 8) & 0x0000ff00) | \
                                         ((val << 8) & 0x00ff0000) | \
                                         ((val <<24) & 0xff000000) )
#define	ENDIAN_DECLARE_CHAR32(a,b,c,d)  ((d << 24) | (c << 16) | (b << 8)  | (a))

#ifdef __cpluscplus
extern "C" {
#endif

// Metrowerks have their own idea of what constitues a "standard" C library
char *strlwr (char * string);
char *strupr (char * string);
int strnicmp (const char * first, const char * last, unsigned int count);
int strcmpi(const char * dst, const char * src);
void ZeroPlatformInit();

#ifdef __cpluscplus
}
#endif

#else

// non gamecube defines
#define ZeroPlatformInit()             ((void)0)
#define	ENDIAN_DECLARE_INT32(val)      (val)
#define	ENDIAN_DECLARE_CHAR32(a,b,c,d) ((a << 24) | (b << 16) | (c << 8)  | (d))

#endif

// this is some gamecube debug stuff
#if ((PLATFORM == PLATFORM_GAMECUBE) && defined(_DEBUG))
void gcSetBufferDescription(const char *szCurDesc);
#define SET_BUFFER_DESCRIPTION(str) gcSetBufferDescription(str)
#else
#define SET_BUFFER_DESCRIPTION(str) ((void)0)
#endif

#endif // #ifdef __ZeroPlatform__