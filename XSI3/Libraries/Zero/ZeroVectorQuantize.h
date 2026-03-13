#ifndef _VECTOR_QUANTIZE_H_
#define _VECTOR_QUANTIZE_H_

#ifdef TOOL_DEVELOPMENT 

#include "ZeroTypes.h"


// ========================================================================
// Function:
//     VectorQuantize
// Purpose:
//     Compute an array of ouput code vectors from an array of input data
//     vectors, and then assign a code index to each input data vector.
// Arguments:
//     vectorSize: size of a data or code vector in floats
//     dataArray: array of input data vectors
//     dataCount: number of input data vectors
//     codeArray: array of output code vectors
//     codeCount: maximum number of output code vectors
//     codeIndex: array of output code indices, one per input data vector
//     meterFunc: metering function called after generating each code entry
// ========================================================================
DLL_DECLSPEC void VectorQuantize(const int vectorSize,
								 const float dataArray[], const int dataCount,
								 float codeArray[], const int codeCount, int codeIndex[],
								 void (*meterFunc)(const int addCur, const int addMax) = NULL);

#endif

#endif //#ifdef TOOL_DEVELOPMENT 
