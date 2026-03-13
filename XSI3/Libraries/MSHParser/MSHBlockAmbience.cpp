#include "MSHBlockAmbience.h"

CMSHBlockAmbience::CMSHBlockAmbience(BlockHeader *pHdr,CZeroData *pData):
	CMSHBlock(pHdr,pData)
{
	m_fA=m_fR=m_fG=m_fB=0.0;
}

CMSHBlockAmbience::~CMSHBlockAmbience()
{
}

void CMSHBlockAmbience::Read(FILE *file)
{
	ReadData(file, &m_fR, sizeof(float));
	ReadData(file, &m_fG, sizeof(float));
	ReadData(file, &m_fB, sizeof(float));
	ReadData(file, &m_fA, sizeof(float));
}

void CMSHBlockAmbience::Write(FILE *file)
{
	// start block
	StartBlock(file, 'AMBI');
	WriteData(file,&m_fR,sizeof(float));
	WriteData(file, &m_fG, sizeof(float));
	WriteData(file, &m_fB, sizeof(float));
	WriteData(file, &m_fA, sizeof(float));
	EndBlock(file);
	
}

void CMSHBlockAmbience::WriteText(FILE *)
{
}

void CMSHBlockAmbience::SetColor(float fR, float fG, float fB, float fA)
{
	m_fR = fR;
	m_fG = fG;
	m_fB = fB;
	m_fA = fA;
}

void CMSHBlockAmbience::GetColor(float &fR,float &fG,float &fB,float &fA)
{
	fR=m_fR;
	fG=m_fR;
	fB=m_fB;
	fA=m_fA;
}

