#ifndef MSHBLOCK_MATERIALSET_H
#define MSHBLOCK_MATERIALSET_H

#include "ApplicationSpecific.h"
#include "MSHBlock.h"

/*
	CMSHBlockMaterial
	
	Class for .msh file data blocks with 'MATD' header
*/

class CMSHBlockMaterial: public CMSHBlock
{

protected:
	char *m_sPrefix;
	char *m_sMaterialName;

	//	Diffuse color
	float m_fR, m_fG, m_fB, m_fA;

	//	Specular color
	float m_fSpecularR, m_fSpecularG, m_fSpecularB, m_fSpecularA;
	float m_fPower;

	//	Ambient color
	float m_fAmbientR, m_fAmbientG, m_fAmbientB, m_fAmbientA;

	//	Shading model.  Values correspond to:
	//	0 = CONSTANT
	//	1 = LAMBERT
	//	2 = PHONG
	//	3 = BLINN
	//	4 = SHADOW
	//	5 = VERTEXCOLOR
	int m_iShadingModel;

	// list of textures attached to this material
	int m_nTextureFilenames;
	char *m_ppTextureFilenames[MAX_TEXTURES_PER_MSH_MATERIAL];
	char *m_ppTextureSourcePaths[MAX_TEXTURES_PER_MSH_MATERIAL];

	unsigned int m_Attribute;


public:
	CMSHBlockMaterial(BlockHeader *pHdr,CZeroData *pData);
	virtual ~CMSHBlockMaterial();

	//--------------------
	//	File read/write methods
	virtual void Read(FILE *file);
	virtual void Write(FILE *file);
	virtual void WriteText(FILE *fp);

	//--------------------
	//	Get methods
	char *GetPrefix();
	char *GetName();
	void GetColor(float &fR,float &fG, float &fB, float &fA);
	void GetSpecularColor(float &fR,float &fG, float &fB, float &fA);
	void GetAmbientColor(float &fR,float &fG, float &fB, float &fA);
	int GetShadingModel();
	char **GetTextureFilenames(int &nTextures);
//	char **GetLightmapFilenames(int &nLightmaps);
	unsigned int GetAttribute();
	float GetPower();
	char **GetTextureSourcePaths(int &nTextures);
//	char *GetTextureSourcePath();
//	char *GetLightmapSourcePath();

	//--------------------
	//	Set methods
	void AddTexture(char *pFilename,char *pFullPath = NULL);
	void ReplaceTexture(char *pFilename);
//	void AddLightmap(char *pFilename);
	void SetPrefix(char *sPrefix);
	void SetName(char *sName);
	void SetColor(float fR, float fG, float fB, float fA);
	void SetSpecularColor(float fR, float fG, float fB, float fA);
	void SetAmbientColor(float fR, float fG, float fB, float fA);
	void SetShadingModel(int iShadingModel);
	void SetAttribute(unsigned int attrib);
	void SetPower(float power);
//	void SetTextureSourcePath(char *aTxtSrcPath);
//	void SetLightmapSourcePath(char *aLgtSrcPath);
	bool bMarked;
};


/*
	CMSHBlockMaterialSet
	
	Class for .msh file data blocks with 'MSET' header
*/

class CMSHBlockMaterialSet: public CMSHBlock
{

protected:
	//	list of materials
	unsigned int m_nMaterials;
	CMSHBlockMaterial **m_ppMaterials;


public:
	CMSHBlockMaterialSet(BlockHeader *pHdr,CZeroData *pData);
	virtual ~CMSHBlockMaterialSet();

	
	//--------------------
	//	File read/write methods
	virtual void Read(FILE *fp);
	virtual void Write(FILE *fp);
	virtual void WriteText(FILE *fp);

	
	//--------------------
	//	Get methods
	
	// Set nMaterials to m_nMaterials and return m_ppMaterials
	CMSHBlockMaterial **GetMaterials(unsigned int &nMaterials);

	//	Return the material with name == matName
	CMSHBlockMaterial *GetMaterialByName(const char *matName);
	CMSHBlockMaterial *GetMaterialByIndex(int index);
	int GetMaterialIndexByName(const char *matName);
	
	//	Return a material that has the same properties as pMaterial
	CMSHBlockMaterial *GetMaterialMatch(CMSHBlockMaterial *pMaterial);

	
	//--------------------
	//	Set methods
	
	//	Looks for a material to the material list that has a texture = pTextureFilename.  
	//	If such a material doesn't exist, then this function creates 
	//	a new one.  Sets ppOutMaterialName to the name of the (found/new) material.
	void AddTextureMaterial(char *pTextureFilename, char **ppOutMaterialName);

	//	Add a new material to the list and return it's pointer.
	CMSHBlockMaterial *AddNewMaterial();



	//	RemoveDuplicates removes duplicated materials from the list by calling CompareMaterials()
	void RemoveDuplicates();
	void RemoveMarkedMaterials();

	//	CompareMaterials returns true if the materials have the same properties.  Note:
	//	material names do not have to be the same for this to return true.
	bool CompareMaterials(CMSHBlockMaterial *pMaterial1, CMSHBlockMaterial *pMaterial2);
};
#endif