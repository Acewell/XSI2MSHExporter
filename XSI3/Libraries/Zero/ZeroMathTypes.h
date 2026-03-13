#ifndef __ZeroMathTypes__
#define __ZeroMathTypes__

#include "ZeroTypes.h"

class ZeroVector3;

// define some specific stuff

enum ZeroCPU 
{
	PENTIUM,
	PENTIUM_MMX,
	AMD_K62,
	PENTIUM_III
};

// Set_Coprocessor_RC_Flag
const unsigned ZeroRoundNearest		= 0x0000;
const unsigned ZeroRoundDown			= 0x0400;
const unsigned ZeroRoundUp				= 0x0800;
const unsigned ZeroRoundChop			= 0x0c00;

const unsigned ZeroSinglePrecision	= 0x0000;
const unsigned ZeroDoublePrecision	= 0x0200;

const unsigned ZeroInvalidOper 		= 0x0001;
const unsigned ZeroDenormalizeOper	= 0x0002;
const unsigned ZeroDivideByZero		= 0x0004;
const unsigned ZeroOverflow	 		= 0x0008;
const unsigned ZeroUnderflow			= 0x0010;

DLL_DECLSPEC int __cdecl ZeroGetCtrlWorld ();
DLL_DECLSPEC int __cdecl ZeroSetRounding (unsigned Mask);
DLL_DECLSPEC int __cdecl ZeroSetPrecision(unsigned Mask);
DLL_DECLSPEC void __cdecl ZeroSetCtrlWorld (unsigned Mask);

DLL_DECLSPEC ZeroCPU  __cdecl ZeroGetCpuType ();
DLL_DECLSPEC unsigned  __cdecl ZeroSetFloatExceptionsMask (unsigned mask);
DLL_DECLSPEC LONGLONG  __cdecl ZeroCpuTimeStamp ();



// some bz2 const
#define Float_Precision       ZeroSinglePrecision
#define Float_DoublePrecision ZeroDoublePrecision
#define Invalid_Oper          ZeroInvalidOper
#define Denormalize_Oper      ZeroDenormalizeOper
#define Divide_By_Zero        ZeroDivideByZero 
#define Overflow              ZeroOverflow 
#define Underflow             ZeroUnderflow
#define SetFloatExceptions	   ZeroSetFloatExceptionsMask

// legacy bz2 stuff
struct UVPair
{
	float u;
	float v;
	
	// uv-pair constructor
	UVPair()
	{
	}
	UVPair(const float u, const float v)
		: u(u), v(v)
	{
	}
};

// float-sign union
union FLOATSIGN
{
	float f;
	int i;
};

union FLOAT2INT
{
   double f;
   int i;
};



// float-to-integer constants
//extern DLL_DECLSPEC volatile double FloatToIntF;
//extern DLL_DECLSPEC volatile double FloatToIntK;



//----------------------------------------------------------------------------
inline float fsign (float a)
{
	float Sign = 1.0f;
	if (a < 0) {
		Sign = -1.0f;
	}
	return Sign;
}

// I would like to now any reason (beside personal ego) to change the above 
// function to this function [julio] 

// floating-point sign
//inline float fsign(float val)
//{
//	return val < 0.0f ? -1.0f : 1.0f;
//}






//----------------------------------------------------------------------------

// interpolation
inline float Interpolate (float a, float b, float t)
{
	return a * (1.0f - t) + b * t;
}

//----------------------------------------------------------------------------

// low-pass filtering
inline float lpfilter (float x, float x_target, float q)
{
	if (q <= 0.0f)
		return x;
	if (q >= 1.0f)
		return x_target;
	return x + q * (x_target - x);
}

//----------------------------------------------------------------------------

inline float GetMin (float A, float B)
{
	if (B < A) {
		A = B;
	}
	return A;
}

inline float GetMin (float A, float B, float C)
{
	if (B < A) {
		A = B;
	}
	if (C < A) {
		A = C;
	}
	return A;
}



inline float GetMax (float A, float B)
{
	if (B > A) {
		A = B;
	}
	return A;
}

inline float GetMax (float A, float B, float C)
{
	if (B > A) {
		A = B;
	}
	if (C > A) {
		A = C;
	}
	return A;
}

inline double GetMin (double A, double B)
{
	if (B < A) {
		A = B;
	}
	return A;
}

inline double GetMin (double A, double B, double C)
{
	if (B < A) {
		A = B;
	}
	if (C < A) {
		A = C;
	}
	return A;
}
inline double GetMax (double A, double B)
{
	if (B > A) {
		A = B;
	}
	return A;
}

inline double GetMax (double A, double B, double C)
{
	if (B > A) {
		A = B;
	}
	if (C > A) {
		A = C;
	}
	return A;
}
// fast float/double to integer conversion
//inline int FloatToInt(double f)
inline int FloatToInt(float f)
{
#ifdef _WIN32
	int retVal;
	__asm {
		fld	f
		fistp	retVal
	}
	return retVal;
#else
	// FLOAT2INT fi;
	// fi.f = (f - FloatToIntK) + FloatToIntF;
	// return fi.i;
	return (int)f;
#endif
}

