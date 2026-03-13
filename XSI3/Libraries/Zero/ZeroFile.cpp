/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/

#include "ZeroTypes.h"
#include "ZeroFile.h"
#include "ZeroDebug.h"
#include "ZeroTree.h"
#include "ZeroCRC.h"
#include "ZeroRecord.h"

#include <io.h>
#include <fcntl.h>

static bool doCRC = false;
static ZeroExtCache *extCache;

// the parsing of binary files has not been converted to use the MemFile yet,
// but i dont think we are using binary files that are being parsed at this level.
// i tried to make a binary version of odfs once but the parsing constructs
// were so dynamic that it was actually slower to try to use a binary version.
// actually i dont think we should be doing parsing at this level. orthogonal
// design would suggest that file io and parsing should be separate - wbp

// implement readonly files with the same interface as external files to
// keep code easier to manage.

class MemFile : public ZeroExtFile {
public:
	void *operator new(size_t size)
	{
		// Allocate this as a temporary block if new memory system is on
		return ZEROMALLOC(size, true, false);
		// Was, with old memory manager
		//return ZeroMalloc(size, true);
	}

	void operator delete(void *ptr)
	{
		ZeroFree(ptr);
	}

	MemFile(char *buffer, int size)
	{
		bufBase = buffer;
		bufSize = size;
		bufPtr = bufBase;
	}

	void Release(void)
	{
		delete this;
	}

	int GetSize(void)
	{
		return bufSize;
	}

	int Tell(void)
	{
		return bufPtr - bufBase;
	}

	// seek to a relative offset
	int Seek(int offset)
	{
		bufPtr += offset;
		return bufPtr - bufBase;
	}

	int Read(char *buffer, int size)
	{
		int left = bufSize - Tell();
		int num = min(size, left);
		memcpy(buffer, bufPtr, num);
		bufPtr += num;
		return num;
	}

	char *bufBase;
	int bufSize;
	char *bufPtr;
};

enum {
	TOKEN_EOF,
	TOKEN_EOLN,
	TOKEN_SECTION,
	TOKEN_VAR,
	TOKEN_STRING,
	TOKEN_FLOAT
};

class PathTree : public ZeroVisTree<unsigned, unsigned>
{
public:
	PathTree() { }
	~PathTree() { }
};

static PathTree pathTree;

static void DoInit(void)
{
	static bool didInit = false;
	if (didInit)
		return;
	didInit = true;
	int value = GetConfigInt("doCRC");
	doCRC = (value != 0);
}

ZeroFile::ZeroFile ()
{
	DoInit();
	handle = -1;
	bufBase = NULL;
	bufSize = 0;
	bufPtr = bufBase;
	isBinary = false;
	isBatch = false;
	extFile = NULL;
}

ZeroFile::ZeroFile (const char *Name, const char *type)
{
	DoInit();
	handle = -1;
	bufBase = NULL;
	bufSize = 0;
	bufPtr = bufBase;
	isBinary = false;
	isBatch = false;
	extFile = NULL;
	OpenFile (Name, type);
}

ZeroFile::~ZeroFile ()
{
	CloseFile();
}

static bool IsBatched(const char *name);
static void ClearBatched(void);
static bool DumpBatch(void);
static bool OpenedBatch(void);
static void OpenBatch(bool doBatch, bool ok);

void ZeroFile::OpenFile (const char *name, const char *type)
{
	if (IsBatched(name))
	{
		ClearBatched();
		isBatch = true;
		if (!DumpBatch())
		{
			if (OpenedBatch())
				handle = 3;
			return;
		}
	}

#ifdef _DEBUG
	char path[256];
	path[0] = 0;
#endif
	char fullPathName[256];
	
	if (handle >= 0)
	{
		_close (handle);
	}

	// see if the file is in the external cache
	extFile = NULL;
	if (extCache)
	{
		extFile = extCache->Open((char *)name);
		if (extFile)
		{
			OpenBatch(isBatch, true);
			handle = 2;
			return;
		}
	}

	// generate the open flags
	int oflag = _O_SEQUENTIAL;
	for (const char *ptr = type; *ptr; ptr++)
	{
		switch (*ptr)
		{
		case 'a': oflag |= _O_WRONLY | _O_CREAT | _O_APPEND; break;
		case 'r': oflag |= _O_RDONLY; break;
		case 'w': oflag |= _O_WRONLY | _O_CREAT | _O_TRUNC; break;
		case '+': oflag &= ~(_O_WRONLY | _O_RDONLY); oflag |= _O_RDWR; break;
		case 'b': oflag |= _O_BINARY; break;
		case 't': oflag |= _O_TEXT; break;
		}
	}

	bool containPath = strchr(name, '\\') || strchr(name, '/');

	// if the file is opened for read only or name contains path information...
	if (((oflag & (_O_RDONLY | _O_WRONLY | _O_RDWR)) != _O_RDONLY) || containPath )
	{
#if (PLATFORM==PLATFORM_XBOX)
		const char *p = name;
		if ( (p[0] == 'D' || p[0] == 'd') && p[1]==':' )	// Skip leading D:
			p += 2;
		while ( *p == '.' || *p == '\\' || *p == '/' )		// Skip leading .\/ characters
			++p;
		sprintf(fullPathName, "D:\\%s", p );
		handle = _open (fullPathName, oflag, _S_IREAD | _S_IWRITE);		// attempt to open the file name
#else
		handle = _open (name, oflag, _S_IREAD | _S_IWRITE);		// attempt to open the file name
#endif
	}
	else
	{
		// find the name in the path tree
		PathTree::ZeroTreeNode *node = pathTree.Find(CalcLowerCRC(name));

		// if the name is found...
		if (node)
		{
			// generate the full path name
			sprintf(fullPathName, "%s\\%s", InvertCRC(node->info), name);

			// attempt to open the full path name
			handle = _open (fullPathName, oflag, _S_IREAD | _S_IWRITE);
		}
		else
		{
			ZeroTrace(("Searching for file \"%s\"...\n", name));
#if (PLATFORM==PLATFORM_XBOX)
			if (SearchFile(name, "D:", fullPathName))
#else
			if (SearchFile(name, ".", fullPathName))
#endif
			{
				// attempt to open the full path name
				handle = _open (fullPathName, oflag, _S_IREAD | _S_IWRITE);
			}
		}
	}

	// if the file is not open...
	if (handle < 0)
	{
		// print a warning and exit
		ZeroTrace (("Cannot open file \"%s\"\n", name));
		OpenBatch(isBatch, false);
		return;
	}
	
	if ((oflag & (_O_RDONLY | _O_WRONLY | _O_RDWR)) == _O_RDONLY)
	{
		_lseek(handle, 0, SEEK_END);
		long size = _tell(handle);
		_lseek(handle, 0, SEEK_SET);

		bufSize = size;
		if(bufSize)
		{
			bufBase = ZERONEW(char[bufSize],false);
			bufPtr = bufBase;
			_read(handle, bufBase, bufSize);
			if (doCRC)
			{
				unsigned crc = CalcCRC(bufBase, bufSize, 0);
				ZeroValue::Send(ZeroValue::BRK_OFF);
				try
				{
					ZeroValue::Record((int)crc, ZeroValue::EXTENDED);
				}
				catch (char *msg)
				{
					msg;
					// files differ on playback
					ZeroTrace(("File \"%s\" differs on playback (%s)\n", fullPathName + strlen(path), msg));
				}
				ZeroValue::Send(ZeroValue::BRK_ON);
			}
			if (bufBase[0] <= TOKEN_FLOAT)
				isBinary = true;

			extFile = new MemFile(bufBase, bufSize);
		}
	}

	OpenBatch(isBatch, true);
}

