/*
CMSHBlockAnimation

  Base class for the following light classes:
  CMSHBlockSpotLight
  CMSHBlockInfiniteLight
  CMSHBlockPointLight
  
*/

#include "zero.h"
#include "MSHBlock.h"
#include "ZeroData.h"
#include "MSHBlockModel.h"
#include "MSHBlockAnimations.h"


//#define COMPRESS_KEYFRAMES

//#define WRITE_EULERS_ANGLES



//CMSHBlockAnimation::CMSHBlockAnimation(BlockHeader *pHdr,CZeroData *pData)
//	:CMSHBlock (pHdr, pData), ZeroVisList<CMSHBlockNodeAnimation>(), mshCycles()

CMSHBlockAnimation::CMSHBlockAnimation()
	:CMSHBlock (), ZeroVisList<CMSHBlockNodeAnimation>(), mshCycles()
{
	mVersion=1;
}


CMSHBlockAnimation::~CMSHBlockAnimation()
{
}

void CMSHBlockAnimation::AddCycle (
	char *cycleName, 
	float frequency, 
	int beginFrame, 
	int endFrame, 
	int playStyle)
{
	CycleDescriptor cycle (cycleName, frequency, beginFrame, endFrame, playStyle);
	mshCycles.Append (cycle);
}



CMSHBlockNodeAnimation* CMSHBlockAnimation::CreateKeyFrames (
	BlockHeader *header,
	CMSHBlockModel *model,
	int jointsCount)
{
	ZeroListNode *node;
	CMSHBlockNodeAnimation keyframes (model, jointsCount);
	node = Append (keyframes);
	return &node->info;
}


CMSHBlockAnimation::CycleDescriptor::CycleDescriptor(
	char *cycleNameArg, 
	float frequencyArg, 
	int beginFrameArg, 
	int endFrameArg, 
	int playStyleArg)
{
	frequency = frequencyArg; 
	playStyle = playStyleArg;
	beginFrame = beginFrameArg;
	endFrame = endFrameArg;
	strncpy (cycleName, cycleNameArg, sizeof (cycleName));
}

void CMSHBlockAnimation::CycleDescriptor::GetData(int *aStyle, int *aStart, int *aEnd, float *aFreq, char *name)
{
	*aStyle = playStyle;
	*aStart = beginFrame;
	*aEnd = endFrame;
	*aFreq = frequency; 
	strcpy(name, cycleName);
}

void CMSHBlockAnimation::CycleDescriptor::SetData(int aStyle, int aStart, int aEnd, float aFreq, char *name)
{
	playStyle = aStyle ;
	beginFrame=aStart;
	endFrame=aEnd;
	frequency=aFreq; 
	strcpy(cycleName,name);
}

void CMSHBlockAnimation::CycleDescriptor::Read(FILE *file)
{
	fread (cycleName, MAX_CYCLE_NAME_LENGTH, 1, file);
	CalcStrCRC(cycleName,0);

	fread (&frequency, sizeof (int), 1, file); 
	fread (&playStyle, sizeof (int), 1, file); 
	fread (&beginFrame, sizeof (int), 1, file); 
	fread (&endFrame, sizeof (int), 1, file); 
}


void CMSHBlockAnimation::CycleDescriptor::WriteText(FILE *file)
{
	fprintf(file,"\t\tcycleName: %s\n", cycleName);
	fprintf(file,"\t\tfrequency: %f\n", frequency);
	fprintf(file,"\t\tplayStyle: %d\n", playStyle);
	fprintf(file,"\t\tbeginFrame: %d\n", beginFrame);
	fprintf(file,"\t\tendFrame: %d\n", endFrame);
	
	fprintf(file,"\n");
}


void CMSHBlockAnimation::CycleDescriptor::WriteZeroFile(ZeroFile &file, CZeroData *data)
{
	char name[256];
	memset (name, 0, sizeof(name));
	strcpy (name, cycleName);
	strlwr (name); 
	file.Write(name, MAX_CYCLE_NAME_LENGTH, 1);
	file.Write(&frequency, sizeof (int), 1); 
	file.Write(&playStyle, sizeof (int), 1); 
	file.Write(&beginFrame, sizeof (int), 1); 
	file.Write(&endFrame, sizeof (int), 1); 
}

