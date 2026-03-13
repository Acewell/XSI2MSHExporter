/*
	CMSHBlockPointLight
	
	Class for .msh file data blocks with 'LGTP' header
*/

#ifndef MSHBLOCK_POINTLIGHT_H
#define MSHBLOCK_POINTLIGHT_H

#include "MSHBlockLight.h"


class CMSHBlockPointLight: public CMSHBlockLight
{

protected:
	//	position of light
	float m_fPositionX,m_fPositionY,m_fPositionZ;
	//	light falloff info
	float m_fFalloffStart, m_fFalloffEnd;

public:
	CMSHBlockPointLight(BlockHeader *pHdr,CZeroData *pData);
	virtual ~CMSHBlockPointLight();

	//--------------------
	//	File read/write methods
	virtual void Read(FILE *file);
	virtual void Write(FILE *file);
	virtual void WriteText(FILE *);

	//--------------------
	//	Get methods
	void GetPosition(float &fX,float &fY,float &fZ);
	float GetFalloffStart();
	float GetFalloffEnd();

	//--------------------
	//	Set methods
	void SetPosition(float fX, float fY, float fZ);
	void SetFalloff(float fFalloffStart, float fFalloffEnd);
	static float *debugLightY;
};
#endif