#if (PLATFORM != PLATFORM_PS2)
// Doubles are bad on PS2, ok? Shut up warnings if this function disappears
inline int FloatToInt(double f)
{
#ifdef _WIN32
	int retVal;
	__asm {
		fld	f
		fistp	retVal
	}
	return retVal;
#else
	// FLOAT2INT fi;
	// fi.f = (f - FloatToIntK) + FloatToIntF;
	// return fi.i;
	return (int)f;
#endif
}
#endif

template <class T> int ZeroBinarySearch (
	T const* array, 
	int elements,
	int entry)
{
	int index0;
	int index1;
	int index2;
	int entry1;

	index0 = 0;
	index2 = elements;

   while (index2 > index0) {
      index1 = (index0 + index2) >> 1;
		entry1 = array[index1].key;
		if (entry1 == entry) {
//			_ASSERTE (array[index1].key <= entry);
//			_ASSERTE (((index1 + 1) == elements) || (array[index1 + 1].key >= entry));
			return index1;
		} else if (entry < entry1) {
			index2 = index1;
		} else {
			index0 = index1 + 1;
		}
	}

   if(array[index0].key>entry)
	   index0--;

   _ASSERTE(index0>=0);
//	_ASSERTE (array[index0].key <= entry);
//	_ASSERTE (array[index0 + 1].key >= entry);
	return index0;
}





DLL_DECLSPEC void __cdecl GetMinMax (ZeroVector3 &Min, ZeroVector3 &Max,
									 const float *vertexArray, int strideInBytes,
									 int vCount);

template <class T> void ZeroSwap(T& A, T& B)
{
	T tmp (A);
	A = B;
	B = tmp;
}	



// return the minimum of two values
#ifdef min
#undef min
#endif
template <class T> T min(const T a, const T b)
{
	return (a < b) ? (a) : (b);
}

// return the maximum of two values
#ifdef max
#undef max
#endif
template <class T> T max(const T a, const T b)
{
	return (a < b) ? (b) : (a);
}

// clamp a value between a lower and upper bound
template <class T> T clamp(const T x, const T l, const T u)
{
	if (x < l) return l;
	if (x > u) return u;
	return x;
}

#if (PLATFORM == PLATFORM_PS2)
	inline float rsqrt (float x) return result;
	{
		const float one = 1.0f;
		
		__asm__ volatile
		(
		"
			.set noreorder

			rsqrt.s		%0,%1,%2

			.set reorder

		": "=&f" ( result )
		: "f" ( one ), "f" (x)
		
		);
		
	}

	inline float sqrtf (float x) return result;
	{
		
		__asm__ volatile
		(
		"
			.set noreorder

			sqrt.s		%0,%1

			.set reorder

		": "=&f" ( result )
		:  "f" (x)
		
		);
		
	}
	
#else
	DLL_DECLSPEC float __cdecl rsqrt (double x);
#endif

DLL_DECLSPEC float __cdecl Zero_sin(float ang);
DLL_DECLSPEC float __cdecl Zero_cos(float ang);
DLL_DECLSPEC float __cdecl Zero_asin(float x);
DLL_DECLSPEC float __cdecl Zero_acos(float x);
DLL_DECLSPEC float __cdecl Zero_atan2(float x, float y);


DLL_DECLSPEC float __cdecl CircularInterpolation (float p0, float p1, float period, float t);




// compute partial reminder of x / y
// return a value in the interval [ -Period/2  <= x < Period/2 ]
DLL_DECLSPEC float __cdecl a_mod (float x, float Period);

// compute partial reminder of x / y
// return a value in the interval [0 <= x < Period]
inline float s_mod (float x, float Period)
{
	x = a_mod (x, Period);
	if (x < 0.0f) {
		x += Period;
	}
	return x;
}


inline int exp_2 (int x)
{
	int exp;
	for (exp = -1; x; x >>= 1) {
		exp ++;
	}
	return exp;
}


/*



// return an aproximation of the function 1.0/sqrt(x);

// return a random number between 0 and 0xffffffff;
DLL_DECLSPEC unsigned __cdecl random();
DLL_DECLSPEC void __cdecl Randomize (unsigned Seed);
DLL_DECLSPEC unsigned __cdecl Get_CheckSum (const void *Stream, int Size);
DLL_DECLSPEC unsigned __cdecl Get_CheckSum (const char *Name);
DLL_DECLSPEC const char* __cdecl String_Lookup (unsigned CheckSum);
DLL_DECLSPEC void __cdecl Make_File_Name (const char *Name, char *Full_PathName);
DLL_DECLSPEC void __cdecl Set_Path (const char *Name);








//this funtion (+) if  [x0 <= x <= x1] around the circle
inline int Circular_Range_Compare (float x0, float x1, float x)
{
	FLOATSIGN dx0;
	FLOATSIGN dx1;
	
	dx0.f = x - x0;
	dx1.f = x1 - x;
	
	if (x1 < x0) {
		return dx0.i & dx1.i;
	}
	return dx0.i | dx1.i;
}


inline float frandom()
{
	return (float)((double)random() * (1.0 / (double)0xffffffff));
}
	
	
*/        

#endif
