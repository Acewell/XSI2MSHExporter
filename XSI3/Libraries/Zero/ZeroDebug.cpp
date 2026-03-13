/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"
#include "ZeroDebug.h"
#include "ZeroMathTypes.h"


#ifdef _DEBUG

static char lastAssertMessage[1024];

void __cdecl __ExpandTraceMessage__  (char *fmt, ...)
{
	va_list v_args;

#ifdef _WIN32
	unsigned ctrl;
	ctrl = ZeroGetCtrlWorld (); 
#endif

	va_start (v_args, fmt);     
	vsprintf(lastAssertMessage, fmt, v_args);
	va_end (v_args);            
		
#ifdef _WIN32
	ZeroSetCtrlWorld (ctrl); 
#endif
}

bool __cdecl __ExpandAssertMessage__ (unsigned fCondition, char *fmt, ...)
{
	va_list v_args;

	if (!fCondition) {
#ifdef _WIN32
		unsigned ctrl;
		ctrl = ZeroGetCtrlWorld (); 
#endif

		va_start (v_args, fmt);     
		vsprintf(lastAssertMessage, fmt, v_args);
		va_end (v_args);            
		
#ifdef _WIN32
		ZeroSetCtrlWorld (ctrl); 
#endif
		return true;
	}
	return false;
}

bool __cdecl __ExpandAssertMessage__ (const void *fCondition, char *fmt, ...)
{
	va_list v_args;

	if (!fCondition) {
#ifdef _WIN32
		unsigned ctrl;
		ctrl = ZeroGetCtrlWorld (); 
#endif

		va_start (v_args, fmt);     
		vsprintf(lastAssertMessage, fmt, v_args);
		va_end (v_args);            

#ifdef _WIN32
		ZeroSetCtrlWorld (ctrl); 
#endif
		return true;
	}
	return false;
}



char *__cdecl __GetAssertMessage__ ()
{
	return lastAssertMessage;
}


#endif
