#include "ZeroData.h"
#include <string.h>

#include "MSHBlockModel.h"
#include "MSHBlockCamera.h"
#include "MSH_ClothConfig.h"
#include "MSHBlockFogData.h"
#include "MSHBlockSkeleton.h"
#include "MSHBlockFileInfo.h"
#include "MSHBlockAmbience.h"
#include "MSHBlockSceneInfo.h"
#include "MSHBlockSpotLight.h"
#include "MSHBlockPointLight.h"
#include "MSHBlockAnimations.h"
#include "MSHBlockMaterialSet.h"
#include "MSHBlockInfiniteLight.h"


CZeroData *gZeroData=NULL;

CZeroData::CZeroData()
    :skeleton()
{
	mHasShadowVolumes=false;
	m_pFileInfo=NULL;
	m_pSceneInfo=NULL;
	m_pAmbience=NULL;
	m_pFogData=NULL;
	m_pCamera=NULL;

	m_nPointLights=0;
	m_ppPointLights=NULL;
	
	m_nInfiniteLights=0;
	m_ppInfiniteLights=NULL;
	
	m_nSpotLights=0;
	m_ppSpotLights=NULL;
	
	m_pMaterialSet=NULL;

	m_nModels=0;
	m_ppModels=NULL;


	m_MSHVersion = 2;
	m_bExportFromModeler = false;
	strcpy(mExportFilename,"");

}

CZeroData::~CZeroData()
{

	if(m_pFileInfo)
		delete m_pFileInfo;
	if(m_pSceneInfo)
		delete m_pSceneInfo;
	if(m_pAmbience)
		delete m_pAmbience;
	if(m_pFogData)
		delete m_pFogData;
	if(m_pCamera)
		delete m_pCamera;

	if(m_ppPointLights)
	{
		for(int i=0;i<m_nPointLights;i++)
			delete m_ppPointLights[i];
		delete[] m_ppPointLights;
	}
	
	if(m_ppInfiniteLights)
	{
		for(int i=0;i<m_nInfiniteLights;i++)
			delete m_ppInfiniteLights[i];
		delete[] m_ppInfiniteLights;
	}
	
	if(m_ppSpotLights)
	{
		for(int i=0;i<m_nSpotLights;i++)
			delete m_ppSpotLights[i];
		delete[] m_ppSpotLights;
	}
	
	if(m_pMaterialSet)
		delete m_pMaterialSet;

	if(m_ppModels)
	{
		for(int i=0;i<m_nModels;i++)
			delete m_ppModels[i];
		delete[] m_ppModels;
	}
}

void CZeroData::ParseFileInfo(FILE *file, CMSHBlock::BlockHeader *pHdr)
{
	CreateNewFileInfo();
	m_pFileInfo ->SetHeader(pHdr);
	m_pFileInfo ->Read(file);
}

void CZeroData::ParseSceneInfo(FILE *file, CMSHBlock::BlockHeader *pHdr)
{
	CreateNewSceneInfo();
	m_pSceneInfo ->SetHeader(pHdr);
	m_pSceneInfo ->Read(file);
}

void CZeroData::ParseAmbience(FILE *file, CMSHBlock::BlockHeader *pHdr)
{
	CreateNewAmbience();
	m_pAmbience ->SetHeader(pHdr);
	m_pAmbience ->Read(file);
}

void CZeroData::ParseFog(FILE *file, CMSHBlock::BlockHeader *pHdr)
{
	CreateNewFog();
	m_pFogData ->SetHeader(pHdr);
	m_pFogData ->Read(file);
}

void CZeroData::ParseCamera(FILE *file, CMSHBlock::BlockHeader *pHdr)
{
	CreateNewCamera();
	m_pCamera ->SetHeader(pHdr);
	m_pCamera ->Read(file);
}

void CZeroData::ParsePointLight(FILE *file, CMSHBlock::BlockHeader *pHdr)
{
	CMSHBlockPointLight *pLight = AddPointLight();
	pLight ->SetHeader(pHdr);
	pLight ->Read(file);
}

