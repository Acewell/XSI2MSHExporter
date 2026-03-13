#ifndef __ZeroArray__
#define __ZeroArray__

// zero array template definition
template <class T> class ZeroArray
{
public:
	// number of entries
	int count;

	// array contents
	T *data;

	// default constructor
	ZeroArray()
	{
		count = 0;
		data = NULL;
	}

	// count constructor
	ZeroArray(const int c)
	{
		count = c;
		data = ZeroNew (T[count]);
	}

	// component constructor
	ZeroArray(const int c, T * const d)
	{
		count = c;
		data = d;
	}

	// copy constructor
	ZeroArray(const ZeroArray &a)
	{
		count = a.count;
		data = ZeroNew (T[count]);
		for (int i = 0; i < count; i++)
		{
			data[i] = a[i];
		}
	}

	// destructor
	~ZeroArray()
	{
		if (data)
		{
			delete[] data;
		}
	}

	// allocate data
	void Alloc(const int c)
	{
		if (data)
		{
			delete[] data;
		}
		count = c;
		data = ZeroNew (T[count]);
	}

	// free data
	void Free()
	{
		count = 0;
		if (data)
		{
			delete[] data;
			data = NULL;
		}
	}

	// resize
	void Resize(const int c)
	{
		T *old = data;
		data = ZeroNew (T[c]);
		if (old)
		{
			count = min(c, count);
			for (int i = 0; i < count; i++)
			{
				data[i] = old[i];
			}
			delete[] old;
		}
		count = c;
	}

	// copy a c-style array
	void Copy(const int c, const T * const a)
	{
		if (count != c)
		{
			Alloc(c);
		}
		for (int i = 0; i < count; i++)
		{
			data[i] = a[i];
		}
	}

	// copy another array
	void Copy(const ZeroArray<T> &a)
	{
		Copy(a.count, a.data);
	}

	// index operators
	T &operator[](const int index)
	{
		_ASSERTE(index >= 0);
		_ASSERTE(index < count);
		return data[index];
	}
	const T &operator[](const int index) const
	{
		_ASSERTE(index >= 0);
		_ASSERTE(index < count);
		return data[index];
	}
};

#endif
