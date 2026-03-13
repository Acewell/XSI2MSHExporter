#include "MSHBlockMaterialSet.h"
#include "string.h"

#define FLCOMPARE(A,B,C) ((((A)<(B)) ? (((B)-(A)) <= (C)) : (((A)-(B)) <= (C))))
/*----------------------------------------------------------------------------------------
CMSHBlockMaterial
----------------------------------------------------------------------------------------*/
CMSHBlockMaterial::CMSHBlockMaterial(BlockHeader *pHdr,CZeroData *pData):
	CMSHBlock(pHdr,pData)
{
	m_sPrefix=NULL;
	m_sMaterialName=NULL;

	m_nTextureFilenames = 0;
	for(int i=0;i<MAX_TEXTURES_PER_MSH_MATERIAL;i++)
	{
		m_ppTextureFilenames[i]=NULL;
		m_ppTextureSourcePaths[i]=NULL;
	}

	m_fR=m_fG=m_fB=0.0;
	m_fA=1.0;
	m_fSpecularR=m_fSpecularG=m_fSpecularB=0.0;
	m_fSpecularA=0.0;
	m_fAmbientR=m_fAmbientG=m_fAmbientB=0.0;
	m_fAmbientA=0.0;
	m_iShadingModel = 2;
	m_fPower = 0.0;

	m_Attribute = 0;
}

CMSHBlockMaterial::~CMSHBlockMaterial()
{
	if(m_sPrefix)
		delete[] m_sPrefix;
	if(m_sMaterialName)
		delete[] m_sMaterialName;

	for(int i=0;i<m_nTextureFilenames && i<MAX_TEXTURES_PER_MSH_MATERIAL;i++)
	{
		delete[] m_ppTextureFilenames[i];
		delete[] m_ppTextureSourcePaths[i];
	}
}

void CMSHBlockMaterial::Read(FILE *file)
{
	unsigned int blockSize=m_Header.size;
	char *pFilename;

	while (blockSize)
	{
		// read a block header
		BlockHeader header;
		ReadHeader(file,&header);
		
		// deduct blockSize
		assert(blockSize >= sizeof(BlockHeader) + header.size);
		blockSize -= sizeof(BlockHeader);
		blockSize -= header.size;
		
		// parse the block
		switch (header.type)
		{
		case 'XFRP':
			ReadString(file, header.size,&m_sPrefix);
			break;
		case 'EMAN':
			ReadString(file, header.size,&m_sMaterialName);
			break;
		case 'BRTA':
			ReadData(file,&m_Attribute,sizeof(unsigned int));
			break;
		case 'ATAD':
			ReadData(file,&m_fR,sizeof(float));
			ReadData(file,&m_fG,sizeof(float));
			ReadData(file,&m_fB,sizeof(float));
			ReadData(file,&m_fA,sizeof(float));
			ReadData(file,&m_fSpecularR,sizeof(float));
			ReadData(file,&m_fSpecularG,sizeof(float));
			ReadData(file,&m_fSpecularB,sizeof(float));
			ReadData(file,&m_fSpecularA,sizeof(float));
			ReadData(file,&m_fAmbientR,sizeof(float));
			ReadData(file,&m_fAmbientG,sizeof(float));
			ReadData(file,&m_fAmbientB,sizeof(float));
			ReadData(file,&m_fAmbientA,sizeof(float));
//			ReadData(file,&m_iShadingModel,sizeof(int));
			ReadData(file,&m_fPower,sizeof(float));
			break;
		case 'DXET':
		case 'D0XT':
		case 'D1XT':
		case 'D2XT':
		case 'D3XT':
		case 'D4XT':
		case 'D5XT':
		case 'D6XT':
		case 'D7XT':
		case 'D8XT':
		case 'D9XT':
			pFilename=NULL;
			ReadString(file,header.size,&pFilename);
			AddTexture(pFilename);
			if(pFilename)
				delete[] pFilename;
			break;
		default:
			SkipBlock(file, header.size);
			break;
		}
	}
}

