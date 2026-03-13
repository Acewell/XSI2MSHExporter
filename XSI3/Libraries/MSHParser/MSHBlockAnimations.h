/*
CMSHBlockAnimationList

  Base class for the following light classes:
  CMSHBlockSpotLight
  CMSHBlockInfiniteLight
  CMSHBlockPointLight
  
*/

#ifndef MSHBLOCK_ANIMATION_H
#define MSHBLOCK_ANIMATION_H

#include "zero.h"
#include "MSHBlock.h"

#define MAX_CYCLE_NAME_LENGTH    64

class ZeroFile;
class CMSHBlockModel;

/*
class CMSHBlockKeyFrame
{
public:
	union {
		int key;
		int index;
	};
	int mark;
	ZeroVector3 position;
	ZeroQuaternion rotation;

	CMSHBlockKeyFrame()
	{
		key = 0;
		mark = 0;
	}
};
*/

class CMSHBlockPositionFrame
{
public:
	union {
		int key;
		int index;
	};
	int mark;
	ZeroVector3 position;

	CMSHBlockPositionFrame()
	{
		index = 0;
		mark = 0;
	}
};

class CMSHBlockRotationFrame
{
public:
	union {
		int key;
		int index;
	};
	int mark;
	ZeroQuaternion rotation;

	CMSHBlockRotationFrame()
	{
		index = 0;
		mark = 0;
	}
};

class CMSHBlockNodeAnimation//: //public ZeroSet<CMSHBlockKeyFrame>
{
	int JointsCount;
	CMSHBlockModel *model;
	unsigned m_ModelCRC;

	int mNumPositions;
	int mNumRotations;
	
public:
	ZeroSet<CMSHBlockPositionFrame> mPositions;
	ZeroSet<CMSHBlockRotationFrame> mRotations;

	CMSHBlockNodeAnimation(CMSHBlockModel *model, int jointsCount);
	~CMSHBlockNodeAnimation();
	
	void SetNumPositions (int count)
	{
		mNumPositions = count; 
		mPositions[count];
	}
	
	int GetNumPositions() const
	{
		return mNumPositions;
	}
	
	void SetNumRotations (int count)
	{
		mNumRotations = count; 
		mRotations[count];
	}
	
	int GetNumRotations() const
	{
		return mNumRotations;
	}
	

	CMSHBlockModel *GetModel() const
	{
		return model;
	}
	
	
	int IsIKKeyFrames () const
	{
		return JointsCount;
	}
	
	//	File read/write methods
	void WriteZeroFile(ZeroFile &file, CZeroData *data);
	void Read(FILE *file, CZeroData *pData);
	void ReadQuat(FILE *file, CZeroData *pData);
	void ReadSeparateKeys(FILE *file, CZeroData *pData);
	void Write(FILE *file);
	void WriteText(FILE *file);
	
	void CompressKeyFrames ();
	void StoreModelCRC();
	void RecoverModel(CZeroData *data);
};



class CMSHBlockAnimation: public CMSHBlock, public ZeroVisList<CMSHBlockNodeAnimation>
{
	public:
	class CycleDescriptor: public CMSHBlock 
	{
		int playStyle;
		int beginFrame;
		int endFrame;
		float frequency; 
		char cycleName[MAX_CYCLE_NAME_LENGTH];
		
		public:
		CycleDescriptor():CMSHBlock() {}
		CycleDescriptor (char *cycleName, float frequency, int beginFrame, int endFrame, int playStyle);
		void WriteZeroFile(ZeroFile &file, CZeroData *data);
		void Read(FILE *);
		void Write(FILE *);
		void WriteText(FILE *);
		void GetData(int *aStyle, int *aStart, int *aEnd, float *aFreq, char *name);
		void SetData(int aStyle, int aStart, int aEnd, float aFreq, char *name);

		void MarkStartAndEndKeyFrames (CMSHBlockNodeAnimation* node);

		
	};
	
	class AnimationCycles: public CMSHBlock, public ZeroVisList<CycleDescriptor>
	{
		public:
		AnimationCycles();
		void WriteZeroFile(ZeroFile &file, CZeroData *data);
		void Read(FILE *);
		void Write(FILE *);
		void WriteText(FILE *);

		void MarkStartAndEndKeyFrames (CMSHBlockNodeAnimation* node);
	};

	void MarkStartAndEndKeyFrames ();
	
	protected:
	AnimationCycles mshCycles;
	int mVersion;
		
		
	public:
//		CMSHBlockAnimation(BlockHeader *pHdr, CZeroData *pData);
		CMSHBlockAnimation();
		virtual ~CMSHBlockAnimation();
		
		void WriteZeroFile(ZeroFile &file, CZeroData *data);
		virtual void Read(FILE *f);
		virtual void Write(FILE *);
		virtual void WriteText(FILE *);
		AnimationCycles *GetAnimationCycles();


		CMSHBlockNodeAnimation* CreateKeyFrames (BlockHeader *header,CMSHBlockModel *model,int jointsCount);
		CMSHBlockNodeAnimation* GetNodeAnimation (const char *modelName);
		
		void CompressKeyFrames ();
		
		void AddCycle (char *cycleName, float frequency, int beginFrame, int endFrame, int playStyle);

		void StoreModelCRCs();
		void RecoverModels(CZeroData *data);

		int GetVersion(){return mVersion;};
		void SetVersion(int aVersion){mVersion=aVersion;};
		
};
#endif