#include "ZeroTypes.h"

#ifdef TOOL_DEVELOPMENT 

#include "ZeroHeap.h"
#include "ZeroStack.h"

#include "ZeroVectorQuantize.h"

class CovarianceVector
{
private:
	int size;
	double *data;

public:
	CovarianceVector(const int size)
		: size (size)
	{
		data = ZeroNew (double[size]);
	}

	~CovarianceVector()
	{
		delete[] data;
	}

	double GetElement(const int index) const
	{
		return data[index];
	}

	void SetElement(const int index, const double value)
	{
		data[index] = value;
	}

	void AddElement(const int index, const double value)
	{
		data[index] += value;
	}

	double DotProduct(const CovarianceVector &vector) const
	{
		double dot = GetElement(0) * vector.GetElement(0);
		for (int i = 1; i < size; i++)
		{
			dot += GetElement(i) * vector.GetElement(i);
		}
		return dot;
	}

	double DotProduct(const float data[]) const
	{
		double dot = GetElement(0) * data[0];
		for (int i = 1; i < size; i++)
		{
			dot += GetElement(i) * data[i];
		}
		return dot;
	}
};

class CovarianceMatrix
{
private:
	int size;
	double *data;

public:
	CovarianceMatrix(const int size)
		: size (size)
	{
		data = ZeroNew (double[size * size]);
	}

	~CovarianceMatrix()
	{
		delete[] data;
	}

	double GetElement(const int col, const int row) const
	{
		return data[row * size + col];
	}

	void SetElement(const int col, const int row, const double value)
	{
		data[row * size + col] = value;
	}

	void AddElement(const int col, const int row, const double value)
	{
		data[row * size + col] += value;
	}

	void Rotate(const int col0, const int row0, const int col1, const int row1, const double s, const double tau)
	{
		double e0 = GetElement (col0, row0);
		double e1 = GetElement (col1, row1);
		
		SetElement (col0, row0, e0 - s * (e1 + e0 * tau)); 
		SetElement (col1, row1, e1 + s * (e0 - e1 * tau));
	}

	double PrincipalAxis(CovarianceVector &axis)
	{
		int i;
		int j;
		int x;
		int y;
		int nrot;
		double thresh;
		double theta;
		double tau;
		double t;
		double sm;
		double s;
		double h;
		double g;
		double c;
		CovarianceVector b(size);
		CovarianceVector z(size);
		CovarianceVector d(size);
		CovarianceMatrix eigen(size);
		
		for (y = 0; y < size; y ++)
		{
			eigen.SetElement (y, y, 1.0); 
			b.SetElement (y, GetElement (y, y));
			d.SetElement (y, GetElement (y, y));
			z.SetElement (y, 0);
			for (x = y + 1; x < size; x ++)
			{
				eigen.SetElement (x, y, 0.0); 
				eigen.SetElement (y, x, 0.0); 
			}
		}
		
		nrot = 0;
		for (i = 0; i < 50; i++)
		{
			sm = 0.0;
			for (y = 0; y < size - 1; y ++)
			{
				for (x = y + 1; x < size; x ++)
				{
					sm += fabs (GetElement (x, y)); 
				}
			}
			
			if (sm < 1e-15)
			{
				// order the eigen vectors	
				sm = 0.0;
				i = 0;
				for (y = 0; y < size; y ++)
				{
					if (d.GetElement(y) > sm)
					{
						sm = d.GetElement(y);
						i = y;
					}
				}
				for (y = 0; y < size; y ++)
				{
					axis.SetElement (y, eigen.GetElement (i, y));
				}
				return sm;
			}
			
			if (i < 3)
			{
				thresh = (0.2 / size * size) * sm; 
			}
			else
			{
				thresh = 0.0;
			}
			
			for (y = 0; y < size - 1; y ++)
			{
				for (x = y + 1; x < size; x ++)
				{
					g = 100.0 * fabs(GetElement (x, y));
					if (i > 3 && 
						fabs(d.GetElement(y)) + g == fabs(d.GetElement(y)) && 
						fabs(d.GetElement(x)) + g == fabs(d.GetElement(x)))
					{
						SetElement (x, y, 0.0); 
					}
					else if (fabs(GetElement (x, y)) > thresh)
					{
						h = d.GetElement(x) - d.GetElement(y);
						if (fabs(h) + g == fabs(h))
						{
							t = GetElement (x, y) / h;
						}
						else
						{
							theta = 0.5 * h / GetElement (x, y);
							t = 1.0 / (fabs(theta) + sqrt(1.0 + theta * theta));
							if (theta < 0.0) t = -t;
						}
						c = 1.0 / sqrt(1 + t * t); 
						s = t * c; 
						tau = s / (1.0 + c); 
						h = t * GetElement (x, y);
						z.AddElement (y, -h); 
						z.AddElement (x, h); 
						d.AddElement (y, -h); 
						d.AddElement (x, h); 
						SetElement (x, y, 0.0);
						for (j = 0; j < y; j ++)
						{
							Rotate (y, j, x, j, s, tau); 
						}
						for (j = y + 1; j < x; j ++)
						{
							Rotate (j, y, x, j, s, tau); 
						}
						for (j = x + 1; j < size; j ++)
						{
							Rotate (j, y, j, x, s, tau); 
						}
						for (j = 0; j < size; j ++)
						{
							eigen.Rotate (y, j, x, j, s, tau); 
						}
						nrot++;
					}
				}
			}
			
			for (x = 0; x < size; x ++)
			{
				b.AddElement(x, z.GetElement(x)); 
				d.SetElement(x, b.GetElement(x)); 
				z.SetElement(x, 0.0); 
			}
		}
		
		axis.SetElement (0, 1.0);
		for (y = 1; y < size; y ++)
		{
			axis.SetElement (y, 0.0);
		}

		return 1.0;
	}
};

