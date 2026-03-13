//********************************************************************
// Direct geometry Hardware independent library
//	By Julio Jerez
// VC: 6.0
//********************************************************************
#include "ZeroTypes.h"
/*
#include "ZeroList.h"
#include "Zerodebug.h"
#include "ZeroInputs.h"

struct EVENT_ENTRY
{
	int code;
	InputEventCallback fnt;
	
	EVENT_ENTRY	(
		int scanCode, 
		unsigned extendedKeyFlag, 
		InputEventCallback fntArg)
	{
		code = (extendedKeyFlag & 0xf) << 8;
		code += (scanCode & 0xff);
		fnt = fntArg;
	}
};

struct InputEventList: public ZeroList<EVENT_ENTRY>
{
	InputEventList()
	{
	}
	
	~InputEventList()
	{
	}
};


static InputEventList* inputEventList = NULL;

ZeroInputs::ZeroInputs (HWND win, HINSTANCE hinst)
:ZeroUnknown ("dxinputs") 
{
	HRESULT res;
	
	if (inputEventList) {
		throw "Input already exits";
	}
	
	inputEventList	= new InputEventList;
	
	
	lpdi = NULL;
	keyboard = NULL; 
	
	res = DirectInputCreateEx (hinst, DIRECTINPUT_VERSION, IID_IDirectInput7,
		(void**) &lpdi,  NULL);
	
	if (res != DI_OK) {
		throw "unable to create dxinput";
	}
	
	res = lpdi->CreateDeviceEx (GUID_SysKeyboard, IID_IDirectInputDevice7,
		(void**)&keyboard, NULL); 
	
	if (res != DI_OK) {
		throw "unable to create dxinput";
	}
	
	res = keyboard->SetDataFormat(&c_dfDIKeyboard); 
	if (res != DI_OK) {
		throw "unable to create dxinput";
	}
	
	//	res = keyboard->SetCooperativeLevel(win, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); 
	res = keyboard->SetCooperativeLevel(win, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE); 
	if (res != DI_OK) {
		throw "unable to create dxinput";
	}
	
	res = keyboard->Acquire();
	if (res != DI_OK) {
		throw "unable to create dxinput";
	}
	
	Update();
	memcpy (keyboardBuffer0, keyboardBuffer1, sizeof (keyboardBuffer0));
}


ZeroInputs::~ZeroInputs ()
{
	delete inputEventList;
	
	if (keyboard) {
		keyboard->Unacquire(); 
		keyboard->Release();
	}
	if (lpdi) {
		lpdi->Release();
	}
}

int ZeroInputs::RegisterKeyboardEvent (
									   InputEventCallback fnt, 
									   unsigned scanCode, 
									   unsigned extendedKeyFlag)
{
	EVENT_ENTRY	entry (scanCode, extendedKeyFlag, fnt);
	
	inputEventList->Append (entry);
	
	return 0;
}


void ZeroInputs::Update()
{
	HRESULT res;
	unsigned keyCode; 
	unsigned extendedCode;
	ZeroInputEvent	event;
	
	res = keyboard->GetDeviceState (sizeof(keyboardBuffer1), keyboardBuffer1);
	_ASSERTE (res == DI_OK);
	
	extendedCode = 0;
	if (keyboardBuffer1[DIK_LCONTROL] != keyboardBuffer1[DIK_LCONTROL]) {
		extendedCode |= CtrlKey;
	}
	if (keyboardBuffer1[DIK_RCONTROL]) {
		extendedCode |= CtrlKey;
	}
	
	if (keyboardBuffer1[DIK_LSHIFT]) {
		extendedCode |= ShiftKey;
	}
	
	if (keyboardBuffer1[DIK_RSHIFT]) {
		extendedCode |= ShiftKey;
	}
	
	if (keyboardBuffer1[DIK_LALT]) {
		extendedCode |= AltKey;
	}
	
	if (keyboardBuffer1[DIK_RALT]) {
		extendedCode |= AltKey;
	}
	
	unsigned Break;
	unsigned Make;
	
	InputEventList::Iterator iter (*inputEventList);
	for (iter.Begin(); iter; iter ++) {
		keyCode = (*iter).code;
		
		Make = keyboardBuffer1[keyCode & 255] >> 7;
		Break = keyboardBuffer0[keyCode & 255] >> 7;
		if (Make || Break) {
			if (extendedCode == (keyCode >> 8)) {
				event.keyboardMake = Make & ~Break;
				event.keyboardBreak = ~Make & Break;
				(*iter).fnt(event);
				//				ZeroTrace ("%d %d %d\n", event.keyboardMake, event.keyboardBreak, event.keyboardDown);
			}
			keyboardBuffer0[keyCode & 255] = keyboardBuffer1[keyCode & 255];
		}
	}
}
*/
