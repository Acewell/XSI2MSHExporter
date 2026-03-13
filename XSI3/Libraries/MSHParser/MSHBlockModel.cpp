#include "MSHBlockModel.h"
#include "MSHBlockConstraintList.h"
#include "MSHBlockMaterialSet.h"
#include "ZeroData.h"
#include <string.h>

/*----------------------------------------------------------------------------------------
CMSHBlockPolygonList
----------------------------------------------------------------------------------------*/


#define PREFIX_NAME                     "msh_"
//#define MAX_TRIANGLE_PER_SEGMENT        1024 * 8
//#define MAX_VERTEX_PER_SEGMENT_IN_BYTES 1024 * 128

#define MAX_TRIANGLE_PER_SEGMENT        1024 * 4
#define MAX_VERTEX_PER_SEGMENT_IN_BYTES 1024 * 64



#define EXPORT_ORIGINAL_POLYGONS
//#define EXPORT_TRIANGLE_INDEXLIST

//extern CZeroData * gZeroData;

void CMSHClothParametricCollisionList::AddParametricClothCollsion (
	CMSHBlockModel* collisonModel, 
	const ZeroMatrix& matrix, 
	const CMSHBlockModel* parentModel)
{
	CMSHClothParametricCollision data;
	MSH_ClothCollisionCone *collCone;
	MSH_ClothCollisionSphere *collSphere;
	MSH_ClothCollisionCylinder *collCylinder;

	data.parent = parentModel;
	//data.offsetMatrix = matrix;
	for ( int i = 0; i < 4; i++ ) {
		for ( int j = 0; j < 4; j++ ) {
			data.offsetMatrix[i][j] = matrix[i][j];
		}
  }


	collCone = NULL;
	collSphere = NULL;
	collCylinder = NULL;
	collisonModel->GetClothCollision(&collSphere, &collCylinder, &collCone);

	data.raddi0 = 0;
	data.raddi1 = 0;
	data.height = 0;

	if (collCone) {
		data.type = CMSHClothParametricCollision::CONE;
		data.raddi0 = collCone->radius1;
		data.raddi1 = collCone->radius2;
		data.height = collCone->height * 0.5f;
	} else if (collSphere) {
		data.type = CMSHClothParametricCollision::SPHERE;
		data.raddi0 = collSphere->radius;
	} else {
		data.type = CMSHClothParametricCollision::CYLINDER;
		data.raddi0 = collCylinder->radius;
		data.height = collCylinder->height * 0.5f;
	}

	Append (data);
}


void CMSHClothParametricCollisionList::WriteText (FILE *fp) const
{
	Iterator iter (*this);

	fprintf(fp,"\t\t\tCollisionsVolumes {%d}\n", GetCount());
	fprintf(fp,"\t\t\t{\n");
	for (iter.Begin(); iter; iter ++) {
		CMSHClothParametricCollision& volume = *iter;

		ZeroMatrix matrix;
    for ( int i = 0; i < 4; i++ )
      for ( int j = 0; j < 4; j++ )
        matrix[i][j] = volume.offsetMatrix[i][j];





		fprintf(fp,"\t\t\t\tparentIndex: %d\n", volume.parent->GetMatrixPaletteIndex());

		fprintf(fp,"\t\t\t\ttype: %d\n", volume.type);
		fprintf(fp,"\t\t\t\tradius0: %f\n", volume.raddi0);
		fprintf(fp,"\t\t\t\tradius1: %f\n", volume.raddi1);
		fprintf(fp,"\t\t\t\theight: %f\n", volume.height);
		fprintf(fp,"\t\t\t\tmatrix right: %f %f %f\n", matrix.right.x, matrix.right.y, matrix.right.z);
		fprintf(fp,"\t\t\t\tmatrix up:    %f %f %f\n", matrix.up.x,	 matrix.up.y,	  matrix.up.z);
		fprintf(fp,"\t\t\t\tmatrix front: %f %f %f\n", matrix.front.x, matrix.front.y, matrix.front.z);
		fprintf(fp,"\t\t\t\tmatrix posit: %f %f %f\n", matrix.posit.x, matrix.posit.y, matrix.posit.z);
		fprintf(fp,"\t\t\t\t\n");
	}
	fprintf(fp,"\t\t\t}\n");
}


void CMSHClothParametricCollisionList::Write (FILE *file) const
{
	int count;
	Iterator iter (*this);

	count = GetCount();
	WriteData(file, &count, sizeof(int));
	for (iter.Begin(); iter; iter ++) {
		CMSHClothParametricCollision& volume = *iter;
		volume.parentIndex = volume.parent->GetMatrixPaletteIndex(); 
		WriteData(file, &volume, sizeof(ZeroClothCollisionVolume));
	}
}



CMSHBlockVertexWeightsList::CMSHBlockVertexWeightsList()
	:ZeroVisList<ZeroMatrixWeightPair>()
{
}


void CMSHBlockVertexWeightsList::WriteText(FILE *fp)
{
	int id;
	float weight;
	char buffer[2048];
	ZeroHeap<int, float> boneHeap (5, 100.0f, buffer);
	
	_ASSERTE (GetCount() <= 4);
	
	Iterator iter (*this);
	for (iter.Begin (); iter; iter ++) {
		ZeroMatrixWeightPair &pair = (*iter);
		
		boneHeap.Push (pair.boneID, pair.weight);
	}
	
	while (boneHeap.GetCount() < 4) {
		int bone = 0;
		boneHeap.Push (bone, 0.0);
	}
	
	fprintf(fp,"\t\t\t");
	while (boneHeap.GetCount()) {
		id = boneHeap[0];
		weight = boneHeap.Value();
		boneHeap.Pop();
		fprintf(fp,"Bone{%ld, %f} ", id, weight);
	}
	fprintf(fp,"\n");
}


void CMSHBlockVertexWeightsList::Write(FILE *fp)
{
	int id;
	float weight;
	char buffer[2048];
	ZeroHeap<int, float> boneHeap (6, 500.0f, buffer);
	
	_ASSERTE (GetCount() <= 4);
	
	Iterator iter (*this);
	for (iter.Begin (); iter; iter ++) {
		ZeroMatrixWeightPair &pair = (*iter);
		
		boneHeap.Push (pair.boneID, pair.weight);
	}
	
	while (boneHeap.GetCount() < 4) {
		int bone = 0;
		boneHeap.Push (bone, 0.0);
	}
	
	while (boneHeap.GetCount()) {
		id = boneHeap[0];
		weight = boneHeap.Value();
		boneHeap.Pop();
		fwrite(&id, sizeof (int), 1, fp);
		fwrite(&weight, sizeof (float), 1, fp);
	}
}

void CMSHBlockVertexWeightsList::Read(FILE *fp)
{
	int i;
	int id;
	float weight;
	
	for (i = 0; i < 4; i ++) {
		fread(&id, sizeof (int), 1, fp);
		fread(&weight, sizeof (float), 1, fp);
		
		if (weight > 1.0e-3) {
			ZeroMatrixWeightPair pair (id, weight);
			Append (pair);
		}
	}
}



CMSHBlockSegmentEnvelop::CMSHBlockSegmentEnvelop(BlockHeader *pHdr,CZeroData *pData)
:CMSHBlock(pHdr,pData),
ZeroVisTree<CMSHBlockVertexWeightsList, int>()
{
//	m_StripIndicesArray = NULL;
//	m_nStripIndices = 0;
}


void CMSHBlockSegmentEnvelop::AddVertexWeight (
	unsigned vertexIndex, 
	unsigned boneID, 
	float weight)
{
	float minWeight;
	ZeroTreeNode *node;
	CMSHBlockVertexWeightsList* weightList;
	
	node = Find (vertexIndex);
	if (!node) {
		CMSHBlockVertexWeightsList weightList;
		node = Insert (weightList, vertexIndex);
	}
	
	weightList = &node->info;
	
#ifdef _DEBUG
/*
	CMSHBlockVertexWeightsList::Iterator xx(*weightList);
	for (xx.Begin(); xx; xx ++) {
		ZeroMatrixWeightPair pair (*xx);
		_ASSERTE (pair.boneID != (int) boneID);
	}
*/
#endif
	
	
	ZeroMatrixWeightPair pair (boneID, weight);
	if (weightList->GetCount() < 4) {
		minWeight = 0;
		if (weightList->GetCount()) {
			minWeight = weightList->GetLast()->info.weight;
		}
		
		if (weight < minWeight) {
			weightList->Append (pair);
		} else {
			weightList->Prepend (pair);
		}
	} else {
		minWeight = weightList->GetLast()->info.weight;
		if (weight > minWeight) {
			weightList->Remove(weightList->GetLast());
			weightList->Append (pair);
			
			CMSHBlockVertexWeightsList::Iterator iter (*weightList);
			CMSHBlockVertexWeightsList::ZeroListNode* smallest;
			
			smallest = NULL;
			minWeight = 1.0e4f;
			for (iter.Begin(); iter; iter ++) {
				weight = iter.GetNode()->info.weight;
				if (weight < minWeight) {
					minWeight = weight;
					smallest = iter.GetNode();
				}
			}
			weightList->RotateToEnd(smallest);
		}
	}
}


void CMSHBlockSegmentEnvelop::Read(FILE *fp)
{
	int i;
	int count;
	ZeroTreeNode *node;
	
	ReadData(fp, &count, sizeof(int));
	for (i = 0; i < count; i++) {
		CMSHBlockVertexWeightsList weightList;
		node = Insert (weightList, i);
		node->info.Read (fp);
	}
}


void CMSHBlockSegmentEnvelop::Write(FILE *fp)
{
	int count;
	int vertex;
	int prevVertex;
	
//return;

	StartBlock(fp,'WGHT');
	
	prevVertex = -1;
	Iterator iter (*this);
	
	count = GetCount();
	fwrite (&count, sizeof (int), 1, fp);
	for (iter.Begin(); iter; iter ++) {
		vertex = (*iter)->key;
		if ((vertex - prevVertex) != 1) {
			_ASSERTE (0);
		} else {
			(*iter)->info.Write(fp);
		}
		prevVertex = vertex;
	}
	EndBlock(fp);

}

void CMSHBlockSegmentEnvelop::WriteText(FILE *fp)
{
	int vertex;
	int prevVertex;

//return;
	fprintf(fp,"\t\t\tnumWeights {%d}\n", GetCount());
	fprintf(fp,"\t\t\tVertexWeights {\n");

	
	prevVertex = -1;
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		vertex = (*iter)->key;
		if ((vertex - prevVertex) != 1) {
			_ASSERTE (0);
		} else {
			(*iter)->info.WriteText(fp);
		}
		prevVertex = vertex;
	}
	fprintf(fp,"\t\t\t}\n");
}


/*----------------------------------------------------------------------------------------
CMSHBlockTriangleList
----------------------------------------------------------------------------------------*/
CMSHBlockTriangleList::CMSHBlockTriangleList (
	BlockHeader *pHdr,
	CZeroData *pData,
	int triangleCount, 
	unsigned *indexList)
	:CMSHBlock(pHdr,pData)
{
	int i;
	m_nTriangleCount = triangleCount;
	m_nTriangleIndices = new unsigned short [triangleCount * 3];
	
	for (i = 0; i < m_nTriangleCount * 3; i ++) {
		m_nTriangleIndices[i] = (unsigned short)indexList[i];
	}
}

CMSHBlockTriangleList::~CMSHBlockTriangleList ()
{
	delete[] m_nTriangleIndices;
}


void CMSHBlockTriangleList::Read(FILE *fp)
{
	fp;
}

void CMSHBlockTriangleList::Write(FILE *file)
{
	StartBlock(file, 'NDXT');
	
	WriteData(file, &m_nTriangleCount, sizeof(int));
	WriteData(file, m_nTriangleIndices, m_nTriangleCount * 3 * sizeof(unsigned short));
	EndBlock(file);
}

void CMSHBlockTriangleList::WriteText(FILE *fp)
{
	int i;
	int i0;
	int i1;
	int i2;
	
	fprintf(fp, "\t\t\tNumTriangles{%d}\n", m_nTriangleCount);
	fprintf(fp,"\t\t\tTriangles\n\t\t\t{\n");
	for (i = 0; i < m_nTriangleCount; i ++) {
		i0 = m_nTriangleIndices[i * 3 + 0];
		i1 = m_nTriangleIndices[i * 3 + 1];
		i2 = m_nTriangleIndices[i * 3 + 2];
		fprintf(fp,"\t\t\t\t%d %d %d\n", i0, i1, i2);
	}                            
	fprintf(fp,"\t\t\t}\n");
}



/*----------------------------------------------------------------------------------------
CMSHBlockTriangleStripList
----------------------------------------------------------------------------------------*/

int CMSHBlockTriangleStripList::maxStripLength = 0;
int CMSHBlockTriangleStripList::minStripLength = 1000000;
int CMSHBlockTriangleStripList::avgStripLength = 0;
int CMSHBlockTriangleStripList::numStrips = 0;
char CMSHBlockTriangleStripList::stripLengthOutput[5000] = {0};

CMSHBlockTriangleStripList::CMSHBlockTriangleStripList (
	BlockHeader *pHdr,
	CZeroData *pData,
	int indexCount, 
	unsigned *indexList)
	:CMSHBlock(pHdr,pData)
{

	numStrips = 0;

	int i;
	int index;
	m_nIndexCount = indexCount;
	m_nStripIndices = new unsigned short [indexCount];

	
	for (i = 0; i < m_nIndexCount; i ++) {
		short smallIndex;
		index = indexList[i];
		smallIndex = (unsigned short)(index & 0x7fff);
		if (index & 0x80000000) {
			smallIndex |= (unsigned short)0x8000;
		}
		m_nStripIndices[i] = smallIndex;
	}
}

CMSHBlockTriangleStripList::~CMSHBlockTriangleStripList ()
{
	delete[] m_nStripIndices;
}



void CMSHBlockTriangleStripList::Read(FILE *fp)
{
	fp;
}

void CMSHBlockTriangleStripList::Write(FILE *file)
{
	if(m_nIndexCount)
	{
		StartBlock(file, 'STRP');
	
		WriteData(file, &m_nIndexCount, sizeof(int));
		WriteData(file, m_nStripIndices, m_nIndexCount * sizeof(unsigned short));
		EndBlock(file);

		// below is for debug tristrip output info, by Mike Z
		if(numStrips == 0) // reset values because DLL might not have been unloaded
		{
			stripLengthOutput[0] = 0;
			maxStripLength = 0;
			minStripLength = 1000000;
			avgStripLength = 0;
		}

		if(m_nIndexCount > maxStripLength)
			maxStripLength = m_nIndexCount;	
		if(m_nIndexCount < minStripLength)
		minStripLength = m_nIndexCount;
		avgStripLength += m_nIndexCount;
		numStrips++;

		char info[200];
		
		sprintf(info, "Strip %d\nLength %d\n\n", CMSHBlockTriangleStripList::numStrips, m_nIndexCount);

		strncat(stripLengthOutput, info, sizeof(stripLengthOutput));
	}
}

void CMSHBlockTriangleStripList::WriteText(FILE *fp)
{

	int i;
	unsigned short index;
	
	fprintf(fp, "\t\t\tNumIndices{%d}\n", m_nIndexCount);
	fprintf(fp,"\t\t\tTrianglesIndexStrips\n\t\t\t{\n");
	for (i = 0; i < m_nIndexCount; i ++) {
		index = m_nStripIndices[i];
		if (index & 0x8000) {
			fprintf(fp,"\t\t\t\t%d stripStart\n", index & 0x7fff);
		} else {
			fprintf(fp,"\t\t\t\t%d\n", index & 0x7fff);
		}
	}                            
	fprintf(fp,"\t\t\t}\n");
}






/*----------------------------------------------------------------------------------------
CMSHBlockPolygonList
----------------------------------------------------------------------------------------*/
CMSHBlockPolygonList::CMSHBlockPolygonList(BlockHeader *pHdr,CZeroData *pData)
:CMSHBlock(pHdr,pData), ZeroVisTree<MSHBLOCKPolygon, int>()
{
}


int CMSHBlockPolygonList::GetTriangleCount()
{
	int count;
	
	count = 0;
	Iterator iter (*this);
	for (iter.Begin(); iter; iter++) {
		MSHBLOCKPolygon& polygon = (*iter)->info;
		count += polygon.GetCount() - 2;
	}
	return count;
}


void CMSHBlockPolygonList::Read(FILE *file)
{
	int i;
	int j;
	int polyCount;
	unsigned short index;
	unsigned short indexCount;
	unsigned int blockSize;
	
	blockSize = m_Header.size;
	
	ReadData(file, &polyCount,sizeof(int));
	blockSize -= sizeof(int);
	
	for (i =0 ; i < polyCount; i++) {
		ReadData(file, &indexCount, sizeof(unsigned short));
		blockSize-=sizeof(unsigned short);
		for (j = 0; j < indexCount; j++) {
			ReadData(file, &index, sizeof(unsigned short));
			blockSize -= sizeof(unsigned short);
			SetIndex (i, (unsigned short)j, index);
		}
	}
	fseek(file, blockSize, SEEK_CUR);
}

void CMSHBlockPolygonList::Write(FILE *file)
{
	int polyCount;

	StartBlock(file, 'NDXL');
	
	polyCount = GetCount();
	WriteData(file, &polyCount, sizeof(int));
	
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		unsigned short indexCount;
		
		indexCount = (unsigned short) (*iter)->info.GetCount();
		WriteData(file, &indexCount, sizeof(unsigned short));
		
		MSHBLOCKPolygon::Iterator polyIter ((*iter)->info);
		for (polyIter.Begin(); polyIter; polyIter ++) {
			unsigned short index;
			index = (unsigned short)*polyIter;
			WriteData(file, &index, sizeof(unsigned short));
		}
	}
	EndBlock(file);
}

void CMSHBlockPolygonList::WriteText(FILE *fp)
{
	fprintf(fp,"\t\t\tNumPolygons{%d}\n", GetCount());
	fprintf(fp,"\t\t\tPolygons\n\t\t\t{\n");
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		fprintf(fp,"\t\t\t\t(%d: ", (*iter)->info.GetCount());
		MSHBLOCKPolygon::Iterator polyIter ((*iter)->info);
		for (polyIter.Begin(); polyIter; polyIter ++) {
			fprintf(fp,"%d,", *polyIter);
		}
		fprintf(fp,")\n");
	}
	fprintf(fp,"\t\t\t}\n");
}


void CMSHBlockPolygonList::SetIndex(
	int polyIndex, 
	unsigned short vertex, 
	unsigned short indexValue)
{
	CMSHBlockPolygonList::ZeroTreeNode *node;
	
	node = Find (polyIndex);
	if (!node) {
		MSHBLOCKPolygon polygon;
		node = Insert (polygon, polyIndex);
	}
	node->info.SetIndex(vertex, indexValue);
}

void CMSHBlockPolygonList::AddPoly(unsigned short nIndices, unsigned short *pIndices)
{
	_ASSERTE (0);
	/*
	unsigned short *new_nPolyIndices;
	unsigned short **new_ppPolyIndices;
	
	  new_nPolyIndices=new unsigned short[m_nPolyCount+1];
	  new_ppPolyIndices = new unsigned short *[m_nPolyCount+1];
	  if(m_nPolyCount)
	  {
	  memcpy(new_nPolyIndices,m_nPolyIndices,m_nPolyCount*sizeof(unsigned short));
	  memcpy(new_ppPolyIndices,m_ppPolyIndices,m_nPolyCount*sizeof(unsigned short *));
	  
		 delete[] m_nPolyIndices;
		 delete[] m_ppPolyIndices;
		 }
		 new_nPolyIndices[m_nPolyCount] = nIndices;
		 new_ppPolyIndices[m_nPolyCount] = new unsigned short[nIndices];
		 memcpy(new_ppPolyIndices[m_nPolyCount],pIndices,nIndices*sizeof(unsigned short));
		 m_nPolyCount++;
		 m_nPolyIndices = new_nPolyIndices;
		 m_ppPolyIndices = new_ppPolyIndices;
	*/
}


int CMSHBlockPolygonList::TriangleList (unsigned faces[], const vect3 vertex[])
{
	int indexCount;
	int triangleCount;
	
	ZeroPolyhedra polyhedra;
	polyhedra.BeginFace();
	
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		MSHBLOCKPolygon& polygon = (*iter)->info;
		ZeroStack<int> index(polygon.GetCount());
		
		indexCount = 0;
		MSHBLOCKPolygon::Iterator polyIter (polygon);
		for (polyIter.Begin(); polyIter; polyIter ++) {
			index[indexCount] = *polyIter;
			indexCount ++;
		}
		polyhedra.AddFace (indexCount, &index[0]);
		
	}
	polyhedra.EndFace();
	
	triangleCount = polyhedra.WireMeshing ((ZeroFace*)faces, &vertex[0].fX, sizeof (vect3));
	
	return triangleCount * 3;
}


int CMSHBlockPolygonList::TriangleStrip (unsigned indexOut[], const vect3 vertex[])
{
	int i;
	int j;
	int index;
	int count;
	int indexCount;
	int stripIndexCount;
	
	ZeroPolyhedra polyhedra;
	polyhedra.BeginFace();
	
	indexCount = 0;
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		MSHBLOCKPolygon& polygon = (*iter)->info;
		ZeroStack<int> index(polygon.GetCount());
		
		indexCount = 0;
		MSHBLOCKPolygon::Iterator polyIter (polygon);
		for (polyIter.Begin(); polyIter; polyIter ++) {
			index[indexCount] = *polyIter;
			indexCount ++;
		}
		polyhedra.AddFace (indexCount, &index[0]);
		
	}
	polyhedra.EndFace();

	polyhedra.OptimalTriangulation (&vertex[0].fX, sizeof (vect3));

	ZeroStack<unsigned> indexStripsPool(1024 * 32);
	indexCount = polyhedra.TriangleStrips(&indexStripsPool[0], 1024 * 32);


