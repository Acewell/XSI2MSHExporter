#include "ZeroTypes.h"
#include "ZeroRecord.h"
#include "ZeroCRC.h"
#include "ZeroDebug.h"
#include "ZeroPredict.h"
#include "ZeroFile.h"
#include "ZeroMemory.h"

#if (PLATFORM == PLATFORM_GAMECUBE)
static void ZeroRecord_Init(void);
static void ZeroRecord_Done(void);
#else
static void ZeroRecord_Init(void)
{
}
static void ZeroRecord_Done(void)
{
}
#endif

static int phase = 1;

static bool jFirst = true;
static bool jRecord;

class JournalFile {
public:
	static JournalFile *Open(char *name, bool record);

	virtual ~JournalFile() { }
	virtual bool IsOpen() { return false; }
	virtual void Write(void *data, int size) { }
	virtual void Read(void *data, int size) { }
};

static JournalFile *journal;

#define STRING_FORMAT " = \"%[^\"]\""

int GetConfigInt(char *name, bool record)
{
	int r;
	ZeroValue::Send(ZeroValue::IO_OFF);
	ZeroFile cfg("config.ini", "r");
	ZeroValue::Send(ZeroValue::IO_ON);
	if (!cfg)
		r = 0;
	else if (cfg.ReadSectionToken ("[default]", name, " = %d", &r) != 1)
		r = 0;
	if (record)
		ZeroJournal::Record(r);
	return r;
}

void GetConfigStr(char *name, char *dflt, char *value, int size)
{
	ZeroFile cfg("config.ini", "r");
	char str[256];
	if (!cfg || cfg.ReadSectionToken ("[default]", name, STRING_FORMAT, str) != 1)
	{
		strcpy(value, dflt);
	}
	else
	{
		if (size > 256)
			size = 256;
		str[--size] = 0;
		strcpy(value, str);
	}
}

static void CloseJournal(void)
{
	if (!journal)
		return;
	delete journal;
	journal = NULL;
}

static void OpenJournal(void)
{
	// only open the journal once
	if (!jFirst)
		return;
	jFirst = false;
	ZeroRecord_Init();
	// set up the requested journal mode
	int mode = GetConfigInt("journalMode", false);
	if (mode == 0)
	{
		journal = NULL;
		jRecord = false;
		return;
	}
	jRecord = (mode == 1);
	// open it
	char jName[32];
	sprintf(jName, "journal%d", phase);
	journal = JournalFile::Open(jName, jRecord);
	// deal with file problem
	if (!journal)
	{
		jRecord = false;
		return;
	}
	// register close function
	atexit(CloseJournal);
}

void ZeroJournal::Record(void *data, int size)
{
	OpenJournal();
	if (!journal)
		return;
	if (jRecord)
		journal->Write(data, size);
	else
		journal->Read(data, size);
	if (!journal->IsOpen())
	{
		ZeroTrace(("journal closed\n"));
		delete journal;
		journal = NULL;
	}
}

void ZeroJournal::Record(char &data)
{
	ZeroJournal::Record(&data, sizeof(data));
	ZeroValue::Record(data, ZeroValue::EXTENDED);
}

void ZeroJournal::Record(short &data)
{
	ZeroJournal::Record(&data, sizeof(data));
	ZeroValue::Record(data, ZeroValue::EXTENDED);
}

void ZeroJournal::Record(int &data)
{
	ZeroJournal::Record(&data, sizeof(data));
	ZeroValue::Record(data, ZeroValue::EXTENDED);
}

void ZeroJournal::Record(float &data)
{
	ZeroJournal::Record(&data, sizeof(data));
	ZeroValue::Record(data, ZeroValue::EXTENDED);
}

bool ZeroJournal::NotPlaying(void)
{
	return jRecord || (journal == NULL);
}

static bool vFirst = true;
static bool vRecord;
static int valueCount;
static ZeroValue::Type vLevel;
static int cmpDisableCount = 0;
static int ioDisableCount = 0;
static int brkDisableCount = 0;

static unsigned int crcSeed;
static int curStream = 0;
static int crcSeeds[32];

static JournalFile *values;

static void CloseValue(void)
{
	if (!values)
		return;
	delete values;
	values = NULL;
}

