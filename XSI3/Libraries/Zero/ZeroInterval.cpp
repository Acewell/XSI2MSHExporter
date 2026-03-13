#include "ZeroTypes.h"
#include "ZeroDebug.h"
#include "ZeroInterval.h"
#include "ZeroTime.h"

#ifdef INTERVAL_FUNC

#ifdef _DEBUG
#define SHOW_DEBUG_WINDOW  // Show output in debug window instead of file
#else
#define SHOW_DEBUG_WINDOW  // Show output in debug window instead of file
#endif // _DEBUG

// Collect statistics at each frame, and when buffer overflows
// (or time has expired) then dump the structure to file or screen
// Intervals tracked are identified by a unique labels.(id)
// Each unique 4-char (32-bit int) label defines a interval to be tracked
// For each frame, intervals are added to the structure (one for each label used).
// Since not all labels are used in each frame (which is a bad design
// if you want to do optimization), each label may not have same number of
// measured intervals.

#define MAX_FRAME		128
#define MAX_INTERVAL	64

//#define USE_TIMEGETTIME
#ifdef USE_TIMEGETTIME
	typedef unsigned int INTERVAL_T;
	#define ELAPSED_T(i,j) (intervalTable.interval[i].elapsedTime[j])
	#define CURRENT_T(t) t=GetTickCount();
	#define CLEAR_T(t) t=0;
	#define UPDATE_T(elapsed,end,start) elapsed+=end-start;
	#define NOTZERO_T(t) t
	#define ELAPSED_F(j) (intervalTable.frame.elapsedTime[j])
#else
	typedef LARGE_INTEGER INTERVAL_T;
	#define ELAPSED_T(i,j) ((double)intervalTable.interval[i].elapsedTime[j].QuadPart*freq_to_ms)
#ifdef WIN32
	#define CURRENT_T(t) QueryPerformanceCounter(&t);
#else
	#define CURRENT_T(t) (t.QuadPart = tTimer_GetCount(tTIMER_MILLISECOND));
#endif
	#define CLEAR_T(t) t.QuadPart=0;
	#define UPDATE_T(elapsed,end,start) elapsed.QuadPart=elapsed.QuadPart+end.QuadPart-start.QuadPart;
	#define NOTZERO_T(t) (t.QuadPart!=0)
	#define ELAPSED_F(j) ((double)intervalTable.frame.elapsedTime[j].QuadPart*freq_to_ms)
#endif //USE_TIMEGETTIME

struct INTERVAL_INFO
{
	union										// Interval id
	{
		unsigned int	dw[2];
		char				sz[8];
	}id;
	int levelsOfRecursion;
	// Both ZERO if not used in a frame, both POSITIVE if used
	INTERVAL_T	elapsedTime[MAX_FRAME];		// Accumulated elapsed time
	INTERVAL_T	startTime[MAX_FRAME];		// Last time started
	INTERVAL_T	endTime[MAX_FRAME];			// Last time ended
};

struct INTERVAL_TABLE
{
	int				frameIndex;					// Frame being filled right now
	int				frameCount;					// Number of frames filled so far
	int				frameTotalCount;			// Number of frames filled so far
	int				newLabel;					// If a new label was added since last dump
	int				dumpToFile;					// Dump to a FILE instead of CRT
	unsigned int	timerExpires;				// If dumping to CRT on periodic basis...
	INTERVAL_INFO	frame;						// Frame time
	int				idCount;					// Number of "labels" tracked
	INTERVAL_INFO	interval[MAX_INTERVAL];		// Info for each "label" tracked
};

static INTERVAL_TABLE	intervalTable;
static unsigned int		firstTickCount; 
static LARGE_INTEGER	largeFreq;
static double			freq_to_ms;

static FILE	*fout;