int xxxx;
xxxx = 0;	
	// create index map for soft body segment
	stripIndexCount = 0;
	for (i =0 ; i < indexCount;) {
		count = indexStripsPool[i];
		_ASSERTE (count >= 3);

		index = indexStripsPool[i + 1 + 0];
		indexOut[stripIndexCount] = index | 0x80000000;
		stripIndexCount++;

		index = indexStripsPool[i + 1 + 1];
		indexOut[stripIndexCount] = index | 0x80000000;
		stripIndexCount++;

		for(j = 2 ; j < count; j++) 	{
			index = indexStripsPool[i + 1 + j];
			indexOut[stripIndexCount] = index;
			stripIndexCount++;
		}
		i += count + 1;

xxxx ++;
//if (xxxx == 32)
//break;
	}

	return stripIndexCount;
}


/*----------------------------------------------------------------------------------------
CMSHBlockModelSegment
----------------------------------------------------------------------------------------*/
CMSHBlockModelSegment::CMSHBlockModelSegment(BlockHeader *pHdr,CZeroData *pData,CMSHBlockModel *myModel):
CMSHBlock(pHdr,pData), envelop (pHdr, pData), m_pPolygonList (pHdr, pData)
{
	m_pMyModel=myModel;
	maxFaceCount = 0;
	maxVetexCount = 0;
	userFlags = 0;
	
	m_shadow = NULL;
	m_sMaterialName=NULL;

	
	m_nPositions=0;
	m_v3Positions=NULL;
	
	m_nNormals=0;
	m_v3Normals=NULL;
	
	m_nColors=0;
	m_rgbaColors=NULL;
	m_rgbaColorBase.cR=m_rgbaColorBase.cG=m_rgbaColorBase.cB=m_rgbaColorBase.cA=255;
	
	for(int i=0;i<MAX_TEXTURES_PER_MSH_MATERIAL;i++)
	{
		m_nUVCoordinates[i]=0;
		m_UVCoordinates[i]=NULL;
	}
	
	m_nLightColors=0;
	m_rgbaLightColors=NULL;
	m_rgbaLightColorBase.cR=m_rgbaLightColorBase.cG=m_rgbaLightColorBase.cB=m_rgbaLightColorBase.cA=255;
	
	m_nAttributes=0;
	m_pAttributes=NULL;
	m_AttributeBase=0;
	
	m_pTriangleList = NULL;
	m_pTriangleStrips = NULL;

	
	//	m_HasBBox=false;
	m_HasColorBase = false;
	m_HasLightColorBase = false;
}



CMSHBlockModelSegment::CMSHBlockModelSegment( 
	const CMSHBlockModelSegment& me, 
	const ZeroPolyhedra& polyhedra,CMSHBlockModel *myModel)
	:CMSHBlock (me), envelop (me.envelop), m_pPolygonList (me.m_pPolygonList)
{
	int mark;
	int index;
	int polyCount;
	int vertexCount;
	int i;
	
	ZeroEdge *ptr;
	ZeroEdge *edge;

	userFlags = me.userFlags;
	
	m_pMyModel = myModel;
	maxFaceCount = me.maxFaceCount;
	maxVetexCount = me.maxVetexCount;
	m_sMaterialName=NULL;
	
	m_nPositions=0;
	m_v3Positions=NULL;
	
	m_nNormals=0;
	m_v3Normals=NULL;
	
	m_nColors=0;
	m_rgbaColors=NULL;
	
	for(i=0;i<MAX_TEXTURES_PER_MSH_MATERIAL;i++)
	{
		m_nUVCoordinates[i]=0;
		m_UVCoordinates[i]=NULL;
	}
	
	m_nLightColors=0;
	m_rgbaLightColors = NULL;
	
	m_nAttributes = 0;
	m_pAttributes = NULL;
	
	m_pTriangleList = NULL;
	m_pTriangleStrips = NULL;

	
	
	ZeroStack<int> indexMap (me.m_nPositions);
	memset (&indexMap[0], 0xff, sizeof (int) * me.m_nPositions);
	
	
	// remap face indices
	vertexCount = 0;
	ZeroPolyhedra::Iterator iter (polyhedra);
	for (iter.Begin(); iter; iter ++) {
		edge = &(*iter)->info;
		if (indexMap[edge->incidentVertex] == -1) {
			indexMap[edge->incidentVertex] = vertexCount;
			vertexCount ++;
		}
	}
	
	// copy faces
	polyCount = 0;
	mark = polyhedra.IncLRU();
	for (iter.Begin(); iter; iter ++) {
		edge = &(*iter)->info;
		if (edge->mark != mark) {
			if (edge->incidentFace >= 0) {
				index = 0;
				ptr = edge;
				do {
					int i;
					i = indexMap[ptr->incidentVertex];
					m_pPolygonList.SetIndex(polyCount, (unsigned short)index, (unsigned short)i);
					index ++;
					ptr->mark = mark;
					ptr = ptr->next;
				} while (ptr != edge);
				polyCount ++;
			}
		}
	}
	
	vect3 *positions;
	positions = me.GetPositions(index);
	SetNumPositions (vertexCount);
	
	for (iter.Begin(); iter; iter ++) {
		edge = &(*iter)->info;
		index = edge->incidentVertex;
		
		vect3 p (positions[index]);
		index = indexMap[index];
		SetPosition (index, p.fX, p.fY, p.fZ);
	}
	
	
	mark = polyhedra.IncLRU();
	if (me.envelop.GetCount()) {
		ZeroStack<char> markEntry (me.m_nPositions);
		memset (&markEntry[0], 0xff, sizeof (char) * me.m_nPositions);
		
		for (iter.Begin(); iter; iter ++) {
			edge = &(*iter)->info;
			if (edge->mark != mark) {
				ptr = edge;
				do {
					ptr->mark = mark;
					ptr = ptr->twin->next;
				} while (ptr != edge);
				
				
				index = edge->incidentVertex;
				CMSHBlockVertexWeightsList::Iterator iter(me.envelop.Find(index)->info);
				
				index = indexMap[index];
				if (markEntry[index]) {
					markEntry[index] = 0;
					for (iter.Begin(); iter; iter ++) {
						ZeroMatrixWeightPair& pair = *iter;
						envelop.AddVertexWeight (index, pair.boneID, pair.weight);
					}
				}
			}
		}
	}
	
	
	vect3 *normals;
	normals = me.GetNormals(index);
	if (normals) { 
		SetNumNormals (vertexCount);
		for (iter.Begin(); iter; iter ++) {
			edge = &(*iter)->info;
			
			index = edge->incidentVertex;
			
			vect3 n (normals[index]);
			
			index = indexMap[index];
    	    SetNormal (index, n.fX, n.fY, n.fZ);
		}
	}
	
	uv *uvs;
	for(i=0;i<MAX_TEXTURES_PER_MSH_MATERIAL;i++)
	{
		uvs = me.GetUVCoords(index,i);
		if (uvs) { 
			SetNumUVs (vertexCount,i);
			for (iter.Begin(); iter; iter ++) {
				edge = &(*iter)->info;
				
				index = edge->incidentVertex;
				
				uv n (uvs[index]);
				
				index = indexMap[index];
    			SetUV (index, n.fU, n.fV,i);
			}
		}
	}
	

	unsigned int *attributes;
	attributes = me.GetAttributes(index);
	if (attributes) { 
		SetNumAttributes (vertexCount);
		for (iter.Begin(); iter; iter ++) {
			edge = &(*iter)->info;
			
			index = edge->incidentVertex;
			
			unsigned int a (attributes[index]);
			
			index = indexMap[index];
    	    SetAttribute (index, a);
		}
	}
	
	rgba *colors;
	colors = me.GetColors(index);
	if (colors) { 
		SetNumColors (vertexCount);
		for (iter.Begin(); iter; iter ++) {
			edge = &(*iter)->info;
			
			index = edge->incidentVertex;

			unsigned int c = ConvertRGBAToLong(&colors[index]);

			index = indexMap[index];

    	    SetColor (index, c);
		}
	}
	
	colors = me.GetLightColors(index);
	if (colors) { 
		SetNumLightColors (vertexCount);
		for (iter.Begin(); iter; iter ++) {
			edge = &(*iter)->info;
			
			index = edge->incidentVertex;
			
			rgba c(colors[index]);
			
			index = indexMap[index];
    	    SetLightColor (index, c.cR, c.cG, c.cB, c.cA);
		}
	}
	
	m_HasColorBase = me.m_HasColorBase;
	m_rgbaColorBase = me.m_rgbaColorBase;
	
	m_HasLightColorBase = me.m_HasLightColorBase;
	m_rgbaLightColorBase = me.m_rgbaLightColorBase;
	
	m_AttributeBase = me.m_AttributeBase;
	
	SetMaterialName( me.GetMaterialName());




	m_shadow	= NULL;
	if (me.m_shadow) {
		m_shadow = new CMSHShadowVolume (*me.m_shadow);
		m_pData->SetHasShadowVolumes(true);
	}
}


CMSHBlockModelSegment::~CMSHBlockModelSegment()
{
	if (m_shadow) {
		delete m_shadow;
	}

	if(m_sMaterialName)
		delete[] m_sMaterialName;
	if(m_v3Positions)
		delete[] m_v3Positions;
	if(m_v3Normals)
		delete[] m_v3Normals;
	if(m_rgbaColors)
		delete[] m_rgbaColors;

	for(int i=0;i<MAX_TEXTURES_PER_MSH_MATERIAL;i++)
	{
		if(m_UVCoordinates[i])
		delete[] m_UVCoordinates[i];
	}

	if(m_rgbaLightColors)
		delete[] m_rgbaLightColors;
	if(m_pAttributes)
		delete[] m_pAttributes;
	
	if (m_pTriangleList) { 
		delete m_pTriangleList;
	}

	if (m_pTriangleStrips) {
		delete m_pTriangleStrips;
	}

}

void CMSHBlockModelSegment::Read(FILE *file)
{
	int materialIndex;
	int i;
	unsigned int blockSize = m_Header.size;
	CMSHBlockMaterial *pMaterial;
	bool bSkipPolySection=false;

	char *sTextureName=NULL;
	char *sLightMapName=NULL;
	unsigned int color;
	while (blockSize)	{
		BlockHeader materialheader;
		unsigned int materialblockSize;
		
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
			case 'WDHS':
				{
					m_pData->SetHasShadowVolumes(true);
					int vertexCount;
					int edgeCount;
					ReadData(file,&vertexCount, sizeof(int));
					ZeroStack<vect3> vArray(vertexCount);
					for(i=0 ; i < vertexCount; i++)	
					{
						ReadData(file,&(vArray[i].fX),sizeof(float));
						ReadData(file,&(vArray[i].fY),sizeof(float));
						ReadData(file,&(vArray[i].fZ),sizeof(float));
					}

					ReadData(file,&edgeCount, sizeof(int));
					if(m_shadow)
						delete m_shadow;
					m_shadow	= new CMSHShadowVolume (vertexCount, edgeCount);
					memcpy(&m_shadow->vertexArray[0].fX,&vArray[0].fX,vertexCount*sizeof(vect3));
					for(i=0 ; i<m_shadow->edgeCount; i++)	
					{
						ReadData(file,&(m_shadow->edgeList[i].index), sizeof (ZeroMshShadowEdge));
					}
				}
				break;

    		case 'NTAM':
				ReadString(file, header.size,&m_sMaterialName);
				break;
			case 'ITAM':
				ReadData(file, &materialIndex, sizeof(int));
				pMaterial = m_pData ->GetMaterialSet() ->GetMaterialByIndex(materialIndex);
				if(pMaterial)
				{
					if(m_sMaterialName)
						delete[] m_sMaterialName;
					m_sMaterialName = new char[strlen(pMaterial ->GetName())+1];
					strcpy(m_sMaterialName,pMaterial ->GetName());
				}
				break;
			case 'DTAM':	// [COMPATABILITY]
				materialblockSize=header.size;
				while (materialblockSize)
				{
					// read a block header
					ReadHeader(file,&materialheader);
					
					// deduct blockSize
					_ASSERTE(materialblockSize >= sizeof(BlockHeader) + materialheader.size);
					materialblockSize -= sizeof(BlockHeader);
					materialblockSize -= materialheader.size;
					
					// parse the block
					switch (materialheader.type)
					{
					case 'D0XT':
						if(sTextureName)
							delete[] sTextureName;
						ReadString(file, materialheader.size,&sTextureName);
						break;
					case 'D1XT':
						if(sLightMapName)
							delete[] sLightMapName;
						ReadString(file, materialheader.size,&sLightMapName);
						break;
					default:
						SkipBlock(file, materialheader.size);
						break;
						break;
					}
				}
				break;
			case 'XOBB':
				_ASSERTE (0);
				//    			m_HasBBox=true;
				//    			ReadData(file,&m_fBBoxRX,sizeof(float));
				//    			ReadData(file,&m_fBBoxRY,sizeof(float));
				//    			ReadData(file,&m_fBBoxRZ,sizeof(float));
				//    			ReadData(file,&m_fBBoxRW,sizeof(float));
				//    			ReadData(file,&m_fBBoxTX,sizeof(float));
				//    			ReadData(file,&m_fBBoxTY,sizeof(float));
				//    			ReadData(file,&m_fBBoxTZ,sizeof(float));
				//    			ReadData(file,&m_fBBoxDX,sizeof(float));
				//    			ReadData(file,&m_fBBoxDY,sizeof(float));
				//    			ReadData(file,&m_fBBoxDZ,sizeof(float));
				//    			ReadData(file,&m_fBBoxDW,sizeof(float));
				SkipBlock(file, header.size);
				break;
			case 'LSOP':
				ReadData(file,&m_nPositions,sizeof(int));
				m_v3Positions = new vect3[m_nPositions];
				if(header.size==sizeof(int)+3*m_nPositions*sizeof(float))
				{
					for(i=0;i<m_nPositions;i++)
					{
						ReadData(file,&(m_v3Positions[i].fX),sizeof(float));
						ReadData(file,&(m_v3Positions[i].fY),sizeof(float));
						ReadData(file,&(m_v3Positions[i].fZ),sizeof(float));
					}
				}
				else
				{
					if(header.size==sizeof(int)+3*m_nPositions*sizeof(float)+m_nPositions*sizeof(unsigned int))
					{
						bSkipPolySection=true;
						int triCount,nTris,flagCount;
						unsigned int temp;
						triCount=0;
						flagCount=0;
						nTris=0;
						for(i=0;i<m_nPositions;i++)
						{
							ReadData(file,&(m_v3Positions[i].fX),sizeof(float));
							ReadData(file,&(m_v3Positions[i].fY),sizeof(float));
							ReadData(file,&(m_v3Positions[i].fZ),sizeof(float));
							ReadData(file,&(temp),sizeof(unsigned int));
							if(temp)
							{
								if(flagCount==3)
								{
									flagCount=1;
									triCount=0;
								}
								else
								{
									flagCount++;
								}
							}
							else
							{
								if(flagCount==2)
									flagCount=3;
							}

							if(flagCount==3)
							{
								if(triCount%2==0)
								{
									m_pPolygonList.SetIndex(nTris,(unsigned short)0,(unsigned short)(i-2));
									m_pPolygonList.SetIndex(nTris,(unsigned short)1,(unsigned short)(i-1));
									m_pPolygonList.SetIndex(nTris,(unsigned short)2,(unsigned short)i);
									nTris++;
								}
								else
								{
									m_pPolygonList.SetIndex(nTris,(unsigned short)0,(unsigned short)(i-1));
									m_pPolygonList.SetIndex(nTris,(unsigned short)1,(unsigned short)(i-2));
									m_pPolygonList.SetIndex(nTris,(unsigned short)2,(unsigned short)i);
									nTris++;
								}
								triCount++;
							}
						}
					}
				}
				break;
				
			case 'THGW':
				envelop.Read(file);
				break;
				
			case 'LMRN':
				ReadData(file,&m_nNormals,sizeof(int));
				m_v3Normals = new vect3[m_nNormals];
				for(i=0;i<m_nNormals;i++)
				{
					ReadData(file,&(m_v3Normals[i].fX),sizeof(float));
					ReadData(file,&(m_v3Normals[i].fY),sizeof(float));
					ReadData(file,&(m_v3Normals[i].fZ),sizeof(float));
				}
				break;
			case 'BRLC':
				m_HasColorBase = true;
				ReadData(file,&(color),sizeof(unsigned int));
				ConvertLongToRGBA(color,&m_rgbaColorBase);
				break;
			case 'LRLC':
				ReadData(file,&m_nColors,sizeof(int));
				m_rgbaColors = new rgba[m_nColors];
				for(i=0;i<m_nColors;i++)
				{
					ReadData(file,&(color),sizeof(unsigned int));
					ConvertLongToRGBA(color,&(m_rgbaColors[i]));
				}
				break;
			case 'BTLV':
				m_HasLightColorBase = true;
				ReadData(file,&(color),sizeof(unsigned int));
				ConvertLongToRGBA(color,&m_rgbaLightColorBase);
				break;
			case 'LTLV':
				ReadData(file,&m_nLightColors,sizeof(int));
				m_rgbaLightColors = new rgba[m_nLightColors];
				for(i=0;i<m_nLightColors;i++)
				{
					ReadData(file,&(color),sizeof(unsigned int));
					ConvertLongToRGBA(color,&(m_rgbaLightColors[i]));
				}
				break;
			case 'BRTA':
				ReadData(file,&(m_AttributeBase),sizeof(unsigned int));
				break;
			case 'LRTA':
				ReadData(file,&m_nAttributes,sizeof(int));
				m_pAttributes = new unsigned int[m_nAttributes];
				for(i=0;i<m_nAttributes;i++)
					ReadData(file,&(m_pAttributes[i]),sizeof(unsigned int));
				break;
			case 'LCXT':
			case 'L0VU': 
				ReadData(file,&m_nUVCoordinates[0],sizeof(int));
				m_UVCoordinates[0] = new uv[m_nUVCoordinates[0]];
				for(i=0;i<m_nUVCoordinates[0];i++)
				{
					ReadData(file,&(m_UVCoordinates[0][i].fU),sizeof(float));
					ReadData(file,&(m_UVCoordinates[0][i].fV),sizeof(float));
				}
				break;
			case 'L1VU': 
			case 'L2VU': 
			case 'L3VU': 
			case 'L4VU': 
			case 'L5VU': 
			case 'L6VU': 
			case 'L7VU':
				{
					unsigned int idx=((0x00FF0000 & header.type) >> 16)-48;
					ReadData(file,&m_nUVCoordinates[idx],sizeof(int));
					m_UVCoordinates[idx] = new uv[m_nUVCoordinates[idx]];
					for(i=0;i<m_nUVCoordinates[idx];i++)
					{
						ReadData(file,&(m_UVCoordinates[idx][i].fU),sizeof(float));
						ReadData(file,&(m_UVCoordinates[idx][i].fV),sizeof(float));
					}
				}
				break;

			case 'LXDN':
				if(bSkipPolySection)
				{
					SkipBlock(file, header.size);
				}
				else
				{
					m_pPolygonList.SetHeader(&header);
					m_pPolygonList.Read(file);
				}
				break;

			default:
				SkipBlock(file, header.size);
				break;
		}
	}
	if(sTextureName)
	{
		m_pData ->AddTextureMaterial(sTextureName,&m_sMaterialName);
		if(sTextureName)
			delete[] sTextureName;
	}
}

void CMSHBlockModelSegment::CalculateLightColorBase()
{
	if(m_HasLightColorBase)
		return;
	
	if(m_nLightColors)
	{
		bool bDoIt=true;
		for(int i =1;((i<m_nLightColors) && (bDoIt));i++)
		{
			if((m_rgbaLightColors[i].cR!=m_rgbaLightColors[0].cR) ||
				(m_rgbaLightColors[i].cG!=m_rgbaLightColors[0].cG) ||
				(m_rgbaLightColors[i].cB!=m_rgbaLightColors[0].cB) ||
				(m_rgbaLightColors[i].cA!=m_rgbaLightColors[0].cA))
				bDoIt = false;
		}
		
		if(bDoIt)
			SetLightColorBase(m_rgbaLightColors[0].cR,m_rgbaLightColors[0].cG,m_rgbaLightColors[0].cB,m_rgbaLightColors[0].cA);
	}
}


//void CMSHBlockModelSegment::WriteShadowVolumeInfo (FILE *file)
//{
//
//}

/*

void CMSHBlockModelSegment::ParseCloth (FILE *file)
{
	_ASSERTE (0);
//	if (m_softVertexIndex) {
//		delete[] m_softVertexIndex;
//	}
//	if (m_softVertexPositions) {
//		delete [] m_softVertexPositions;
//	}
//
//
//	if (m_softVertexIndexMap) {
//		delete[] m_softVertexIndexMap;
//	}
//
//
//	ReadData (file, &m_softVertexIndexCount, sizeof (int));
//	m_softVertexIndex = new int[m_softVertexIndexCount];
//	ReadData (file, m_softVertexIndex, sizeof (int) *m_softVertexIndexCount);
//
//	ReadData (file, &m_softVertexPositionsCount, sizeof (int));
//	m_softVertexPositions = new SoftVertex[m_softVertexPositionsCount];
//	ReadData (file, m_softVertexPositions, sizeof (SoftVertex) * m_softVertexPositionsCount);
//
//	ReadData (file, &m_softIndexMapCount, sizeof (int));
//	m_softVertexIndexMap = new int[m_softIndexMapCount];
//	ReadData (file, m_softVertexIndexMap, sizeof (int) * m_softIndexMapCount);
}
*/