void CMSHBlockAnimation::CycleDescriptor::Write(FILE *file)
{
	char name[256];
	memset (name, 0, sizeof(name));
	strcpy (name, cycleName);
	strlwr (name); 
	fwrite (name, MAX_CYCLE_NAME_LENGTH, 1, file);
	fwrite (&frequency, sizeof (int), 1, file); 
	fwrite (&playStyle, sizeof (int), 1, file); 
	fwrite (&beginFrame, sizeof (int), 1, file); 
	fwrite (&endFrame, sizeof (int), 1, file); 
}


void CMSHBlockAnimation::CycleDescriptor::MarkStartAndEndKeyFrames (CMSHBlockNodeAnimation* node)
{
	int index;
	index = ZeroBinarySearch (&node->mPositions[0], node->GetNumPositions(), beginFrame);
	node->mPositions[index].mark = 1;
	index = ZeroBinarySearch (&node->mRotations[0], node->GetNumRotations(), beginFrame);
	node->mRotations[index].mark = 1;

	index = ZeroBinarySearch (&node->mPositions[0], node->GetNumPositions(), endFrame);
	node->mPositions[index].mark = 1;
	index = ZeroBinarySearch (&node->mRotations[0], node->GetNumRotations(), endFrame);
	node->mRotations[index].mark = 1;
}



CMSHBlockAnimation::AnimationCycles::AnimationCycles()
	:ZeroVisList<CMSHBlockAnimation::CycleDescriptor>()
{
	
}

void CMSHBlockAnimation::AnimationCycles::Read(FILE *file)
{
	int i;
	int cyclesCount;
	BlockHeader header;
	
	ReadHeader(file, &header);
	
	_ASSERTE (header.type == 'LCYC');
	
	fread(&cyclesCount, sizeof (int), 1, file); 
	
	for(i = 0; i < cyclesCount; i++) {
		CycleDescriptor cycle ("", 30.0, 1, 100, 0);
		cycle.Read(file);
		Append (cycle);
	}
}

void CMSHBlockAnimation::AnimationCycles::WriteZeroFile(ZeroFile &file, CZeroData *data)
{
	int cyclesCount;
	StartZeroFileBlock(file,'CYCL');
	cyclesCount = GetCount();
	file.Write(&cyclesCount, sizeof (int), 1); 

	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		(*iter).WriteZeroFile(file,data);
	}
	EndZeroFileBlock(file);
}

void CMSHBlockAnimation::AnimationCycles::Write(FILE *file)
{
   int cyclesCount;
	StartBlock(file, 'CYCL');
	cyclesCount = GetCount();
	fwrite (&cyclesCount, sizeof (int), 1, file); 
	
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		(*iter).Write(file);
	}
	EndBlock(file);
}

void CMSHBlockAnimation::AnimationCycles::WriteText(FILE *file)
{
	fprintf(file,"\tcyclesNames\n");
	fprintf(file,"\t{\n");
	
   fprintf(file,"\t\tcyclesCount: %d\n", GetCount());
	fprintf(file,"\n");
	
   Iterator iter (*this);
   for (iter.Begin(); iter; iter ++) {
		(*iter).WriteText(file);
   }
	
	fprintf(file,"\t}\n");
}

void CMSHBlockAnimation::AnimationCycles::MarkStartAndEndKeyFrames (CMSHBlockNodeAnimation* node)
{
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		(*iter).MarkStartAndEndKeyFrames (node);
	}
}



void CMSHBlockAnimation::WriteText(FILE *file)
{
	//    int keyframesCount;
	CMSHBlockNodeAnimation* node;
	
	fprintf(file,"\nAnimation\n");
	fprintf(file,"{\n");
	
	mshCycles.WriteText(file);
	
	//    keyframesCount = GetFirst()->info[GetFirst()->info.GetKeyframesCount()-1].key;
	
	fprintf(file,"\teffectorsCount: %d\n", GetCount());
	//    fprintf(file,"\tkeyframesCount: %d\n", keyframesCount);
	
	fprintf(file,"\n");
	Iterator iter (*this);
	
	for (iter.Begin(); iter; iter ++) {
		node = &*iter;
		node->WriteText(file);
		if (iter.GetNode()->GetNext()) {
        	fprintf(file,"\n");
		}
	}
	
	fprintf(file,"\n");
	
	
	
	fprintf(file,"}\n");
	fflush(file);
}


