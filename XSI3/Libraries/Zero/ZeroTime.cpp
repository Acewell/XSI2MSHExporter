#include "ZeroTypes.h"
#include "ZeroTime.h"
#include "ZeroRecord.h"

namespace ZeroTime
{
	// forward declaration
	static void CalcTimes(void);

	// current time in milliseconds
	static unsigned dwTime;

	// time step in milliseconds
	static unsigned dwStep;

	// current time in seconds
	static float fTime;

	// time step in seconds
	static float fStep;
	
	// inverse time step in hz
	static float fStepInv;

	// is the time system paused?
	static bool bPaused;

	// previous time in milliseconds
	static unsigned dwPrevious;

	// pause time in milliseconds
	static unsigned dwWait;

	// adjustment time in milliseconds
	static int dwAdjust = 0;

	// current frame number
	static unsigned dwFrame;

#if ((PLATFORM == PLATFORM_X86_WIN32) || (PLATFORM == PLATFORM_XBOX))
	// performance counter variables
	static double dPeriod;
	static __int64 llStart;
	static bool bUsePerf;
#else
	static unsigned extraMs;
	static unsigned lastTicks;
#endif

	//
	// Function:
	//     GetSysClock
	// Purpose:
	//     Get the system time in milliseconds
	// Arguments:
	//     void
	// Returns:
	//     System time in milliseconds
	//
	unsigned GetSysClock(void)
	{
#if (PLATFORM == PLATFORM_X86_WIN32)
		if (bUsePerf)
		{
			LARGE_INTEGER counter;
			QueryPerformanceCounter (&counter);
			return (unsigned)((counter.QuadPart - llStart) * dPeriod);
		}
		else
		{
			return timeGetTime();
		}
#elif (PLATFORM == PLATFORM_XBOX)
		LARGE_INTEGER counter;
		QueryPerformanceCounter (&counter);
		return (unsigned)((counter.QuadPart - llStart) * dPeriod);

#elif (PLATFORM == PLATFORM_PS2)
		return tTimer_GetCount(tTIMER_MILLISECOND);

		// * 2132 / 128 is like multiplying by 16.66 but the / 128 should be
		// optimized into a shift instead of using floats or divides
		// this assumes vblanks occur at 60hz
//		return tTimer_GetCount(tTIMER_VBLANK_START) * 2132 / 128;

//		unsigned int ticks = tTimer_GetCount(tTIMER_CPU_CYCLE);
//		if (ticks < lastTicks)
//			extraMs += 14559;	// wrapped
//		lastTicks = ticks;
//		float ms = (float)ticks / (float)295000;
//		unsigned ret = (unsigned)ms + extraMs;
//		return ret;
#elif (PLATFORM == PLATFORM_GAMECUBE)
//		return OSTicksToMilliseconds(OSGetTime());
//		return OSTicksToMilliseconds(OSGetTime());
		static OSTime old = OSGetTime();
		static OSTime time = 0;
		OSTime now = OSGetTime();
		time += now - old;
		old = now;
		return (unsigned)OSTicksToMilliseconds(time);
/*		static unsigned old = OSGetTick();
		static unsigned time = 0;
		unsigned now = OSGetTick();
		if(now >= old)
			time += OSTicksToMilliseconds(now - old);
		else //Wrapped
			time += OSTicksToMilliseconds(now + (0xffff - old));
		old = now;
		return time;*/
#endif
	}

	//
	// Function:
	//     Init
	// Purpose:
	//     Initialize the time system
	// Arguments:
	//     void
	// Returns:
	//     void
	//
	void Init(void)
	{
#if (PLATFORM == PLATFORM_X86_WIN32)
		bUsePerf = false;

		LARGE_INTEGER freq;
		if (QueryPerformanceFrequency(&freq))
		{
			bUsePerf = true;

			dPeriod = 1000.0 / (double) freq.QuadPart;

			LARGE_INTEGER count;
			QueryPerformanceCounter (&count);

			llStart = count.QuadPart;
		}

#elif (PLATFORM == PLATFORM_XBOX)
		bUsePerf = true;

		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		dPeriod = 1000.0 / (double) freq.QuadPart;

		LARGE_INTEGER count;
		QueryPerformanceCounter (&count);

        llStart = count.QuadPart;
#elif (PLATFORM == PLATFORM_PS2)
		lastTicks = tTimer_GetCount(tTIMER_CPU_CYCLE);
		extraMs = 0;
#endif

		bPaused = true;
		dwFrame = 0;
		Start(0);
	}

	//
	// Function:
	//     Pause
	// Purpose:
	//     Pause the time system
	// Arguments:
	//     void
	// Returns:
	//     void
	//
	void Pause(void)
	{
		if (!bPaused)
		{
			dwWait = GetSysClock() - dwPrevious;
			ZeroJournal::Record(*(int *)&dwWait);
			bPaused = true;
		}
	}

	//
	// Function:
	//     Resume
	// Purpose:
	//     Resume the time system
	// Arguments:
	//     void
	// Returns:
	//     void
	//
	void Resume(void)
	{
		if (bPaused)
		{
			dwPrevious = GetSysClock() - dwWait;
			ZeroJournal::Record(*(int *)&dwPrevious);
			bPaused = false;
		}
	}

	//
	// Function:
	//     IsPaused
	// Purpose:
	//     Return whether or not the time system is paused
	// Arguments:
	//     void
	// Returns:
	//     true if the time system is paused, false otherwise
	//
	bool IsPaused(void)
	{
		return bPaused;
	}