void interval_init()
{
	memset(&intervalTable, 0, sizeof(intervalTable));
#ifdef SHOW_DEBUG_WINDOW
	intervalTable.dumpToFile = false;
#else
	// If dumping to a FILE, then open the file
	intervalTable.dumpToFile = true;
	if ((fout = fopen ("interval.txt", "w")) == 0)
//		DEBUG_systemError ("unable to open interval.txt");
#endif //SHOW_DEBUG_WINDOW
	CLEAR_T(intervalTable.frame.elapsedTime[intervalTable.frameIndex]);
	CURRENT_T(intervalTable.frame.startTime[intervalTable.frameIndex]);
#ifdef WIN32
	QueryPerformanceFrequency(&largeFreq);
#else
	largeFreq.QuadPart = 1000;
#endif

#ifdef WIN32
	_ASSERTE(largeFreq.LowPart != 0 || largeFreq.HighPart != 0);
#else
	_ASSERTE(largeFreq.intPart.LowPart != 0 || largeFreq.intPart.HighPart != 0);
#endif
	freq_to_ms = 1000./(double)largeFreq.QuadPart;
}

void interval_close()
{
	// Close file
	if (fout)
	{
		if	(intervalTable.frameCount > 0 && intervalTable.dumpToFile)
			interval_dump();
		fclose(fout);
		fout = NULL;
	}
}

void interval_begin_frame()
{
}

void interval_in(char *str)
{
	INTERVAL_INFO		*pInfo;
	int					i;

	// Check if label already in the table, and if so then set the new start time
	for (pInfo = &intervalTable.interval[0], i = intervalTable.idCount; i > 0; i--, pInfo++)
		if (pInfo->id.dw[0] == *((unsigned int *)str))
		{
			pInfo->levelsOfRecursion ++;
			if (pInfo->levelsOfRecursion == 1) {
				CURRENT_T(pInfo->startTime[intervalTable.frameIndex]);
				CLEAR_T(pInfo->endTime[intervalTable.frameIndex]);
			}
			return;
		}



	// If label not found, then add it to the end of the table, then set the start time
	_ASSERTE(intervalTable.idCount < MAX_INTERVAL);
	pInfo = &intervalTable.interval[intervalTable.idCount];
	pInfo->id.dw[0] = *((unsigned int *)&str[0]);
//	pInfo->id.dw[1] = *((unsigned int *)&str[4]);
	pInfo->id.sz[4] = 0; // ...truncate after 4 chars

	pInfo->levelsOfRecursion = 1;
	CURRENT_T(pInfo->startTime[intervalTable.frameIndex]);
	CLEAR_T(pInfo->endTime[intervalTable.frameIndex]);
	CLEAR_T(pInfo->elapsedTime[intervalTable.frameIndex]);
	intervalTable.idCount++;
	intervalTable.newLabel = 1;
}

void interval_out(char *str)
{
	INTERVAL_INFO	*pInfo;
	int				i;

	// Check if label already in the table
	for (pInfo = &intervalTable.interval[0], i = intervalTable.idCount; i > 0; i--, pInfo++)
		if (pInfo->id.dw[0] == *((unsigned int *)str))
		{
			pInfo->levelsOfRecursion --;
			_ASSERTE (pInfo->levelsOfRecursion	>= 0);

			if (pInfo->levelsOfRecursion == 0) {
				CURRENT_T(pInfo->endTime[intervalTable.frameIndex]);
				UPDATE_T(pInfo->elapsedTime[intervalTable.frameIndex],
							pInfo->endTime[intervalTable.frameIndex],
							pInfo->startTime[intervalTable.frameIndex]);
			}
			return;
		}

	// If label not found, then abort with error
	_ASSERTE(0 || pInfo->id.dw[0] == *((unsigned int *)str));
}

