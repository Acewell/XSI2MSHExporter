#include "ZeroTypes.h"
#include "ZeroStack.h"
#include "ZeroDebug.h"
#include "ZeroSPHeap.h"
#include "ZeroMemory.h"
#include "ZeroCamera.h"
#include "ZeroSegment.h"
#include "ZeroMaterial.h"
#include "ZeroRenderState.h"
#include "ZeroClothGeometry.h"
#include "ZeroRenderDescriptor.h"
#include "ZeroTransformConstraintGeometry.h"



//#define USE_COLLISION_MAP

ZERO_RTTI_CONTRUCTOR(ZeroClothGeometry);							  


const float CLOTH_GRAVITY = 9.8f;
const float MAX_STRETCH_RATE = 0.02f;

const float THREAD_ELASTIC_FREQ = 8.0f; 
const float THREAD_ELASTIC_OVERDAMPING = 4.0f;


const float FABRIC_BEND_FREQ = 1.0f;
const float FABRIC_BEND_OVERDAMPING = 4.0f;


const float MAX_SPEED_CHANGE = 20.0f;
const float BODY_SPEED_VISCUOS_DAMPING = 5.0f;

const float AIR_VISCUOS_DAMPING = 4.0f;

const int COLLISION_MAP_SIZE = 8;
const int COLLISION_MAP_SECTOR_SIZE = 4;


const	int MAX_HEAP_VIOLATIONS = 32;

static ZeroRenderState renderStatesFace[] =
{
	ZeroRenderState (ZERO_RENDERSTATE_DESTBLEND, ZERO_BLEND_ONE),
	ZeroRenderState (ZERO_RENDERSTATE_CULLMODE, ZERO_CULLMODE_NONE),
//	ZeroRenderState (ZERO_RENDERSTATE_FILLMODE, ZERO_FILL_WIREFRAME),
	ZeroRenderState (ZERO_RENDERSTATE_END, 0)
};


 
class ZeroClothPhysics: public ZeroUnknown
{
	friend class ZeroClothGeometry;

	struct FabricFiber
	{
		int particleIndex;
		mutable unsigned cyclingCount : 31;
		mutable unsigned isInTheHeap  :  1;
		float invMassRatio;
		float restLength;
		float maxStretch;
		FabricFiber *twin;
		FabricFiber *next;
	};

	struct BendPair: public ZeroBendEdge
	{
		float invMassRatio0;
		float invMassRatio1;
		float restLength;
	};


	struct CollisionStruct
	{
		ZeroVector4 center;
		float radii0;
		float radiiSquare;
		float height;
		int parentIndex;
	};



	struct ElasticStretchContraint: public ZeroHeap<const FabricFiber* , float>
	{

		unsigned cycleCount;
		ElasticStretchContraint(int size)
			: ZeroHeap<const FabricFiber* , float>(size, 128.0f)
		{
			cycleCount = MAX_HEAP_VIOLATIONS;
		}

		void UpMark()
		{
			cycleCount += MAX_HEAP_VIOLATIONS;
			_ASSERTE (cycleCount < 0x3fffffff); 
		}

		void AddFiber (const FabricFiber* fiber, float errLength) 
		{
			if (fiber->cyclingCount >= cycleCount) {
				return;
			}

			fiber->isInTheHeap = TRUE;
			fiber->twin->isInTheHeap = TRUE;
			if (GetCount() < GetMaxSize()) {
				Push (fiber, errLength);
			} else {
				_ASSERTE (0);
				Pop();
				Push (fiber, errLength);
			}

			fiber->cyclingCount ++;
			if (fiber->cyclingCount	>= cycleCount) {
				const FabricFiber* ptr;
				ptr = fiber;
				do {
					ptr->cyclingCount	= cycleCount;
					ptr = ptr->twin->next;
				} while (ptr != fiber);
			}
		}


		void RemoveWorstFiber()
		{
			const FabricFiber* fiber;

			fiber = (*this)[0];
			fiber->isInTheHeap = FALSE;
			fiber->twin->isInTheHeap = FALSE;
			Pop();
		}
	};

	struct CollisionMap
	{
		struct CollisionSector
		{
			const CollisionStruct *sector[COLLISION_MAP_SECTOR_SIZE];
		};

		float mapScale;
		ZeroVector4 origin;

		int collisionMapCount [COLLISION_MAP_SIZE][COLLISION_MAP_SIZE][COLLISION_MAP_SIZE];
		CollisionSector collisionMap [COLLISION_MAP_SIZE][COLLISION_MAP_SIZE][COLLISION_MAP_SIZE];

