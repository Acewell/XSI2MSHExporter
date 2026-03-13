/*
	CMSHBlockSkeleton

  Base class for the following light classes:
	CMSHBlockSpotLight
	CMSHBlockInfiniteLight
	CMSHBlockPointLight
	
*/

#include "zero.h"
#include "ZeroData.h"
#include "MSHBlockModel.h"
#include "MSHBlockSkeleton.h"
#include "ZeroImportExportNode.h"

CMSHBlockEndEffector::CMSHBlockEndEffector()
{
   boneCount = 0;
   bone1Length = 0;
   bone2Length = 0;
   constrainSign = 0;
   model = NULL;
}


CMSHBlockEndEffector::CMSHBlockEndEffector(ZeroImportExportNode *effector, CMSHBlockEndEffector *copyMe)
{
	model = effector->GetMshModel();
	constrainSign=copyMe->GetConstrainSign();
	boneCount=copyMe->GetBoneCount();
    bone1Length = copyMe->GetBone1Length();
    bone2Length = copyMe->GetBone2Length();
}

CMSHBlockEndEffector::CMSHBlockEndEffector(
    ZeroImportExportNode *effector, 
    int jointCount,
    float constrainSignArg) 
{
    float fX;
    float fY;
    float fZ;

    bone1Length = 0;
    bone2Length = 0;
    boneCount = jointCount;

    constrainSign = constrainSignArg;
	model = effector->GetMshModel();
    switch (boneCount) {
        case 0:
            break;

        case 1:
			model = effector->GetParent()->GetMshModel();
            effector->GetMshModel()->GetTrans(fX, fY, fZ);
            bone1Length = fX;
            break;

        case 2:
		    model = effector->GetParent()->GetMshModel();
            effector->GetMshModel()->GetTrans(fX, fY, fZ);
            bone2Length = fX;

            effector = effector->GetParent(); 
            effector->GetMshModel()->GetTrans(fX, fY, fZ);
            bone1Length = fX;
            break;

        default:;
    }
}


void CMSHBlockEndEffector::AnimNameChanged(char *srcAnimName,char *dstAnimName)
{
    unsigned srcCRC,dstCRC;

    srcCRC = CalcStrCRC (srcAnimName);
    dstCRC = CalcStrCRC (dstAnimName);
	if(srcCRC==dstCRC)
		return;
	FloatByCRC::ZeroTreeNode *node=m_AnimBlendFactors.Find(srcCRC);
	if(node)
	{
		SetBlendFactor(dstCRC,node->info);
		m_AnimBlendFactors.Remove(node);
		m_AnimCRCs.Remove(srcCRC);
	}
}

void CMSHBlockEndEffector::WriteText(FILE *file)
{
//    ZeroMatrix mat (rot, pos);

    fprintf(file,"\t\tnameCRC: %s\n", model->GetName());
    fprintf(file,"\t\ttype:  %d\n", boneCount);
    fprintf(file,"\t\tconstrain:  %f\n", constrainSign);
    fprintf(file,"\t\tbone1Lenght:  %f\n", bone1Length);
    fprintf(file,"\t\tbone2Lenght:  %f\n", bone2Length);
    fprintf(file,"\t\tblendFactors:\n");

	ZeroVisList<unsigned>::ZeroListNode *node=m_AnimCRCs.GetFirst();
	while(node)
	{
		FloatByCRC::ZeroTreeNode *blendNode=m_AnimBlendFactors.Find(node->info);
		if(blendNode)
		{
			const char *name=InvertCRC(node->info);
			if(name)
			{
			    fprintf(file,"\t\t\t%s\t%0.3f\n", name, blendNode->info);
			}
			else
			{
			    fprintf(file,"\t\t\t%u\t%0.3f\n", node->info, blendNode->info);
			}
		}
		node=node->GetNext();
	}
    fprintf(file,"\n");
}

void CMSHBlockEndEffector::WriteZeroFile(ZeroFile &file,CZeroData *pData)
{
    unsigned effCRC;

    effCRC = CalcStrCRC (model->GetName());

    file.Write(&effCRC, sizeof (int), 1);
    file.Write(&boneCount, sizeof (int), 1);
    file.Write(&constrainSign, sizeof (int), 1);
    file.Write(&bone1Length, sizeof (float), 1);
    file.Write(&bone2Length, sizeof (float), 1);

//    fwrite (&pos, sizeof (ZeroVector3), 1, file);
//    fwrite (&rot, sizeof (ZeroQuaternion), 1, file);

}