void CMSHBlockAnimation::WriteZeroFile(ZeroFile &file, CZeroData *data)
{
   int effectorsCount;
   CMSHBlockNodeAnimation* node;
	
	StartZeroFileBlock(file, 'ANM2');
	mshCycles.WriteZeroFile(file,data);
	
#ifdef WRITE_EULERS_ANGLES
	StartZeroFileBlock(file, 'KFRM');
#else
	StartZeroFileBlock(file, 'KFR3');
#endif
	effectorsCount = GetCount();
	
	file.Write(&effectorsCount, sizeof (int), 1); 
	
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		node = &*iter;
		node->WriteZeroFile(file,data);
	}
	EndZeroFileBlock(file);
	
	EndZeroFileBlock(file);
}

void CMSHBlockAnimation::Write(FILE *file)
{
   int effectorsCount;
   CMSHBlockNodeAnimation* node;
	
	StartBlock(file, 'ANM2');
	mshCycles.Write(file);
	
#ifdef WRITE_EULERS_ANGLES
	StartBlock(file, 'KFRM');
#else
	StartBlock(file, 'KFR3');
#endif
	effectorsCount = GetCount();
	
	fwrite (&effectorsCount, sizeof (int), 1, file); 
	
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		node = &*iter;
		node->Write(file);
	}
	EndBlock(file);
	
	EndBlock(file);
}


void CMSHBlockAnimation::Read(FILE *file)
{
   int i;
   int effectorsCount;
   BlockHeader header;
	
   CMSHBlockNodeAnimation* node;
	
	mshCycles.Read(file);
	
   ReadHeader(file, &header);
   _ASSERTE ((header.type == 'MRFK') || (header.type == '2RFK') || (header.type == '3RFK'));
	
  	fread (&effectorsCount, sizeof (int), 1, file);
   for (i = 0; i < effectorsCount; i ++) {
     	node = CreateKeyFrames (&header, NULL, 0);
     	if(header.type== 'MRFK')
			node->Read(file, m_pData);
     	if(header.type== '2RFK')
			node->ReadQuat(file, m_pData);
     	if(header.type== '3RFK')
			node->ReadSeparateKeys(file, m_pData);
   }
	
}

CMSHBlockAnimation::AnimationCycles *CMSHBlockAnimation::GetAnimationCycles()
{
	return &mshCycles;
}



CMSHBlockNodeAnimation* CMSHBlockAnimation::GetNodeAnimation (const char *modelName)
{
	Iterator iter (*this);
	CMSHBlockNodeAnimation* node;

	for (iter.Begin(); iter; iter ++) 
	{
		node = &*iter;
		if(node->GetModel())
			if(strcmp(node->GetModel()->GetName(),modelName)==0)
				return node;
	}

	return NULL;
}

void CMSHBlockAnimation::StoreModelCRCs()
{
	Iterator iter (*this);
	CMSHBlockNodeAnimation* node;

	for (iter.Begin(); iter; iter ++) 
	{
		node = &*iter;
		node->StoreModelCRC();
	}
}

void CMSHBlockAnimation::RecoverModels(CZeroData *data)
{
	Iterator iter (*this);
	CMSHBlockNodeAnimation* node;

	for (iter.Begin(); iter; iter ++) 
	{
		node = &*iter;
		node->RecoverModel(data);
	}
}

void CMSHBlockAnimation::MarkStartAndEndKeyFrames ()
{
	Iterator iter (*this);
	CMSHBlockNodeAnimation* node;

	for (iter.Begin(); iter; iter ++) {
		node = &*iter;
		mshCycles.MarkStartAndEndKeyFrames (node);
	}
}


void CMSHBlockAnimation::CompressKeyFrames ()
{
	Iterator iter (*this);
	CMSHBlockNodeAnimation* node;
	
	MarkStartAndEndKeyFrames ();
	for (iter.Begin(); iter; iter ++) {
		node = &*iter;
		node->CompressKeyFrames();
	}
}