		void SetMap (const CollisionStruct collsionVolumes[], int volumesCount) 
		{
			int i;
			int sphereSteps;
			float tmp;

			return;
			memset (collisionMapCount, 0, sizeof (collisionMapCount));

			ZeroVector4 min (1.0e-10f, 1.0e-10f, 1.0e-10f);
			ZeroVector4 max (-1.0e-10f, -1.0e-10f, -1.0e-10f);
			for (i = 0; i < volumesCount; i ++) {

				tmp = collsionVolumes[i].center.x - collsionVolumes[i].radii0;
				if (tmp < min.x) {
					min.x	= tmp;
				}
				tmp = collsionVolumes[i].center.y - collsionVolumes[i].radii0;
				if (tmp < min.y) {
					min.y	= tmp;
				}
				tmp = collsionVolumes[i].center.z - collsionVolumes[i].radii0;
				if (tmp < min.z) {
					min.z	= tmp;
				}


				tmp = collsionVolumes[i].center.x + collsionVolumes[i].radii0;
				if (tmp > max.x) {
					max.x	= tmp;
				}
				tmp = collsionVolumes[i].center.y + collsionVolumes[i].radii0;
				if (tmp > max.y) {
					max.y	= tmp;
				}
				tmp = collsionVolumes[i].center.z + collsionVolumes[i].radii0;
				if (tmp > max.z) {
					max.z	= tmp;
				}
			}

			origin = min;
			ZeroVector4 stepVector (max - min);

			tmp = GetMax (stepVector.x, stepVector.y, stepVector.z) + 0.01f;

			mapScale	= COLLISION_MAP_SIZE / tmp;
			for (i = 0; i < volumesCount; i ++) {
				int x;
				int y;
				int z;
				int ix;
				int iy;
				int iz;
				float x0;
				float y0;
				float z0;


				sphereSteps = FloatToInt (floorf (2.0f * collsionVolumes[i].radii0 * mapScale));
				_ASSERTE (sphereSteps <= COLLISION_MAP_SIZE);

				z0 = collsionVolumes[i].center.z - collsionVolumes[i].radii0 - origin.z;
				iz = FloatToInt (floorf (z0 * mapScale));
				for (z = 0; z < sphereSteps; z ++) {
					y0 = collsionVolumes[i].center.y - collsionVolumes[i].radii0 - origin.y;
					iy = FloatToInt (floorf (y0 * mapScale));
					for (y = 0; y < sphereSteps; y ++) {
						x0 = collsionVolumes[i].center.x - collsionVolumes[i].radii0 - origin.x;
						ix = FloatToInt (floorf (x0 * mapScale));
						for (x = 0; x < sphereSteps; x ++) {
							int index;
							index = collisionMapCount[iz][iy][ix];
							if (index < COLLISION_MAP_SECTOR_SIZE) {
								collisionMap[iz][iy][ix].sector[index] = &collsionVolumes[i];
								collisionMapCount[iz][iy][ix]	= index + 1;
							}
							ix ++;
						}
						iy ++;
					}
					iz ++;
				}
			} 
		}


		int GetVolumeArray (const ZeroVector4& posit, const CollisionStruct** arrayOut) const
		{
			int ix;
			int iy;
			int iz;
			ZeroVector4 p ((posit - origin).Scale (mapScale));

			ix = FloatToInt (floorf (p.x));
			if (ix < 0) {
				return 0;
			}
			if (ix >= COLLISION_MAP_SIZE) {
				return 0;
			}

			iy = FloatToInt (floorf (p.y));
			if (iy < 0) {
				return 0;
			}

			if (iy >= COLLISION_MAP_SIZE) {
				return 0;
			}

			iz = FloatToInt (floorf (p.z));
			if (iz < 0) {
				return 0;
			}

			if (iz >= COLLISION_MAP_SIZE) {
				return 0;
			}

			*arrayOut = collisionMap[iz][iy][ix].sector[0];
			return collisionMapCount[iz][iy][ix];
		}
	};

	ZeroVector4 particleWeight;

	// independent values
	int particlesCount;
	unsigned char collisionMark;
	unsigned char* collisionMarkArray;

	float* particlesInvMass;
	ZeroVector4* particlesOriginalPosit;

	int faceCount;
	unsigned short *faces;

	int indexMapCount;
	int* indexMap;

	int edgeCount;
	FabricFiber *fabric;

	int bendPairsCount;
	BendPair *bendPairs;

	ZeroVertexRecord vertexRecord;


	CollisionMap* collisionMap;
	int collisionVolumesCount;
	CollisionStruct *originalCollision;
	ElasticStretchContraint	elasticContraint;


	// local variables
	// in ps2 place these variables on scratch pad
	union 
	{
		ZeroVector4* particleAccel;	 // use first to compute the particle acceleration
		ZeroVector4* particlePosit;	 // use after to compute tmp particle position
	};
	CollisionStruct *localCollision;

	bool SolveParicleCollision (ZeroVector4& p) ;
	CollisionMap* GetCollisionMap()
	{
		static CollisionMap map;
		return& map;
	}



