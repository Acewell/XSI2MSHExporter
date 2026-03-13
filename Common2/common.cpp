#include <SAA.h>
#include "Common.h"
#include "ZeroColObject.h"
#include "ZeroPolyhedra.h"
#include "GCMacros.h"

void GetBoundingBox(const SAA_Scene	currentScene, SAA_Elem	*model, ZeroSphere &sph)
{

	int nbFaces;
	SAA_meshGetNbPolygons(&currentScene,model,&nbFaces);

	if(nbFaces)
	{

		SAA_SubElem *polygons = (SAA_SubElem *)malloc(nbFaces*sizeof(SAA_SubElem));
		SAA_meshGetPolygons(&currentScene,model,SAA_GEOM_ORIGINAL,0,nbFaces,polygons);

		int *nbPolyVertices = (int *)malloc(nbFaces*sizeof(int));
		SAA_polygonGetNbCtrlVertices(&currentScene, model, nbFaces, polygons, nbPolyVertices);

		int totalVertexCount=0;
		int i;
		for(i=0;i<nbFaces;i++)
			totalVertexCount+=nbPolyVertices[i];

		SAA_SubElem *ctrlVertices = (SAA_SubElem *)malloc(totalVertexCount*sizeof(SAA_SubElem));
		SAA_DVector *positions = (SAA_DVector *)malloc(totalVertexCount*sizeof(SAA_DVector));

		SAA_polygonGetCtrlVertices(&currentScene, model, nbFaces, polygons, nbPolyVertices, ctrlVertices);
		SAA_ctrlVertexGetPositions(&currentScene, model, totalVertexCount, ctrlVertices, positions);

		float *pVertex = new float[3*totalVertexCount];
		float *ptr=pVertex;
		for(i=0;i<totalVertexCount;i++)
		{
			ptr[0]=(float)positions[i].x;
			ptr[1]=(float)positions[i].y;
			ptr[2]=(float)positions[i].z;
			ptr+=3;
		}

		ZeroPolyhedra polyMsh;
		polyMsh.BeginFace();
		i=0;
		for(int j=0;j<nbFaces;j++)
		{
			int *newIndices = (int *)malloc(sizeof(int)*nbPolyVertices[j]);
			for(int k=0;k<nbPolyVertices[j];k++)
				newIndices[k] = i+k;
			polyMsh.AddFace(nbPolyVertices[j],newIndices);
			i+=nbPolyVertices[j];
			_SAA_FREE(&newIndices);
		}
		polyMsh.EndFace();
		polyMsh.Triangulate(pVertex,3*sizeof(float));

		sph = polyMsh.CalculateSphere(pVertex,3*sizeof(float));

		delete[] pVertex;
		_SAA_FREE(&positions);
		_SAA_FREE(&ctrlVertices);
		_SAA_FREE(&nbPolyVertices);
		_SAA_FREE(&polygons);
	}
}

void GetCollisionSphere(const SAA_Scene	currentScene, SAA_Elem	*model, MSH_ClothCollisionSphere &sphereData)
{
	ZeroSphere sph;
	GetBoundingBox(currentScene, model, sph);

	sphereData.center_x=sph.posit.x;
	sphereData.center_y=sph.posit.y;
	sphereData.center_z=sph.posit.z;
	sphereData.radius=sph.size.x;
	if(sph.size.y>sphereData.radius)
		sphereData.radius=sph.size.y;
	if(sph.size.z>sphereData.radius)
		sphereData.radius=sph.size.z;

}

void GetCollisionCylinder(const SAA_Scene	currentScene, SAA_Elem	*model, MSH_ClothCollisionCylinder &cylinderData)
{
	ZeroSphere bSphere;
	GetBoundingBox(currentScene, model, bSphere);
	float xz,xy,yz;

	ZeroVector4 center = ZeroVector4(bSphere.posit);
	xy=fabsf(bSphere.size.x-bSphere.size.y);
	xz=fabsf(bSphere.size.x-bSphere.size.z);
	yz=fabsf(bSphere.size.y-bSphere.size.z);
	if((xy<xz) && (xy<yz))
	{
		cylinderData.height = 2*(bSphere.size.z);
		cylinderData.radius = bSphere.size.x;
		cylinderData.dir_x=bSphere.front.x;
		cylinderData.dir_y=bSphere.front.y;
		cylinderData.dir_z=bSphere.front.z;
	}
	else
	{
		if((xz<xy) && (xz<yz))
		{
			cylinderData.radius = bSphere.size.x;
			cylinderData.height = 2*(bSphere.size.y);
			cylinderData.dir_x=bSphere.up.x;
			cylinderData.dir_y=bSphere.up.y;
			cylinderData.dir_z=bSphere.up.z;
		}
		else
		{
			cylinderData.radius = bSphere.size.y;
			cylinderData.height = 2*(bSphere.size.x);
			cylinderData.dir_x=bSphere.right.x;
			cylinderData.dir_y=bSphere.right.y;
			cylinderData.dir_z=bSphere.right.z;
		}
	}

	cylinderData.top_x=center.x-0.5f*cylinderData.dir_x*cylinderData.height;
	cylinderData.top_y=center.y-0.5f*cylinderData.dir_y*cylinderData.height;
	cylinderData.top_z=center.z-0.5f*cylinderData.dir_z*cylinderData.height;
}