CMSHBlockNodeAnimation::CMSHBlockNodeAnimation(
	CMSHBlockModel *modelArg, 
	int jointsCountArg):
	mPositions(16),
		mRotations(16)
{
	mNumPositions = 0;
	mNumRotations = 0;
	model = modelArg;
	JointsCount = jointsCountArg;
}

CMSHBlockNodeAnimation::~CMSHBlockNodeAnimation()
{
}

void CMSHBlockNodeAnimation::WriteText(FILE *file)
{
	int i;
	fprintf(file,"\tnodeAnim\n");
	fprintf(file,"\t{\n");
	fprintf(file,"\t\tname: %s\n", model->GetName());
	fprintf(file,"\t\tkeyframeType: %d\n", JointsCount);
	
	fprintf(file,"\t\tnumPositions: %d\n", mNumPositions);
	for (i = 0; i < mNumPositions; i ++) {
		
		int index;
		ZeroVector3 p (mPositions[i].position);
		
		index = mPositions[i].key;

		fprintf(file, "\t\t%d: %8.5f %8.5f %8.5f\n",
							  index, p.x,  p.y,  p.z);
	}
	
	fprintf(file,"\t\tnumRotations: %d\n", mNumRotations);
	for (i = 0; i < mNumRotations; i ++) {
		
		int index;
		ZeroQuaternion r (mRotations[i].rotation);
		index = mRotations[i].key;

		ZeroMatrix m (r, ZeroVector3 (0, 0, 0));
		ZeroVector3 e (m.CalcPitchYawRollEulerAngles());

		fprintf(file, "\t\t%d: %8.5f %8.5f %8.5f\n",
							  index, RAD2DEG*e.x,  RAD2DEG*e.y,  RAD2DEG*e.z);
	}
	
	fprintf(file,"\t}\n");
}

void CMSHBlockNodeAnimation::WriteZeroFile(ZeroFile &file,CZeroData *pData)
{
	int i;
	unsigned nodeNameCRC;
	
	nodeNameCRC = CalcStrCRC (model->GetName());
	file.Write(&nodeNameCRC, sizeof (int), 1); 
	file.Write(&JointsCount, sizeof (int), 1); 
	file.Write(&mNumPositions, sizeof (int), 1); 
	file.Write(&mNumRotations, sizeof (int), 1); 
	for (i = 0; i < mNumPositions; i ++) 
	{
		int index;
		ZeroVector3 p (mPositions[i].position);
		index = mPositions[i].key;
		file.Write(&index, sizeof (int), 1); 
		file.Write(&p, sizeof (ZeroVector3), 1); 
	}
	for (i = 0; i < mNumRotations; i ++) 
	{
		int index;
		ZeroQuaternion r (mRotations[i].rotation);
		
		index = mRotations[i].key;
		file.Write(&index, sizeof (int), 1); 
		file.Write(&r, sizeof (ZeroQuaternion), 1); 
	}
}

void CMSHBlockNodeAnimation::Write(FILE *file)
{
	int i;
	unsigned nodeNameCRC;
	
	nodeNameCRC = CalcStrCRC (model->GetName());
	fwrite (&nodeNameCRC, sizeof (int), 1, file); 
	fwrite (&JointsCount, sizeof (int), 1, file); 
	fwrite (&mNumPositions, sizeof (int), 1, file); 
	fwrite (&mNumRotations, sizeof (int), 1, file); 
	for (i = 0; i < mNumPositions; i ++) 
	{
		int index;
		ZeroVector3 p (mPositions[i].position);
		index = mPositions[i].key;
		
		fwrite (&index, sizeof (int), 1, file); 
		fwrite (&p, sizeof (ZeroVector3), 1, file); 
	}

	for (i = 0; i < mNumRotations; i ++) 
	{
		int index;
		ZeroQuaternion r (mRotations[i].rotation);
		index = mRotations[i].key;
		
		fwrite (&index, sizeof (int), 1, file); 
		fwrite (&r, sizeof (ZeroQuaternion), 1, file); 
	}
}

