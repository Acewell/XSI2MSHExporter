#include "ZeroExporter.h"
#include "ZeroData.h"
#include "MSHBlockFileInfo.h"
#include "MSHBlockSceneInfo.h"
#include "MSHBlockAmbience.h"
#include "MSHBlockFogData.h"
#include "MSHBlockCamera.h"
#include "MSHBlockPointLight.h"
#include "MSHBlockInfiniteLight.h"
#include "MSHBlockSpotLight.h"
#include "MSHBlockMaterialSet.h"
#include "MSHBlockModel.h"
#include "MSHBlockSkeleton.h"
#include "MSHBlockAnimations.h"

CZeroExporter::CZeroExporter()
{
}

CZeroExporter::~CZeroExporter()
{
}

void CZeroExporter::ExportFile(FILE *fp, CZeroData *pData)
{
	int i;

	pData->RemoveUnusedMaterials();
	CMSHBlock::StartBlock(fp, 'HEDR');

	int hasShadowVolumes=pData->HasShadowVolumes()?1:0;
	if(hasShadowVolumes)
	{
		CMSHBlock::StartBlock(fp, 'SHVO');
		fwrite(&hasShadowVolumes,sizeof(int),1,fp);
		CMSHBlock::EndBlock(fp);
	}


	CMSHBlock::StartBlock(fp,'MSH2');
	pData ->SetMSHVersion(2);
	// export file info
	if(pData ->GetFileInfo())
		pData ->GetFileInfo()->Write(fp);

	// export scene info
	if(pData ->GetSceneInfo())
		pData ->GetSceneInfo()->Write(fp);
		
	// export ambience information
	if(pData ->GetAmbience())
		pData ->GetAmbience()->Write(fp);
		
	// export fog information
	if(pData ->GetFogData())
		pData ->GetFogData()->Write(fp);

	// export camera information
	if(pData ->GetCamera())
		pData ->GetCamera()->Write(fp);
			
	// export lights information
	CMSHBlockPointLight **ppPointLights;
	CMSHBlockInfiniteLight **ppInfiniteLights;
	CMSHBlockSpotLight **ppSpotLights;
	int nLights=0;
	ppPointLights = pData ->GetPointLights(nLights);
	if(nLights)
		for(i=0;i<nLights;i++)
			ppPointLights[i] ->Write(fp);
	
	nLights=0;
	ppInfiniteLights = pData ->GetInfiniteLights(nLights);
	if(nLights)
		for(i=0;i<nLights;i++)
			ppInfiniteLights[i] ->Write(fp);

	nLights=0;
	ppSpotLights = pData ->GetSpotLights(nLights);
	if(nLights)
		for(i=0;i<nLights;i++)
			ppSpotLights[i] ->Write(fp);

	
	// export material information
	if(pData ->GetMaterialSet())
		pData ->GetMaterialSet() ->Write(fp);

	// export models
	CMSHBlockModel **ppModels;
	int nModels;
	ppModels = pData ->GetModels(nModels);
	for(i=0;i<nModels;i++)
		ppModels[i] ->Write(fp);

	CMSHBlock::EndBlock(fp);


    // export skeleton 
    if (pData->GetSkeleton()->GetCount()) {
        pData->GetSkeleton()->Write(fp);

        // if it has skeleton, export animations
        if (pData->GetAnimation()) {
            pData->GetAnimation()->Write(fp);
        }
    }

	// export animation trigger collision volumes
	 
	CMSHBlock::StartBlock(fp, 'CL1L');
	for(i=0;i<nModels;i++)
		ppModels[i] ->WriteAnimTriggerCollisionObject(fp);
	CMSHBlock::EndBlock(fp);


	CMSHBlock::EndBlock(fp);

}

