#include "ZeroTypes.h"
#include "ZeroProfiler.h"
#include "ZeroList.h"
#include "ZeroTime.h"
#include "ZeroCRC.h"



class ZeroProfilerData
{
public:

	double mStartTime;
	double mTime;
	unsigned int mCrc;
	char mName[128];
	unsigned short mNumCalls;
	char mPresentationLine[256];
	ZeroProfilerData* mParent;

	ZeroProfilerData(unsigned int aCrc, const char* aName, unsigned int aChildLevel, ZeroProfilerData* aParent)
	{
		char childStack[100];
		unsigned int t;

		if(aChildLevel > 5)
			aChildLevel = 5;
		for(t=0; t < aChildLevel; t++)
		{
			childStack[t] = '-';
		}
		childStack[t] = 0;

		sprintf(mName, "%s%s", childStack, aName);
		mCrc = aCrc;
		Reset();
		mPresentationLine[0] = 0;
		mParent = aParent;
	}

	void SetStart(double aTime)
	{
		mStartTime = aTime;
		mNumCalls++;
	}

	void SetEnd(double aTime)
	{
		mTime += aTime - mStartTime;
	}

	void Reset()
	{
		mStartTime = 0;
		mTime = 0;
		mNumCalls = 0;
	}
};



namespace ZeroProfiler
{
	ZeroVisList<ZeroProfilerData> mList;
	ZeroVisList<ZeroProfilerData>::Iterator mIter(mList);
	double mStartTime = 0;
	int mFrameCount;
	int mNumOpen;
	ZeroProfilerData* mParent = NULL;

	ZeroProfilerData* GetData(const char* aName)
	{
#if (PLATFORM == PLATFORM_PS2)
		return NULL;
#else
		ZeroVisList<ZeroProfilerData>::Iterator it(mList);

		unsigned int crc = CalcLowerCRC(aName);

		//Try to find it
		it.Begin();
		while(it)
		{
			if((*it).mCrc == crc)
			{
				return &(*it);
			}
			it++;
		}

		//Create a new one, sorted in the list
		if(mParent != NULL)
		{
			//Try to find it
			it.Begin();
			while(it)
			{
				if(&(*it) == mParent)
				{
					mList.InsertAfter(ZeroProfilerData(crc, aName, mNumOpen, mParent), it.GetNode());
					it++;
					return &(*it);
				}
				it++;
			}
		}

		mList.Append(ZeroProfilerData(crc, aName, mNumOpen, mParent));
		return &(mList.GetLast()->info);
#endif
	}

	double GetClock()
	{
#if (PLATFORM == PLATFORM_PS2)
		return 0;
#else
		return (double)ZeroTime::GetSysClock() * 0.001;
#endif
	}
};



void ZeroProfiler::Start(const char* aName)
{
#if (PLATFORM != PLATFORM_PS2)
	ZeroProfilerData* data = GetData(aName);
	mParent = data;
	data->SetStart(GetClock());
	mNumOpen++;
#endif
}


void ZeroProfiler::End(const char* aName)
{
#if (PLATFORM != PLATFORM_PS2)
	ZeroProfilerData* data = GetData(aName);
	mParent = data->mParent;
	data->SetEnd(GetClock());
	mNumOpen--;
#endif
}


void ZeroProfiler::Update(double aInterval)
{
#if (PLATFORM != PLATFORM_PS2)
	double time = GetClock() - mStartTime;
	mFrameCount++;
	if(time > aInterval)
	{
		if(mStartTime != 0)
		{
			ZeroVisList<ZeroProfilerData>::Iterator it(mList);

			it.Begin();
			while(it)
			{
				sprintf((*it).mPresentationLine, "%03.0f (%03.01f calls)",  (float)((*it).mTime / time) * 100.0f, float((*it).mNumCalls)/mFrameCount);
				(*it).Reset();
				it++;
			}
		}
		mStartTime += time;
		mFrameCount = 0;
	}
#endif
}


void ZeroProfiler::BeginReport(void)
{
	mIter.Begin();
}

bool ZeroProfiler::NextReport(const char *&aName, const char *&aInfo)
{
	if (mIter)
	{
		aName = (*mIter).mName;
		aInfo = (*mIter).mPresentationLine;
		mIter++;
		return true;
	}
	return false;
}

/*
int ZeroProfiler::GetNumReportStrings()
{
	return mList.GetCount();
}


const char* ZeroProfiler::GetReportName(int aId)
{
	ZeroVisList<ZeroProfilerData>::Iterator it(mList);
	it.Begin();
	while(it && aId > 0)
	{
		it++;
		aId--;
	}

	if(it)
		return (*it).mName;

	return NULL;
}


const char* ZeroProfiler::GetReportInfo(int aId)
{
	ZeroVisList<ZeroProfilerData>::Iterator it(mList);
	it.Begin();
	while(it && aId > 0)
	{
		it++;
		aId--;
	}

	if(it)
		return (*it).mPresentationLine;

	return NULL;
}
*/

void ZeroProfiler::Flush(void)
{
	mList.RemoveAll();
}

/*
void ZeroProfiler::Report(ZeroCamera* aCamera)
{
	ZeroVisList<ZeroProfilerData>::Iterator it(mList);

	ZeroColor fgColor (255, 255, 0, 255);
	ZeroColor bgColor (0, 0, 0, 127);
	Font* font = FontSys::GetFont("Console");


	int y = 100;
	int x = 450;
	int len;

	it.Begin();
	while(it)
	{
		len = strlen((*it).mName);
		int size = font->Width((*it).mName, len) + 5;
		font->Draw(aCamera, x + 1 - size, y + 1, (*it).mName, len, bgColor);
		font->Draw(aCamera, x - size, y, (*it).mName, len, fgColor);
		len = strlen((*it).mPresentationLine);
		font->Draw(aCamera, x + 1, y + 1, (*it).mPresentationLine, len, bgColor);
		font->Draw(aCamera, x, y, (*it).mPresentationLine, len, fgColor);
		it++;
		y += 11;
	}			
}

*/
