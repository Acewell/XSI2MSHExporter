#include "zero.h"
#include "MSHBlockSpotLight.h"
#include "MSHBlockConstraintList.h"

CMSHBlockSpotLight::CMSHBlockSpotLight(BlockHeader *pHdr,CZeroData *pData):
	CMSHBlockLight(pHdr,pData)
{
}

CMSHBlockSpotLight::~CMSHBlockSpotLight()
{
}

void CMSHBlockSpotLight::Read(FILE *file)
{
	unsigned int blockSize=m_Header.size;

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
			ReadData(file,&m_fInterestX,sizeof(float));
			ReadData(file,&m_fInterestY,sizeof(float));
			ReadData(file,&m_fInterestZ,sizeof(float));
			ReadData(file,&m_fFalloffStart,sizeof(float));
			ReadData(file,&m_fFalloffEnd,sizeof(float));
			ReadData(file,&m_fConeAngle,sizeof(float));
			ReadData(file,&m_fSpreadAngle,sizeof(float));
			if(header.size>14*sizeof(float))
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

void CMSHBlockSpotLight::Write(FILE *file)
{
	StartBlock(file, 'LGTS');
			
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
			WriteData(file,&m_fInterestX,sizeof(float));
			WriteData(file,&m_fInterestY,sizeof(float));
			WriteData(file,&m_fInterestZ,sizeof(float));
			WriteData(file,&m_fFalloffStart,sizeof(float));
			WriteData(file,&m_fFalloffEnd,sizeof(float));
			WriteData(file,&m_fConeAngle,sizeof(float));
			WriteData(file,&m_fSpreadAngle,sizeof(float));
			WriteData(file,&m_uFlags,sizeof(unsigned int));
		EndBlock(file);

//		for(unsigned int i=0;i<m_nConstraintLists;i++)
//			m_ppConstraintLists[i] ->Write(file);	

	// close light block
	EndBlock(file);
}

void CMSHBlockSpotLight::SetPosition(float fX, float fY, float fZ)
{
	m_fPositionX = fX;
	m_fPositionY = fY;
	m_fPositionZ = fZ;
}

void CMSHBlockSpotLight::SetFalloff(float fFalloffStart, float fFalloffEnd)
{
	m_fFalloffStart = fFalloffStart;
	m_fFalloffEnd = fFalloffEnd;
}

void CMSHBlockSpotLight::SetInterest(float fX, float fY, float fZ)
{
	m_fInterestX = fX;
	m_fInterestY = fY;
	m_fInterestZ = fZ;
}

void CMSHBlockSpotLight::SetConeAngle(float fConeAngle)
{
	m_fConeAngle = fConeAngle;
}

void CMSHBlockSpotLight::SetSpreadAngle(float fSpreadAngle)
{
	m_fSpreadAngle = fSpreadAngle;
}

void CMSHBlockSpotLight::WriteText(FILE *f)
{
	CMSHBlockLight::WriteText(f);
}

void CMSHBlockSpotLight::GetPosition(float &fX,float &fY,float &fZ)
{
	fX=m_fPositionX;
	fY=m_fPositionY;
	fZ=m_fPositionZ;
}

void CMSHBlockSpotLight::GetInterest(float &fX,float &fY,float &fZ)
{
	fX=m_fInterestX;
	fY=m_fInterestY;
	fZ=m_fInterestZ;
}

float CMSHBlockSpotLight::GetConeAngle()
{
	return m_fConeAngle;
}

float CMSHBlockSpotLight::GetSpreadAngle()
{
	return m_fSpreadAngle;
}

float CMSHBlockSpotLight::GetFalloffStart()
{
	return m_fFalloffStart;
}

float CMSHBlockSpotLight::GetFalloffEnd()
{
	return m_fFalloffEnd;
}

