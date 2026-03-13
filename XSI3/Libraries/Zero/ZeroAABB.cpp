/****************************************************************************
*
*  File Name  : Bitmap.C
*  Visual C++ 4.0 base by Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"
#include "ZeroAABB.h"

#include "ZeroStack.h"
#include "ZeroAABB.h"
#include "ZeroMatrix.h"
#include "ZeroCamera.h"
#include "ZeroGeoUtil.h"
#include "ZeroRenderState.h"
#include "ZeroRenderDescriptor.h" 

// allocator for AABB data
ZeroAllocator *clsnAlloc;

ZERO_RTTI_CONTRUCTOR(ZeroAABB);

struct ByteVector
{
	char x;
	char y;
	char z;

	operator unsigned int (void) const
	{
		return *(unsigned int *)this;
	}
};

class ZeroAABBBuilderBox
{
public:
	ZeroVector3 m_min;
	ZeroVector3 m_max;
	int m_poygonIndex;
	int m_indexCount;

	void CalcExtends (int indexCount, const int indexArray[], const ZeroVector3 points[])
	{
		int i;

		m_indexCount = indexCount;

		m_min.x = FLT_MAX;
		m_min.y = FLT_MAX;
		m_min.z = FLT_MAX;
		m_max.x = -FLT_MAX;
		m_max.y = -FLT_MAX;
		m_max.z = -FLT_MAX;

		for (i = 0; i < indexCount; i ++) {
			const ZeroVector3 &p = points[indexArray[i]];

			m_min.x = GetMin (p.x, m_min.x); 
			m_min.y = GetMin (p.y, m_min.y); 
			m_min.z = GetMin (p.z, m_min.z); 

			m_max.x = GetMax (p.x, m_max.x); 
			m_max.y = GetMax (p.y, m_max.y); 
			m_max.z = GetMax (p.z, m_max.z); 
		}

		m_min -= ZeroVector3 (0.25f, 0.25f, 0.25f); 
		m_max += ZeroVector3 (0.25f, 0.25f, 0.25f); 
	}
};

class ZeroAABBTree
{
public:
	class TreeNode
	{
		protected:
		unsigned int index;

		public:
		void SetLeafNode(unsigned int faceIndices, unsigned int indexCount)
		{
			index = (faceIndices << 8) | (indexCount <<1) | 1;
		}

		inline bool IsLeafNode() const
		{
			return index & 1;
		}

		inline unsigned int GetIndexCount() const
		{
			_ASSERTE(IsLeafNode());
			return (index & 0xFE) >> 1;
		}
		inline unsigned int GetFaceIndices() const
		{
			_ASSERTE(IsLeafNode());
			return index >> 8;
		}

		inline void SetNodePointer(ZeroAABBTree* node)
		{
			index = (unsigned int)node;
			_ASSERTE ((index & 1) == 0);
		}
		inline ZeroAABBTree* GetNodePointer() const
		{
			_ASSERTE(!IsLeafNode());
			return (ZeroAABBTree*)index;
		}
	};

	ByteVector m_min;
	ByteVector m_max;
	TreeNode m_back;
	TreeNode m_front;

	static int *m_indexArray;
	static ZeroVector3 *m_vertexArray;
	static ZeroVector3 m_scale;

public:
	inline void GetExtents (ZeroVector3 &min, ZeroVector3 &max) const
	{
		min.x = m_min.x * m_scale.x;
		min.y = m_min.y * m_scale.y;
		min.z = m_min.z * m_scale.z;

		max.x = m_max.x * m_scale.x + m_scale.x;
		max.y = m_max.y * m_scale.y + m_scale.y;
		max.z = m_max.z * m_scale.z + m_scale.z;
	}

	inline void SetExtents (const ZeroVector3 &min, const ZeroVector3 &max)
	{
		m_min.x = (char)clamp(FloatToInt(floorf(min.x / m_scale.x)), -128, 127);
		m_min.y = (char)clamp(FloatToInt(floorf(min.y / m_scale.y)), -128, 127);
		m_min.z = (char)clamp(FloatToInt(floorf(min.z / m_scale.z)), -128, 127);

		m_max.x = (char)clamp(FloatToInt(ceilf(max.x / m_scale.x)) - 1, -128, 127);
		m_max.y = (char)clamp(FloatToInt(ceilf(max.y / m_scale.y)) - 1, -128, 127);
		m_max.z = (char)clamp(FloatToInt(ceilf(max.z / m_scale.z)) - 1, -128, 127);
	}

	inline bool BoxIntersect (const ZeroVector3& min, const ZeroVector3& max) const
	{
		return
			(min.x <= m_max.x * m_scale.x + m_scale.x) &&
			(max.x >= m_min.x * m_scale.x) &&
			(min.z <= m_max.z * m_scale.z + m_scale.z) &&
			(max.z >= m_min.z * m_scale.z) &&
			(min.y <= m_max.y * m_scale.y + m_scale.y) &&
			(max.y >= m_min.y * m_scale.y);
	}

	inline bool RayIntersect (const ZeroVector3& q0, const ZeroVector3& q1) const
	{
		float t;
		FLOATSIGN tmp0;
		FLOATSIGN tmp1;

		ZeroVector4 p0 (q0);
		ZeroVector4 p1 (q1);

		// x axis
		float max_x (m_max.x * m_scale.x + m_scale.x);
		tmp0.f = max_x - p0.x;
		if (tmp0.i > 0) {
			tmp1.f = max_x - p1.x;
			if (tmp1.i < 0) {
				t = tmp0.f / (p1.x - p0.x);
				p1.x = max_x;
				p1.y = p0.y + (p1.y - p0.y) * t;
				p1.z = p0.z + (p1.z - p0.z) * t;
			}
		} else {
			tmp1.f = max_x - p1.x;
			if (tmp1.i > 0) {
				t = tmp0.f / (p1.x - p0.x);
				p0.x = max_x;
				p0.y = p0.y + (p1.y - p0.y) * t;
				p0.z = p0.z + (p1.z - p0.z) * t;
			} else {
				return false;
			}
		}

		float min_x (m_min.x * m_scale.x);
		tmp0.f = min_x - p0.x;
		if (tmp0.i < 0) {
			tmp1.f = min_x - p1.x;
			if (tmp1.i > 0) {
				t = tmp0.f / (p1.x - p0.x);
				p1.x = min_x;
				p1.y = p0.y + (p1.y - p0.y) * t;
				p1.z = p0.z + (p1.z - p0.z) * t;
			}
		} else {
			tmp1.f = min_x - p1.x;
			if (tmp1.i < 0) {
				t = tmp0.f / (p1.x - p0.x);
				p0.x = min_x;
				p0.y = p0.y + (p1.y - p0.y) * t;
				p0.z = p0.z + (p1.z - p0.z) * t;
			} else {
				return false;
			}
		}

		// z axis
		float max_z (m_max.z * m_scale.z + m_scale.z);
		tmp0.f = max_z - p0.z;
		if (tmp0.i > 0) {
			tmp1.f = max_z - p1.z;
			if (tmp1.i < 0) {
				t = tmp0.f / (p1.z - p0.z);
				p1.z = max_z;
				p1.x = p0.x + (p1.x - p0.x) * t;
				p1.y = p0.y + (p1.y - p0.y) * t;
			}
		} else {
			tmp1.f = max_z - p1.z;
			if (tmp1.i > 0) {
				t = tmp0.f / (p1.z - p0.z);
				p0.z = max_z;
				p0.x = p0.x + (p1.x - p0.x) * t;
				p0.y = p0.y + (p1.y - p0.y) * t;
			} else {
				return false;
			}
		}

		float min_z (m_min.z * m_scale.z);
		tmp0.f = min_z - p0.z;
		if (tmp0.i < 0) {
			tmp1.f = min_z - p1.z;
			if (tmp1.i > 0) {
				t = tmp0.f / (p1.z - p0.z);
				p1.z = min_z;
				p1.x = p0.x + (p1.x - p0.x) * t;
				p1.y = p0.y + (p1.y - p0.y) * t;
			}
		} else {
			tmp1.f = min_z - p1.z;
			if (tmp1.i < 0) {
				t = tmp0.f / (p1.z - p0.z);
				p0.z = min_z;
				p0.x = p0.x + (p1.x - p0.x) * t;
				p0.y = p0.y + (p1.y - p0.y) * t;
			} else {
				return false;
			}
		}

		// Y axis
		float max_y (m_max.y * m_scale.y + m_scale.y);
		tmp0.f = max_y - p0.y;
		tmp1.f = max_y - p1.y;
		if ((tmp0.i & tmp1.i) < 0) {
			return false;
		}

		float min_y (m_min.y * m_scale.y);
		tmp0.f = min_y - p0.y;
		tmp1.f = min_y - p1.y;
		if ((tmp0.i | tmp1.i) > 0) {
			return false;
		}

		return true;
	}

	void CalcNodeCount (int &count) const
	{
		count ++;
		if (!m_back.IsLeafNode()) {
			m_back.GetNodePointer()->CalcNodeCount (count);
		}

		if (!m_front.IsLeafNode()) {
			m_front.GetNodePointer()->CalcNodeCount (count);
		}
	}

	void CalcBoxFromBoxArray (ZeroAABBBuilderBox boxArray[], int boxCount)
	{
		int i;
		ZeroVector3 min;
		ZeroVector3 max;

		min.x = FLT_MAX;
		min.y = FLT_MAX;
		min.z = FLT_MAX;
		max.x = -FLT_MAX;
		max.y = -FLT_MAX;
		max.z = -FLT_MAX;

		for (i = 0; i < boxCount; i ++) {
			const ZeroVector3 &p0 = boxArray[i].m_min;
			min.x = GetMin (p0.x, min.x); 
			min.y = GetMin (p0.y, min.y); 
			min.z = GetMin (p0.z, min.z); 

			const ZeroVector3 &p1 = boxArray[i].m_max;
			max.x = GetMax (p1.x, max.x); 
			max.y = GetMax (p1.y, max.y); 
			max.z = GetMax (p1.z, max.z); 
		}

		SetExtents(min, max);
	}

	int CalcCost (const ZeroAABBBuilderBox boxArray[], int boxCount, int axis, float value) const
	{
		int i;
		int leftCost;
		int rightCost;
		int splitePenalty;
		float vMin;
		float vMax;

		leftCost	= 0;
		rightCost = 0;
		splitePenalty = 0;
		for (i = 0; i < boxCount; i ++) {
			const ZeroVector3 &pMin = boxArray[i].m_min;
			const ZeroVector3 &pMax = boxArray[i].m_max;

			vMin = pMin[axis];
			vMax = pMax[axis];
			if (vMax < value) {
				leftCost ++;
			} else if (vMin >= value) {
				rightCost ++;
			} else {
				splitePenalty += 1;
			}
		}
		return splitePenalty + abs (rightCost - leftCost);
	}


	int SpliteBoxArray (ZeroAABBBuilderBox boxArray[], int boxCount)
	{
		int i;
		int axis;
		int xCost;
		int yCost;
		int zCost;
		int value;
		int first;
		float axisValue;

		ZeroVector3 min;
		ZeroVector3 max;
		GetExtents(min, max);

		_ASSERTE (boxCount >= 2);
		xCost = CalcCost (boxArray, boxCount, 0, (max.x + min.x) * 0.5f);
		yCost = CalcCost (boxArray, boxCount, 1, (max.y + min.y) * 0.5f);
		zCost = CalcCost (boxArray, boxCount, 2, (max.z + min.z) * 0.5f);

		axis = 0;
		value	= xCost;
		if (yCost < value) {
			axis = 1;
			value	= yCost;
		}
		if (zCost < value) {
			axis = 2;
			value	= zCost;
		}

		axisValue = (min[axis] + max[axis]) * 0.5f;


		first	= 0;
		for (i = 0; i < boxCount; i ++) {
			const ZeroVector3 &pMax = boxArray[i].m_max;
			if (pMax[axis] < axisValue) {
				ZeroSwap (boxArray[first], boxArray[i]);
				first ++;
			}
		}

#ifdef _DEBUG
		for (i = 0; i < first; i ++) {
			_ASSERTE (boxArray[i].m_max[axis] < axisValue);
			_ASSERTE (boxArray[i].m_min[axis] < axisValue);
		}
#endif

		if (first == boxCount) {
			first	= boxCount / 2;
		}
		if (!first) {
			first	= boxCount / 2;
		}

		return first;
	}


	void ShufleVertexAndIndices (
		TreeNode entry, 
		int indexmap[], 
		int& indx, 
		const ZeroVector3 input[],
		void (*meterFunc)(const int addCur, const int addMax))	const
	{
		int i;
		int j;
		int count;
		int baseIndex;

		if (meterFunc) {
			meterFunc (1, 0);
		}


		count = entry.GetIndexCount();
		baseIndex = entry.GetFaceIndices();

		for (i = 0; i < count ; i ++) {
			j = m_indexArray[baseIndex + i];
			if (unsigned (indexmap[j]) == 0xffffffff) {
				m_vertexArray[indx].x = input[j].x;
				m_vertexArray[indx].y = input[j].y;
				m_vertexArray[indx].z = input[j].z;
				indexmap[j] = indx;
				indx ++;
			}
		}
	}

	void SortVertexArray (
		int indexmap[], 
		int& indx, 
		const ZeroVector3 input[],
		void (*meterFunc)(const int addCur, const int addMax)) const
	{
		if (m_back.IsLeafNode()) {
			ShufleVertexAndIndices (m_back, indexmap, indx, input, meterFunc);
		} else {
			m_back.GetNodePointer()->SortVertexArray (indexmap, indx, input, meterFunc);
		}

		if (m_front.IsLeafNode()) {
			ShufleVertexAndIndices (m_front, indexmap, indx, input, meterFunc);
		} else {
			m_front.GetNodePointer()->SortVertexArray (indexmap, indx, input, meterFunc);
		}
	}

	void RemapIndex (TreeNode entry, const int indexMap[]) const
	{
		int i;
		int j;
		int count;
		int baseIndex;

		count = entry.GetIndexCount();
		baseIndex = entry.GetFaceIndices();

		for (i = 0; i < count ; i ++) {
			j = m_indexArray[baseIndex + i];
			m_indexArray[baseIndex + i] = indexMap[j];
		}
	}

		
	void RemapIndex (const int indexMap[]) const
	{
		if (m_back.IsLeafNode()) {
			RemapIndex (m_back, indexMap);
		} else {
			m_back.GetNodePointer()->RemapIndex (indexMap);
		}

		if (m_front.IsLeafNode()) {
			RemapIndex (m_front, indexMap);
		} else {
			m_front.GetNodePointer()->RemapIndex (indexMap);
		}
	}

public: 
	void Build (
		ZeroAABBBuilderBox boxArray[], 
		int boxCount, 
		ZeroAABBTree*& allocator,
		void (*meterFunc)(const int addCur, const int addMax))
	{
		int frontCount;
		int backCount;
		int spliteCentre;

		_ASSERTE (boxCount > 1);
		CalcBoxFromBoxArray (boxArray, boxCount);
		spliteCentre = SpliteBoxArray (boxArray, boxCount);


		if (meterFunc) {
			meterFunc (1, 0);
		}


		backCount = spliteCentre;
		if (backCount > 1) {
			m_back.SetNodePointer(allocator);
			allocator ++;
		}

		frontCount = boxCount - spliteCentre;
		if (frontCount > 1) {
			m_front.SetNodePointer(allocator);
			allocator ++;
		}

		if (backCount > 1) {
			m_back.GetNodePointer()->Build (&boxArray[0], backCount, allocator, meterFunc);
		} else {
			m_back.SetLeafNode(boxArray[0].m_poygonIndex, boxArray[0].m_indexCount);
		}

		if (frontCount > 1) {
			m_front.GetNodePointer()->Build (&boxArray[spliteCentre], frontCount, allocator, meterFunc);
		} else {
			m_front.SetLeafNode(boxArray[spliteCentre].m_poygonIndex, boxArray[spliteCentre].m_indexCount);
		}
	}

	void SortVertexArray (
		int indexCount, 
		int vertexCount,
		void (*meterFunc)(const int addCur, const int addMax)) const
	{
		int indx;
		ZeroStack<int> indexMap (vertexCount);
		ZeroStack<ZeroVector3> pool (vertexCount);

		indx = 0;
		memset (&indexMap[0], 0xff, sizeof (int) * vertexCount);
		memcpy (&pool[0], m_vertexArray, sizeof (ZeroVector3) * vertexCount);

		SortVertexArray (&indexMap[0], indx, &pool[0], meterFunc);
		_ASSERTE (indx == vertexCount);

		RemapIndex (&indexMap[0]);
	}

	void SetVertexIndexArray (int *indexArray, ZeroVector3* vertexArray)
	{
		m_indexArray = indexArray;
		m_vertexArray = vertexArray;
	}
	void SetScale (const ZeroVector3 &scale)
	{
		m_scale = scale;
	}

	int CalcNodeCount () const
	{
		int count;
		count = 0;
		CalcNodeCount (count);
		return count;
	}

	void ForAllSectors (
		const ZeroVector3& min, 
		const ZeroVector3& max, 
		ZeroIntersectCallback callback, 
		void* context) const
	{
		int stack;
		int count;
		int baseIndex;
		const ZeroAABBTree *me;
		const ZeroAABBTree *stackPool[32];

		stack	= 1;
		stackPool[0] = this;
		while (stack) {
			stack	--;
			me	= stackPool[stack];
			if (me->BoxIntersect (min, max)) {
				if (me->m_back.IsLeafNode()) {
					count = me->m_back.GetIndexCount();
					baseIndex = me->m_back.GetFaceIndices();
					if (callback(context, &m_indexArray[baseIndex], count, &m_vertexArray[0].x, sizeof (ZeroVector3)) == stopSearch) {
						break;
					}
				}	else {
					_ASSERTE (stack < 32);
					stackPool[stack] = me->m_back.GetNodePointer();
					stack++;
				}

				if (me->m_front.IsLeafNode()) {
					count = me->m_front.GetIndexCount();
					baseIndex = me->m_front.GetFaceIndices();
					if (callback(context, &m_indexArray[baseIndex], count, &m_vertexArray[0].x, sizeof (ZeroVector3)) == stopSearch) {
						break;
					}
				} else {
					_ASSERTE (stack < 32);
					stackPool[stack] = me->m_front.GetNodePointer();
					stack ++;
				}
			}
		}
	}


	void ForAllSectorsRayHit (
		const ZeroVector3& l0, 
		const ZeroVector3& l1,
		ZeroIntersectCallback callback, 
		void* context) const
	{

		int stack;
		int count;
		int baseIndex;
		const ZeroAABBTree *me;
		const ZeroAABBTree *stackPool[32];

		stack	= 1;
		stackPool[0] = this;
		while (stack) {
			stack	--;
			me	= stackPool[stack];
			if (me->RayIntersect (l0, l1)) {
				if (me->m_back.IsLeafNode()) {
					count = me->m_back.GetIndexCount();
					baseIndex = me->m_back.GetFaceIndices();
					if (callback(context, &m_indexArray[baseIndex], count, &m_vertexArray[0].x, sizeof (ZeroVector3)) == stopSearch) {
						break;
					}
				}	else {
					_ASSERTE (stack < 32);
					stackPool[stack] = me->m_back.GetNodePointer();
					stack++;
				}

				if (me->m_front.IsLeafNode()) {
					count = me->m_front.GetIndexCount();
					baseIndex = me->m_front.GetFaceIndices();
					if (callback(context, &m_indexArray[baseIndex], count, &m_vertexArray[0].x, sizeof (ZeroVector3)) == stopSearch) {
						break;
					}
				} else {
					_ASSERTE (stack < 32);
					stackPool[stack] = me->m_front.GetNodePointer();
					stack ++;
				}
			}
		}

	}


	void Save (ZeroFile &file, const ZeroAABBTree* root) const
	{
#if (PLATFORM == PLATFORM_X86_WIN32)
		int index;

		file.Write (&m_min, sizeof (m_min), 1);
		file.Write (&m_max, sizeof (m_max), 1);

		if (m_back.IsLeafNode()) {
			file.Write (&m_back, sizeof (m_back), 1);
		} else {
			index = m_back.GetNodePointer() - root;
			index *= 2;
			file.Write (&index, sizeof (m_back), 1);
		}

		if (m_front.IsLeafNode()) {
			file.Write (&m_front, sizeof (m_back), 1);
		} else {
			int index;
			index = m_front.GetNodePointer() - root;
			index *= 2;
			file.Write (&index, sizeof (m_back), 1);
		}
#endif
	}

	void Load (ZeroFile &file, ZeroAABBTree* root)
	{
		int index;

		file.Read32 (&m_min, sizeof (m_min), 1);
		file.Read32 (&m_max, sizeof (m_max), 1);

		file.Read32 (&m_back, sizeof (m_back), 1);
		file.Read32 (&m_front, sizeof (m_back), 1);

		if (!m_back.IsLeafNode()) {
			index = unsigned (m_back.GetNodePointer());
			index /= 2;
			m_back.SetNodePointer(root + index);
		}

		if (!m_front.IsLeafNode()) {
			index = unsigned (m_front.GetNodePointer());
			index /= 2;
			m_front.SetNodePointer(root + index);
		}
	}


	void DebugPrint (ZeroFile &file, const ZeroAABBTree* root) const
	{
#if (PLATFORM == PLATFORM_PS2)
	_ASSERTE(0); // We shouldn't be in here!
#else
		int index;

		index = this - root;
		file.Print ("node: %d\n", index);

		ZeroVector3 min;
		ZeroVector3 max;
		GetExtents(min, max);
		file.Print ("min %f %f %f\n", min.x, min.y, min.z);
		file.Print ("max %f %f %f\n", max.x, max.y, max.z);

		if (m_back.IsLeafNode()) {
			file.Print ("leaf %d %d\n", m_back.GetIndexCount(), m_back.GetFaceIndices());
		} else {
			index = m_back.GetNodePointer() - root;
			_ASSERTE (index > 0);
			file.Print ("backNode %d\n", index);
		}

		if (m_front.IsLeafNode()) {
			file.Print ("leaf %d %d\n", m_front.GetIndexCount(), m_front.GetFaceIndices());
		} else {
			int index;
			index = m_front.GetNodePointer() - root;
			_ASSERTE (index > 0);
			file.Print ("frontNode %d\n", index);
		}
#endif
	}
};

int *ZeroAABBTree::m_indexArray;
ZeroVector3 *ZeroAABBTree::m_vertexArray;
ZeroVector3 ZeroAABBTree::m_scale;


ZeroAABB::ZeroAABB ()
	:ZeroPolySoup()
{
	m_vertexCount = 0;
	m_indexCount = 0;
	m_vertexPool = NULL;
	m_indexPool = NULL;
	m_aabb = NULL;
}

ZeroAABB::~ZeroAABB ()
{
	if (m_aabb) {
		delete[] m_aabb;
	}
	if (m_indexPool) {
		clsnAlloc->free(m_indexPool);
	}
	if (m_vertexPool) {
		clsnAlloc->free(m_vertexPool);
	}
}


void ZeroAABB::Create___ (
	const ZeroPolySoupBuilder &builder,
	void (*meterFunc)(const int addCur, const int addMax))
{

	int i;
	int polygonIndex;
	const int* indices;
	const ZeroVector3* pool;

	if (builder.m_faceCount <= 0) {
		return;
	}

	if (meterFunc ) {
		meterFunc (0, builder.m_faceCount * 3);
	}


	m_vertexCount = builder.m_vertexCount;
	m_indexCount = builder.m_indexCount;

	ZeroStack<ZeroAABBBuilderBox> boxArray (builder.m_faceCount);

	pool = &builder.m_vertexArray[0];
	indices = &builder.m_indexArray[0];
	polygonIndex = 0;
	for (i = 0; i < builder.m_faceCount; i ++) {
		int indexCount;

		if (meterFunc ) {
			meterFunc (1, 0);
		}

		boxArray[i].m_poygonIndex = polygonIndex + 1;
		indexCount = builder.m_faceArray[i];
		boxArray[i].CalcExtends (indexCount - 1, &indices[polygonIndex + 1], pool);
		polygonIndex += indexCount;
	}

	m_vertexPool = ZeroNew (ZeroVector3[m_vertexCount]);
	m_indexPool = ZeroNew (int[m_indexCount]);

	m_scale.x = 0.0f;
	m_scale.y = 0.0f;
	m_scale.z = 0.0f;
	for (i = 0; i <  m_vertexCount; i ++) {
		m_vertexPool[i] = pool[i]; 
		if (m_scale.x < fabsf(pool[i].x))
			m_scale.x = fabsf(pool[i].x);
		if (m_scale.y < fabsf(pool[i].y))
			m_scale.y = fabsf(pool[i].y);
		if (m_scale.z < fabsf(pool[i].z))
			m_scale.z = fabsf(pool[i].z);
	}
	m_scale.x /= 128.0f;
	m_scale.y /= 128.0f;
	m_scale.z /= 128.0f;

	memcpy (m_indexPool, indices, sizeof (int) * m_indexCount); 

	if (builder.m_faceCount > 1) {
		m_aabb = ZeroNew (ZeroAABBTree[builder.m_faceCount - 1]);
		memset (m_aabb, 0, sizeof (ZeroAABBTree) * (builder.m_faceCount - 1));

		m_aabb->SetVertexIndexArray (m_indexPool, m_vertexPool);
		m_aabb->SetScale (m_scale);

		ZeroAABBTree* allocator = &m_aabb[1];
		m_aabb->Build ( &boxArray[0], builder.m_faceCount, allocator, meterFunc);

		m_aabb->SortVertexArray (builder.m_indexCount, m_vertexCount, meterFunc);
	}
}

void ZeroAABB::DebugSave (ZeroFile &file) const
{
#if (PLATFORM == PLATFORM_PS2)
	_ASSERTE(0); // We shouldn't be in here!
#else
	int i;
	int nodes;

	file.Print ("Vertex Count %d\n", m_vertexCount);
	for (i = 0; i < m_vertexCount; i ++) {
		file.Print ("%d: %f %f %f\n", i,
			m_vertexPool[i].x,
			m_vertexPool[i].y,
			m_vertexPool[i].z);
	}
	file.Print ("\n");

	file.Print ("Index Count %d\n", m_indexCount);
	for (int j = 0; j < m_indexCount; j += 3) {
		file.Print ("%d: ", j);
		for (i = 0; i < 3; i ++) {
			file.Print ("%d ", m_indexPool[j + i]);
		}
		file.Print ("\n");
	}
	file.Print ("\n");

	if (m_aabb) {
		file.Print ("\n");
		m_aabb->SetVertexIndexArray (m_indexPool, m_vertexPool);
		m_aabb->SetScale (m_scale);
		nodes = m_aabb->CalcNodeCount ();
		file.Print ("Nodes Count %d\n", nodes);
		for (i = 0; i < nodes; i ++) {
			m_aabb[i].DebugPrint (file, m_aabb);
			file.Print ("\n");
		}
	}
	file.Flush();
#endif
}


void ZeroAABB::Save___ (ZeroFile &file) const
{
#if (PLATFORM == PLATFORM_X86_WIN32)
	int i;
	int nodes;

	unsigned int type = '1TBA';
	file.Write (&type, sizeof(type), 1); 

	file.Write (&m_vertexCount, sizeof (m_vertexCount), 1);
	file.Write (m_vertexPool, sizeof (m_vertexPool[0]), m_vertexCount);

	file.Write (&m_indexCount, sizeof (m_indexCount), 1);
	file.Write (m_indexPool, sizeof (m_indexPool[0]), m_indexCount);

	file.Write (&m_scale, sizeof (m_scale), 1);

	if (m_aabb) {
		m_aabb->SetVertexIndexArray (m_indexPool, m_vertexPool);
		m_aabb->SetScale (m_scale);
		nodes = m_aabb->CalcNodeCount ();
		file.Write (&nodes, sizeof (nodes), 1);
		for (i = 0; i < nodes; i ++) {
			m_aabb[i].Save (file, m_aabb);
		}
	} else {
		nodes = 0;
		file.Write (&nodes, sizeof (nodes), 1);
	}

	file.Flush();
#endif
}

bool ZeroAABB::Load___ (ZeroFile& file)
{
	int i;
	int nodes;
	unsigned int type;

	file.Read32 (&type, sizeof(unsigned int), 1);
	if (type != '1TBA')
	{
		return false;
	}

	file.Read32 (&m_vertexCount, sizeof (m_vertexCount), 1);
	m_vertexPool = (ZeroVector3 *)clsnAlloc->malloc(sizeof(*m_vertexPool) * m_vertexCount);
	file.Read32 (m_vertexPool, sizeof (m_vertexPool[0]), m_vertexCount);

	file.Read32 (&m_indexCount, sizeof (m_indexCount), 1);
	m_indexPool = (int *)clsnAlloc->malloc(sizeof(*m_indexPool) * m_indexCount);
	file.Read32 (m_indexPool, sizeof (m_indexPool[0]), m_indexCount);

	file.Read32 (&m_scale, sizeof (m_scale), 1);

	file.Read32 (&nodes, sizeof (nodes), 1);
	if (nodes > 0) {
		m_aabb = ZeroNew (ZeroAABBTree[nodes]);
		m_aabb->SetVertexIndexArray (m_indexPool, m_vertexPool);
		m_aabb->SetScale (m_scale);
		for (i = 0; i < nodes; i ++) {
			m_aabb[i].Load (file, m_aabb);
		}
	}
	return true;
}


void ZeroAABB::ForAllSectors (
	const ZeroVector3& min, 
	const ZeroVector3& max, 
	ZeroIntersectCallback callback, 
	void* context) const
{
	if (m_aabb)
	{
		m_aabb->SetVertexIndexArray (m_indexPool, m_vertexPool);
		m_aabb->SetScale (m_scale);
		m_aabb->ForAllSectors (min, max, callback, context);
	}
	else
	{
		callback(context, &m_indexPool[0], m_indexCount, &m_vertexPool[0].x, sizeof (ZeroVector3));
	}
}



void ZeroAABB::ForAllSectorsRayHit (
	const ZeroVector3& p0, 
	const ZeroVector3& p1, 
	ZeroIntersectCallback callback, 
	void* context) const
{
	if (m_aabb)
	{
		m_aabb->SetVertexIndexArray (m_indexPool, m_vertexPool);
		m_aabb->SetScale (m_scale);
		m_aabb->ForAllSectorsRayHit (p0, p1, callback, context);
	}
	else
	{
		callback(context, &m_indexPool[0], m_indexCount, &m_vertexPool[0].x, sizeof (ZeroVector3));
	}
}


namespace InternalAABB
{
	void Startup(void)
	{
		clsnAlloc = ZeroAllocator::GetGenericAllocator();
	}
}
