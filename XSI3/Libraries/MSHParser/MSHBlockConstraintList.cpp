#include "MSHBlockConstraintList.h"
#include <string.h>


/*
CMSHBlockConstraintList::CMSHBlockConstraintList(BlockHeader *pHdr,CZeroData *pData):
	CMSHBlock(pHdr,pData)
{
	m_sType = NULL;
	m_sObjectName = NULL;
	m_nConstraintNames = 0;
	m_ppConstraintNames = NULL;
}

CMSHBlockConstraintList::~CMSHBlockConstraintList()
{
	if(m_sType)
		delete[] m_sType;
	if(m_sObjectName)
		delete[] m_sObjectName;
	
	if(m_ppConstraintNames)
	{
		unsigned int i;
		for(i=0;i<m_nConstraintNames;i++)
			delete m_ppConstraintNames[i];
		delete m_ppConstraintNames;
	}
}

void CMSHBlockConstraintList::Read(FILE *file)
{
	unsigned int blockSize=m_Header.size;
	BlockHeader header;

	// 'NAME'
	// read a block header
	ReadHeader(file,&header);
	// deduct blockSize
	assert(blockSize >= sizeof(BlockHeader) + header.size);
	blockSize -= sizeof(BlockHeader);
	blockSize -= header.size;
	ReadString(file, header.size,&m_sType);

	
	// 'OBJT'
	// read a block header
	ReadHeader(file,&header);
	// deduct blockSize
	assert(blockSize >= sizeof(BlockHeader) + header.size);
	blockSize -= sizeof(BlockHeader);
	blockSize -= header.size;
	ReadString(file, header.size,&m_sObjectName);

	ReadData(file,&m_nConstraintNames,sizeof(unsigned int));
	blockSize -= sizeof(unsigned int);

	if(m_nConstraintNames)
	{
		unsigned int i;
		m_ppConstraintNames = new char *[m_nConstraintNames];
		for(i=0;i<m_nConstraintNames;i++)
		{
			// read a block header
			ReadHeader(file,&header);
			// deduct blockSize
			assert(blockSize >= sizeof(BlockHeader) + header.size);
			blockSize -= sizeof(BlockHeader);
			blockSize -= header.size;
			ReadString(file, header.size,&(m_ppConstraintNames[i]));
		}
	}
	assert(blockSize==0);
}

void CMSHBlockConstraintList::Write(FILE *fp)
{
	StartBlock(fp,'CLST');

		StartBlock(fp,'NAME');
		WriteString(fp,m_sType);
		EndBlock(fp);

		StartBlock(fp,'OBJT');
		WriteString(fp,m_sObjectName);
		EndBlock(fp);

		WriteData(fp,&m_nConstraintNames,sizeof(unsigned int));

		unsigned int i;
		for(i=0;i<m_nConstraintNames;i++)
		{
			StartBlock(fp,'CONS');
			WriteString(fp,m_ppConstraintNames[i]);
			EndBlock(fp);
		}

	EndBlock(fp);
}

void CMSHBlockConstraintList::SetType(char *sType)
{
	if(m_sType)
		delete[] m_sType;
	m_sType = new char[strlen(sType)+1];
	strcpy(m_sType,sType);
}

void CMSHBlockConstraintList::SetObjectName(char *sObjectName)
{
	if(m_sObjectName)
		delete[] m_sObjectName;
	m_sObjectName = new char[strlen(sObjectName)+1];
	strcpy(m_sObjectName,sObjectName);
}

void CMSHBlockConstraintList::AddConstraint(char *sCnsName)
{
	char **pNewList;
	
	pNewList = new char *[m_nConstraintNames+1];
	if(m_ppConstraintNames)
	{
		memcpy(pNewList,m_ppConstraintNames, m_nConstraintNames*sizeof(char *));
		delete[] m_ppConstraintNames;
	}
	pNewList[m_nConstraintNames]=new char[strlen(sCnsName)+1];
	strcpy(pNewList[m_nConstraintNames],sCnsName);
	m_ppConstraintNames = pNewList;
	m_nConstraintNames++;
}

void CMSHBlockConstraintList::WriteText(FILE *)
{
}

char *CMSHBlockConstraintList::GetType()
{
	return m_sType;
}

char *CMSHBlockConstraintList::GetObjectName()
{
	return m_sObjectName;
}

char **CMSHBlockConstraintList::GetConstraintNames(unsigned int &nNames)
{
	nNames=m_nConstraintNames;
	return m_ppConstraintNames;
}
*/