void CMSHBlockEndEffector::WriteZeroFileBlendFactors(ZeroFile &file)
{
    unsigned effCRC;
	float blendZero=0.f;

    effCRC = CalcStrCRC (model->GetName());
	unsigned count=m_AnimCRCs.GetCount();
    file.Write(&effCRC, sizeof (unsigned), 1);

    file.Write(&count, sizeof (unsigned), 1);
	ZeroVisList<unsigned>::ZeroListNode *node=m_AnimCRCs.GetFirst();
	while(node)
	{
		FloatByCRC::ZeroTreeNode *blendNode=m_AnimBlendFactors.Find(node->info);
		file.Write(&(node->info), sizeof (unsigned), 1);
		if(blendNode)
			file.Write(&(blendNode->info), sizeof (float), 1);
		else
			file.Write(&blendZero, sizeof (float), 1);
		node=node->GetNext();
	}
}

void CMSHBlockEndEffector::WriteBlendFactors(FILE *file)
{
    unsigned effCRC;
	float blendZero=0.f;

    effCRC = CalcStrCRC (model->GetName());
	unsigned count=m_AnimCRCs.GetCount();
    fwrite(&effCRC, sizeof (unsigned), 1, file);

    fwrite(&count, sizeof (unsigned), 1, file);
	ZeroVisList<unsigned>::ZeroListNode *node=m_AnimCRCs.GetFirst();
	while(node)
	{
		FloatByCRC::ZeroTreeNode *blendNode=m_AnimBlendFactors.Find(node->info);
		fwrite(&(node->info), sizeof (unsigned), 1, file);
		if(blendNode)
			fwrite(&(blendNode->info), sizeof (float), 1, file);
		else
			fwrite(&blendZero, sizeof (float), 1, file);
		node=node->GetNext();
	}
}

void CMSHBlockEndEffector::ReadBlendFactors(FILE *file)
{
	unsigned i,count;
    fread(&count, sizeof (unsigned), 1, file);
	for(i=0;i<count;i++)
	{
		unsigned crc;
		float blend;
		fread(&crc, sizeof (unsigned), 1, file);
		fread(&blend, sizeof (float), 1, file);
		SetBlendFactor(crc,blend);
	}
}

void CMSHBlockEndEffector::Write(FILE *file)
{
    unsigned effCRC;

    effCRC = CalcStrCRC (model->GetName());

    fwrite(&effCRC, sizeof (int), 1, file);
    fwrite(&boneCount, sizeof (int), 1, file);
    fwrite(&constrainSign, sizeof (int), 1, file);
    fwrite(&bone1Length, sizeof (float), 1, file);
    fwrite(&bone2Length, sizeof (float), 1, file);

//    fwrite (&pos, sizeof (ZeroVector3), 1, file);
//    fwrite (&rot, sizeof (ZeroQuaternion), 1, file);

}


void CMSHBlockEndEffector::Read(FILE *file, CZeroData *data)
{
    unsigned effCRC;
    const char *name;

	CMSHBlockSkeleton *pSkel=data->GetSkeleton();
	int skelVer=pSkel->GetVersion();

    fread(&effCRC, sizeof (int), 1, file);
    fread(&boneCount, sizeof (int), 1, file);
    fread(&constrainSign, sizeof (int), 1, file);
    fread(&bone1Length, sizeof (float), 1, file);
    fread(&bone2Length, sizeof (float), 1, file);

    name = InvertCRC(effCRC);
	model = data->FindModelByName(name);
    _ASSERTE (model);
	if(skelVer==0 && boneCount>0) // don't want to reference end effector anymore
	{
		model=data->FindModelByName(model->GetParentName());
	}
    _ASSERTE (model);

//  fread (&pos, sizeof (ZeroVector3), 1, file);
//  fread (&rot, sizeof (ZeroQuaternion), 1, file);
}



void CMSHBlockEndEffector::StoreModelCRC()
{
	m_ModelCRC=CalcStrCRC(model->GetName());
}

