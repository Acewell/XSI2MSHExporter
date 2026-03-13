#include "MSHBlockLight.h"
#include "MSHBlockConstraintList.h"
#include <string.h>
CMSHBlockLight::CMSHBlockLight(BlockHeader *pHdr,CZeroData *pData):
	CMSHBlock(pHdr,pData)
{
	m_sPrefix=NULL;
	m_sName=NULL;
	m_uFlags = 0;

//	m_nConstraintLists = 0;
//	m_ppConstraintLists = NULL;
}

CMSHBlockLight::~CMSHBlockLight()
{
	if(m_sPrefix)
		delete[] m_sPrefix;
	if(m_sName)
		delete[] m_sName;


//	if(m_ppConstraintLists)
//	{
//		unsigned int i;
//		for(i=0;i<m_nConstraintLists;i++)
//			delete m_ppConstraintLists[i];
//		delete m_ppConstraintLists;
//	}
}

void CMSHBlockLight::SetPrefix(char *sPrefix)
{
	if(m_sPrefix)
		delete[] m_sPrefix;
	m_sPrefix = new char[strlen(sPrefix)+1];
	strcpy(m_sPrefix,sPrefix);
}

void CMSHBlockLight::SetName(char *sName)
{
	if(m_sName)
		delete[] m_sName;
	m_sName = new char[strlen(sName)+1];
	strcpy(m_sName,sName);
}

void CMSHBlockLight::SetColor(float fR, float fG, float fB, float fA)
{
	m_fR = fR;
	m_fG = fG;
	m_fB = fB;
	m_fA = fA;
}

void CMSHBlockLight::Read(FILE *)
{
}

void CMSHBlockLight::Write(FILE *)
{
}

void CMSHBlockLight::WriteText(FILE *f)
{
	fprintf(f,"Light\n{");
	if(m_sName)
		fprintf(f,"\t%s\n",m_sName);
	fprintf(f,"}\n");
}

char *CMSHBlockLight::GetPrefix()
{
	return m_sPrefix;
}

char *CMSHBlockLight::GetName()
{
	return m_sName;
}

void CMSHBlockLight::GetColor(float &fR,float &fG,float &fB,float &fA)
{
	fR=m_fR;
	fG=m_fG;
	fB=m_fB;
	fA=m_fA;
}

bool CMSHBlockLight::GetCastShadow()
{ 
	return m_uFlags & LIGHT_CAST_SHADOW;
}

void CMSHBlockLight::SetCastShadow(bool cast)
{ 
	if(cast)
		m_uFlags |= LIGHT_CAST_SHADOW;
	else
		m_uFlags &= ~LIGHT_CAST_SHADOW;
}

/*
CMSHBlockConstraintList **CMSHBlockLight::GetConstraintLists(int &nLists)
{
	nLists=m_nConstraintLists;
	return m_ppConstraintLists;
}

void CMSHBlockLight::AddConstraintList(CMSHBlockConstraintList *pNewCnsList)
{
	CMSHBlockConstraintList **pNewList;
	
	pNewList = new CMSHBlockConstraintList *[m_nConstraintLists+1];
	if(*m_ppConstraintLists)
	{
		memcpy(pNewList,m_ppConstraintLists, m_nConstraintLists*sizeof(CMSHBlockConstraintList *));
		delete[] m_ppConstraintLists;
	}
	pNewList[m_nConstraintLists]=pNewCnsList;
	m_ppConstraintLists = pNewList;
	m_nConstraintLists++;
}
*/