class DataPartition
{
public:
	DataPartition *parent;
	DataPartition *left;
	DataPartition *right;

	int lower;
	int upper;

	CovarianceVector axis;
	double dist;
	double dev;

public:
	DataPartition(DataPartition *parentPtr, const int vectorSize,
		const float dataArray[], const int dataCount,
		int dataIndex[], int indexLower, int indexUpper)
		: parent(parentPtr), left(NULL), right(NULL),
		lower(indexLower), upper(indexUpper), axis(vectorSize), dist(0.0), dev(0.0)
	{
		int i;
		int x;
		int y;

		// center of mass vector
		CovarianceVector center(vectorSize);

		// covariance matrix
		CovarianceMatrix matrix(vectorSize);
		
		// clear out the center and matrix
		for (y = 0; y < vectorSize; y ++)
		{
			center.SetElement (y, 0.0);
			for (x = y; x < vectorSize; x ++)
			{
				matrix.SetElement (x, y, 0.0);
			}
		}
		
		// compute the moments
		double scale = 1.0 / (upper - lower);
		for (i = lower; i < upper; i ++)
		{
			const float *dataPtr = &dataArray[dataIndex[i] * vectorSize];
			for (y = 0; y < vectorSize; y ++)
			{
				float dataY = dataPtr[y];
				center.AddElement (y, dataY * scale);
				matrix.AddElement (y, y, dataY * dataY * scale);
				for (x = y + 1; x < vectorSize; x ++)
				{
					float dataX = dataPtr[x];
					matrix.AddElement (x, y, dataX * dataY * scale);
				}
			}
		}
	
		// fix up the covariance values
		for (y = 0; y < vectorSize; y ++)
		{
			for (x = y; x < vectorSize; x ++)
			{
				double data = matrix.GetElement(x, y) - center.GetElement(x) * center.GetElement(y);
				matrix.SetElement (x, y, data);
				matrix.SetElement (y, x, data);
			}
		}

		// get the principal axis
		dev = matrix.PrincipalAxis(axis);

		// get the distance value
		dist = axis.DotProduct(center);
	}

	~DataPartition()
	{
		if (left)
		{
			delete left;
		}
		if (right)
		{
			delete right;
		}
	}

	void Split(const int vectorSize,
		const float dataArray[], const int dataCount,
		int dataIndex[])
	{
		// partition the indices
		int l = lower;
		int u = upper - 1;
		for (;;)
		{
			while (axis.DotProduct(&dataArray[dataIndex[l]*vectorSize]) < dist)
				l++;
			while (axis.DotProduct(&dataArray[dataIndex[u]*vectorSize]) >= dist)
				u--;
			if (l >= u)
				break;
			int t = dataIndex[l];
			dataIndex[l] = dataIndex[u];
			dataIndex[u] = t;
			l++;
			u--;
			if (l >= u)
				break;
		}

		left = ZeroNew (DataPartition(this, vectorSize, dataArray, dataCount, dataIndex, lower, l));
		right = ZeroNew (DataPartition(this, vectorSize, dataArray, dataCount, dataIndex, u + 1, upper));
	}
};


void VectorQuantize(const int vectorSize,
					const float dataArray[], const int dataCount,
					float codeArray[], const int codeCount, int codeIndex[],
					void (*meterFunc)(const int addCur, const int addMax))
{
	int i, j;

	// initialize the data index array
	ZeroStack<int> dataIndex(dataCount);
	for (i = 0; i < dataCount; i++)
	{
		dataIndex[i] = i;
	}

	// create a partition heap
	ZeroHeap<DataPartition *, double> partitionHeap(codeCount + 1, DBL_MAX);

	// initialize the meter
	if (meterFunc)
		meterFunc(0, codeCount);

	// create a root partition
	DataPartition *partitionRoot = ZeroNew (DataPartition(NULL, vectorSize, dataArray, dataCount, &dataIndex[0], 0, dataCount));

	// add the root to the heap
	partitionHeap.Push(partitionRoot, partitionRoot->dev);

	// update the meter
	if (meterFunc)
		meterFunc(1, 0);

	// while the codebook is not full...
	for (i = 1; i < codeCount; i++)
	{
		// get the partition with the largest deviation
		DataPartition *partitionPtr = partitionHeap[0];
		partitionHeap.Pop();

		// split the partition in two
		partitionPtr->Split(vectorSize, dataArray, dataCount, &dataIndex[0]);

		// push the lower partition
		partitionHeap.Push(partitionPtr->left, partitionPtr->left->dev);

		// push the upper partition
		partitionHeap.Push(partitionPtr->right, partitionPtr->right->dev);

		// update the meter
		if (meterFunc)
			meterFunc(1, 0);
	}

	// for each codebook entry...
	for (i = 0; i < codeCount; i++)
	{
		// get the partition
		DataPartition *partitionPtr = partitionHeap[i];

		// average together the contents
		for (j = 0; j < vectorSize; j++)
		{
			codeArray[i * vectorSize + j] = 0.0f;
		}
		for (int l = partitionPtr->lower; l < partitionPtr->upper; l++)
		{
			codeIndex[dataIndex[l]] = i;
			for (j = 0; j < vectorSize; j++)
			{
				codeArray[i * vectorSize + j] += dataArray[dataIndex[l] * vectorSize + j];
			}
		}
		float scale = 1.0f / (partitionPtr->upper - partitionPtr->lower);
		for (j = 0; j < vectorSize; j++)
		{
			codeArray[i * vectorSize + j] *= scale;
		}
	}

	// free all partitions
	delete partitionRoot;
}

#endif //#ifdef TOOL_DEVELOPMENT 