void CMSHBlockModelSegment::SetVertexData (
	ZeroPolyhedra& polyhedra, 
	const MSH_FLATVERTEX array[],
	bool hasNormal,
	bool hasColor,
	bool hasLight,
	bool hasWeight,
	bool hasUVs[]
	)
{
	int i;
	int index;
	int mark;
	int polyCount;
	int vertexCount;
	ZeroEdge *ptr;
	ZeroEdge *edge;

	
	// copy faces
	polyCount = 0;
	vertexCount = 0;

	m_pPolygonList.RemoveAll();

	mark = polyhedra.IncLRU();
	ZeroPolyhedra::Iterator iter (polyhedra);
	for (iter.Begin(); iter; iter ++) {
		edge = &(*iter)->info;
		if (edge->mark != mark) {
			if (edge->incidentFace >= 0) {
				index = 0;
				ptr = edge;
				do {
					m_pPolygonList.SetIndex(polyCount, (unsigned short)index, (unsigned short)ptr->incidentVertex);
					index ++;
					ptr->mark = mark;
					if (ptr->incidentVertex	>= vertexCount) {
						vertexCount	= ptr->incidentVertex + 1;
					}
					ptr = ptr->next;
				} while (ptr != edge);
				polyCount ++;
			}
		}
	}

	SetNumPositions (vertexCount);
	if (hasNormal) {
		SetNumNormals (vertexCount);
	}

	if (hasColor) {
		SetNumColors (vertexCount);
	}

	if (hasLight) {
		SetNumLightColors (vertexCount);
	}

	for(i=0;i<MAX_TEXTURES_PER_MSH_MATERIAL;i++)
	{
		if (hasUVs[i]) 
		{
			SetNumUVs(vertexCount, i);
		}
	}

	if (hasWeight) {
		envelop.RemoveAll();
	}


	for (i = 0; i < m_nPositions; i ++) {
		m_v3Positions[i].fX = array[i].vertex.x; 
		m_v3Positions[i].fY = array[i].vertex.y; 
		m_v3Positions[i].fZ = array[i].vertex.z; 

		if(hasNormal)	{								   
		  m_v3Normals[i].fX = array[i].vertex.nx; 
		  m_v3Normals[i].fY = array[i].vertex.ny; 
		  m_v3Normals[i].fZ = array[i].vertex.nz; 
		}
		
		if(hasColor) {
			 m_rgbaColors[i].val = array[i].color;
		}

		
		if(hasLight) {
			m_rgbaLightColors[i].val = array[i].pnLtColor;
		}
		
//		if(m_nAttributes)	{
//			array[i].attribute = m_pAttributes[i];
//		}

		int j;
		for(j=0;j<MAX_TEXTURES_PER_MSH_MATERIAL;j++)
		{
			if(hasUVs[j]) 
			{
				m_UVCoordinates[j][i].fU = array[i].vertex.uvs[j].fU;
				m_UVCoordinates[j][i].fV = array[i].vertex.uvs[j].fV; 
			}
		}
		
		if (hasWeight) {
			for (j = 0; (j < MAX_WEIGHT_COUNT) && array[i].weightId[j]; j ++) {
				envelop.AddVertexWeight (i, array[i].weightId[j], array[i].vertex.weight[j]);
			}
		}

	}

}


int CMSHBlockModelSegment::CopyFlatVertex (MSH_FLATVERTEX array[])
{

	int i,j;
//	_ASSERTE (m_nPositions);


	for (i = 0; i < m_nPositions; i ++) {
		memset (&array[i], 0, sizeof (MSH_FLATVERTEX));

		array[i].vertex.x = m_v3Positions[i].fX;
		array[i].vertex.y = m_v3Positions[i].fY;
		array[i].vertex.z = m_v3Positions[i].fZ;
		if(m_nNormals)	{
		  array[i].vertex.nx = m_v3Normals[i].fX;
		  array[i].vertex.ny = m_v3Normals[i].fY;
		  array[i].vertex.nz = m_v3Normals[i].fZ;
		}
		
		
		if(m_nColors) {
			 array[i].color = m_rgbaColors[i].val;
		}

		
		if(m_nLightColors) {
			array[i].pnLtColor = m_rgbaLightColors[i].val;
		}
		
//		if(m_nAttributes)	{
//			array[i].attribute = m_pAttributes[i];
//		}

		for(j=0;j<MAX_TEXTURES_PER_MSH_MATERIAL;j++)
		{
			if(m_nUVCoordinates[j]) {
				array[i].vertex.uvs[j].fU = m_UVCoordinates[j][i].fU;
				array[i].vertex.uvs[j].fV = m_UVCoordinates[j][i].fV;
			}
		}
	}



	CMSHBlockSegmentEnvelop::Iterator iter (envelop);
	_ASSERTE (!envelop.GetCount() || (envelop.GetCount() == (unsigned)m_nPositions));
	for (iter.Begin(); iter; iter ++) {
		int i;
		int index; 
		CMSHBlockVertexWeightsList::Iterator iter1((*iter)->info);
		index = (*iter)->key;
		i = 0;
		for (iter1.Begin(); iter1; iter1 ++) {
			ZeroMatrixWeightPair& pair = *iter1;

			array[index].vertex.weight[i] = pair.weight;
			array[index].weightId[i] = pair.boneID;
			i ++;
			_ASSERTE (i < MAX_WEIGHT_COUNT);
		}
	}


	return m_nPositions;
}

void CMSHBlockModelSegment::Write(FILE *file)
{

	if(!m_shadow)
	{
//		if (!m_pTriangleStrips) {
//				return;
//		}
		
//		if (!m_pTriangleStrips->m_nIndexCount) {
//			return;
//		}
	}


	StartBlock(file,'SEGM');


	if (m_shadow) {
		int i;
		StartBlock(file,'SHDW');

		WriteData(file,&m_shadow->vertexCount, sizeof(int));
		for(i=0 ; i < m_shadow->vertexCount ; i++)	{
			WriteData(file,&(m_shadow->vertexArray[i].fX),sizeof(float));
			WriteData(file,&(m_shadow->vertexArray[i].fY),sizeof(float));
			WriteData(file,&(m_shadow->vertexArray[i].fZ),sizeof(float));
		}

		WriteData(file,&m_shadow->edgeCount, sizeof(int));
		for(i=0 ; i<m_shadow->edgeCount; i++)	{
			WriteData(file,&(m_shadow->edgeList[i].index), sizeof (ZeroMshShadowEdge));
		}

		EndBlock(file);

	} else {
		if(m_sMaterialName)
		{
			StartBlock(file,'MATI');
			int index = m_pData ->GetMaterialSet() ->GetMaterialIndexByName(m_sMaterialName);
			WriteData(file,&index,sizeof(int));
			EndBlock(file);
		}
		
		
		if(m_nPositions)
		{
			StartBlock(file,'POSL');
			WriteData(file,&m_nPositions,sizeof(int));
				for(int i=0 ; i<m_nPositions ; i++)	{
					WriteData(file,&(m_v3Positions[i].fX),sizeof(float));
					WriteData(file,&(m_v3Positions[i].fY),sizeof(float));
					WriteData(file,&(m_v3Positions[i].fZ),sizeof(float));
				}
			EndBlock(file);
			
			if (envelop.GetCount()) {
				envelop.Write (file);
			}
			
		}
		
		if(m_nNormals)
		{
			//	only export normals if dynamically lit or if RetainNormals is set or not exporting from modeler
			if(!m_pData->GetExportFromModeler() || m_pMyModel->GetDynamicallyLit() || m_pMyModel->GetRetainNormals())
			{
				StartBlock(file,'NRML');
				WriteData(file,&m_nNormals,sizeof(int));
				for(int i=0;i<m_nNormals;i++)
				{
					WriteData(file,&(m_v3Normals[i].fX),sizeof(float));
					WriteData(file,&(m_v3Normals[i].fY),sizeof(float));
					WriteData(file,&(m_v3Normals[i].fZ),sizeof(float));
				}
				EndBlock(file);
			}
		}
		
		
		unsigned int color=ConvertRGBAToLong(&m_rgbaColorBase);
		if(m_HasColorBase)
		{
			StartBlock(file,'CLRB');
			WriteData(file,&(color),sizeof(unsigned int));
			EndBlock(file);
		}
		else
		{
			if(m_nColors)
			{
				StartBlock(file,'CLRL');
				WriteData(file,&m_nColors,sizeof(int));
				for(int i=0;i<m_nColors;i++)
				{
					color=ConvertRGBAToLong(&m_rgbaColors[i]);
					WriteData(file,&(color),sizeof(unsigned int));
				}
				EndBlock(file);
			}
		}
		
		CalculateLightColorBase();
		color=ConvertRGBAToLong(&m_rgbaLightColorBase);
		if(m_HasLightColorBase)
		{
			StartBlock(file,'VLTB');
			WriteData(file,&(color),sizeof(unsigned int));
			EndBlock(file);
		}
		else
		{
			if(m_nLightColors)
			{
				StartBlock(file,'VLTL');
				WriteData(file,&m_nLightColors,sizeof(int));
				for(int i=0;i<m_nLightColors;i++)
				{
					color=ConvertRGBAToLong(&m_rgbaLightColors[i]);
					WriteData(file,&(color),sizeof(unsigned int));
				}
				EndBlock(file);
			}
		}
		
		/*
		WriteData(file,&(m_AttributeBase),sizeof(unsigned int));
		EndBlock(file);
		if(m_nAttributes)
		{
				StartBlock(file,'ATRL');
				WriteData(file,&m_nAttributes,sizeof(int));
				for(int i=0;i<m_nAttributes;i++)
				WriteData(file,&(m_pAttributes[i]),sizeof(unsigned int));
				EndBlock(file);
				}
		*/

		if (m_pMyModel->IsClothEnabled()/*GetModelType() == Cloth_Node*/) {
			int i;
			StartBlock(file,'UV0L');
			if (m_nUVCoordinates[0]) {
				int i;
				WriteData(file,&m_nUVCoordinates[0],sizeof(int));
				for(i=0; i < m_nUVCoordinates[0]; i++)	{
					WriteData(file,&(m_UVCoordinates[0][i].fU),sizeof(float));
					WriteData(file,&(m_UVCoordinates[0][i].fV),sizeof(float));
				}
			} else {
				float u;
				float v;
				u = 0;
				v = 0;
				WriteData(file, &m_nPositions, sizeof(int));
				for(i=0; i < m_nPositions; i++)
				{
					WriteData(file,&u,sizeof(float));
					WriteData(file,&v,sizeof(float));
				}
			}
			EndBlock(file);

		} else {
			if(m_nUVCoordinates[0]) {
				bool bWriteUVs = false;
				for(int i=0;((i<m_nUVCoordinates[0]) && (!bWriteUVs));i++)
				{
					if((m_UVCoordinates[0][i].fU!=0.0) ||
						(m_UVCoordinates[0][i].fV!=0.0))
						bWriteUVs = true;
				}
				
				if(bWriteUVs)
				{
					StartBlock(file,'UV0L');
					WriteData(file,&m_nUVCoordinates[0],sizeof(int));
					for(int i=0;i<m_nUVCoordinates[0];i++)
					{
						WriteData(file,&(m_UVCoordinates[0][i].fU),sizeof(float));
						WriteData(file,&(m_UVCoordinates[0][i].fV),sizeof(float));
					}
					EndBlock(file);
				}
			}
		}
		
		for(int j=1;j<MAX_TEXTURES_PER_MSH_MATERIAL;j++)
		{
			if(m_nUVCoordinates[j])
			{
				bool bWriteUVs = false;
				for(int i=0;((i<m_nUVCoordinates[j]) && (!bWriteUVs));i++)
				{
					if((m_UVCoordinates[j][i].fU!=0.0) ||
						(m_UVCoordinates[j][i].fV!=0.0))
						bWriteUVs = true;
				}
				
				if(bWriteUVs)
				{
					unsigned int type;
					type = 'UV0L';
					type &= 0xFFFF00FF;
					type |= ('0' + j) << 8;
					StartBlock(file,type);
					WriteData(file,&m_nUVCoordinates[j],sizeof(int));
					for(int i=0;i<m_nUVCoordinates[j];i++)
					{
						WriteData(file,&(m_UVCoordinates[j][i].fU),sizeof(float));
						WriteData(file,&(m_UVCoordinates[j][i].fV),sizeof(float));
					}
					EndBlock(file);
				}
			}
		}

		// We now always export polys, tristrips and tris --DY
//#ifdef EXPORT_ORIGINAL_POLYGONS
		if(m_pPolygonList.GetCount())
			m_pPolygonList.Write(file);
//#endif		
		
//#ifdef EXPORT_TRIANGLE_INDEXLIST
		m_pTriangleList->Write(file);
//#endif		

		m_pTriangleStrips->Write(file);
	}
		
	EndBlock(file);
}

void CMSHBlockModelSegment::WriteText(FILE *fp)
{
	fprintf(fp,"\t\tSegment\n\t\t{\n");

	if (m_shadow) {
		int i;
		fprintf(fp,"\t\t\tShadow block\n");
		fprintf(fp,"\t\t\t\tNumPositions{%d}\n",m_shadow->vertexCount);
		for( i=0 ; i<m_shadow->vertexCount ; i++)	{
			fprintf(fp,"\t\t\t\t(%f,%f,%f)\n", m_shadow->vertexArray[i].fX, m_shadow->vertexArray[i].fY, m_shadow->vertexArray[i].fZ);
		}


		fprintf(fp,"\n");
		fprintf(fp,"\t\t\t\tedge list {%d}\n", m_shadow->edgeCount);
		for(i=0 ; i<m_shadow->edgeCount; i++)	{
			fprintf(fp,"\t\t\t\t(%d, %d, %d, %d)\n", m_shadow->edgeList[i].index[0], m_shadow->edgeList[i].index[1], m_shadow->edgeList[i].index[2], m_shadow->edgeList[i].index[3]);
		}



	} else {
	
		if(m_sMaterialName) {
			fprintf(fp,"\t\t\tMaterialName{%s}\n",m_sMaterialName);
		}
		
		int i;
		if(m_nPositions)
		{
			fprintf(fp,"\t\t\tNumPositions{%d}\n",m_nPositions);
			i=0;
			fprintf(fp,"\t\t\tPositions\n\t\t\t{\n");
			for (i = 0; i<m_nPositions; i ++) {
				fprintf(fp,"\t\t\t\t(%f,%f,%f)\n",m_v3Positions[i].fX,m_v3Positions[i].fY,m_v3Positions[i].fZ);
			}
			fprintf(fp,"\t\t\t}\n");
			
			if (envelop.GetCount()) {
				envelop.WriteText (fp);
			}
		}
		
		if(m_nNormals)
		{
			fprintf(fp,"\t\t\tNumNormals{%d}\n",m_nNormals);
			i=0;
			fprintf(fp,"\t\t\tNormals\n\t\t\t{\n\t\t\t");
			for (i = 0; i<m_nNormals; i ++) {
				fprintf(fp,"\t\t\t\t(%f,%f,%f)\n",m_v3Normals[i].fX,m_v3Normals[i].fY,m_v3Normals[i].fZ);
			}
			fprintf(fp,"\n\t\t\t}\n");
		}
		
		if(m_nColors)
		{
			fprintf(fp,"\t\t\tNumColors{%d}\n",m_nColors);
			i=0;
			fprintf(fp,"\t\t\tColors\n\t\t\t{\n\t\t\t");
			while(i<m_nColors)
//			{
//				if((i%8==0) && (i!=0))
//					fprintf(fp,"\n\t\t\t");
//				fprintf(fp,"(%u,%u,%u,%u)",m_rgbaColors[i].cR,m_rgbaColors[i].cG,m_rgbaColors[i].cB,m_rgbaColors[i].cA);
//				if(i+1<m_nColors)
//					fprintf(fp,", ");
//				i++;
//			}
			for (i = 0; i<m_nColors; i ++) {
				fprintf(fp,"\t\t\t\t(%u,%u,%u,%u)\n", m_rgbaColors[i].cR,m_rgbaColors[i].cG,m_rgbaColors[i].cB,m_rgbaColors[i].cA);
			}

			fprintf(fp,"\n\t\t\t}\n");
		}
		else
		{
			if(m_HasColorBase)
			{
				fprintf(fp,"\t\t\tColorBase(%u,%u,%u,%u)\n",m_rgbaColorBase.cR,m_rgbaColorBase.cG,m_rgbaColorBase.cB,m_rgbaColorBase.cA);
			}
		}
		
		CalculateLightColorBase();
		if(m_HasLightColorBase)
		{
			fprintf(fp,"\t\t\tLightColorBase(%u,%u,%u,%u)\n",m_rgbaLightColorBase.cR,m_rgbaLightColorBase.cG,m_rgbaLightColorBase.cB,m_rgbaLightColorBase.cA);
		}
		else
		{
			if(m_nLightColors)
			{
				fprintf(fp,"\t\t\tNumLightColors{%d}\n",m_nLightColors);
				i=0;
				fprintf(fp,"\t\t\tLightColors\n\t\t\t{\n\t\t\t");
				while(i<m_nLightColors)
				{
					if((i%8==0) && (i!=0))
						fprintf(fp,"\n\t\t\t");
					fprintf(fp,"(%u,%u,%u,%u)",m_rgbaLightColors[i].cR,m_rgbaLightColors[i].cG,m_rgbaLightColors[i].cB,m_rgbaLightColors[i].cA);
					if(i+1<m_nLightColors)
						fprintf(fp,", ");
					i++;
				}
				fprintf(fp,"\n\t\t\t}\n");
			}
		}
		
		
		if(m_nAttributes)
		{
			fprintf(fp,"\t\t\tNumAttributes{%d}\n",m_nAttributes);
			i=0;
			fprintf(fp,"\t\t\tAttributes\n\t\t\t{\n\t\t\t");
			while(i<m_nAttributes)
			{
				if((i%8==0) && (i!=0))
					fprintf(fp,"\n\t\t\t");
				fprintf(fp,"%u",m_pAttributes[i]);
				if(i+1<m_nAttributes)
					fprintf(fp,", ");
				i++;
			}
			fprintf(fp,"\n\t\t\t}\n");
		}
		fprintf(fp,"\t\t\tAttributeBase{%u}\n",m_AttributeBase);

		
		if (m_pMyModel->IsClothEnabled()/*GetModelType() == Cloth_Node*/) {
			fprintf(fp,"\t\t\tNumUVCoordinates0{%d}\n",m_nUVCoordinates[0]);
			i=0;
			fprintf(fp,"\t\t\tUVCoordinates0\n\t\t\t{\n\t\t\t");
			while(i<m_nUVCoordinates[0])
			{
				if((i%6==0) && (i!=0)) {
					fprintf(fp,"\n\t\t\t");
				}
				fprintf(fp,"(%f,%f)", 0.0, 0.0);
				if(i+1<m_nUVCoordinates[0])
					fprintf(fp,", ");
				i++;
			}
			fprintf(fp,"\n\t\t\t}\n");

		} else {
			if(m_nUVCoordinates[0]) {
				fprintf(fp,"\t\t\tNumUVCoordinates0{%d}\n",m_nUVCoordinates[0]);
				i=0;
				fprintf(fp,"\t\t\tUVCoordinates0\n\t\t\t{\n");
//				while(i<m_nUVCoordinates[0])
//				{
//					if((i%6==0) && (i!=0))
//						fprintf(fp,"\n\t\t\t\t");
//					fprintf(fp,"(%f,%f)",m_UVCoordinates[0][i].fU,m_UVCoordinates[0][i].fV);
//					if(i+1<m_nUVCoordinates[0])
//						fprintf(fp,", ");
//					i++;
//				}
				for (i = 0; i<m_nUVCoordinates[0]; i ++) {
					fprintf(fp,"\t\t\t\t(%f,%f)\n", m_UVCoordinates[0][i].fU,m_UVCoordinates[0][i].fV);
				}

				fprintf(fp,"\n\t\t\t}\n");
			}
		}
		
		for(int j=1;j<MAX_TEXTURES_PER_MSH_MATERIAL;j++)
		{
			if(m_nUVCoordinates[j])
			{
				fprintf(fp,"\t\t\tNumUVCoordinates%d{%d}\n",j,m_nUVCoordinates[j]);
				i=0;
				fprintf(fp,"\t\t\tUVCoordinates%d\n\t\t\t{\n\t\t\t",j);
//				while(i<m_nUVCoordinates[j])
//				{
//					if((i%6==0) && (i!=0))
//						fprintf(fp,"\n\t\t\t");
//					fprintf(fp,"(%f,%f)",m_UVCoordinates[j][i].fU,m_UVCoordinates[j][i].fV);
//					if(i+1<m_nUVCoordinates[j])
//						fprintf(fp,", ");
//					i++;
//				}
				for (i = 0; i<m_nUVCoordinates[j]; i ++) {
					fprintf(fp,"\t\t\t\t(%f,%f)\n", m_UVCoordinates[j][i].fU,m_UVCoordinates[j][i].fV);
				}

				fprintf(fp,"\n\t\t\t}\n");
			}
		}
		
#ifdef EXPORT_ORIGINAL_POLYGONS
		if(m_pPolygonList.GetCount())	{
			m_pPolygonList.WriteText(fp);
		}
#endif

		
#ifdef EXPORT_TRIANGLE_INDEXLIST
		m_pTriangleList->WriteText(fp);
#endif

		if(m_pTriangleStrips) {
			m_pTriangleStrips->WriteText(fp);
		}




	}
		
		
	fprintf(fp,"\n\t\t}\n");
}
	
void CMSHBlockModelSegment::SetMaterialName(const char *sMaterialName)
{
	if(m_sMaterialName)
		delete[] m_sMaterialName;
	m_sMaterialName = new char[strlen(sMaterialName)+1];
	strcpy(m_sMaterialName,sMaterialName);
}