void interval_end_frame(void)
{   
	// Save how int this frame took
	CURRENT_T(	intervalTable.frame.endTime[intervalTable.frameIndex]);
	UPDATE_T(	intervalTable.frame.elapsedTime[intervalTable.frameIndex],
				intervalTable.frame.endTime[intervalTable.frameIndex],
				intervalTable.frame.startTime[intervalTable.frameIndex]);

	// Advance to next frame in the table
	intervalTable.frameIndex = (intervalTable.frameIndex + 1) % MAX_FRAME;
	intervalTable.frameCount++;
	intervalTable.frameTotalCount++;

	// Dump the table if table overflowed (or timer for CRT dump has expired)
    // Do this about once a second, unless the array overflows
	if	(intervalTable.frameCount >= MAX_FRAME
		|| (!intervalTable.dumpToFile && intervalTable.frameCount > 0 && 
#ifdef WIN32
		GetTickCount() 
#else
		tTimer_GetCount(tTIMER_MILLISECOND)
#endif
		
		> intervalTable.timerExpires)
		)
	{ 
		interval_dump();
#ifdef WIN32
		intervalTable.timerExpires = GetTickCount() + 1000;
#else
		intervalTable.timerExpires = tTimer_GetCount(tTIMER_MILLISECOND) + 1000;
#endif
		intervalTable.frameIndex = 0;
		intervalTable.frameCount = 0;

		INTERVAL_INFO	*pInfo;
		int				i;
		for (pInfo = &intervalTable.interval[0], i = intervalTable.idCount; i > 0; i--, pInfo++)
		{
			memset(&pInfo->elapsedTime[0], 0, MAX_FRAME * sizeof(INTERVAL_T));
			memset(&pInfo->endTime[0], 0, MAX_FRAME * sizeof(INTERVAL_T));
		}
	}

	// ...next frame timing starts at the end for previous frame timing
	CLEAR_T(intervalTable.frame.elapsedTime[intervalTable.frameIndex]);
	CURRENT_T(intervalTable.frame.startTime[intervalTable.frameIndex]);

#if 0	// removed until we figure out what it's actually doing
	// Redraw performance stats
	PERF_REDRAW
		
	// Redraw init system
	INIT_REDRAW
#endif
} 

void interval_dump()
{
	static int	labelsDone = 0;
	int			i, j;
	char		str[512];
	float		timeSum, timeAvg, timeFrm;
	int			framesUsed;

	if (intervalTable.dumpToFile && fout == NULL)
		return;

	// If first time to file, or to screen, then do labels
	if (!(intervalTable.dumpToFile && labelsDone) || intervalTable.newLabel)
	{
		// Labels
		sprintf(str, "Frame ");
		for (i = 0; i < intervalTable.idCount; i++)
			sprintf(&str[strlen(str)], " %5s", intervalTable.interval[i].id.sz);

		sprintf(&str[strlen(str)], "   Sum  Totl\n");

		if (intervalTable.dumpToFile)
			fprintf (fout, str);
		else
			OutputDebugString(str);

		labelsDone = 1;
	}


	if (intervalTable.dumpToFile)
	{
		for (j = 0; j < intervalTable.frameCount; j++)
		{
			// Frame number
			sprintf(str, "%5d", intervalTable.frameTotalCount-intervalTable.frameCount+j);

			// Values for individual labels
			for (timeSum = 0, i = 0; i < intervalTable.idCount; i++)
			{
				if (NOTZERO_T(intervalTable.interval[i].endTime[j]))
				{
					timeAvg = (float)ELAPSED_T(i,j);
					timeSum = timeSum + timeAvg;
					if		(timeAvg >= 10.)	sprintf(&str[strlen(str)], " %5.1f", timeAvg);
					else						sprintf(&str[strlen(str)], " %5.2f", timeAvg);
				}
				else
				{
					sprintf(&str[strlen(str)], "     0");
				}
			}

			// Total frame time (and total time seen in all intervals)
			timeFrm = (float)ELAPSED_F(j);
			sprintf(&str[strlen(str)], " %5.1f %5.1f\n", timeSum, timeFrm);

			// Dump the next line of data
			fprintf (fout,"%s", str);
		}
	}
	else
	{
		// Frame number
		sprintf(str, "%4d:", intervalTable.frameCount);

		// Values for individual labels
		for (timeSum = 0, i = 0; i < intervalTable.idCount; i++)
		{
			timeAvg		= 0;
			framesUsed	= 0;
			//...average over all frames in which this label was used
			for (j = 0; j < intervalTable.frameCount; j++)
			{
				if (NOTZERO_T(intervalTable.interval[i].endTime[j]))
				{
					timeAvg = timeAvg + (float)ELAPSED_T(i,j);
					framesUsed++;
				}
			}
			if (framesUsed)
			{
				timeAvg = timeAvg / intervalTable.frameCount;
				timeSum = timeSum + timeAvg;
				if (timeAvg >= 10.0f)
				{
					sprintf(&str[strlen(str)], " %5.1f", timeAvg);
				}
				else
				{
					sprintf(&str[strlen(str)], " %5.2f", timeAvg);
				}
			}
			else
			{
				sprintf(&str[strlen(str)], "      -");
			}
		}

		// Average total frame time (and total time seen in all intervals)
		for (timeFrm = 0, j = 0; j < intervalTable.frameCount; j++)
			timeFrm = timeFrm + (float)ELAPSED_F(j);
		timeFrm = timeFrm/intervalTable.frameCount;
		sprintf(&str[strlen(str)], " %5.1f %5.1f\n", timeSum, timeFrm);

		// Dump the next line of data
		OutputDebugString(str);
		//intervalTable.idCount = 0; //force new labels on next set of frames
	}

	intervalTable.newLabel = 0;
}