void CZeroData::ParseInfiniteLight(FILE *file, CMSHBlock::BlockHeader *pHdr)
{
	CMSHBlockInfiniteLight *pLight = AddInfiniteLight();
	pLight ->SetHeader(pHdr);
	pLight ->Read(file);
}

void CZeroData::ParseSpotLight(FILE *file, CMSHBlock::BlockHeader *pHdr)
{
	CMSHBlockSpotLight *pLight = AddSpotLight();
	pLight ->SetHeader(pHdr);
	pLight ->Read(file);
}

void CZeroData::ParseMaterialSet(FILE *file, CMSHBlock::BlockHeader *pHdr)
{
	if(!m_pMaterialSet)
		m_pMaterialSet = new CMSHBlockMaterialSet(pHdr,this);
	else
		m_pMaterialSet ->SetHeader(pHdr);

	m_pMaterialSet ->Read(file);
}

void CZeroData::ParseMaterialList(FILE *file, CMSHBlock::BlockHeader *pHdr)
{
	CMSHBlock::BlockHeader tmpHdr;

	unsigned int nMaterials;
	if(m_pMaterialSet)
		delete m_pMaterialSet;

	m_pMaterialSet = new CMSHBlockMaterialSet(pHdr,this);

	CMSHBlock::ReadData(file,&nMaterials,sizeof(unsigned int));
	tmpHdr.type = pHdr ->type;
	tmpHdr.size = pHdr ->size - sizeof(unsigned int);

	m_pMaterialSet ->SetHeader(&tmpHdr);
	m_pMaterialSet ->Read(file);
}

void CZeroData::ParseModel(FILE *file, CMSHBlock::BlockHeader *pHdr)
{
	CMSHBlockModel *pModel = AddModel();
	pModel ->SetHeader(pHdr);
	pModel ->Read(file);
}



void CZeroData::ParseSkeleton(FILE *file, CMSHBlock::BlockHeader *pHdr, int aSkelVersion)
{
    skeleton.Read(file, this, aSkelVersion);
}


void CZeroData::ParseSkeletonBlendFactors(FILE *file, CMSHBlock::BlockHeader *pHdr, int aVersion)
{
    skeleton.ReadBlendFactors(file, this, aVersion);
}


