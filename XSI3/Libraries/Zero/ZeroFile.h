/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroFile__
#define __ZeroFile__

#include <stdio.h>
#include <stdlib.h>

#include "ZeroTypes.h"
#include "ZeroList.h"

enum ZeroSaveType 
{
	ZeroAscii,
	ZeroBinary
};

struct ZeroFileDirEntry
{
	bool bIsDir;
	char szName[256];
	ZeroFileDirEntry & operator=(const ZeroFileDirEntry &s)
	{
		bIsDir = s.bIsDir;
		::strcpy(szName, s.szName);
		return *this;
	}
};

// allow the file system to use external caches
//
// externally cached file
class ZeroExtFile {
public:
	virtual ~ZeroExtFile() {}
	virtual void Release(void) {};
	virtual int GetSize(void) = 0;
	// use like a normal file
	virtual int Tell(void) = 0;
	virtual int Seek(int offset) = 0;
	virtual int Read(char *buffer, int size) = 0;
	virtual void Rewind(void)
	{
		Seek(0 - Tell());
	}
	// use like blocks of memory
	virtual void StartBlock(char *&buf, int &size)
	{
		buf = NULL;
		size = 0;
	}
	virtual void NextBlock(char *&buf, int &size)
	{
		buf = NULL;
		size = 0;
	}
	// if GetCmpSize() != GetSize() then file data is compressed
	virtual int GetCmpSize(void)
	{
		return GetSize();
	}
};
// external file cache
class ZeroExtCache {
public:
	virtual ~ZeroExtCache() {}

	// open a file from the cache, return NULL if not cached
	virtual ZeroExtFile *Open(char *name) = 0;
};

class ZeroFile 
{
	int handle;
	char *bufBase;
	int bufSize;
	mutable char *bufPtr;
	bool isBinary;
	bool isCompressed;
	bool isBatch;
	int cmpSize;
	void *cmpState;
	ZeroExtFile *extFile;

protected:

	DLL_DECLSPEC int Vscanf (const char *format, va_list arglist) const;
	DLL_DECLSPEC int Inc () const;
	DLL_DECLSPEC int UnInc (int ch) const;
	DLL_DECLSPEC int SkipWhite () const;

	int Scan(const char *format, va_list arglist) const;
	int BinScan(const char *format, va_list arglist) const;
	int BinFindToken (const char *token) const;
	char *BinGets(char *string, int n) const;

public:
	DLL_DECLSPEC ZeroFile ();
	DLL_DECLSPEC ZeroFile (const char *Name, const char *type);
	virtual DLL_DECLSPEC ~ZeroFile ();
	
	DLL_DECLSPEC void OpenFile (const char *Name, const char *type);	
	DLL_DECLSPEC void CloseFile();

	operator int() const
	{
		return (handle >= 0);
	}
	
	
	//	static DLL_DECLSPEC void __cdecl FindInit (const char *path);
	static DLL_DECLSPEC void __cdecl ClearPaths (void);
	static DLL_DECLSPEC void __cdecl AddPath (const char *path, bool recurse);
	static DLL_DECLSPEC bool __cdecl SearchFile (const char *name, const char *path, char *fullPathName);
	static DLL_DECLSPEC bool __cdecl FindFile (const char *name, const char *path, char *fullPathName);
	static DLL_DECLSPEC int __cdecl FindCookie (const char *name);
	static DLL_DECLSPEC bool __cdecl CheckDependency(const char *pathFileName, const char *pathDependencyFileName);
	static DLL_DECLSPEC void SetCache(bool on);
	static DLL_DECLSPEC void SetExtCache(ZeroExtCache *extCache);

	DLL_DECLSPEC int Eof() const;
	
	DLL_DECLSPEC int SeekSet (int offset) const;
	DLL_DECLSPEC int SeekCur (int offset) const;
	DLL_DECLSPEC int SeekEnd (int offset) const;
	DLL_DECLSPEC int Tell () const;
	DLL_DECLSPEC int Size () const;
	
	DLL_DECLSPEC int __cdecl Scanf (const char *format, ...) const;
	DLL_DECLSPEC int __cdecl Print (const char *format, ...) const;
	DLL_DECLSPEC int __cdecl ReadToken (const char *Token, const char *format, ...) const;
	DLL_DECLSPEC int __cdecl ReadSectionToken (const char *section, const char *token, const char *format, ...) const;

	// endian conversion
	DLL_DECLSPEC int Read8 (void *const buffer, int size, int count) const;
	DLL_DECLSPEC int Read16 (void *const buffer, int size, int count) const;
	DLL_DECLSPEC int Read32 (void *const buffer, int size, int count) const;

	DLL_DECLSPEC int Read (void *const buffer, int size, int count) const;
	DLL_DECLSPEC int Write (const void *buffer, int size, int count) const;
	DLL_DECLSPEC int FindToken (const char *Token) const;
	
	DLL_DECLSPEC char *Gets (char *string, int n) const;
	DLL_DECLSPEC int Getc() const;
	DLL_DECLSPEC int Putc (int ch) const;
	DLL_DECLSPEC int Ungetc (int ch) const;
	DLL_DECLSPEC int Flush() const;
	DLL_DECLSPEC void Rewind() const;

	static DLL_DECLSPEC void GetDirListing(const char * dir, ZeroVisList<ZeroFileDirEntry> & list);

	char *GetBuffer() const
	{
		return bufBase;
	}

	static DLL_DECLSPEC void BeginBatch(const char *name);
	static DLL_DECLSPEC void EndBatch(void);
	static DLL_DECLSPEC void BatchOdf(void);
};

class ZeroStreamFile {
public:
	static ZeroStreamFile *Open(const char *name, bool read = true);

	ZeroStreamFile(const char *name, bool read = true);
	~ZeroStreamFile();

	int Stream(char *buffer, int size);
	bool IsReading(void) { return reading; }
	bool IsOpen(void) { return (file != -1); }

protected:
	void NextBlock(void);
	void WaitBlock(int blockNum);

	int file;
	bool reading;
	char *blockBuffer;
	char *block[2];
	int mdFields[128];	// machine dependent fields
	int headBlock;
	int tailBlock;
};

DLL_DECLSPEC void SplitPath(const char *path, char *drive, char *dir, char *name, char *ext);

#endif