void CMSHBlockMaterial::Write(FILE *file)
{
	// start material block
	StartBlock(file, 'MATD');

		if (m_sPrefix)
		{
			StartBlock(file, 'PRFX');
			WriteString(file, m_sPrefix);
			EndBlock(file);
		}

		if (m_sMaterialName)
		{
			StartBlock(file, 'NAME');
			WriteString(file, m_sMaterialName);
			EndBlock(file);
		}

		// start data block
		StartBlock(file, 'DATA');
			WriteData(file,&m_fR,sizeof(float));
			WriteData(file,&m_fG,sizeof(float));
			WriteData(file,&m_fB,sizeof(float));
			WriteData(file,&m_fA,sizeof(float));
			WriteData(file,&m_fSpecularR,sizeof(float));
			WriteData(file,&m_fSpecularG,sizeof(float));
			WriteData(file,&m_fSpecularB,sizeof(float));
			WriteData(file,&m_fSpecularA,sizeof(float));
			WriteData(file,&m_fAmbientR,sizeof(float));
			WriteData(file,&m_fAmbientG,sizeof(float));
			WriteData(file,&m_fAmbientB,sizeof(float));
			WriteData(file,&m_fAmbientA,sizeof(float));
//			WriteData(file,&m_iShadingModel,sizeof(int));
			WriteData(file,&m_fPower,sizeof(float));
		EndBlock(file);

		StartBlock(file, 'ATRB');
			WriteData(file, &m_Attribute,sizeof(unsigned int));
		EndBlock(file);

		int i;
		for(i=0;i<m_nTextureFilenames;i++)
		{
			if(i<MAX_TEXTURES_PER_MSH_MATERIAL)
			{
				switch(i)
				{
				case 0:StartBlock(file, 'TX0D');break;
				case 1:StartBlock(file, 'TX1D');break;
				case 2:StartBlock(file, 'TX2D');break;
				case 3:StartBlock(file, 'TX3D');break;
				case 4:StartBlock(file, 'TX4D');break;
				case 5:StartBlock(file, 'TX5D');break;
				case 6:StartBlock(file, 'TX6D');break;
				case 7:StartBlock(file, 'TX7D');break;
				case 8:StartBlock(file, 'TX8D');break;
				case 9:StartBlock(file, 'TX9D');break;
				}
				
				WriteString(file,m_ppTextureFilenames[i]);
				EndBlock(file);
			}
		}
	// close material block
	EndBlock(file);
}

void CMSHBlockMaterial::WriteText(FILE *fp)
{
	fprintf(fp,"\tMaterial\n\t{\n");
	if(m_sPrefix)
		fprintf(fp,"\t\tPrefix{%s}\n",m_sPrefix);
	if(m_sMaterialName)
		fprintf(fp,"\t\tName{%s}\n",m_sMaterialName);
	fprintf(fp,"\t\tColor{%f,%f,%f,%f}\n",m_fR,m_fG,m_fB,m_fA);
	fprintf(fp,"\t\tSpecularColor{%f,%f,%f,%f}\n",m_fSpecularR,m_fSpecularG,m_fSpecularB,m_fSpecularA);
	fprintf(fp,"\t\tAmbientColor{%f,%f,%f,%f}\n",m_fAmbientR,m_fAmbientG,m_fAmbientB,m_fAmbientA);
	fprintf(fp,"\t\tShadingModel{%d}\n",m_iShadingModel);
	fprintf(fp,"\t\tAttribute{%d}\n",m_Attribute);
	
	if(m_nTextureFilenames)
	{
		int i;
		for(i=0;i<m_nTextureFilenames;i++)
			fprintf(fp,"\t\tTexture{%s}\n",m_ppTextureFilenames[i]);
	}
	fprintf(fp,"\t}\n");
}