void GetCollisionCone(const SAA_Scene	currentScene, SAA_Elem	*model, MSH_ClothCollisionCone &coneData)
{
	ZeroSphere bSphere;
	GetBoundingBox(currentScene, model, bSphere);
	float xz,xy,yz;

	ZeroVector4 center = ZeroVector4(bSphere.posit);
	xy=fabsf(bSphere.size.x-bSphere.size.y);
	xz=fabsf(bSphere.size.x-bSphere.size.z);
	yz=fabsf(bSphere.size.y-bSphere.size.z);
	if((xy<xz) && (xy<yz))
	{
		coneData.height = 2*(bSphere.size.z);
		coneData.radius1 = bSphere.size.x;
		coneData.dir_x=bSphere.front.x;
		coneData.dir_y=bSphere.front.y;
		coneData.dir_z=bSphere.front.z;
	}
	else
	{
		if((xz<xy) && (xz<yz))
		{
			coneData.radius1 = bSphere.size.x;
			coneData.height = 2*(bSphere.size.y);
			coneData.dir_x=bSphere.up.x;
			coneData.dir_y=bSphere.up.y;
			coneData.dir_z=bSphere.up.z;
		}
		else
		{
			coneData.radius1 = bSphere.size.y;
			coneData.height = 2*(bSphere.size.x);
			coneData.dir_x=bSphere.right.x;
			coneData.dir_y=bSphere.right.y;
			coneData.dir_z=bSphere.right.z;
		}
	}

	coneData.top_x = center.x-0.5f*coneData.dir_x*coneData.height;
	coneData.top_y = center.y-0.5f*coneData.dir_y*coneData.height;
	coneData.top_z = center.z-0.5f*coneData.dir_z*coneData.height;
	coneData.radius2=0.0;

	int nbVertices;
	SAA_modelGetNbVertices(&currentScene, model, &nbVertices);
	if(nbVertices)
	{
		SAA_DVector *vertices = (SAA_DVector *) malloc(nbVertices*sizeof(SAA_DVector));
		SAA_modelGetVertices(&currentScene, model, SAA_GEOM_ORIGINAL, 0, nbVertices, vertices);
		ZeroVector3 tmpTop(coneData.top_x,coneData.top_y,coneData.top_z);
		ZeroVector3 Dir(coneData.dir_x,coneData.dir_y,coneData.dir_z);
		float tmpTest0=0.01f;
		float tmpTest1=0.1f;
		float tmpTest2=0.1f;
		float tmpTest3=0.1f;
		float tmpTest4=0.01f;
		for(int i=0;i<nbVertices;i++)
		{
			ZeroVector3 vtx((float)vertices[i].x,(float)vertices[i].y,(float)vertices[i].z);
			vtx-=tmpTop;

			float r=Magnitude(vtx);
			if(fabsf(r)>tmpTest0)
			{
				if(fabsf(DotProduct(Dir,vtx))<tmpTest1)
				{
					tmpTest0 = fabsf(r);
					tmpTest1 = fabsf(DotProduct(Dir,vtx));
					if(fabsf(r-coneData.radius1)<tmpTest2)
					{
						tmpTest2 = fabsf(r-coneData.radius1);
						ZeroVector3 tmpBottom(center.x+0.5f*coneData.dir_x*coneData.height,
												center.y+0.5f*coneData.dir_y*coneData.height,
												center.z+0.5f*coneData.dir_z*coneData.height);
						for(int j=0;j<nbVertices;j++)
						{
							ZeroVector3 vtx2((float)vertices[j].x,(float)vertices[j].y,(float)vertices[j].z);
							vtx2-=tmpBottom;
							float r2=Magnitude(vtx2);
							float tmpTest1=1.0;
							float tmpTest2=1.0;
							if(fabsf(r2)>tmpTest4)
							{
								if(fabsf(DotProduct(Dir,vtx2))<tmpTest3)
								{
									tmpTest4 = fabsf(r2);
									tmpTest3 = fabsf(DotProduct(Dir,vtx2));
									if(coneData.radius2<r2)
										coneData.radius2=r2;
								}
							}
						}
						break;
					}
					else
					{
						if(coneData.radius2<r)
							coneData.radius2=r;
						coneData.top_x = center.x+0.5f*coneData.dir_x*coneData.height;
						coneData.top_y = center.y+0.5f*coneData.dir_y*coneData.height;
						coneData.top_z = center.z+0.5f*coneData.dir_z*coneData.height;
						coneData.dir_x=-coneData.dir_x;
						coneData.dir_y=-coneData.dir_y;
						coneData.dir_z=-coneData.dir_z;
					}
				}
			}
		}

		_SAA_FREE(&vertices);
	}

}


