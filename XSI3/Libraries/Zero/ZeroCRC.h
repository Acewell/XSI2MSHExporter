///////////////////////////////////////////////////////////////////////////////
//
// Copyright 1997-1999 Pandemic Studios, Dark Reign II
//
// Cyclic Redundancy Checking
// 1-DEC-1997

#ifndef __ZeroCRC__
#define __ZeroCRC__

#include "ZeroTypes.h"



//
// Calculate the CRC of given buffer (32bit)
//
DLL_DECLSPEC unsigned __cdecl CalcCRC(const void *buff, unsigned len, unsigned seed = 0);

//
// As for Calc but using lowercase ascii characters
//
DLL_DECLSPEC unsigned __cdecl CalcLowerCRC(const void *buff, unsigned len, unsigned seed = 0);
DLL_DECLSPEC unsigned __cdecl CalcLowerCRC(const char*buff, unsigned seed = 0);

//
// As for CalcLower, but stops at null terminator and register the ineverse
//
DLL_DECLSPEC unsigned __cdecl CalcStrCRC(const char *str, unsigned seed = 0);

// As for CRC but stops at null terminator and register the ineverse
DLL_DECLSPEC const char* __cdecl InvertCRC (unsigned CheckSum);

// CRCs for when we don't need so much precision (i.e. network code area
inline unsigned short CalcCrc16(const unsigned char *p, const unsigned int len)
{
	return (unsigned short)	(CalcCRC (p, len, 0) >> 16);
}

// another interface to the CRC id generation which doesn't need to assume anything
// about the returned value except that it is uniq for a case insensitive string
DLL_DECLSPEC unsigned int __cdecl CalcStrID(const char *src, unsigned int seed = 0);
DLL_DECLSPEC const char * __cdecl InvertStrID(unsigned int id);

DLL_DECLSPEC void CRC_Preset(void);
DLL_DECLSPEC void CRC_Reset(void);

#endif