static void OpenValue(void)
{
	if (!vFirst)
		return;
	vFirst = false;
	ZeroRecord_Init();
	// set default to no values
	values = NULL;
	vLevel = ZeroValue::NEVER;
	// see if values are on
	int mode = GetConfigInt("valueMode", false);
	if (mode == 0)
	{
		vRecord = false;
		return;
	}
	vRecord = (mode == 1);
	char vName[32];
	sprintf(vName, "value%d", phase);
	values = JournalFile::Open(vName, vRecord);
	if (!values)
	{
		vRecord = false;
		return;
	}
	int level = GetConfigInt("valueLevel");
	vLevel = (ZeroValue::Type)level;
	atexit(CloseValue);
}


static bool Recording(ZeroValue::Type level)
{
	// don't record if io is disabled
	if (ioDisableCount != 0)
		return false;
	// don't record values from a higher detail level than requested
	if (level > vLevel)
		return false;
	// don't record never level
	if (level == ZeroValue::NEVER)
		return false;
	// if this is a sync value make sure sync values are valid
	if ((level <= ZeroValue::FULL) && ZeroPredict::inProgress)
		return false;
	// it must be ok
	return true;
}

inline bool NetSync(ZeroValue::Type level)
{
	return (level != ZeroValue::EXTENDED);
}

static void OutOfSync(void)
{
#if (PLATFORM == PLATFORM_X86_WIN32)
	throw "out of sync";
#elif (PLATFORM == PLATFORM_PS2)
	printf("out of sync");
	ZeroBreak();
#elif (PLATFORM == PLATFORM_GAMECUBE)
	OSHalt("Journal out of sync");
#endif
}

template <class T>
void RecordValue(T &value, ZeroValue::Type level)
{
	OpenValue();
	if (!Recording(level))
		return;
	++valueCount;
	if (valueCount == -1)
		ZeroBreak();
	if (!values)
		return;
	if (vRecord)
	{
		values->Write(&value, sizeof(value));
	}
	else
	{
		T oldValue;
		values->Read(&oldValue, sizeof(oldValue));
		if ((cmpDisableCount == 0) && (oldValue != value))
		{
			if (brkDisableCount == 0)
				ZeroBreak();
			OutOfSync();
		}
	}
	if (!values->IsOpen())
	{
		ZeroTrace(("values closed\n"));
		delete values;
		values = NULL;
	}
}

void ZeroValue::Record(char value, Type level)
{
	if (NetSync(level))
		crcSeed = CalcCRC((unsigned char *)&value, sizeof(value), crcSeed);
	RecordValue(value, level);
}

void ZeroValue::Record(short value, Type level)
{
	if (NetSync(level))
		crcSeed = CalcCRC((unsigned char *)&value, sizeof(value), crcSeed);
	RecordValue(value, level);
}

void ZeroValue::Record(int value, Type level)
{
	if (NetSync(level))
		crcSeed = CalcCRC((unsigned char *)&value, sizeof(value), crcSeed);
	RecordValue(value, level);
}

void ZeroValue::Record(float value, Type level)
{
	if (NetSync(level))
		crcSeed = CalcCRC((unsigned char *)&value, sizeof(value), crcSeed);
	RecordValue(value, level);
}

void ZeroValue::SetCheckSum(int seed)
{
	crcSeed = seed;
}

int ZeroValue::GetCheckSum(void)
{
	return crcSeed;
}

void ZeroValue::SetStream(int stream)
{
	if (stream == curStream)
		return;
	crcSeeds[curStream] = crcSeed;
	curStream = stream;
	crcSeed = crcSeeds[curStream];
}

void ZeroValue::Send(Event e)
{
	switch (e) {
	case CMP_OFF:
		++cmpDisableCount;
		break;
	case CMP_ON:
		--cmpDisableCount;
		break;
	case IO_OFF:
		++ioDisableCount;
		break;
	case IO_ON:
		--ioDisableCount;
		break;
	case BRK_OFF:
		++brkDisableCount;
		break;
	case BRK_ON:
		--brkDisableCount;
		break;
	}
}

void CloseRecording(void)
{
	CloseJournal();
	CloseValue();
}

void ZeroRecord_PreDump(void)
{
	CloseJournal();
	CloseValue();
	//
	jFirst = true;
	vFirst = true;
}

void ZeroRecord_PrepLoad(void)
{
	CloseJournal();
	CloseValue();
}

void ZeroRecord_PostLoad(void)
{
	++phase;
}

