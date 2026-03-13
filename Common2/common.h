#include "MSH_ClothConfig.h"

class ZeroSphere;

void GetBoundingBox(const SAA_Scene	currentScene, SAA_Elem	*model, ZeroSphere &sph);
void GetCollisionSphere(const SAA_Scene	currentScene, SAA_Elem	*model, MSH_ClothCollisionSphere &sphereData);
void GetCollisionCylinder(const SAA_Scene	currentScene, SAA_Elem	*model, MSH_ClothCollisionCylinder &cylinderData);
void GetCollisionCone(const SAA_Scene	currentScene, SAA_Elem	*model, MSH_ClothCollisionCone &coneData);
void CalculateCollisionSphere(const SAA_Scene	currentScene, SAA_Elem	*model, const SAA_CustomContext context, MSH_ClothCollisionSphere &sphereData, int textViewID);
void CalculateCollisionCylinder(const SAA_Scene	currentScene, SAA_Elem	*model, const SAA_CustomContext context, MSH_ClothCollisionCylinder &cylinderData, int textViewID);
void CalculateCollisionCone(const SAA_Scene	currentScene, SAA_Elem	*model, const SAA_CustomContext context, MSH_ClothCollisionCone &coneData, int textViewID);
void CalculateCollisionParameters(const SAA_Scene	currentScene, SAA_Elem	*model, const SAA_CustomContext context, const MSH_ClothCollisionConfig collData, MSH_ClothCollisionSphere &sphereData, MSH_ClothCollisionCylinder &cylinderData, MSH_ClothCollisionCone &coneData, int textViewID);
