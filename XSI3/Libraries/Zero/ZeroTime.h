#ifndef _TIMERS_H_
#define _TIMERS_H_

#include "ZeroTypes.h"

namespace ZeroTime
{
	// read the system clock time
	DLL_DECLSPEC unsigned GetSysClock(void);

	// initialize time system
	DLL_DECLSPEC void Init(void);

	// set clock time
	DLL_DECLSPEC void Start(unsigned dw);

	// pause clock time
	DLL_DECLSPEC void Pause(void);

	// resume clock time
	DLL_DECLSPEC void Resume(void);

	// is time paused?
	DLL_DECLSPEC bool IsPaused(void);

	// set current clock time
	DLL_DECLSPEC void Update(void);

	// get current time in milliseconds
	DLL_DECLSPEC unsigned GetTimeMs(void);

	// get time step in milliseconds
	DLL_DECLSPEC unsigned GetStepMs(void);

	// get current time in seconds
	DLL_DECLSPEC float GetTime(void);

	// get time step in seconds
	DLL_DECLSPEC float GetStep(void);

	// get inverse time step in hz
	DLL_DECLSPEC float GetStepInv(void);

	// increment current world time
	DLL_DECLSPEC void AddTime(unsigned ms);

	// add time to the real time clock
	DLL_DECLSPEC void AddRealTime(int ms);

	// Set the wait time
	DLL_DECLSPEC void SetWaitTime(unsigned ms);

	// return the frame number since the game start
	DLL_DECLSPEC unsigned GetFrame(void);

	// sleep for a given number of ms (is there a better module for this)
	DLL_DECLSPEC void Sleep(int ms);

	// convert time (in sec.) to a string
	DLL_DECLSPEC void ToString(const float aTime, char* aOutStr);
}

#endif
