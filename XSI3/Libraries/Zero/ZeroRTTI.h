/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroRtti__
#define __ZeroRtti__

#include "ZeroTypes.h"
#include "ZeroCrc.h"



class ZeroRttiInfo
{
	unsigned typeId;

#ifdef _DEBUG
	char typeName[64]; // useful during debugging to see an object's type
#endif //_DEBUG

public:
	ZeroRttiInfo (const char *classID)
	{
#ifdef _DEBUG
		strncpy (typeName, classID, sizeof (typeName) - 1);
#endif //_DEBUG
		typeId = CalcLowerCRC(classID, strlen (classID), 0);
	}

	unsigned GetTypeID() const
	{
		return typeId;
	}

	bool IsTypeID(unsigned id) const 
	{
		return typeId == id;
	}

#ifdef _DEBUG
	const char *GetTypeName() const
	{
		return typeName;
	}
#else
	const char *GetTypeName() const
	{
		return "<Unknown>";
	}
#endif
};




#define ZERO_RTTI(BaseClass)						\
	private:										\
	static ZeroRttiInfo rtti;						\
	public:											\
	virtual bool IsTypeID(unsigned idcrc) const		\
	{												\
		if (rtti.IsTypeID(idcrc)) {					\
			return true;							\
		}											\
		return BaseClass::IsTypeID (idcrc);			\
	}												\
	virtual unsigned GetTypeID() const				\
	{												\
		return rtti.GetTypeID();					\
	}												\
	static unsigned GetRttiTypeID()					\
	{												\
		return rtti.GetTypeID();					\
	}												\
	virtual const char *GetTypeName() const			\
	{												\
		return rtti.GetTypeName();					\
	}												\
	static const char *GetRttiTypeName()			\
	{												\
		return rtti.GetTypeName();					\
	}												\



#define ZERO_RTTI_CONTRUCTOR(ClassType)				\
	ZeroRttiInfo ClassType::rtti (""#ClassType"");




#endif