void CMSHBlockEndEffector::RecoverModel(CZeroData *data)
{
    model = data->FindModelByName(InvertCRC(m_ModelCRC));
}

void CMSHBlockSkeleton::WriteText(FILE *file)
{

	fprintf(file,"\nSkeleton\n");
	fprintf(file,"{\n");

    fprintf(file, "\tchainsCount %d\n", GetCount());
	fprintf(file,"\t{\n");
    Iterator iter (*this);
    for (iter.Begin(); iter; iter ++) {
        (*iter).WriteText(file);
    }
	fprintf(file,"\t}\n");
	fprintf(file,"}\n");
    fflush(file);
}



void CMSHBlockSkeleton::WriteZeroFile(ZeroFile &file,CZeroData *pData)
{
    int count;

	//	write out skeleton
    CMSHBlock::StartZeroFileBlock(file, 'SKL2');

    count = GetCount();
    file.Write(&count, sizeof (int), 1); 

    Iterator iter (*this);
    for (iter.Begin(); iter; iter ++) {
        (*iter).WriteZeroFile(file,pData);
    }

	CMSHBlock::EndZeroFileBlock(file);

	CMSHBlock::StartZeroFileBlock(file,'BLN2');
    file.Write(&count, sizeof (int), 1); 
    for (iter.Begin(); iter; iter ++) {
        (*iter).WriteZeroFileBlendFactors(file);
    }
	CMSHBlock::EndZeroFileBlock(file);
}



void CMSHBlockSkeleton::Write(FILE *file)
{
    int count;

    CMSHBlock::StartBlock(file, 'SKL2');

    count = GetCount();
    fwrite (&count, sizeof (int), 1, file); 

    Iterator iter (*this);
    for (iter.Begin(); iter; iter ++) {
        (*iter).Write(file);
    }

	CMSHBlock::EndBlock(file);

	CMSHBlock::StartBlock(file,'BLN2');
    fwrite (&count, sizeof (int), 1, file); 
    for (iter.Begin(); iter; iter ++) {
        (*iter).WriteBlendFactors(file);
    }
	CMSHBlock::EndBlock(file);
}



void CMSHBlockSkeleton::Read(FILE *file, CZeroData *data, int aVersion)
{
    int i;
    int effectorsCount;

	mSkelVersion=aVersion;
    fread (&effectorsCount, sizeof (int), 1, file);
    for (i = 0 ; i < effectorsCount; i ++) {
        CMSHBlockEndEffector effector;
        effector.Read(file, data);
        Append (effector);
    }
	mSkelVersion=1;
}

void CMSHBlockSkeleton::ReadBlendFactors(FILE *file, CZeroData *data, int aVersion)
{
    int i;
    int effectorsCount;

    fread (&effectorsCount, sizeof (int), 1, file);
    for (i = 0 ; i < effectorsCount; i ++) 
	{
		unsigned effectorCRC;
	    fread (&effectorCRC, sizeof (unsigned), 1, file);
		const char *name = InvertCRC(effectorCRC);
		CMSHBlockModel *tmpModel = data->FindModelByName(name);
		_ASSERTE(tmpModel);

        CMSHBlockEndEffector *pEffector=FindEffector(tmpModel);
		
		//	old version (crc still refers to end effectors, not last bone)
		if(aVersion==0 && !pEffector)
		{
			tmpModel=data->FindModelByName(tmpModel->GetParentName());
			pEffector=FindEffector(tmpModel);
		}
		_ASSERTE(pEffector);

		pEffector->ReadBlendFactors(file);
    }
}

void CMSHBlockSkeleton::AnimNameChanged(char *srcAnimName,char *dstAnimName)
{
    Iterator iter (*this);
    for (iter.Begin(); iter; iter ++) {
        (*iter).AnimNameChanged(srcAnimName,dstAnimName);
    }

}

void CMSHBlockSkeleton::StoreEffectorModelCRCs()
{
    Iterator iter (*this);
    for (iter.Begin(); iter; iter ++) 
	{
        (*iter).StoreModelCRC();
    }
}

void CMSHBlockSkeleton::RecoverEffectorModels(CZeroData *data)
{
    Iterator iter (*this);
    for (iter.Begin(); iter; iter ++) 
	{
        (*iter).RecoverModel(data);
    }
}