#if (PLATFORM == PLATFORM_X86_WIN32)
// Windows

class WinJournalFile : public JournalFile {
public:
	WinJournalFile(char *name, bool record)
	{
		char path[256];
		sprintf(path, "%s.dat", name);
		f = fopen(path, jRecord ? "wb" : "rb");
	}

	bool IsOpen(void)
	{
		return (f != NULL);
	}

	~WinJournalFile()
	{
		if (f)
			fclose(f);
	}

	void Write(void *data, int size)
	{
		fwrite(data, 1, size, f);
	}

	void Read(void *data, int size)
	{
		fread(data, 1, size, f);
	}

	FILE *f;
};

JournalFile *JournalFile::Open(char *name, bool record)
{
	WinJournalFile *j = ZeroNew (WinJournalFile(name, record));
	if (!j->IsOpen())
	{
		delete j;
		return NULL;
	}
	return j;
}

#elif (PLATFORM == PLATFORM_XBOX)
JournalFile *JournalFile::Open(char *name, bool record)
{
	return NULL;
}

#elif (PLATFORM == PLATFORM_PS2)
// Playstation 2

#include <eekernel.h>
#include <sifdev.h>

#include "../zlib/zlib.h"

#include "ZeroBatch.h"

static void *ZAlloc(voidpf opaque, uInt items, uInt size)
{
	if (ZeroBatch::DidPreset())
		return NewMemory_malloc(items * size, false);
	else
		return NewMemory_malloc(items * size, true);
}

static void ZFree(voidpf opaque, voidpf address)
{
	NewMemory_free(address);
}

class PS2JournalFile : public JournalFile {
public:
	PS2JournalFile(char *name, bool record)
	{
		isOpen = false;
		recording = record;
		char path[256];
		sprintf(path, "host0:.\\PS2\\%s.DAT", name);
#if 0
		char *p = strrchr(path, '\\');
		while (*p)
		{
			if (isalpha(*p))
				*p = toupper(*p);
			++p;
		}
#endif
		memset(&stream, 0, sizeof(stream));
		stream.zalloc = ZAlloc;
		stream.zfree = ZFree;
		if (record)
		{
			// printf("record\n");
			if ((f = sceOpen(path, SCE_WRONLY | SCE_CREAT | SCE_TRUNC)) < 0)
				return;
			stream.next_out = (Bytef*)buffer;
			stream.avail_out = sizeof(buffer);
			int err = deflateInit(&stream, Z_BEST_SPEED);
			nextFlush = 10*1024;
		}
		else
		{
			// printf("playback %s\n", path);
			if ((f = sceOpen(path, SCE_RDONLY)) < 0)
			{
				// printf("open failed\n");
				return;
			}
			stream.avail_in = 0;
			int err = inflateInit(&stream);
		}
		isOpen = true;
		used = 0;
	}

	bool IsOpen(void)
	{
		return isOpen;
	}

	~PS2JournalFile()
	{
		if (isOpen)
		{
			if (recording)
			{
				// flush the last bits of data
				while (true)
				{
					// if no room in buffer, send it
					if (stream.avail_out == 0)
					{
						sceWrite(f, buffer, sizeof(buffer));
						stream.next_out = (Bytef*)buffer;
						stream.avail_out = sizeof(buffer);
					}
					// compress as much as possible
					int err = deflate(&stream, Z_FINISH);
					if (err == Z_STREAM_END)
						break;
				}
				deflateEnd(&stream);
				int n = stream.next_out - (Bytef*)buffer;
				if (n > 0)
					sceWrite(f, buffer, n);
			}
			else
			{
				inflateEnd(&stream);
			}
			sceClose(f);
		}
	}

	void Write(void *data, int size)
	{
		stream.next_in = (Bytef*)data;
		stream.avail_in = (uInt)size;
		while (stream.avail_in > 0)
		{
			// if no room in buffer, send it
			if (stream.avail_out == 0)
			{
				sceWrite(f, buffer, sizeof(buffer));
				stream.next_out = (Bytef*)buffer;
				stream.avail_out = sizeof(buffer);
			}
			// copy as much as possible to buffer
			int flush = Z_NO_FLUSH;
			if (stream.total_in > nextFlush)
			{
				flush = Z_SYNC_FLUSH;
				nextFlush = stream.total_in + 10*1024;
			}
			int err = deflate(&stream, flush);
			if (err != Z_OK)
				ZeroBreak();
		}
	}