void CMSHBlockModelSegment::SetNumPositions(int nPositions)
{
	if(m_v3Positions)
		delete m_v3Positions;
	m_nPositions = nPositions;
	m_v3Positions = new vect3[m_nPositions];
}

void CMSHBlockModelSegment::SetPosition(int index, float fX, float fY, float fZ)
{
	if(index<m_nPositions)
	{
		m_v3Positions[index].fX = fX;
		m_v3Positions[index].fY = fY;
		m_v3Positions[index].fZ = fZ;
	}
}

void CMSHBlockModelSegment::SetNumNormals(int nNormals)
{
	if(m_v3Normals)
		delete m_v3Normals;
	m_nNormals = nNormals;
	m_v3Normals = new vect3[m_nNormals];
}

void CMSHBlockModelSegment::SetNormal(int index, float fX, float fY, float fZ)
{
	if(index<m_nNormals)
	{
		m_v3Normals[index].fX = fX;
		m_v3Normals[index].fY = fY;
		m_v3Normals[index].fZ = fZ;
	}
}
void CMSHBlockModelSegment::SetNumColors(int nColors)
{
	if(m_rgbaColors)
		delete m_rgbaColors;
	m_nColors = nColors;
	m_rgbaColors = new rgba[m_nColors];
}

void CMSHBlockModelSegment::SetColor(int index, unsigned int col)
{
	if(index<m_nColors)
		ConvertLongToRGBA(col,&(m_rgbaColors[index]));
}

void CMSHBlockModelSegment::SetNumLightColors(int nColors)
{
	if(m_rgbaLightColors)
		delete m_rgbaLightColors;
	m_nLightColors = nColors;
	m_rgbaLightColors = new rgba[m_nLightColors];
}

void CMSHBlockModelSegment::SetLightColor(int index, unsigned char cR, unsigned char cG, unsigned char cB, unsigned char cA)
{
	if(index<m_nLightColors)
	{
		m_rgbaLightColors[index].cR = cR;
		m_rgbaLightColors[index].cG = cG;
		m_rgbaLightColors[index].cB = cB;
		m_rgbaLightColors[index].cA = cA;
	}
}

void CMSHBlockModelSegment::SetNumUVs(int nUVs,int idx)
{
	if(m_UVCoordinates[idx])
		delete m_UVCoordinates[idx];
	m_nUVCoordinates[idx] = nUVs;
	m_UVCoordinates[idx] = new uv[m_nUVCoordinates[idx]];
/*
	switch(idx)
	{
	case 0:
		if(m_UVCoordinates0)
			delete m_UVCoordinates0;
		m_nUVCoordinates0 = nUVs;
		m_UVCoordinates0 = new uv[m_nUVCoordinates0];
		break;
	case 1:
		if(m_UVCoordinates1)
			delete m_UVCoordinates1;
		m_nUVCoordinates1 = nUVs;
		m_UVCoordinates1 = new uv[m_nUVCoordinates1];
		break;
	}
*/
}

void CMSHBlockModelSegment::SetUV(int index, float fU, float fV,int idx)
{
	if(m_UVCoordinates[idx])
	{
		if(index<m_nUVCoordinates[idx])
		{
			m_UVCoordinates[idx][index].fU = fU;
			m_UVCoordinates[idx][index].fV = fV;
		}
	}
/*
	switch(idx)
	{
	case 0:
		if(m_UVCoordinates0)
		{
			if(index<m_nUVCoordinates0)
			{
				m_UVCoordinates0[index].fU = fU;
				m_UVCoordinates0[index].fV = fV;
			}
		}
		break;
	case 1:
		if(m_UVCoordinates1)
		{
			if(index<m_nUVCoordinates1)
			{
				m_UVCoordinates1[index].fU = fU;
				m_UVCoordinates1[index].fV = fV;
			}
		}
		break;
	}
*/
}

/*
void CMSHBlockModelSegment::SetNumLightmapUVs(int nLightmapUVs)
{
	if(m_LightmapUVCoordinates)
		delete m_LightmapUVCoordinates;
	m_nLightmapUVCoordinates = nLightmapUVs;
	m_LightmapUVCoordinates = new uv[m_nLightmapUVCoordinates];
}

void CMSHBlockModelSegment::SetLightmapUV(int index, float fU, float fV)
{
	if(index<m_nLightmapUVCoordinates)
	{
		m_LightmapUVCoordinates[index].fU = fU;
		m_LightmapUVCoordinates[index].fV = fV;
	}
}
*/

void CMSHBlockModelSegment::SetNumAttributes(int nAttributes)
{
	if(m_pAttributes)
		delete m_pAttributes;
	m_nAttributes = nAttributes;
	m_pAttributes = new unsigned int[m_nAttributes];
}

void CMSHBlockModelSegment::SetAttribute(int index, unsigned int Attribute)
{
	if(index<m_nAttributes)
		m_pAttributes[index] = Attribute;
}

void CMSHBlockModelSegment::SetAttributeBase(unsigned int val)
{
	m_AttributeBase = val;
}

void CMSHBlockModelSegment::ConvertLongToRGBA(unsigned int val, rgba *pCol)
{
	pCol->cA=(unsigned char)((0xFF000000&val)>>24);
	pCol->cR=(unsigned char)((0x00FF0000&val)>>16);
	pCol->cG=(unsigned char)((0x0000FF00&val)>>8);
	pCol->cB=(unsigned char)(0x000000FF&val);
}

unsigned int CMSHBlockModelSegment::ConvertRGBAToLong(rgba *pCol)
{
	return (pCol->cA<<24) + 
		(pCol->cR<<16) + 
		(pCol->cG<<8) + 
		(pCol->cB);
}



CMSHBlockTriangleList *CMSHBlockModelSegment::CreateNewTriangleList(int triangleCount, unsigned *pIndices)
{
	BlockHeader header;
	header.size=0;

	if(m_pTriangleList) {
		delete m_pTriangleList;
	}
	m_pTriangleList = new CMSHBlockTriangleList(&header, m_pData, triangleCount, pIndices);
	return m_pTriangleList;
	
}

CMSHBlockTriangleStripList *CMSHBlockModelSegment::CreateNewTriangleStrip(int indexCount, unsigned int *pIndices)
{

	BlockHeader header;
	header.size=0;

	if(m_pTriangleStrips) {
		delete m_pTriangleStrips;
	}
	m_pTriangleStrips = new CMSHBlockTriangleStripList(&header, m_pData, indexCount, pIndices);
	return m_pTriangleStrips;
}


CMSHShadowVolume *CMSHBlockModelSegment::CreateEmptyShadowVolume(int vertCount,int edgeCount)
{
	if(m_shadow)
		delete m_shadow;
	m_shadow = new CMSHShadowVolume (vertCount, edgeCount);
	m_pData->SetHasShadowVolumes(true);
	return m_shadow;
}

void CMSHBlockModelSegment::CreateShadowVolume()
{
	int i;
	int mark;
	int edgeCount;
	int polyCount;
	int indexCount;
	int vertexCount;


	CMSHBlockPolygonList& polyList = *GetPolygonList();

	polyCount = polyList.GetCount();
	if (!polyCount) {
		return;
	}


	ZeroStack<unsigned int>indexList (polyCount * 12);
	ZeroStack<vect3>unpackVertexArray (polyCount * 12);

	
	m_pMyModel->SetHidden (false);

	indexCount = 0;
	CMSHBlockPolygonList::Iterator iter(polyList);
	for (iter.Begin(); iter; iter ++) {
		MSHBLOCKPolygon &polygon = (*iter)->info;
		MSHBLOCKPolygon::Iterator polyIter (polygon);
		for (polyIter.Begin(); polyIter; polyIter ++) {
			int index; 
			index = (*polyIter);
			unpackVertexArray [indexCount] = m_v3Positions[index];
			indexCount ++;
			_ASSERTE (indexCount	< polyCount * 12);
		}
	}


	// convert flat triangle list to triangle index list, and remove all duplicated vertices
	vertexCount	= VertexListToIndexList (&unpackVertexArray [0].fX, sizeof (vect3), indexCount, &indexList[0], 0, 0.01f);



	// create polyhedra form triangle list
	ZeroPolyhedra polyhedra;
	polyhedra.BeginFace();

	indexCount = 0;
	for (iter.Begin(); iter; iter ++) {
		int index;
		int face[256];
		MSHBLOCKPolygon &polygon = (*iter)->info;

		index = polygon.GetCount();
		for (i = 0; i < index; i ++) {
			face[i] = indexList[indexCount];
			indexCount ++;
		}
		polyhedra.AddFace (index, face);
	}
	polyhedra.EndFace();



	polyhedra.ConvexPartition(&unpackVertexArray [0].fX, sizeof (vect3));


	ZeroStack<vect3>vertexArray (polyCount * 12);

	ZeroPolyhedraDescriptor desc (&polyhedra);
	polyhedra.PackVertex (desc, &vertexArray[0].fX, &unpackVertexArray[0].fX, sizeof (vect3));


	edgeCount = 0;
	mark = polyhedra.IncLRU();

	// check edgelist
	ZeroPolyhedra::Iterator polyIter(polyhedra);
	for (polyIter.Begin(); polyIter; polyIter ++) {
		ZeroEdge *edge;
		edge = &(*polyIter)->info;


		_ASSERTE ((edge->incidentFace > 0) && (edge->twin->incidentFace > 0));

		if (edge->mark != mark) {
			edge->userData = edgeCount;
			edge->twin->userData = edgeCount + 1;
			edgeCount += 2;

			edge->mark = mark;
			edge->twin->mark = mark;
		}
	}

	m_pData->SetHasShadowVolumes(true);
	m_shadow	= new CMSHShadowVolume (desc.vertexCount, edgeCount);
	memcpy (m_shadow->vertexArray, &vertexArray[0].fX, desc.vertexCount * sizeof (vect3));


	edgeCount = 0;
	mark = polyhedra.IncLRU();
	for (polyIter.Begin(); polyIter; polyIter ++) {
		ZeroEdge *edge;

		edge = &(*polyIter)->info;
		if (edge->mark == mark) {
			continue;
		}

		_ASSERTE (edge->incidentFace > 0);
		_ASSERTE (edge->twin->incidentFace > 0);

		edge->mark = mark;
		edge->twin->mark = mark;

		m_shadow->edgeList[edgeCount].index[0] = unsigned short(edge->incidentVertex);
		m_shadow->edgeList[edgeCount].index[1] = unsigned short(edge->next->userData);
		m_shadow->edgeList[edgeCount].index[2] = unsigned short(edge->twin->userData);
		m_shadow->edgeList[edgeCount].index[3]	= unsigned short(-1);
		edgeCount ++;

		edge = edge->twin;
		m_shadow->edgeList[edgeCount].index[0] = unsigned short(edge->incidentVertex);
		m_shadow->edgeList[edgeCount].index[1] = unsigned short(edge->next->userData);
		m_shadow->edgeList[edgeCount].index[2] = unsigned short(edge->twin->userData);
		m_shadow->edgeList[edgeCount].index[3]	= unsigned short(-1);
		edgeCount ++;
	}


	CreateVertexIndexStripsAndIndexList ();
}



void CMSHBlockModelSegment::CreateVertexIndexStripsAndIndexList ()
{
	int vertecCount;
	int stripIndexCount;
	int triangleIndexCount;
	vect3 *vertex;


	ZeroStack<unsigned> indices(1024 * 512);
	vertex = GetPositions(vertecCount);

	triangleIndexCount = GetPolygonList()->TriangleList (&indices[0], &vertex[0]);
	CreateNewTriangleList (triangleIndexCount / 3, &indices[0]);

	stripIndexCount = GetPolygonList()->TriangleStrip (&indices[0], &vertex[0]);
	CreateNewTriangleStrip(stripIndexCount, &indices[0]);
}


void CMSHBlockModelSegment::CreateClothVertexList()
{

	// create the index list and index strips
	CreateVertexIndexStripsAndIndexList ();
	m_pMyModel->CreateClothVertexList(*this);
}


void CMSHBlockModelSegment::SetColorBase(unsigned char cR, unsigned char cG, unsigned char cB, unsigned char cA)
{
	m_HasColorBase = true;
	m_rgbaColorBase.cR=cR;
	m_rgbaColorBase.cG=cG;
	m_rgbaColorBase.cB=cB;
	m_rgbaColorBase.cA=cA;
}

void CMSHBlockModelSegment::SetLightColorBase(unsigned char cR, unsigned char cG, unsigned char cB, unsigned char cA)
{
	m_HasLightColorBase = true;
	m_rgbaLightColorBase.cR=cR;
	m_rgbaLightColorBase.cG=cG;
	m_rgbaLightColorBase.cB=cB;
	m_rgbaLightColorBase.cA=cA;
}


void CMSHBlockModelSegment::AddVertexWeight (
															unsigned vertexIndex, 
															unsigned boneID, 
															float weight)
{
	envelop.AddVertexWeight (vertexIndex, boneID, weight);
}



char *CMSHBlockModelSegment::GetMaterialName() const
{
	return m_sMaterialName;
}

vect3 *CMSHBlockModelSegment::GetPositions(int &nPos) const
{
	nPos=m_nPositions;
	return m_v3Positions;
}

vect3 *CMSHBlockModelSegment::GetNormals(int &nNorms) const
{
	nNorms=m_nNormals;
	return m_v3Normals;
}

rgba *CMSHBlockModelSegment::GetColors(int &nColors) const
{
	nColors=m_nColors;
	return m_rgbaColors;
}

rgba *CMSHBlockModelSegment::GetLightColors(int &nColors) const
{
	nColors=m_nLightColors;
	return m_rgbaLightColors;
}


unsigned int *CMSHBlockModelSegment::GetAttributes(int &nAttributes) const
{
	nAttributes=m_nAttributes;
	return m_pAttributes;
}

uv *CMSHBlockModelSegment::GetUVCoords(int &nUV,int idx) const
{
	nUV=m_nUVCoordinates[idx];
	return m_UVCoordinates[idx];
/*
	switch(idx)
	{
	case 0:nUV=m_nUVCoordinates0;return m_UVCoordinates0;
	case 1:nUV=m_nUVCoordinates1;return m_UVCoordinates1;
	}

	nUV=0;
	return NULL;
*/
}

CMSHBlockTriangleList *CMSHBlockModelSegment::GetTriangleList() const
{
	return m_pTriangleList;
}


CMSHBlockTriangleStripList *CMSHBlockModelSegment::GetTriangleStrip() const
{
	return m_pTriangleStrips;
}


CMSHBlockPolygonList *CMSHBlockModelSegment::GetPolygonList() const
{
	return &m_pPolygonList;
}

void CMSHBlockModelSegment::GetColorBase(unsigned char &cR,unsigned char &cG,unsigned char &cB,unsigned char &cA)
{
	cR=m_rgbaColorBase.cR;
	cG=m_rgbaColorBase.cG;
	cB=m_rgbaColorBase.cB;
	cA=m_rgbaColorBase.cA;
}

void CMSHBlockModelSegment::GetLightColorBase(unsigned char &cR,unsigned char &cG,unsigned char &cB,unsigned char &cA)
{
	cR=m_rgbaLightColorBase.cR;
	cG=m_rgbaLightColorBase.cG;
	cB=m_rgbaLightColorBase.cB;
	cA=m_rgbaLightColorBase.cA;
}

unsigned int CMSHBlockModelSegment::GetAttributeBase()
{
	return m_AttributeBase;
}


bool CMSHBlockModelSegment::HasColorBase()
{
	return m_HasColorBase;
}

bool CMSHBlockModelSegment::HasLightColorBase()
{
	return m_HasLightColorBase;
}


/*----------------------------------------------------------------------------------------
CMSHBlockModel
----------------------------------------------------------------------------------------*/
CMSHBlockModel::CMSHBlockModel(BlockHeader *pHdr,CZeroData *pData)
:CMSHBlock(pHdr,pData)
{
	isHidden = false;
	isDynamicallyLit = false;
	isRetainNormals = false;
	isRenderAfterShadows = false;
	isLeaveChildGeometry = false;
	isPS2Optimize = false;
	
	modelType = Null_Node;
	matrixPaletteIndex = -1;

/*
	constraintParent = 0;
*/
	
	m_sName=NULL;
	m_sParentName=NULL;
	
	m_nModelSegments=0;
	m_ppModelSegments=NULL;
	
	
	m_HasBBox=false;
	
	// cloth data
	m_ClothData.bClothEnabled = 0;

	m_ClothParticlesCount = 0;
	m_ClothParticles = NULL;

	m_TriangleCount = 0;
	m_ClothTriagleList = NULL;

	m_ClothParticleToVertexIndexMapCount = 0;
	m_ClothParticleToVertexIndexMap = NULL;
	m_ClothUV = NULL;


	m_nClothFixedPoints = 0;
	m_pClothFixedIndices = NULL;
	m_FixedPointBoneName = NULL;
	m_nUserCloth = 0;
	m_pUserClothFirst = NULL;
	m_pUserClothSecond = NULL;

	m_StretchPairsCount = 0;
	m_softVertexStretchPairs = NULL;

	m_BendPairsCount = 0;
	m_softVertexBendPairs = NULL;

	m_CrossPairsCount = 0;
	m_softVertexCrossPairs = NULL;

	m_CollData.bCollisionEnabled = 0;
	m_CollData.sType = -1;

	m_envelopsCount = 0;
	m_envelopArray = NULL;

	
	m_sCollTriggeredAnimation = NULL;
	m_AnimTriggerCollData.config.bCollisionEnabled = 0;
	m_AnimTriggerCollData.config.sType = -1;

	m_bIsSWCICollisionObject = false;
	m_SWCICollisionType = PST_SPHERE;
	m_fSWCI_Radius = 0.0f;
	m_fSWCI_Height = 0.0f;
	m_fSWCI_Length = 0.0f;

	m_bClothEnabled = false;

}

CMSHBlockModel::~CMSHBlockModel()
{

	if (m_envelopArray) {
		delete[] m_envelopArray;
	}
	
	if(m_sName)	{
		delete[] m_sName;
	}
	if(m_sParentName)	{
		delete[] m_sParentName;
	}
	
	if(m_ppModelSegments) {
		int i;
		for(i=0;i<m_nModelSegments;i++)
			delete m_ppModelSegments[i];
		delete[] m_ppModelSegments;
	}

	
	if (m_ClothUV) {
		delete[]m_ClothUV;
	}

	if(m_ClothParticles) {
		delete[]m_ClothParticles;
	}
	if(m_ClothTriagleList) {
		delete[]m_ClothTriagleList;
	}

	if(m_ClothParticleToVertexIndexMap) {
		delete[]m_ClothParticleToVertexIndexMap;
	}

	if(m_pClothFixedIndices) {
		delete[] m_pClothFixedIndices;
	}

	if(m_pUserClothFirst) {
		delete[] m_pUserClothFirst;
	}

	if(m_pUserClothSecond) {
		delete[] m_pUserClothSecond;
	}
	
	if(m_FixedPointBoneName) {
		delete[] m_FixedPointBoneName;
	}

	if(m_sCollTriggeredAnimation)	{
		delete[] m_sCollTriggeredAnimation;
	}

	if (m_softVertexStretchPairs) {
		delete[] m_softVertexStretchPairs;
	}

	if (m_softVertexBendPairs) {
		delete[] m_softVertexBendPairs;
	}

	if(m_softVertexCrossPairs)
		delete[] m_softVertexCrossPairs;
}


int *CMSHBlockModel::GetEnvelopArray(int &nMatrices)
{
	nMatrices=m_envelopsCount;
	return m_envelopArray;
}

void CMSHBlockModel::SetEnvelopArray(int nIndices, const int pIndices[]) 
{
	if (m_envelopArray) {
		delete[] m_envelopArray;
	}
	m_envelopsCount = nIndices;

	m_envelopArray	= ZeroNew (int[m_envelopsCount]);
	memcpy (m_envelopArray, pIndices, sizeof (int) * m_envelopsCount);
}

/*
 void CMSHBlockModel::SetConstraint	(char* name)
 {
	 constraintParent = CalcStrCRC (name);
 }

 unsigned CMSHBlockModel::GetConstraint ()
 {
	 return constraintParent;
 }
*/

void CMSHBlockModel::RemoveGeometry()
{
	if(m_ppModelSegments)
	{
		int i;
		for(i=0;i<m_nModelSegments;i++)
			delete m_ppModelSegments[i];
		delete m_ppModelSegments;

		m_ppModelSegments = NULL;
		m_nModelSegments = 0;
	}
	SetModelType(Null_Node);	
}