void CalculateCollisionSphere(const SAA_Scene	currentScene, SAA_Elem	*model, const SAA_CustomContext context, MSH_ClothCollisionSphere &sphereData, int textViewID)
{
	GetCollisionSphere(currentScene, model, sphereData);
	char s[1024];
	sprintf(s,"Center = (%0.3f,%0.3f,%0.3f)",sphereData.center_x,sphereData.center_y,sphereData.center_z);
	SAA_dialogitemSetStringValue(context,textViewID,0,s);
	sprintf(s,"Radius = %0.3f",sphereData.radius);
	SAA_dialogitemSetStringValue(context,textViewID,1,s);
}

void CalculateCollisionCylinder(const SAA_Scene	currentScene, SAA_Elem	*model, const SAA_CustomContext context, MSH_ClothCollisionCylinder &cylinderData, int textViewID)
{
	GetCollisionCylinder(currentScene,model,cylinderData);
	char s[1024];
	sprintf(s,"Top = (%0.3f,%0.3f,%0.3f)",cylinderData.top_x,cylinderData.top_y,cylinderData.top_z);
	SAA_dialogitemSetStringValue(context,textViewID,0,s);
	sprintf(s,"Direction = (%0.3f,%0.3f,%0.3f)",cylinderData.dir_x,cylinderData.dir_y,cylinderData.dir_z);
	SAA_dialogitemSetStringValue(context,textViewID,1,s);
	sprintf(s,"Radius = %0.3f",cylinderData.radius);
	SAA_dialogitemSetStringValue(context,textViewID,2,s);
	sprintf(s,"Height = %0.3f",cylinderData.height);
	SAA_dialogitemSetStringValue(context,textViewID,3,s);
}

void CalculateCollisionCone(const SAA_Scene	currentScene, SAA_Elem	*model, const SAA_CustomContext context, MSH_ClothCollisionCone &coneData, int textViewID)
{
	GetCollisionCone(currentScene,model,coneData);
	char s[1024];
	sprintf(s,"Top = (%0.3f,%0.3f,%0.3f)",coneData.top_x,coneData.top_y,coneData.top_z);
	SAA_dialogitemSetStringValue(context,textViewID,0,s);
	sprintf(s,"Direction = (%0.3f,%0.3f,%0.3f)",coneData.dir_x,coneData.dir_y,coneData.dir_z);
	SAA_dialogitemSetStringValue(context,textViewID,1,s);
	sprintf(s,"Radius1 = %0.3f",coneData.radius1);
	SAA_dialogitemSetStringValue(context,textViewID,2,s);
	sprintf(s,"Radius2 = %0.3f",coneData.radius2);
	SAA_dialogitemSetStringValue(context,textViewID,3,s);
	sprintf(s,"Height = %0.3f",coneData.height);
	SAA_dialogitemSetStringValue(context,textViewID,4,s);
}

void CalculateCollisionParameters(const SAA_Scene	currentScene, SAA_Elem	*model, const SAA_CustomContext context, const MSH_ClothCollisionConfig collData, MSH_ClothCollisionSphere &sphereData, MSH_ClothCollisionCylinder &cylinderData, MSH_ClothCollisionCone &coneData, int textViewID)
{
	for(int i =0;i<6;i++)
	{
		SAA_dialogitemSetStringValue(context,textViewID,i,"");
	}

	switch(collData.sType)
	{
	case COLLISION_SPHERE:
		CalculateCollisionSphere(currentScene, model, context, sphereData, textViewID);
		break;
	case COLLISION_CYLINDER:
		CalculateCollisionCylinder(currentScene, model, context, cylinderData, textViewID);
		break;
	case COLLISION_CONE:
		CalculateCollisionCone(currentScene, model, context, coneData, textViewID);
		break;
	}
}

