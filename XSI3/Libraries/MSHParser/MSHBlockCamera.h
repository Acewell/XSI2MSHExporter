/*
	CMSHBlockCamera
	
	Class for .msh file data blocks with 'CAMR' header
*/

#ifndef MSHBLOCK_CAMERA_H
#define MSHBLOCK_CAMERA_H

#include "MSHBlock.h"
#include "MSHBlockAnimations.h"


class CMSHBlockConstraintList;

class CMSHCameraKeyFrame
{
public:
	union {
		int key;
		int index;
	};
	int mark;
	ZeroVector3 position;
	ZeroQuaternion rotation;
	float mFOV; // in radians

	CMSHCameraKeyFrame()
	{
		key = 0;
		mark = 0;
		mFOV = 1.3963f; // 80 degrees
	}
};

class CMSHBlockCameraAnimation: public ZeroSet<CMSHCameraKeyFrame>
{
	int mNumKeyframes;
	char mName[32];
	
public:
	CMSHBlockCameraAnimation(char *aName);
	virtual ~CMSHBlockCameraAnimation();
	
	void SetKeyframesCount (int count)
	{
		mNumKeyframes = count; 
		(*this)[count];
	}
	
	int GetKeyframesCount() const
	{
		return mNumKeyframes;
	}
	
	//	File read/write methods
	void Read(FILE *file);
	void Write(FILE *file);
};

class CMSHBlockCamera: public CMSHBlock
{

protected:
	ZeroVisList<CMSHBlockCameraAnimation> m_Anims;

	char *m_sPrefix;
	char *m_sName;
	
	//	Camera position
	float m_fPositionX,m_fPositionY,m_fPositionZ;
	
	//	Camera interest
	float m_fInterestX,m_fInterestY,m_fInterestZ;
	
	//	Camera roll angle (in radians)
	float m_fRoll; 
	
	//	Camera field of view (in radians)
	float m_fFOV; 
	
	//	Camera near and far planes
	float	m_fNearPlane,
			m_fFarPlane;

	//	Camera constraints
//	unsigned int m_nConstraintLists;
//	CMSHBlockConstraintList **m_ppConstraintLists;

public:
	CMSHBlockCamera(BlockHeader *pHdr,CZeroData *pData);
	virtual ~CMSHBlockCamera();

	//--------------------
	//	File read/write methods
	virtual void Read(FILE *file);
	virtual void Write(FILE *file);
	virtual void WriteText(FILE *);

	//--------------------
	//	Set methods
	void SetPrefix(char *sPrefix);
	void SetName(char *sName);
	void SetPosition(float fX, float fY, float fZ);
	void SetInterest(float fX, float fY, float fZ);
	void SetRoll(float fRoll);
	void SetFOV(float fFOV);
	void SetPlanes(float fNear, float fFar);
//	void AddConstraintList(CMSHBlockConstraintList *pNewCnsList);

	//--------------------
	//	Get methods
	char *GetPrefix();
	char *GetName();
	void GetPosition(float &fX,float &fY,float &fZ);
	void GetInterest(float &fX,float &fY,float &fZ);
	float GetRoll();
	float GetFOV();
	float GetNearPlane();
	float GetFarPlane();

	CMSHBlockCameraAnimation* CreateAnimation (char *aName);

	ZeroVisList<CMSHBlockCameraAnimation> &GetAnimations(){return m_Anims;};
//	CMSHBlockConstraintList **GetConstraintLists(int &nLists);
};
#endif