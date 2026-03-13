/*
	CMSHBlockFileInfo
	
	Class for .msh file data blocks with 'FINF' header
*/

#ifndef MSHBLOCK_FILEINFO_H
#define MSHBLOCK_FILEINFO_H

#include "MSHBlock.h"

class CMSHBlockFileInfo: public CMSHBlock
{

protected:
	//	name of project
	char *m_sProjectName;

	// name of user
	char *m_sUserName;

	// last time file was saved
	char *m_sTime;

public:
	CMSHBlockFileInfo(BlockHeader *pHdr,CZeroData *pData);
	virtual ~CMSHBlockFileInfo();

	//--------------------
	//	File read/write methods
	virtual void Read(FILE *file);
	virtual void Write(FILE *file);
	virtual void WriteText(FILE *);

	//--------------------
	//	Set methods
	void SetProjectName(char *sProjectName);
	void SetUserName(char *sUserName);
};
#endif
