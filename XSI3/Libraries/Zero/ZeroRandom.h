//********************************************************************
// Direct geometry Hardware independent library
//	By Julio Jerez
// VC: 6.0
//********************************************************************
#ifndef __ZeroRandom__
#define __ZeroRandom__

// seed the random number generator
DLL_DECLSPEC void ZeroSeed(unsigned seed);

// return a random number between 0x00000000 and 0xffffffff
DLL_DECLSPEC unsigned ZeroRand();

// return a random number between 0.0f and 1.0f
DLL_DECLSPEC float ZeroRandFloat();

// return a random number distributed about the given mean, with the
// given standard deviation.
DLL_DECLSPEC float ZeroRandNormal( float mean, float std_dev );

#endif