void CZeroData::ParseAnimTriggerCollision(FILE *file, CMSHBlock::BlockHeader *pHdr)
{
	char *sModelName=NULL;
	char *sAnimName=NULL;
	MSH_AnimTriggerCollisionConfig animTriggerCollData;
	MSH_ClothCollisionSphere animTriggerCollSphereData;
	MSH_ClothCollisionCylinder animTriggerCollCylinderData;
	MSH_ClothCollisionCone animTriggerCollConeData;
	vect3 pos,front;

	pos.fX=pos.fY=pos.fZ=front.fX=front.fY=front.fZ=0.f;
	memset(&animTriggerCollData,0,sizeof(MSH_AnimTriggerCollisionConfig));

	unsigned int size=pHdr->size;
	// while there is size left...
	while (size)
	{
		// read a block header
		CMSHBlock::BlockHeader header;
		CMSHBlock::ReadHeader(file,&header);
		
		// deduct size
		assert(size >= sizeof(CMSHBlock::BlockHeader) + header.size);
		size -= sizeof(CMSHBlock::BlockHeader);
		size -= header.size;

		// parse the block
		switch (header.type)
		{
		    
			case 'ATAD':
				CMSHBlock::ReadData(file,&animTriggerCollData,header.size);
				break;
			case 'EMAN':
				CMSHBlock::ReadString(file,header.size,&sModelName);
				break;
			case 'MINA':
				CMSHBlock::ReadString(file,header.size,&sAnimName);
				break;
			case 'RHPS':
				CMSHBlock::ReadData(file,&animTriggerCollSphereData,sizeof(MSH_ClothCollisionSphere));
				break;
			case 'NLYC':
				CMSHBlock::ReadData(file,&animTriggerCollCylinderData,sizeof(MSH_ClothCollisionCylinder));
				break;
			case 'ENOC':
				CMSHBlock::ReadData(file,&animTriggerCollConeData,sizeof(MSH_ClothCollisionCone));
				break;
			case 'SROT':
				CMSHBlock::ReadData(file,&pos,sizeof(vect3));
				CMSHBlock::ReadData(file,&front,sizeof(vect3));
				break;
    		default:
    			CMSHBlock::SkipBlock(file, header.size);
	    		break;
		}
	}

	if(sModelName)
	{
		CMSHBlockModel *pModel=FindModelByName(sModelName);
		if(animTriggerCollData.config.bCollisionEnabled)
		{
			switch(animTriggerCollData.config.sType)
			{
			case COLLISION_SPHERE:
				if(sAnimName)
					pModel->SetAnimTriggerCollisionSphere(&animTriggerCollSphereData,sAnimName,animTriggerCollData.iLateStartFrame,animTriggerCollData.fTime,animTriggerCollData.iFrameMin,animTriggerCollData.iFrameMax,animTriggerCollData.fToleranceMin,animTriggerCollData.fToleranceMax,animTriggerCollData.fRecoveryTime,pos,front);
				break;
			case COLLISION_CONE:
				if(sAnimName)
					pModel->SetAnimTriggerCollisionCone(&animTriggerCollConeData,sAnimName,animTriggerCollData.iLateStartFrame,animTriggerCollData.fTime,animTriggerCollData.iFrameMin,animTriggerCollData.iFrameMax,animTriggerCollData.fToleranceMin,animTriggerCollData.fToleranceMax,animTriggerCollData.fRecoveryTime,pos,front);
				break;
			case COLLISION_CYLINDER:
				if(sAnimName)
					pModel->SetAnimTriggerCollisionCylinder(&animTriggerCollCylinderData,sAnimName,animTriggerCollData.iLateStartFrame,animTriggerCollData.fTime,animTriggerCollData.iFrameMin,animTriggerCollData.iFrameMax,animTriggerCollData.fToleranceMin,animTriggerCollData.fToleranceMax,animTriggerCollData.fRecoveryTime,pos,front);
				break;
			}
		}

		delete[] sModelName;
	}

	if(sAnimName)
		delete[] sAnimName;
}

void CZeroData::ParseAnimTriggerCollisions(FILE *file, CMSHBlock::BlockHeader *pHdr)
{
	unsigned int size=pHdr->size;
	// while there is size left...
	while (size)
	{
		// read a block header
		CMSHBlock::BlockHeader header;
		CMSHBlock::ReadHeader(file,&header);
		
		// deduct size
		assert(size >= sizeof(CMSHBlock::BlockHeader) + header.size);
		size -= sizeof(CMSHBlock::BlockHeader);
		size -= header.size;

		// parse the block
		switch (header.type)
		{
		    
            case '1LOC':
				ParseAnimTriggerCollision(file,&header);
    			break;
    		default:
    			CMSHBlock::SkipBlock(file, header.size);
	    		break;
		}
	}

}

void CZeroData::ParseAnimation(FILE *file, CMSHBlock::BlockHeader *pHdr, int aAnimVersion)
{
    CMSHBlockAnimation* animation;

    CreateNewAnimation(pHdr);
    animation = GetAnimation();
	animation->SetVersion(aAnimVersion);
    animation->Read(file);
}


void CZeroData::AddTextureMaterial(char *pTextureFilename,char **ppOutMaterialName)
{
	CMSHBlock::BlockHeader hdr;
	hdr.size=0;
	if(!m_pMaterialSet)
		m_pMaterialSet = new CMSHBlockMaterialSet(&hdr,this);
	m_pMaterialSet ->AddTextureMaterial(pTextureFilename,ppOutMaterialName);
}

