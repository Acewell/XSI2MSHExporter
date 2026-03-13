//********************************************************************
// Direct geometry Hardware independent library
//	By Julio Jerez
// VC: 6.0
//********************************************************************
#ifndef __ZeroFilters__
#define __ZeroFilters__

#include "ZeroSets.h"

class ZeroHysterisis
{
	int mem;
	float minVal;
	float maxVal;
	
public:
	ZeroHysterisis ()
	{
		SetLimits (0.5f, 0.7f);
	}
	
	void SetLimits (float Min, float Max)
	{
		_ASSERTE (Min < Max);
		mem = 0;
		maxVal = Max;
		minVal = Min;
	}
	
	int Evalue (float t)
	{
		if (t < minVal) {
			mem = 0;
		} else if (t > maxVal) {
			mem = 1;
		}
		return mem;
	}
};



// y(n)_= b0 * x(n) - a1 * y(n-1) - a2 * y(n-2)
class ZeroIIR2Filter
{
protected:
	float a1;
	float a2;
	float b0;
	float my1;
	float my2;
	
public:
	
	ZeroIIR2Filter (float kb0, float ka1, float ka2 = 0) 
	{
		a1 = ka1;
		a2 = ka2;
		b0 = kb0;
		SetMemories ();
	}
	
	void SetMemories (float y1=0, float y2=0) {
		my1 = y1;
		my2 = y2;
	}
	float Value()
	{
		return my1;
	}
	
	DLL_DECLSPEC float Update (float x);
};

class ZeroLPFilter: public ZeroIIR2Filter
{
public:
	ZeroLPFilter (float kb0):ZeroIIR2Filter (kb0, kb0 - 1.0f){}
};

class ZeroLP2Filter: public ZeroIIR2Filter
{
public:
	DLL_DECLSPEC ZeroLP2Filter (float Damping, float Period);
};
/*				 
class LinearFilter: public ZeroSet<float>
{
	protected:
	int index;
	ZeroSet<float> memories;

	public:
	LinearFilter (int taps) 
		:ZeroSet<float>(taps), memories(taps)
	{
		int i;
		index = 0;
		for (i = 0; i < taps; i ++) {
			memories[i] = 0.0;
			(*this)[i] = 0.0f;
		}
	}

	LinearFilter (
		int taps, 
		float* Kernel)
		:ZeroSet<float>(taps), memories(taps)
	{
		int i;
	
		index = 0;
		for (i = 0; i < taps; i ++) {
			memories[i] = 0.0;
			(*this)[i] = Kernel[i];
		}
	}

	void Normalize ()
	{
		int i;
		int size;
		float area;
	
		size = GetSize();
		area = 0;
		for (i = 0; i < size ; i ++) {
			area += (*this)[i];
		}
	
		if (area < 1.0e-20) {
			area = 1.0e-20f;
		}
		Scale (1.0f / area);
	}


	void Scale (float scale)
	{
		int i;
		int size;
	
		size = GetSize();
		for (i = 0; i < size ; i ++) {
			(*this)[i] *= scale;
		}
	}

	float Update (float value)
	{
		int i;
		int j;
		int mod;
		float retVal;
		float* kernel;
		float* memory;
	
		retVal = 0;
		mod = GetSize();
		kernel = &(*this)[0];
		memory = &memories[0];
	
		memory[index] = value;
	
		index ++;
		if (index >= mod) {
			index = 0;
		}
	
		for (i = 0; i < mod; i ++) {
			j = index + i;
			if (j >= mod) {
				j = 0;
			}
			retVal += kernel[i] * memory[j];
		}
		return retVal;
	}
};

class MedianFilter: public LinearFilter
{
	public:
	MedianFilter::MedianFilter (int taps)
		:LinearFilter (taps + 1)
	{
		(*this)[0] = -1.0e15f;
	}

	float Update (float value)
	{
	
		int i;
		int j;
		int mod;
		float a;
		float* kernel;
		float* memory;
	
		a = 0;
		mod = GetSize() - 1;
	
		memory = &memories[0];
		memory[index] = value;
		index ++;
		if (index >= mod) {
			index = 0;
		}
	
		kernel = &(*this)[1];
		memcpy (kernel, memory, mod * sizeof (float));
	
		for (i = 1; i < mod; i ++) {
			a = kernel[i];
			for (j = i; a < kernel[j - 1]; j --) {
				_ASSERTE (j >= 0);
				_ASSERTE (j < mod);
				kernel[j] = kernel[j - 1];
			}
			_ASSERTE (j >= 0);
			_ASSERTE (j < mod);
			kernel[j] = a;
		}
		return kernel[1 + (mod >> 1)];
	}
};
*/
#endif
