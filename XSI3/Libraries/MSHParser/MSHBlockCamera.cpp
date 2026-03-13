#include "zero.h"
#include "MSHBlockCamera.h"
#include "MSHBlockConstraintList.h"
#include <string.h>


CMSHBlockCameraAnimation::CMSHBlockCameraAnimation(char *aName)
	:ZeroSet<CMSHCameraKeyFrame>(16)
{
	memset(mName,0,sizeof(mName));
	strcpy(mName,aName);
}

CMSHBlockCameraAnimation::~CMSHBlockCameraAnimation()
{
}

void CMSHBlockCameraAnimation::Read(FILE *file)
{
	int i;

	fread (&mName, sizeof (char), 32, file); 
	fread (&mNumKeyframes, sizeof (int), 1, file); 
	for (i = 0; i < mNumKeyframes; i ++) 
	{
		int index;
		ZeroVector3 p;
		ZeroQuaternion q;
		float fov;
		
		fread (&index, sizeof (int), 1, file); 
		fread (&p, sizeof (ZeroVector3), 1, file); 
		fread (&q, sizeof (ZeroQuaternion), 1, file); 
		fread (&fov, sizeof (float), 1, file); 

		(*this)[i].key = index;
		((*this)[i].position) = p;
		((*this)[i].rotation) = q; 
		((*this)[i].mFOV) = fov; 
	}
}

void CMSHBlockCameraAnimation::Write(FILE *file)
{
	int i;

	fwrite (mName, sizeof (char), 32, file); 
	fwrite (&mNumKeyframes, sizeof (int), 1, file); 
	
	for (i = 0; i < mNumKeyframes; i ++) 
	{
		int index;
		ZeroVector3 p ((*this)[i].position);
		ZeroQuaternion r ((*this)[i].rotation);
		float fov = ((*this)[i].mFOV);
		
		index = (*this)[i].key;
		
		fwrite (&index, sizeof (int), 1, file); 
		fwrite (&p, sizeof (ZeroVector3), 1, file); 
		fwrite (&r, sizeof (ZeroQuaternion), 1, file); 
		fwrite (&fov, sizeof (float), 1, file); 
	}
}


CMSHBlockCamera::CMSHBlockCamera(BlockHeader *pHdr,CZeroData *pData):
	CMSHBlock(pHdr,pData)
{
	m_sPrefix=NULL;
	m_sName=NULL;

//	m_nConstraintLists = 0;
//	m_ppConstraintLists = NULL;
}

CMSHBlockCamera::~CMSHBlockCamera()
{
	if(m_sPrefix)
		delete[] m_sPrefix;
	if(m_sName)
		delete[] m_sName;

/*
	if(m_ppConstraintLists)
	{
		unsigned int i;
		for(i=0;i<m_nConstraintLists;i++)
			delete m_ppConstraintLists[i];
		delete m_ppConstraintLists;
	}
*/
}

void CMSHBlockCamera::Read(FILE *file)
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
			ReadData(file,&m_fPositionX,sizeof(float));
			ReadData(file,&m_fPositionY,sizeof(float));
			ReadData(file,&m_fPositionZ,sizeof(float));
			ReadData(file,&m_fInterestX,sizeof(float));
			ReadData(file,&m_fInterestY,sizeof(float));
			ReadData(file,&m_fInterestZ,sizeof(float));
			ReadData(file,&m_fRoll,sizeof(float));
			ReadData(file,&m_fFOV,sizeof(float));
			ReadData(file,&m_fNearPlane,sizeof(float));
			ReadData(file,&m_fFarPlane,sizeof(float));
			break;
		case 'MINA':
			{
				CMSHBlockCameraAnimation *node = CreateAnimation("");
				node->Read(file);
			}
			break;
		case 'TSLC':
            _ASSERTE (0);
			SkipBlock(file, header.size);
//			ReadConstraintList(file,&header,&m_ppConstraintLists,m_nConstraintLists);
			break;
		default:
			SkipBlock(file, header.size);
			break;
		}
	}
}