void CMSHBlockMaterial::AddTexture(char *pFilename,char *pFilePath)
{

	bool bFound=false;
	for(int i=0;i<m_nTextureFilenames && !bFound;i++)
	{
		if(strcmpi(m_ppTextureFilenames[i],pFilename)==0)
			bFound=true;
	}
//	if(bFound)
//		return;

	if(m_nTextureFilenames>=MAX_TEXTURES_PER_MSH_MATERIAL)
		return;
	
	m_ppTextureFilenames[m_nTextureFilenames]=new char[strlen(pFilename)+1];
	strcpy(m_ppTextureFilenames[m_nTextureFilenames],pFilename);
	if(pFilePath)
	{
		m_ppTextureSourcePaths[m_nTextureFilenames]=new char[strlen(pFilePath)+1];
		strcpy(m_ppTextureSourcePaths[m_nTextureFilenames],pFilePath);
	}
	else
	{
		m_ppTextureSourcePaths[m_nTextureFilenames]=new char[1];
		strcpy(m_ppTextureSourcePaths[m_nTextureFilenames],"");
	}
/*
	char **pNewList;
	char **pNewPathList;
	
	pNewList = new char *[m_nTextureFilenames+1];
	pNewPathList = new char *[m_nTextureFilenames+1];
	if(m_ppTextureFilenames)
	{
		memcpy(pNewList,m_ppTextureFilenames, m_nTextureFilenames*sizeof(char *));
		delete[] m_ppTextureFilenames;
	}
	if(m_ppTextureSourcePaths)
	{
		memcpy(pNewPathList,m_ppTextureSourcePaths, m_nTextureFilenames*sizeof(char *));
		delete[] m_ppTextureSourcePaths;
	}
	pNewList[m_nTextureFilenames]=new char[strlen(pFilename)+1];
	strcpy(pNewList[m_nTextureFilenames],pFilename);
	if(pFilePath)
	{
		pNewPathList[m_nTextureFilenames]=new char[strlen(pFilePath)+1];
		strcpy(pNewPathList[m_nTextureFilenames],pFilePath);
	}
	else
	{
		pNewPathList[m_nTextureFilenames]=new char[1];
		strcpy(pNewPathList[m_nTextureFilenames],"");
	}
	
	m_ppTextureFilenames = pNewList;
	m_ppTextureSourcePaths = pNewPathList;
*/
	m_nTextureFilenames++;
}

//	ReplaceTexture looks for a texture with the same filename, but possibly different
//	extension.  Then it replaces the texture with the new filename.extension.  This is here
//	because SoftImage 3.9 doesn't let you texture with tga files
void CMSHBlockMaterial::ReplaceTexture(char *pFilename)
{
	if(!m_ppTextureFilenames)
		return;

	bool bFound=false;
	for(int i = 0;i<m_nTextureFilenames && !bFound;i++)
	{
		if(strnicmp(m_ppTextureFilenames[i],pFilename,strlen(pFilename)-3)==0)
		{
//			bFound = true;
			strcpy(m_ppTextureFilenames[i],pFilename);
		}
	}
}

void CMSHBlockMaterial::SetName(char *sName)
{
	if(m_sMaterialName)
		delete[] m_sMaterialName;
	m_sMaterialName=new char[strlen(sName)+1];
	strcpy(m_sMaterialName,sName);
}

void CMSHBlockMaterial::SetPrefix(char *sPrefix)
{
	if(m_sPrefix)
		delete[] m_sPrefix;
	m_sPrefix = new char[strlen(sPrefix)+1];
	strcpy(m_sPrefix,sPrefix);
}

void CMSHBlockMaterial::SetColor(float fR, float fG, float fB, float fA)
{
	m_fR = fR;
	m_fG = fG;
	m_fB = fB;
	m_fA = fA;
}

void CMSHBlockMaterial::SetSpecularColor(float fR, float fG, float fB, float fA)
{
	m_fSpecularR = fR;
	m_fSpecularG = fG;
	m_fSpecularB = fB;
	m_fSpecularA = fA;
}