void CMSHBlockNodeAnimation::Read(FILE *file, CZeroData *pData)
{
	int i;
	char name[256];
	unsigned nodeNameCRC;
	
	fread (&nodeNameCRC, sizeof (int), 1, file); 
	strcpy (name, InvertCRC (nodeNameCRC));
	model = pData->FindModelByName(name);
	if(pData->GetAnimation()->GetVersion()==0)
	{
		CMSHBlockSkeleton *pSkel=pData->GetSkeleton();
		if(!pSkel->FindEffector(model))
		{
			model=pData->FindModelByName(model->GetParentName());
			_ASSERTE (pSkel->FindEffector(model));
		}
	}
	_ASSERTE (model);
	
	fread (&JointsCount, sizeof (int), 1, file); 
	fread (&mNumPositions, sizeof (int), 1, file); 
	mNumRotations=mNumPositions;
	
	for (i = 0; i < mNumPositions; i ++) {
		int index;
		ZeroVector3 p;
		ZeroQuaternion r;
		
		fread (&index, sizeof (int), 1, file); 
		fread (&p, sizeof (ZeroVector3), 1, file); 

		ZeroVector3 e; 
		fread (&e, sizeof (ZeroVector3), 1, file); 
		ZeroMatrix pitch (PitchMatrix (e.x));
		ZeroMatrix yaw (YawMatrix (e.y));
		ZeroMatrix roll (RollMatrix (e.z));
		ZeroMatrix matrix (pitch * yaw * roll);
		r = ZeroQuaternion (matrix);
		mPositions[i].key = index;
		mRotations[i].key = index;
		(mPositions[i].position) = p;
		(mRotations[i].rotation) = r; 
	}
}

void CMSHBlockNodeAnimation::ReadQuat(FILE *file, CZeroData *pData)
{
	int i;
	char name[256];
	unsigned nodeNameCRC;
	
	fread (&nodeNameCRC, sizeof (int), 1, file); 
	strcpy (name, InvertCRC (nodeNameCRC));
	model = pData->FindModelByName(name);
	if(pData->GetAnimation()->GetVersion()==0)
	{
		CMSHBlockSkeleton *pSkel=pData->GetSkeleton();
		if(!pSkel->FindEffector(model))
		{
			model=pData->FindModelByName(model->GetParentName());
			_ASSERTE (pSkel->FindEffector(model));
		}
	}
	_ASSERTE (model);
	
	fread (&JointsCount, sizeof (int), 1, file); 
	fread (&mNumPositions, sizeof (int), 1, file); 
	mNumRotations=mNumPositions;
	
	for (i = 0; i < mNumPositions; i ++) {
		int index;
		ZeroVector3 p;
		ZeroQuaternion r;
		
		fread (&index, sizeof (int), 1, file); 
		fread (&p, sizeof (ZeroVector3), 1, file); 
		fread (&r, sizeof (ZeroQuaternion), 1, file); 
		mPositions[i].key = index;
		mRotations[i].key = index;
		(mPositions[i].position) = p;
		(mRotations[i].rotation) = r; 
	}
}

void CMSHBlockNodeAnimation::ReadSeparateKeys(FILE *file, CZeroData *pData)
{
	int i;
	char name[256];
	unsigned nodeNameCRC;
	
	fread (&nodeNameCRC, sizeof (int), 1, file); 
	strcpy (name, InvertCRC (nodeNameCRC));
	model = pData->FindModelByName(name);
	if(pData->GetAnimation()->GetVersion()==0)
	{
		CMSHBlockSkeleton *pSkel=pData->GetSkeleton();
		if(!pSkel->FindEffector(model))
		{
			model=pData->FindModelByName(model->GetParentName());
			_ASSERTE (pSkel->FindEffector(model));
		}
	}
	_ASSERTE (model);
	
	fread (&JointsCount, sizeof (int), 1, file); 
	fread (&mNumPositions, sizeof (int), 1, file); 
	fread (&mNumRotations, sizeof (int), 1, file); 
	
	for (i = 0; i < mNumPositions; i ++) {
		int index;
		ZeroVector3 p;
		
		fread (&index, sizeof (int), 1, file); 
		fread (&p, sizeof (ZeroVector3), 1, file); 
		mPositions[i].key = index;
		(mPositions[i].position) = p;
	}

	for (i = 0; i < mNumRotations; i ++) {
		int index;
		ZeroQuaternion r;
		
		fread (&index, sizeof (int), 1, file); 
		fread (&r, sizeof (ZeroQuaternion), 1, file); 
		mRotations[i].key = index;
		(mRotations[i].rotation) = r; 
	}

}