void CMSHBlockModel::Read(FILE *file)
{
	unsigned int blockSize=m_Header.size;
	
	while (blockSize)
	{
		BlockHeader parentheader;
		unsigned int parentblockSize;
		BlockHeader geomheader;
		unsigned int geomblockSize;
		
		BlockHeader header;
		ReadHeader(file,&header);
		
		// deduct blockSize
		_ASSERTE(blockSize >= sizeof(BlockHeader) + header.size);
		blockSize -= sizeof(BlockHeader);
		blockSize -= header.size;
		
		// parse the block
		CMSHBlockModelSegment *pSegment;
		switch (header.type)
		{
			
		case 'PYTM':
			{
				unsigned type;
				ReadData(file, &type, sizeof(int));
				SetModelType ((ZeroMshModelTypes) type);
				break;
			}
			
		case 'XDNM':
			{
				unsigned index;
				ReadData(file, &index, sizeof(int));
				SetMatrixPaletteIndex (index);
				break;
			}
			
			
			
	    	case 'EMAN':
				ReadString(file, header.size,&m_sName);
				CalcStrCRC (m_sName);
				if(m_sName)
					ZeroTrace(("%s\n",m_sName));
				break;
				
				//-------------------------------------------------------------
				// FOR COMPATABILITY, LEAVE THIS !!!  
				//	currently, the exporter uses 'PRNT' but there's no guarantee that 
				//	the imported file is in latest format
			case 'NRAP':
				parentblockSize=header.size;
				while (parentblockSize)
				{
					// read a block header
					ReadHeader(file,&parentheader);
					
					// deduct blockSize
					_ASSERTE(parentblockSize >= sizeof(BlockHeader) + parentheader.size);
					parentblockSize -= sizeof(BlockHeader);
					parentblockSize -= parentheader.size;
					
					// parse the block
					switch (parentheader.type)
					{
						//					case 'XFRP':
						//						ReadString(file,parentheader.size,m_sParentPrefix);
						//						break;
					case 'EMAN':
						ReadString(file,parentheader.size,&m_sParentName);
						break;
					default:
						SkipBlock(file, parentheader.size);
						break;
					}
				}
				break;
				
			case 'TNRP':
				ReadString(file, header.size, &m_sParentName);
				break;
				
			case 'SGLF':
				{
					unsigned flags;
					ReadData(file,&flags,sizeof(int));
					SetHidden ((flags & 1) ? true : false);
					SetDynamicallyLit ((flags & 2) ? true : false);
					SetRetainNormals ((flags & 4) ? true : false);
					SetRenderAfterShadows ((flags & 8) ? true : false);
					SetLeaveChildGeometry ((flags & 16) ? true : false);
					SetPS2Optimize ((flags & 16) ? true : false);
					break;
				}
				
				
				// ------------ Cloth collision object ---------------------
			case 'LLOC': // for compatability
			case '0LOC':
				_ASSERTE (0);
/*
				parentblockSize=header.size;
				while (parentblockSize)
				{
					// read a block header
					ReadHeader(file,&parentheader);
					
					// deduct blockSize
					_ASSERTE(parentblockSize >= sizeof(BlockHeader) + parentheader.size);
					parentblockSize -= sizeof(BlockHeader);
					parentblockSize -= parentheader.size;
					
					// parse the block
					switch (parentheader.type)
					{
					case 'ATAD':
						ReadData(file,&m_CollData,sizeof(MSH_ClothCollisionConfig));
						break;
					case 'RHPS':
						ReadData(file,&m_CollSphereData,sizeof(MSH_ClothCollisionSphere));
						break;
					case 'NLYC':
						ReadData(file,&m_CollCylinderData,sizeof(MSH_ClothCollisionCylinder));
						break;
					case 'ENOC':
						ReadData(file,&m_CollConeData,sizeof(MSH_ClothCollisionCone));
						break;
					default:
						SkipBlock(file, parentheader.size);
						break;
						break;
					}
				}
*/
				break;
				// ------------ Animation trigger collision object ---------------------
			case '1LOC':
				parentblockSize=header.size;
				while (parentblockSize)
				{
					// read a block header
					ReadHeader(file,&parentheader);
					
					// deduct blockSize
					_ASSERTE(parentblockSize >= sizeof(BlockHeader) + parentheader.size);
					parentblockSize -= sizeof(BlockHeader);
					parentblockSize -= parentheader.size;
					
					// parse the block
					switch (parentheader.type)
					{
					case 'ATAD':
						ReadData(file,&m_AnimTriggerCollData,parentheader.size);
						break;
					case 'EMAN':
						ReadString(file,parentheader.size,&m_sCollTriggeredAnimation);
						break;
					case 'RHPS':
						ReadData(file,&m_AnimTriggerCollSphereData,sizeof(MSH_ClothCollisionSphere));
						break;
					case 'NLYC':
						ReadData(file,&m_AnimTriggerCollCylinderData,sizeof(MSH_ClothCollisionCylinder));
						break;
					case 'ENOC':
						ReadData(file,&m_AnimTriggerCollConeData,sizeof(MSH_ClothCollisionCone));
						break;
					default:
						SkipBlock(file, parentheader.size);
						break;
						break;
					}
				}
				break;
			case 'NART':
				ReadData(file,&m_fScaleX,sizeof(float));
				ReadData(file,&m_fScaleY,sizeof(float));
				ReadData(file,&m_fScaleZ,sizeof(float));
				ReadData(file,&m_fQuatRotX,sizeof(float));
				ReadData(file,&m_fQuatRotY,sizeof(float));
				ReadData(file,&m_fQuatRotZ,sizeof(float));
				ReadData(file,&m_fQuatRotW,sizeof(float));
				ReadData(file,&m_fTransX,sizeof(float));
				ReadData(file,&m_fTransY,sizeof(float));
				ReadData(file,&m_fTransZ,sizeof(float));
				break;
				
				
			case 'MOEG':
				geomblockSize=header.size;
				while (geomblockSize)
				{
					// read a block header
					ReadHeader(file,&geomheader);
					
					// deduct blockSize
					_ASSERTE(geomblockSize >= sizeof(BlockHeader) + geomheader.size);
					geomblockSize -= sizeof(BlockHeader);
					geomblockSize -= geomheader.size;
					
					// parse the block
					switch (geomheader.type)
					{
					case 'MGES':
						pSegment = new CMSHBlockModelSegment(&geomheader,m_pData,this);
						pSegment->Read(file);
	                	
						CMSHBlockModelSegment **pNewList;
						pNewList = new CMSHBlockModelSegment *[m_nModelSegments+1];
						if(m_ppModelSegments)
						{
							memcpy(pNewList,m_ppModelSegments, m_nModelSegments*sizeof(CMSHBlockModelSegment *));
							delete[] m_ppModelSegments;
						}
						pNewList[m_nModelSegments]=pSegment;
						m_ppModelSegments = pNewList;
						m_nModelSegments++;
						break;
						
					case 'XOBB':
						m_HasBBox=true;
						ReadData(file,&m_fBBoxRX,sizeof(float));
						ReadData(file,&m_fBBoxRY,sizeof(float));
						ReadData(file,&m_fBBoxRZ,sizeof(float));
						ReadData(file,&m_fBBoxRW,sizeof(float));
						ReadData(file,&m_fBBoxTX,sizeof(float));
						ReadData(file,&m_fBBoxTY,sizeof(float));
						ReadData(file,&m_fBBoxTZ,sizeof(float));
						ReadData(file,&m_fBBoxDX,sizeof(float));
						ReadData(file,&m_fBBoxDY,sizeof(float));
						ReadData(file,&m_fBBoxDZ,sizeof(float));
						ReadData(file,&m_fBBoxDW,sizeof(float));
						break;
					
					case 'LVNE':
						ReadData(file, &m_envelopsCount,sizeof(int));
						if(m_envelopArray)
							delete[] m_envelopArray;
						m_envelopArray	= ZeroNew (int[m_envelopsCount]);
						ReadData(file, m_envelopArray, sizeof(int) * m_envelopsCount);
						break;

					default:
						SkipBlock(file, geomheader.size);
						break;
					}
				}
				break;
				
			case 'HTLC':
				ParseCloth(file,&header);
				break;

			case 'ICWS':
				// Special SpecWarrior collision info block --DY
				{
					m_bIsSWCICollisionObject = true;
					unsigned int cnv = 0;
					ReadData(file, &cnv, sizeof(unsigned int));
					m_SWCICollisionType = (SWCI_CollisionType)cnv;
					ReadData(file, &m_fSWCI_Radius, sizeof(float));
					ReadData(file, &m_fSWCI_Height, sizeof(float));
					ReadData(file, &m_fSWCI_Length, sizeof(float));
				}
				break;
				
			case 'XFRP':
			case 'TSLC':
			case 'SOPB':
			case 'DI_M':
			default:
				SkipBlock(file, header.size);
				break;
			}
		}
}



void CMSHBlockModel::ParseCloth(FILE *file, BlockHeader *pHdr)
{
	unsigned int blockSize=pHdr->size;
	
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
		    case 'ATAD':
				 {
					 MSH_ClothConfig clothData;
					 unsigned int clothBlockSize = header.size;
					 
					 clothData.bClothEnabled = 1;
					 
					 ReadData(file, &clothData, sizeof(MSH_ClothConfig));
					 clothBlockSize -= sizeof(MSH_ClothConfig);
					 SetClothData(clothData);
					 
					 fseek(file, clothBlockSize, SEEK_CUR);
				 }
				 break;
				 
			 case 'LXIF':
				 {
					 int nIndices;
					 int *pIndices;
					 
					 ReadData(file, &nIndices, sizeof(int));
					 pIndices = new int[nIndices];
					 ReadData(file, pIndices, nIndices*sizeof(int));
				 }
				 break;
				 
			 case 'HSEM':
				 {
					 if(m_nModelSegments)
					 {
//						 m_ppModelSegments[0]->ParseCloth(file);
						 SkipBlock(file,header.size);
					 }
					 else
					 {
						 _ASSERTE(0);
						 SkipBlock(file,header.size);
					 }
				 }
				 break;

			 default:
				 SkipBlock(file,header.size);
				 break;
		}
	}
}



void CMSHBlockModel::WriteAnimTriggerCollisionObject(FILE *file)
{
	if(m_AnimTriggerCollData.config.bCollisionEnabled) // animation trigger collision object
	{
		StartBlock(file, 'COL1');

		StartBlock(file, 'DATA');
		WriteData(file,&m_AnimTriggerCollData,sizeof(MSH_AnimTriggerCollisionConfig));
		EndBlock(file);
		
		StartBlock(file, 'NAME');
		WriteString(file, m_sName);
		EndBlock(file);

		StartBlock(file, 'ANIM');
		strlwr(m_sCollTriggeredAnimation);
		for(unsigned i=0;i<strlen(m_sCollTriggeredAnimation);i++)
		{
			int keepChar;
			keepChar=isalnum(m_sCollTriggeredAnimation[i])|(m_sCollTriggeredAnimation[i]=='_');
			if(!keepChar)
				m_sCollTriggeredAnimation[i]=0;
		}
		WriteString(file, m_sCollTriggeredAnimation);
		EndBlock(file);
		
		switch(m_AnimTriggerCollData.config.sType)
		{
		case COLLISION_SPHERE:
			StartBlock(file, 'SPHR');
			WriteData(file,&m_AnimTriggerCollSphereData,sizeof(MSH_ClothCollisionSphere));
			EndBlock(file);
			break;
		case COLLISION_CYLINDER:
			StartBlock(file, 'CYLN');
			WriteData(file,&m_AnimTriggerCollCylinderData,sizeof(MSH_ClothCollisionCylinder));
			EndBlock(file);
			break;
		case COLLISION_CONE:
			StartBlock(file, 'CONE');
			WriteData(file,&m_AnimTriggerCollConeData,sizeof(MSH_ClothCollisionCone));
			EndBlock(file);
			break;
		}

		StartBlock(file, 'TORS');
		WriteData(file,&m_TorsoPosAtHit,sizeof(vect3));
		WriteData(file,&m_TorsoFrontAtHit,sizeof(vect3));
		EndBlock(file);

		EndBlock(file);
	}
}

void CMSHBlockModel::Write(FILE *file)
{
	StartBlock(file,'MODL');
	
	StartBlock(file, 'MTYP');
	unsigned type = GetModelType();
	if(IsClothEnabled())
		type = Cloth_Node;
	
	WriteData(file, &type, sizeof(int));
	EndBlock(file);
	
	if (matrixPaletteIndex >= 0) {
    		StartBlock(file, 'MNDX');
			WriteData(file, &matrixPaletteIndex, sizeof(int));
			EndBlock(file);
	}
	
	
	_ASSERTE (m_sName);
	StartBlock(file, 'NAME');
	WriteString(file, m_sName);
	EndBlock(file);
	
	if (m_sParentName) {
		StartBlock(file, 'PRNT');
		WriteString(file, m_sParentName);
		EndBlock(file);
	}
	
	unsigned flags=0;
	if(GetHidden())
		flags|=1;
	if(GetDynamicallyLit())
		flags|=2;
	if(GetRetainNormals())
		flags|=4;
	if(GetRenderAfterShadows())
		flags|=8;
	if(GetLeaveChildGeometry())
		flags|=16;
	if(GetPS2Optimize())
		flags|=32;
	if (flags) 
	{
    	StartBlock(file, 'FLGS');
		WriteData(file, &flags, sizeof(int));
		EndBlock(file);
	}
	
	
	StartBlock(file, 'TRAN');
		WriteData(file,&m_fScaleX,sizeof(float));
		WriteData(file,&m_fScaleY,sizeof(float));
		WriteData(file,&m_fScaleZ,sizeof(float));
		WriteData(file,&m_fQuatRotX,sizeof(float));
		WriteData(file,&m_fQuatRotY,sizeof(float));
		WriteData(file,&m_fQuatRotZ,sizeof(float));
		WriteData(file,&m_fQuatRotW,sizeof(float));
		WriteData(file,&m_fTransX,sizeof(float));
		WriteData(file,&m_fTransY,sizeof(float));
		WriteData(file,&m_fTransZ,sizeof(float));
	EndBlock(file);
			
	if(m_nModelSegments) 
	{
		StartBlock(file, 'GEOM');
		{
			if(m_HasBBox) 
			{
				StartBlock(file,'BBOX');
					WriteData(file,&m_fBBoxRX,sizeof(float));
					WriteData(file,&m_fBBoxRY,sizeof(float));
					WriteData(file,&m_fBBoxRZ,sizeof(float));
					WriteData(file,&m_fBBoxRW,sizeof(float));
					WriteData(file,&m_fBBoxTX,sizeof(float));
					WriteData(file,&m_fBBoxTY,sizeof(float));
					WriteData(file,&m_fBBoxTZ,sizeof(float));
					WriteData(file,&m_fBBoxDX,sizeof(float));
					WriteData(file,&m_fBBoxDY,sizeof(float));
					WriteData(file,&m_fBBoxDZ,sizeof(float));
					WriteData(file,&m_fBBoxDW,sizeof(float));
				EndBlock(file);
			}
			if(!IsClothEnabled()/*GetModelType() != Cloth_Node*/)
			{
      			for(int i=0;i<m_nModelSegments;i++) 
				{
						m_ppModelSegments[i] ->Write(file);
						//m_ppModelSegments[i]->GetVertexWeightsEnvelop().
      			}
			}
			/*else
			{
				for(int i=0;i<m_nModelSegments;i++) 
				{
						m_ppModelSegments[i] ->Write(file);
						//m_ppModelSegments[i]->GetVertexWeightsEnvelop().
      			}
			}*/


			if (m_envelopsCount) 
			{
				StartBlock(file,'ENVL');
					WriteData(file, &m_envelopsCount,sizeof(int));
					WriteData(file, m_envelopArray, sizeof(int) * m_envelopsCount);
				EndBlock(file);
			}

			if (IsClothEnabled()/*GetModelType() == Cloth_Node*/) {
				_ASSERTE (m_nModelSegments == 1);
                
				SetFixedPointsWeights();
				WriteCloth (file);
			}
		}
		EndBlock(file);
	}
			
	if (m_bIsSWCICollisionObject)
	{
		StartBlock(file, 'SWCI');
		// convert enum to unsigned before we write out
		unsigned int cnv = (unsigned int)m_SWCICollisionType;
		WriteData(file, &cnv, sizeof(unsigned int));
		WriteData(file, &m_fSWCI_Radius, sizeof(float));
		WriteData(file, &m_fSWCI_Height, sizeof(float));
		WriteData(file, &m_fSWCI_Length, sizeof(float));
		EndBlock(file);
	}

//	WriteClothCollisionObject(file);
	// Note: AnimTriggerCollisionObjects are stored separately from the MODL because
	//	their use is application-specific.
			
			
	EndBlock(file);
}

void CMSHBlockModel::WriteText(FILE *fp)
{
	//'MODL'
	fprintf(fp,"\nModel\n{\n");
	
	// write model type
	switch (modelType) {
	case Static_Node:
		{
			if(IsClothEnabled())
				fprintf(fp,"\tModelType{static} (with Cloth Enabled)\n");
			else
				fprintf(fp,"\tModelType{static}\n");
			break;
		}
		
	case Skin_Node:
		{
			if(IsClothEnabled())
				fprintf(fp,"\tModelType{skin} (with Cloth Enabled)\n");
			else
				fprintf(fp,"\tModelType{skin}\n");

			break;
		}
		
	case Envelop_Node:
		fprintf(fp,"\tModelType{envelop}\n");
		break;
		
	case Cloth_Node:
		fprintf(fp,"\tModelType{cloth}\n");
		break;
		
	case Child_Skin_Node:
		fprintf(fp,"\tModelType{child_skin}\n");
		break;

		
	case Null_Node:
	default:
		fprintf(fp,"\tModelType{null}\n");
		break;
	}
	
	
	//'NAME'
	_ASSERTE (m_sName);
	fprintf(fp,"\tName{%s}\n",m_sName);
	
	if (m_sParentName) {
		fprintf(fp,"\tParent{%s}\n", m_sParentName);
	}
	
	
	if (matrixPaletteIndex >= 0) {
		fprintf(fp,"\tmatrixPaletteIndex{%d}\n", matrixPaletteIndex );
	}
	
	if (GetHidden()) {
		fprintf(fp,"\tFlags {Hidden}\n");
	}
	if (GetDynamicallyLit()) {
		fprintf(fp,"\tFlags {DynamicallyLit}\n");
	}
	if (GetRetainNormals()) {
		fprintf(fp,"\tFlags {RetainNormals}\n");
	}
	if (GetRenderAfterShadows()) {
		fprintf(fp,"\tFlags {RenderAfterShadows}\n");
	}
	if (GetLeaveChildGeometry()) {
		fprintf(fp,"\tFlags {Don't Flatten Geometry}\n");
	}
	if (GetPS2Optimize()) {
		fprintf(fp,"\tFlags {Optimize for PS2}\n");
	}
	
	//'TRAN'
	ZeroQuaternion r(m_fQuatRotW,m_fQuatRotX,m_fQuatRotY, m_fQuatRotZ);
	ZeroMatrix m (r, ZeroVector3 (0, 0, 0));
	ZeroVector3 e (m.CalcPitchYawRollEulerAngles());

	fprintf(fp,"\tTransform\n\t{\n");
	fprintf(fp,"\t\tScale{%f,%f,%f}\n",m_fScaleX, m_fScaleY, m_fScaleZ);
	fprintf(fp,"\t\tRotation{%f,%f,%f}\n",RAD2DEG*e.x,  RAD2DEG*e.y,  RAD2DEG*e.z);
	fprintf(fp,"\t\tTranslation{%f,%f,%f}\n",m_fTransX, m_fTransY, m_fTransZ);
	fprintf(fp,"\t}\n");
	
	
	if(m_nModelSegments)
	{
		int i;
		fprintf(fp,"\tGeometry\n\t{\n");
		
		//'BBOX'
		if(m_HasBBox) {
			fprintf(fp,"\t\tBoundingBox\n");
			fprintf(fp,"\t\t{\n");
			fprintf(fp,"\t\t\tRotation{%0.3f,%0.3f,%0.3f,%0.3f}\n",m_fBBoxRX,m_fBBoxRY, m_fBBoxRZ, m_fBBoxRW);
			fprintf(fp,"\t\t\tTranslation{%0.3f,%0.3f,%0.3f}\n",m_fBBoxTX, m_fBBoxTY, m_fBBoxTZ);
			fprintf(fp,"\t\t\tDimension{%0.3f,%0.3f,%0.3f,%0.3f}\n",m_fBBoxDX, m_fBBoxDY, m_fBBoxDZ,m_fBBoxDW);
			fprintf(fp,"\t\t}\n");
		}
		
		if (m_envelopsCount) 
		{
			fprintf(fp,"\t\tEnvelopes\n");
			fprintf(fp,"\t\t{\n");
			fprintf(fp,"\t\t\t");
			for(i=0;i<m_envelopsCount;i++)
			{
				fprintf(fp,"%d ",m_envelopArray[i]);
			}
			fprintf(fp,"\n\t\t}\n");
		}
		if(1/*GetModelType() != Cloth_Node*/)
		{
			fprintf(fp,"\t\tNumModelSegments{%d}\n",m_nModelSegments);
		        
			for(i=0;i<m_nModelSegments;i++)
			{
				m_ppModelSegments[i] ->WriteText(fp);
			}
		}

		if (m_envelopsCount) {
			fprintf(fp,"\t\tenvelops {%d}\n", m_envelopsCount);
			fprintf(fp,"\t\t\t");
			for (i = 0; i < m_envelopsCount; i ++) {
				fprintf (fp, "%d ", m_envelopArray[i]);
			}
			fprintf(fp,"\n");
			fprintf(fp,"\t\t}\n");
		}

		if (IsClothEnabled()/*GetModelType() == Cloth_Node*/) {

			SetFixedPointsWeights();
			WriteClothText (fp);
		}

		fprintf(fp,"\t}\n");
	}

	fprintf(fp,"}\n");
}


void CMSHBlockModel::SetCollisionSphere()
{
 	memset(&m_CollSphereData,0,sizeof(MSH_ClothCollisionSphere));
	m_CollData.bCollisionEnabled = 1;
	m_CollData.sType = COLLISION_SPHERE;
	
}

void CMSHBlockModel::SetCollisionCylinder()
{
	memset(&m_CollCylinderData, 0, sizeof(MSH_ClothCollisionCylinder));
	m_CollData.bCollisionEnabled = 1;
	m_CollData.sType = COLLISION_CYLINDER;
}

void CMSHBlockModel::SetCollisionCone()
{
	memset(&m_CollConeData, 0, sizeof(MSH_ClothCollisionCone));
	m_CollData.bCollisionEnabled = 1;
	m_CollData.sType = COLLISION_CONE;
}