void CMSHBlockMaterial::SetAmbientColor(float fR, float fG, float fB, float fA)
{
	m_fAmbientR = fR;
	m_fAmbientG = fG;
	m_fAmbientB = fB;
	m_fAmbientA = fA;
}


void CMSHBlockMaterial::SetAttribute(unsigned int attrib)
{
	m_Attribute = attrib;
}

void CMSHBlockMaterial::SetShadingModel(int iShadingModel)
{
	m_iShadingModel = iShadingModel;
}

void CMSHBlockMaterial::SetPower(float power)
{
	m_fPower = power;
}

char *CMSHBlockMaterial::GetPrefix()
{
	return m_sPrefix;
}

char *CMSHBlockMaterial::GetName()
{
	return m_sMaterialName;
}

char **CMSHBlockMaterial::GetTextureSourcePaths(int &nPaths)
{
	nPaths=m_nTextureFilenames;
	return m_ppTextureSourcePaths;
}

void CMSHBlockMaterial::GetColor(float &fR,float &fG, float &fB, float &fA)
{
	fR=m_fR;
	fG=m_fG;
	fB=m_fB;
	fA=m_fA;
}

void CMSHBlockMaterial::GetSpecularColor(float &fR,float &fG, float &fB, float &fA)
{
	fR=m_fSpecularR;
	fG=m_fSpecularG;
	fB=m_fSpecularB;
	fA=m_fSpecularA;
}

void CMSHBlockMaterial::GetAmbientColor(float &fR,float &fG, float &fB, float &fA)
{
	fR=m_fAmbientR;
	fG=m_fAmbientG;
	fB=m_fAmbientB;
	fA=m_fAmbientA;
}

int CMSHBlockMaterial::GetShadingModel()
{
	return m_iShadingModel;
}

unsigned int CMSHBlockMaterial::GetAttribute()
{
	return m_Attribute;
}

char **CMSHBlockMaterial::GetTextureFilenames(int &nTextures)
{
	nTextures=m_nTextureFilenames;
	return m_ppTextureFilenames;
}

float CMSHBlockMaterial::GetPower()
{
	return m_fPower;
}

/*----------------------------------------------------------------------------------------
CMSHBlockMaterialSet
----------------------------------------------------------------------------------------*/
CMSHBlockMaterialSet::CMSHBlockMaterialSet(BlockHeader *pHdr,CZeroData *pData):
	CMSHBlock(pHdr,pData)
{
	m_nMaterials = 0;
	m_ppMaterials = NULL;
}

CMSHBlockMaterialSet::~CMSHBlockMaterialSet()
{
	if(m_ppMaterials)
	{
		unsigned int i;
		for(i=0;i<m_nMaterials;i++)
			delete m_ppMaterials[i];
		delete m_ppMaterials;
	}
}

void CMSHBlockMaterialSet::Read(FILE *file)
{
	unsigned int blockSize=m_Header.size;
	unsigned int size;
	unsigned int count;
	unsigned int i;

	while (blockSize)
	{
		// read a block header
		BlockHeader matheader;
		BlockHeader header;
		ReadHeader(file,&header);
		
		// deduct blockSize
		assert(blockSize >= sizeof(BlockHeader) + header.size);
		blockSize -= sizeof(BlockHeader);
		blockSize -= header.size;
		
		CMSHBlockMaterial *pMaterial;
		// parse the block
		switch (header.type)
		{
		case 'DTAM':
			pMaterial = AddNewMaterial();
			pMaterial ->SetHeader(&header);
			pMaterial ->Read(file);
			break;
		case 'LTAM':
			if(m_ppMaterials)
			{
				for(i=0;i<m_nMaterials;i++)
					delete m_ppMaterials[i];
				delete m_ppMaterials;
			}
			size = header.size;
			ReadData(file,&count, sizeof(unsigned));

			// deduct size
			size -= sizeof(unsigned);


			// while there is size left...
			while (size)
			{
				// read a block header
				ReadData(file,&matheader, sizeof(BlockHeader));

				// deduct size
				size -= sizeof(BlockHeader);
				size -= header.size;
				// parse the block
				switch (header.type)
				{
				case 'DTAM':
					{
						// parse material block
						pMaterial = AddNewMaterial();
						pMaterial ->SetHeader(&header);
						pMaterial ->Read(file);
					}
					break;

				default:
					{
						// skip unrecognized block
						SkipBlock(file, header.size);
					}
					break;
				}
			}
			break;
		default:
			SkipBlock(file, header.size);
			break;
		}
	}
}

