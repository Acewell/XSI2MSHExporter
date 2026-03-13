/*
	CMSHBlockInfiniteLight
	
	Class for .msh file data blocks with 'LGTI' header
*/

#ifndef MSHBLOCK_INFINITELIGHT_H
#define MSHBLOCK_INFINITELIGHT_H

#include "MSHBlockLight.h"

class CMSHBlockConstraintList;

class CMSHBlockInfiniteLight: public CMSHBlockLight
{

protected:
	//	Direction of light
	float m_fDirectionX,m_fDirectionY,m_fDirectionZ;
public:
	CMSHBlockInfiniteLight(BlockHeader *pHdr,CZeroData *pData);
	virtual ~CMSHBlockInfiniteLight();

	//--------------------
	//	File read/write methods
	virtual void Read(FILE *file);
	virtual void Write(FILE *file);
	virtual void WriteText(FILE *);

	//--------------------
	//	Get methods
	void GetDirection(float &fX,float &fY,float &fZ);

	//--------------------
	//	Set methods
	void SetDirection(float fX, float fY, float fZ);
};
#endif