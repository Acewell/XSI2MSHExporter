#include "MSHBlockSceneInfo.h"
#include <string.h>

CMSHBlockSceneInfo::CMSHBlockSceneInfo(BlockHeader *pHdr,CZeroData *pData):
	CMSHBlock(pHdr,pData)
{
	m_sSceneName = NULL;
	
	m_StartFrame = 1;
	m_EndFrame = 100;
	m_FrameRate=30.0;

    SetGlobalBBox(0, 0, 0, 0, 0, 0, 0);
}

CMSHBlockSceneInfo::~CMSHBlockSceneInfo()
{
	if(m_sSceneName)
		delete[] m_sSceneName;
}

void CMSHBlockSceneInfo::Read(FILE *file)
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
    		case 'EMAN':
    			ReadString(file, header.size,&m_sSceneName);
	    		break;

	        case 'MARF':
			    ReadData(file,&m_StartFrame,sizeof(int));
    			ReadData(file,&m_EndFrame,sizeof(int));
	    		ReadData(file,&m_FrameRate,sizeof(float));
		    	break;

		    default:
			    SkipBlock(file, header.size);
    			break;
		}
	}

}

void CMSHBlockSceneInfo::Write(FILE *file)
{
	StartBlock(file, 'SINF');

		if (m_sSceneName)
		{
			StartBlock(file, 'NAME');
			WriteString(file, m_sSceneName);
			EndBlock(file);
		}

		StartBlock(file, 'FRAM');
		WriteData(file,&m_StartFrame,sizeof(int));
		WriteData(file,&m_EndFrame,sizeof(int));
		WriteData(file,&m_FrameRate,sizeof(float));
		EndBlock(file);

        // global BBox
		StartBlock(file, 'BBOX');
        float q0 = 0.0;
        float q1 = 0.0;
        float q2 = 0.0;
        float q3 = 1.0;
        
		WriteData(file,&q0,sizeof(float));
		WriteData(file,&q1,sizeof(float));
		WriteData(file,&q2,sizeof(float));
		WriteData(file,&q3,sizeof(float));

		WriteData(file,&m_fTransX,sizeof(float));
		WriteData(file,&m_fTransY,sizeof(float));
		WriteData(file,&m_fTransZ,sizeof(float));

		WriteData(file,&m_fSizeX,sizeof(float));
		WriteData(file,&m_fSizeY,sizeof(float));
		WriteData(file,&m_fSizeZ,sizeof(float));
		WriteData(file,&m_fSizeW,sizeof(float));
		EndBlock(file);

	EndBlock(file);
}

void CMSHBlockSceneInfo::WriteText(FILE *fp)
{
	fprintf(fp,"\tScene Info\n\t{\n");
	if (m_sSceneName)
		fprintf(fp,"\t\tPrefix{%s}\n",m_sSceneName);

	fprintf(fp,"\t\tFrameRange{%d,%d}\n",m_StartFrame,m_EndFrame);
	fprintf(fp,"\t\tFrameRate{%f}\n",m_FrameRate);
	fprintf(fp,"\t}\n");
}

void CMSHBlockSceneInfo::SetSceneName(char *sSceneName)
{
	if(m_sSceneName)
		delete[] m_sSceneName;
	m_sSceneName = new char[strlen(sSceneName)+1];
	strcpy(m_sSceneName,sSceneName);

}

void CMSHBlockSceneInfo::SetFrameInfo(int StartFrame, int EndFrame, float FrameRate)
{
	m_StartFrame = StartFrame;
	m_EndFrame = EndFrame;
	m_FrameRate = FrameRate;
}

char *CMSHBlockSceneInfo::GetName()
{
	return m_sSceneName;
}

float CMSHBlockSceneInfo::GetFrameRate()
{
	return m_FrameRate;
}

int CMSHBlockSceneInfo::GetStartFrame()
{
	return m_StartFrame;
}

int CMSHBlockSceneInfo::GetEndFrame()
{
	return m_EndFrame;
}


void CMSHBlockSceneInfo::SetGlobalBBox(float SizeX, float SizeY, float SizeZ, float SizeW, float TransX, float TransY, float TransZ)
{
   m_fSizeX = SizeX; 
   m_fSizeY = SizeY; 
   m_fSizeZ = SizeZ;
   m_fSizeW = SizeW;
   m_fTransX = TransX;
   m_fTransY = TransY;
   m_fTransZ = TransZ;
}