	void SolveElasticContrains(const ZeroClothGeometry& me, float invDt)
	{
		int i;
		int i0;
		int i1;
		int iterationsCount;
		int maxiterationsCount;
		float kl;
		float error;
		float length;
		float length2;
		float invLength;
		ZeroVector4* posit;
		ZeroVector4* veloc;
		const FabricFiber* ptr;


		elasticContraint.UpMark();

		veloc = me.particleVeloc;
		posit = me.particlePosit;

		iterationsCount = 0;
		maxiterationsCount = edgeCount;
		while (elasticContraint.GetCount()) {
			const FabricFiber& fiber0 = *elasticContraint[0];
			const FabricFiber& fiber1 = *fiber0.twin; 

			elasticContraint.RemoveWorstFiber();

			iterationsCount ++;
			i0 = fiber0.particleIndex;
			i1 = fiber1.particleIndex;
			

			ZeroVector4 p1p0 (particlePosit[i1] - particlePosit[i0]);
			invLength = rsqrt (DotProduct (p1p0, p1p0));
			kl = 1.0f - fiber0.maxStretch * invLength;

			particlePosit[i0] += p1p0.Scale (kl * fiber0.invMassRatio);
			particlePosit[i1] -= p1p0.Scale (kl * fiber1.invMassRatio);

			if (particlesInvMass[i0] > 0.0f) {
				SolveParicleCollision (particlePosit[i0]); 
			}
			if (particlesInvMass[i1] > 0.0f) {
				SolveParicleCollision (particlePosit[i1]); 
			}

			for (ptr = fiber0.next->twin; ptr != &fiber0; ptr = ptr->next->twin) {
				if (!ptr->isInTheHeap) {
					i0 = ptr->particleIndex;
					i1 = ptr->twin->particleIndex;

					ZeroVector4 p1p0 (particlePosit[i1] - particlePosit[i0]);

					length2 = DotProduct (p1p0, p1p0);
					invLength = rsqrt (length2);
					length = length2 * invLength;
					error	= length - ptr->maxStretch;
					if (error > 1.0e-2f) {
						elasticContraint.AddFiber (ptr, error); 
					}																					         
				}
			}

			for (ptr = fiber1.next->twin; ptr != &fiber1; ptr = ptr->next->twin) {
				if (!ptr->isInTheHeap) {
					i0 = ptr->particleIndex;
					i1 = ptr->twin->particleIndex;

					ZeroVector4 p1p0 (particlePosit[i1] - particlePosit[i0]);

					length2 = DotProduct (p1p0, p1p0);
					invLength = rsqrt (length2);
					length = length2 * invLength;
					error	= length - ptr->maxStretch;
					if (error > 1.0e-2f) {
						elasticContraint.AddFiber (ptr, error); 
					}																					         
				}
			}

			if (iterationsCount > maxiterationsCount) {
				maxiterationsCount = elasticContraint.GetCount();
				for (i = 0; i < maxiterationsCount; i ++) {
					const FabricFiber& fiber = *elasticContraint[i];
					fiber.isInTheHeap = FALSE;
					fiber.twin->isInTheHeap = FALSE;
				}
				elasticContraint.Flush();
			}
		}

		for (i = 0; i < particlesCount; i ++) {
			veloc[i]	= (particlePosit[i] - posit[i]).Scale (invDt);
			posit[i]	= particlePosit[i];
		}
	}


	void ComputeNormals(const ZeroClothGeometry& me) const
	{
		int i;
		int ind0;
		int ind1;
		int ind2;
		float length;
		ZeroVector4* posit;
		ZeroVector4* normal;
 	
		posit = me.particlePosit;
		normal = me.particleNormal;

		memset (normal, 0, sizeof (ZeroVector4) * particlesCount);
		for(i = 0 ; i < faceCount; i++) {
			ind0 = faces[i * 3 + 0];
			ind1 = faces[i * 3 + 1];
			ind2 = faces[i * 3 + 2];

			ZeroVector3& p0 = posit[ind0];
			ZeroVector3& p1 = posit[ind1];
			ZeroVector3& p2 = posit[ind2];
 
			ZeroVector3 n (CrossProduct(p1 - p0, p2 - p0));
			normal[ind0] += n;
			normal[ind1] += n;
			normal[ind2] += n;
		}
	
		for(i = 0; i < particlesCount; i++ ) {
			length = DotProduct (normal[i], normal[i]) + 1.0e-8f; 
			normal[i] = normal[i].Scale (rsqrt (length));
		}
	}

		  

	void ApplyExternalForces (
		const ZeroClothGeometry& me, 
		const ZeroMatrix& worldMatrix, 
		float dtInv)
	{
		int i;
		float mag2;
		float airViscosity;
		ZeroVector4* veloc;
		ZeroVector4* normal;


		ZeroVector4 frameWorldVelocity ((worldMatrix.posit	- me.prevPosit).Scale (dtInv * BODY_SPEED_VISCUOS_DAMPING));
		mag2 = DotProduct (frameWorldVelocity, frameWorldVelocity); 
		if (mag2	> (MAX_SPEED_CHANGE * MAX_SPEED_CHANGE)) {
			frameWorldVelocity = frameWorldVelocity.Scale (MAX_SPEED_CHANGE * rsqrt (mag2));
		}

//ZeroTrace_x (("%f %f %f\n", frameWorldVelocity.x, frameWorldVelocity.y, frameWorldVelocity.z));

		ZeroVector4 localWindVeloc (worldMatrix.UnrotateVector (me.windVeloc - frameWorldVelocity));
		ZeroVector4 localParticleWeight (worldMatrix.UnrotateVector (particleWeight));

		veloc = me.particleVeloc;
		normal = me.particleNormal;


		for (i = 0; i < particlesCount; i ++) {
			airViscosity = AIR_VISCUOS_DAMPING * DotProduct ((localWindVeloc - veloc[i]), normal[i]);
			particleAccel[i] = (localParticleWeight + normal[i].Scale (airViscosity)).Scale (particlesInvMass[i]);
		}

/*		float airDamping = AIR_VISCUOS_DAMPING;
		int endPtr = ((int)particleAccel) + ((particlesCount-1)*16);
		
		__asm__ volatile
		(
		"
			.set noreorder
			
			lqc2			vf03, 0(%2)
			lqc2			vf06, 0(%3)
			lwc1			$f5 ,0(%5)
			mfc1			$5 , $f5
			qmtc2		   $5 , vf08

NextParticle:
			lqc2			vf01, 0(%0)
						
			vsub.xyz		vf04  , vf03 , vf01
			lqc2			vf02, 0(%1)
			vmul.xyz		vf05  , vf02 , vf04
			lwc1			$f4 ,0(%4)
			addi			%0,%0,16
			addi			%1,%1,16
			vaddy.x		vf05  , vf05 , vf05y
			mfc1			$4 , $f4
			qmtc2		   $4 , vf07
			addi			%4,%4,16
			vaddz.x		vf05  , vf05 , vf05z
			vmulx.xyz	vf12,vf06,vf07x
			vmulx.xyz	vf09,vf02,vf07x
			vmulx.x		vf13,vf05,vf08x
			vmulx.xyz	vf10,vf09,vf13x
			vadd.xyz		vf11,vf10,vf12

		

			sqc2			vf11 , 0(%6)
			bne			%6,%7,NextParticle
			addi			%6,%6,16
			
			.set reorder

		": 
		: "r" ( &(veloc[0]) ), "r" ( &(normal[0]) ),"r" ( &(localWindVeloc) ),"r" ( &(localParticleWeight) )
			,"r" (&(particlesInvMass[0])),"r" (&airDamping),"r" ( &(particleAccel[0]) ),"r" (endPtr)
		: "$4","$5","$f5","$f4"
		);*/
	}

