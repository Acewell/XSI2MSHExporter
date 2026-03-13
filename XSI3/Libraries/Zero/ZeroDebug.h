/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroDebug__
#define __ZeroDebug__
#include "ZeroTypes.h"

#ifdef _SHOW_REMINDS

	//-----------------------
	// Use this as shown to send a reminder to another developer
	// (or to yourself).  The message will show up in the compile
	// window when the program is compiled.  The programmer can
	// then double-click on the message to go to the message's
	// location.
	//
	// As a convention use the format FROM->TO MESSAGE
	// For example:
	//
	//	 #pragma REMIND("JOE->JANE  Add accessor functions!")
	//
	//-----------------------

	#define QUOTE(x)   #x
	#define QUOTE_2(y)  QUOTE(y)
	#define REMIND(str)         message(__FILE__ "(" QUOTE_2(__LINE__) ") : " str)

#else
	#define REMIND(str)    
#endif // _SHOW_REMINDS


	
#endif
