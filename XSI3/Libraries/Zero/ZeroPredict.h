#ifndef ZEROPREDICT_H
#define ZEROPREDICT_H

class ZeroPredict {
public:
	// init module
	static void Init(void);

	// clean module
	static void Clean(void);

	// begin predicting
	static DLL_DECLSPEC void Begin(void);

	// end prediction
	static DLL_DECLSPEC void End(void);

	// is a prediction in progress
	static DLL_DECLSPEC bool inProgress;

	// allocate state that will be saved before prediction and
	// restored afterwards
	static DLL_DECLSPEC void *AllocState(unsigned int size);

	// free state
	static DLL_DECLSPEC void FreeState(void *ptr);

	// mark memory that needs to be saved and restored
	static DLL_DECLSPEC void MarkState(void *ptr, unsigned int size);

	// clear marked memory
	static DLL_DECLSPEC void UnmarkState(void *ptr);
};

#endif
