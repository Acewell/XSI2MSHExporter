/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"
#include "Msh.h"
#include "ZeroFile.h"
#include "ZeroStack.h"
#include "ZeroMatrix.h"
#include "ZeroEntity.h"
#include "ZeroCamera.h"
#include "ZeroUnknown.h"
#include "ZeroGeoUtil.h"
#include "ZeroSegment.h"
#include "ZeroResource.h"
#include "ZeroGeometry.h"
#include "ZeroInterval.h"
#include "ZeroTransform.h"
#include "ZeroCollision.h"
#include "ZeroPolyhedra.h"
#include "ZeroMeshGeometry.h"
#include "ZeroMeshContext.h"


ZERO_RTTI_CONTRUCTOR(ZeroEntity);


namespace InternalMeshLoader
{
	class MeshLoader: public ResourceLoader<ZEROMESH_DRIVER>
	{
		MeshLoader()
			:ResourceLoader<ZEROMESH_DRIVER>()
		{
			SetDefualtFormats ();
		}
		
	public:
		void SetDefualtFormats ()
		{
			int i;
			int Count;
			
			static ZEROMESH_DRIVER ZeroMesh_Ext[] = 
			{
				{ ".msh", MSH::Load, MSH::Save },
			};
			
			Count = sizeof (ZeroMesh_Ext) / sizeof (ZEROMESH_DRIVER);
			for (i = 0; i < Count; i ++) {
				AddResourceLoader (ZeroMesh_Ext[i]);
			}
		}
		
		friend MeshLoader *GetResourceLoader();
		
	};
	
	MeshLoader *GetResourceLoader()
	{
		ZeroStartup::InitEngine();
		static MeshLoader loader;
		return &loader;
	}
	
	void Startup()
	{
		GetResourceLoader();
	}
}


ZeroEntity::ZeroEntity (const char *file, ZeroMeshContext *context)
	: ZeroObject(0), mSize(0, 0, 0), mCollision(NULL), mUserDataList()
{
	char *ext;
	ZeroObject *mesh;
	ZeroObject *child;
	ZeroGeometry *geom;
	
	ext = strrchr (file, '.');
	if (!ext) {
#ifdef _WIN32
		throw "Error: File name without extension";
#else
		printf("Error: File name %s without extension\n",file);
		ZeroTrace(("Error: File name %s without extension\n",file));
#endif
	}
	
	mesh = MSH::Load (file, context);
	if (!mesh) { 
#ifdef _WIN32
		throw "Error: File not found";
#else
		printf("Error: File %s not found\n",file);
		ZeroTrace(("Error: File %s not found\n",file));
#endif
	}
	
	SetObjectRotation(mesh->GetObjectRotation());
	SetObjectPosition(mesh->GetObjectPosition());
	SetDimensions(mesh->GetDimensions());
	
	SetNameCRC (mesh->GetNameCRC());
	if (mesh->GetTransform())
	{
		mTransform = mesh->GetTransform()->CreateClone(this, false);
	}
	
	for (child = mesh->GetChild(); child; child = mesh->GetChild()) {
		child->AddRef();
		child->Detach();
		child->Attach(this);
		child->Release();
	}
	mesh->Release();
	
	// calculate the geometry spheres
	for (child = GetFirst(); child; child = child->GetNext()) {
		geom = child->GetGeometry();
		if (geom && geom->SphereDirty()) {
			geom->SetSphere();
		}
	}
	
	if (context) {
		context->OnFileLoaded(this);
	}
}

ZeroEntity::~ZeroEntity ()
{
	AttachCollision (NULL);
	mUserDataList.Purge();
}



ZeroEntity::ZeroEntity (const ZeroEntity &clone)
	: ZeroObject(clone), mSize(clone.mSize), mCollision(NULL), mUserDataList()
{
	if (clone.mCollision) {
		mCollision = (ZeroCollision*)clone.mCollision->CreateClone (this, IsSim());
		if (mCollision) {
			ZeroOwnRef(mCollision, "ZeroEntity");
		}
	}
 
	ZeroUserDataList::Iterator iter (clone.mUserDataList);
	for (iter.Begin(); iter; iter ++) {
		ZeroUserData *cloneData = *iter;

		// i dont know if i need to get allocation type from clone user data or object isSim
		ZeroUserData *userData = cloneData->CreateClone (this, IsSim());
		AttachUserData (userData);	
		userData->Release();
	}
}


void ZeroEntity::CloneFixUp (const ZeroBaseNode &clone)
{
	const ZeroEntity& cloneObj = (const ZeroEntity&) clone;

	ZeroUserDataList::Iterator iter (mUserDataList);
	ZeroUserDataList::Iterator cloneIter (cloneObj.mUserDataList);
	for (iter.Begin(); iter; iter ++) {
		ZeroUserData *userData = *iter;
		userData->CloneFixUp (*(*cloneIter));
		cloneIter;
	}

	ZeroObject::CloneFixUp (clone);
}

#if (PLATFORM == PLATFORM_X86_WIN32)

void ZeroEntity::UpgradeFileFormat (const ZEROMESH_DRIVER & newFormat)
{
	InternalMeshLoader::GetResourceLoader()->UpdateResourceLoader (newFormat);
}

bool ZeroEntity::SaveMesh (const char *name)
{
	char *ext;
	ZEROMESH_DRIVER *loader;
	
	ext = strrchr (name, '.');
	if (!ext) {
#ifdef _WIN32
		throw "Error: File name without extension";
#else
		printf ("ZeroObject::SaveMesh.. Error, file name without extension\n");
#endif
	}
	
	loader = InternalMeshLoader::GetResourceLoader()->FindLoader (ext);
	if (!loader) {
#ifdef _WIN32
		throw "Error: File format not supported";
#else
		printf ("ZeroObject::SaveMesh - File format not supported\n");
#endif
	}
	
	return loader->Save (name, this);
}

#endif