void CMSHBlockMaterialSet::Write(FILE *file)
{
	unsigned int nMaterials = m_nMaterials;
	StartBlock(file, 'MATL');
	
	WriteData(file,&nMaterials,sizeof(unsigned int));
	// for each material...
	for (unsigned int i=0;i<m_nMaterials;i++)
		m_ppMaterials[i] ->Write(file);

	// close material set block
	EndBlock(file);
/* old
	StartBlock(file, 'MSET');
	
	// for each material...
	for (unsigned int i=0;i<m_nMaterials;i++)
		m_ppMaterials[i] ->Write(file);

	// close material set block
	EndBlock(file);
*/
}

void CMSHBlockMaterialSet::WriteText(FILE *fp)
{
	fprintf(fp,"\nMaterialSet\n{\n");
	if(m_nMaterials)
	{
		unsigned int i;
		for(i=0;i<m_nMaterials;i++)
			m_ppMaterials[i] ->WriteText(fp);
	}
	fprintf(fp,"}\n");
}

CMSHBlockMaterial *CMSHBlockMaterialSet::AddNewMaterial()
{
	BlockHeader header;
	header.size = 0;

	CMSHBlockMaterial *pMaterial=new CMSHBlockMaterial(&header,m_pData);
	CMSHBlockMaterial **pNewList=new CMSHBlockMaterial *[m_nMaterials+1];
	if(m_ppMaterials)
	{
		memcpy(pNewList,m_ppMaterials, m_nMaterials*sizeof(CMSHBlockMaterial *));
		delete[] m_ppMaterials;
	}
	pNewList[m_nMaterials]=pMaterial;
	m_ppMaterials = pNewList;
	m_nMaterials++;
	return pMaterial;
}

void CMSHBlockMaterialSet::AddTextureMaterial(char *pTextureFilename, char **ppOutMaterialName)
{
	unsigned int i;
	char **ppFilenames;
	int nFilenames;
	char *matName=NULL;
	bool bTextureMatch;
	
	for(i=0;i<m_nMaterials;i++)
	{
		bTextureMatch=false;
		ppFilenames = m_ppMaterials[i]->GetTextureFilenames(nFilenames);
		
		if(pTextureFilename)
		{
			if(nFilenames>0)
				if(strcmpi(ppFilenames[0],pTextureFilename)==0)
					bTextureMatch=true;
		}
		else
			if(nFilenames==0)
				bTextureMatch=true;

		if(bTextureMatch)
			matName = m_ppMaterials[i] ->GetName();
	}



	CMSHBlockMaterial *pMaterial;
	CMSHBlockMaterial **pNewList;
	BlockHeader header;
	header.size=0;
	char newName[1024];
	int j;
	if(!matName)
	{
		pMaterial=new CMSHBlockMaterial(&header,m_pData);
		if(pTextureFilename)
			pMaterial ->AddTexture(pTextureFilename);
		pNewList = new CMSHBlockMaterial *[m_nMaterials+1];
		if(m_ppMaterials)
		{
			memcpy(pNewList,m_ppMaterials, m_nMaterials*sizeof(CMSHBlockMaterial *));
			delete[] m_ppMaterials;
		}
		pNewList[m_nMaterials]=pMaterial;
		m_ppMaterials = pNewList;
		m_nMaterials++;

		j=0;
		while(!matName)
		{
			sprintf(newName,"MSHmat%d",j);
			for(i=0;i<m_nMaterials;i++)
			{
				if(m_ppMaterials[i] ->GetName())
					if(strcmp(m_ppMaterials[i] ->GetName(),newName)==0)
						break;
			}
			if(i==m_nMaterials)
			{
				m_ppMaterials[m_nMaterials-1] ->SetName(newName);
				matName = m_ppMaterials[m_nMaterials-1] ->GetName();
			}
			j++;
		}
	}

	assert(matName);
	*ppOutMaterialName = new char[strlen(matName)+1];
	strcpy(*ppOutMaterialName,matName);
}