void CZeroData::CreateNewFileInfo()
{
	CMSHBlock::BlockHeader hdr;
	hdr.size=0;
	if(m_pFileInfo)
		delete m_pFileInfo;
	m_pFileInfo = new CMSHBlockFileInfo(&hdr,this);
}

void CZeroData::CreateNewSceneInfo()
{
	CMSHBlock::BlockHeader hdr;
	hdr.size=0;
	if(m_pSceneInfo)
		delete m_pSceneInfo;
	m_pSceneInfo = new CMSHBlockSceneInfo(&hdr,this);
}

void CZeroData::CreateNewSkeleton()
{
}


void CZeroData::CreateNewAnimation(CMSHBlock::BlockHeader *phdr)
{
//    if(animation) {
//		delete animation;
///    }
//	 animation = new CMSHBlockAnimation(phdr, this);

	animation.SetData(this);
	animation.SetHeader (phdr);

	animation.RemoveAll();
}



void CZeroData::CreateNewAmbience()
{
	CMSHBlock::BlockHeader hdr;
	hdr.size=0;
	if(m_pAmbience)
		delete m_pAmbience;
	m_pAmbience = new CMSHBlockAmbience(&hdr,this);
}

void CZeroData::CreateNewFog()
{
	CMSHBlock::BlockHeader hdr;
	hdr.size=0;
	if(m_pFogData)
		delete m_pFogData;
	m_pFogData = new CMSHBlockFogData(&hdr,this);
}

void CZeroData::CreateNewCamera()
{
	CMSHBlock::BlockHeader hdr;
	hdr.size=0;
	if(m_pCamera)
		delete m_pCamera;
	m_pCamera = new CMSHBlockCamera(&hdr,this);
}

void CZeroData::CreateNewMaterialSet()
{
	CMSHBlock::BlockHeader hdr;
	hdr.size=0;
	if(m_pMaterialSet)
		delete m_pMaterialSet;
	m_pMaterialSet = new CMSHBlockMaterialSet(&hdr,this);
}

CMSHBlockPointLight *CZeroData::AddPointLight()
{
	CMSHBlock::BlockHeader hdr;
	hdr.size=0;
	CMSHBlockPointLight *pLight = new CMSHBlockPointLight(&hdr,this);
	CMSHBlockPointLight **pNewList;
	pNewList = new CMSHBlockPointLight *[m_nPointLights+1];
	if(m_ppPointLights)
	{
		memcpy(pNewList,m_ppPointLights, m_nPointLights*sizeof(CMSHBlockPointLight *));
		delete[] m_ppPointLights;
	}
	pNewList[m_nPointLights]=pLight;
	m_ppPointLights = pNewList;
	m_nPointLights++;
	return pLight;
}

CMSHBlockSpotLight *CZeroData::AddSpotLight()
{
	CMSHBlock::BlockHeader hdr;
	hdr.size=0;
	CMSHBlockSpotLight *pLight = new CMSHBlockSpotLight(&hdr,this);

	CMSHBlockSpotLight **pNewList;
	pNewList = new CMSHBlockSpotLight *[m_nSpotLights+1];
	if(m_ppSpotLights)
	{
		memcpy(pNewList,m_ppSpotLights, m_nSpotLights*sizeof(CMSHBlockSpotLight *));
		delete[] m_ppSpotLights;
	}
	pNewList[m_nSpotLights]=pLight;
	m_ppSpotLights = pNewList;
	m_nSpotLights++;

	return pLight;
}

CMSHBlockInfiniteLight *CZeroData::AddInfiniteLight()
{
	CMSHBlock::BlockHeader hdr;
	hdr.size=0;
	CMSHBlockInfiniteLight *pLight = new CMSHBlockInfiniteLight(&hdr,this);

	CMSHBlockInfiniteLight **pNewList;
	pNewList = new CMSHBlockInfiniteLight *[m_nInfiniteLights+1];
	if(m_ppInfiniteLights)
	{
		memcpy(pNewList,m_ppInfiniteLights, m_nInfiniteLights*sizeof(CMSHBlockInfiniteLight *));
		delete[] m_ppInfiniteLights;
	}
	pNewList[m_nInfiniteLights]=pLight;
	m_ppInfiniteLights = pNewList;
	m_nInfiniteLights++;

	return pLight;
}