void ZeroFile::CloseFile() 
{
	if (handle >= 0) 
	{
		_close(handle);
		handle = -1;
	}
	if (extFile)
	{
		extFile->Release();
		extFile = NULL;
	}
	if (bufBase)
	{
		delete[] bufBase;
		bufBase = NULL;
		bufSize = 0;
		bufPtr = bufBase;
	}
}

void ZeroFile::SetCache(bool on)
{
	// not implemented yet
}

void ZeroFile::SetExtCache(ZeroExtCache *extCacheArg)
{
	extCache = extCacheArg;
}

void __cdecl ZeroFile::ClearPaths(void)
{
	pathTree.RemoveAll();
}

void __cdecl ZeroFile::AddPath(const char *path, bool recurse)
{
	char tmpPath[256];
	int handle;
	_finddata_t data;

	unsigned int crc(CalcStrCRC(path));

	sprintf(tmpPath, "%s\\*.*", path);

	handle = _findfirst (tmpPath, &data);
	if (handle != -1)
	{
		do
		{
			if (data.attrib & _A_SUBDIR)
			{
				if ((recurse) && (data.name[0] != '.'))
				{
					sprintf (tmpPath, "%s\\%s", path, data.name);
					AddPath(tmpPath, recurse);
				}
			}
			else
			{
				pathTree.Insert(crc, CalcLowerCRC(data.name));
			}
		}
		while (_findnext (handle, &data) == 0);

		_findclose (handle);
	}
}

bool __cdecl ZeroFile::SearchFile (const char *name,
								   const char *path,
								   char *fullPathName)
{
	char tmpPath[256];
	int handle;
	_finddata_t data;

	sprintf(tmpPath, "%s\\*.*", path);

	handle = _findfirst (tmpPath, &data);
	if (handle != -1)
	{
		do
		{
			if (data.attrib & _A_SUBDIR)
			{
				if (data.name[0] != '.')
				{
					sprintf (tmpPath, "%s\\%s", path, data.name);
					if (SearchFile(name, tmpPath, fullPathName))
						return true;
				}
			}
			else
			{
				if (strnicmp(name, data.name, sizeof(data.name)) == 0)
				{
					// found it!
					sprintf (fullPathName, "%s\\%s", path, data.name);
					return true;
				}
			}
		}
		while (_findnext (handle, &data) == 0);

		_findclose (handle);
	}

	return false;
}

bool __cdecl ZeroFile::FindFile (const char *name, 
								 const char *path,  
								 char *fullPathName)
{
	int handle;

	// if the name contains a path...
	if (strrchr(name, '\\') || strrchr(name, '/'))
	{
		// use the name as an explicit path
		strcpy(fullPathName, name);
	}
	else
	{
		// find the name in the path tree
		PathTree::ZeroTreeNode *node = pathTree.Find(CalcLowerCRC(name));

		// if the name is found...
		if (node)
		{
			// generate the full path
			sprintf(fullPathName, "%s\\%s", InvertCRC(node->info), name);
		}
		else
		{
			// explicitly search for the file
			ZeroTrace(("Searching for file \"%s\"...\n", name));
			if (!SearchFile(name, path, fullPathName))
			{
				return false;
			}
		}
	}

	// attempt to open the file
	handle = _open (fullPathName, _O_RDONLY);
	if (handle >= 0)
	{
		_close (handle);
		return true;
	}
	
	return false;
}

int __cdecl ZeroFile::FindCookie (const char *name)
{
	return 0;
}

int __cdecl ZeroFile::Print (const char *format, ...) const 
{
	int retVal;
	char buf[1024];
	va_list marker;
	
	va_start (marker, format);
	retVal = vsprintf (buf, format, marker);
	retVal = _write(handle, buf, retVal);
	va_end (marker);
	
	return retVal;
}

int ZeroFile::BinScan(const char *format, va_list arglist) const
{
	int count = 0;
	const char *fmt = format;
	if ((fmt[0] == '%') && (fmt[1] == 's') && (fmt[2] == ' ') && (fmt[3] == '='))
	{
		if (*bufPtr == TOKEN_EOLN)
			++bufPtr;
		if (*bufPtr != TOKEN_VAR)
		{
			char *pointer = va_arg(arglist, char *);
			*pointer = 0;
			return 0;
		}
		++bufPtr;
		bufPtr += sizeof(unsigned);
		unsigned char len = *bufPtr++;
		char *pointer = va_arg(arglist, char *);
		strncpy(pointer, bufPtr, len);
		pointer[len] = 0;
		bufPtr += len;
		++count;
		fmt += 2;
	}
	if ((fmt[0] == ' ') && (fmt[1] == '=') && (fmt[2] == ' '))
	{
		fmt = fmt + 3;
		char strFmt[] = "\"%[^\"]\"";
		char fltFmt[] = "%f";
		for (;;)
		{
			if (fmt[0] == 0)
				break;
			if (strncmp(fmt, strFmt, sizeof(strFmt) - 1) == 0)
			{
				fmt += sizeof(strFmt) - 1;
				if (*fmt == ' ')
					++fmt;
				if (*bufPtr != TOKEN_STRING)
					break;
				++bufPtr;
				unsigned char len = *bufPtr++;
				char *pointer = va_arg(arglist, char *);
				strncpy(pointer, bufPtr, len);
				pointer[len] = 0;
				bufPtr += len;
				++count;
			}
			else if (strncmp(fmt, fltFmt, sizeof(fltFmt) - 1) == 0)
			{
				fmt += sizeof(fltFmt) - 1;
				if (*fmt == ' ')
					++fmt;
				if (*bufPtr != TOKEN_FLOAT)
					break;
				++bufPtr;
				float *pointer = va_arg(arglist, float *);
				memcpy(pointer, bufPtr, sizeof(float));
				bufPtr += sizeof(float);
				++count;
			}
			else if ((fmt[0] == '\\') && (fmt[1] == 'n'))
			{
				if (*bufPtr == TOKEN_EOLN)
					++bufPtr;
				fmt += 2;
			}
			else if (fmt[0] == '\n')
			{
				if (*bufPtr == TOKEN_EOLN)
					++bufPtr;
				++fmt;
			}
		}
	}
	else
	{
		ZeroBreak();
	}
	return count;
}

