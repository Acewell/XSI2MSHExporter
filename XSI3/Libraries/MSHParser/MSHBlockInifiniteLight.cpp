#include "zero.h"
#include "MSHBlockInfiniteLight.h"
#include "MSHBlockConstraintList.h"

CMSHBlockInfiniteLight::CMSHBlockInfiniteLight(BlockHeader *pHdr,CZeroData *pData):
	CMSHBlockLight(pHdr,pData)
{
}

CMSHBlockInfiniteLight::~CMSHBlockInfiniteLight()
{
}

void CMSHBlockInfiniteLight::Read(FILE *file)
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
			ReadData(file,&m_fDirectionX,sizeof(float));
			ReadData(file,&m_fDirectionY,sizeof(float));
			ReadData(file,&m_fDirectionZ,sizeof(float));
			if(header.size>7*sizeof(float))
			{
				ReadData(file,&m_uFlags,sizeof(unsigned int));
			}
			break;
		case 'TSLC':
//			ReadConstraintList(file,&header,&m_ppConstraintLists,m_nConstraintLists);
			SkipBlock(file, header.size);
			break;
		default:
			SkipBlock(file, header.size);
			break;
		}
	}
}

void CMSHBlockInfiniteLight::Write(FILE *file)
{
	StartBlock(file, 'LGTI');
			
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
			WriteData(file,&m_fDirectionX,sizeof(float));
			WriteData(file,&m_fDirectionY,sizeof(float));
			WriteData(file,&m_fDirectionZ,sizeof(float));
			WriteData(file,&m_uFlags,sizeof(unsigned int));
		EndBlock(file);

//		for(unsigned int i=0;i<m_nConstraintLists;i++)
//			m_ppConstraintLists[i] ->Write(file);	

	// close light block
	EndBlock(file);
}

void CMSHBlockInfiniteLight::SetDirection(float fX, float fY, float fZ)
{
	m_fDirectionX = fX;
	m_fDirectionY = fY;
	m_fDirectionZ = fZ;
}

void CMSHBlockInfiniteLight::WriteText(FILE *f)
{
	CMSHBlockLight::WriteText(f);
}

void CMSHBlockInfiniteLight::GetDirection(float &fX,float &fY,float &fZ)
{
	fX=m_fDirectionX;
	fY=m_fDirectionY;
	fZ=m_fDirectionZ;
}
