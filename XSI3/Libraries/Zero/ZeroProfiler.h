#ifndef ZeroProfiler_H
#define ZeroProfiler_H


namespace ZeroProfiler
{
	void Start(const char* aName);
	void End(const char* aName);
	void Update(double aInterval = 1.0);
	
	void BeginReport(void);
	bool NextReport(const char *&aName, const char *&aInfo);
	//int GetNumReportStrings();
	//const char* GetReportName(int aId);
	//const char* GetReportInfo(int aId);

	void Flush(void);

	//void Report(ZeroCamera* aCamera);
};


#endif