int ZeroFile::BinFindToken(const char *token) const
{
	char str[64];
	if (token[0] == '[')
	{
		strcpy(str, token+1);
		str[strlen(str) - 1] = 0;
	}
	else
		strcpy(str, token);
	unsigned crc = CalcStrCRC(str);
	bool looped = false;
	char *savePtr = bufPtr;
	for (;;)
	{
		if (looped)
		{
			if (bufPtr == savePtr)
			{
				// we are back to where we started without finding the token
				return 0;
			}
		}
		switch (*bufPtr) {
		case TOKEN_EOF:
			if (!looped)
			{
				looped = true;
				bufPtr = bufBase;
				continue;
			}
			return 0;
		case TOKEN_EOLN:
			++bufPtr;
			break;
		case TOKEN_SECTION:
			{
				++bufPtr;
				unsigned sectionCrc;
				memcpy(&sectionCrc, bufPtr, sizeof(sectionCrc));
				bufPtr += sizeof(sectionCrc);
				if (sectionCrc == crc)
					return 1;
			}
			break;
		case TOKEN_VAR:
			{
				++bufPtr;
				unsigned varCrc;
				memcpy(&varCrc, bufPtr, sizeof(varCrc));
				bufPtr += sizeof(varCrc);
				unsigned char len = *bufPtr++;
				bufPtr += len;
				if (varCrc == crc)
					return 1;
			}
			break;
		case TOKEN_STRING:
			{
				++bufPtr;
				unsigned char len = *bufPtr++;
				bufPtr += len;
			}
			break;
		case TOKEN_FLOAT:
			{
				++bufPtr;
				bufPtr += sizeof(float);
			}
			break;
		default:
			ZeroBreak();
			break;
		}
	}
//	return 0;
}

char *ZeroFile::BinGets(char *string, int n) const
{
	if (*bufPtr != TOKEN_VAR)
		return NULL;
	++bufPtr;
	bufPtr += sizeof(unsigned);
	unsigned char len = *bufPtr++;
	char *p = string;
	memcpy(p, bufPtr, len);
	bufPtr += len;
	p += len;
	*p++ = ' ';
	*p++ = '=';
	bool done = false;
	while (!done)
	{
		switch (*bufPtr) {
		case TOKEN_EOLN:
			bufPtr++;
			done = true;
			break;
		case TOKEN_STRING:
			bufPtr++;
			*p++ = ' ';
			len = *bufPtr++;
			*p++ = '"';
			memcpy(p, bufPtr, len);
			p += len;
			*p++ = '"';
			bufPtr += len;
			break;
		case TOKEN_FLOAT:
			{
				bufPtr++;
				float fval;
				memcpy(&fval, bufPtr, sizeof(float));
				sprintf(p, " %f", fval);
				p += strlen(p);
			}
			break;
		default:
			ZeroBreak();
			break;
		}
	}
	strcpy(p, "\n");
	return string;
}

int ZeroFile::Scan(const char *format, va_list arglist) const
{
	int retVal;
	if (isBinary)
		retVal = BinScan(format, arglist);
	else
		retVal = Vscanf(format, arglist);
	return retVal;
}

int __cdecl ZeroFile::Scanf (const char *format, ...) const
{
	int retVal;
	va_list marker;
	
	va_start (marker, format);
	retVal = Scan(format, marker);
	va_end (marker);
	
	return retVal;
}

int ZeroFile::SeekSet (int offset) const
{
	if (extFile)
	{
		int current = extFile->Tell();
		return extFile->Seek(offset - current);
	}
	else
	{
		return _lseek(handle, offset, SEEK_SET);
	}
}

int ZeroFile::SeekCur (int offset) const
{
	if (extFile)
	{
		return extFile->Seek(offset);
	}
	else
	{
		return _lseek(handle, offset, SEEK_CUR);
	}
}

int ZeroFile::SeekEnd (int offset) const
{
	if (extFile)
	{
		int dest = extFile->GetSize() - offset;
		int current = extFile->Tell();
		return extFile->Seek(dest - current);
	}
	else
	{
		return _lseek(handle, offset, SEEK_END);
	}
}

int ZeroFile::Tell () const
{
	if (extFile)
	{
		return extFile->Tell();
	}
	else
	{
		return (int)_tell(handle);
	}
}

int ZeroFile::Size () const
{
	if (extFile)
	{
		return extFile->GetSize();
	}
	else
	{
		long pos = _tell(handle);
		_lseek(handle, 0, SEEK_END);
		long size = _tell(handle);
		_lseek(handle, pos, SEEK_SET);
		return (int)size;
	}
}

int ZeroFile::Getc() const
{
	if (extFile)
	{
		char c;
		if (extFile->Tell() < extFile->GetSize())
			extFile->Read(&c, 1);
		else
			c = EOF;
		return c;
	}
	else
	{
		unsigned char c;
		if (_read(handle, &c, 1))
			return c;
		else
			return EOF;
	}
}

int ZeroFile::Ungetc (int ch) const
{
	if (extFile)
	{
		char c;
		if (extFile->Tell() > 0)
		{
			// eek. this looks pretty in efficient
			// do i have to make the external file more complicated or make ZeroFile
			// less complicated?
			extFile->Seek(-1);
			extFile->Read(&c, 1);
			extFile->Seek(-1);
		}
		else
			c = EOF;
		return c;
	}
	else
	{
		if (_lseek(handle, -1, SEEK_CUR) >= 0)
			return ch;
		else
			return EOF;
	}
}

int ZeroFile::Putc (int ch) const
{
	_ASSERTE(!bufBase);
	unsigned char c = (unsigned char)ch;
	if (_write(handle, &c, 1))
		return c;
	else
		return EOF;
}

int ZeroFile::Eof() const
{
	if (extFile)
	{
//		return (bufPtr < bufBase) || (bufPtr >= bufBase + bufSize);
		return extFile->Tell() >= extFile->GetSize();
	}
	else
	{
		return _eof (handle);
	}
}

char *ZeroFile::Gets (char *string, int n) const
{
	if (isBinary)
		return BinGets(string, n);
	char *retVal = string;
	for (int i = 0; i < n - 1; i++)
	{
		int ch = Getc();
		if (ch == EOF)
		{
			retVal = NULL;
			break;
		}
		string[i] = (char)ch;
		if (ch == '\n')
		{
			break;
		}
	}
	string[i] = '\0';
	return retVal;
}

int ZeroFile::Read (void *const buffer, 
					int size, 
					int count) const
{
	if (extFile)
	{
		int num = size * count;
		num = extFile->Read((char *)buffer, num);
		return num / size;
	}
	else
	{
		return _read (handle, buffer, size * count) / size;
	}
}

int ZeroFile::Read8 (void *const buffer, 
					int size, 
					int count) const
{
	return Read(buffer, size, count);
}

int ZeroFile::Read16 (void *const buffer, 
					int size, 
					int count) const
{
	return Read(buffer, size, count);
}

int ZeroFile::Read32 (void *const buffer, 
					int size, 
					int count) const
{
	return Read(buffer, size, count);
}