void CMSHBlockModel::SetAnimTriggerCollisionSphere(MSH_ClothCollisionSphere *collSphere, char *sName, int iLateStart, float fTime, int frameMin, int frameMax, float toleranceMin, float toleranceMax, float recoveryTime, vect3 torsoPos, vect3 torsoFront)
{
	m_AnimTriggerCollData.config.bCollisionEnabled = 1;
	m_AnimTriggerCollData.config.sType = COLLISION_SPHERE;
	
	memcpy(&m_AnimTriggerCollSphereData,collSphere,sizeof(MSH_ClothCollisionSphere));
	
	m_sCollTriggeredAnimation = new char[strlen(sName)+1];
	strcpy(m_sCollTriggeredAnimation,sName);
	m_AnimTriggerCollData.iLateStartFrame = iLateStart;
	m_AnimTriggerCollData.fTime = fTime;
	m_AnimTriggerCollData.iFrameMin = frameMin;
	m_AnimTriggerCollData.iFrameMax = frameMax;
	m_AnimTriggerCollData.fToleranceMin = toleranceMin;
	m_AnimTriggerCollData.fToleranceMax = toleranceMax;
	m_AnimTriggerCollData.fRecoveryTime = recoveryTime;
	m_TorsoPosAtHit=torsoPos;
	m_TorsoFrontAtHit=torsoFront;
}

void CMSHBlockModel::SetAnimTriggerCollisionCylinder(MSH_ClothCollisionCylinder *collCylinder, char *sName, int iLateStart, float fTime, int frameMin, int frameMax, float toleranceMin, float toleranceMax, float recoveryTime, vect3 torsoPos, vect3 torsoFront)
{
	m_AnimTriggerCollData.config.bCollisionEnabled = 1;
	m_AnimTriggerCollData.config.sType = COLLISION_CYLINDER;
	
	memcpy(&m_AnimTriggerCollCylinderData,collCylinder,sizeof(MSH_ClothCollisionCylinder));
	
	m_sCollTriggeredAnimation = new char[strlen(sName)+1];
	strcpy(m_sCollTriggeredAnimation,sName);
	m_AnimTriggerCollData.iLateStartFrame = iLateStart;
	m_AnimTriggerCollData.fTime = fTime;
	m_AnimTriggerCollData.iFrameMin = frameMin;
	m_AnimTriggerCollData.iFrameMax = frameMax;
	m_AnimTriggerCollData.fToleranceMin = toleranceMin;
	m_AnimTriggerCollData.fToleranceMax = toleranceMax;
	m_AnimTriggerCollData.fRecoveryTime = recoveryTime;
	m_TorsoPosAtHit=torsoPos;
	m_TorsoFrontAtHit=torsoFront;
}

void CMSHBlockModel::SetAnimTriggerCollisionCone(MSH_ClothCollisionCone *collCone, char *sName, int iLateStart, float fTime, int frameMin, int frameMax, float toleranceMin, float toleranceMax, float recoveryTime, vect3 torsoPos, vect3 torsoFront)
{
	m_AnimTriggerCollData.config.bCollisionEnabled = 1;
	m_AnimTriggerCollData.config.sType = COLLISION_CONE;
	
	memcpy(&m_AnimTriggerCollConeData,collCone,sizeof(MSH_ClothCollisionCone));
	
	m_sCollTriggeredAnimation = new char[strlen(sName)+1];
	strcpy(m_sCollTriggeredAnimation,sName);
	m_AnimTriggerCollData.iLateStartFrame = iLateStart;
	m_AnimTriggerCollData.fTime = fTime;
	m_AnimTriggerCollData.iFrameMin = frameMin;
	m_AnimTriggerCollData.iFrameMax = frameMax;
	m_AnimTriggerCollData.fToleranceMin = toleranceMin;
	m_AnimTriggerCollData.fToleranceMax = toleranceMax;
	m_AnimTriggerCollData.fRecoveryTime = recoveryTime;
	m_TorsoPosAtHit=torsoPos;
	m_TorsoFrontAtHit=torsoFront;
}


void CMSHBlockModel::RotateTriggerBox(ZeroMatrix &matrix,ZeroVector3 startPos)
{
	if(m_AnimTriggerCollData.config.bCollisionEnabled)
	{
		ZeroVector3 pos,front;
		pos=ZeroVector3(m_TorsoPosAtHit.fX,m_TorsoPosAtHit.fY,m_TorsoPosAtHit.fZ);
		front=ZeroVector3(m_TorsoFrontAtHit.fX,m_TorsoFrontAtHit.fY,m_TorsoFrontAtHit.fZ);
		pos=matrix.TransformVector(pos);
		front=matrix.RotateVector(front);
//		pos=parentMatrix.TransformVector(pos);
//		front=parentMatrix.TransformVector(front);
		m_TorsoPosAtHit.fX=pos.x-startPos.x;
		m_TorsoPosAtHit.fY=pos.y-startPos.y;
		m_TorsoPosAtHit.fZ=pos.z-startPos.z;

		m_TorsoFrontAtHit.fX=front.x;
		m_TorsoFrontAtHit.fY=front.y;
		m_TorsoFrontAtHit.fZ=front.z;
	}
}

void CMSHBlockModel::LeftToRightTriggerBox()
{
	if(m_AnimTriggerCollData.config.bCollisionEnabled)
	{
		m_TorsoPosAtHit.fZ=-m_TorsoPosAtHit.fZ;
		m_TorsoFrontAtHit.fZ=-m_TorsoFrontAtHit.fZ;
	}
}

void CMSHBlockModel::SetPrefix(char *sPrefix)
{
}

void CMSHBlockModel::SetName(char *sName)
{
	if(m_sName) {
		delete[] m_sName;
	}
	m_sName = new char[strlen(sName)+1];
	strcpy(m_sName,sName);
	CalcStrCRC (m_sName);
}

void CMSHBlockModel::SetParentPrefix(char *sParentPrefix)
{
}

void CMSHBlockModel::SetParentName(char *sParentName)
{
	if(m_sParentName)
		delete[] m_sParentName;
	m_sParentName = new char[strlen(sParentName)+1];
	strcpy(m_sParentName,sParentName);
}

void CMSHBlockModel::SetScale(float fX, float fY, float fZ)
{
	m_fScaleX = fX;
	m_fScaleY = fY;
	m_fScaleZ = fZ;
}

void CMSHBlockModel::SetRotation(float fX, float fY, float fZ, float fW)
{
	float magInv;
	magInv = 1.0f / (fX * fX + fY * fY + fZ * fZ + fW * fW);
	m_fQuatRotX = fX * magInv;
	m_fQuatRotY = fY * magInv;
	m_fQuatRotZ = fZ * magInv;
	m_fQuatRotW = fW * magInv;
}

void CMSHBlockModel::SetTranslation(float fX, float fY, float fZ)
{
	m_fTransX = fX;
	m_fTransY = fY;
	m_fTransZ = fZ;
}


void CMSHBlockModel::AddSegment(CMSHBlockModelSegment *pSegment)
{
	CMSHBlockModelSegment **pNewList;

	pNewList = new CMSHBlockModelSegment *[m_nModelSegments+1];
	if(m_ppModelSegments) {
		memcpy(pNewList,m_ppModelSegments, m_nModelSegments*sizeof(CMSHBlockModelSegment *));
		delete[] m_ppModelSegments;
	}

	pSegment->SetParent (this);

	pNewList[m_nModelSegments]=pSegment;
	m_ppModelSegments = pNewList;
	m_nModelSegments++;
}


CMSHBlockModelSegment *CMSHBlockModel::CreateSegment()
{
	CMSHBlock::BlockHeader hdr;
	CMSHBlockModelSegment *pSegment;

	hdr.size=0;
	pSegment = new CMSHBlockModelSegment(&hdr,m_pData,this);
	return pSegment;
}

CMSHBlockModelSegment *CMSHBlockModel::CreateAddSegment()
{
	CMSHBlockModelSegment *pSegment;

	pSegment = CreateSegment();
	AddSegment (pSegment);
	return pSegment;
}


void CMSHBlockModel::RemoveSegment(CMSHBlockModelSegment *pSegment)
{
	int i;

	if (m_nModelSegments) {
		for(i = 0; i < m_nModelSegments - 1; i++) {
			if(m_ppModelSegments[i]==pSegment) {
				memcpy (&m_ppModelSegments[i], &m_ppModelSegments[i + 1], (m_nModelSegments - i - 1) * sizeof (CMSHBlockModelSegment*)); 
				break;
			}
		}
		m_nModelSegments --;
		if (!m_nModelSegments) {
			delete[] m_ppModelSegments;
			m_ppModelSegments	= NULL;
		}
	}
}



void CMSHBlockModel::SetBBoxRotation(float fX,float fY, float fZ, float fW)
{
	m_HasBBox = true;
	m_fBBoxRX=fX;
	m_fBBoxRY=fY;
	m_fBBoxRZ=fZ;
	m_fBBoxRW=fW;
}

void CMSHBlockModel::SetBBoxTranslation(float fX,float fY, float fZ)
{
	m_HasBBox = true;
	m_fBBoxTX=fX;
	m_fBBoxTY=fY;
	m_fBBoxTZ=fZ;
}

void CMSHBlockModel::SetBBoxDimension(float fX,float fY, float fZ, float fW)
{
	m_HasBBox = true;
	m_fBBoxDX=fX;
	m_fBBoxDY=fY;
	m_fBBoxDZ=fZ;
	m_fBBoxDW=fW;
}

void CMSHBlockModel::SetClothData(const MSH_ClothConfig &newData)
{
	m_ClothData = newData;
}

void CMSHBlockModel::SetClothFixedPoints(
	int nIndices, 
	const int pIndices[])
{
	if(m_pClothFixedIndices) {
		delete[] m_pClothFixedIndices;
		delete[] m_FixedPointBoneName;
	}

	m_pClothFixedIndices = NULL;
	m_nClothFixedPoints = nIndices;
	if(nIndices) {
		int i;
		m_pClothFixedIndices = new int[nIndices];
		m_FixedPointBoneName = new char*[nIndices];
		for (i = 0; i < nIndices; i ++) {
			m_pClothFixedIndices[i] = pIndices[i];
		}
		for (i = 0; i < nIndices; i ++) {
			m_FixedPointBoneName[i] = NULL;
		}
	}
}

void CMSHBlockModel::SetUserCloth(int nIndices, const int pIndicesFirst[], const int pIndicesSecond[])
{
	if(m_pUserClothFirst) {
		delete[] m_pUserClothFirst;
	}
	if(m_pUserClothSecond) {
		delete[] m_pUserClothSecond;
	}

	m_pUserClothFirst = NULL;
	m_pUserClothSecond = NULL;
	m_nUserCloth = 0;
	if(nIndices > 0) {
		int i;
		m_nUserCloth = nIndices;

		m_pUserClothFirst = new int[nIndices];
		m_pUserClothSecond = new int[nIndices];
		for (i = 0; i < nIndices; i ++) {
			m_pUserClothFirst[i] = pIndicesFirst[i];
			m_pUserClothSecond[i] = pIndicesSecond[i];
		}
	}
}


MSH_ClothConfig *CMSHBlockModel::GetClothData()
{
	return &m_ClothData;
}

int *CMSHBlockModel::GetClothFixedPoints(int &nIndices)
{
	nIndices = m_nClothFixedPoints;
	return m_pClothFixedIndices;
}



void CMSHBlockModel::GetClothCollision(MSH_ClothCollisionSphere **collSphere,MSH_ClothCollisionCylinder **collCylinder,MSH_ClothCollisionCone **collCone)
{
	*collSphere = NULL;
	*collCylinder = NULL;
	*collCone = NULL;
	if(m_CollData.bCollisionEnabled)
	{
		switch(m_CollData.sType)
		{
		case COLLISION_SPHERE:
			*collSphere = &m_CollSphereData;
			break;
		case COLLISION_CYLINDER:
			*collCylinder = &m_CollCylinderData;
			break;
		case COLLISION_CONE:
			*collCone = &m_CollConeData;
			break;
		}
	}
}

void CMSHBlockModel::GetAnimTriggerCollision(MSH_ClothCollisionSphere **collSphere,MSH_ClothCollisionCylinder **collCylinder,MSH_ClothCollisionCone **collCone)
{
	*collSphere = NULL;
	*collCylinder = NULL;
	*collCone = NULL;
	if(m_AnimTriggerCollData.config.bCollisionEnabled)
	{
		switch(m_AnimTriggerCollData.config.sType)
		{
		case COLLISION_SPHERE:
			*collSphere = &m_AnimTriggerCollSphereData;
			break;
		case COLLISION_CYLINDER:
			*collCylinder = &m_AnimTriggerCollCylinderData;
			break;
		case COLLISION_CONE:
			*collCone = &m_AnimTriggerCollConeData;
			break;
		}
	}
}

char *CMSHBlockModel::GetCollTriggeredAnimation()
{
	return m_sCollTriggeredAnimation;
}

int CMSHBlockModel::GetAnimTriggerLateStart()
{
	return m_AnimTriggerCollData.iLateStartFrame;
}

float CMSHBlockModel::GetAnimTriggerTime()
{
	return m_AnimTriggerCollData.fTime;
}

int CMSHBlockModel::GetAnimTriggerHitFrameMin()
{
	return m_AnimTriggerCollData.iFrameMin;
}

int CMSHBlockModel::GetAnimTriggerHitFrameMax()
{
	return m_AnimTriggerCollData.iFrameMax;
}

float CMSHBlockModel::GetAnimTriggerToleranceMin()
{
	return m_AnimTriggerCollData.fToleranceMin;
}

float CMSHBlockModel::GetAnimTriggerToleranceMax()
{
	return m_AnimTriggerCollData.fToleranceMax;
}

float CMSHBlockModel::GetAnimTriggerRecoveryTime()
{
	return m_AnimTriggerCollData.fRecoveryTime;
}

char *CMSHBlockModel::GetPrefix()
{
	return PREFIX_NAME;
}

char *CMSHBlockModel::GetName()
{
	return m_sName;
}

char *CMSHBlockModel::GetParentPrefix()
{
	return PREFIX_NAME;
}

char *CMSHBlockModel::GetParentName()
{
	return m_sParentName;
}

void CMSHBlockModel::GetScale(float &fX, float &fY, float &fZ)
{
	fX=m_fScaleX;
	fY=m_fScaleY;
	fZ=m_fScaleZ;
}

void CMSHBlockModel::GetQuatRot(float &fX, float &fY, float &fZ, float &fW)
{
	fX=m_fQuatRotX;
	fY=m_fQuatRotY;
	fZ=m_fQuatRotZ;
	fW=m_fQuatRotW;
}

void CMSHBlockModel::GetTrans(float &fX, float &fY, float &fZ)
{
	fX=m_fTransX;
	fY=m_fTransY;
	fZ=m_fTransZ;
}


CMSHBlockModelSegment **CMSHBlockModel::GetSegments(int &nSegments)
{
	nSegments=m_nModelSegments;
	return m_ppModelSegments;
}




bool CMSHBlockModel::HasBBox()
{
	return m_HasBBox;
}

void CMSHBlockModel::GetBBoxRotation(float &fX,float &fY, float &fZ, float &fW)
{
	fX = m_fBBoxRX;
	fY = m_fBBoxRY;
	fZ = m_fBBoxRZ;
	fW = m_fBBoxRW;
}

void CMSHBlockModel::GetBBoxTranslation(float &fX,float &fY, float &fZ)
{
	fX = m_fBBoxTX;
	fY = m_fBBoxTY;
	fZ = m_fBBoxTZ;
}

void CMSHBlockModel::GetBBoxDimension(float &fX,float &fY, float &fZ, float &fW)
{
	fX = m_fBBoxDX;
	fY = m_fBBoxDY;
	fZ = m_fBBoxDZ;
	fW = m_fBBoxDW;
}



void CMSHBlockModelSegment::Splite(ZeroVisList<CMSHBlockModelSegment*>& segmentListOut)
{
	int n;
	int faceCount;
	int vertexCount;
	int vertexSizeInBytes;
	
	faceCount = GetPolygonList()->GetTriangleCount();
	
	GetPositions(vertexCount);
	

	vertexSizeInBytes = sizeof (ZeroVector3);
	if (GetNormals(n)) {
		vertexSizeInBytes  += sizeof (ZeroVector3);
	}
	
	if (GetColors(n)) {
		vertexSizeInBytes  += sizeof (ZeroColor);
	}
	
	if (GetLightColors(n)) {
		vertexSizeInBytes  += sizeof (ZeroColor);
	}
	
	if (GetUVCoords(n,0)) {
		vertexSizeInBytes  += sizeof (ZeroUV);
	}
	
	if (GetUVCoords(n,1)) {
		vertexSizeInBytes  += sizeof (ZeroUV);
	}
	
	if (GetVertexWeightsEnvelop().GetCount()) {
		vertexSizeInBytes += sizeof (ZeroWeight);
	}
	
	maxFaceCount = MAX_TRIANGLE_PER_SEGMENT;
	maxVetexCount = MAX_VERTEX_PER_SEGMENT_IN_BYTES / vertexSizeInBytes;
	
	if ((vertexCount > maxVetexCount) || (faceCount > maxFaceCount)){
		CMSHBlockModelSegment* segA;
		CMSHBlockModelSegment* segB;
		Splite(&segA, &segB);
		
		delete this;
		segA->Splite(segmentListOut);
		segB->Splite(segmentListOut);
	} else {
		segmentListOut.Append (this);
	}
}


void CMSHBlockModelSegment::SeparateFaces(
	int color,
	ZeroPolyhedra& polyhedra, 
	ZeroEdge *start, 
	ZeroPolyhedra& output,
	int* vertexMark,
	int& vertexCountOut,
	int& faceCountOut)
{
	int stack;
	int indexCount;
	int vCount;
	int fCount;

	ZeroEdge *ptr;
	ZeroEdge *edge;
	ZeroStack<int> index (polyhedra.GetCount() / 2);
	ZeroStack<ZeroEdge*> pool (polyhedra.GetCount() / 2);

	
	pool[0] = start;
	stack = 1;
	while (stack) {
		stack --;
		edge = pool[stack];
		
		if (edge->mark != color) { 
			
			vCount = faceCountOut;
			fCount = vertexCountOut;
			ptr = edge;
			do {
				if (!vertexMark[ptr->incidentVertex]) {
					vCount ++;
				}
				fCount ++;
				ptr = ptr->next;
			} while (ptr != edge);
			fCount -= 2;
			if ((vCount > maxVetexCount) || (fCount > maxFaceCount)) {
				return;
			}
			
			faceCountOut = vCount; 
			vertexCountOut = fCount; 
			
			
			indexCount = 0;
			ptr = edge;
			do {
				if (ptr->twin->incidentFace >= 0) {
					pool[stack] = ptr->twin;
					stack ++;
				}
				ptr->mark = color;
				
				if (!vertexMark[ptr->incidentVertex]) {
					vertexMark[ptr->incidentVertex] = 1;
				}
				
				index[indexCount] = ptr->incidentVertex;

				indexCount ++;
				ptr = ptr->next;
			} while (ptr != edge);
			
			output.AddFace (indexCount, &index[0]);
		}
	}
}

void CMSHBlockModelSegment::Splite(
	CMSHBlockModelSegment** segA, 
	CMSHBlockModelSegment** segB)
{

	int i;
	int edgeColor;
	int faceCountOut;
	int vertexCountOut;


	ZeroPolyhedra polyhedra;
	ZeroPolyhedra polyhedraA;
	ZeroPolyhedra polyhedraB;
	
	
	CMSHBlockPolygonList::Iterator polyListIter (m_pPolygonList);
	for (polyListIter.Begin(); polyListIter; polyListIter ++) {
		int indexCount;
		MSHBLOCKPolygon& polygon = (*polyListIter)->info;
		ZeroStack<int> index(polygon.GetCount());
		
		indexCount = 0;
		MSHBLOCKPolygon::Iterator polyIter (polygon);
		for (polyIter.Begin(); polyIter; polyIter ++) {
			index[indexCount] = *polyIter;
			indexCount ++;
		}
		polyhedra.AddFace (indexCount, &index[0]);
		
	}
	polyhedra.EndFace();
	
	ZeroStack<int> vertexMark(m_nPositions);
	
	faceCountOut = 0;
	vertexCountOut = 0;
	memset (&vertexMark[0], 0, m_nPositions * sizeof (int)); 
	
	edgeColor = polyhedra.IncLRU();
	polyhedraA.BeginFace();
	polyhedraB.BeginFace();
	ZeroEdge *edge;
	ZeroPolyhedra::Iterator iter (polyhedra);  
	for (iter.Begin(); iter; iter ++) {
		edge = &(*iter)->info;
		if (edge->mark != edgeColor) {
			if (edge->incidentFace > 0) {
				SeparateFaces(edgeColor, polyhedra, edge, polyhedraA, &vertexMark[0], vertexCountOut, faceCountOut);
				if ((vertexCountOut > maxVetexCount) || (faceCountOut > maxFaceCount)) {
					break;
				}
			}
		}
	}
	
	ZeroEdge *ptr;
	for (iter.Begin(); iter; iter ++) {
		edge = &(*iter)->info;
		if (edge->mark != edgeColor) {
			if (edge->incidentFace > 0) {
				i = 0;
				ptr = edge;
				do {
					ptr->mark = edgeColor;
					vertexMark[i] = ptr->incidentVertex;
					i ++;
					ptr = ptr->next;
				} while (ptr != edge);
				polyhedraB.AddFace (i, &vertexMark[0]);
			}
		}
	}
	
	polyhedraA.EndFace();
	polyhedraB.EndFace();
	
	
	*segA = new CMSHBlockModelSegment (*this, polyhedraA,m_pMyModel);
	*segB = new CMSHBlockModelSegment (*this, polyhedraB,m_pMyModel);
}