	//
	// Function:
	//     CalcTimes
	// Purpose:
	//     Calculate derived time values
	// Arguments:
	//     void
	// Returns:
	//     void
	//
	static void CalcTimes(void)
	{
		fTime = (float)dwTime / 1000.0f;
		fStep = (float)dwStep / 1000.0f;
		fStepInv = (fStep != 0.0f) ? 1.0f / fStep : HUGE_NUMBER;
	}

	//
	// Function:
	//     Update
	// Purpose:
	//     Update the current time and time step
	// Arguments:
	//     void
	// Returns:
	//     void
	//
	void Update (void)
	{
		unsigned dwNow = GetSysClock();

		// record time but lose some resolution to save room in the journal
		// dwNow >>= 6;	// temporarily removed by ken
		ZeroJournal::Record((int &)dwNow);
		// dwNow <<= 6;	// temporarily removed by ken

		dwFrame++;

		dwStep = dwNow - dwPrevious;
		dwPrevious = dwNow;

#ifdef _DEBUG
		// see if frame took longer than one second (probably in debugger)
		if (dwStep > 1000)
		{
			dwStep = 30;	// just pretend it took 0.03 seconds
		}
#endif

		if (dwAdjust)
		{
			if (dwAdjust > 0)
			{
				dwStep += dwAdjust;
				dwAdjust = 0; // Don't timeajust until told to again.
			}
			else
			{
				// Make sure we don't wrap around
				dwAdjust = -dwAdjust;
				if((unsigned)dwAdjust > dwStep)
				{
					// Would make dwTimestep negative, bleed from Adjust instead
					dwAdjust = dwStep - dwAdjust;
					dwStep = 0;
				}
				else
				{
					dwStep -= dwAdjust;
					dwAdjust = 0;
				}
			}
		}

		if (!bPaused)
		{
			dwTime += dwStep;
		}

		CalcTimes();
	}

	//
	// Function:
	//     AddTime
	// Purpose:
	//     Add to the current time
	// Arguments:
	//     ms: time in milliseconds
	// Returns:
	//     void
	//
	void AddTime(unsigned ms)
	{
		dwStep = ms;
		dwTime += dwStep;
		CalcTimes();
	}

	//
	// Function:
	//     AddRealTime
	// Purpose:
	//     Add time to the real-time clock
	// Arguments:
	//     ms: time in milliseconds
	// Returns:
	//     void
	//
	void AddRealTime(int ms)
	{
		dwAdjust = ms;
	}

	//
	// Function:
	//     Init
	// Purpose:
	//     Start the time system
	// Arguments:
	//     ms: initial time in milliseconds
	// Returns:
	//     void
	//
	void Start(unsigned ms)
	{
		dwPrevious = GetSysClock();
		ZeroJournal::Record(*(int *)&dwPrevious);
		dwTime = ms;
		dwStep = 100;
		dwAdjust = 0;
		CalcTimes();
	}

	//
	// Function:
	//     GetTimeMs
	// Purpose:
	//     Get the current time in milliseconds
	// Arguments:
	//     void
	// Returns:
	//     current time in milliseconds
	//
	unsigned GetTimeMs(void)
	{
		return dwTime;
	}

	//
	// Function:
	//     GetStepMs
	// Purpose:
	//     Get the time step in milliseconds
	// Arguments:
	//     void
	// Returns:
	//     time step in milliseconds
	//
	unsigned GetStepMs(void)
	{
		return dwStep;
	}

	//
	// Function:
	//     Init
	// Purpose:
	//     Get the current time in seconds
	// Arguments:
	//     void
	// Returns:
	//     current time in seconds
	//
	float GetTime(void)
	{
	//	ZeroValue::Record(fTime, ZeroValue::EXTENDED);
		return fTime;
	}

	//
	// Function:
	//     Init
	// Purpose:
	//     Get the time step in seconds
	// Arguments:
	//     void
	// Returns:
	//     time step in seconds
	//
	float GetStep(void)
	{
	//	ZeroValue::Record(fStep, ZeroValue::EXTENDED);
		return fStep;
	}

	//
	// Function:
	//     Init
	// Purpose:
	//     Get the inverse time step in hz
	// Arguments:
	//     void
	// Returns:
	//     inverse time step in hz
	//
	float GetStepInv(void)
	{
		return fStepInv;
	}


	//
	// Function:
	//     SetWaitTime
	// Purpose:
	//     Set the wait time in milliseconds
	// Arguments:
	//     ms: wait time in milliseconds
	// Returns:
	//     void
	//
	void SetWaitTime(unsigned ms)
	{
		dwWait = ms;
	}

	//
	// Function:
	//     GetFrame
	// Purpose:
	//     Get the current frame number
	// Arguments:
	//     void
	// Returns:
	//     current frame number
	//
	unsigned GetFrame (void)
	{
		return dwFrame;
	}

	void Sleep(int ms)
	{
#ifdef _WIN32
		::Sleep(ms);
#else
		unsigned start = GetSysClock();
		while (true)
		{
			unsigned end = GetSysClock();
			if (end - start >= ms)
				break;
		}
#endif
	}

	void ToString(const float aTime, char* aOutStr)
	{
		int sec = (int)aTime % 60;
		int min;
		int hr = (int)aTime / 3600;
		
		if (hr > 0) {
			min = ((int)aTime % 3600 ) / 60;
			sprintf(aOutStr, "%d:%.2d:%.2d", hr, min, sec);
		}
		else {
			min = (int)aTime / 60;
			sprintf(aOutStr, "%d:%.2d", min, sec);
		}
	}
}
