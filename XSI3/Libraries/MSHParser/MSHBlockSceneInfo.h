/*
	CMSHBlockSceneInfo
	
	Class for .msh file data blocks with 'SINF' header
*/

#ifndef MSHBLOCK_SCENEINFO_H
#define MSHBLOCK_SCENEINFO_H

#include "MSHBlock.h"

class CMSHBlockSceneInfo: public CMSHBlock
{

protected:
	//	name of scene
	char *m_sSceneName;

	//	frame info for scene
	int m_StartFrame;
	int	m_EndFrame;
	float m_FrameRate;

    // scene bounding box
	float m_fSizeX, m_fSizeY, m_fSizeZ, m_fSizeW;
	float m_fTransX, m_fTransY, m_fTransZ;
//	float m_fQuatRotX, m_fQuatRotY, m_fQuatRotZ, m_fQuatRotW;


public:
	CMSHBlockSceneInfo(BlockHeader *pHdr,CZeroData *pData);
	virtual ~CMSHBlockSceneInfo();

	//--------------------
	//	File read/write methods
	virtual void Read(FILE *file);
	virtual void Write(FILE *file);
	virtual void WriteText(FILE *);

	//--------------------
	//	Get methods
	char *GetName();
	float GetFrameRate();
	int GetStartFrame();
	int GetEndFrame();

	//--------------------
	//	Set methods
	void SetSceneName(char *sSceneName);
	void SetFrameInfo(int StartFrame, int EndFrame, float FrameRate);

    void SetGlobalBBox(float SizeX, float SizeY, float SizeZ, float SizeW, float TransX, float TransY, float TransZ);

};
#endif