#endif //INTERVAL_FUNC

struct INTERVAL {
	char heading[8];
	LARGE_INTEGER stamp;
};

bool perfMissing = false;
static LARGE_INTEGER startTime;
static LARGE_INTEGER finishTime;
static bool first = true;
static double freq;
static INTERVAL val;

void simple_interval_in(char *str)
{
#ifdef _WIN32
	if (!perfMissing)
	{
		QueryPerformanceCounter(&val.stamp);
		strncpy(val.heading, str, 8);
		val.heading[7] = 0;
	}
#else
#endif
}

void simple_interval_out(char *str)
{
#ifdef _WIN32
	if (!perfMissing)
	{
		QueryPerformanceCounter(&finishTime);
		unsigned int diff;
		if (val.stamp.HighPart == finishTime.HighPart)
			diff = finishTime.LowPart - val.stamp.LowPart;
		else
			diff = finishTime.LowPart + (0xffffffff - val.stamp.LowPart);
		if (first)
		{
			first = false;
			LARGE_INTEGER largeFreq;
			QueryPerformanceFrequency(&largeFreq);
			if (largeFreq.LowPart == 0)
			{
				perfMissing = true;
				return;
			}
			freq = (double)largeFreq.LowPart;
		}
		float secs = (float)((double)diff / freq);
		char str[64];
		sprintf("%s %.3fs\n", val.heading, secs);
		OutputDebugString(str);
	}
#else
#endif
}

// Benchmark stuff
POLY_STATS	polyStats;

#define BENCHMARK_FLAG_ENABLED			0x0001
#define BENCHMARK_FLAG_STARTED			0x0002
#define BENCHMARK_FLAG_COMPLETED		0x0004
#define BENCHMARK_FLAG_CRTSTATS			0x0008
#define BENCHMARK_FLAG_TRACKFPS			0x0010	// Save frame number and its duration
#define BENCHMARK_FLAG_TRACKTRIS		0x0020	// Save stats on triangle counts
#define BENCHMARK_FLAG_DONOTQUIT		0x0040	// Do not quit when framesToDraw reached
#define BENCHMARK_FLAG_TRIANGLECOUNT	0x0080	// Display triangle counts

enum BSTATES
{
	BENCHMARK_STATE_SKIPPING,
	BENCHMARK_STATE_RUNNING,
	BENCHMARK_STATE_DONE,
	BENCHMARK_STATE_LAST
};
typedef struct tagBENCHMARK_INFO
{
	int		flags;
	// Control parameters
	unsigned int	framesToDraw;	// Collect statistics for this many frames
	unsigned int	framesToSkip;	// Don't start collecting statistics until after this many frames
	unsigned int	frameToExit;	// If non-zero, will quit program after "frameToExit" frames
	unsigned int	timeStep;		// Each frame represents this many mili-seconds
	unsigned int	frameNumber;	// Current frame (and time)
	char	fileName[256];
	// Run time data
	BSTATES	state;
	unsigned int	lastTime, minTime, maxTime, startTime;
	float	*fpsHistory;
} BENCHMARK_INFO;

