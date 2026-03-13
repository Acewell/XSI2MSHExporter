/*
	CMSHBlockSpotLight
	
	Class for .msh file data blocks with 'LGTS' header
*/

#ifndef MSHBLOCK_SPOTLIGHT_H
#define MSHBLOCK_SPOTLIGHT_H

#include "MSHBlockLight.h"

class CMSHBlockSpotLight: public CMSHBlockLight
{

protected:
	//	light position
	float m_fPositionX,m_fPositionY,m_fPositionZ;
	//	light interest
	float m_fInterestX,m_fInterestY,m_fInterestZ;
	//	light falloff info
	float m_fFalloffStart, m_fFalloffEnd;
	//	light cone angle (in radians)
	float m_fConeAngle;
	//	light spread angle (in radians)
	float m_fSpreadAngle;

public:
	CMSHBlockSpotLight(BlockHeader *pHdr,CZeroData *pData);
	virtual ~CMSHBlockSpotLight();

	//--------------------
	//	File read/write methods
	virtual void Read(FILE *file);
	virtual void Write(FILE *file);
	virtual void WriteText(FILE *);

	//--------------------
	//	Get methods
	void GetPosition(float &fX,float &fY,float &fZ);
	void GetInterest(float &fX,float &fY,float &fZ);
	float GetConeAngle();
	float GetSpreadAngle();
	float GetFalloffStart();
	float GetFalloffEnd();

	//--------------------
	//	Set methods
	void SetPosition(float fX, float fY, float fZ);
	void SetFalloff(float fFalloffStart, float fFalloffEnd);
	void SetInterest(float fX, float fY, float fZ);
	void SetConeAngle(float fConeAngle);
	void SetSpreadAngle(float fSpreadAngle);
};
#endif