int ZeroFile::Write (const void *buffer, 
					 int size, 
					 int count) const
{
	_ASSERTE(!bufBase);
	return _write (handle, buffer, size * count) / size;
}

int ZeroFile::Flush() const
{
	_ASSERTE(!bufBase);
	return _commit (handle);
}

void ZeroFile::Rewind() const
{
	if (extFile)
	{
		int current = extFile->Tell();
		extFile->Seek(0 - current);
	}
	else
	{
		_lseek (handle, 0, SEEK_SET);
	}
}

// ZeroFile::GetDirListing -- get a list of ZeroFileDirEntries for
// the passed in path.  This is, of course, the Win32 implementation.
// GC and PSX2 versions are in ZeroFile_GC.cpp and ZeroFile_PS2.cpp
// respectively.  No versions return "." paths in the list, although
// some return ".." entries, useful in determining if you're at the root
void ZeroFile::GetDirListing(const char * path, ZeroVisList<ZeroFileDirEntry> & list)
{
	if (path == NULL || ::strlen(path) <= 0)
		return;
	list.RemoveAll();

	char tmpPath[256];
	int handle;
	_finddata_t data;
	ZeroFileDirEntry workEntry;

	sprintf(tmpPath, "%s\\*.*", path);

	handle = _findfirst (tmpPath, &data);
	if (handle != -1)
	{
		do
		{
			if (data.attrib & _A_SUBDIR)
			{
				if (::strcmp(data.name, ".") != 0)
				{
					workEntry.bIsDir = true;
					::strncpy(workEntry.szName, data.name, 256);
					list.Append(workEntry); // should copy, we hope
				}
			}
			else
			{
				workEntry.bIsDir = false;
				::strncpy(workEntry.szName, data.name, 256);
				list.Append(workEntry); // should copy, we hope
			}
		}
		while (_findnext (handle, &data) == 0);

		_findclose (handle);
	}
}

int ZeroFile::FindToken (const char *Token) const
{
	if (isBinary)
	{
		return BinFindToken(Token);
	}
	//

	int filePosition = Tell();

	int c = Getc();

	while (c != EOF)
	{
		const char *tokenPtr = Token;
		while ((c != EOF) && (tolower(c) == tolower(*tokenPtr)))
		{
			c = Getc();
			tokenPtr++;
			if (*tokenPtr == '\0')
			{
				if (isalnum(c))
				{
					break;
				}
				else
				{
					Ungetc(c);
					return 1;
				}
			}
		}
		while ((c != EOF) && (c != '\n' && c != '\r'))
		{
			c = Getc();
		}
		while ((c != EOF) && (c == '\n' || c == '\r'))
		{
			c = Getc();
		}
	}
		
	if (Tell() >= filePosition)
	{
		SeekSet(0);
		c = Getc();
		while (Tell() < filePosition)
		{
			const char *tokenPtr = Token;
			while ((c != EOF) && (tolower(c) == tolower(*tokenPtr)))
			{
				c = Getc();
				tokenPtr++;
				if (*tokenPtr == '\0')
				{
					if (isalnum(c))
					{
						break;
					}
					else
					{
						Ungetc(c);
						return 1;
					}
				}
			}
			while ((c != EOF) && (c != '\n' && c != '\r'))
			{
				c = Getc();
			}
			while ((c != EOF) && (c == '\n' || c == '\r'))
			{
				c = Getc();
			}
		}
	}
	
	return 0;
}

int __cdecl  ZeroFile::ReadToken (const char *Token, 
								  const char *format, ...) const
{
	int retVal = 0;
	
	if (FindToken(Token)) {
		va_list marker;
		
		va_start (marker, format);
		retVal = Scan(format, marker);
		va_end (marker);
	}
	
	return retVal;
}

static bool LoadReadSectionToken(bool doBatch, int &retVal, const char *token, const char *format, va_list marker);
static void StoreReadSectionToken(bool doBatch, int retVal, const char *token, const char *format, va_list marker);

int __cdecl ZeroFile::ReadSectionToken(const char *section, 
									   const char *token, 
									   const char *format, ...) const
{
	va_list marker;
	va_start (marker, format);

	int retVal = 0;
	
	if (!LoadReadSectionToken(isBatch, retVal, token, format, marker))
	{
		Rewind();
		if (FindToken(section)) {
			if (FindToken(token)) {			
				retVal = Scan(format, marker);
				va_end (marker);
			}
		}

		va_start (marker, format);
		StoreReadSectionToken(isBatch, retVal, token, format, marker);
	}

	va_end (marker);
	return retVal;
}

static const int CHECK_DEPENDENCY_TIME = -10;

