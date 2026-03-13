#ifndef __ZeroCallStack__
#define __ZeroCallStack__

namespace ZeroCallStack
{
	void Init(void);
	void Done(void);

	void Trace(char aBuffer[], unsigned aSize, unsigned aLevels, unsigned aSkip = 1);
}

#endif