	void Read(void *data, int size)
	{
		stream.next_out = (Bytef*)data;
		stream.avail_out = size;
		while (stream.avail_out > 0)
		{
			if (stream.avail_in == 0)
			{
				stream.avail_in = sceRead(f, buffer, sizeof(buffer));
				stream.next_in = (Bytef*)buffer;
			}
			int err = inflate(&stream, Z_FINISH);
		}
	}

	bool isOpen;
	bool recording;
	int f;
	char buffer[2048];
	int used;
	z_stream stream;
	int nextFlush;
};

JournalFile *JournalFile::Open(char *name, bool record)
{
	PS2JournalFile *j = ZeroNew (PS2JournalFile(name, record));
	if (!j->IsOpen())
	{
		delete j;
		return NULL;
	}
	return j;
}

#elif (PLATFORM == PLATFORM_GAMECUBE)
// Game Cube

#include "zlib.h"

// these will be in FastLoad.cpp for now. later they should be put
// in a more appropriate file

int USB_Open(char *name);
void USB_Close(int fd);
int USB_Write(int fd, void *data, int size);

//

class JAllocator {
public:
	enum {
		POOLS = 6,
		SIZE1 = 100,
		SIZE2 = 1300,
		SIZE3 = 12000,
		SIZE4 = 33000,
		SIZE5 = 66000,
		SIZE6 = 3	// not used yet
	};
	
	char *orig;
	char *base;
	char *limit;
	
	char *pools[POOLS];
	
	JAllocator(void)
	{
		orig = NULL;
	}
	
	~JAllocator()
	{
		if (orig)
			ZeroFree(orig);
	}
	
	void Init(void)
	{
		// only init once
		if (orig)
			return;
		int size = 276000*2;
		char *buffer = (char *)ZeroMalloc(size, 1);
		//
		orig = buffer;
		base = orig;
		limit = buffer + size;
		for (int i = 0; i < POOLS; i++)
			pools[i] = NULL;
	}
	
	int AssignPool(unsigned int &size)
	{
		int sizes[] = { SIZE1, SIZE2, SIZE3, SIZE4, SIZE5, SIZE6 };
		for (int i = 0; i < POOLS; i++)
		{
			if (size <= sizes[i])
			{
				size = sizes[i];
				return i;
			}
		}
		OSHalt("");
		return -1;
	}
	
	void *malloc(unsigned int size)
	{
		// add room for size field
		size += sizeof(size);

		// see if we have a pooled block available
		void *ptr;
		int index = AssignPool(size);
		if (index >= 0)
		{
			if (pools[index])
			{
				ptr = pools[index];
				pools[index] = *(char **)pools[index];
				return ptr;
			}
		}
		OSReport("jalloc: %d,0x%x\n", size, base + sizeof(size));
		
		// allocate a new block
		*(unsigned int *)base = size;
		ptr = (void *)(base + sizeof(size));
		base += size;
		if (base > limit)
			OSHalt("Not enough mem\n");

		return ptr;
	}
	
	void free(void *ptr)
	{
		unsigned int size = *((unsigned int *)ptr - 1);
		
		int index = AssignPool(size);
		if (index >= 0)
		{
			*(char **)ptr = pools[index];
			pools[index] = (char *)ptr;
		}
		else
		{
			// we are not prepared to handle freeing
			// OSReport("free: %d\n", size);	
			OSHalt("");
		}
	}

	void Report(void)
	{
		OSReport("zalloc used %d\n", base - orig);
	}
	
};

JAllocator jAlloc;

static void *ZAlloc(voidpf opaque, uInt items, uInt size)
{
	return jAlloc.malloc(items * size);
}

static void ZFree(voidpf opaque, voidpf address)
{
	jAlloc.free(address);
}

#define BUFFER_SIZE 2048

char gcJournalMem[(BUFFER_SIZE+220)*2];

class GCJournalFile : public JournalFile {
public:
	
	void *operator new(size_t size)
	{
		char *p = gcJournalMem;
		while (p < gcJournalMem + sizeof(gcJournalMem))
		{
			GCJournalFile *ptr = (GCJournalFile *)p;
			if (!ptr->inUse)
				return ptr;
			p += size;
		}
		return NULL;
	}
	
	void operator delete(void *ptr)
	{
		// marked !inUse in destructor
	}
	
