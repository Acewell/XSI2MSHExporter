/*
	CMSHBlockSkeleton

  Base class for the following light classes:
	CMSHBlockSpotLight
	CMSHBlockInfiniteLight
	CMSHBlockPointLight
	
*/

#ifndef MSHBLOCK_SKELETON_H
#define MSHBLOCK_SKELETON_H

#include "zero.h"

class CMSHBlock;
class CMSHBlockModel;
class ZeroImportExportNode;
class ZeroFile;

class FloatByCRC: public ZeroTree<float, unsigned>
{
public:
	FloatByCRC ()
		:ZeroTree<float, unsigned>(ZeroAllocator::GetGenericAllocator())
	{
	}
};

/*
class CRCList: public ZeroVisList<unsigned>
{
	public:
	CRCList()
		:ZeroVisList<unsigned>()
   {
   }
};
*/
class CMSHBlockEndEffector
{
    friend class CMSHBlockSkeleton;
	
	ZeroVisList<unsigned> m_AnimCRCs;
	FloatByCRC m_AnimBlendFactors;

	unsigned m_ModelCRC;
    int boneCount;
    float bone1Length;
    float bone2Length;
    float constrainSign;
    CMSHBlockModel* model;

    public:
    CMSHBlockEndEffector();
    CMSHBlockEndEffector(ZeroImportExportNode *effector, int jointCount, float constrainSign);
    CMSHBlockEndEffector(ZeroImportExportNode *effector, CMSHBlockEndEffector *copyMe);


    CMSHBlockModel* GetModel()
    {
        return model;
    }


	void StoreModelCRC();
	void RecoverModel(CZeroData *data);

    void LeftToRight (const ZeroMatrix& scale)
    {
        _ASSERTE (0);
//        pos = scale.RotateVector (pos);
//        pos.z *= -1.0;
//        rot.x *= -1.0;
//        rot.y *= -1.0;
    }


	void SetBlendFactor(unsigned crc, float blend)
	{
		if(m_AnimBlendFactors.Find(crc))
		{
			m_AnimBlendFactors.Replace(blend,crc);
		}
		else
		{
			m_AnimCRCs.Append(crc);
			m_AnimBlendFactors.Insert(blend,crc);
		}
	}

	float GetBlendFactor(unsigned crc)
	{
		FloatByCRC::ZeroTreeNode *node=m_AnimBlendFactors.Find(crc);
		if(node)
		{
			return node->info;
		}
		else
		{
			return 0.f;
		}
	}

	int GetNumBlendFactors()
	{
		return m_AnimBlendFactors.GetCount();
	}

	void ReadBlendFactors(FILE *file);
    void WriteBlendFactors(FILE *file);
    void WriteZeroFileBlendFactors(ZeroFile &file);
    float GetConstrainSign() const
    {
        return constrainSign;
    }

    int GetBoneCount() const 
    {
        return boneCount;
    }

	float GetBone1Length(){return bone1Length;};
	float GetBone2Length(){return bone2Length;};

	void WriteZeroFile(ZeroFile &file, CZeroData *data);
	 void Read(FILE *file,  CZeroData *data);
    void Write(FILE *file);
    void WriteText(FILE *file);
	void AnimNameChanged(char *srcAnimName,char *dstAnimName);
};

class CMSHBlockSkeleton: public ZeroVisList<CMSHBlockEndEffector>
{
protected:
	//
	//	mSkelVersion==0 means that the crc refers to a chain end effector
	//	mSkelVersion==1 means that the crc refers to the last bone in the chain
	int mSkelVersion;

public:
	CMSHBlockSkeleton()
		:ZeroVisList<CMSHBlockEndEffector>()
   {
		mSkelVersion=1;
   }

	void WriteZeroFile(ZeroFile &file, CZeroData *data);
	void Read(FILE *, CZeroData *data, int aVersion);
	void ReadBlendFactors(FILE *, CZeroData *data, int aVersion);
	void Write(FILE *);
	void WriteText(FILE *);


   void CreateEffector (ZeroImportExportNode *node, int jointCount, float constrainSign) 
   {
       CMSHBlockEndEffector effector (node, jointCount, constrainSign);
       Append (effector);
   }

   void CopyEffector (ZeroImportExportNode *node, CMSHBlockEndEffector *eff) 
   {
       CMSHBlockEndEffector effector (node, eff);
       Append (effector);
   }


   CMSHBlockEndEffector* FindEffector (CMSHBlockModel* model) const
	{
		Iterator iter (*this);
		for (iter.Begin(); iter; iter ++) {
			if ((*iter).GetModel() == model) {
				return &(*iter);
			}
		}
		return NULL;
	}
    void WriteBlendFactors(FILE *file);
	void AnimNameChanged(char *srcAnimName,char *dstAnimName);

	void StoreEffectorModelCRCs();
	void RecoverEffectorModels(CZeroData *data);

	int GetVersion(){return mSkelVersion;};
	void SetVersion(int aVer){mSkelVersion=aVer;};
};
#endif