void CMSHBlockMaterialSet::RemoveMarkedMaterials()
{
	if(!m_nMaterials)
		return;

	unsigned int i,j;
	for(i=0;i<m_nMaterials;i++)
	{
		if(m_ppMaterials[i]->bMarked)
		{
			delete m_ppMaterials[i];
			m_ppMaterials[i] = NULL;
		}
	}

	unsigned int new_nMaterials=0;
	CMSHBlockMaterial **ppNewList=NULL;
	for(i=0;i<m_nMaterials;i++)
		if(m_ppMaterials[i])
			new_nMaterials++;

	if(new_nMaterials)
	{
		ppNewList = new CMSHBlockMaterial *[new_nMaterials];
		j=0;
		for(i=0;i<m_nMaterials;i++)
		{
			if((m_ppMaterials[i]) && (j<new_nMaterials))
				ppNewList[j++] = m_ppMaterials[i];
		}
	}
	delete[] m_ppMaterials;

	m_ppMaterials = ppNewList;
	m_nMaterials = new_nMaterials;
}

void CMSHBlockMaterialSet::RemoveDuplicates()
{
	if(!m_nMaterials)
		return;

	unsigned int i,j;
	for(i=0;i<m_nMaterials;i++)
	{
		CMSHBlockMaterial *pTest1 = m_ppMaterials[i];
		if(pTest1)
		{
			for(j=i+1;j<m_nMaterials;j++)
			{
				CMSHBlockMaterial *pTest2 = m_ppMaterials[j];
				if(pTest2)
				{
					if(CompareMaterials(pTest1,pTest2))
					{
						delete pTest2;
						m_ppMaterials[j] = NULL;
					}
				}
			}
		}
	}

	unsigned int new_nMaterials=0;
	CMSHBlockMaterial **ppNewList=NULL;
	for(i=0;i<m_nMaterials;i++)
		if(m_ppMaterials[i])
			new_nMaterials++;

	if(new_nMaterials)
	{
		ppNewList = new CMSHBlockMaterial *[new_nMaterials];
		j=0;
		for(i=0;i<m_nMaterials;i++)
		{
			if((m_ppMaterials[i]) && (j<new_nMaterials))
				ppNewList[j++] = m_ppMaterials[i];
		}
	}
	delete[] m_ppMaterials;

	m_ppMaterials = ppNewList;
	m_nMaterials = new_nMaterials;
}

CMSHBlockMaterial *CMSHBlockMaterialSet::GetMaterialByName(const char *matName)
{
	unsigned int i;
	for(i=0;i<m_nMaterials;i++)
	{
		if(strcmp(m_ppMaterials[i] ->GetName(),matName)==0)
			return m_ppMaterials[i];
	}

	return NULL;
}

CMSHBlockMaterial *CMSHBlockMaterialSet::GetMaterialByIndex(int index)
{
	if((unsigned)index<m_nMaterials)
		return m_ppMaterials[index];
	else
		return NULL;
}

