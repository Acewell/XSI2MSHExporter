#ifndef ZERODATA_H
#define ZERODATA_H

#include "zero.h"
#include "MSHBlock.h"
#include "MSHBlockSkeleton.h"
#include "MSHBlockAnimations.h"

class CMSHBlockModel;
class CMSHBlockCamera;
class CMSHBlockFogData;
class CMSHBlockFileInfo;
class CMSHBlockAmbience;
class CMSHBlockSpotLight;
class CMSHBlockSceneInfo;
class CMSHBlockPointLight;
class CMSHBlockMaterialSet;
class CMSHBlockInfiniteLight;

// Textures


enum MshTextureFormat
{
    TEXFORMAT_PIC = 0,
    TEXFORMAT_TGA = 1,
    TEX_FORCE32 = 0x0fffffff
};


enum MshPlatform
{
    PLATFORM_PC = 0,
    PLATFORM_XBOX___ = 1,
    PLATFORM_PLAY_STATION2___ = 2,
    PLATFORM_Pathform = 0x0fffffff
};

class CZeroData
{
    protected:
	bool mHasShadowVolumes;
	char mExportFilename[MAX_PATH];

	bool m_bExportFromModeler;
	CMSHBlockFileInfo *m_pFileInfo;
	CMSHBlockSceneInfo *m_pSceneInfo;
	CMSHBlockAmbience *m_pAmbience;
	CMSHBlockFogData *m_pFogData;
	CMSHBlockCamera *m_pCamera;
	MshPlatform m_platform;

	//	array of point lights
	int m_nPointLights;
	CMSHBlockPointLight **m_ppPointLights;
	
	//	array of infinite lights
	int m_nInfiniteLights;
	CMSHBlockInfiniteLight **m_ppInfiniteLights;
	
	//	array of spot lights
	int m_nSpotLights;
	CMSHBlockSpotLight **m_ppSpotLights;
	
	CMSHBlockMaterialSet *m_pMaterialSet;

	//	array of models
	int m_nModels;
	CMSHBlockModel **m_ppModels;

	unsigned int m_MSHVersion;

    CMSHBlockSkeleton skeleton;
    CMSHBlockAnimation animation;

public:
	CZeroData();
	virtual ~CZeroData();

	//--------------------
	//	File read/write methods
	void ParseFileInfo(FILE *file, CMSHBlock::BlockHeader *pHdr);
	void ParseSceneInfo(FILE *file, CMSHBlock::BlockHeader *pHdr);
	void ParseAmbience(FILE *file, CMSHBlock::BlockHeader *pHdr);
	void ParseFog(FILE *file, CMSHBlock::BlockHeader *pHdr);
	void ParseCamera(FILE *file, CMSHBlock::BlockHeader *pHdr);
	void ParsePointLight(FILE *file, CMSHBlock::BlockHeader *pHdr);
	void ParseInfiniteLight(FILE *file, CMSHBlock::BlockHeader *pHdr);
	void ParseSpotLight(FILE *file, CMSHBlock::BlockHeader *pHdr);
	void ParseMaterialSet(FILE *file, CMSHBlock::BlockHeader *pHdr);
	void ParseMaterialList(FILE *file, CMSHBlock::BlockHeader *pHdr);
	void ParseModel(FILE *file, CMSHBlock::BlockHeader *pHdr);

	void ParseAnimTriggerCollision(FILE *file, CMSHBlock::BlockHeader *pHdr);
	void ParseAnimTriggerCollisions(FILE *file, CMSHBlock::BlockHeader *pHdr);

	void ParseSkeletonBlendFactors(FILE *file, CMSHBlock::BlockHeader *pHdr, int aVersion);
    void ParseSkeleton(FILE *file, CMSHBlock::BlockHeader *pHdr, int aSkelVersion);
    void ParseAnimation(FILE *file, CMSHBlock::BlockHeader *pHdr, int aAnimVersion);

	void RemoveUnusedMaterials();

	//--------------------
	//	Set methods
	void CreateNewFileInfo();
	void CreateNewSceneInfo();
	void CreateNewAmbience();
	void CreateNewFog();
	void CreateNewCamera();
	void CreateNewMaterialSet();

	void CreateNewSkeleton();
	void CreateNewAnimation(CMSHBlock::BlockHeader *phdr);

	CMSHBlockPointLight *AddPointLight();
	CMSHBlockSpotLight *AddSpotLight();
	CMSHBlockInfiniteLight *AddInfiniteLight();
	CMSHBlockModel *AddModel();
	void AddTextureMaterial(char *pTextureFilename,char **ppOutMaterialName);
	void SetMSHVersion(unsigned int ver);

	//--------------------
	//	Get methods
	CMSHBlockFileInfo *GetFileInfo();
	CMSHBlockSceneInfo *GetSceneInfo();
	CMSHBlockAmbience *GetAmbience();
	CMSHBlockFogData *GetFogData();
	CMSHBlockCamera *GetCamera();
	CMSHBlockPointLight **GetPointLights(int &nLights);
	CMSHBlockInfiniteLight **GetInfiniteLights(int &nLights);
	CMSHBlockSpotLight **GetSpotLights(int &nLights);
	CMSHBlockMaterialSet *GetMaterialSet();
	CMSHBlockModel **GetModels(int &nModels);
    CMSHBlockSkeleton *GetSkeleton();
    CMSHBlockAnimation *GetAnimation();
	bool HasShadowVolumes(){return mHasShadowVolumes;};
	void SetHasShadowVolumes(bool aHasShadowVolumes){mHasShadowVolumes=aHasShadowVolumes;};
	MshPlatform GetPlatform();
	void SetPlatform(MshPlatform platform);
	void RemoveOrphans();
	void RemoveModel(CMSHBlockModel *aModel);
	void RemoveAllModels();


	void PackModelArray (CMSHBlockModel* array[], int count);


	CMSHBlockModel *FindModelByName(const char* name);



	unsigned int GetMSHVersion();

	void DebugOutput(const char *name);
	void SetExportFromModeler(bool b){m_bExportFromModeler=b;};
	bool GetExportFromModeler(){return m_bExportFromModeler;};
	void SetExportFilename(const char *aName);
	const char *GetExportFilename();
};

extern CZeroData *gZeroData;
#endif