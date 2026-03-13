#include "MSHBlockFileInfo.h"
#include <ctype.h>
#include <time.h>
#include <string.h>

CMSHBlockFileInfo::CMSHBlockFileInfo(BlockHeader *pHdr,CZeroData *pData):
	CMSHBlock(pHdr,pData)
{
	m_sProjectName = NULL;
	m_sUserName = NULL;
	m_sTime = NULL;
}

CMSHBlockFileInfo::~CMSHBlockFileInfo()
{
	if(m_sProjectName)
		delete[] m_sProjectName;
	if(m_sUserName)
		delete[] m_sUserName;
	if(m_sTime)
		delete[] m_sTime;
}

void CMSHBlockFileInfo::Read(FILE *file)
{
	unsigned int blockSize=m_Header.size;

	while (blockSize)
	{
		// read a block header
		BlockHeader header;
		ReadHeader(file,&header);
		
		// deduct blockSize
		assert(blockSize >= sizeof(BlockHeader) + header.size);
		blockSize -= sizeof(BlockHeader);
		blockSize -= header.size;
		
		// parse the block
		switch (header.type)
		{
		case 'JORP':
			ReadString(file, header.size,&m_sProjectName);
			break;
		case 'RESU':
			ReadString(file, header.size,&m_sUserName);
			break;
		case 'EMIT':
			ReadString(file, header.size,&m_sTime);
			break;
		default:
			SkipBlock(file, header.size);
			break;
		}
	}
}

void CMSHBlockFileInfo::Write(FILE *file)
{
	// get the current time
	time_t lTime;
	time(&lTime);

	// convert time to text
	char szTime[30];
	strcpy(szTime, ctime(&lTime));
	szTime[24] = '\0';

	StartBlock(file, 'FINF');

		if (m_sProjectName)
		{
			StartBlock(file, 'PROJ');
			WriteString(file, m_sProjectName);
			EndBlock(file);
		}

		if (m_sUserName)
		{
			StartBlock(file, 'USER');
			WriteString(file, m_sUserName);
			EndBlock(file);
		}

		StartBlock(file, 'TIME');
		WriteString(file, szTime);
		EndBlock(file);

	EndBlock(file);
}

void CMSHBlockFileInfo::WriteText(FILE *)
{
}

void CMSHBlockFileInfo::SetProjectName(char *sProjectName)
{
	if(m_sProjectName)
		delete[] m_sProjectName;
	m_sProjectName = new char[strlen(sProjectName)+1];
	strcpy(m_sProjectName, sProjectName);
}

void CMSHBlockFileInfo::SetUserName(char *sUserName)
{
	if(m_sUserName)
		delete[] m_sUserName;
	m_sUserName = new char[strlen(sUserName)+1];
	strcpy(m_sUserName, sUserName);
}

