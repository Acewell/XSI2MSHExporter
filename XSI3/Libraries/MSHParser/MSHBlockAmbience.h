/*
	CMSHBlockAmbience
	
	Class for .msh file data blocks with 'AMBI' header
	This class stores the ambient color for the scene in the MSH file
*/

#ifndef MSHBLOCK_AMBIENCE_H
#define MSHBLOCK_AMBIENCE_H

#include "MSHBlock.h"

class CMSHBlockAmbience: public CMSHBlock
{

protected:
	//	Ambient color
	float	m_fR,
			m_fG,
			m_fB,
			m_fA;

public:
	CMSHBlockAmbience(BlockHeader *pHdr,CZeroData *pData);
	virtual ~CMSHBlockAmbience();

	//--------------------
	//	Read/Write methods
	virtual void Read(FILE *file);
	virtual void Write(FILE *file);
	virtual void WriteText(FILE *);

	//--------------------
	//	Get methods
	void GetColor(float &fR,float &fG,float &fB,float &fA);

	//--------------------
	//	Set methods
	void SetColor(float fR, float fG, float fB, float fA);
};
#endif