void CMSHBlockCamera::Write(FILE *file)
{
	StartBlock(file, 'CAMR');

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
			WriteData(file,&m_fPositionX,sizeof(float));
			WriteData(file,&m_fPositionY,sizeof(float));
			WriteData(file,&m_fPositionZ,sizeof(float));
			WriteData(file,&m_fInterestX,sizeof(float));
			WriteData(file,&m_fInterestY,sizeof(float));
			WriteData(file,&m_fInterestZ,sizeof(float));
			WriteData(file,&m_fRoll,sizeof(float));
			WriteData(file,&m_fFOV,sizeof(float));
			WriteData(file,&m_fNearPlane,sizeof(float));
			WriteData(file,&m_fFarPlane,sizeof(float));
		EndBlock(file);

		int nAnims=m_Anims.GetCount();
		if(nAnims)
		{
			ZeroVisList<CMSHBlockCameraAnimation>::ZeroListNode *node=m_Anims.GetFirst();
			while(node)
			{
				StartBlock(file, 'ANIM');
				node->info.Write(file);
				EndBlock(file);

				node=node->GetNext();
			}
		}
//		for(unsigned int i=0;i<m_nConstraintLists;i++)
//			m_ppConstraintLists[i] ->Write(file);	

	EndBlock(file);
}

void CMSHBlockCamera::WriteText(FILE *)
{
}

void CMSHBlockCamera::SetPrefix(char *sPrefix)
{
	if(m_sPrefix)
		delete[] m_sPrefix;
	m_sPrefix = new char[strlen(sPrefix)+1];
	strcpy(m_sPrefix,sPrefix);
}

void CMSHBlockCamera::SetName(char *sName)
{
	if(m_sName)
		delete[] m_sName;
	m_sName = new char[strlen(sName)+1];
	strcpy(m_sName,sName);
}

void CMSHBlockCamera::SetPosition(float fX, float fY, float fZ)
{
	m_fPositionX = fX;
	m_fPositionY = fY;
	m_fPositionZ = fZ;
}

void CMSHBlockCamera::SetInterest(float fX, float fY, float fZ)
{
	m_fInterestX = fX;
	m_fInterestY = fY;
	m_fInterestZ = fZ;
}

void CMSHBlockCamera::SetRoll(float fRoll)
{
	m_fRoll = fRoll;
}

void CMSHBlockCamera::SetFOV(float fFOV)
{
	m_fFOV = fFOV;
}

void CMSHBlockCamera::SetPlanes(float fNear, float fFar)
{
	m_fNearPlane = fNear;
	m_fFarPlane = fFar;
}


char *CMSHBlockCamera::GetPrefix()
{
	return m_sPrefix;
}

char *CMSHBlockCamera::GetName()
{
	return m_sName;
}

void CMSHBlockCamera::GetPosition(float &fX,float &fY,float &fZ)
{
	fX=m_fPositionX;
	fY=m_fPositionY;
	fZ=m_fPositionZ;
}

void CMSHBlockCamera::GetInterest(float &fX,float &fY,float &fZ)
{
	fX=m_fInterestX;
	fY=m_fInterestY;
	fZ=m_fInterestZ;
}

float CMSHBlockCamera::GetRoll()
{
	return m_fRoll;
}

float CMSHBlockCamera::GetFOV()
{
	return m_fFOV;
}

float CMSHBlockCamera::GetNearPlane()
{
	return m_fNearPlane;
}

float CMSHBlockCamera::GetFarPlane()
{
	return m_fFarPlane;
}

CMSHBlockCameraAnimation* CMSHBlockCamera::CreateAnimation (char *aName)
{
	ZeroVisList<CMSHBlockCameraAnimation>::ZeroListNode *node;
	CMSHBlockCameraAnimation keyframes (aName);
	node = m_Anims.Append (keyframes);
	return &node->info;
}

/*
CMSHBlockConstraintList **CMSHBlockCamera::GetConstraintLists(int &nLists)
{
	nLists=m_nConstraintLists;
	return m_ppConstraintLists;
}

void CMSHBlockCamera::AddConstraintList(CMSHBlockConstraintList *pNewCnsList)
{
	CMSHBlockConstraintList **pNewList;
	
	pNewList = new CMSHBlockConstraintList *[m_nConstraintLists+1];
	if(*m_ppConstraintLists)
	{
		memcpy(pNewList,m_ppConstraintLists, m_nConstraintLists*sizeof(CMSHBlockConstraintList *));
		delete[] m_ppConstraintLists;
	}
	pNewList[m_nConstraintLists]=pNewCnsList;
	m_ppConstraintLists = pNewList;
	m_nConstraintLists++;
}
*/