	GCJournalFile(char *name, bool record)
	{
		inUse = true;
		jAlloc.Init();
		buffer = (char *)(((u32)buf + 31) & ~31);
		recording = record;
		char path[256];
		sprintf(path, "%s.dat", name);
		isOpen = false;
		memset(&stream, 0, sizeof(stream));
		stream.zalloc = ZAlloc;
		stream.zfree = ZFree;
		if (record)
		{
			fd = USB_Open(path);
			if (fd < 0)
				return;
			stream.next_out = (Bytef*)buffer;
			stream.avail_out = BUFFER_SIZE;
			int err = deflateInit(&stream, Z_BEST_SPEED);
			nextFlush = 10*1024;
		}
		else
		{
			int entry = ZeroFile::FindCookie(path);
			if (entry < 0)
				return;
			DVDFastOpen(entry, &dvdFile);
			stream.avail_in = 0;
			int err = inflateInit(&stream);
		}
		isOpen = true;
//		used = 0;
	}

	~GCJournalFile()
	{
		if (isOpen)
		{
			if (recording)
			{
				// flush the last bits of data
				while (true)
				{
					// if no room in buffer, send it
					if (stream.avail_out == 0)
					{
						DCFlushRange(buffer, BUFFER_SIZE);
						USB_Write(fd, buffer, BUFFER_SIZE);
						stream.next_out = (Bytef*)buffer;
						stream.avail_out = BUFFER_SIZE;
					}
					// compress as much as possible
					int err = deflate(&stream, Z_FINISH);
					if (err == Z_STREAM_END)
						break;
				}
				int n = stream.next_out - (Bytef*)buffer;
				if (n > 0)
				{
					DCFlushRange(buffer, BUFFER_SIZE);
					USB_Write(fd, buffer, n);
				}
				USB_Close(fd);
			}
			else
			{
				DVDClose(&dvdFile);
			}
		}
		jAlloc.Report();
		inUse = false;
	}

	bool IsOpen(void)
	{
		return isOpen;
	}

	void Write(void *data, int size)
	{
		stream.next_in = (Bytef*)data;
		stream.avail_in = (uInt)size;
		while (stream.avail_in > 0)
		{
			// if no room in buffer, send it
			if (stream.avail_out == 0)
			{
				DCFlushRange(buffer, BUFFER_SIZE);
				USB_Write(fd, buffer, BUFFER_SIZE);
				stream.next_out = (Bytef*)buffer;
				stream.avail_out = BUFFER_SIZE;
			}
			// copy as much as possible to buffer
			int flush = Z_NO_FLUSH;
			if (stream.total_in > nextFlush)
			{
				flush = Z_SYNC_FLUSH;
				nextFlush = stream.total_in + 10*1024;
			}
			int err = deflate(&stream, flush);
		//	if (err != Z_OK)
		//		ZeroBreak();
		}
	}

	void Read(void *data, int size)
	{
		stream.next_out = (Bytef*)data;
		stream.avail_out = size;
		while (stream.avail_out > 0)
		{
			if (stream.avail_in == 0)
			{
				int size = BUFFER_SIZE;
				int left = DVDGetLength(&dvdFile) - stream.total_in;
				if (size > left)
					size = left;
				stream.avail_in = DVDRead(&dvdFile, buffer, size, stream.total_in);
				stream.next_in = (Bytef*)buffer;
				if (stream.avail_in == 0)
				{
					isOpen = false;
					DVDClose(&dvdFile);
					return;
				}
			}
			int err = inflate(&stream, Z_FINISH);
		}
	}

	bool inUse;	
	bool isOpen;
	bool recording;
	int fd;
	char *buffer;
	char buf[BUFFER_SIZE+32];
//	int used;
	z_stream stream;
	int nextFlush;
	DVDFileInfo dvdFile;
};

JournalFile *JournalFile::Open(char *name, bool record)
{
	GCJournalFile *j = ZeroNew (GCJournalFile(name, record));
	if (!j->IsOpen())
	{
		delete j;
		j = NULL;
	}
	return j;
}

static void ZeroRecord_Init(void)
{
#ifdef _DEBUG
	jAlloc.Init();
#endif
}

// i dont know where to put a call to this yet
static void ZeroRecord_Done(void)
{
}

// the reason memory use seems overly complicated here is because i want need
// the memory footprint to be the same with journal recording/journal playing
// and no journaling

#endif
