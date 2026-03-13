/*
	CMSHBlockFogData
	
	Class for .msh file data blocks with 'FOGD' header
*/

#ifndef MSHBLOCK_FOGDATA_H
#define MSHBLOCK_FOGDATA_H

#include "MSHBlock.h"

class CMSHBlockFogData: public CMSHBlock
{

protected:
	//	Fog color
	float	m_fR,
			m_fG,
			m_fB,
			m_fA;
	
	//	Fog near plane and far plane
	float	m_fNearPlane,
			m_fFarPlane;

public:
	CMSHBlockFogData(BlockHeader *pHdr,CZeroData *pData);
	virtual ~CMSHBlockFogData();

	//--------------------
	//	File read/write methods
	virtual void Read(FILE *file);
	virtual void Write(FILE *file);
	virtual void WriteText(FILE *);

	//--------------------
	//	Get methods
	void GetColor(float &fR,float &fG,float &fB,float &fA);
	float GetNearPlane();
	float GetFarPlane();

	//--------------------
	//	Set methods
	void SetColor(float fR, float fG, float fB, float fA);
	void SetPlanes(float fNear, float fFar);
};
#endif