void CMSHBlockModelSegment::Merge(CMSHBlockModelSegment* aInSegment)
{

	int i;


	ZeroPolyhedra polyhedra;
	polyhedra.BeginFace();
	CMSHBlockPolygonList::Iterator polyListIter (m_pPolygonList);
	for (polyListIter.Begin(); polyListIter; polyListIter ++) 
	{
		int indexCount;
		MSHBLOCKPolygon& polygon = (*polyListIter)->info;
		ZeroStack<int> index(polygon.GetCount());
		
		indexCount = 0;
		MSHBLOCKPolygon::Iterator polyIter (polygon);
		for (polyIter.Begin(); polyIter; polyIter ++) {
			index[indexCount] = *polyIter;
			indexCount ++;
		}
		polyhedra.AddFace (indexCount, &index[0]);
		
	}

	CMSHBlockPolygonList::Iterator polyListIter2 (aInSegment->m_pPolygonList);
	for (polyListIter2.Begin(); polyListIter2; polyListIter2 ++) 
	{
		int indexCount;
		MSHBLOCKPolygon& polygon = (*polyListIter2)->info;
		ZeroStack<int> index(polygon.GetCount());
		
		indexCount = 0;
		MSHBLOCKPolygon::Iterator polyIter (polygon);
		for (polyIter.Begin(); polyIter; polyIter ++) {
			index[indexCount] = *polyIter + m_nPositions;
			indexCount ++;
		}
		polyhedra.AddFace (indexCount, &index[0]);
		
	}
	polyhedra.EndFace();

	bool hasUVs[MAX_TEXTURES_PER_MSH_MATERIAL];
	int n;
	for(i=0;i<MAX_TEXTURES_PER_MSH_MATERIAL;i++)
		hasUVs[i] = GetUVCoords(n, i) ? true : false;
	vect3 *pos;
	int nPos;
	pos = aInSegment->GetPositions(nPos);

	int nInColors;
	aInSegment->GetColors(nInColors);
	
	if(m_nColors && !nInColors)
	{
		CMSHBlockMaterial *mat=m_pData->GetMaterialSet()->GetMaterialByName(aInSegment->GetMaterialName());
		float fr,fg,fb,fa;
		unsigned char cr,cg,cb,ca;
		if(aInSegment->HasColorBase())
			aInSegment->GetColorBase(cr,cg,cb,ca);
		else
		{
			mat->GetColor(fr,fg,fb,fa);
			fr*=255;
			fg*=255;
			fb*=255;
			fa*=255;
			cr=(unsigned char)fr;
			cg=(unsigned char)fg;
			cb=(unsigned char)fb;
			ca=(unsigned char)fa;
//			cr=cg=cb=ca=255;
		}
		unsigned col=(ca<<24)+(cr<<16)+(cg<<8)+(cb);
		aInSegment->SetNumColors(nPos);
		for(int i=0;i<nPos;i++)
			aInSegment->SetColor(i,col);
	}
	else if(nInColors && !m_nColors)
	{
		CMSHBlockMaterial *mat=m_pData->GetMaterialSet()->GetMaterialByName(GetMaterialName());
		float fr,fg,fb,fa;
		unsigned char cr,cg,cb,ca;
		if(HasColorBase())
			GetColorBase(cr,cg,cb,ca);
		else
		{
			mat->GetColor(fr,fg,fb,fa);
			fr*=255;
			fg*=255;
			fb*=255;
			fa*=255;
			cr=(unsigned char)fr;
			cg=(unsigned char)fg;
			cb=(unsigned char)fb;
			ca=(unsigned char)fa;
//			cr=cg=cb=ca=255;
		}
		unsigned col=(ca<<24)+(cr<<16)+(cg<<8)+(cb);
		SetNumColors(m_nPositions);
		for(int i=0;i<m_nPositions;i++)
			SetColor(i,col);
	}
	
	ZeroStack<MSH_FLATVERTEX> array(m_nPositions+nPos);
	CopyFlatVertex(&array[0]);
	aInSegment->CopyFlatVertex(&array[m_nPositions]);
	SetVertexData(polyhedra,&array[0],m_nNormals!=0, m_nColors!=0, m_nLightColors!=0,envelop.GetCount()!=0,hasUVs);
}



void CMSHBlockModel::SpliteSegments()
{
	int i;
	CMSHBlockModelSegment *segment;
	ZeroVisList<CMSHBlockModelSegment*> segmentList;
	
	if (!m_nModelSegments) {
		return;
	}
	
	for (i = 0; i < m_nModelSegments; i ++) {
		segment = m_ppModelSegments[i];
		segment->Splite(segmentList);
	}
	
  	delete[] m_ppModelSegments;
	
	m_nModelSegments = segmentList.GetCount();
	
	m_ppModelSegments = new CMSHBlockModelSegment*[m_nModelSegments];
	
	i = 0;
	
	ZeroVisList<CMSHBlockModelSegment*>::Iterator iter(segmentList);
	for (iter.Begin(); iter; iter ++) {
		m_ppModelSegments[i] = *iter;
		i ++;
	}
}



void CMSHBlockModel::WriteCloth (FILE *file)
{

	int iSegCount;
	CMSHBlockModelSegment* pSegment = (GetSegments(iSegCount))[0];
	_ASSERTE (iSegCount == 1);
	int index =pSegment->GetData()->GetMaterialSet()->GetMaterialIndexByName(pSegment->GetMaterialName() );
	unsigned int numMaterials;
	CMSHBlockMaterial **ppMaterials = pSegment->GetData()->GetMaterialSet()->GetMaterials(numMaterials);
	int numTextures;
	char* textureName = (ppMaterials[index]->GetTextureFilenames(  numTextures ))[0];


	StartBlock(file, 'CLTH');

    
		//texture Name
		StartBlock(file, 'CTEX');
		if(textureName)
			WriteString(file, textureName);
		else
		{
			WriteString(file, "no_texture");
		}
		EndBlock(file);
        
		// positions
		StartBlock(file, 'CPOS');
			WriteData(file, &m_ClothParticlesCount, sizeof(int));
			WriteData(file, m_ClothParticles, m_ClothParticlesCount * sizeof(vect3));
		EndBlock(file);



		// visible mesh uvs
		StartBlock(file, 'CUV0');
			WriteData(file, &m_ClothParticleToVertexIndexMapCount, sizeof(int));
			WriteData(file, m_ClothUV, m_ClothParticleToVertexIndexMapCount * sizeof(uv));
		EndBlock(file);        		

		// nailed positions
		StartBlock(file, 'FIDX');
			WriteData(file, &m_nClothFixedPoints, sizeof(int));
			WriteData(file, &m_pClothFixedIndices[0], m_nClothFixedPoints * sizeof(float));
		EndBlock(file);

		//skinning data for the nailed positions
		StartBlock(file, 'FWGT');
		if(m_envelopsCount)
		{
			WriteData(file, &m_nClothFixedPoints, sizeof(int));
			for(int i = 0; i < m_nClothFixedPoints; i++)
			{
				WriteString(file, m_FixedPointBoneName[i]);	
			}
		}
		else
		{
			int ii = 0;
			WriteData(file, &ii, sizeof(int));
		}
			
		EndBlock(file);

		// triangle for visible mesh
		StartBlock(file, 'CMSH');
			WriteData(file, &m_TriangleCount, sizeof(int));
			WriteData(file, m_ClothTriagleList, 3 * sizeof(int) * m_TriangleCount);
		EndBlock(file);
		
		// cloth Stretch Constraints
		StartBlock(file, 'SPRS');
			WriteData (file, &m_StretchPairsCount, sizeof (int));
			WriteData (file, m_softVertexStretchPairs , sizeof (ZeroBendEdge) * m_StretchPairsCount);
		EndBlock(file);

		// cloth Cross Constraints
		StartBlock(file, 'CPRS');
			WriteData (file, &m_CrossPairsCount, sizeof (int));
			WriteData (file, m_softVertexCrossPairs , sizeof (ZeroBendEdge) * m_CrossPairsCount);
		EndBlock(file);
		
		// cloth Bend Constraints
		StartBlock(file, 'BPRS');
			WriteData (file, &m_BendPairsCount, sizeof (int));
			WriteData (file, m_softVertexBendPairs , sizeof (ZeroBendEdge) * m_BendPairsCount);
		EndBlock(file);

		// cloth collision volumes
		StartBlock(file, 'COLL');

			int numVolumes = m_clothCollisionVolumes.GetCount();
			WriteData(file, &numVolumes, sizeof(int));

			ZeroVisList<ClothCollision>::Iterator iter(m_clothCollisionVolumes);
			for (iter.Begin(); iter; iter ++) {
				WriteString(file, InvertCRC((*iter).nameCRC));
				WriteString(file, InvertCRC((*iter).parentCRC));
				WriteData(file, &(*iter).type, sizeof(int));
				WriteData(file, &(*iter).width, sizeof(float));
				WriteData(file, &(*iter).height, sizeof(float));
				WriteData(file, &(*iter).depth, sizeof(float));
			}

		EndBlock(file);

	EndBlock(file);
}


void CMSHBlockModel::WriteClothText (FILE *fp)
{

	int i;

	_ASSERTE (m_ClothData.bClothEnabled);
	int iSegCount;
	CMSHBlockModelSegment* pSegment = (GetSegments(iSegCount))[0];
	_ASSERTE (iSegCount == 1);
	int index =pSegment->GetData()->GetMaterialSet()->GetMaterialIndexByName(pSegment->GetMaterialName() );
	unsigned int numMaterials;
	CMSHBlockMaterial **ppMaterials = pSegment->GetData()->GetMaterialSet()->GetMaterials(numMaterials);
	int numTextures;
	char* textureName = (ppMaterials[index]->GetTextureFilenames(  numTextures ))[0];

	fprintf(fp,"\t\tCloth {\n");
//		fprintf(fp,"\t\t\tMass{%0.3f}\n",m_ClothData.Mass);
//		fprintf(fp,"\t\t\tStretch{%0.3f}\n",m_ClothData.Ks_Stretch);
//		fprintf(fp,"\t\t\tShear{%0.3f}\n",m_ClothData.Ks_Shear);
//		fprintf(fp,"\t\t\tBend{%0.3f}\n",m_ClothData.Ks_Bend);
//		fprintf(fp,"\t\t\tNumCorrections{%d}\n",m_ClothData.nCorrections);
//		fprintf(fp,"\t\t\tAir Resistance\n");
//		fprintf(fp,"\t\t\tCollide\n");
//		fprintf(fp,"\t\t\tSpring\n");
//		fprintf(fp,"\t\t\tWind\n");

		//texture name
		if(textureName)
		{
			fprintf(fp,"\t\t\t textureName{%s}\n", textureName);
		}
		else
			fprintf(fp,"\t\t\t textureName{no_texture}\n");
		// positions
		fprintf(fp,"\t\t\tParticlesPositions{%d}\n", m_ClothParticlesCount);
		fprintf(fp,"\t\t\t{\n");
		for(i = 0 ; i < m_ClothParticlesCount; i++) {
			fprintf(fp,"\t\t\t\t%f %f %f\n", m_ClothParticles[i].fX, m_ClothParticles[i].fY, m_ClothParticles[i].fZ);
		}
		fprintf(fp,"\t\t\t}\n");


		// nailed points
		ZeroStack<float> particlesInvMass(m_ClothParticlesCount);
		fprintf(fp,"\t\t\tClothNailedPoints{%d}\n", m_nClothFixedPoints);
		fprintf(fp,"\t\t\t{\n");
		for (i = 0; i < m_nClothFixedPoints; i ++) {
			fprintf(fp,"\t\t\t\t%d\n", m_pClothFixedIndices[i]);
		}
		fprintf(fp,"\t\t\t}\n");

		if(m_envelopsCount)
		{
			fprintf(fp,"\t\t\tClothNailedPoints Bone Names\n");
			fprintf(fp,"\t\t\t{\n");
			for (i = 0; i < m_nClothFixedPoints; i ++) {
				fprintf(fp,"\t\t\t\t%s\n", m_FixedPointBoneName[i]);
			}
			fprintf(fp,"\t\t\t}\n");
		}
		

		// triangle for visible mesh
		fprintf(fp,"\t\t\tTrianglesCount{%d}\n", m_TriangleCount);
		fprintf(fp,"\t\t\t{\n");
		for(i = 0 ; i < m_TriangleCount; i++) {
			fprintf(fp,"\t\t\t\t%d %d %d\n", m_ClothTriagleList[i * 3 + 0], m_ClothTriagleList[i * 3 + 1], m_ClothTriagleList[i * 3 + 2]);
		}
		fprintf(fp,"\t\t\t}\n");


		// visible mesh uvs
		fprintf(fp,"\t\t\tCeometryUV{%d}\n", m_ClothParticleToVertexIndexMapCount);
		fprintf(fp,"\t\t\t{\n");
		for(i = 0 ; i < m_ClothParticleToVertexIndexMapCount; i++) {
			fprintf(fp,"\t\t\t\t%f %f\n", m_ClothUV[i].fU, m_ClothUV[i].fU);
		}
		fprintf(fp,"\t\t\t}\n");



		// cloth structrural net
		fprintf(fp,"\t\t\tClothStrechPairs {%d}\n", m_StretchPairsCount);
		fprintf(fp,"\t\t\t{\n");
		for (i = 0; i < m_StretchPairsCount; i ++) {
			fprintf(fp,	"\t\t\t\t %d %d\n", m_softVertexStretchPairs[i].v0, m_softVertexStretchPairs[i].v1);
		}
		fprintf(fp,"\t\t\t}\n");
		
		fprintf(fp,"\t\t\tClothCrossPairs {%d}\n", m_CrossPairsCount);
		fprintf(fp,"\t\t\t{\n");
		for (i = 0; i < m_CrossPairsCount; i ++) {
			fprintf(fp,	"\t\t\t\t %d %d\n", m_softVertexCrossPairs[i].v0, m_softVertexCrossPairs[i].v1);
		}
		fprintf(fp,"\t\t\t}\n");
		
		
		fprintf(fp,"\t\t\tClothBendPairs {%d}\n", m_BendPairsCount);
		fprintf(fp,"\t\t\t{\n");
		for (i = 0; i < m_BendPairsCount; i ++) {
			fprintf(fp,	"\t\t\t\t %d %d\n", m_softVertexBendPairs[i].v0, m_softVertexBendPairs[i].v1);
		}
		fprintf(fp,"\t\t\t}\n");


		clothCollisionList.WriteText (fp);


	fprintf(fp,"\t\t}\n");

}


void CMSHBlockModel::CreateClothVertexList(const CMSHBlockModelSegment& segment)
{
	int i;
	int nUV;
	int nPositions;
	uv	*uvPositions;
	vect3	*v3Positions;

	// convert cloth mesh into polyhedra
	ZeroPolyhedra clothPatch;
	ZeroVisTree<MSHBLOCKPolygon, int>& polyList = *segment.GetPolygonList();
	ZeroVisTree<MSHBLOCKPolygon, int>::Iterator polyIter(polyList);

	v3Positions	= segment.GetPositions (nPositions);
	uvPositions = segment.GetUVCoords(nUV, 0);

	m_ClothParticleToVertexIndexMapCount = nPositions;
	m_ClothParticleToVertexIndexMap = ZeroNew (int[nPositions]);

	m_ClothUV = ZeroNew (uv[nPositions]);
	for (i = 0; i < nPositions; i ++) {
		m_ClothUV[i].fU = 0.0f;
		m_ClothUV[i].fV = 0.0f;
		if (uvPositions) {
		  m_ClothUV[i].fU = uvPositions[i].fU;
		  m_ClothUV[i].fV = uvPositions[i].fV;
		}
	}


	ZeroStack<vect3> vertexPool (nPositions);
	for (i = 0; i < nPositions; i ++) {
		vertexPool[i].fX = v3Positions[i].fX;
		vertexPool[i].fY = v3Positions[i].fY;
		vertexPool[i].fZ = v3Positions[i].fZ;
	}

	m_ClothParticlesCount = nPositions;
	//VertexListToIndexList(&vertexPool[0].fX, sizeof (ZeroVector3), nPositions, (unsigned*)&m_ClothParticleToVertexIndexMap[0], sizeof (ZeroVector3), 2.5e-3f);
	for(i = 0; i < m_ClothParticlesCount; i++)
		m_ClothParticleToVertexIndexMap[i] = i;

	m_ClothParticles = ZeroNew (vect3[m_ClothParticlesCount]);
	memcpy (m_ClothParticles, &vertexPool[0].fX, sizeof (vect3) * m_ClothParticlesCount);


	clothPatch.BeginFace();
	for (polyIter.Begin(); polyIter; polyIter ++) {
		int indexCount;
		int index[256];

		MSHBLOCKPolygon& polygon = (*polyIter)->info;

		indexCount = 0;
		MSHBLOCKPolygon::Iterator polyIter (polygon);
		for (polyIter.Begin(); polyIter; polyIter ++) {
			int i;
			i = *polyIter;
			index[indexCount] = m_ClothParticleToVertexIndexMap[i];
			indexCount ++;
		}
		clothPatch.AddFace (indexCount, &index[0]);
	}
	clothPatch.EndFace();

	ZeroPolyhedra::Iterator iter1 (clothPatch);
	for (iter1.Begin(); iter1; iter1 ++) {
		ZeroEdge *edge;
		edge = &(*iter1)->info;
		if(!edge->next)
			edge->next = NULL;
		int count = 0;
		ZeroEdge *ptr;
		ptr = edge;
		do {
			count++;
			ptr = ptr->next;
		} while (ptr != edge);
		if(count !=3 || count != 4)
			count = 0;

	}
	clothPatch.Quadrangulate (&m_ClothParticles[0].fX, sizeof (vect3));



	// get unique fix points
	ZeroVisTree<int, int> fixPointsTree;
	for (i = 0;	i < m_nClothFixedPoints; i ++) {
		int index;

		index = m_pClothFixedIndices[i];
		index = m_ClothParticleToVertexIndexMap[index];
		fixPointsTree.Insert (index, index);
	}

	// remap fixpoints
	ZeroVisTree<int, int>::Iterator fixIter(fixPointsTree);
	m_nClothFixedPoints = fixPointsTree.GetCount();

	i = 0; 
	for (fixIter.Begin(); fixIter; fixIter++) {
		int index;
		index = (*fixIter)->info;
		m_pClothFixedIndices[i]	= index;
		i ++;
	}

	// remap user cloth points - Mike Z
	for(int i = 0; i < m_nUserCloth; i++)
	{
		int index;

		index = m_pUserClothFirst[i];
		index = m_ClothParticleToVertexIndexMap[index];
		m_pUserClothFirst[i] = index; // remap

		index = m_pUserClothSecond[i];
		index = m_ClothParticleToVertexIndexMap[index];
		m_pUserClothSecond[i] = index; // remap
	}

	// Create the vertex stretch net;
	int mark;
	//int pairsCount;

	ZeroVisTree<int, unsigned> StretchPairTree;
	mark = clothPatch.IncLRU();
	ZeroPolyhedra::Iterator iter (clothPatch);

	int uniqueUserCloth = 0;

	for(int i = 0; i < m_nUserCloth; i++)
		if(m_pUserClothFirst[i] != m_pUserClothSecond[i])
		{
			bool notDupe = true;
			for(int j = 0; j < i && notDupe; j++)
				// remove duplicates
				if((m_pUserClothFirst[i] == m_pUserClothFirst[j] && m_pUserClothSecond[i] == m_pUserClothSecond[j])
					||(m_pUserClothFirst[i] == m_pUserClothSecond[j] && m_pUserClothSecond[i] == m_pUserClothFirst[j]))
				{
					notDupe = false;
				}
				if(!notDupe)
					continue;

				unsigned key;
				key = ((m_pUserClothFirst[i] << 16) | m_pUserClothSecond[i]);
				StretchPairTree.Insert(key);

				uniqueUserCloth++;
		}

	// debug output
	//char msg[50];
	//sprintf(msg, "Unique: %d  Total: %d", uniqueUserCloth, m_nUserCloth);
	//::MessageBox(NULL, msg, "Un", 0);

	for (iter.Begin(); iter; iter ++) {
		ZeroEdge *edge;

		edge = &(*iter)->info;
		if (edge->mark != mark ) {
			ZeroEdge *ptr;
			ptr = edge;
			do {
				if (ptr->twin->mark != mark) {
					if (!(fixPointsTree.Find(ptr->incidentVertex) && fixPointsTree.Find(ptr->twin->incidentVertex))) {
						unsigned key;
						key = (ptr->incidentVertex << 16) | ptr->twin->incidentVertex ;
						StretchPairTree.Insert(key);
					} 
				}
				ptr->mark = mark;
				ptr->twin->mark = mark;
				ptr = ptr->twin->next;
			} while (ptr != edge);
		}
	}

	ZeroVisTree<int, unsigned>::Iterator StretchIter(StretchPairTree);
	m_StretchPairsCount = StretchPairTree.GetCount();
	m_softVertexStretchPairs = new ZeroBendEdge[m_StretchPairsCount];
	memset (m_softVertexStretchPairs, 0xff, sizeof (ZeroBendEdge) * m_StretchPairsCount);

	i = 0;
	for (StretchIter.Begin(); StretchIter; StretchIter++) {
		int index;
		index = (*StretchIter)->key ;
		m_softVertexStretchPairs[i].v0	= (short) (index >> 16);
		m_softVertexStretchPairs[i].v1	=  (short) (index & 0x00FF);
		i ++;
	}


	//cross edge constraints

	ZeroVisTree<int, unsigned> CrossPairTree;
	mark = clothPatch.IncLRU();
		
	for (iter.Begin(); iter; iter ++) {
		ZeroEdge *edge;
		edge = &(*iter)->info;
		if ( (edge->mark != mark) && (edge->incidentFace !=-1) ) {
			ZeroEdge *ptr;
			ptr = edge;
			int count = 0;
			do {
				count++;
				ZeroEdge* ptrNext = ptr->next;

				while (ptrNext->next != ptr->prev )
				{
					if (!(fixPointsTree.Find(ptr->incidentVertex) && fixPointsTree.Find(ptrNext->twin->incidentVertex))) {
						unsigned key0, key1;
						key0 = (ptr->incidentVertex << 16) | ptrNext->twin->incidentVertex ;
						key1 = ( ptrNext->twin->incidentVertex << 16) |  ptr->incidentVertex;
						if(!(CrossPairTree.Find(key0) || CrossPairTree.Find(key1)))
						{
							CrossPairTree.Insert(key0);
						}
					}
					ptrNext = ptrNext->next;
				}

				ptr->mark = mark;
				ptr = ptr->next;
			} while (ptr != edge);
			
		}
	}

	ZeroVisTree<int, unsigned>::Iterator CrossIter(CrossPairTree);
	m_CrossPairsCount = CrossPairTree.GetCount();
	m_softVertexCrossPairs = new ZeroBendEdge[m_CrossPairsCount];
	memset (m_softVertexCrossPairs, 0xff, sizeof (ZeroBendEdge) * m_CrossPairsCount);

	i = 0;
	for (CrossIter.Begin(); CrossIter; CrossIter++) {
		int index;
		index = (*CrossIter)->key ;
		m_softVertexCrossPairs[i].v0	= (short) (index >> 16);
		m_softVertexCrossPairs[i].v1	=  (short) (index & 0x00FF);
		i ++;
	}

	

	/*for (iter.Begin(); iter; iter ++) {
		ZeroEdge *edge;

		edge = &(*iter)->info;
		if (edge->mark != mark) {
			ZeroEdge *ptr;
			ptr = edge;
			do {
				if (ptr->twin->mark != mark) {
					if (!(fixPointsTree.Find(ptr->incidentVertex) && fixPointsTree.Find(ptr->twin->incidentVertex))) {
						ptr->userData = pairsCount;
						ptr->twin->userData = pairsCount	+ 1;
						pairsCount += 2;
					} else {
						ptr->userData = unsigned (-1);
						ptr->twin->userData = unsigned (-1);
					}
				}
				ptr->mark = mark;
				ptr = ptr->twin->next;
			} while (ptr != edge);
		}
	}


	m_StretchPairsCount = pairsCount;
	m_softVertexStretchPairs = new ZeroClothVertexEdge[pairsCount];
	memset (m_softVertexStretchPairs, 0xff, sizeof (ZeroClothVertexEdge) * m_StretchPairsCount);

	for (iter.Begin(); iter; iter ++) {
		int index ;
		ZeroEdge *ptr;
		ZeroEdge *edge;

		edge = &(*iter)->info;
		if (unsigned (edge->userData) != 0xffffffff) {
			index = edge->userData;
			m_softVertexStretchPairs[index].particleIndex = (unsigned short) edge->incidentVertex;
			m_softVertexStretchPairs[index].twin = unsigned short (edge->twin->userData);

			for (ptr = edge->next; unsigned (ptr->userData) == unsigned (-1); ptr = ptr->twin->next);
			m_softVertexStretchPairs[index].next = unsigned short (ptr->userData);
		}
	}*/



	// calculate bend pairs
	int newBendsCount = 0;

	ZeroStack<ZeroBendEdge> bendEdges(65536);

	mark = clothPatch.IncLRU();
	ZeroVisTree<int, unsigned> bendFilter;
	for (iter.Begin(); iter; iter ++) {
		ZeroEdge *edge;
		edge = &(*iter)->info;

		if (edge->mark != mark) {
			int index0;
			ZeroEdge *ptr;

			index0 = edge->incidentVertex;
			ptr = edge;
			do {
				ZeroEdge *bendEdgeCentre;
				ptr->mark = mark;


				bendEdgeCentre	= ptr->twin;
				do {
					int index1;
					index1 = bendEdgeCentre->twin->incidentVertex;
					if (index1 != index0) {
						unsigned key;
						ZeroBendEdge bendEdge;

						bendEdge.v0 = short (index0);
						bendEdge.v1 = short (index1);
						key = (bendEdge.v1 << 16) + bendEdge.v0;
						if (!bendFilter.Find (key)) {

							bendFilter.Insert (index0, key);

							key = (bendEdge.v0 << 16) + bendEdge.v1;
							bendFilter.Insert (index0, key);
							if (!(fixPointsTree.Find(index0) && fixPointsTree.Find(index1))) {

								int key0 = (bendEdge.v0 << 16) + bendEdge.v1;
								int key1 = (bendEdge.v1 << 16) + bendEdge.v0;
								if(!(CrossPairTree.Find(key0) || CrossPairTree.Find(key1)))
								{
									bendEdges[newBendsCount].v0 = short (index0);
									bendEdges[newBendsCount].v1 = short (index1);
									newBendsCount ++;
								}
							}
						}
					}
					bendEdgeCentre = bendEdgeCentre->twin->next;
				} while (bendEdgeCentre != ptr->twin);


				ptr = ptr->twin->next;
			} while (ptr != edge);
		}
	}

	m_BendPairsCount = newBendsCount;
	m_softVertexBendPairs = new ZeroBendEdge[m_BendPairsCount];
	memcpy (m_softVertexBendPairs, &bendEdges[0], m_BendPairsCount * sizeof (ZeroBendEdge));


	ZeroStack<ZeroFace> triangleList(clothPatch.GetCount() + 100);
	m_TriangleCount = clothPatch.WireMeshing (&triangleList[0], &m_ClothParticles[0].fX, sizeof (vect3));

	m_ClothTriagleList = ZeroNew (int[m_TriangleCount * 3]);
	memcpy (m_ClothTriagleList, &triangleList[0], sizeof (ZeroFace) * m_TriangleCount);

}