bool __cdecl ZeroFile::CheckDependency(const char *name, 
									   const char *dependency)
{
	HANDLE handle;
	HANDLE dependencyHandle;
	FILETIME dependencyCreationTime;
	FILETIME nameCreationTime;
	
	handle = CreateFile(name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle != INVALID_HANDLE_VALUE)	{
		GetFileTime(handle, NULL, NULL, &nameCreationTime); 
		CloseHandle(handle);
		
		dependencyHandle = CreateFile(dependency, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (dependencyHandle != INVALID_HANDLE_VALUE)	{
			GetFileTime(dependencyHandle, NULL, NULL, &dependencyCreationTime); 
			CloseHandle(dependencyHandle);
			if (CompareFileTime(&dependencyCreationTime, &nameCreationTime) > CHECK_DEPENDENCY_TIME)	{
				return true;
			}
		}
	}
	
	return false;
}


static const int CVTBUFSIZE = (309+40); /* # of digits in max. dp value + slop */

static const char LEFT_BRACKET = '[' | ('a' - 'A'); /* 'lowercase' version */

static const int ASCII = 32;          /* # of bytes needed to hold 256 bits */

static char sbrackset[] = " \t-\r]";  /* use range-style list */
static char cbrackset[] = "]";


// convert a hex digit to an integer
static int HexToDec (int chr)
{
    return isdigit(chr) ? chr : (chr & ~('a' - 'A')) - 'A' + 10 + '0';
}


/***
*int Vcanf(format, arglist), static int input(format, arglist)
*
*Purpose:
*   get input items (data items or literal matches) from the input stream
*   and assign them if appropriate to the items thru the arglist. this
*   function is intended for internal library use only, not for the user
*
*   The vfscanf entry point is for the normal scanf() functions
*   The input entry point is used when compiling for _cscanf() [CPRFLAF
*   defined] and is a static function called only by _cscanf() -- reads from
*   console.
*
*Entry:
*   char *format - format string to determine the data to read
*   arglist - list of pointer to data items
*
*Exit:
*   returns number of items assigned and fills in data items
*   returns EOF if error or EOF found on stream before 1st data item matched
*
*Exceptions:
*
*******************************************************************************/

int ZeroFile::Vscanf (const char *format, va_list arglist) const
{
    char table[ASCII];                  /* which chars allowed for %[], %s   */
    char floatstring[CVTBUFSIZE + 1];   /* ASCII buffer for floats           */
	
    unsigned int number;                /* temp hold-value                   */
    void *pointer;                      /* points to user data receptacle    */
    void *start;                        /* indicate non-empty string         */
	
	
    cw_wchar_t wctemp;
    char *scanptr;                      /* for building "table" data         */
	register int ch;
	register int comchr;                /* holds designator type             */
    int count;                          /* return value.  # of assignments   */
	
    int started;                        /* indicate good number              */
    int width;                          /* width of field                    */
    int widthset;                       /* user has specified width          */
	
	/* Neither coerceshort nor farone are need for the 386 */
	
	
    char done_flag;                     /* general purpose loop monitor      */
    char longone;                       /* 0 = SHORT, 1 = LONG, 2 = L_DOUBLE */
    signed char widechar;               /* -1 = char, 0 = ????, 1 = cw_wchar_t  */
    char reject;                        /* %[^ABC] instead of %[ABC]         */
    char negative;                      /* flag for '-' detected             */
    char suppress;                      /* don't assign anything             */
    char match;                         /* flag: !0 if any fields matched    */
    va_list arglistsave;                /* save arglist value                */
	
    char fl_wchar_arg;                  /* flags wide char/string argument   */
    unsigned char rngch;                /* used while scanning range         */
    unsigned char last;                 /* also for %[a-z]                   */
    unsigned char prevchar;             /* for %[a-z]                        */
	
    _ASSERTE(handle >= 0);
    _ASSERTE(format != NULL);
	
    /*
    count = # fields assigned
    match = flag indicating if any fields were matched
	
	  [Note that we need both count and match.  For example, a field
	  may match a format but have assignments suppressed.  In this case,
	  match will get set, but 'count' will still equal 0.  We need to
	  distinguish 'match vs no-match' when terminating due to EOF.]
    */
	
    count = 0;
	match = 0;
	ch = 0;

    while (*format)
	{
        if (isspace(*format))
		{
            UnInc(SkipWhite()); /* put first non-space char back */
			
            while ((isspace)(*++format)); /* NULL */
            /* careful: isspace macro may evaluate argument more than once! */
        }
		
        if ('%' == *format)
		{
            number = 0;
            prevchar = 0;
            width = 0;
			widthset = 0;
			started = 0;
            fl_wchar_arg = 0;
			done_flag = 0;
			suppress = 0;
			negative = 0;
			reject = 0;
            widechar = 0;
			
            arglistsave = arglist;
			pointer = NULL;
			
            longone = 1;
			
            while (!done_flag)
			{
                comchr = *++format;
                if (isdigit(comchr))
				{
                    ++widthset;
                    width *= 10;
					width += (comchr - '0');
                }
				else
				{
                    switch (comchr)
					{
					case 'F' :
					case 'N' :   /* no way to push NEAR in large model */
						break;  /* NEAR is default in small model */
					case 'h' :
						/* set longone to 0 */
						--longone;
						--widechar;         /* set widechar = -1 */
						break;
						
					case 'L' :
						/*  ++longone;  */
						++longone;
						break;
						
					case 'l' :
						++longone;
						/* NOBREAK */
					case 'w' :
						++widechar;         /* set widechar = 1 */
						break;
						
					case '*' :
						++suppress;
						break;
						
					default:
						++done_flag;
						break;
					}
				}
            }
			
            if (!suppress)
			{
                pointer = va_arg(arglist,void *);
            }
			
            done_flag = 0;
			
            if (!widechar)
			{
				/* use case if not explicitly specified */
                if ((*format == 'S') || (*format == 'C'))
				{
                    ++widechar;
				}
                else
				{
                    --widechar;
				}
            }
			
            /* switch to lowercase to allow %E,%G, and to
			keep the switch table small */
			
            comchr = *format | ('a' - 'A');
			
            if ('c' != comchr && LEFT_BRACKET != comchr)
			{
                ch = SkipWhite();
			}
            else
			{
                ch = Inc();
			}
			
			if (!widthset || width)
			{
				switch(comchr)
				{
				case 'c':
					/*  case 'C':  */
					if (!widthset)
					{
						++widthset;
						++width;
					}
					if (widechar > 0)
					{
						fl_wchar_arg++;
					}
					scanptr = cbrackset;
					--reject; /* set reject to 255 */
					goto scanit2;
					
				case 's':
					/*  case 'S':  */
					if (widechar>0)
					{
						fl_wchar_arg++;
					}
					scanptr = sbrackset;
					--reject; /* set reject to 255 */
					goto scanit2;
					
				case LEFT_BRACKET :   /* scanset */
					if (widechar>0)
					{
						fl_wchar_arg++;
					}
					scanptr = (char *)++format;
					
					if ('^' == *scanptr)
					{
						++scanptr;
						--reject; /* set reject to 255 */
					}
					
scanit2:
					memset(table, 0, ASCII);
					
					if (LEFT_BRACKET == comchr)
					{
						if (']' == *scanptr)
						{
							prevchar = ']';
							++scanptr;
							
							table[ ']' >> 3] = 1 << (']' & 7);
						}
					}
					
					while (']' != *scanptr)
					{
						rngch = *scanptr++;
						
						if ('-' != rngch ||
							!prevchar ||           /* first char */
							']' == *scanptr) /* last char */
						{
							table[(prevchar = rngch) >> 3] |= 1 << (rngch & 7);
						}
						else
						{
							/* handle a-z type set */
							rngch = *scanptr++; /* get end of range */
							
							if (prevchar < rngch)  /* %[a-z] */
							{
								last = rngch;
							}
							else              /* %[z-a] */
							{
								last = prevchar;
								prevchar = rngch;
							}
							
							for (rngch = prevchar; rngch <= last; ++rngch)
							{
								table[rngch >> 3] |= 1 << (rngch & 7);
							}
							
							prevchar = 0;
						}
					}
					
					if (!*scanptr)
					{
						goto error_return;      /* trunc'd format string */
					}
					
					/* scanset completed.  Now read string */
					if (LEFT_BRACKET == comchr)
					{
						format = scanptr;
					}
					
					start = pointer;
					
					/*
					* execute the format directive. that is, scan input
					* characters until the directive is fulfilled, eof
					* is reached, or a non-matching character is
					* encountered.
					*
					* it is important not to get the next character
					* unless that character needs to be tested! other-
					* wise, reads from line-buffered devices (e.g.,
					* scanf()) would require an extra, spurious, newline
					* if the first newline completes the current format
					* directive.
					*/
					UnInc(ch);
					
					while ( !widthset || width-- )
					{
						ch = Inc();
						if (
							(EOF != ch) &&
							((table[ch >> 3] ^ reject) & (1 << (ch & 7)))
							)
						{
							if (!suppress)
							{
								if (fl_wchar_arg)
								{
									char temp[2];
									temp[0] = (char) ch;
									if (isleadbyte(ch))
										temp[1] = (char) Inc();
									mbtowc(&wctemp, temp, MB_CUR_MAX);
									*(cw_wchar_t UNALIGNED *)pointer =
										wctemp;
									/* do nothing if mbtowc fails */
									pointer = (cw_wchar_t *)pointer + 1;
								}
								else
								{
									*(char *)pointer = (char)ch;
									pointer = (char *)pointer + 1;
								}
							} /* suppress */
							else
							{
								/* just indicate a match */
								start = (char *)start + 1;
							}
						}
						else
						{
							UnInc(ch);
							break;
						}
					}
					
					/* make sure something has been matched and, if
					assignment is not suppressed, null-terminate
					output string if comchr != c */
					
					if (start != pointer)
					{
						if (!suppress)
						{
							++count;
							if ('c' != comchr)
							{
								/* null-terminate strings */
								if (fl_wchar_arg)
								{
									*(cw_wchar_t UNALIGNED *)pointer = L'\0';
								}
								else
								{
									*(char *)pointer = '\0';
								}
							}
						} else /*NULL*/;
					}
					else
					{
						goto error_return;
					}
					
					break;
					
				case 'i' :      /* could be d, o, or x */
					
					comchr = 'd'; /* use as default */
					
				case 'x':
					
					if ('-' == ch)
					{
						++negative;
						
						goto x_incwidth;
						
					}
					else if ('+' == ch)
					{
x_incwidth:
						if (!--width && widthset)
						{
							++done_flag;
						}
						else
						{
							ch = Inc();
						}
					}
					
					if ('0' == ch)
					{
						if ('x' == (char)(ch = Inc()) || 'X' == (char)ch)
						{
							ch = Inc();
							comchr = 'x';
						}
						else
						{
							++started;
							if ('x' != comchr)
							{
								comchr = 'o';
							}
							else
							{
								/* scanning a hex number that starts */
								/* with a 0. push back the character */
								/* currently in ch and restore the 0 */
								UnInc(ch);
								ch = '0';
							}
						}
					}
					goto getnum;
					
					/* NOTREACHED */
					
				case 'p' :
					/* force %hp to be treated as %p */
					longone = 1;
					
				case 'o' :
				case 'u' :
				case 'd' :
					
					if ('-' == ch)
					{
						++negative;
						
						goto d_incwidth;
					}
					else if ('+' == ch)
					{
d_incwidth:
						if (!--width && widthset)
						{
							++done_flag;
						}
						else
						{
							ch = Inc();
						}
					}
					
getnum:
					while (!done_flag)
					{
						if ('x' == comchr || 'p' == comchr)
						{
							if (isxdigit(ch))
							{
								number = (number << 4);
								ch = HexToDec(ch);
							}
							else
							{
								++done_flag;
							}
						}
						else if (isdigit(ch))
						{
							if ('o' == comchr)
							{
								if ('8' > ch)
								{
									number = (number << 3);
								}
								else
								{
									++done_flag;
								}
							}
							else /* 'd' == comchr */
							{
								number *= 10;
							}
						}		
						else
						{
							++done_flag;
						}
									
						if (!done_flag)
						{
							++started;
							number += ch - '0';
							
							if (widthset && !--width)
							{
								++done_flag;
							}
							else
							{
								ch = Inc();
							}
						}
						else
						{
							UnInc(ch);
						}
					} /* end of WHILE loop */
					
					if (negative)
					{
						number = (unsigned int)(-(int)number);
					}
					if ('F'==comchr)
					{
						/* expected ':' in long pointer */
						started = 0;
					}
					
					if (started)
					{
						if (!suppress)
						{
							++count;
							if (longone)
							{
								*(int UNALIGNED *)pointer = (unsigned int)number;
							}
							else
							{
								*(short UNALIGNED *)pointer = (unsigned short)number;
							}
						}
					}
					else
					{
						goto error_return;
					}
						
					break;
						
				case 'e' :
					/* case 'E' : */
				case 'f' :
				case 'g' : /* scan a float */
					/* case 'G' : */
					
					scanptr = floatstring;
					
					if ('-' == ch)
					{
						*scanptr++ = '-';
						goto f_incwidth;
						
					}
					else if ('+' == ch)
					{
f_incwidth:
						--width;
						ch = Inc();
					}
					
					if (!widthset || width > CVTBUFSIZE)              /* must watch width */
					{
						width = CVTBUFSIZE;
					}
					
					
					/* now get integral part */
					while (isdigit(ch) && width--)
					{
						++started;
						*scanptr++ = (char)ch;
						ch = Inc();
					}
					
					/* now check for decimal */
					if ('.' == (char)ch && width--)
					{
						ch = Inc();
						*scanptr++ = '.';
						
						while (isdigit(ch) && width--)
						{
							++started;
							*scanptr++ = (char)ch;
							ch = Inc();
						}
					}
					
					/* now check for exponent */
					if (started && ('e' == ch || 'E' == ch) && width--)
					{
						*scanptr++ = 'e';
						
						if ('-' == (ch = Inc()))
						{
							*scanptr++ = '-';
							goto f_incwidth2;
						}
						else if ('+' == ch)
						{
f_incwidth2:
							if (!width--)
							{
								++width;
							}
							else
							{
								ch = Inc();
							}
						}
						
						
						while (isdigit(ch) && width--)
						{
							++started;
							*scanptr++ = (char)ch;
							ch = Inc();
						}
					}
					
					UnInc(ch);
					
					if (started)
					{
						if (!suppress)
						{
							++count;
							*scanptr = '\0';
#if 0
							_fassign( longone-1, pointer , floatstring);
#else
							if (longone-1)
							{
								*(double UNALIGNED *)pointer = atof(floatstring);
							}
							else
							{
								*(float UNALIGNED *)pointer = (float)atof(floatstring);
							}
#endif
						}
						else
						{
							goto error_return;
						}
					}

					break;
						
				default:    /* either found '%' or something else */
					
					if ((int)*format != (int)ch)
					{
						UnInc(ch);
						goto error_return;
					}
					else
					{
						match--; /* % found, compensate for inc below */
					}
					
					if (!suppress)
					{
						arglist = arglistsave;
					}
                } /* SWITCH */
				
                match++;        /* matched a format field - set flag */
            } /* WHILE (width) */
            else
			{
				/* zero-width field in format string */
                UnInc(ch);  /* check for input error */
                goto error_return;
            }
			
            ++format;  /* skip to next char */
        }
		else  /*  ('%' != *format) */
        {
            if ((int)*format++ != (int)(ch = Inc()))
			{
                UnInc(ch);
                goto error_return;
			}
            if (isleadbyte(ch))
			{
                int ch2;
                if ((int)*format++ != (ch2=Inc()))
				{
                    UnInc(ch2);
                    UnInc(ch);
                    goto error_return;
				}
			}
		}
		
        if (EOF == ch)
		{
            break;
		}
    }  /* WHILE (*format) */
	
error_return:
	
    if (EOF == ch)
	{
        /* If any fields were matched or assigned, return count */
        return ( (count || match) ? count : EOF);
	}
    else
	{
        return count;
	}
}

// get the next character
int ZeroFile::Inc () const
{
	return Getc();
}

// un-get the next character
int ZeroFile::UnInc (int ch) const
{
	if (EOF == ch)
		return EOF;
	else
		return Ungetc(ch);
}

// return the first non-blank character
int ZeroFile::SkipWhite () const
{
    register int ch;
    while (isspace(ch = Getc()));
    return ch;
}

#define DO_BATCH 1
#define DO_DEBUG 0

static bool inBatch;
static bool batchOdf;
static bool dumpBatch;

#if DO_BATCH
static ZeroStreamFile *batchFile;

template <class T>
inline void DoStreamF(T &data)
{
	batchFile->Stream((char *)&data, sizeof(data));
}
#else
inline void DoStreamF(bool &) {}
#endif

void ZeroFile::BeginBatch(const char *name)
{
	inBatch = true;
	batchOdf = false;
	dumpBatch = false;
#if DO_BATCH
	batchFile = NULL;
	char batchName[64];
	strcpy(batchName, name);
	char *dot = strrchr(batchName, '.');
	if (dot)
		*dot = 0;
	strcat(batchName, "_odf.bin");
#if 1
	batchFile = ZeroStreamFile::Open(batchName);
#else
	batchFile = NULL;
#endif
	if (batchFile)
	{
	}
	else
	{
		batchFile = ZeroStreamFile::Open(batchName, false);
		if (batchFile)
		{
			dumpBatch = true;
		}
	}
#endif
}

void ZeroFile::EndBatch(void)
{
	inBatch = false;
#if DO_BATCH
	if (batchFile)
	{
		if (dumpBatch)
		{
			for (int i = 0; i < 1024*64; i++)
			{
				DoStreamF(i);
			}
		}
		delete batchFile;
	}
#endif
}

void ZeroFile::BatchOdf(void)
{
#if DO_BATCH
	batchOdf = true;
#endif
}

static bool IsBatched(const char *name)
{
	bool isBatched = inBatch && batchOdf;
	if (isBatched)
	{
		//ZeroTrace(("Batched: %s\n", name));
	}
	return isBatched;
}

static void ClearBatched(void)
{
	batchOdf = false;
}

static bool DumpBatch(void)
{
	return dumpBatch;
}

static bool OpenedBatch(void)
{
	bool ok = true;
	DoStreamF(ok);
	return ok;
}

static void OpenBatch(bool doBatch, bool ok)
{
	if (!inBatch || !doBatch)
		return;
	DoStreamF(ok);
}

inline bool IsString(const char *p)
{
	if (p[1] == '[')
		return true;
	return false;
}

inline bool IsFloat(const char *p)
{
	if (p[1] == 'f')
		return true;
	return false;
}

inline bool IsInt(const char *p)
{
	if (p[1] == 'd')
		return true;
	if (p[1] == 'i')
		return true;
	return false;
}

inline bool IsByte(const char *p)
{
	return false;
}

inline bool IsWord(const char *p)
{
	return false;
}

inline bool IsLong(const char *p)
{
	if (p[1] == 'f')
		return true;
	if (p[1] == 'd')
		return true;
	return false;
}

inline bool IsDouble(const char *p)
{
	return false;
}

static bool LoadReadSectionToken(bool doBatch, int &retVal, const char *token, const char *format, va_list marker)
{
	if (!doBatch)
		return false;
#if DO_BATCH
	static int count = 0;
	++count;
	//ZeroTrace(("Count %d\n", count));
	static int debugCount = -1;
	if (count == debugCount)
		ZeroBreak();
	if (dumpBatch)
		return false;
	//ZeroTrace(("token: %s\n", token));
	// load the return value
	{
		int v;
		DoStreamF(v);
		retVal = v;
	}
#if DO_DEBUG
	unsigned int crc;
	DoStreamF(crc);
	if (crc != CalcLowerCRC(token))
		ZeroBreak();
#endif
	// load the token values
	const char *p = format;
	//ZeroTrace(("format: |%s|\n", format));
	for (int i = 0; i < retVal; i++)
	{
		p = strchr(p, '%');
		if (IsString(p))
		{
			char *v = va_arg(marker, char *);
			unsigned char cLen;
			DoStreamF(cLen);
			batchFile->Stream(v, cLen);
			v[cLen] = 0;
			//ZeroTrace(("String: |%s|\n", v));
		}
		else if (IsFloat(p))
		{
			float &v = *va_arg(marker, float *);
			DoStreamF(v);
			//ZeroTrace(("Float: %f\n", v));
		}
		else if (IsInt(p))
		{
			int &v = *va_arg(marker, int *);
			DoStreamF(v);
			//ZeroTrace(("Int: %d\n", v));
		}
		else
		{
			ZeroBreak();
		}
		++p;
	}
#endif
	return true;
}

static void StoreReadSectionToken(bool doBatch, int retVal, const char *token, const char *format, va_list marker)
{
	if (!doBatch)
		return;
#if DO_BATCH
	DoStreamF(retVal);
#if DO_DEBUG
	unsigned int crc = CalcLowerCRC(token);
	DoStreamF(crc);
#endif
	const char *p = format;
	//ZeroTrace(("format: |%s|\n", format));
	while (retVal > 0)
	{
		--retVal;
		p = strchr(p, '%');
		if (!p)
			break;
		if (IsString(p))
		{
			char *v = va_arg(marker, char *);
			//ZeroTrace(("String: |%s|\n", v));
			int len = strlen(v);
			if (len > 255)
				ZeroBreak();
			unsigned char cLen = (unsigned char)len;
			DoStreamF(cLen);
			batchFile->Stream(v, len);
		}
		else if (IsFloat(p))
		{
			float &v = *va_arg(marker, float *);
			//ZeroTrace(("Float: %f\n", v));
			DoStreamF(v);
		}
		else if (IsInt(p))
		{
			int &v = *va_arg(marker, int *);
			//ZeroTrace(("Int: %d\n", v));
			DoStreamF(v);
		}
		else
		{
			ZeroBreak();
		}
		++p;
	}
#endif
}

#include "zlib.h"
#define BUFFER_SIZE (1024*32)

inline char *MakeOffset(int *base, int offset)
{
	return (char *)base + offset;
}

ZeroStreamFile *ZeroStreamFile::Open(const char *name, bool read)
{
	ZeroStreamFile *f = new ZeroStreamFile(name, read);
	if (f->file == -1)
	{
		delete f;
		return NULL;
	}
	return f;
}

ZeroStreamFile::ZeroStreamFile(const char *name, bool read)
{
	reading = read;

	blockBuffer = new char[BUFFER_SIZE * 2 + 511];
	block[0] = (char *)(((unsigned int)blockBuffer + 511) & ~511);
	block[1] = block[0] + BUFFER_SIZE;

	memset(mdFields, 0, sizeof(mdFields));

	OVERLAPPED *overlapped = (OVERLAPPED *)MakeOffset(mdFields, 0);
	z_stream &stream = *(z_stream *)MakeOffset(mdFields, sizeof(OVERLAPPED) * 2);

	file = -1;
	HANDLE fh;

	if (reading)
	{

		fh = CreateFile(name, GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING,
			NULL);

		if (fh == INVALID_HANDLE_VALUE)
			return;

		DWORD fileSize = GetFileSize(fh, NULL);
		if (fileSize == 0)
		{
			CloseHandle(fh);
			return;
		}

		file = (int)fh;

		headBlock = tailBlock = 0;

		for (int i = 0; i < 2; i++)
		{
			memset(&overlapped[i], 0, sizeof(OVERLAPPED));
			overlapped[i].hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		}

		NextBlock();
		NextBlock();

		WaitBlock(tailBlock);

		stream.avail_in = BUFFER_SIZE;
		stream.next_in = (Bytef*)block[tailBlock & 1];

	 	int err = inflateInit(&stream);
		err;
	}
	else
	{
		fh = CreateFile(name, GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
			NULL);

		if (fh == INVALID_HANDLE_VALUE)
			return;

		stream.next_out = (Bytef*)block[0];
		stream.avail_out = BUFFER_SIZE;

		int err = deflateInit(&stream, Z_BEST_COMPRESSION);
		err;
	}

	file = (int)fh;
}

ZeroStreamFile::~ZeroStreamFile()
{
	HANDLE fh = (HANDLE)file;

	OVERLAPPED *overlapped = (OVERLAPPED *)MakeOffset(mdFields, 0);
	z_stream &stream = *(z_stream *)MakeOffset(mdFields, sizeof(OVERLAPPED) * 2);

	if (reading)
	{
		overlapped[0].Offset;
	}
	else
	{
		// flush the last bits of data
		while (true)
		{
			// if no room in buffer, send it
			if (stream.avail_out == 0)
			{
				DWORD bytes;
				WriteFile(fh, block[0], BUFFER_SIZE, &bytes, NULL);
				stream.next_out = (Bytef*)block[0];
				stream.avail_out = BUFFER_SIZE;
			}
			// compress as much as possible
			int err = deflate(&stream, Z_FINISH);
			if (err == Z_STREAM_END)
				break;
		}
		if (stream.next_out > (Bytef*)block[0])
		{
			DWORD bytes;
			WriteFile(fh, block[0], BUFFER_SIZE, &bytes, NULL);
		}
	}
	CloseHandle(fh);
	delete []blockBuffer;
}

int ZeroStreamFile::Stream(char *buffer, int size)
{
	HANDLE fh = (HANDLE)file;

	OVERLAPPED *overlapped = (OVERLAPPED *)MakeOffset(mdFields, 0);
	z_stream &stream = *(z_stream *)MakeOffset(mdFields, sizeof(OVERLAPPED) * 2);

	overlapped[0].Offset;

	int count = 0;
	if (reading)
	{
		stream.next_out = (Bytef*)buffer;
		stream.avail_out = size;
		while (stream.avail_out > 0)
		{
			if (stream.avail_in == 0)
			{
				// switch to the next block
				NextBlock();
				// wait for previous read to finish
				++tailBlock;
				WaitBlock(tailBlock);
				// set up decompression
				stream.avail_in = BUFFER_SIZE;
				stream.next_in = (Bytef*)block[tailBlock & 1];
			}
			int err = inflate(&stream, Z_FINISH);
			if ((err != Z_OK) && (err != Z_BUF_ERROR))
				ZeroTrace(("inflate: %d\n", err));
		}
		count = size - stream.avail_out;
	}
	else
	{
		stream.next_in = (Bytef*)buffer;
		stream.avail_in = (uInt)size;
		while (stream.avail_in > 0)
		{
			// if no room in buffer, send it
			if (stream.avail_out == 0)
			{
				DWORD bytes;
				WriteFile(fh, block[0], BUFFER_SIZE, &bytes, NULL);
				stream.next_out = (Bytef*)block[0];
				stream.avail_out = BUFFER_SIZE;
			}
			// copy as much as possible to buffer
			int err = deflate(&stream, Z_NO_FLUSH);
			ZeroAssert((err == Z_OK, "deflate error\n"));
		}
		count = size - stream.avail_in;
	}
	return count;
}

void ZeroStreamFile::NextBlock(void)
{
	HANDLE fh = (HANDLE)file;

	OVERLAPPED *overlapped = (OVERLAPPED *)MakeOffset(mdFields, 0);

	if (reading)
	{
		int i = headBlock & 1;
		overlapped[i].Offset = headBlock * BUFFER_SIZE;
		overlapped[i].OffsetHigh = 0;
		DWORD bytesRead;
		if (ReadFile(fh, block[i], BUFFER_SIZE, &bytesRead, &overlapped[i]))
		{
			ZeroTrace(("Huh?\n"));
		}
		else
		{
			DWORD err = GetLastError();
			if (err == ERROR_IO_PENDING)
			{
			}
			else
			{
				ZeroTrace(("What? offset(%d) err(%d)\n", headBlock * BUFFER_SIZE, err));
			}
		}
		headBlock++;
	}
	else
	{
	}
}

void ZeroStreamFile::WaitBlock(int blockNum)
{
	HANDLE fh = (HANDLE)file;

	OVERLAPPED *overlapped = (OVERLAPPED *)MakeOffset(mdFields, 0);

	int i = blockNum & 1;

	if (reading)
	{
		DWORD bytesRead;
		BOOL ok = GetOverlappedResult(fh, &overlapped[i], &bytesRead, TRUE);
		ok;
	}
}

void SplitPath(const char *path, char *drive, char *dir, char *name, char *ext)
{
	// get the drive part
	const char *drive_start = path;
	const char *drive_end = strrchr(drive_start, ':');
	if (drive_end == NULL)
	{
		drive_end = drive_start;
	}
	else
	{
		drive_end++;
	}
	if (drive)
	{
		memcpy(drive, drive_start, drive_end - drive_start);
		drive[drive_end - drive_start] = '\0';
	}
	
	// get the directory part
	const char *dir_start = drive_end;
	const char *dir_end = strrchr(dir_start, '\\');
	if (dir_end == NULL)
	{
		dir_end = strrchr(dir_start, '/');
	}
	if (dir_end == NULL)
	{
		dir_end = dir_start;
	}
	else
	{
		dir_end++;
	}
	if (dir)
	{
		memcpy(dir, dir_start, dir_end - dir_start);
		dir[dir_end - dir_start] = '\0';
	}

	// get the name part
	const char *name_start = dir_end;
	const char *name_end = strrchr(name_start, '.');
	if (name_end == NULL)
	{
		name_end = name_start + strlen(name_start);
	}
	if (name)
	{
		memcpy(name, name_start, name_end - name_start);
		name[name_end - name_start] = '\0';
	}

	// get the extension part
	const char *ext_start = name_end;
	const char *ext_end = ext_start + strlen(ext_start);
	if (ext)
	{
		memcpy(ext, ext_start, ext_end - ext_start);
		ext[ext_end - ext_start] = '\0';
	}
}