	void ApplyInternalForces (const ZeroClothGeometry& me)
	{
		int i;
		int i0;
		int i1;
		float ks;
		float kd;
		float length;
		float invLength;
		float length2;
		float bend_Kdamp;
		float bend_KSpring;
		float stretch_Kdamp;
		float stretch_KSpring;

		ZeroVector4* posit;
		ZeroVector4* veloc;

		posit = me.particlePosit;
		veloc = me.particleVeloc;

		// calculate bend forces
		bend_KSpring = ((PI2 * FABRIC_BEND_FREQ) * (PI2 * FABRIC_BEND_FREQ));
		bend_Kdamp = sqrtf (FABRIC_BEND_OVERDAMPING + 4.0f * bend_KSpring);
		for (i = 0; i < bendPairsCount; i ++) {
			const BendPair& tread = bendPairs[i];

			i0 = tread.v0;
			i1 = tread.v1;

			ZeroVector4 p1p0 (posit[i1] - posit[i0]);
			ZeroVector4 v1v0 (veloc[i1] - veloc[i0]);

			length2 = DotProduct (p1p0, p1p0);
			_ASSERTE (length2 > 1.0e-10f);
			invLength = rsqrt (length2);

			ks = bend_KSpring * (1.0f - tread.restLength * invLength);
			kd	= bend_Kdamp * DotProduct (v1v0, p1p0) * invLength * invLength;

			particleAccel[i0] += p1p0.Scale ((ks + kd) * tread.invMassRatio0);
			particleAccel[i1] -= p1p0.Scale ((ks + kd) * tread.invMassRatio1);
		}


		// calculate stretch	forces
		stretch_KSpring = ((PI2 * THREAD_ELASTIC_FREQ) * (PI2 * THREAD_ELASTIC_FREQ));
		stretch_Kdamp = sqrtf (THREAD_ELASTIC_OVERDAMPING + 4.0f * stretch_KSpring);
		for (i = 0; i < edgeCount; i += 2) {
			const FabricFiber& tread0 = fabric[i + 0];
			const FabricFiber& tread1 = fabric[i + 1];

			i0 = tread0.particleIndex;
			i1 = tread1.particleIndex;

			ZeroVector4 p1p0 (posit[i1] - posit[i0]);
			ZeroVector4 v1v0 (veloc[i1] - veloc[i0]);

			length2 = DotProduct (p1p0, p1p0);
			_ASSERTE (length2 > 1.0e-10f);
			invLength = rsqrt (length2);

			length = length2 * invLength;
			if (length > tread0.maxStretch) {
				elasticContraint.AddFiber (&tread0, length - tread0.maxStretch); 
			}																					         

			ks = stretch_KSpring * (1.0f - tread0.restLength * invLength);
			kd	= stretch_Kdamp * DotProduct (v1v0, p1p0) * invLength * invLength;

			particleAccel[i0] += p1p0.Scale ((ks + kd) * tread0.invMassRatio);
			particleAccel[i1] -= p1p0.Scale ((ks + kd) * tread1.invMassRatio);
		}
	}