int CMSHBlockMaterialSet::GetMaterialIndexByName(const char *matName)
{
	unsigned int i;
	for(i=0;i<m_nMaterials;i++)
	{
		if(strcmp(m_ppMaterials[i] ->GetName(),matName)==0)
			return i;
	}

	return -1;
}

CMSHBlockMaterial *CMSHBlockMaterialSet::GetMaterialMatch(CMSHBlockMaterial *pMaterial)
{
	unsigned int i;

	if(!pMaterial)
		return NULL;

	for(i=0;i<m_nMaterials;i++)
	{
		if(CompareMaterials(pMaterial, m_ppMaterials[i]))
			return m_ppMaterials[i];
	}

	return NULL;
}

bool CMSHBlockMaterialSet::CompareMaterials(CMSHBlockMaterial *pMaterial1, CMSHBlockMaterial *pMaterial2)
{
	bool bMatch=true;
	char **ppTextures;
	int nTextures;
	float fR, fG, fB, fA;
	float fSpR, fSpG, fSpB, fSpA;
	float fAmbR, fAmbG, fAmbB, fAmbA;
	int iShadModel;
	unsigned int attribute;
	
	pMaterial1 ->GetAmbientColor(fAmbR, fAmbG, fAmbB, fAmbA);
	pMaterial1 ->GetColor(fR, fG, fB, fA);
	pMaterial1 ->GetSpecularColor(fSpR, fSpG, fSpB, fSpA);
	ppTextures = pMaterial1 ->GetTextureFilenames(nTextures);
	iShadModel = pMaterial1 ->GetShadingModel();
	attribute = pMaterial1 ->GetAttribute();

	char **ppTextures2;
	int nTextures2;
	float fR2, fG2, fB2, fA2;
	float fSpR2, fSpG2, fSpB2, fSpA2;
	float fAmbR2, fAmbG2, fAmbB2, fAmbA2;
	int iShadModel2;
	unsigned int attribute2;
	
	pMaterial2 ->GetAmbientColor(fAmbR2, fAmbG2, fAmbB2, fAmbA2);
	pMaterial2 ->GetColor(fR2, fG2, fB2, fA2);
	pMaterial2 ->GetSpecularColor(fSpR2, fSpG2, fSpB2, fSpA2);
	ppTextures2 = pMaterial2 ->GetTextureFilenames(nTextures2);
	iShadModel2 = pMaterial2 ->GetShadingModel();
	attribute2 = pMaterial2 ->GetAttribute();

	// compare colors
	bMatch =	((FLCOMPARE(fR, fR2, 0.000001)) && 
				(FLCOMPARE(fG, fG2, 0.000001)) && 
				(FLCOMPARE(fB, fB2, 0.000001)) && 
				(FLCOMPARE(fA, fA2, 0.000001)) && 
				(FLCOMPARE(fSpR, fSpR2, 0.000001)) && 
				(FLCOMPARE(fSpG, fSpG2, 0.000001)) && 
				(FLCOMPARE(fSpB, fSpB2, 0.000001)) && 
// 				(FLCOMPARE(fSpA, fSpA2, 0.000001)) && 
				(FLCOMPARE(fAmbR, fAmbR2, 0.000001)) && 
				(FLCOMPARE(fAmbG, fAmbG2, 0.000001)) && 
				(FLCOMPARE(fAmbB, fAmbB2, 0.000001)) && 
				(FLCOMPARE(fAmbA, fAmbA2, 0.000001)) && 
				(nTextures==nTextures2) &&	
				(attribute==attribute2) &&	
				(iShadModel==iShadModel2));
	if(bMatch)
	{
		int k;
		for(k=0;k<nTextures;k++)
		{
			if(strcmpi(ppTextures2[k],ppTextures[k]))
				bMatch=false;
		}
	}

	return bMatch;
}

CMSHBlockMaterial **CMSHBlockMaterialSet::GetMaterials(unsigned int &nMaterials)
{
	nMaterials=m_nMaterials;
	return m_ppMaterials;
}