//void CMSHBlockModel::CreateClothVertexList(const CMSHBlockModelSegment& segment)
//{
//	int i;
//	int nUV;
//	int nPositions;
//	uv	*uvPositions;
//	vect3	*v3Positions;
//
//	// convert cloth mesh into polyhedra
//	ZeroPolyhedra clothPatch;
//	ZeroVisTree<MSHBLOCKPolygon, int>& polyList = *segment.GetPolygonList();
//	ZeroVisTree<MSHBLOCKPolygon, int>::Iterator polyIter(polyList);
//
//	v3Positions	= segment.GetPositions (nPositions);
//	uvPositions = segment.GetUVCoords(nUV, 0);
//
//	m_ClothParticleToVertexIndexMapCount = nPositions;
//	m_ClothParticleToVertexIndexMap = ZeroNew (int[nPositions]);
//
//	m_ClothUV = ZeroNew (uv[nPositions]);
//	for (i = 0; i < nPositions; i ++) {
//		m_ClothUV[i].fU = 0.0f;
//		m_ClothUV[i].fV = 0.0f;
//		if (uvPositions) {
//		  m_ClothUV[i].fU = uvPositions[i].fU;
//		  m_ClothUV[i].fV = uvPositions[i].fV;
//		}
//	}
//
//
//	ZeroStack<vect3> vertexPool (nPositions);
//	for (i = 0; i < nPositions; i ++) {
//		vertexPool[i].fX = v3Positions[i].fX;
//		vertexPool[i].fY = v3Positions[i].fY;
//		vertexPool[i].fZ = v3Positions[i].fZ;
//	}
//
//	m_ClothParticlesCount = nPositions;
//	//VertexListToIndexList(&vertexPool[0].fX, sizeof (ZeroVector3), nPositions, (unsigned*)&m_ClothParticleToVertexIndexMap[0], sizeof (ZeroVector3), 2.5e-3f);
//	for(i = 0; i < m_ClothParticlesCount; i++)
//		m_ClothParticleToVertexIndexMap[i] = i;
//
//	m_ClothParticles = ZeroNew (vect3[m_ClothParticlesCount]);
//	memcpy (m_ClothParticles, &vertexPool[0].fX, sizeof (vect3) * m_ClothParticlesCount);
//
//
//	clothPatch.BeginFace();
//	for (polyIter.Begin(); polyIter; polyIter ++) {
//		int indexCount;
//		int index[256];
//
//		MSHBLOCKPolygon& polygon = (*polyIter)->info;
//
//		indexCount = 0;
//		MSHBLOCKPolygon::Iterator polyIter (polygon);
//		for (polyIter.Begin(); polyIter; polyIter ++) {
//			int i;
//			i = *polyIter;
//			index[indexCount] = m_ClothParticleToVertexIndexMap[i];
//			indexCount ++;
//		}
//		clothPatch.AddFace (indexCount, &index[0]);
//	}
//	clothPatch.EndFace();
//
//	ZeroPolyhedra::Iterator iter1 (clothPatch);
//	for (iter1.Begin(); iter1; iter1 ++) {
//		ZeroEdge *edge;
//		edge = &(*iter1)->info;
//		if(!edge->next)
//			edge->next = NULL;
//		int count = 0;
//		ZeroEdge *ptr;
//		ptr = edge;
//		do {
//			count++;
//			ptr = ptr->next;
//		} while (ptr != edge);
//		if(count !=3 || count != 4)
//			count = 0;
//
//	}
//	clothPatch.Quadrangulate (&m_ClothParticles[0].fX, sizeof (vect3));
//
//
//
//	// get unique fix points
//	ZeroVisTree<int, int> fixPointsTree;
//	for (i = 0;	i < m_nClothFixedPoints; i ++) {
//		int index;
//
//		index = m_pClothFixedIndices[i];
//		index = m_ClothParticleToVertexIndexMap[index];
//		fixPointsTree.Insert (index, index);
//	}
//
//	// remap fixpoints
//	ZeroVisTree<int, int>::Iterator fixIter(fixPointsTree);
//	m_nClothFixedPoints = fixPointsTree.GetCount();
//
//	i = 0; 
//	for (fixIter.Begin(); fixIter; fixIter++) {
//		int index;
//		index = (*fixIter)->info;
//		m_pClothFixedIndices[i]	= index;
//		i ++;
//	}
//
//	// remap user cloth points - Mike Z
//	for(int i = 0; i < m_nUserCloth; i++)
//	{
//		int index;
//
//		index = m_pUserClothFirst[i];
//		index = m_ClothParticleToVertexIndexMap[index];
//		m_pUserClothFirst[i] = index; // remap
//
//		index = m_pUserClothSecond[i];
//		index = m_ClothParticleToVertexIndexMap[index];
//		m_pUserClothSecond[i] = index; // remap
//	}
//
//	// Create the vertex stretch net;
//	int mark;
//	//int pairsCount;
//
//	ZeroVisTree<int, unsigned> StretchPairTree;
//	mark = clothPatch.IncLRU();
//	ZeroPolyhedra::Iterator iter (clothPatch);
//	
//	::MessageBox(NULL, "This is the exporter you think it is, again", "Yay!", 0);
//
//	for (iter.Begin(); iter; iter ++) {
//		ZeroEdge *edge;
//
//		edge = &(*iter)->info;
//		if (edge->mark != mark ) {
//			ZeroEdge *ptr;
//			ptr = edge;
//			do {
//				if (ptr->twin->mark != mark) {
//					if (!(fixPointsTree.Find(ptr->incidentVertex) && fixPointsTree.Find(ptr->twin->incidentVertex))) {
//						unsigned key;
//						key = (ptr->incidentVertex << 16) | ptr->twin->incidentVertex ;
//						StretchPairTree.Insert(key);
//					} 
//				}
//				ptr->mark = mark;
//				ptr->twin->mark = mark;
//				ptr = ptr->twin->next;
//			} while (ptr != edge);
//		}
//	}
//
//	// moved stretch extra bits to after Bend, for user cloth
//
//
//	//cross edge constraints
//
//	ZeroVisTree<int, unsigned> CrossPairTree;
//	mark = clothPatch.IncLRU();
//		
//	for (iter.Begin(); iter; iter ++) {
//		ZeroEdge *edge;
//		edge = &(*iter)->info;
//		if ( (edge->mark != mark) && (edge->incidentFace !=-1) ) {
//			ZeroEdge *ptr;
//			ptr = edge;
//			int count = 0;
//			do {
//				count++;
//				ZeroEdge* ptrNext = ptr->next;
//
//				while (ptrNext->next != ptr->prev )
//				{
//					if (!(fixPointsTree.Find(ptr->incidentVertex) && fixPointsTree.Find(ptrNext->twin->incidentVertex))) {
//						unsigned key0, key1;
//						key0 = (ptr->incidentVertex << 16) | ptrNext->twin->incidentVertex ;
//						key1 = ( ptrNext->twin->incidentVertex << 16) |  ptr->incidentVertex;
//						if(!(CrossPairTree.Find(key0) || CrossPairTree.Find(key1)))
//						{
//							CrossPairTree.Insert(key0);
//						}
//					}
//					ptrNext = ptrNext->next;
//				}
//
//				ptr->mark = mark;
//				ptr = ptr->next;
//			} while (ptr != edge);
//			
//		}
//	}
//
//
//
//
//	ZeroVisTree<int, unsigned>::Iterator CrossIter(CrossPairTree);
//	m_CrossPairsCount = CrossPairTree.GetCount();
//	m_softVertexCrossPairs = new ZeroBendEdge[m_CrossPairsCount];
//	memset (m_softVertexCrossPairs, 0xff, sizeof (ZeroBendEdge) * m_CrossPairsCount);
//
//	i = 0;
//	for (CrossIter.Begin(); CrossIter; CrossIter++) {
//		int index;
//		index = (*CrossIter)->key ;
//		m_softVertexCrossPairs[i].v0	= (short) (index >> 16);
//		m_softVertexCrossPairs[i].v1	=  (short) (index & 0x00FF);
//		i ++;
//	}
//
//	
//
//	/*for (iter.Begin(); iter; iter ++) {
//		ZeroEdge *edge;
//
//		edge = &(*iter)->info;
//		if (edge->mark != mark) {
//			ZeroEdge *ptr;
//			ptr = edge;
//			do {
//				if (ptr->twin->mark != mark) {
//					if (!(fixPointsTree.Find(ptr->incidentVertex) && fixPointsTree.Find(ptr->twin->incidentVertex))) {
//						ptr->userData = pairsCount;
//						ptr->twin->userData = pairsCount	+ 1;
//						pairsCount += 2;
//					} else {
//						ptr->userData = unsigned (-1);
//						ptr->twin->userData = unsigned (-1);
//					}
//				}
//				ptr->mark = mark;
//				ptr = ptr->twin->next;
//			} while (ptr != edge);
//		}
//	}
//
//
//	m_StretchPairsCount = pairsCount;
//	m_softVertexStretchPairs = new ZeroClothVertexEdge[pairsCount];
//	memset (m_softVertexStretchPairs, 0xff, sizeof (ZeroClothVertexEdge) * m_StretchPairsCount);
//
//	for (iter.Begin(); iter; iter ++) {
//		int index ;
//		ZeroEdge *ptr;
//		ZeroEdge *edge;
//
//		edge = &(*iter)->info;
//		if (unsigned (edge->userData) != 0xffffffff) {
//			index = edge->userData;
//			m_softVertexStretchPairs[index].particleIndex = (unsigned short) edge->incidentVertex;
//			m_softVertexStretchPairs[index].twin = unsigned short (edge->twin->userData);
//
//			for (ptr = edge->next; unsigned (ptr->userData) == unsigned (-1); ptr = ptr->twin->next);
//			m_softVertexStretchPairs[index].next = unsigned short (ptr->userData);
//		}
//	}*/
//
//
//
//	// calculate bend pairs
//	int newBendsCount = 0;
//
//	ZeroStack<ZeroBendEdge> bendEdges(65536);
//
//	mark = clothPatch.IncLRU();
//	ZeroVisTree<int, unsigned> bendFilter;
//	for (iter.Begin(); iter; iter ++) {
//		ZeroEdge *edge;
//		edge = &(*iter)->info;
//
//		if (edge->mark != mark) {
//			int index0;
//			ZeroEdge *ptr;
//
//			index0 = edge->incidentVertex;
//			ptr = edge;
//			do {
//				ZeroEdge *bendEdgeCentre;
//				ptr->mark = mark;
//
//
//				bendEdgeCentre	= ptr->twin;
//				do {
//					int index1;
//					index1 = bendEdgeCentre->twin->incidentVertex;
//					if (index1 != index0) {
//						unsigned key;
//						ZeroBendEdge bendEdge;
//
//						bendEdge.v0 = short (index0);
//						bendEdge.v1 = short (index1);
//						key = (bendEdge.v1 << 16) + bendEdge.v0;
//						if (!bendFilter.Find (key)) {
//
//							bendFilter.Insert (index0, key);
//
//							key = (bendEdge.v0 << 16) + bendEdge.v1;
//							bendFilter.Insert (index0, key);
//							if (!(fixPointsTree.Find(index0) && fixPointsTree.Find(index1))) {
//
//								int key0 = (bendEdge.v0 << 16) + bendEdge.v1;
//								int key1 = (bendEdge.v1 << 16) + bendEdge.v0;
//								if(!(CrossPairTree.Find(key0) || CrossPairTree.Find(key1)))
//								{
//									bendEdges[newBendsCount].v0 = short (index0);
//									bendEdges[newBendsCount].v1 = short (index1);
//									newBendsCount ++;
//								}
//							}
//						}
//					}
//					bendEdgeCentre = bendEdgeCentre->twin->next;
//				} while (bendEdgeCentre != ptr->twin);
//
//
//				ptr = ptr->twin->next;
//			} while (ptr != edge);
//		}
//	}
//
//	m_BendPairsCount = newBendsCount;
//	m_softVertexBendPairs = new ZeroBendEdge[m_BendPairsCount];
//	memcpy (m_softVertexBendPairs, &bendEdges[0], m_BendPairsCount * sizeof (ZeroBendEdge));
//
//	for(int i = 0; i < m_nUserCloth; i++)
//		if(m_pUserClothFirst[i] != m_pUserClothSecond[i])
//		{
//			bool notDupe = true;
//			for(int j = 0; j < i && notDupe; j++)
//				// remove duplicates
//				if((m_pUserClothFirst[i] == m_pUserClothFirst[j] && m_pUserClothSecond[i] == m_pUserClothSecond[i])
//					||(m_pUserClothFirst[i] == m_pUserClothSecond[j] && m_pUserClothSecond[i] == m_pUserClothFirst[i]))
//				{
//					notDupe = false;
//				}
//			if(!notDupe)
//				continue;
//			
//			unsigned key;
//			key = ((m_pUserClothFirst[i] << 16) | m_pUserClothSecond[i]);
//			StretchPairTree.Insert(key);
//		}
//
//	ZeroVisTree<int, unsigned>::Iterator StretchIter(StretchPairTree);
//	m_StretchPairsCount = StretchPairTree.GetCount();
//	m_softVertexStretchPairs = new ZeroBendEdge[m_StretchPairsCount];
//	memset (m_softVertexStretchPairs, 0xff, sizeof (ZeroBendEdge) * m_StretchPairsCount);
//
//	i = 0;
//	for (StretchIter.Begin(); StretchIter; StretchIter++) {
//		int index;
//		index = (*StretchIter)->key ;
//		m_softVertexStretchPairs[i].v0	= (short) (index >> 16);
//		m_softVertexStretchPairs[i].v1	=  (short) (index & 0x00FF);
//		i ++;
//	}
//
//	ZeroStack<ZeroFace> triangleList(clothPatch.GetCount() + 100);
//	m_TriangleCount = clothPatch.WireMeshing (&triangleList[0], &m_ClothParticles[0].fX, sizeof (vect3));
//
//	m_ClothTriagleList = ZeroNew (int[m_TriangleCount * 3]);
//	memcpy (m_ClothTriagleList, &triangleList[0], sizeof (ZeroFace) * m_TriangleCount);
//
//}

int CMSHBlockModel::IsFixed(int index)
{
	for (int i = 0; i < m_nClothFixedPoints; i++)
	{
		if(m_pClothFixedIndices[i] == index)
			return i;
	}
	return -1;
}

char* CMSHBlockModel::GetBoneNameFromPaletteIndex(int index)
{
	CMSHBlockModel **ppModels;
	int nModels;
	ppModels = gZeroData ->GetModels(nModels);
	for(int i = 0; i < nModels; i++)
	{
		if( ppModels[i]->GetMatrixPaletteIndex() == index)
		{
			return ppModels[i]->GetName();
		}
	}
	return NULL;
}


void CMSHBlockModel::SetFixedPointsWeights()
{
	int vertex;
	int rank;
	int iSegCount;
	CMSHBlockModelSegment* pSegment = (GetSegments(iSegCount))[0];
	
	CMSHBlockSegmentEnvelop::Iterator iter(pSegment->GetVertexWeightsEnvelop());
	char buffer[2048];
	for (iter.Begin(); iter; iter ++) {

		vertex = (*iter)->key;
		//if this vertex is a fixed position
		if( (rank = IsFixed(vertex)) >= 0 )
		{
			ZeroHeap<int, float> boneHeap (6, 500.0f, buffer);
	
			CMSHBlockVertexWeightsList::Iterator weightIter ((*iter)->info);
			
			for (weightIter.Begin(); weightIter; weightIter ++) {
				ZeroMatrixWeightPair& boneWeightPair = *weightIter;
				boneHeap.Push (boneWeightPair.boneID, boneWeightPair.weight);
            }
			while (boneHeap.GetCount() < 4) {
				int bone = 0;
				boneHeap.Push (bone, 0.0);
			}
			int id = boneHeap[0];
			
			while (boneHeap.GetCount() ) {
				boneHeap.Pop();
			}


			m_FixedPointBoneName[rank] = GetBoneNameFromPaletteIndex(m_envelopArray[id]);
		}
	}

}

