//********************************************************************
// Direct geometry Hardware independent library
//	By Julio Jerez
// VC: 6.0
//********************************************************************
#ifndef __ZeroInputs__
#define __ZeroInputs__

#include "ZeroTypes.h"
//#include <dinput.h>

/*
#define AltKey		0x01
#define CtrlKey	0x02
#define ShiftKey	0x02

struct ZeroInputEvent				  
{
	unsigned keyboardMake:  1;
	unsigned keyboardBreak: 1;
};


typedef void (__cdecl *InputEventCallback) (const ZeroInputEvent& event);

class ZeroInputs: public ZeroUnknown
{
	unsigned char keyboardBuffer0[256];
	unsigned char keyboardBuffer1[256];
	LPDIRECTINPUT7 lpdi;
	LPDIRECTINPUTDEVICE7 keyboard; 
	
	
	DLL_DECLSPEC ~ZeroInputs();
public:
	DLL_DECLSPEC ZeroInputs(HWND win, HINSTANCE hinst);
	
	DLL_DECLSPEC void Update();
	
	DLL_DECLSPEC int RegisterKeyboardEvent (InputEventCallback fnt, unsigned scanCode, unsigned extendedKeyFlag = 0);
};
*/
#endif
