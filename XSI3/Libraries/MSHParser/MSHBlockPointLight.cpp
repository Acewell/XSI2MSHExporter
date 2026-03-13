#include "zero.h"
#include "MSHBlockPointLight.h"
#include "MSHBlockConstraintList.h"

float *CMSHBlockPointLight::debugLightY=NULL;
CMSHBlockPointLight::CMSHBlockPointLight(BlockHeader *pHdr,CZeroData *pData):
	CMSHBlockLight(pHdr,pData)
{
	debugLightY=&m_fPositionY;
}

CMSHBlockPointLight::~CMSHBlockPointLight()
{
}

void CMSHBlockPointLight::Read(FILE *file)
{
	unsigned int blockSize=m_Header.size;

	while (blockSize)
	{
		// read a block header
		BlockHeader header;
		ReadHeader(file,&header);
		
		// deduct blockSize
		_ASSERTE(blockSize >= sizeof(BlockHeader) + header.size);
		blockSize -= sizeof(BlockHeader);
		blockSize -= header.size;
		
		// parse the block
		switch (header.type)
		{
		case 'XFRP':
			ReadString(file, header.size,&m_sPrefix);
			break;
		case 'EMAN':
			ReadString(file, header.size,&m_sName);
			break;
		case 'ATAD':
			ReadData(file,&m_fR,sizeof(float));
			ReadData(file,&m_fG,sizeof(float));
			ReadData(file,&m_fB,sizeof(float));
			ReadData(file,&m_fA,sizeof(float));
			ReadData(file,&m_fPositionX,sizeof(float));
			ReadData(file,&m_fPositionY,sizeof(float));
			ReadData(file,&m_fPositionZ,sizeof(float));
			ReadData(file,&m_fFalloffStart,sizeof(float));
			ReadData(file,&m_fFalloffEnd,sizeof(float));
			if(header.size>9*sizeof(float))
			{
				ReadData(file,&m_uFlags,sizeof(unsigned int));
			}
			break;
		case 'TSLC':
			SkipBlock(file, header.size);
//			ReadConstraintList(file,&header,&m_ppConstraintLists,m_nConstraintLists);
			break;
		default:
			SkipBlock(file, header.size);
			break;
		}
	}
}

void CMSHBlockPointLight::Write(FILE *file)
{
	StartBlock(file, 'LGTP');
			
		if (m_sPrefix)
		{
			StartBlock(file, 'PRFX');
			WriteString(file, m_sPrefix);
			EndBlock(file);
		}

		if (m_sName)
		{
			StartBlock(file, 'NAME');
			WriteString(file, m_sName);
			EndBlock(file);
		}

		StartBlock(file, 'DATA');
			WriteData(file,&m_fR,sizeof(float));
			WriteData(file,&m_fG,sizeof(float));
			WriteData(file,&m_fB,sizeof(float));
			WriteData(file,&m_fA,sizeof(float));
			WriteData(file,&m_fPositionX,sizeof(float));
			WriteData(file,&m_fPositionY,sizeof(float));
			WriteData(file,&m_fPositionZ,sizeof(float));
			WriteData(file,&m_fFalloffStart,sizeof(float));
			WriteData(file,&m_fFalloffEnd,sizeof(float));
			WriteData(file,&m_uFlags,sizeof(unsigned int));
		EndBlock(file);

//		for(unsigned int i=0;i<m_nConstraintLists;i++)
//			m_ppConstraintLists[i] ->Write(file);	

	// close light block
	EndBlock(file);
}

void CMSHBlockPointLight::SetPosition(float fX, float fY, float fZ)
{
	m_fPositionX = fX;
	m_fPositionY = fY;
	m_fPositionZ = fZ;
}

void CMSHBlockPointLight::SetFalloff(float fFalloffStart, float fFalloffEnd)
{
	m_fFalloffStart = fFalloffStart;
	m_fFalloffEnd = fFalloffEnd;
}

void CMSHBlockPointLight::WriteText(FILE *f)
{
	CMSHBlockLight::WriteText(f);
}

void CMSHBlockPointLight::GetPosition(float &fX,float &fY,float &fZ)
{
	fX=m_fPositionX;
	fY=m_fPositionY;
	fZ=m_fPositionZ;
}

float CMSHBlockPointLight::GetFalloffStart()
{
	return m_fFalloffStart;
}

float CMSHBlockPointLight::GetFalloffEnd()
{
	return m_fFalloffEnd;
}

