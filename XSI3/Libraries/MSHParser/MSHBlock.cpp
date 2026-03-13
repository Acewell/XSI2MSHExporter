#include "Zero.h"
#include "MSHBlock.h"
#include "string.h"
#include "MSHBlockConstraintList.h"

CMSHBlock::BlockHeader CMSHBlock::header_stack[16];
CMSHBlock::BlockHeader *CMSHBlock::header_ptr = &header_stack[0];

CMSHBlock::CMSHBlock()
{
    m_Header.type = 0;
    m_Header.size = 0;
	m_pData = NULL;
}


CMSHBlock::CMSHBlock(BlockHeader *pHdr,CZeroData *pData)
{
	m_Header.type=pHdr ->type;
	m_Header.size=pHdr ->size;
	m_pData=pData;
}

CMSHBlock::~CMSHBlock()
{
}

void CMSHBlock::SetHeader(BlockHeader *pHdr)
{
	m_Header.type=pHdr ->type;
	m_Header.size=pHdr ->size;
}


void CMSHBlock::SetData(CZeroData *pData)
{
	m_pData=pData;
}

void CMSHBlock::ReadHeader(FILE *file, BlockHeader *pHdr)
{
#ifdef ASCII_OUTPUT
#else
	fread(pHdr, sizeof(BlockHeader), 1, file);
#endif
}

void CMSHBlock::StartBlock(FILE *file, unsigned int ulTypeID)
{
#ifdef ASCII_OUTPUT
#else

	// fill in the header data
	header_ptr->type = BEND(ulTypeID);
	header_ptr->size = ftell(file);

	// write the header to the file
	fwrite(header_ptr, sizeof(BlockHeader), 1, file);

	// push the header onto the stack
	++header_ptr;

#endif
}

void CMSHBlock::EndBlock(FILE *file)
{
#ifdef ASCII_OUTPUT
#else

	// pop the header off the stack
	--header_ptr;

	// save the current position
	// (longword aligned)
	int cur_pos = (ftell(file) + 3) & ~3;

	// seek to the header position
	fseek(file, header_ptr->size, SEEK_SET);

	// finalize the block size
	header_ptr->size = cur_pos - header_ptr->size - sizeof(BlockHeader);

	// rewrite the header to the file
	fwrite(header_ptr, sizeof(BlockHeader), 1, file);

	// seek to the current position
	fseek(file, cur_pos, SEEK_SET);

#endif
}

void CMSHBlock::StartZeroFileBlock(ZeroFile &file, unsigned int ulTypeID)
{
#ifdef ASCII_OUTPUT
#else

	// fill in the header data
	header_ptr->type = BEND(ulTypeID);
	header_ptr->size = file.Tell();

	// write the header to the file
	file.Write(header_ptr, sizeof(BlockHeader), 1);

	// push the header onto the stack
	++header_ptr;

#endif
}

void CMSHBlock::EndZeroFileBlock(ZeroFile &file)
{
#ifdef ASCII_OUTPUT
#else

	// pop the header off the stack
	--header_ptr;

	// save the current position
	// (longword aligned)
	int cur_pos = (file.Tell() + 3) & ~3;

	// seek to the header position
	file.SeekSet(header_ptr->size);

	// finalize the block size
	header_ptr->size = cur_pos - header_ptr->size - sizeof(BlockHeader);

	// rewrite the header to the file
	file.Write(header_ptr, sizeof(BlockHeader), 1);

	// seek to the current position
	file.SeekSet(cur_pos);

#endif
}

//***************************************************************************************
//
//	Function : ReadData
//	Author : CGF
//	Date : 3/9/01
//	Arguments :		file = input file
//					pData = output data
//					nSize = # of bytes to read
//	Return Value :
//	Comments :

//***************************************************************************************
void CMSHBlock::ReadData(FILE *file, void *pData, int nSize)
{
#ifdef ASCII_OUTPUT
#else
	fread(pData, nSize, 1, file);
#endif
}

//***************************************************************************************
//
// Author : Games S.W.A.T. Team
// Date   : 
//
// @mfunc   write arbitrary binary data to a file
//
// @rvalue	void
//
// @comm :  
//
// @devnote	
//***************************************************************************************
void CMSHBlock::WriteData(FILE *file, const void *pData, int nSize)
{
	fwrite(pData, nSize, 1, file);
}

//***************************************************************************************
//
//	Function : ReadString
//	Author : CGF
//	Date : 3/9/01
//	Arguments :		file = input file
//					size = size of string to read
//					ppOutStr = output string
//	Return Value :
//	Comments : This function allocates *ppOutStr.  Caller of function is responsible
//				for de-allocating

//***************************************************************************************
void CMSHBlock::ReadString(FILE *file, const unsigned int size, char **ppOutStr)
{
	// allocate space for the name
	*ppOutStr=new char[size];
	// read the name
	fread(*ppOutStr, size, 1, file);
//	*ppOutStr[size]=0;
}

//***************************************************************************************
//
//	Function : WriteString
//	Author : CGF
//	Date : 3/6/01
//	Arguments :
//	Return Value :
//	Comments :

//***************************************************************************************
void CMSHBlock::WriteString(FILE *file, const char *szString)
{
#ifdef ASCII_OUTPUT
#else
	fwrite(szString, 1, strlen(szString) + 1, file);
#endif
}

//***************************************************************************************
//
//	Function : ReadConstraintList
//	Author : CGF
//	Date : 3/9/01
//	Arguments :		file = input file
//					pHdr = BlockHeader for the constraintlist to read
//					ppConstraintLists = array of constraintlists, receives the new constraintlist
//					unsigned int = number of constraintlists in ppConstraintLists
//	Return Value :
//	Comments :	Allocates a new CMSHBlockConstraintList and adds it to
//				ppConstraintLists

//***************************************************************************************

/*
void CMSHBlock::ReadConstraintList(FILE *file, BlockHeader *pHdr, CMSHBlockConstraintList ***pppConstraintLists, unsigned int &nConstraintLists)
{
	CMSHBlockConstraintList **pNewList;
	CMSHBlockConstraintList *pConstraintList = new CMSHBlockConstraintList(pHdr,m_pData);
	
	pConstraintList ->Read(file);

	pNewList = new CMSHBlockConstraintList *[nConstraintLists+1];
	if(*pppConstraintLists)
	{
		memcpy(pNewList,*pppConstraintLists, nConstraintLists*sizeof(CMSHBlockConstraintList *));
		delete[] *pppConstraintLists;
	}
	pNewList[nConstraintLists]=pConstraintList;
	*pppConstraintLists = pNewList;
	nConstraintLists++;
}
*/

//***************************************************************************************
//
//	Function : SkipBlock
//	Author : CGF
//	Date : 3/9/01
//	Arguments :		file = input file
//					blockSize = # of bytes to skip in file
//	Return Value :
//	Comments :

//***************************************************************************************
void CMSHBlock::SkipBlock(FILE *file, unsigned int blockSize)
{
	fseek(file,blockSize, SEEK_CUR);
}

unsigned int CMSHBlock::BEND(unsigned int l)
{
	return (int) (
		((l & 0x000000FF) << 24) | 
		((l & 0x0000FF00) << 8)  | 
		((l & 0x00FF0000) >> 8)  | 
		((l & 0xFF000000) >> 24)
		);
};
