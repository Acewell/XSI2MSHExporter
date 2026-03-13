/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"
#include "ZeroUserData.h"


ZERO_RTTI_CONTRUCTOR(ZeroUserData);							  


//
// USER DATA LIST DESTRUCTOR
//
ZeroUserDataList::~ZeroUserDataList ()
{
	ZeroUserData *data;

	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		data = *iter;
		data->Kill();
		ZeroRelease(data, "ZeroUserDataList");
	}
}

//
// USER DATA LIST ATTACH DATA
//
void ZeroUserDataList::AttachData (ZeroUserData *userDataArg)
{
	unsigned crc;

	if (userDataArg) {
		crc = userDataArg->GetNameCRC();
		Iterator iter (*this);
		for (iter.Begin(); iter; iter ++) {
			if ((*iter)->GetNameCRC() == crc) {
				break;
			}
		}

		if (iter) {
			(*iter)->Kill();
			ZeroRelease(*iter, "ZeroUserDataList");
			Remove (iter.GetNode());
		}

		Append (userDataArg);
		ZeroAddRef(userDataArg, "ZeroUserDataList");
	}
}

//
// USER DATA LIST FIND DATA
//
ZeroUserData *ZeroUserDataList::FindUserData (unsigned CRC) const 
{
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		if ((*iter)->GetNameCRC() == CRC) {
			return *iter;
		}
	}
	return NULL;
}

void ZeroUserDataList::Purge ()
{
	ZeroUserDataList::ZeroListNode *node;

	Iterator iter (*this);
	for (iter.Begin(); iter; ) {
		node = iter.GetNode();
		iter ++;
		if (!node->info->IsAlive()) {
			ZeroRelease(node->info, "ZeroUserDataList");
			Remove(node);
		}
	}
}


//
// USER DATA LIST REMOVE DATA
//
void ZeroUserDataList::RemoveUserData (unsigned CRC)
{
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		if ((*iter)->GetNameCRC() == CRC) {
			(*iter)->Kill();
			ZeroRelease(*iter, "ZeroUserDataList");
			Remove(iter.GetNode());
			return;
		}
	}
}

//
// USER DATA LIST SAVE
//
void *ZeroUserDataList::Save (void *data)
{
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		data = (*iter)->Save (data);
	}
	return data;
}

//
// USER DATA LIST LOAD
//
void *ZeroUserDataList::Load (void *data)
{
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		data = (*iter)->Load (data);
	}
	return data;
}

//
// USER DATA LIST UPDATE
//
void ZeroUserDataList::Update (float dt)
{
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		(*iter)->Update (dt);
	}
}