CMSHBlockModel *CZeroData::AddModel()
{
	CMSHBlock::BlockHeader hdr;
	hdr.size=0;
	CMSHBlockModel *pModel = new CMSHBlockModel(&hdr, this);

	CMSHBlockModel **pNewList;
	pNewList = new CMSHBlockModel *[m_nModels+1];
	if(m_ppModels)
	{
		memcpy(pNewList,m_ppModels, m_nModels*sizeof(CMSHBlockModel *));
		delete[] m_ppModels;
	}
	pNewList[m_nModels]=pModel;
	m_ppModels = pNewList;
	m_nModels++;
	return pModel;
}

void CZeroData::RemoveAllModels()
{
	if(m_ppModels)
	{
		for(int i=0;i<m_nModels;i++)
			delete m_ppModels[i];
		delete[] m_ppModels;
		m_ppModels = NULL;
		m_nModels = 0;
	}
}

void CZeroData::RemoveOrphans()
{
	if(!m_ppModels)
		return;

	int nChildren=0;
	ZeroStack<CMSHBlockModel *> children(m_nModels);
	memset(&children[0],0,m_nModels * sizeof(CMSHBlockModel *));

	int i,j;
	for(i=0;i<m_nModels;i++)
	{
		if(m_ppModels[i]->GetParentName())
		{
			CMSHBlockModel *mshparent=FindModelByName(m_ppModels[i]->GetParentName());
			if(!mshparent)
			{
				children[nChildren++]=m_ppModels[i];
			}
			else
			{
				for(j=0;j<nChildren;j++)
				{
					if(children[j]==mshparent)
					{
						children[nChildren++]=m_ppModels[i];
						break;
					}
				}
			}
		}
	}

	for(i=0;i<nChildren;i++)
		RemoveModel(children[i]);
}

void CZeroData::RemoveModel(CMSHBlockModel *aModel)
{
	if(!m_ppModels)
		return;
	bool bFound=false;
	int idx=0;
	int i;
	for(i=0;i<m_nModels;i++)
	{
		if(aModel==m_ppModels[i])
		{
			idx=i;
			bFound=true;
		}
	}

	if(!bFound)
		return;

	CMSHBlockModel **pNewList;
	pNewList = new CMSHBlockModel *[m_nModels-1];
	for(i=0;i<idx;i++)
	{
		pNewList[i]=m_ppModels[i];
	}
	delete[] m_ppModels[idx];
	for(i=idx+1;i<m_nModels;i++)
	{
		pNewList[i-1]=m_ppModels[i];
	}
	delete[] m_ppModels;
	m_ppModels = pNewList;
	m_nModels--;
}

void CZeroData::SetMSHVersion(unsigned int ver)
{
	m_MSHVersion = ver;
}

unsigned int CZeroData::GetMSHVersion()
{
	return m_MSHVersion;
}

CMSHBlockFileInfo *CZeroData::GetFileInfo()
{
	return m_pFileInfo;
}

CMSHBlockSceneInfo *CZeroData::GetSceneInfo()
{
	return m_pSceneInfo;
}

CMSHBlockAmbience *CZeroData::GetAmbience()
{
	return m_pAmbience;
}

CMSHBlockFogData *CZeroData::GetFogData()
{
	return m_pFogData;
}

CMSHBlockCamera *CZeroData::GetCamera()
{
	return m_pCamera;
}

CMSHBlockPointLight **CZeroData::GetPointLights(int &nLights)
{
	nLights=m_nPointLights;
	return m_ppPointLights;
}

CMSHBlockInfiniteLight **CZeroData::GetInfiniteLights(int &nLights)
{
	nLights=m_nInfiniteLights;
	return m_ppInfiniteLights;
}

