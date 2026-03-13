#ifndef INTERVAL_H
#define INTERVAL_H

#ifdef _DEBUG
//#define INTERVAL_FUNC		// Undefine this to disable interval profiling
#define INTERVAL_DETAIL_2	// Use this to get detailed intervals (see below what detail means)
#else
//#define INTERVAL_FUNC		// Undefine this to disable interval profiling
#define INTERVAL_DETAIL_0	// Use this to get detailed intervals (see below what detail means)
#endif //_DEBUG

#ifdef INTERVAL_FUNC

extern DLL_DECLSPEC void interval_init(void);
extern DLL_DECLSPEC void interval_close(void);
extern DLL_DECLSPEC void interval_begin_frame(void);
extern DLL_DECLSPEC void interval_in(char *str);
extern DLL_DECLSPEC void interval_out(char *str);
extern DLL_DECLSPEC void interval_end_frame(void);
extern DLL_DECLSPEC void interval_dump(void);
extern DLL_DECLSPEC void simple_interval_in(char *str);
extern DLL_DECLSPEC void simple_interval_out(char *str);

#define INIT_INTERVAL()		interval_init()
#define CLOSE_INTERVAL()	interval_close()
#define BEGIN_FRAME()		interval_begin_frame()
#define INTERVAL_START(x)	interval_in(x)
#define INTERVAL_STOP(x)	interval_out(x)
#define DUMP_INTERVAL()		interval_dump()
#define END_FRAME()			interval_end_frame()
#define TIMER_START(x)		simple_interval_in(x)
#define TIMER_STOP(x)		simple_interval_out(x)

#else

#define INIT_INTERVAL()
#define CLOSE_INTERVAL()
#define BEGIN_FRAME()
#define INTERVAL_START(x)
#define INTERVAL_STOP(x)
#define DUMP_INTERVAL()
#define END_FRAME()
#define TIMER_START(x)
#define TIMER_STOP(x)

#endif //INTERVAL_FUNC

// Detail
#ifdef INTERVAL_DETAIL_0								// Lowest level of detail (sum should be same as total)
#define INTERVAL0_IN(x)		INTERVAL_START(x);
#define INTERVAL0_OUT(x)	INTERVAL_STOP(x);
#define INTERVAL1_IN(x)
#define INTERVAL1_OUT(x)
#define INTERVAL2_IN(x)
#define INTERVAL2_OUT(x)
#endif // INTERVAL_DETAIL_0

#ifdef INTERVAL_DETAIL_1								// Medium level (some overlap)
#define INTERVAL0_IN(x)		INTERVAL_START(x)
#define INTERVAL0_OUT(x)	INTERVAL_STOP(x)
#define INTERVAL1_IN(x)		INTERVAL_START(x)
#define INTERVAL1_OUT(x)	INTERVAL_STOP(x)
#define INTERVAL2_IN(x)
#define INTERVAL2_OUT(x)
#endif // INTERVAL_DETAIL_1

#ifdef INTERVAL_DETAIL_2								// Most output (intervals within intervals)
#define INTERVAL0_IN(x)		INTERVAL_START(x)
#define INTERVAL0_OUT(x)	INTERVAL_STOP(x)
#define INTERVAL1_IN(x)		INTERVAL_START(x)
#define INTERVAL1_OUT(x)	INTERVAL_STOP(x)
#define INTERVAL2_IN(x)		INTERVAL_START(x)
#define INTERVAL2_OUT(x)	INTERVAL_STOP(x)
#endif // INTERVAL_DETAIL_2

#define INTERVAL_IN(x) INTERVAL2_IN(x)
#define INTERVAL_OUT(x) INTERVAL2_OUT(x)

// Benchmark function prototypes
DLL_DECLSPEC void Init_Benchmark(void);
DLL_DECLSPEC void Close_Benchmark(void);
DLL_DECLSPEC void Update_Benchmark(void);
DLL_DECLSPEC unsigned int Get_Benchmark_Time(void);

extern bool doBenchMark;

struct TRI_COUNTS
{
	unsigned int	last;
	unsigned int	min;
	unsigned int	max;
	unsigned int	avg;
	unsigned int	total;
};

struct POLY_STATS
{
	TRI_COUNTS	toCullNoClip;
	TRI_COUNTS	toCullClip;
	TRI_COUNTS	toLight;
	TRI_COUNTS	toClip;
	TRI_COUNTS	toRender;
	TRI_COUNTS	toDX;
};

extern POLY_STATS	polyStats;
#endif	// INTERVAL_H

