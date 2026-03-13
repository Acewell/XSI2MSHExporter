#include "MSHBlockFogData.h"

CMSHBlockFogData::CMSHBlockFogData(BlockHeader *pHdr,CZeroData *pData):
	CMSHBlock(pHdr,pData)
{
	m_fA=m_fR=m_fG=m_fB=0.0;
	m_fNearPlane=0.0;
	m_fFarPlane=100.0;
}

CMSHBlockFogData::~CMSHBlockFogData()
{
}

void CMSHBlockFogData::Read(FILE *file)
{
	ReadData(file, &m_fR, sizeof(float));
	ReadData(file, &m_fG, sizeof(float));
	ReadData(file, &m_fB, sizeof(float));
	ReadData(file, &m_fA, sizeof(float));
	ReadData(file, &m_fNearPlane, sizeof(float));
	ReadData(file, &m_fFarPlane, sizeof(float));
}

void CMSHBlockFogData::Write(FILE *file)
{
	StartBlock(file, 'FOGD');
	WriteData(file, &m_fR, sizeof(float));
	WriteData(file, &m_fG, sizeof(float));
	WriteData(file, &m_fB, sizeof(float));
	WriteData(file, &m_fA, sizeof(float));
	WriteData(file, &m_fNearPlane, sizeof(float));
	WriteData(file, &m_fFarPlane, sizeof(float));
	EndBlock(file);
}

void CMSHBlockFogData::SetColor(float fR, float fG, float fB, float fA)
{
	m_fR = fR;
	m_fG = fG;
	m_fB = fB;
	m_fA = fA;
}

void CMSHBlockFogData::SetPlanes(float fNear, float fFar)
{
	m_fNearPlane = fNear;
	m_fFarPlane = fFar;
}

void CMSHBlockFogData::WriteText(FILE *)
{
}

void CMSHBlockFogData::GetColor(float &fR,float &fG,float &fB,float &fA)
{
	fR=m_fR;
	fG=m_fR;
	fB=m_fB;
	fA=m_fA;
}

float CMSHBlockFogData::GetNearPlane()
{
	return m_fNearPlane;
}

float CMSHBlockFogData::GetFarPlane()
{
	return m_fFarPlane;
}