CMSHBlockSpotLight **CZeroData::GetSpotLights(int &nLights)
{
	nLights=m_nSpotLights;
	return m_ppSpotLights;
}

CMSHBlockMaterialSet *CZeroData::GetMaterialSet()
{
	return m_pMaterialSet;
}

CMSHBlockModel **CZeroData::GetModels(int &nModels)
{
	nModels=m_nModels;return m_ppModels;
}

CMSHBlockSkeleton *CZeroData::GetSkeleton()
{
    return &skeleton;
}

CMSHBlockAnimation *CZeroData::GetAnimation()
{
    return &animation;
}

MshPlatform CZeroData::GetPlatform()
{
	return m_platform;
}
void CZeroData::SetPlatform(MshPlatform platform)
{
	m_platform = platform;
}

const char *CZeroData::GetExportFilename()
{
	return mExportFilename;
}

void CZeroData::SetExportFilename(const char *aName)
{
	strcpy(mExportFilename,aName);
}

CMSHBlockModel *CZeroData::FindModelByName(const char* name)
{
    int i;
	CMSHBlockModel *model;

    for (i = 0; i < m_nModels; i ++) {
        model = m_ppModels[i];
        if (!stricmp (name, model->GetName())) {
            return model;
        }
    }

    return NULL;
}


void CZeroData::RemoveUnusedMaterials()
{
	if(m_pMaterialSet)
	{
		unsigned int i,nMaterials;
		CMSHBlockMaterial **ppMaterials = m_pMaterialSet->GetMaterials(nMaterials);
		for(i=0;i<nMaterials;i++)
		{
			ppMaterials[i]->bMarked=true;
		}

		int k;
		if(m_ppModels)
		{
			for(k=0;k<m_nModels;k++)
			{
				int n,nSegments;
				CMSHBlockModelSegment **ppSegments;
				ppSegments=m_ppModels[k]->GetSegments(nSegments);
				for(n=0;n<nSegments;n++)
				{
					CMSHBlockMaterial *pMaterial=NULL;
					if(ppSegments[n]->GetMaterialName())
							pMaterial = m_pMaterialSet->GetMaterialByName(ppSegments[n]->GetMaterialName());
					
					if(pMaterial)
					{
						pMaterial->bMarked=false;
					}
				}
			}
		}
		m_pMaterialSet->RemoveMarkedMaterials();
	}
}

void CZeroData::DebugOutput(const char* name)
{
	FILE *fDebugFile;
	char sfile[1024];

	strcpy (sfile, name);
	strtok (sfile, ".");
	strcat (sfile, ".txt");

	fDebugFile = fopen(sfile,"wt");

	if(m_pSceneInfo)
		m_pSceneInfo->WriteText(fDebugFile);

	int i;
	if(m_pMaterialSet)
		m_pMaterialSet ->WriteText(fDebugFile);

	if(m_nPointLights)
		for(i=0;i<m_nPointLights;i++)
			m_ppPointLights[i] ->WriteText(fDebugFile);

	if(m_nInfiniteLights)
		for(i=0;i<m_nInfiniteLights;i++)
			m_ppInfiniteLights[i] ->WriteText(fDebugFile);

	if(m_nSpotLights)
		for(i=0;i<m_nSpotLights;i++)
			m_ppSpotLights[i] ->WriteText(fDebugFile);

	if(m_nModels)
		for(i=0;i<m_nModels;i++)
			m_ppModels[i] ->WriteText(fDebugFile);

    // export skeleton 
    if (GetSkeleton()->GetCount()) {
        GetSkeleton()->WriteText(fDebugFile);

        // if it has skeleton, export animations
        if (GetAnimation()) {
            GetAnimation()->WriteText(fDebugFile);
        }
    }
	
	fclose(fDebugFile);
}



void CZeroData::PackModelArray (CMSHBlockModel* array[], int count)
{
	int i;
	_ASSERTE (count <= m_nModels);

	for (i= 0; i < count; i ++) {
		m_ppModels[i] = array[i];
	}

	m_nModels = count;


}
