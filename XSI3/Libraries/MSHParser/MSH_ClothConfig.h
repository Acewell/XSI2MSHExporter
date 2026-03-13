#ifndef MSH_CLOTHCONFIG_H
#define MSH_CLOTHCONFIG_H

/************************************* Deprecated Cloth Structures ************************************/
#define COLLISION_SPHERE	0
#define COLLISION_CYLINDER	1
#define COLLISION_CONE		2

struct MSH_ClothConfig
{
	short bClothEnabled;
	float Mass;
	float Ks_Stretch;
	short bBend;
	float Ks_Bend;
	short bShear;
	float Ks_Shear;
	int nCorrections;
	short bWind;
	short bAirResist;
	short bCollide;
	short bSpring;
};


struct MSH_ClothCollisionConfig
{
	short bCollisionEnabled;
	short sType;
};

struct MSH_ClothCollisionSphere
{
	float center_x,center_y,center_z;
	float radius;
};

struct MSH_ClothCollisionCylinder
{
	float top_x, top_y, top_z;
	float dir_x, dir_y, dir_z;
	float radius;
	float height;
};

struct MSH_ClothCollisionCone
{
	float top_x, top_y, top_z;
	float dir_x, dir_y, dir_z;
	float radius1;
	float radius2;
	float height;
};

struct MSH_AnimTriggerCollisionConfig
{
	MSH_ClothCollisionConfig config;
	int iLateStartFrame;
	float fTime;
	int iFrameMin;
	int iFrameMax;
	float fToleranceMin;
	float fToleranceMax;
	float fRecoveryTime;
};

/***********************************************************************************************************/

enum ClothCollisionType
{
	CLOTH_COLLISION_SPHERE = 0,
	CLOTH_COLLISION_CYLINDER,
	CLOTH_COLLISION_BOX,
	CLOTH_COLLISION_FORCE_DWORD = 0x7FFFFFFF,
};

struct ClothCollision {
	unsigned int nameCRC;
	unsigned int parentCRC;
	ClothCollisionType type;
	float width;
	float height;
	float depth;
};

#endif