static BENCHMARK_INFO	benchMark;
bool					doBenchMark;

static void PolyStatsUpdate(TRI_COUNTS *c)
{
	if (c->last == 0)
	{
		c->min = c->total;
		c->max = c->total;
	}
	else
	{
		unsigned int elapsed = c->total - c->last;
		if (elapsed < c->min) c->min = elapsed;
		if (elapsed > c->max) c->max = elapsed;
	}
	c->last = c->total;
}

void Init_Benchmark()
{
#if (PLATFORM == PLATFORM_X86_WIN32)
	char	*fileName = "fps.cfg",
			*secName  = "Benchmark";

	// Clear info structure
	memset(&benchMark, 0, sizeof(benchMark));
	memset(&polyStats, 0, sizeof(polyStats));
	if (!doBenchMark)
		return;

	// Setup default values
	benchMark.flags			= BENCHMARK_FLAG_ENABLED;
	benchMark.framesToDraw	= 200;
	benchMark.framesToSkip	= 30;
	benchMark.frameToExit	= benchMark.framesToDraw + benchMark.framesToSkip;
	benchMark.timeStep		= 30;
	benchMark.frameNumber	= 0;
	strcpy(benchMark.fileName, "fps.txt");
	//If enabled, read parameter file
	/*  ...ParameterDB::GetYYYY() does not read from current directory
	int		tmpInt;
	ParameterDB::Open(fileName);
	ParameterDB::GetInt(fileName, secName, "framesToDraw",	(int *)&benchMark.framesToDraw,	2000);
	ParameterDB::GetInt(fileName, secName, "framesToSkip",	(int *)&benchMark.framesToSkip,	30);
	ParameterDB::GetInt(fileName, secName, "doNotQuit",		(int *)&tmpInt,					0);
	if (tmpInt) benchMark.flags |= BENCHMARK_FLAG_DONOTQUIT;
	ParameterDB::GetInt(fileName, secName, "triangleCount",	&tmpInt,					0);
	if (tmpInt) benchMark.flags |= BENCHMARK_FLAG_TRIANGLECOUNT;
	ParameterDB::Close(fileName);
	*/
	char tmpName[256];
	GetCurrentDirectory(sizeof(tmpName), tmpName);
	strcat(tmpName, "\\");
	strcat(tmpName, fileName);
	benchMark.framesToDraw	= GetPrivateProfileInt(secName, "framesToDraw",	2000,	tmpName);
	benchMark.framesToSkip	= GetPrivateProfileInt(secName, "framesToSkip",	100,	tmpName);
	benchMark.timeStep		= GetPrivateProfileInt(secName, "timeStep",		30,		tmpName);
	if (GetPrivateProfileInt(secName, "doNotQuit",	0,		tmpName))
		benchMark.flags |= BENCHMARK_FLAG_DONOTQUIT;

	//Allocate memory for histogram

	benchMark.minTime	= 0x7fffffff;
	benchMark.maxTime	= 0;
	benchMark.lastTime	= ZeroTime::GetSysClock();
	benchMark.state = BENCHMARK_STATE_SKIPPING;
#endif
}

void Close_Benchmark()
{
#ifdef _WIN32
	if (!doBenchMark)
		return;
	//Free histogram
#else
#endif
}

unsigned int Get_Benchmark_Time(void)
{
	_ASSERTE(doBenchMark);
	return benchMark.frameNumber * benchMark.timeStep;
}

