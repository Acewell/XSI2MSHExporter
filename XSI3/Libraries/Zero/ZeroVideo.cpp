//********************************************************************
// Direct geometry Hardware independent library
//	By Julio Jerez
// VC: 6.0
//********************************************************************
#include "ZeroTypes.h"
#include "ZeroDebug.h"
#include "ZeroVideo.h"
#include "ZeroCache.h"
#include "ZeroCamera.h"
#include "ZeroTextureLight.h"

namespace InternalVideo
{
	class VideoCreator;
//	static VideoCreator *videoCache;

	class VideoCreator: public ZeroCache<ZeroVideoConstructor, void *>
	{
		VideoCreator ()
			:ZeroCache<ZeroVideoConstructor, void *>()
		{
		}
//		friend void Startup();
		friend VideoCreator* GetVideoCreator();
	};

	VideoCreator* GetVideoCreator() 
	{
		static VideoCreator videoCacheLocal;
		return &videoCacheLocal;
	}

	void Startup() 
	{
//		static VideoCreator videoCacheLocal;
//		videoCache = &videoCacheLocal;
		GetVideoCreator();
	}
}


ZeroVideoConstructor::ZeroVideoConstructor(const char *name, const void *)
	:ZeroUnknown ()
{
	_ASSERTE (0);
	const char *ptr;
	char string[256];

	ptr = strrchr (name, '\\');
	if (ptr) {
		ptr ++;
	} else {
		ptr = name;
	}


	strcpy (string, ptr);
	strupr (string);
	strtok (string, ".");

	ZeroTrace (("Loading Library \"%s\"\n", string));

	crc = CalcStrCRC (string);
	InternalVideo::GetVideoCreator()->AddItem (crc, this);
}

ZeroVideoConstructor::~ZeroVideoConstructor ()
{
	InternalVideo::GetVideoCreator()->EvictItem (crc);
}


ZeroVideo::ZeroVideo (HWND window)
	:ZeroUnknown ()
#ifdef WIN32
		, videoModesList()
#endif
{
	win = window;

	ZeroCamera::SetVideo (this); 
}



ZeroVideo* __cdecl ZeroVideo::CreateVideo (
	const char *deviceName, 
	HWND win)
{
	char tmpName[256];
	ZeroVideoConstructor *constructor;

	strcpy (tmpName, deviceName);
	strupr (tmpName);

	constructor	= InternalVideo::GetVideoCreator()->FindItem (tmpName); 
	return constructor ? constructor->CreateVideo (win) : NULL;
}

ZeroTxLight* ZeroVideo::CreateTxLight(int aShape, ZeroUnknown &aData)
{
	return ZeroTxLightShape::Build(aShape, aData);
}