void CMSHBlockNodeAnimation::StoreModelCRC()
{
	m_ModelCRC = CalcStrCRC (model->GetName());
}

void CMSHBlockNodeAnimation::RecoverModel(CZeroData *data)
{
	model=data->FindModelByName(InvertCRC(m_ModelCRC));
}

void CMSHBlockNodeAnimation::CompressKeyFrames ()
{
#ifdef COMPRESS_KEYFRAMES


	int i;

	struct KeyFramesCompressor: public ZeroVisList<CMSHBlockKeyFrame>
	{
	
		KeyFramesCompressor (const CMSHBlockNodeAnimation& animation)
			:ZeroVisList<CMSHBlockKeyFrame> (false)
		{
			int i;
			int count;
			float positionTol;
			float rotationTol;
			ZeroListNode *node;
			ZeroListNode *node0;
			ZeroListNode *node1;
			
			count = animation.GetKeyframesCount();
			
			positionTol = 0;
			for (i = 0; i < count; i ++) {
				positionTol = GetMax (sqrtf (DotProduct (animation[i].position, animation[i].position)), 0);
				Append (animation[i]);
			}

			if (positionTol < 1.0e-4) {
				positionTol	= 1.0f;
			}
			rotationTol	= 0.25f * DEG2RAD;
			positionTol = positionTol / 5000.0f;
			
			node0 = GetFirst();
			node1 = GetLinearNode(node0, positionTol, rotationTol); 
			do {
				for (node = node0->GetNext(); node != node1; node = node0->GetNext()) {
					Remove (node);
				}
				node0 = node1;
				node1 = GetLinearNode(node1, positionTol, rotationTol); 
			} while (node1 != GetLast());
			
		}
		
		ZeroListNode *GetLinearNode(ZeroListNode *start, float positionTol, float rotationTol)
		{
			int steps;

			ZeroMatrix rotation (start->info.rotation, ZeroVector3 (0, 0, 0));
			
			ZeroVector3 p0 (start->info.position);
			ZeroVector3	e0 (rotation.CalcPitchYawRollEulerAngles());
			
			ZeroVector3 positAcc (p0);
			ZeroVector3 eulerAcc (e0);
			
			steps = 2;
			for (start = start->GetNext(); start; start = start->GetNext()) {

				if (start->info.mark) {
					return start;
				}

				ZeroMatrix rotation (start->info.rotation, ZeroVector3 (0, 0, 0));
				
				ZeroVector3 p1 (start->info.position);
				ZeroVector3 e1 (rotation.CalcPitchYawRollEulerAngles());
				
				positAcc += p1;
				eulerAcc += e1;
				
				ZeroVector3 pArea (p1 + p0);
				ZeroVector3 eArea (e1 + e0);
				
				pArea = pArea.Scale ((float)steps);
				eArea = eArea.Scale ((float)steps);
				
				ZeroVector3 delP (positAcc.Scale (2.0) - pArea);
				ZeroVector3 delE (eulerAcc.Scale (2.0) - eArea);
				
				if (fabsf (delP.x) > positionTol) {
					return start->GetPrev();
				}
				if (fabsf (delP.y) > positionTol) {
					return start->GetPrev();
				}
				if (fabsf (delP.z) > positionTol) {
					return start->GetPrev();
				}
				
				if (fabsf (delE.x) > rotationTol) {
					return start->GetPrev();
				}
				if (fabsf (delE.y) > rotationTol) {
					return start->GetPrev();
				}
				if (fabsf (delE.z) > rotationTol) {
					return start->GetPrev();
				}
				steps ++;
			}
			return GetLast();
		}
	};
	KeyFramesCompressor compressKeyFrames(*this);
	
	keyframesCount = compressKeyFrames.GetCount();
	
	i = 0;
	KeyFramesCompressor::Iterator iter (compressKeyFrames);
	for (iter.Begin(); iter; iter ++) {
		(*this)[i] = (*iter);
		i ++;
	}
#endif
}



