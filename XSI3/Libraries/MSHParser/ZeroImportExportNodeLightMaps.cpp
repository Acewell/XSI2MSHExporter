#include "Zero.h"
/*
#include "ZeroImportExportNode.h"

#include "ZeroData.h"
#include "MSHBlockModel.h"
#include "MSHBlockCamera.h"
#include "MSHBlockFogData.h"
#include "MSHBlockFileInfo.h"
#include "MSHBlockAmbience.h"
#include "MSHBlockSpotLight.h"
#include "MSHBlockSceneInfo.h"
#include "MSHBlockPointLight.h"
#include "MSHBlockAnimations.h"
#include "MSHBlockMaterialSet.h"
#include "MSHBlockConstraintList.h"
#include "MSHBlockInfiniteLight.h"





void ZeroImportExportNode::ApplyLights (CZeroData* scene, LIGHTING_MODE mode)
{

	CMSHLightsList lights (scene);

	if (!lights.GetCount()) {
		return;
	}


	switch (mode) {
		// apply ligght model to vertex
		case FAST_LIGHT:
		{
			ApplyFastLightsToAllModels (lights);
			break;
		}

		// calculate vertex color by ray casting to a collision geometry
		case FULL_LIGHT:			  
		{
			ApplyFullLights (lights);
			break;
		}

		// calculate light maps textures ray casting to a collision geometry
		case FULL_LIGHT_MAPS:	   
		{
			ApplyLightsMaps (lights);
			break;
		}
	}
}


void ZeroImportExportNode::ApplyLightsMaps (const CMSHLightsList& list)
{
	_ASSERTE (0);
}

void ZeroImportExportNode::ApplyFullLights (const CMSHLightsList& list)
{
	_ASSERTE (0);
}

void ZeroImportExportNode::ApplyFastLightsToAllModels (const CMSHLightsList& lights)
{
	ZeroImportExportNode	*node;

	// get the working bsp
	for (node = GetFirst(); node; node = node->GetNext()) {
		if (node->GetMshModel()) {
		 node->ApplyFastLightsToModel (lights);
		}
	}

}


void ZeroImportExportNode::ApplyFastLightsToModel (const CMSHLightsList& lights)
{
	int i;
	int segmentsCount;
	CMSHBlockModel *model;
	CMSHBlockModelSegment **segments;


	ZeroMatrix matrix (GetWorldMatrix(NULL));

	model = GetMshModel();
	segments = model->GetSegments(segmentsCount);

	for (i = 0; i < segmentsCount; i ++) {
		segments[i]->CalculateVertexColors (matrix, lights);
	}




}
*/