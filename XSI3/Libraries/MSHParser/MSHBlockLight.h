/*
	CMSHBlockLight

  Base class for the following light classes:
	CMSHBlockSpotLight
	CMSHBlockInfiniteLight
	CMSHBlockPointLight
	
*/

#ifndef MSHBLOCK_LIGHT_H
#define MSHBLOCK_LIGHT_H

#include "MSHBlock.h"
#include "ZeroLight.h" // for LIGHT_CAST_SHADOW

//class CMSHBlockConstraintList;

class CMSHBlockLight: public CMSHBlock
{

protected:
	char *m_sPrefix;
	char *m_sName;

	//	Light color
	float	m_fR,
			m_fG,
			m_fB,
			m_fA;

	// Light property flags
	unsigned int m_uFlags;

	//	Light constraints
//	unsigned int m_nConstraintLists;
//	CMSHBlockConstraintList **m_ppConstraintLists;

public:
	CMSHBlockLight(BlockHeader *pHdr,CZeroData *pData);
	virtual ~CMSHBlockLight();

	//--------------------
	//	File read/write methods
	virtual void Read(FILE *);
	virtual void Write(FILE *);
	virtual void WriteText(FILE *);

	//--------------------
	//	Get methods
	char *GetPrefix();
	char *GetName();
	void GetColor(float &fR,float &fG,float &fB,float &fA);
	bool GetCastShadow() ;

	//--------------------
	// Set methods
	void SetPrefix(char *sPrefix);
	void SetName(char *sName);
	void SetColor(float fR, float fG, float fB, float fA);
	void SetCastShadow(bool cast);

//	CMSHBlockConstraintList **GetConstraintLists(int &nLists);
//	void AddConstraintList(CMSHBlockConstraintList *pNewCnsList);
};
#endif