	void ComputeParticlesPositions (
		const ZeroClothGeometry& me, 
		const ZeroMatrix& worldMatrix,
		float dt)
	{
		int i;
		float halfDt;
		ZeroVector4* posit;
		ZeroVector4* veloc;
		ZeroTransformContraintGeometry *collisionVolume;
		ZeroTransformContraintGeometry **collisionVolumeArray;


		// update the collsion volumes
		collisionVolumeArray = me.collisionContraint;
		for (i = 0; i < collisionVolumesCount; i ++) {
			collisionVolume = collisionVolumeArray[i];

			const ZeroMatrix& matrix = collisionVolume->GetWorldMatrix();

			localCollision[i].center = worldMatrix.UntransformVector(matrix.TransformVector(originalCollision[i].center)); 
		}

#ifdef USE_COLLISION_MAP
		collisionMap->SetMap (localCollision, collisionVolumesCount);
#endif

		// update particles position
		halfDt = dt * 0.5f;
		posit = me.particlePosit;
		veloc = me.particleVeloc;

		collisionMark ++; 
		for (i = 0; i < particlesCount; i ++) {
			ZeroVector4 v (particleAccel[i].Scale (halfDt) + veloc[i]);
			ZeroVector4 p (posit[i] + v.Scale (dt));
			if (particlesInvMass[i] > 0.0f) {
				if (SolveParicleCollision (p)) {
					collisionMarkArray[i] = collisionMark; 
				}
			}
			particlePosit[i] = p;
		}


		for (i = 0; i < edgeCount; i += 2) {
			int i0;
			int i1;
			float length;
			float length2;
			float invLength;

			const FabricFiber& tread0 = fabric[i + 0];
			const FabricFiber& tread1 = fabric[i + 1];
			if (!tread0.isInTheHeap) {
				i0 = tread0.particleIndex;
				i1 = tread1.particleIndex;
				if ((collisionMarkArray[i0] == collisionMark) || (collisionMarkArray[i1] == collisionMark)) {   
					ZeroVector4 p1p0 (particlePosit[i1] - particlePosit[i0]);
					length2 = DotProduct (p1p0, p1p0);
					_ASSERTE (length2 > 1.0e-10f);
					invLength = rsqrt (length2);
		
					length = length2 * invLength;
					if (length > tread0.maxStretch) {
						elasticContraint.AddFiber (&tread0, length - tread0.maxStretch); 
					}																					         
				}
			}

			tread0.cyclingCount = elasticContraint.cycleCount;
			tread1.cyclingCount = elasticContraint.cycleCount;
		}

	}




	public: 
	ZeroClothPhysics (const ZeroGeometryData &data)
		:ZeroUnknown(), 
		 particleWeight (0.0f, -CLOTH_GRAVITY, 0.0f), 
		 elasticContraint(data.clothData->edgeCount)
	{
		int i;
		const ZeroClothData &clothData = *data.clothData;

		// copy particles
		particlesCount	= clothData.particlesCount;

		collisionMark = 0;;
		collisionMarkArray = ZeroNew (unsigned char[particlesCount]);
		particlesOriginalPosit = ZeroNew (ZeroVector4[particlesCount]);
		for (i = 0; i < particlesCount; i ++) {
			collisionMarkArray[i] = 0;
			particlesOriginalPosit[i] = clothData.particlesPosit[i];
			particlesOriginalPosit[i].w = 0.0f; 
		}



		// copy faceArray
		faceCount = clothData.indexTriangleCount;
		faces = ZeroNew (unsigned short [faceCount * 3]);
		for (i = 0; i < faceCount; i ++) {
			faces[i * 3 + 0] = (unsigned short) (clothData.indexTriangleList[i * 3 + 0]);
			faces[i * 3 + 1] = (unsigned short) (clothData.indexTriangleList[i * 3 + 1]);
			faces[i * 3 + 2] = (unsigned short) (clothData.indexTriangleList[i * 3 + 2]);
		}

		// copy indexMap
		indexMapCount = clothData.particlesIndexMapCount;
		indexMap = ZeroNew (int[indexMapCount]);
		for (i = 0; i < indexMapCount; i ++) {
			indexMap[i] = clothData.particlesIndexMap[i];
		}

		// copy cloth texture (uvArray);
		memset (&vertexRecord, 0, sizeof (ZeroVertexRecord));
		vertexRecord.size = 2 * sizeof (ZeroVector4) / sizeof (float);
		vertexRecord.format = ZERO_FORMAT_NORMAL;

		vertexRecord.position.ptr = NULL;
		vertexRecord.position.size = sizeof (ZeroVector4) / sizeof(float);
		vertexRecord.position.stride = sizeof (ZeroVector4) / sizeof(float);

		vertexRecord.normal.ptr = NULL;
		vertexRecord.normal.size = sizeof (ZeroVector4) / sizeof(float);
		vertexRecord.normal.stride = sizeof (ZeroVector4) / sizeof(float);

		// copy physics values
		particlesInvMass = ZeroNew (float[particlesCount]);
		for (i = 0; i < particlesCount; i ++) {
			particlesInvMass[i] = 1.0f;
		}

		// nailed pairticles has infinite mass 
		for (i = 0; i < clothData.nailedPointsCount; i ++) {
			particlesInvMass[clothData.nailedPoints[i]] = 0.0f;
		}


		// copy bend edges
		bendPairsCount	= 0;
		bendPairs = NULL;
		if (bendPairsCount) {
			_ASSERTE (clothData.bendPairs);
			bendPairsCount	= clothData.bendPairsCount;
			bendPairs = ZeroNew (BendPair [bendPairsCount]);
			for (i = 0; i < bendPairsCount; i ++) {
				int i0;
				int i1;
				float invMass0;
				float invMass1;
			
				i0 = clothData.bendPairs[i].v0;
				i1 = clothData.bendPairs[i].v1;
			
				invMass0	= particlesInvMass[i0];
				invMass1	= particlesInvMass[i1];
			
				ZeroVector3 dist (particlesOriginalPosit[i1] - particlesOriginalPosit[i0]);
			
				bendPairs[i].v0 = (unsigned short) i0;
				bendPairs[i].v1 = (unsigned short) i1;
				bendPairs[i].restLength = sqrtf (DotProduct (dist, dist));
				bendPairs[i].invMassRatio0 = invMass0 / (invMass0 + invMass1);
				bendPairs[i].invMassRatio1 = invMass1 / (invMass0 + invMass1);
			}
		}


		// copy fabric structure
		edgeCount = clothData.edgeCount;
		fabric = ZeroNew (FabricFiber[edgeCount]);
		memset (fabric, 0, sizeof (FabricFiber) * edgeCount);
		for (i = 0; i < edgeCount; i ++) {
			fabric[i].particleIndex	= clothData.clothVertexEdge[i].particleIndex;
			fabric[i].next	= &fabric[clothData.clothVertexEdge[i].next];
			fabric[i].twin	= &fabric[clothData.clothVertexEdge[i].twin];
		}

		// calculate fabric treads
		for (i = 0; i < edgeCount; i += 2) {
			int i0;
			int i1;
			float invMass0;
			float invMass1;
			FabricFiber	*edge;

			edge = &fabric[i];
			_ASSERTE (edge->twin->twin == &fabric[i]);

		 	i0	= edge->particleIndex;
		 	i1	= edge->twin->particleIndex;

			ZeroVector3 dist (particlesOriginalPosit[i1] - particlesOriginalPosit[i0]);

			edge->restLength = sqrtf (DotProduct (dist, dist));
			edge->twin->restLength = edge->restLength;
			edge->maxStretch = edge->restLength * (MAX_STRETCH_RATE + 1.0f);
			edge->twin->maxStretch = edge->maxStretch;

			invMass0	= particlesInvMass[i0];
			invMass1	= particlesInvMass[i1];
			edge->cyclingCount = 0;
			edge->isInTheHeap	= FALSE;
			edge->invMassRatio = invMass0 / (invMass0 + invMass1);
			edge->twin->invMassRatio = invMass1 / (invMass0 + invMass1);
		}


		// create array for prodedural normals
#define SP_SIZE (16*1024)
#if (PLATFORM == PLATFORM_PS2)
		
		particleAccel = (ZeroVector4*) 0x70000000;
		char* endSPad = ((char*) particleAccel) + (particlesCount * sizeof(ZeroVector4));
		if( endSPad >= (char*)(0x70000000 + SP_SIZE)  )
			printf("warning :position data too big for the SP \n");

#else
		particlePosit = ZeroNew (ZeroVector4[particlesCount]);
#endif


		collisionVolumesCount = 0;
		localCollision = NULL;
		originalCollision = NULL;
		if (clothData.collisionCount) {
			collisionVolumesCount = clothData.collisionCount;
#if (PLATFORM == PLATFORM_PS2)
			localCollision = (CollisionStruct*) endSPad;
			endSPad = ((char*) particleAccel) + (collisionVolumesCount * sizeof(CollisionStruct));
			if( endSPad >= (char*)(0x70000000 + SP_SIZE)  )
				printf("warning :collision data too big for the SP \n");

#else
			localCollision = ZeroNew (CollisionStruct[collisionVolumesCount]);
#endif
			originalCollision = ZeroNew (CollisionStruct[collisionVolumesCount]);

			ZeroVector4 dir (0.0f, 1.0f, 0.0f);
			for (i = 0; i < collisionVolumesCount ; i ++) {
				localCollision[i].radii0 = clothData.collisionVolumes[i].raddi0 ;
				localCollision[i].radiiSquare = localCollision[i].radii0 * localCollision[i].radii0;
				
				ZeroMatrix offsetMatrix;

				for(int ii = 0; ii < 4; ii++)	{
					for(int jj = 0; jj < 4; jj++)	{
						offsetMatrix[ii][jj] = clothData.collisionVolumes[i].offsetMatrix[ii][jj];
					}
				}

				localCollision[i].parentIndex	= clothData.collisionVolumes[i].parentIndex;
				localCollision[i].center = offsetMatrix.posit;
				originalCollision[i] =  localCollision[i];
			}
		}

		collisionMap = GetCollisionMap();
	}