void Dump_Benchmark(char *fileName, unsigned int frameCount, unsigned int timeElapsed, unsigned int minFrameTime, unsigned int maxFrameTime)
{
#ifdef _WIN32
	// Open file
	FILE	*fout;
	if ((fout = fopen (fileName, "w")) == 0)
		throw "unable to open fps.txt";

	// Write frame info
	fprintf (fout,"             Average        Min        Max      Total\n");
	fprintf (fout,"Frames      %8dfps%8dfps%8dfps%8dframes\n",
		(int)(0.5f + (frameCount*1000.f)/((float)timeElapsed)),
		(int)(0.5f + 1000.f/maxFrameTime),
		(int)(0.5f + 1000.f/minFrameTime),
		frameCount);

	// Write time info
	fprintf (fout,"Time        %8dms %8dms %8dms %8dms\n",
		(int)(0.5f + ((float)timeElapsed)/((float)frameCount)),
		minFrameTime,
		maxFrameTime,
		timeElapsed);
	// Write polygon info
	fprintf (fout, "TRIANGLE COUNTS\n");
	TRI_COUNTS	&c = polyStats.toCullNoClip;
	fprintf (fout,"ToCullNoClip%8d   %8d   %8d   %8d\n",
		(int)(0.5f + ((float)c.total)/(3.f*(float)frameCount)),
		(int)(c.min/3.f), (int)(c.max/3.f), (int)(c.total/3.f));

	c = polyStats.toCullClip;
	fprintf (fout,"ToCullClip  %8d   %8d   %8d   %8d Triangles\n",
		(int)(0.5f + ((float)c.total)/(3.f*(float)frameCount)),
		(int)(c.min/3.f), (int)(c.max/3.f), (int)(c.total/3.f));

	c = polyStats.toLight;
	fprintf (fout,"ToLight     %8d   %8d   %8d   %8d Vertices\n",
		(int)(0.5f + ((float)c.total)/((float)frameCount)), c.min, c.max, c.total);

	c = polyStats.toClip;
	fprintf (fout,"ToClip      %8d   %8d   %8d   %8d Triangles\n",
		(int)(0.5f + ((float)c.total)/(3.f*(float)frameCount)),
		(int)(c.min/3.f), (int)(c.max/3.f), (int)(c.total/3.f));

	c = polyStats.toRender;
	fprintf (fout,"ToRender    %8d   %8d   %8d   %8d Triangles\n",
		(int)(0.5f + ((float)c.total)/(3.f*(float)frameCount)),
		(int)(c.min/3.f), (int)(c.max/3.f), (int)(c.total/3.f));

	// Close file and return
	fclose(fout);
#else
#endif
}

void Update_Benchmark()
{
#ifdef _WIN32
	if (!doBenchMark)
		return;

	unsigned int nowTime = ZeroTime::GetSysClock();

	benchMark.frameNumber++;
	switch(benchMark.state)
	{
	case BENCHMARK_STATE_SKIPPING:
		if (benchMark.frameNumber >= benchMark.framesToSkip)
		{
			memset(&polyStats, 0, sizeof(polyStats));
			benchMark.startTime = nowTime;
			benchMark.state = BENCHMARK_STATE_RUNNING;
		}
		break;

	case BENCHMARK_STATE_RUNNING:
		if (benchMark.frameNumber >= benchMark.framesToDraw + benchMark.framesToSkip)
		{
			// Save frame rates
			Dump_Benchmark("fps.txt",
				benchMark.framesToDraw, nowTime - benchMark.startTime,
				benchMark.minTime, benchMark.maxTime);
			benchMark.state = BENCHMARK_STATE_DONE;
		}
		else
		{
			unsigned int	elapsedTime = nowTime - benchMark.lastTime;
			if (elapsedTime < benchMark.minTime) benchMark.minTime = elapsedTime;
			if (elapsedTime > benchMark.maxTime) benchMark.maxTime = elapsedTime;

			PolyStatsUpdate(&polyStats.toCullNoClip);
			PolyStatsUpdate(&polyStats.toCullClip);
			PolyStatsUpdate(&polyStats.toClip);
			PolyStatsUpdate(&polyStats.toLight);
			PolyStatsUpdate(&polyStats.toRender);
			PolyStatsUpdate(&polyStats.toDX);
		}
		break;

	case BENCHMARK_STATE_DONE:
		break;
	}

	benchMark.lastTime = nowTime;
#else
#endif
}

