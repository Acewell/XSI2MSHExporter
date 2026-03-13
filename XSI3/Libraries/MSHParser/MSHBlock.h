/*
	CMSHBlock
	
	Base class for data blocks stored in .msh file format
	Descendants should override:
		Read()
		Write()
		WriteText()
*/

#ifndef MSHBLOCK_H
#define MSHBLOCK_H

#include "stdio.h"
#include "assert.h"
#include "memory.h"

class ZeroFile;
class CZeroData;
//class CMSHBlockConstraintList;

class CMSHBlock
{
protected:
	CZeroData *m_pData;
	
public:
	class BlockHeader
	{
		public:
			unsigned int type;
			unsigned int size;
	};

    CMSHBlock();

	CMSHBlock(BlockHeader *pHdr,CZeroData *pData);
	virtual ~CMSHBlock();

	// Override these functions to read/write a specific block type.
	virtual void Read(FILE *fp) = 0;
	virtual void Write(FILE *fp) = 0;
	virtual void WriteText(FILE *fp) = 0;

	//	Read in a CMSHBlock::BlockHeader
	static void ReadHeader(FILE *file, BlockHeader *hdr);

	//	Read generic data from file
	static void ReadData(FILE *file, void *pData, int nSize);

	static void ReadString(FILE *file, const unsigned int size, char **ppOutStr);

	// Write generic data to file
	static void WriteData(FILE *file, const void *pData, int nSize);
	
	// Write a string to file
	static void WriteString(FILE *file, const char *szString);

	//	Skip ahead blockSize bytes in file
	static void SkipBlock(FILE *file, unsigned int blockSize);

	//	Start a file block of type ulTypeID
	static void StartBlock(FILE *file, unsigned int ulTypeID);

	// Finish the topmost block in header_stack.  Specifically, this functions sets
	//	the block size for the current block.
	static void EndBlock(FILE *file);

	//	Start a file block of type ulTypeID
	static void StartZeroFileBlock(ZeroFile &file, unsigned int ulTypeID);

	// Finish the topmost block in header_stack.  Specifically, this functions sets
	//	the block size for the current block.
	static void EndZeroFileBlock(ZeroFile &file);

	//	Sets this object's m_Header member
	void SetHeader(BlockHeader *pHdr);
	void SetData(CZeroData *pData);

protected:
	// m_Header identifies the type and size of this block in the file.
	BlockHeader m_Header;
	// header stack
	static BlockHeader header_stack[16];
	// header pointer
	static BlockHeader *header_ptr;

	static unsigned int BEND(unsigned int l);

//	void ReadConstraintList(FILE *file, BlockHeader *pHdr, CMSHBlockConstraintList ***pppConstraintLists, unsigned int &nConstraintLists);
};
#endif