	~ZeroClothPhysics ()
	{
		ZeroFree (faces);
		ZeroFree (fabric);
		ZeroFree (indexMap);
		if (bendPairs) {
			ZeroFree (bendPairs);
		}
#if (PLATFORM != PLATFORM_PS2)
		ZeroFree (particlePosit);
#endif
		ZeroFree (particlesInvMass);
		ZeroFree (collisionMarkArray); 
		ZeroFree (particlesOriginalPosit);

		if (localCollision) {
#if (PLATFORM != PLATFORM_PS2)
			ZeroFree (localCollision);
#endif
			ZeroFree (originalCollision);
		}
	}

	void Update (
		ZeroCamera *camera, 
		ZeroClothGeometry& me, 
		const ZeroMatrix& worldMatrix, 
		float dt)
	{
		float dtInv;
		ZeroSegment *segment;
		


		dtInv = 1.0f / dt;

		ApplyExternalForces (me, worldMatrix, dtInv);
		ApplyInternalForces (me);
		ComputeParticlesPositions (me, worldMatrix, dt);

		SolveElasticContrains(me, dtInv);
		ComputeNormals(me);

		segment = me.GetSegments()->GetFirst()->info;

		vertexRecord.position.ptr = &me.particlePosit[0].x;
		vertexRecord.normal.ptr = &me.particleNormal[0].x;

		segment->UpdateProceduralVertexData (camera, vertexRecord, indexMap, indexMapCount, NULL, 0);
	}


	void RestorePosition (ZeroClothGeometry& me)
	{
		memcpy (me.particlePosit, particlesOriginalPosit, sizeof (ZeroVector4) * particlesCount);
		ComputeNormals(me);
	}


