#ifndef __ZeroClothGeometry__
#define __ZeroClothGeometry__

#include "ZeroMeshGeometry.h"



class ZeroTransformContraintGeometry;




struct ZeroClothVertexEdge
{
	short particleIndex;
	short next;
	short twin;
};


struct ZeroBendEdge
{
	short v0;
	short v1;
};


struct ZeroClothCollisionVolume
{
	enum VOLUME_TYPE
	{
		SPHERE,
		CYLINDER,
		CONE,
		FORCE_32BITS = 0xffffffff
	};
	
	VOLUME_TYPE	type;
	int parentIndex;
	float raddi0;
	float raddi1;
	float height;
	float offsetMatrix[4][4];
};


class ZeroClothData
{
public:
	int edgeCount;
	int bendPairsCount;
	int particlesCount;
	int nailedPointsCount;
	int indexTriangleCount;
	int particlesIndexMapCount;
	int collisionCount;
	
	int* indexTriangleList;
	int* particlesIndexMap;
	int* nailedPoints;
	ZeroUV* geometryUV;
	ZeroBendEdge *bendPairs;
	ZeroVector3* particlesPosit;
	ZeroClothVertexEdge* clothVertexEdge;
	
	ZeroClothCollisionVolume *collisionVolumes;
	
	ZeroClothData ()
	{
		edgeCount = 0;
		bendPairsCount = 0;
		particlesCount = 0;
		collisionCount	= 0;
		nailedPointsCount	= 0;
		indexTriangleCount = 0;
		particlesIndexMapCount = 0;
		
		bendPairs = NULL;
		nailedPoints = NULL;
		particlesPosit = NULL;
		clothVertexEdge = NULL;
		collisionVolumes = NULL;
		indexTriangleList = NULL;
		particlesIndexMap = NULL;
	}
};


class ZeroClothPhysics; 
class ZeroClothCollision; 

class ZeroClothGeometry: public ZeroMeshGeometry
{
	ZERO_RTTI(ZeroMeshGeometry);
	
	friend class ZeroClothPhysics;
	
	ZeroClothPhysics* physics;
	ZeroVector4* particlePosit;
	ZeroVector4* particleVeloc;
	ZeroVector4* particleNormal;
	
	int collisionCount;
	ZeroTransformContraintGeometry **collisionContraint;
	
	ZeroVector4 windVeloc;
	mutable ZeroVector4 prevPosit;
	
protected:
	
	void Scale (float scale);
	~ZeroClothGeometry();
	
public:
	ZeroClothGeometry(const ZeroGeometryData &data);
	ZeroClothGeometry(const ZeroClothGeometry &clone);
	
	
	virtual ZeroUnknown *CreateClone(bool isSim);
	virtual void CloneFixUp (const ZeroObject& myOwner, const ZeroObject& myClone);
	
	void RemapCollisions(ZeroObject *owner); 
	int* GetIndexMap();
	
	void SetWindVelocity(const ZeroVector4& wind)
	{
		windVeloc = wind;
	}
	
	
	virtual void Render (const ZeroObject *owner, ZeroCamera *camera, const ZeroMatrix &world, unsigned flags = 0) const;
};


#endif