	void Scale (float scale)
	{
		int i;

		for (i = 0; i < particlesCount; i ++) {
			particlesOriginalPosit[i] = particlesOriginalPosit[i].Scale (scale);
		}

		for (i = 0; i < edgeCount; i ++) {
			fabric[i].restLength *= scale;
			fabric[i].maxStretch *= scale;
		}

		for (i = 0; i < bendPairsCount; i ++) {
			bendPairs[i].restLength *= scale;
		}

		for (i = 0; i < collisionVolumesCount ; i ++) {
			localCollision[i].radii0 *= scale;
			localCollision[i].radiiSquare *= scale * scale;
			localCollision[i].center *= scale;

			originalCollision[i].radii0 *= scale;
			originalCollision[i].center *= scale;
			originalCollision[i].radiiSquare *= (scale * scale);
		}

	}
};



#if (PLATFORM != PLATFORM_PS2)
bool ZeroClothPhysics::SolveParicleCollision (ZeroVector4& p) 
	{
		int i;
		float mag2;
		float scale;
		bool retValue;

		retValue = false;

#ifdef USE_COLLISION_MAP
		int volumesCount;
		const CollisionStruct* volumeArray;
		volumesCount = collisionMap->GetVolumeArray (p, &volumeArray);
		for (i = 0; i < volumesCount; i ++) {
			const CollisionStruct &volume = volumeArray[i];
#else
		for (i = 0; i < collisionVolumesCount; i ++) {
			CollisionStruct &volume = localCollision[i];
#endif

  			ZeroVector4 dp (p - volume.center);
  			mag2 = DotProduct (dp, dp);
  			if (mag2 < volume.radiiSquare) {
  				retValue	= true;
  				scale = (volume.radii0 ) * rsqrt (mag2);
  				p = volume.center + dp.Scale (scale);
  				retValue = true;
			}
		}
		return retValue;
	}
#else


bool ZeroClothPhysics::SolveParicleCollision(ZeroVector4& p) 
{
	register int retValue;
	retValue = 0;
	int i;
	//return false;
	//vf4->position
	//vf5->volume center
	//vf6.x->volume radius
	//vf6.y->volume radius square
	for (i = 0; i < collisionVolumesCount; i ++) {
	
		CollisionStruct &volume = localCollision[i];
		__asm__ volatile
		(
		"
			.set noreorder
			
			lqc2			vf4, 0(%1)
			lqc2			vf5, 0(%2)
			
			lwc1			$f14 ,0(%3)
			lwc1			$f15 ,0(%4)

			vsub			vf7  , vf4 , vf5

			vmul.xyz		vf8  , vf7 , vf7
			vaddy.x		vf8  , vf8 , vf8y
			vaddz.x		vf8  , vf8 , vf8z
			
			qmfc2			$17 , vf8
			mtc1			$17 , $f17
			
			c.lt.s		$f17 , $f15
			bc1f			fin
			nop
			rsqrt.s		$f16	, $f14 , $f17

			ori			%0 , %0 , 1
			mfc1			$16 , $f16
			qmtc2		   $16 , vf6

			vmulax		ACC , vf7 , vf6x	
			vmaddw		vf9 , vf5 , vf0w
			
			sqc2			vf9 , 0(%1)
			fin:

			.set reorder

		": "=&r" ( retValue )
		: "r" ( &p ), "r" ( &(volume.center) ),"r" ( &(volume.radii0) ),"r" ( &(volume.radiiSquare) )
		: "$12","$13","$16","$17","$f14","$f15","$f16","$f17"
		);
	}

/*	ZeroVector4 dp (v2 - volume.center);
	mag2 = DotProduct (dp, dp);
	if (mag2 < (volume.radiiSquare)) {
		retValue	= true;
		scale = (volume.radii0) * rsqrt (mag2);
		v2 = volume.center + dp.Scale (scale);
	//	return retValue;
		retValue = true;
	}*/

	return retValue;

}
#endif

ZeroClothGeometry::ZeroClothGeometry(const ZeroGeometryData& data)
	:ZeroMeshGeometry(data),
	 prevPosit(0.0f, 0.0f, 0.0f),
	 windVeloc (0.0f, 0.0f, 0.0f)
{
	physics = new (false) ZeroClothPhysics (data);

	particlePosit = NULL;
	particleVeloc = NULL;
	particleNormal = NULL;


	collisionCount = 0;
	collisionContraint = NULL;

	if (data.clothData->collisionCount) {
		collisionCount = data.clothData->collisionCount;
		collisionContraint  = ZeroNew (ZeroTransformContraintGeometry*[collisionCount]);
	}

	ZeroSegmentList::Iterator iter(segments);
	for (iter.Begin(); iter; iter ++) {
		ZeroSegment* segment = *iter;
		segment->FixupData(this);
	}
}



ZeroClothGeometry::ZeroClothGeometry(const ZeroClothGeometry &clone)
	:ZeroMeshGeometry(clone),
	 prevPosit (clone.prevPosit),
	 windVeloc (clone.windVeloc)
{
	ZeroSegment *staticSegment;
	ZeroSegment *proceduralSegment;

	physics = clone.physics;
	physics->AddRef();


	particlePosit = ZeroNew (ZeroVector4[physics->particlesCount]);
	particleVeloc = ZeroNew (ZeroVector4[physics->particlesCount]);
	particleNormal = ZeroNew (ZeroVector4[physics->particlesCount]);

	memset (particlePosit, 0, sizeof (ZeroVector4) * physics->particlesCount);
	memset (particleVeloc, 0, sizeof (ZeroVector4) * physics->particlesCount);


	//replace static segment with dynamic segment
	staticSegment = GetSegments()->GetFirst()->info;

	proceduralSegment = staticSegment->CreateProceduralSegment();

	proceduralSegment->GetMaterial()->AttachRenderStates(renderStatesFace);

	ZeroSegmentList::Iterator iter (*GetSegments());
	for (iter.Begin(); iter; iter ++) {
		(*iter)->Release();
	}
	GetSegments()->RemoveAll();
	AddSegment(proceduralSegment);
	proceduralSegment->Release();

	collisionContraint = NULL;
	collisionCount	= clone.collisionCount;
	if (collisionCount) {
		collisionContraint = ZeroNew (ZeroTransformContraintGeometry*[collisionCount]);
	}
}


ZeroClothGeometry::~ZeroClothGeometry()
{
	if (collisionContraint) {
		ZeroFree (collisionContraint);
	}

	physics->Release();

	if (particleNormal) {
		ZeroFree (particleNormal);
	}

	if (particlePosit) {
		ZeroFree (particlePosit);
	}

	if (particleVeloc) {
		ZeroFree (particleVeloc);
	}
}


ZeroUnknown *ZeroClothGeometry::CreateClone(bool isSim)
{
	return new (isSim) ZeroClothGeometry (*this);
}


void ZeroClothGeometry::CloneFixUp (const ZeroObject& myOwner, const ZeroObject& myClone)
{
	int i;
	int j;
	int myCount;
	int cloneCount;
	ZeroTransformContraintGeometry **cloneCollisionContraintArray;
	
	ZeroMeshGeometry::CloneFixUp (myOwner, myClone);

	ZeroStack<const ZeroObject*> myConstraints(2048);
	ZeroStack<const ZeroObject*> cloneConstraints(2048);

	myCount = myOwner.EnumerateByTypeID (ZeroObject::GetRttiTypeID(), &myConstraints[0]);
	cloneCount = myClone.EnumerateByTypeID (ZeroObject::GetRttiTypeID(), &cloneConstraints[0]);
	_ASSERTE (myCount	== cloneCount);


	const ZeroClothGeometry& myCloneGeo = *((ZeroClothGeometry*) myClone.GetGeometry());
	_ASSERTE (myCloneGeo.IsTypeID (ZeroClothGeometry::GetRttiTypeID()));

	cloneCollisionContraintArray = myCloneGeo.collisionContraint ;

	for (i = 0; i < collisionCount; i ++) {
		for (j = 0; j < cloneCount; j ++) {
			const ZeroObject *obj; 
			obj = cloneConstraints[j];
			if (obj->GetGeometry() == cloneCollisionContraintArray[i]) {
				obj = myConstraints[j];
				collisionContraint[i] = (ZeroTransformContraintGeometry *) obj->GetGeometry();
				_ASSERTE (collisionContraint[i]->IsTypeID (ZeroTransformContraintGeometry::GetRttiTypeID()));
				break;
			}
		}
	}

	physics->RestorePosition (*this);
}


void ZeroClothGeometry::Scale (float scale)
{
   ZeroMeshGeometry::Scale (scale); 
	physics->Scale (scale);
}


void ZeroClothGeometry::RemapCollisions(ZeroObject *cloth)
{
	int i;
	int objCount;
	int constraintID;
	ZeroObject *constraint;
	ZeroGeometry *geometry;
	ZeroClothPhysics::CollisionStruct *initArray;
	ZeroStack<const ZeroObject*> constraints(2048);



	if (!collisionContraint) {
		return;
	}

	objCount	= cloth->EnumerateByTypeID (ZeroObject::GetRttiTypeID(), &constraints[0]);
  

	initArray = physics->originalCollision;
	for (i = 0; i < collisionCount; i ++) {
		constraintID = initArray[i].parentIndex;

		constraint = (ZeroObject *)constraints[constraintID];
		geometry	= constraint->GetGeometry();

		if (geometry) {
			if (!geometry->IsTypeID (ZeroTransformContraintGeometry::GetRttiTypeID())) {
				ZeroGeometryData data;
				data.dataGeometry = geometry;
				geometry = new (false) ZeroTransformContraintGeometry (data);
				constraint->AttachGeometry (geometry);
				geometry->Release();
			}
		} else {
			geometry = new (false) ZeroTransformContraintGeometry;

			constraint->AttachGeometry (geometry);
			geometry->Release();
		}

		collisionContraint[i] = (ZeroTransformContraintGeometry*) geometry;
	}
}


int* ZeroClothGeometry::GetIndexMap()
{
	return physics->indexMap;
}


void ZeroClothGeometry::Render(
		const ZeroObject *owner, 
		ZeroCamera *camera, 
		const ZeroMatrix& worldMatrix, 
		unsigned flags) const
{

	int clipFlag;
	clipFlag = TrivialRejection (camera, worldMatrix, flags);
	if (clipFlag < 0) {
		return;
	}

	ZeroClothGeometry& me = (ZeroClothGeometry&) *this;

	physics->Update (camera, me, worldMatrix, 1.0f / 60.0f);

	ZeroMeshGeometry::Render (owner, camera, worldMatrix, clipFlag);

	prevPosit = worldMatrix.posit;
}











