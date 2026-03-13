#include "stdafx.h"
#include "ZeroData.h"
#include "Scene2MSH.h"
#include "io.h"
#include "XSI2MSHProgressDialog.h"

#include "Zero.h"

#pragma warning (disable : 4310)

#define FLCOMPARE(A,B,C) ((((A)<(B)) ? (((B)-(A)) <= (C)) : (((A)-(B)) <= (C))))

#include "MSHBlockCamera.h"
#include "MSHBlockFogData.h"
#include "MSHBlockAmbience.h"
#include "MSHBlockFileInfo.h"
#include "MSHBlockSkeleton.h"
#include "MSHBlockSceneInfo.h"
#include "MSHBlockSpotLight.h"
#include "MSHBlockAnimations.h"
#include "MSHBlockPointLight.h"
#include "MSHBlockMaterialSet.h"
#include "ZeroImportExportNode.h"
#include "MSHBlockInfiniteLight.h"
#include "MSHBlockConstraintList.h"
#include "ZeroImportExportUserData.h"

#include "SIMath.h"

#import "SIobjectmodel.tlb"
#import "SI3dobjectmodel.tlb"

extern char *g_szSceneName;

CScene2MSH *CScene2MSH::mMe;
OLECHAR FAR *CScene2MSH::sBSTRInit=L"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

static float AngleMod (double x)
{
	 x = fmod (x, 360.0);
	 if (x < 0) {
		 x += 360;
	 }

	 if (x > 180.0) {
		x = x - 360.0;
	 }

	 _ASSERTE (x < 180.001);
	 _ASSERTE (x > -180.001);
	 return (float) x;
}

void MatrixSanityCheck (const ZeroMatrix& matrix)
{
	_ASSERTE (matrix.SanityCheck());
	_ASSERTE (fabsf (DotProduct (matrix.right, matrix.right) - 1.0f)  < 1.0e-4f);
	_ASSERTE (fabsf (DotProduct (matrix.up, matrix.up) - 1.0f)  < 1.0e-4f);
	_ASSERTE (fabsf (DotProduct (matrix.front, matrix.front) - 1.0f)  < 1.0e-4f);
}


void ModelByCRC::AddModel (X3DObject *model)
{
	if(!model)
		return;

	VARIANT name;
	name.vt=VT_BSTR;
	name.bstrVal=::SysAllocString(L"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	SIObject *pSIModel=NULL;
	if(SUCCEEDED(model->QueryInterface(IID_SIObject,(void **)&pSIModel)))
	{
		pSIModel->get_FullName(&name.bstrVal);
		pSIModel->Release();
	}
	_bstr_t tempName(name.bstrVal,true);
	unsigned int crc=0;
	crc=CalcStrCRC(tempName,crc);
	if(!Find(crc))
	{
		char *pos=strstr(tempName,"torso_eff__h");
		if(pos)
		{
			if(strlen(pos)==12) // to rule out torso_eff__h.child
			{
				mTorsoModel=model;
			}
		}
		Insert(model,crc);
	}
	::SysFreeString (name.bstrVal);  
}

void PolygonListByType::AddPoly(CScene2MSH *aConverter, CZeroData *aData, ObjClsAttrib *key, unsigned int polyIndex, unsigned int numVertices)
{
	unsigned crc=0;
	ZeroTreeNode *node;
	
	crc = CalcStrCRC(aConverter->GetNewMaterialName(aData,key), crc);
	node = Find (crc);
	if (!node) 
	{
		UniformPolygonList polyType (InvertCRC(crc));
		node = Insert (polyType, crc);
	}
	node->info.AddPoly(polyIndex,numVertices);
	m_maxVertexCount += numVertices;
}

CScene2MSH::CScene2MSH()
{
	m_bWarningsOn = true;
	m_sDestinationPath = NULL;
    mMe = this;
	m_CurrentID=0;
	mExportSelectedOnly=true;
	mExportTextures=true;
	mExportAnimations=true;
	mCameraAnimations=false;
	mExportLights=true;
	mExportSIInfo=true;
	mClipLoopedAnimations=false;
	mBaseposeFromCurrentFrame=false;
//	mFilterPoseCNSAnims=false;
	mFlattenGeometry=false;
	m_TextureFormat = 0;
	m_Platform=PLATFORM_PC;
	m_App=NULL;
	mExportFKAnimation=false;
	mSkipBasepose = false;
	mExportLeftHanded = false;
}

CScene2MSH::~CScene2MSH()
{
	if(m_sDestinationPath)
		delete[] m_sDestinationPath;

	// extra code by Mike Z
	char info[200];
	if(CMSHBlockTriangleStripList::numStrips > 0)
		CMSHBlockTriangleStripList::avgStripLength /= CMSHBlockTriangleStripList::numStrips;

    sprintf(info, "Max Length %d\nMin Length %d\nNum Strips %d\nAvg Length %d", CMSHBlockTriangleStripList::maxStripLength,
			CMSHBlockTriangleStripList::minStripLength, CMSHBlockTriangleStripList::numStrips,
			CMSHBlockTriangleStripList::avgStripLength);

	strncat(CMSHBlockTriangleStripList::stripLengthOutput, info, sizeof(CMSHBlockTriangleStripList::stripLengthOutput));

#ifdef DEBUG
	::MessageBox(NULL, CMSHBlockTriangleStripList::stripLengthOutput, "Tristrip info", 0);
#endif
}

void CScene2MSH::SetFilters(BOOL aExportSelected, BOOL aExportTextures, BOOL aExportAnimations, BOOL aExportLights, BOOL aExportSIInfo,  BOOL aClipLoopedAnims, BOOL aBaseposeFromCurrentFrame, BOOL aFlattenGeometry, BOOL aCameraAnims, BOOL aExportFKAnimations, BOOL aSkipBasepose, BOOL aExportLeftHanded)
{
	mExportSelectedOnly=aExportSelected;
	mExportTextures=aExportTextures;
	mExportAnimations=aExportAnimations;
	mCameraAnimations=aCameraAnims;
	mExportLights=aExportLights;
	mExportSIInfo=aExportSIInfo;
	mClipLoopedAnimations=aClipLoopedAnims;
	mBaseposeFromCurrentFrame = aBaseposeFromCurrentFrame;
	mFlattenGeometry=aFlattenGeometry;
	mExportFKAnimation=aExportFKAnimations;
	mSkipBasepose = aSkipBasepose;
	mExportLeftHanded = aExportLeftHanded;
}

void CScene2MSH::SetPlatform(int platform)
{
//	switch(platform)
//	{
//	case 0:m_Platform=PLATFORM_PC;break;
//	case 1:m_Platform=PLATFORM_XBOX___;break;
//	case 2:m_Platform=PLATFORM_PLAY_STATION2___;break;
//	}

	m_Platform=PLATFORM_PC;
}

void CScene2MSH::SetTextureFormat(int format)
{
	m_TextureFormat = format;
}

void CScene2MSH::SetDestinationPath(const char *aDstPath)
{
	if(m_sDestinationPath)
		delete[] m_sDestinationPath;
	m_sDestinationPath = new char[strlen(aDstPath)+1];
	strcpy(m_sDestinationPath,aDstPath);
}

void CScene2MSH::SetWarningsOn(bool warnings)
{
	m_bWarningsOn = warnings;
}

char *CScene2MSH::StripPath(char *aFilename) 
{
	int len = strlen(aFilename);

	do 
	{
		len--;
	} while ((len >= 0) && aFilename[len] != '/' && aFilename[len] != '\\' && aFilename[len] != ':');

	if (len >= 0) 
	{
		char *pth = NULL;

		pth = strdup(aFilename);
		pth[len + 1] = 0;
		return pth;
	}

	return strdup("");
}

void CScene2MSH::CreateZeroStructure(CZeroData *pData, Project *pProject, Scene *pScene, char *pathFiles,CXSI2MSHProgressDialog *progDlg)
{
	m_ProgDlg=progDlg;

    mZeroData = pData;
	mZeroData->SetPlatform(m_Platform);
	XSIProject *pXSIProject=NULL;
	if(SUCCEEDED(pProject->QueryInterface(IID_XSIProject,(void **)&pXSIProject)))
	{
		PropertyCollection *props=NULL;
		pXSIProject->get_Properties(&props);
		VARIANT varPropIdx;
		varPropIdx.vt=VT_BSTR;
		_bstr_t bstrPropIdx("Play Control");
		varPropIdx.bstrVal = bstrPropIdx;

		Property *prop=NULL;
		props->get_Item(varPropIdx,&prop);
		if(prop)
		{
			VARIANT retVal;
			retVal.vt=VT_R8;

			GetParameter(prop,"current",1.0,&retVal);
			m_CurrentFrame=(float)retVal.dblVal;

			GetParameter(prop,"in",m_CurrentFrame,&retVal);
			m_StartFrame=(float)retVal.dblVal;

			GetParameter(prop,"out",m_CurrentFrame,&retVal);
			m_EndFrame=(float)retVal.dblVal;

			GetParameter(prop,"rate",m_CurrentFrame,&retVal);
			m_FrameRate=(float)retVal.dblVal;

			prop->Release();
		}
		pXSIProject->Release();

		if (props)
			props->Release();
	}

	SIObject *pSIScene=NULL;
	if(SUCCEEDED(pScene->QueryInterface(IID_SIObject,(void **)&pSIScene)))
	{
		pSIScene->get_Application(&m_App);
		pSIScene->Release();
	}

	m_ProgDlg->SetProgress("Getting Environment Data",0);
	CreateZeroFileInfo(pData, pProject);
	m_ProgDlg->SetProgress(NULL,25);
	CreateZeroSceneInfo(pData, pScene);
	m_ProgDlg->SetProgress(NULL,50);
	CreateZeroAmbience(pData, pScene);
	m_ProgDlg->SetProgress(NULL,75);
	CreateZeroFogData(pData, pScene);
	m_ProgDlg->SetProgress(NULL,100);

	m_ProgDlg->SetProgress("Getting Materials",0);
	CreateMaterials(pData, pScene);
	m_ProgDlg->SetProgress(NULL,100);


	m_ProgDlg->SetProgress("Getting Models",0);
    CreateModels(pData, pScene, pathFiles);
	ZeroMatrix globalMatrix = YawMatrix(PI);
	
	// Corrections made to keep coordinates right-handed:
	// -- the following two lines are commented out
	// -- Call to OnExportFile 3rd param is now false
	// -- In CScene2MSH::CreateFreeJointAnimation, removed the "hardpoint" correction code.
	// -- In _MSHParser_ library (not here), In ZeroImportExportNode.cpp, ZeroImportExportNode::OnExportFile, 
	//    removed call to ExtractNonSkinPolygonsFromSkin().
	// -- UPDATE: 12/13/02 -- now left or right handed is based on user-checkbox

	if (mExportLeftHanded)
	{
		mDatabase->ApplyGlobalRotation(mZeroData,globalMatrix);
		CorrectGlobalRotationForSkeleton(mDatabase,identityMatrix,identityMatrix,false);
	}

	m_ProgDlg->SetProgress(NULL,100);
    
	if (!mExportFKAnimation)
	{
		AlignBonesChainRoot(mDatabase);
	}
	if(mExportAnimations)
		CreateSkeletonAndAnimations(pScene);
//	SplitSegments(mZeroData);
	
	m_ProgDlg->SetProgress("Calculating bounding boxes",0);
	CreateSkinBoundingBoxes(pData);	
	m_ProgDlg->SetProgress("Pre-Processing Data",50);
	mDatabase->OnExportFile(pData,true,!!mExportLeftHanded,true,true,mFlattenGeometry ? true : false);
//	mDatabase->OnExportFile(pData,true,true,true,true, false);
	m_ProgDlg->SetProgress("Done",100);
    mDatabase->Release(); 
    mDatabase = NULL;


//#ifdef _DEBUG_LOG
//	m_ProgDlg->SetProgress("Debug output",0);
//	pData ->DebugOutput();
//	m_ProgDlg->SetProgress("Debug output",100);
//#endif
}


void CScene2MSH::GetGlobalPosition(IDispatch *aDisp,vect3 &vPos)
{
	X3DObject *pX3DObj;
	if(SUCCEEDED(aDisp->QueryInterface(IID_X3DObject,(void **)&pX3DObj)))
	{
		Kinematics *kine;
		if(SUCCEEDED(pX3DObj->get_Kinematics(&kine)))
		{
			IDispatch *pDisp;
			if(SUCCEEDED(kine->get_Global(&pDisp)))
			{
				Parameter *pKineState;
				if(SUCCEEDED(pDisp->QueryInterface(IID_Parameter,(void **)&pKineState)))
				{
					VARIANT posX,posY,posZ;
					posX.vt=posY.vt=posZ.vt=VT_R8;
					GetParameter(pKineState,"posx",m_CurrentFrame,&posX);
					GetParameter(pKineState,"posy",m_CurrentFrame,&posY);
					GetParameter(pKineState,"posz",m_CurrentFrame,&posZ);

					vPos.fX=(float)posX.dblVal;
					vPos.fY=(float)posY.dblVal;
					vPos.fZ=(float)posZ.dblVal;

					pKineState->Release();
				}
				pDisp->Release();
			}
			kine->Release();
		}
		pX3DObj->Release();
	}
}

void CScene2MSH::CreateZeroFileInfo(CZeroData *pData,Project *pProject)
{
//	CMSHBlockFileInfo *pFileInfo;

//	pData ->CreateNewFileInfo();
//	pFileInfo = pData ->GetFileInfo();
//	pFileInfo ->SetProjectName(pProject ->);
//	pFileInfo ->SetUserName((char *)pProject->UserName);
}

void CScene2MSH::CreateZeroSceneInfo(CZeroData *pData,Scene *pScene)
{
	CMSHBlockSceneInfo *pSceneInfo;
	pData ->CreateNewSceneInfo();
	pSceneInfo = pData ->GetSceneInfo();

	// get scene name
	char sSceneName[256];
	GetSIObjectName(pScene, sSceneName, 256);
	pSceneInfo ->SetSceneName(sSceneName);
	pSceneInfo ->SetFrameInfo((int)m_StartFrame,(int)m_EndFrame,m_FrameRate);
}

void CScene2MSH::CreateZeroAmbience(CZeroData *pData,Scene *pScene)
{
/*
	CMSHBlockAmbience *pAmbience;
	pData ->CreateNewAmbience();
	pAmbience = pData ->GetAmbience();
	pAmbience ->SetColor((float)pScene->GetAmbientColor()->GetR(),(float)pScene->GetAmbientColor()->GetG(),(float)pScene->GetAmbientColor()->GetB(),(float)pScene->GetAmbientColor()->GetA());
*/
}

void CScene2MSH::CreateZeroFogData(CZeroData *pData,Scene *pScene)
{
/*
	SI_Error result = SI_SUCCESS;
	SAA_Boolean bFog;
	_SAA_CALL(SAA_sceneGetRenCtrlFadeActive(pScene->GetSAAElem(),&bFog), "SAA_sceneGetRenCtrlFadeActive");

	// if fog is active...
	if (bFog)
	{
		CMSHBlockFogData *pFog;
		pData ->CreateNewFog();
		pFog = pData ->GetFogData();

		// write color
		pFog ->SetColor((float)pScene->GetFogColor()->GetR(),(float)pScene->GetFogColor()->GetG(),(float)pScene->GetFogColor()->GetB(),(float)pScene->GetFogColor()->GetA());
		
		// write fog near
		pFog ->SetPlanes((float)(pScene->GetFogNearPlane()),(float)(pScene->GetFogFarPlane()));
	}
*/
}

void CScene2MSH::SetParameter(IDispatch *aObj, char *aParamName, float aFrameNumber, VARIANT *aValue)
{
	HRESULT hProj;
	HRESULT hParam;
	HRESULT hParams;
	ProjectItem *pProjItem;
	Parameter *pParam;

	ParameterCollection *parameters=NULL;
	hProj = aObj->QueryInterface(IID_ProjectItem,(void **)&pProjItem);
	hParam = aObj->QueryInterface(IID_Parameter,(void **)&pParam);

	if(SUCCEEDED(hProj))
	{
		hParams =pProjItem->get_Parameters(&parameters);
	}
	else
	{
		if(SUCCEEDED(hParam))
			hParams=pParam->get_Parameters(&parameters);
		else
			return;
	}

	if(SUCCEEDED(hParams))
	{
		if(parameters)
		{
			Parameter *param=NULL;
			_bstr_t sName=aParamName;
			VARIANT idx;
			idx.vt=VT_BSTR;
			idx.bstrVal=sName;
			parameters->get_Item(idx,&param);
			if(param)
			{
				VARIANT frameNum;
				frameNum.vt=VT_R8;
				frameNum.dblVal=aFrameNumber;
				param->put_Value(frameNum,*aValue);

				param->Release();
			}
			parameters->Release();
		}
	}
	if(SUCCEEDED(hProj))
		pProjItem->Release();

	if(SUCCEEDED(hParam))
		pParam->Release();

}

bool CScene2MSH::GetParameter(IDispatch *aObj, char *aParamName, float aFrameNumber, VARIANT *aValue)
{
	bool found;
	HRESULT hProj;
	HRESULT hParam;
	HRESULT hParams;
	ProjectItem *pProjItem;
	Parameter *pParam;

	ParameterCollection *parameters=NULL;
	hProj = aObj->QueryInterface(IID_ProjectItem,(void **)&pProjItem);
	hParam = aObj->QueryInterface(IID_Parameter,(void **)&pParam);

	if(SUCCEEDED(hProj))
	{
		hParams =pProjItem->get_Parameters(&parameters);
	}
	else
	{
		if(SUCCEEDED(hParam))
			hParams=pParam->get_Parameters(&parameters);
		else
			return false;
	}

	found	= true;
	if(SUCCEEDED(hParams))
	{
		if(parameters)
		{
			Parameter *param=NULL;
			_bstr_t sName=aParamName;
			VARIANT idx;
			idx.vt=VT_BSTR;
			idx.bstrVal=sName;
			parameters->get_Item(idx,&param);

			if(param)
			{
				VARIANT tempVar;
				VARIANT frameNum;
				frameNum.vt=VT_R8;
				frameNum.dblVal=aFrameNumber;

				VariantClear(&tempVar);
				tempVar.vt=aValue->vt;
				if(tempVar.vt==VT_BSTR)
				{
					tempVar.vt|=VT_BYREF;
					param->get_Value(frameNum,&tempVar);
					SysReAllocString(&aValue->bstrVal, tempVar.bstrVal);
				}
				else
				{
					param->get_Value(frameNum,&tempVar);
					VariantChangeType(aValue,&tempVar,0,aValue->vt);
				}

				param->Release();
			}
			else
			{
				found	= false;
			}

			parameters->Release();
		}

	}
	if(SUCCEEDED(hProj))
		pProjItem->Release();

	if(SUCCEEDED(hParam))
		pParam->Release();

	return found;

}

void CScene2MSH::GetPropertyByType(IDispatch *aObj, char *aPropType,Property **aProp,int offset)
{
	HRESULT hSceneItem;
	HRESULT hProperties;
	SceneItem *pSceneItem;

	PropertyCollection *properties=NULL;
	hSceneItem = aObj->QueryInterface(IID_SceneItem,(void **)&pSceneItem);

	*aProp=NULL;
	if(SUCCEEDED(hSceneItem))
	{
		hProperties =pSceneItem->get_Properties(&properties);

		if(SUCCEEDED(hProperties))
		{
			long nCount;
			if(SUCCEEDED(properties->get_Count(&nCount)))
			{
				bool bFound=false;
				int j=0;
				for(long i=0;i<nCount && !bFound;i++)
				{
					Property *prop;
					VARIANT idx;
					idx.vt=VT_I4;
					idx.lVal=i;
					if(SUCCEEDED(properties->get_Item(idx,&prop)))
					{
						char sType[32];
						GetSIObjectType(prop,sType,32);
						if(strcmpi((char *)sType, aPropType)==0)
						{
							if(offset==j)
							{
								*aProp=prop;
								bFound = true;
							}
							else
							{
								j++;
							}
						}

						prop->Release();
					}
				}
			}

			// Releasing this here causes a crash in GetPropertyType later, who knows why -fcole
			// properties->Release();
		}

	
		pSceneItem->Release();
	}
}

void CScene2MSH::DumpParameters(IDispatch *aObj, char *objName)
{
	HRESULT hProj;
	HRESULT hParam;
	HRESULT hParams;
	ProjectItem *pProjItem;
	Parameter *pParam;

	ParameterCollection *parameters=NULL;
	hProj = aObj->QueryInterface(IID_ProjectItem,(void **)&pProjItem);
	hParam = aObj->QueryInterface(IID_Parameter,(void **)&pParam);

	if(SUCCEEDED(hProj))
	{
		hParams =pProjItem->get_Parameters(&parameters);
	}
	else
	{
		if(SUCCEEDED(hParam))
			hParams=pParam->get_Parameters(&parameters);
		else
			return;
	}

	if(SUCCEEDED(hParams))
	{
		FILE *f=fopen("C:\\Parameters.txt","at");
		fprintf(f,"\n%s\n",objName);
		long nCount;
		if(SUCCEEDED(parameters->get_Count(&nCount)))
		{
			bool bFound=false;
			for(long i=0;i<nCount && !bFound;i++)
			{
				Parameter *param;
				VARIANT idx;
				idx.vt=VT_I4;
				idx.lVal=i;
				if(SUCCEEDED(parameters->get_Item(idx,&param)))
				{
					BSTR temp;
					_bstr_t sName;
					if(SUCCEEDED(param->get_ScriptName(&temp)))
					{
						sName=temp;
						fprintf(f,"\t%s\n",(char *)sName);				
					}

					param->Release();
				}
			}
		}
		fclose(f);
		parameters->Release();
	}

	if(SUCCEEDED(hProj))
		pProjItem->Release();
	if(SUCCEEDED(hParam))
		pParam->Release();

}

void CScene2MSH::CreateCamera(CZeroData *pData,Camera *pCamera)
{
	CMSHBlockCamera *pMSHCamera;
	if(!pData ->GetCamera())
		pData ->CreateNewCamera();
	pMSHCamera = pData ->GetCamera();

	// camera name
	VARIANT name;
	name.vt=VT_BSTR;
	name.bstrVal=::SysAllocString(sBSTRInit);
	GetParameter(pCamera,"name",m_CurrentFrame,&name);
	_bstr_t tempName=name.bstrVal;
	pMSHCamera->SetName((char *)tempName);
	::SysFreeString (name.bstrVal);  

	// camera position
	Kinematics *kine=NULL;
	X3DObject *pX3DObj;
	if(SUCCEEDED(pCamera->QueryInterface(IID_X3DObject,(void **)&pX3DObj)))
	{
		if(SUCCEEDED(pX3DObj->get_Kinematics(&kine)))
		{
			IDispatch *pDisp;
			if(SUCCEEDED(kine->get_Global(&pDisp)))
			{
				Parameter *pKineState;
				if(SUCCEEDED(pDisp->QueryInterface(IID_Parameter,(void **)&pKineState)))
				{
					VARIANT posX,posY,posZ;
					posX.vt=posY.vt=posZ.vt=VT_R8;
					GetParameter(pKineState,"posx",m_CurrentFrame,&posX);
					GetParameter(pKineState,"posy",m_CurrentFrame,&posY);
					GetParameter(pKineState,"posz",m_CurrentFrame,&posZ);
					VARIANT rotX,rotY,rotZ;
					rotZ.vt=VT_R8;
					GetParameter(pKineState,"rotz",m_CurrentFrame,&rotZ);
					pMSHCamera->SetRoll((float)(DEG2RAD*rotZ.dblVal));
					pMSHCamera->SetPosition((float)posX.dblVal,(float)posY.dblVal,(float)posZ.dblVal);

					if(mCameraAnimations)
					{
						char name[32];

						_splitpath(pData->GetExportFilename(),NULL,NULL,name,NULL);
						CMSHBlockCameraAnimation *anim=pMSHCamera->CreateAnimation(name);
						anim->SetKeyframesCount((int)(m_EndFrame-m_StartFrame+1));
						VARIANT fov;
						for(int i=(int)m_StartFrame;i<=(int)m_EndFrame;i++)
						{
							// camera field of view
							fov.vt=VT_R8;
							GetParameter(pCamera,"fov",(float)i,&fov);

							posX.vt=posY.vt=posZ.vt=VT_R8;
							rotX.vt=rotY.vt=rotZ.vt=VT_R8;

							GetParameter(pKineState,"posx",(float)i,&posX);
							GetParameter(pKineState,"posy",(float)i,&posY);
							GetParameter(pKineState,"posz",(float)i,&posZ);
							GetParameter(pKineState,"rotx",(float)i,&rotX);
							GetParameter(pKineState,"roty",(float)i,&rotY);
							GetParameter(pKineState,"rotz",(float)i,&rotZ);

							ZeroMatrix M(PitchYawRollMatrix(DEG2RAD*(float)rotX.dblVal,DEG2RAD*(float)rotY.dblVal,DEG2RAD*(float)rotZ.dblVal));
							ZeroQuaternion q(M);
							ZeroVector3 p((float)posX.dblVal, (float)posY.dblVal, (float)posZ.dblVal);

							(*anim)[i-(int)m_StartFrame].index=i;
							(*anim)[i-(int)m_StartFrame].position=p;
							(*anim)[i-(int)m_StartFrame].rotation=q;
							//
							//	this changes horizontal fov to vertical fov 
							(*anim)[i-(int)m_StartFrame].mFOV=(float)(2*atan(0.75*tan(0.5*DEG2RAD*fov.dblVal)));
						}
					}

					pKineState->Release();
				}
				pDisp->Release();
			}
			kine->Release();
		}
		pX3DObj->Release();
	}

	// camera field of view
	VARIANT curr_fov;
	curr_fov.vt=VT_R8;
	GetParameter(pCamera,"fov",m_CurrentFrame,&curr_fov);
	pMSHCamera->SetFOV((float)(DEG2RAD*curr_fov.dblVal));

	// camera interest
	DirectedObject *pDirObj;
	if(SUCCEEDED(pCamera->QueryInterface(IID_DirectedObject,(void **)&pDirObj)))
	{
		X3DObject *pInterest;
		if(SUCCEEDED(pDirObj->get_Interest(&pInterest)))
		{
			vect3 vInterest;
			GetGlobalPosition(pInterest,vInterest);
			pMSHCamera->SetInterest(vInterest.fX,vInterest.fY,vInterest.fZ);

			pInterest->Release();
		}
		pDirObj->Release();
	}

	// camera near/far planes
	VARIANT nearPlane,farPlane;
	nearPlane.vt=VT_R8;
	farPlane.vt=VT_R8;
	GetParameter(pCamera,"near",m_CurrentFrame,&nearPlane);
	GetParameter(pCamera,"far",m_CurrentFrame,&farPlane);
	pMSHCamera->SetPlanes((float)nearPlane.dblVal,(float)farPlane.dblVal);
}

void CScene2MSH::AddTextureToMaterial(IDispatch *aDisp,CMSHBlockMaterial *aMat,Application *pApp)
{
	HRESULT hr;
	VARIANT varResult;
	EXCEPINFO ExcepInfo;
	unsigned int argErr;

	DISPID dispid;			// id of method/property
	DISPPARAMS dispparams; // arguments for method call
	DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
	_bstr_t bstrParam;

	//	aDisp.Name
	VARIANT dispName;
	dispName.vt=VT_BSTR;
	OLECHAR FAR* szName = OLESTR("Name");
	aDisp->GetIDsOfNames(IID_NULL, &szName, 1,LOCALE_USER_DEFAULT, &dispid);
	aDisp->Invoke(dispid,IID_NULL,LOCALE_USER_DEFAULT,DISPATCH_PROPERTYGET,&dispparamsNoArgs, &dispName, NULL, NULL);
	_bstr_t tempName(dispName.bstrVal,true);

	//	srcName = GetValue("Clips." & aDisp.name & ".SourceName")
	VARIANT varSrcName;
	varSrcName.vt=VT_BSTR;
	dispparams.rgvarg=new VARIANT[1];
	OLECHAR FAR* szGetValue = OLESTR("GetValue");
	dispparams.rgvarg[0].vt = VT_BSTR;
	dispparams.rgvarg[0].bstrVal = _bstr_t("Clips.") + tempName + _bstr_t(".SourceName");
	pApp->GetIDsOfNames(IID_NULL,&szGetValue,1,LOCALE_SYSTEM_DEFAULT,&dispid);
	dispparams.cArgs = 1;
	dispparams.cNamedArgs = 0;
	hr = pApp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,DISPATCH_METHOD,&dispparams,&varSrcName,&ExcepInfo,&argErr);
	_bstr_t srcName(varSrcName.bstrVal,true);
	delete[] dispparams.rgvarg;

	//	GetValue("Sources." & srcName & ".FileName")
	dispparams.rgvarg=new VARIANT[1];
	dispparams.rgvarg[0].vt = VT_BSTR;
	dispparams.rgvarg[0].bstrVal = _bstr_t("Sources.") + srcName + _bstr_t(".Filename");
	dispparams.cArgs = 1;
	dispparams.cNamedArgs = 0;
	hr = pApp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,DISPATCH_METHOD,&dispparams,&varResult,&ExcepInfo,&argErr);
	_bstr_t fileName(varResult.bstrVal,true);
	delete[] dispparams.rgvarg;

	char *sTemp=(char *)fileName;
	char *sDir=StripPath(sTemp);
	if(strlen(sDir)<strlen(sTemp))
	{
		char *namePtr=&sTemp[strlen(sDir)];
		aMat->AddTexture(namePtr,sTemp);
	}
	free(sDir);
/*
	path=GetValue("Sources." & elem.name & ".Filename")
*/
}

void CScene2MSH::AddTexturesToMaterial(IDispatch *aDisp,CMSHBlockMaterial *aMat,Application *pApp)
{
	if(pApp)
	{
		HRESULT hr;
		VARIANT varResult;
		EXCEPINFO ExcepInfo;
		unsigned int argErr;
		
		DISPID dispid;			// id of method/property
		DISPPARAMS dispparams; // arguments for method call
		DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
		_bstr_t bstrParam;

		//	EnumElements obj, True
		dispparams.rgvarg=new VARIANT[2];
		OLECHAR FAR* szEnumElements = OLESTR("EnumElements");

		dispparams.rgvarg[1].vt = VT_DISPATCH;
		dispparams.rgvarg[1].pdispVal = aDisp;
		dispparams.rgvarg[0].vt = VT_BOOL;
		dispparams.rgvarg[0].boolVal = VARIANT_TRUE;

		pApp->GetIDsOfNames(IID_NULL,&szEnumElements,1,LOCALE_SYSTEM_DEFAULT,&dispid);
		dispparams.cArgs = 2;
		dispparams.cNamedArgs = 0;
		hr = pApp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,DISPATCH_METHOD,&dispparams,&varResult,&ExcepInfo,&argErr);
		delete[] dispparams.rgvarg;

		if(varResult.pdispVal)
		{
			VARIANT varEnum;
			varResult.pdispVal->Invoke(DISPID_NEWENUM,IID_NULL,LOCALE_SYSTEM_DEFAULT,DISPATCH_PROPERTYGET,&dispparamsNoArgs,&varEnum,NULL,NULL);
			if(varEnum.punkVal)
			{
				IEnumVARIANT *pIter=NULL;
				if(SUCCEEDED(varEnum.punkVal->QueryInterface(IID_IEnumVARIANT,(void **)&pIter)))
				{
					VARIANT child;
					child.vt=VT_ARRAY|VT_VARIANT;
					child.parray=NULL;
					ULONG nReturned;
					pIter->Reset();
					HRESULT hr = pIter->Next(1,&child,&nReturned);
					while(SUCCEEDED(hr) && (nReturned==1))
					{
						VARIANT childName;
						childName.vt=VT_BSTR;
						//	child.Name
						OLECHAR FAR* szName = OLESTR("Name");
						child.pdispVal->GetIDsOfNames(IID_NULL, &szName, 1,LOCALE_USER_DEFAULT, &dispid);
						child.pdispVal->Invoke(dispid,IID_NULL,LOCALE_USER_DEFAULT,DISPATCH_PROPERTYGET,&dispparamsNoArgs, &childName, NULL, NULL);
						_bstr_t tempName(childName.bstrVal,true);
						if(strcmp(tempName,"Image")==0)
						{
							AddTextureToMaterial(aDisp,aMat,pApp);
						}
						else
							AddTexturesToMaterial(child.pdispVal,aMat,pApp);
						hr = pIter->Next(1,&child,&nReturned);
					}
					pIter->Release();
				}
				varEnum.punkVal->Release();
			}
		}
	}
}

void CScene2MSH::GetUniqueMaterialName(const char *aName,char *aUniqueName,const int nameSize)
{
	bool bMatch=true;
	int id=0;
	char tempName[256];
	while(bMatch)
	{
		sprintf(tempName,"%s%d",aName,id);
		unsigned crc=0;
		crc=CalcStrCRC(tempName,crc);
		MaterialByNameCRC::ZeroTreeNode *node=m_MaterialByNameTree.Find(crc);
		if(!node)
			bMatch=false;
		id++;
	}

	strncpy(aUniqueName,tempName,nameSize);
}

void CScene2MSH::CreateMSHMaterialFromXSIMaterial(CMSHBlockMaterialSet *aMSHMatSet, CMSHBlockMaterial *aMSHMaterial, Material *aMaterial)
{
	// write name
	char sName[256];
	char sUniqueName[256];
	GetSIObjectName(aMaterial,sName,256);
	GetUniqueMaterialName(sName,sUniqueName,256);
	aMSHMaterial ->SetName(sUniqueName);
	unsigned crc=0;
	crc=CalcStrCRC(sUniqueName,crc);
	m_MaterialByNameTree.Insert(aMSHMaterial,crc);
	

	OGLMaterial *oglMaterial;
	Color *col=NULL;
	double r,g,b,a;
	double decay;
	siShadingModel shModel;

	ShaderCollection *shaders=NULL;
	aMaterial->get_Shaders(&shaders);
	a=1.f;
	if(shaders)
	{
		VARIANT varIdx;
		varIdx.vt=VT_I4;
		varIdx.lVal = 0;
		Shader *shader=NULL;
		long nShaders=0;
		shaders->get_Count(&nShaders);
		if(nShaders)
		{
			shaders->get_Item(varIdx,&shader);
			ProjectItem *pPIShader=NULL;
			if(SUCCEEDED(shader->QueryInterface(IID_ProjectItem,(void **)&pPIShader)))
			{
				ParameterCollection *parameters;
				Parameter *pTransparency=NULL;
				_bstr_t tempBstr("Transparency");
				varIdx.vt = VT_BSTR;
				varIdx.bstrVal = tempBstr;
				pPIShader->get_Parameters(&parameters);
				parameters->get_Item(varIdx,&pTransparency);
				if(pTransparency)
				{
					a = 0.f;
					VARIANT varTemp;
					varTemp.vt=VT_R4;
					GetParameter(pTransparency,"red", m_CurrentFrame,&varTemp);
					a += (float)(1.0-varTemp.fltVal);

					GetParameter(pTransparency,"green", m_CurrentFrame,&varTemp);
					a += (float)(1.0-varTemp.fltVal);

					GetParameter(pTransparency,"blue", m_CurrentFrame,&varTemp);
					a += (float)(1.0-varTemp.fltVal);
					a/=3.f;
				}
				pPIShader->Release();
			}

			if (shader)
				shader->Release();
		}

		shaders->Release();
	}

	if(SUCCEEDED(aMaterial->get_OGLMaterial(&oglMaterial)))
	{
		// write color
		r=g=b=0.0;
		oglMaterial->get_Diffuse(&col);
		if(col)
		{
			col->get_Red(&r);
			col->get_Green(&g);
			col->get_Blue(&b);
			aMSHMaterial ->SetColor((float)r,(float)g,(float)b,(float)a);

			col->Release();
		}
	
		// write specular color
		r=g=b=a=0.0;
		col=NULL;
		oglMaterial->get_Specular(&col);
		if(col)
		{
			col->get_Red(&r);
			col->get_Green(&g);
			col->get_Blue(&b);
			col->get_Alpha(&a);
			aMSHMaterial ->SetSpecularColor((float)r,(float)g,(float)b,(float)a);

			col->Release();
		}

		//	write power
		oglMaterial->get_Decay(&decay);
		aMSHMaterial ->SetPower((float)decay);
		
		// write ambient color
		col=NULL;
		r=g=b=a=0.0;
		oglMaterial->get_Ambient(&col);
		if(col)
		{
			col->get_Red(&r);
			col->get_Green(&g);
			col->get_Blue(&b);
			col->get_Alpha(&a);
			aMSHMaterial ->SetAmbientColor((float)r,(float)g,(float)b,(float)a);

			col->Release();
		}

		
		// write shading model
		oglMaterial->get_ShadingModel(&shModel);
		switch(shModel)
		{
		case siShadingModelConstant:
			aMSHMaterial ->SetShadingModel(0);
			break;
			
		case siShadingModelLambert:
			aMSHMaterial ->SetShadingModel(1);
			break;
			
		case siShadingModelPhong:
			aMSHMaterial ->SetShadingModel(2);
			break;
			
		case siShadingModelBlinn:
			aMSHMaterial ->SetShadingModel(3);
			break;
			
		case siShadingModelShadow:
			aMSHMaterial ->SetShadingModel(4);
			break;
			
		case siShadingModelColorVertex:
			aMSHMaterial ->SetShadingModel(5);
			break;

		default:
			aMSHMaterial ->SetShadingModel(0);
			break;
		}
	}



/*
	SIObject *pSI;
	if(SUCCEEDED(aMaterial->QueryInterface(IID_SIObject,(void **)&pSI)))
	{
		Application *pApp=NULL;
		pSI->get_Application(&pApp);
		AddTexturesToMaterial(aMaterial,aMSHMaterial,pApp);
		pSI->Release();
	}
*/
	OGLTexture *oglTexture;
	BSTR fileName;
	fileName=::SysAllocString(sBSTRInit);
	if(SUCCEEDED(aMaterial->get_OGLTexture(&oglTexture)))
	{
		if(oglTexture)
		{
			if(SUCCEEDED(oglTexture->get_FullName(&fileName)))
			{
				_bstr_t temp;
				temp=fileName;
				char *sTemp=(char *)temp;
				char *sDir=StripPath(sTemp);
				if(strlen(sDir)<strlen(sTemp))
				{
					char *namePtr=&sTemp[strlen(sDir)];
					aMSHMaterial->AddTexture(namePtr,sTemp);
				}
				free(sDir);
			}
			oglTexture->Release();
		}
	}
	::SysFreeString(fileName);  
}


enum AttribFlagValues
{
	ATTRIBFLAG_EMISSIVE				= 0x01,	// no lighting
	ATTRIBFLAG_GLOW					= 0x02, // glow
	ATTRIBFLAG_TRANSPARENT_SINGLE	= 0x04,	// single-sided transparency
	ATTRIBFLAG_TRANSPARENT_DOUBLE	= 0x08,	// double-sided transparency
	ATTRIBFLAG_TRANSPARENT_HARDEDGE	= 0x10,	// hard-edged transparency
	ATTRIBFLAG_PERPIXELLIGHTING		= 0x20,	// lighting is done per-pixel
	ATTRIBFLAG_TRANSPARENT_ADDITIVE	= 0x40,	// additive transparency
	ATTRIBFLAG_SPECULAR				= 0x80,	// has specular
};

unsigned int CScene2MSH::GetMaterialAttribute(X3DObject *aObject,char *aPropName)
{
	unsigned int retVal=0;
//	char matPropName[256];
	//	sprintf(matPropName,"MAT_%s",aClusterName);

	SceneItem *pSceneItem=NULL;
	if(SUCCEEDED(aObject->QueryInterface(IID_SceneItem,(void **)&pSceneItem)))
	{
		PropertyCollection *props;
		if(SUCCEEDED(pSceneItem->get_Properties(&props)))
		{
			_bstr_t sName(aPropName);
			VARIANT varName;
			varName.vt=VT_BSTR;
			varName.bstrVal=sName;
			Property *prop;
			if(SUCCEEDED(props->get_Item(varName,&prop)))
			{
				VARIANT val;
				val.vt=VT_UI1;
				val.bVal=0;
				GetParameter(prop,"Lighting",m_CurrentFrame,&val);
				if(val.bVal==1)
					retVal|=ATTRIBFLAG_EMISSIVE;
				if(val.bVal==2)
					retVal|=ATTRIBFLAG_GLOW;

				val.vt=VT_UI1;
				val.bVal=0;
				GetParameter(prop,"Transparency",m_CurrentFrame,&val);
				if(val.bVal==1)
					retVal|=ATTRIBFLAG_TRANSPARENT_SINGLE;
				if(val.bVal==2)
					retVal|=ATTRIBFLAG_TRANSPARENT_DOUBLE;

				val.vt=VT_BOOL;
				val.boolVal=VARIANT_FALSE;
				GetParameter(prop,"Hard_Edged_Transparency",m_CurrentFrame,&val);
				if(val.boolVal)
					retVal|=ATTRIBFLAG_TRANSPARENT_HARDEDGE;

				val.vt=VT_BOOL;
				val.boolVal=VARIANT_FALSE;
				GetParameter(prop,"PerPixelLightingEnable",m_CurrentFrame,&val);
				if(val.boolVal)
					retVal|=ATTRIBFLAG_PERPIXELLIGHTING;

				val.vt=VT_BOOL;
				val.boolVal=VARIANT_FALSE;
				GetParameter(prop,"SpecularEnable",m_CurrentFrame,&val);
				if(val.boolVal)
					retVal|=ATTRIBFLAG_SPECULAR;

				val.vt=VT_BOOL;
				val.boolVal=VARIANT_FALSE;
				GetParameter(prop,"Additive_Transparency",m_CurrentFrame,&val);
				if(val.boolVal)
					retVal|=ATTRIBFLAG_TRANSPARENT_ADDITIVE;
		
				val.vt=VT_UI1;
				GetParameter(prop,"Render_Types",m_CurrentFrame,&val);
				retVal|=val.bVal<<8;

				val.vt=VT_UI1;
				GetParameter(prop,"Data0",m_CurrentFrame,&val);
				if(val.bVal>0)
					retVal|=(val.bVal<<16);

				GetParameter(prop,"Data1",m_CurrentFrame,&val);
				if(val.bVal>0)
					retVal|=(val.bVal<<24);

				prop->Release();
			}
			props->Release();
		}
		pSceneItem->Release();
	}
	return retVal;
}

unsigned int CScene2MSH::GetMaterialTexture(X3DObject *aObject,char *aPropName, int idx)
{
	if(idx<1 || idx>3)
		return 0;

	unsigned int retVal=0;
//	char matPropName[256];
//	sprintf(matPropName,"MAT_%s",aClusterName);

	SceneItem *pSceneItem=NULL;
	if(SUCCEEDED(aObject->QueryInterface(IID_SceneItem,(void **)&pSceneItem)))
	{
		PropertyCollection *props;
		if(SUCCEEDED(pSceneItem->get_Properties(&props)))
		{
			_bstr_t sName(aPropName);
			VARIANT varName;
			varName.vt=VT_BSTR;
			varName.bstrVal=sName;
			Property *prop;
			if(SUCCEEDED(props->get_Item(varName,&prop)))
			{
				VARIANT varTexture1;
				varTexture1.vt=VT_BSTR;
				varTexture1.bstrVal=::SysAllocString(sBSTRInit);
				char parName[32];
				sprintf(parName,"Texture%d",idx);
				if(GetParameter(prop,parName,m_CurrentFrame,&varTexture1))
				{
					_bstr_t sTxt1(varTexture1.bstrVal,true);
					::SysFreeString(varTexture1.bstrVal);
					char *pos = sTxt1;
					retVal=CalcStrCRC(pos,0);
				}

				prop->Release();
			}
			props->Release();
		}
		pSceneItem->Release();
	}
	return retVal;
}

void CScene2MSH::AddATRBtoDefaultMaterials(X3DObject *aObject, CZeroData *aData,Material *aMat,PropertyCollection *aProperties,ObjClsAttrib *aKey)
{
	long idx,nCount;
	VARIANT varIdx;
	Property *pATRBProperty=NULL;
	//ClusterElementCollection *pATRBElems=NULL;
	
	aProperties->get_Count(&nCount);
	for(idx=0;idx<nCount;idx++)
	{
		varIdx.vt=VT_I4;
		varIdx.lVal=idx;
		aProperties->get_Item(varIdx,&pATRBProperty);
		char sPropName[256];
		GetSIObjectName(pATRBProperty,sPropName,256);
		if(strnicmp(sPropName,"MAT_ATRB",8)==0)	//	for each MAT_ATRB property ...
		{
			//	add a material based on aMat with attrib = MAT_ATRB property
			CMSHBlockMaterialSet *pMSHMatSet=aData->GetMaterialSet();
			CMSHBlockMaterial *pMat=pMSHMatSet->AddNewMaterial();
			CreateMSHMaterialFromXSIMaterial(pMSHMatSet,pMat,aMat);
			unsigned int atrb=GetMaterialAttribute(aObject,sPropName);
			pMat->SetAttribute(atrb);
			aKey->attrib=atrb;

			unsigned int txt1=GetMaterialTexture(aObject,sPropName, 1);
			aKey->txt1CRC=txt1;
			if(txt1)
			{
				int nTxts;
				char **txts=pMat->GetTextureFilenames(nTxts);
				if(nTxts<2)
				{
					if(nTxts==0)
						pMat->AddTexture("");

					char *sTemp=(char *)InvertCRC(txt1);
					char *sDir=StripPath(sTemp);
					if(strlen(sDir)<strlen(sTemp))
					{
						char *namePtr=&sTemp[strlen(sDir)];
						pMat->AddTexture(namePtr,sTemp);
					}
					free(sDir);
				}
			}

			unsigned int txt2=GetMaterialTexture(aObject,sPropName, 2);
			aKey->txt2CRC=txt2;
			if(txt2)
			{
				int nTxts;
				char **txts=pMat->GetTextureFilenames(nTxts);
				if(nTxts<3)
				{
					if(nTxts==0)
					{
						pMat->AddTexture("");
						pMat->AddTexture("");
					}
					if(nTxts==1)
					{
						pMat->AddTexture("");
					}

					char *sTemp=(char *)InvertCRC(txt2);
					char *sDir=StripPath(sTemp);
					if(strlen(sDir)<strlen(sTemp))
					{
						char *namePtr=&sTemp[strlen(sDir)];
						pMat->AddTexture(namePtr,sTemp);
					}
					free(sDir);
				}
			}

			unsigned int txt3=GetMaterialTexture(aObject,sPropName, 3);
			aKey->txt3CRC=txt3;
			if(txt3)
			{
				int nTxts;
				char **txts=pMat->GetTextureFilenames(nTxts);
				if(nTxts<4)
				{
					if(nTxts==0)
					{
						pMat->AddTexture("");
						pMat->AddTexture("");
						pMat->AddTexture("");
					}
					if(nTxts==1)
					{
						pMat->AddTexture("");
						pMat->AddTexture("");
					}
					if(nTxts==2)
					{
						pMat->AddTexture("");
					}

					char *sTemp=(char *)InvertCRC(txt3);
					char *sDir=StripPath(sTemp);
					if(strlen(sDir)<strlen(sTemp))
					{
						char *namePtr=&sTemp[strlen(sDir)];
						pMat->AddTexture(namePtr,sTemp);
					}
					free(sDir);
				}
			}

			char *sMatName=pMat->GetName();
			unsigned int matCRC=0;
			matCRC=CalcStrCRC(sMatName,matCRC);
			m_MaterialByObjClsAttrib.Insert(matCRC,*aKey);
		}

		if (pATRBProperty)
			pATRBProperty->Release();
	}
}

void CScene2MSH::AddMaterialsFromObject(CZeroData *aData, X3DObject *aObject,bool branchSelected)
{
	VARIANT_BOOL varSelect;
	varSelect=VARIANT_FALSE;
	ProjectItem *pProjItem;
	if(!branchSelected)
	{
		//
		//	this model is not in a selected branch
		//
		if(SUCCEEDED(aObject->QueryInterface(IID_ProjectItem,(void **)&pProjItem)))
		{
			//
			//	is this branch selected?
			//
			pProjItem->get_Selected(1,&varSelect);
			if(varSelect)
			{
				branchSelected=true;
			}
			else
			{
				//
				//	is this node selected?
				//
				pProjItem->get_Selected(0,&varSelect);
			}
			pProjItem->Release();
		}
	}
	else
	{
		//
		//	this model is in a selected branch
		//
		varSelect=VARIANT_TRUE;
	}


	Primitive *pPrim = 0;
	long idx;
	VARIANT varIdx;
	Cluster *pCluster = 0;
	SceneItem *pSceneItem = 0;
	PropertyCollection *modelProperties=NULL;
	Material *pMat = 0;
	CMSHBlockMaterial *pMSHMaterial;
	CMSHBlockMaterialSet *pMSHMaterialSet = aData->GetMaterialSet();
	long nCount=0;

	char objName[256];
	char msg[300];
	msg[0]=0;
	if(!mExportSelectedOnly || varSelect)
	{

		GetSIObjectName(aObject,objName,255);
		sprintf(msg,"Getting materials from %s",objName);
		m_ProgDlg->SetProgress(msg,0);
		
		ObjClsAttrib key;
		key.objNameCRC=0;
		key.objNameCRC = CalcStrCRC(objName,key.objNameCRC);

		Operator *envelopOp=NULL;
		//	get object's material
		if(SUCCEEDED(aObject->QueryInterface(IID_SceneItem,(void **)&pSceneItem)))
		{
			pMat = NULL;
			pSceneItem->get_Material(&pMat);
			pSceneItem->get_Properties(&modelProperties);
			if(pMat)
			{
				key.clsIdx=-1;
				key.attrib=0;
				key.txt1CRC=0;
				key.txt2CRC=0;
				key.txt3CRC=0;

				pMSHMaterial = pMSHMaterialSet ->AddNewMaterial();
				CreateMSHMaterialFromXSIMaterial(pMSHMaterialSet,pMSHMaterial,pMat);
				char *sMatName=pMSHMaterial->GetName();
				unsigned int matCRC=0;
				matCRC=CalcStrCRC(sMatName,matCRC);
				m_MaterialByObjClsAttrib.Insert(matCRC,key);

				if(SUCCEEDED(aObject->get_ActivePrimitive(&pPrim)))
				{
					long nCount=0;
					ClusterCollection *pClusters=NULL;
					Geometry *pGeometry=NULL;
					VARIANT var;
					var.vt=VT_I4;
					var.lVal=(int)m_CurrentFrame;

					pPrim->get_Geometry(var,&pGeometry);
					if(pGeometry)
					{
						pGeometry->get_Clusters(&pClusters);
						if(pClusters)
						{
	/* add materials with non-zero ATRB's here*/
							AddATRBtoDefaultMaterials(aObject,aData,pMat,modelProperties,&key);
						}
					}
				}
			}
			pSceneItem->Release();
		}

		m_ProgDlg->SetProgress(msg,10);
		//	get cluster materials
		if(SUCCEEDED(aObject->get_ActivePrimitive(&pPrim)))
		{
			ClusterCollection *pClusters=NULL;
			Geometry *pGeometry=NULL;
			VARIANT var;
			var.vt=VT_I4;
			var.lVal=(int)m_CurrentFrame;

			pPrim->get_Geometry(var,&pGeometry);
			if(pGeometry)
			{
				pGeometry->get_Clusters(&pClusters);
				if(pClusters)
				{
					pClusters->get_Count(&nCount);
					for(idx=0;idx<nCount;idx++)
					{
						m_ProgDlg->SetProgress(msg,10+90*(idx+1)/nCount);
						varIdx.vt=VT_I4;
						varIdx.lVal=idx;
						pClusters->get_Item(varIdx,&pCluster);
						char sClusterName[256];
						GetSIObjectName(pCluster,sClusterName,256);

						if(SUCCEEDED(pCluster->QueryInterface(IID_SceneItem,(void **)&pSceneItem)))
						{
							if(SUCCEEDED(pSceneItem->get_Material(&pMat)) && pMat != NULL)
							{
								pMSHMaterial = pMSHMaterialSet ->AddNewMaterial();
								CreateMSHMaterialFromXSIMaterial(pMSHMaterialSet,pMSHMaterial,pMat);
								key.clsIdx=idx;
								key.attrib=0;
								key.txt1CRC=0;
								key.txt2CRC=0;
								key.txt3CRC=0;
								char *sMatName=pMSHMaterial->GetName();
								unsigned int matCRC=0;
								matCRC=CalcStrCRC(sMatName,matCRC);
								m_MaterialByObjClsAttrib.Insert(matCRC,key);

								ClusterElementCollection *pElements;
								if(SUCCEEDED(pCluster->get_Elements(&pElements)))
								{
									if(pElements)
									{
										AddATRBtoDefaultMaterials(aObject,aData,pMat,modelProperties,&key);
										pElements->Release();
									}
								}
							}
							pSceneItem->Release();
						}

						if (pCluster)
							pCluster->Release();
					}

					pClusters->Release();
				}

				pGeometry->Release();
			}
		}
	}

	if (pMat)
		pMat->Release();

	if (modelProperties)
		modelProperties->Release();

	m_ProgDlg->SetProgress(msg,100);
	//	get materials from children
	X3DObjectCollection *children;
	aObject->get_Children(&children);
	children->get_Count(&nCount);
	X3DObject *child;
	varIdx.vt=VT_I4;
	for(idx=0;idx<nCount;idx++)
	{
		varIdx.lVal=idx;
		children->get_Item(varIdx,&child);
		AddMaterialsFromObject(aData,child,branchSelected);
	}
}

void CScene2MSH::RemoveRedundantMaterials()
{
	CMSHBlockMaterialSet *pMSHMatSet = mZeroData->GetMaterialSet();
	if(!pMSHMatSet)
		return;
	unsigned int nMaterials;
	CMSHBlockMaterial **ppMaterials=pMSHMatSet->GetMaterials(nMaterials);
	if(!nMaterials)
		return;

	unsigned int i,j;
	for(i=0;i<nMaterials;i++)
	{
		CMSHBlockMaterial *pTest1 = ppMaterials[i];
		if(pTest1)
		{
			for(j=i+1;j<nMaterials;j++)
			{
				CMSHBlockMaterial *pTest2 = ppMaterials[j];
				if(pTest2)
				{
					if(pMSHMatSet->CompareMaterials(pTest1,pTest2))
					{
						char *sMatName1 = pTest1->GetName();
						unsigned int crc1=0;
						crc1=CalcStrCRC(sMatName1,crc1);
						char *sMatName2 = pTest2->GetName();
						unsigned int crc2=0;
						crc2=CalcStrCRC(sMatName2,crc2);
						m_NewMaterialFromOldMaterial.Insert(crc1,crc2);
						delete pTest2;
						ppMaterials[j] = NULL;
					}
				}
			}
		}
	}

	for(i=0;i<nMaterials;i++)
	{
		if(ppMaterials[i])
		{
			char *sMatName = ppMaterials[i]->GetName();
			unsigned int crc=0;
			crc=CalcStrCRC(sMatName,crc);
			Constraints::ZeroTreeNode *node=m_NewMaterialFromOldMaterial.Find(crc);
			if(!node)
			{
				m_NewMaterialFromOldMaterial.Insert(crc,crc);
			}
		}
	}

	pMSHMatSet->RemoveDuplicates();
}

void CScene2MSH::ConvertTextureFile(char *srcPath, char *dstDir, CMSHBlockMaterial *pMSHMat)
{
	bool bJustCopy=false;

	char sdrive[_MAX_DRIVE];
	char sdir[_MAX_DIR];
	char sfname[_MAX_FNAME];
	char sext[_MAX_EXT];
	char dext[_MAX_EXT];

	_splitpath( srcPath, sdrive, sdir, sfname, sext );

	switch(m_TextureFormat)
	{
	case 1:sprintf(dext,".tga");break; // TARGA
	case 2:sprintf(dext,".pic");break; // PIC
	}

	if(strcmpi(dext,sext)==0)
		bJustCopy=true;

	char dstPath[_MAX_PATH];
	_makepath(dstPath,"",dstDir,sfname,dext);

	if(bJustCopy)
	{
		CopyFile(srcPath,dstPath,FALSE);
	}
	else
	{
		int width=0;
		int height=0;

		bool transparent;

		// get bitmap size
		if(strcmpi(sext,".tga")==0)
		{
			TGA_BITMAP::GetDimension(srcPath, width, height);
		}
		else
		{
			if(strcmpi(sext,".pic")==0)
			{
				PIC_BITMAP::GetDimension(srcPath, width, height);
			}
		}

		// allocate a temporary buffer
		ZeroStack<ZeroColor> pixelArray(width * height);

		// create the pixel format
		ZeroPixelFormat pixelFormat;
		pixelFormat.bitCount = 32;
		pixelFormat.alpha.mask = 0xff000000;
		pixelFormat.red.mask   = 0x00ff0000;
		pixelFormat.green.mask = 0x0000ff00;
		pixelFormat.blue.mask  = 0x000000ff;

		// read the file
		if(strcmpi(sext,".tga")==0)
		{
			TGA_BITMAP::Load(srcPath,(unsigned char *)&pixelArray[0],width * sizeof(ZeroColor),width, height, pixelFormat, transparent);
		}
		else
		{
			if(strcmpi(sext,".pic")==0)
			{
				PIC_BITMAP::Load(srcPath,(unsigned char *)&pixelArray[0],width * sizeof(ZeroColor), width, height, pixelFormat, transparent);
			}
		}

		// write the file
		switch(m_TextureFormat)
		{
		case 1:TGA_BITMAP::Write(dstPath,(unsigned char *)&pixelArray[0],width * sizeof(ZeroColor),width, height, pixelFormat);break; // TARGA
		case 2:PIC_BITMAP::Write(dstPath,(unsigned char *)&pixelArray[0],width * sizeof(ZeroColor),width, height, pixelFormat);break; // PIC
		}

		//	update MSHMaterial with new extension
		char dfname[_MAX_FNAME];
		sprintf(dfname,"%s%s",sfname,dext);
		pMSHMat->ReplaceTexture(dfname);
	}
}

void CScene2MSH::CreateMaterials(CZeroData *aData,Scene *aScene)
{
	Model *pRootModel;

	CMSHBlockMaterialSet *pMSHMaterialSet;
	aData ->CreateNewMaterialSet();
	if(SUCCEEDED(aScene ->get_Root(&pRootModel)))
	{
		X3DObject *p3DObject;
		if(SUCCEEDED(pRootModel->QueryInterface(IID_X3DObject,(void **)&p3DObject)))
		{
			AddMaterialsFromObject(aData,p3DObject,false);
			p3DObject->Release();
		}
	}

	pMSHMaterialSet = aData ->GetMaterialSet();
	RemoveRedundantMaterials();

	if(mExportTextures)
	{
		unsigned int nMaterials;
		CMSHBlockMaterial **ppMaterials = pMSHMaterialSet->GetMaterials(nMaterials);
		for(unsigned int i =0;i<nMaterials;i++)
		{
			char msg[256];
			sprintf(msg,"Writing Texture File (%s)",ppMaterials[i]->GetName());
			m_ProgDlg->SetProgress(msg,100*(i+1)/nMaterials);
			int nTextures;
			char **src=ppMaterials[i]->GetTextureSourcePaths(nTextures);
			if(src)
			{
				if(m_sDestinationPath)
				{
					char **ppTextures = ppMaterials[i]->GetTextureFilenames(nTextures);
					if(nTextures)
					{
						for(int j=0;j<nTextures;j++)
						{
							char *sDir=StripPath(m_sDestinationPath);
							char *sDst=new char[strlen(sDir)+strlen(ppTextures[j])+1];
							sprintf(sDst,"%s%s",sDir,ppTextures[j]);
							if(m_TextureFormat==0)
								CopyFile(src[j],sDst,FALSE);
							else
							{
								ConvertTextureFile(src[j],sDir,ppMaterials[i]);
							}

							unsigned int matAttrib=ppMaterials[i]->GetAttribute();
							if(((matAttrib &0x0000FF00)>>8)==7) // if UV animation
							{
								int nFrames=(matAttrib &0xFF000000)>>24;
								for (int k = 1; k <= nFrames; k++)
								{
									// generate the texture name
									char srcpath[_MAX_PATH];
									char srcext[_MAX_PATH];
									sprintf(srcpath, "%s", src[j]);
									sprintf(srcext, "%s", &srcpath[strlen(srcpath)-4]);
									sprintf(&srcpath[strlen(srcpath)-4],"%d",k);
									strcat(srcpath,srcext);

									char dstpath[_MAX_PATH];
									char dstext[_MAX_PATH];
									sprintf(dstpath, "%s%s",sDir,ppTextures[j]);
									sprintf(dstext, "%s", &dstpath[strlen(dstpath)-4]);
									sprintf(&dstpath[strlen(dstpath)-4],"%d",k);
									strcat(dstpath,dstext);
		
									if(m_TextureFormat==0)
										CopyFile(srcpath,dstpath,FALSE);
									else
									{
										ConvertTextureFile(src[j],sDir,ppMaterials[i]);
									}
								}
							}
							delete[] sDst;
							free(sDir);
						}
					}
				}
			}
		}
		m_ProgDlg->SetProgress("Writing Texture Files",100);
	}
}

void CScene2MSH::SetBaseLightInfo(CMSHBlockLight *aMSHLight, Light *aLight, OGLLight *aOGLLight)
{
	// LIGHT NAME
	VARIANT name;
	name.vt=VT_BSTR;
	name.bstrVal=::SysAllocString(sBSTRInit);
	GetParameter(aLight,"name",m_CurrentFrame,&name);
	_bstr_t tempName(name.bstrVal,true);
	::SysFreeString (name.bstrVal);

	aMSHLight->SetName((char *)tempName);

	// LIGHT COLOR
	Color *col;
	if(SUCCEEDED(aOGLLight->get_Color(&col)))
	{
		if(col)
		{
			double r,g,b,a;
			col->get_Red(&r);
			col->get_Green(&g);
			col->get_Blue(&b);
			col->get_Alpha(&a);
			aMSHLight->SetColor((float)r,(float)g,(float)b,(float)a);
		}
	}

}

void CScene2MSH::CreatePointLight(CZeroData *pData, Light *aLight, OGLLight *aOGLLight)
{
	CMSHBlockPointLight *pMSHLight = pData ->AddPointLight();

	SetBaseLightInfo(pMSHLight,aLight,aOGLLight);
	
	// LIGHT_POSITION
	vect3 vPos;
	GetGlobalPosition(aLight,vPos);
	pMSHLight ->SetPosition(vPos.fX,vPos.fY,vPos.fZ);

	// LIGHT_FALLOFF
	ShaderCollection *pShaders;
	if(SUCCEEDED(aLight->get_Shaders(&pShaders)))
	{
		VARIANT shIdx;
		_bstr_t bstrTemp="soft_light";
		shIdx.vt=VT_BSTR;
		shIdx.bstrVal=bstrTemp; 
		Shader *pShader;
		if(SUCCEEDED(pShaders->get_Item(shIdx,&pShader)))
		{
			VARIANT varStart,varStop;
			varStart.vt=varStop.vt=VT_R8;
			GetParameter(pShader,"start",m_CurrentFrame,&varStart);
			GetParameter(pShader,"stop",m_CurrentFrame,&varStop);
			pMSHLight ->SetFalloff((float)varStart.dblVal,(float)varStop.dblVal);
		}
	}
}

void CScene2MSH::CreateSpotLight(CZeroData *pData, Light *aLight, OGLLight *aOGLLight)
{
	CMSHBlockSpotLight *pMSHLight = pData ->AddSpotLight();
	SetBaseLightInfo(pMSHLight,aLight,aOGLLight);

	// LIGHT POSITION
	vect3 vPos;
	GetGlobalPosition(aLight,vPos);
	pMSHLight ->SetPosition(vPos.fX,vPos.fY,vPos.fZ);

	// LIGHT INTEREST
	DirectedObject *pDir;
	if(SUCCEEDED(aLight->QueryInterface(IID_DirectedObject,(void **)&pDir)))
	{
		X3DObject *pInterest;
		if(SUCCEEDED(pDir->get_Interest(&pInterest)))
		{
			if(pInterest)
			{
				GetGlobalPosition(pInterest,vPos);
				pMSHLight ->SetInterest(vPos.fX,vPos.fY,vPos.fZ);
			}
		}
		pDir->Release();
	}


	// LIGHT FALLOFF and SPREAD ANGLE
	ShaderCollection *pShaders;
	if(SUCCEEDED(aLight->get_Shaders(&pShaders)))
	{
		VARIANT shIdx;
		_bstr_t bstrTemp="soft_light";
		shIdx.vt=VT_BSTR;
		shIdx.bstrVal=bstrTemp; 
		Shader *pShader;
		if(SUCCEEDED(pShaders->get_Item(shIdx,&pShader)))
		{
			VARIANT varStart,varStop,varSpread;
			varStart.vt=varStop.vt=varSpread.vt=VT_R8;
			GetParameter(pShader,"start",m_CurrentFrame,&varStart);
			GetParameter(pShader,"stop",m_CurrentFrame,&varStop);
			GetParameter(pShader,"spread",m_CurrentFrame,&varSpread);
			pMSHLight ->SetFalloff((float)varStart.dblVal,(float)varStop.dblVal);
			pMSHLight->SetSpreadAngle(DEG2RAD*(float)varSpread.dblVal);
		}
	}

	// LIGHT CONE ANGLE
	VARIANT varCone;
	varCone.vt=VT_R8;
	GetParameter(aLight,"LightCone",m_CurrentFrame,&varCone);
	pMSHLight ->SetConeAngle(DEG2RAD*(float)varCone.dblVal);
	
}

void CScene2MSH::CreateInfiniteLight(CZeroData *pData, Light *aLight, OGLLight *aOGLLight)
{
	// LIGHT NAME
	VARIANT name;
	name.vt=VT_BSTR;
	name.bstrVal=::SysAllocString(sBSTRInit);
	GetParameter(aLight,"name",m_CurrentFrame,&name);
	_bstr_t tempName(name.bstrVal,true);
	::SysFreeString (name.bstrVal);  

	if(strcmp(tempName,"light")==0)	//	don't create the light if it's the default hidden light created by XSI
	{
		Property *visProp;
		GetPropertyByType(aLight,"visibility",&visProp);
		if(visProp)
		{
			VARIANT varVisible;
			varVisible.vt=VT_BOOL;
			GetParameter(visProp,"viewvis",m_CurrentFrame,&varVisible);
			if(!varVisible.boolVal)
			{
				return;
			}
		}
	}

	CMSHBlockInfiniteLight *pMSHLight = pData ->AddInfiniteLight();
	SetBaseLightInfo(pMSHLight,aLight,aOGLLight);

	// LIGHT DIRECTION
	Kinematics *kine=NULL;
	X3DObject *pX3DObj;
	if(SUCCEEDED(aLight->QueryInterface(IID_X3DObject,(void **)&pX3DObj)))
	{
		if(SUCCEEDED(pX3DObj->get_Kinematics(&kine)))
		{
			IDispatch *pDisp;
			if(SUCCEEDED(kine->get_Global(&pDisp)))
			{
				Parameter *pKineState;
				if(SUCCEEDED(pDisp->QueryInterface(IID_Parameter,(void **)&pKineState)))
				{
					ISIRotation *pRotation;
					if(SUCCEEDED(CoCreateInstance(CLSID_SIRotation, NULL, CLSCTX_INPROC, IID_ISIRotation, (void **)&pRotation)))
					{
						VARIANT rotX,rotY,rotZ;
						rotX.vt=rotY.vt=rotZ.vt=VT_R8;
						GetParameter(pKineState,"rotx",m_CurrentFrame,&rotX);
						GetParameter(pKineState,"roty",m_CurrentFrame,&rotY);
						GetParameter(pKineState,"rotz",m_CurrentFrame,&rotZ);
						rotX.dblVal=rotX.dblVal*DEG2RAD;
						rotY.dblVal=rotY.dblVal*DEG2RAD;
						rotZ.dblVal=rotZ.dblVal*DEG2RAD;
						ISIVector3 *pDir;
						pRotation->SetFromXYZAnglesValues(rotX.dblVal,rotY.dblVal,rotZ.dblVal);
						if(SUCCEEDED(CoCreateInstance(CLSID_SIVector3, NULL, CLSCTX_INPROC, IID_ISIVector3, (void **)&pDir)))
						{
							double dirX,dirY,dirZ;
							pDir->Set(0.0,0.0,-1.0);
							pDir->MulByRotationInPlace(pRotation);
							pDir->get_X(&dirX);
							pDir->get_Y(&dirY);
							pDir->get_Z(&dirZ);
							pMSHLight->SetDirection((float)dirX, (float)dirY, (float)dirZ);
							pDir->Release();
						}
						pRotation->Release();
					}
					pKineState->Release();
				}
			}
		}
		pX3DObj->Release();
	}
}

void CScene2MSH::CreateLight(CZeroData *aData,Light *aLight)
{
	OGLLight *oglLight;
	if(SUCCEEDED(aLight->get_OGLLight(&oglLight)))
	{
		if(oglLight)
		{
			siLightType lgtType;
			oglLight->get_Type(&lgtType);
			switch(lgtType)
			{
			case siLightPoint:
				CreatePointLight(aData,aLight,oglLight);
				break;
			case siLightSpot:
				CreateSpotLight(aData,aLight,oglLight);
				break;
			case siLightInfinite:
				CreateInfiniteLight(aData,aLight,oglLight);
				break;
			}
		}
	}
}

const char *CScene2MSH::GetNewMaterialName(CZeroData *pData, ObjClsAttrib *aKey)
{
	MaterialByObjClsAttrib::ZeroTreeNode *node=m_MaterialByObjClsAttrib.Find(*aKey);
	if(node)
	{
		unsigned int oldCRC=node->info;
		unsigned int newCRC;

		Constraints::ZeroTreeNode *newNode = m_NewMaterialFromOldMaterial.Find(oldCRC);
		if(newNode)
		{
			newCRC=newNode->info;
			return InvertCRC(newCRC);
		}
		else
			return InvertCRC(oldCRC);
	}
	return NULL;
}

void CScene2MSH::CreateSkinBoundingBoxes(CZeroData *pData)
{
/*
	int nModels;
	CMSHBlockModel **ppModels;
	ppModels = pData ->GetModels(nModels);
	for(int i =0;i<nModels;i++)
	{
		int nBones;
		int *pBoneIDs;
		pBoneIDs = ppModels[i] ->GetBoneIds(nBones);

		int nSegments;
		CMSHBlockModelSegment **ppSegments;
		ppSegments = ppModels[i] ->GetSegments(nSegments);
		if(nSegments==1)
		{
			if(nBones)
			{
				for(int j =0;j<nBones;j++)
				{
					CMSHBlockModel *pBone=GetModelByID(pData,pBoneIDs[i]);
					if(pBone)
						CreateBoneBoundingBox(pBone, ppSegments[0]);
				}
			}
		}
	}

*/
}


void CScene2MSH::CreateModels(
    CZeroData *pData,
    Scene *pScene, 
    char *pathFiles)
{
	Model *pRootModel;
    mDatabase = new (false)ZeroImportExportNode (NULL, NULL);
	if(SUCCEEDED(pScene ->get_Root(&pRootModel)))
	{
		X3DObject *p3DObject;
		if(SUCCEEDED(pRootModel->QueryInterface(IID_X3DObject,(void **)&p3DObject)))
		{
//			AddAllConstraints(p3DObject);
			m_AllModels.AddAllModels(p3DObject);
			X3DObjectCollection *children;
			p3DObject->get_Children(&children);
			long count;
			if(SUCCEEDED(children->get_Count(&count)))
			{
				VARIANT index;
				X3DObject *child;
				index.vt=VT_I4;
				for(long i=0;i<count;i++)
				{
					index.lVal=i;

					if(SUCCEEDED(children->get_Item(index,&child)))
					{
						CreateHierarchy(pData, child,NULL,0,mDatabase,pScene,false);

						child->Release();
					}
				}

				children->Release();
			}
			p3DObject->Release();
		}
	}
}

void CScene2MSH::CreateHierarchy (CZeroData *pData, X3DObject *pModel,X3DObject *pParent,int level,ZeroImportExportNode* database,Scene *aScene,bool branchSelected)
{
	ZeroImportExportNode *pParentNode=database;
	VARIANT_BOOL varSelect=VARIANT_FALSE;
	ProjectItem *pProjItem;
	if(!branchSelected)
	{
		//
		//	this model is not in a selected branch
		//
		if(SUCCEEDED(pModel->QueryInterface(IID_ProjectItem,(void **)&pProjItem)))
		{
			//
			//	is this branch selected?
			//
			pProjItem->get_Selected(1,&varSelect);
			if(varSelect)
			{
				branchSelected=true;
			}
			else
			{
				//
				//	is this node selected?
				//
				pProjItem->get_Selected(0,&varSelect);
			}
			pProjItem->Release();
		}
	}
	else
	{
		//
		//	this model is in a selected branch
		//
		varSelect=VARIANT_TRUE;
	}

	Primitive *pPrim;
	CameraRig *pCameraRig1=NULL;
	Light *pLight=NULL;

	//
	//	CAMERA
	//
	if(SUCCEEDED(pModel->QueryInterface(IID_CameraRig,(void **)&pCameraRig1)))
	{
		Camera *pCamera=NULL;
		// NOTE: In XSI 3.0, apparently you can get a camera rig without a camera. --DY
		if(SUCCEEDED(pCameraRig1->get_Camera(&pCamera)) && pCamera != NULL)
		{
			CreateCamera(pData, pCamera);
		}
		pCameraRig1->Release();
	}
	else 
	{
		//
		//	LIGHT
		//
		if(SUCCEEDED(pModel->QueryInterface(IID_Light,(void **)&pLight)))
		{
			if(mExportLights)
				CreateLight(pData,pLight);
			pLight->Release();
		}
		else
		{
			CameraRig *pCameraRig2=NULL;
			bool bCreate=true;
			if(pParent)
			{
				//
				//	Don't create camera interests
				//
				if(SUCCEEDED(pParent->QueryInterface(IID_CameraRig,(void **)&pCameraRig2)))
				{
					if(pCameraRig2)
						pCameraRig2->Release();
					bCreate=false;
				}
			}
			/*
			only export if not named "Texture_SupportXXX"
			*/
			SIObject *pSIModel=NULL;
			if(SUCCEEDED(pModel->QueryInterface(IID_SIObject,(void **)&pSIModel)))
			{
				VARIANT name;
				name.vt=VT_BSTR;
				name.bstrVal=::SysAllocString(sBSTRInit);
				pSIModel->get_FullName(&name.bstrVal);
				_bstr_t tempName(name.bstrVal,true);
				if(strnicmp(tempName,"Texture_Support",15)==0)
					bCreate=false;
				::SysFreeString (name.bstrVal);  
			}

			if(bCreate)
			{

				Null *pNull;
				//
				//	NULL
				//
				if(SUCCEEDED(pModel->QueryInterface(IID_Null,(void **)&pNull)))
				{
					if(!mExportSelectedOnly || varSelect)
					{
						pParentNode=CreateModel(pData,pModel, NULL,pParent,database,aScene);
					}
					pNull->Release();
				}
				else
				{
					Operator *envelopOp=NULL;
					VARIANT varEnvelop;
					Geometry *pGeometry=NULL;
					if(SUCCEEDED(pModel->get_ActivePrimitive(&pPrim)))
					{
						ConstructionHistory *constHistory=NULL;
						pPrim->get_ConstructionHistory(&constHistory);
						if(constHistory)
						{
							VARIANT varEmpty;
							varEmpty.vt=VT_EMPTY;
							OperatorCollection *envelopOps=NULL;
							_bstr_t bsEnvelopOp="envelopop";
							_bstr_t bsEmpty="";
							constHistory->Filter(bsEnvelopOp,varEmpty,bsEmpty,&envelopOps);
							if(envelopOps)
							{
								long nEnvOps;
								envelopOps->get_Count(&nEnvOps);
								if(nEnvOps>0)
								{
									VARIANT varIdx;
									varIdx.vt=VT_I4;
									varIdx.lVal=0;
									envelopOps->get_Item(varIdx,&envelopOp);
									if(envelopOp && !mSkipBasepose)
									{
										VARIANT varTemp;

										varEnvelop.vt=varTemp.vt=VT_BOOL;
										varTemp.boolVal=VARIANT_TRUE;
										GetParameter(envelopOp,"Mute",m_CurrentFrame,&varEnvelop);
										if(!mBaseposeFromCurrentFrame)
											SetParameter(envelopOp,"Mute",m_CurrentFrame,&varTemp);
									}
								}
							}
						}
						VARIANT var;
						var.vt=VT_I4;
						var.lVal=(int)m_CurrentFrame;

						pPrim->get_Geometry(var,&pGeometry);
					}
					if(!mExportSelectedOnly || varSelect)
						pParentNode = CreateModel(pData,pModel, pGeometry,pParent,database,aScene);
					if(envelopOp && !mSkipBasepose)
						SetParameter(envelopOp,"Mute",m_CurrentFrame,&varEnvelop);
				}
			}
		}
	}


	X3DObjectCollection *children = 0;
	pModel->get_Children(&children);
	long count;
	if(SUCCEEDED(children->get_Count(&count)))
	{
		VARIANT index;
		X3DObject *child;
		index.vt=VT_I4;
		for(long i=0;i<count;i++)
		{
			index.lVal=i;

			if(SUCCEEDED(children->get_Item(index,&child)))
			{
				CreateHierarchy(pData, child,pModel,level+1, pParentNode,aScene,branchSelected);

				child->Release();
			}
		}
	}

	if (children)
		children->Release();
}

void CScene2MSH::GetSIObjectName(IDispatch *aDisp, char *aName, int aNameSize)
{
	SIObject *obj;
	strcpy(aName,"");
	if(SUCCEEDED(aDisp->QueryInterface(IID_SIObject,(void **)&obj)))
	{
		BSTR bName;
		_bstr_t bstrName;
		obj->get_Name(&bName);
		bstrName=bName;
		strncpy(aName,(char *)bstrName, aNameSize);
		obj->Release();
	}
}

void CScene2MSH::GetSIObjectType(IDispatch *aDisp, char *aType, int aTypeSize)
{
	SIObject *obj;
	strcpy(aType,"");
	if(SUCCEEDED(aDisp->QueryInterface(IID_SIObject,(void **)&obj)))
	{
		BSTR bType;
		_bstr_t bstrType;
		obj->get_Type(&bType);
		bstrType=bType;
		strncpy(aType,(char *)bstrType, aTypeSize);
		obj->Release();
	}
}

unsigned int CScene2MSH::GetNumVertices(PolygonFaceCollection *aPolys, VARIANT &varElem)
{
	long nVertices=0;
	PolygonFace *polyFace=NULL;
	aPolys->get_Item(varElem,&polyFace);
	
	if(polyFace)
	{
		PolygonNodeCollection *polyNodes=NULL;
		polyFace->get_Nodes(&polyNodes);
		
		if(polyNodes)
		{
			polyNodes->get_Count(&nVertices);

			polyNodes->Release();
		}
		
		polyFace->Release();
	}

	return (unsigned int)nVertices;
}

void CScene2MSH::GetPolygonData(
	FacetCollection *aFacets, 
	PolygonFaceCollection *aPolys, 
	TriangleCollection *aTris, 
	VARIANT &varElem, 
	MSH_FLATVERTEX **vertexPtr, 
	ClusterElementCollection *aUV0s,
	ClusterElementCollection *aUV1s,
	long *aPolyTriStart,
	bool &aDoColors, 
	X3DObjectCollection *aDeformers, 
	VARIANT *aWeights, 
	ClusterElementCollection *aClothFixPointCluster,
	ClusterElementCollection **aUserClothSprings,
	int numUserClothSprings,
	bool *aShowWeightsWarning)
{
	PolygonFace *polyFace;
	Facet *facet;
	long iNodeIdx;
	long nDeformers=0;
	HRESULT hrTest;

	if(aDeformers)	{
		aDeformers->get_Count(&nDeformers);
	}

	if(SUCCEEDED(aFacets->get_Item(varElem,&facet)) && SUCCEEDED(aPolys->get_Item(varElem,&polyFace)))
	{

		// save the starting index
		PolygonNodeCollection *polyNodes;
		PointCollection *facetPoints;

		// for each vertex...
		if(SUCCEEDED(facet->get_Points(&facetPoints)) && SUCCEEDED(polyFace->get_Nodes(&polyNodes)))
		{
			long nVertices;
			if(SUCCEEDED(polyNodes->get_Count(&nVertices)))
			{
				for (iNodeIdx=0;iNodeIdx<nVertices;iNodeIdx++)
				{
					VARIANT varNodeIdx;
					varNodeIdx.vt=VT_I4;
					varNodeIdx.lVal=iNodeIdx;

					PolygonNode *polyNode;
					Point *point;
					
					memset (*vertexPtr, 0, sizeof (MSH_FLATVERTEX));
					if(SUCCEEDED(facetPoints->get_Item(varNodeIdx,&point)) && SUCCEEDED(polyNodes->get_Item(varNodeIdx,&polyNode)))
					{
						IDispatch *pDisp;
						ISIVector3 *pos;
						ISIVector3 *norm;
						//
						//	Get Position
						//
						if(SUCCEEDED(point->get_Position(&pDisp)))
						{
							if(SUCCEEDED(pDisp->QueryInterface(IID_ISIVector3,(void **)&pos)))
							{	
								double tempVal;
								pos->get_X(&tempVal);
								(*vertexPtr)->vertex.x=(float)tempVal;
								pos->get_Y(&tempVal);
								(*vertexPtr)->vertex.y=(float)tempVal;
								pos->get_Z(&tempVal);
								(*vertexPtr)->vertex.z=(float)tempVal;
								pos->Release();
							}

							pDisp->Release();
							pDisp = 0;
						}



						if(aClothFixPointCluster)
						{
							VARIANT varClothIdx;
							varClothIdx.vt=VT_I4;
							varClothIdx.lVal=-1;
							point->get_Index(&varClothIdx.lVal);
							long retClothIdx;
							aClothFixPointCluster->FindIndex(varClothIdx,&retClothIdx);
							if(retClothIdx>-1) {
								(*vertexPtr)->clothVertexFlags |= CLOTH_FIX_POINT;
							}
						}

						(*vertexPtr)->numUserCloth = 0;
						if(aUserClothSprings) // search and attempt to make it a user connection
						{
							bool markedDupe = false;
							for(int mz = 0; mz < numUserClothSprings && (*vertexPtr)->numUserCloth < MSH_FLATVERTEX::MAX_USER_CLOTH; mz++)
							{
								VARIANT varClothIdx;
								varClothIdx.vt=VT_I4;
								varClothIdx.lVal=-1;
								point->get_Index(&varClothIdx.lVal);
								long retClothIdx;
								aUserClothSprings[mz]->FindIndex(varClothIdx,&retClothIdx);
								if(retClothIdx>-1) 
								{
									(*vertexPtr)->userClothNumbers[(*vertexPtr)->numUserCloth] = mz;
									(*vertexPtr)->numUserCloth++;
								}
							}
							// debug output - Mike Z
							//if((*vertexPtr)->numUserCloth > 1)
							//{
							//	char msg[50];
							//	sprintf(msg, "Vert has %d", (*vertexPtr)->numUserCloth);
							//	::MessageBox(NULL, msg, "Vert", 0);
							//}
						}


						if (nDeformers) 
						{
							int i,wgtIdx;
							long ptIdx=-1;
							long getIdx[2];
							point->get_Index(&ptIdx);							
							wgtIdx = 0;
							VARIANT varWgtValue;
							varWgtValue.vt=VT_R8;
							getIdx[1]=ptIdx;
							for (i=0;i<nDeformers;i++) 
							{
								getIdx[0]=i;
								SafeArrayGetElement(aWeights->parray,getIdx,&varWgtValue);
								if(fabs(varWgtValue.dblVal)>0.0000001)
								{
									if (wgtIdx < MAX_WEIGHT_COUNT) 
									{
										VARIANT varDefIdx;
										varDefIdx.vt=VT_I4;
										varDefIdx.lVal=i;
										X3DObject *pDeformer=NULL;
										aDeformers->get_Item(varDefIdx,&pDeformer);
										float weightValue=(float)varWgtValue.dblVal*0.01f;

										SIObject *pSIDeformer=NULL;
										if(SUCCEEDED(pDeformer->QueryInterface(IID_SIObject,(void **)&pSIDeformer)))
										{
											VARIANT name;
											name.vt=VT_BSTR;
											name.bstrVal=::SysAllocString(sBSTRInit);
											pSIDeformer->get_FullName(&name.bstrVal);
											_bstr_t tempName(name.bstrVal,true);
											unsigned int crc=0;
											crc=CalcStrCRC(tempName,crc);
											::SysFreeString (name.bstrVal);  

											(*vertexPtr)->weightId[wgtIdx] = crc; 
											(*vertexPtr)->vertex.weight[wgtIdx] = weightValue; 
											wgtIdx ++; 
											_ASSERTE (wgtIdx < MAX_WEIGHT_COUNT);

											pSIDeformer->Release();
										}

										if (pDeformer)
											pDeformer->Release();
									}
								}
							}
							if(wgtIdx==0)
							{
								(*vertexPtr)->weightId[0] = 0; 
								(*vertexPtr)->vertex.weight[0] = 1.0; 
								*aShowWeightsWarning=true;
							}
						}

						//
						//	Get Normal
						//
						VARIANT_BOOL outValid=true;

						if(SUCCEEDED(polyNode->get_Normal(&outValid,&pDisp)))
						{
							if(SUCCEEDED(pDisp->QueryInterface(IID_ISIVector3,(void **)&norm)))
							{	
								double tempVal;
								norm->get_X(&tempVal);
								(*vertexPtr)->vertex.nx=(float)tempVal;
								norm->get_Y(&tempVal);
								(*vertexPtr)->vertex.ny=(float)tempVal;
								norm->get_Z(&tempVal);
								(*vertexPtr)->vertex.nz=(float)tempVal;
								norm->Release();
							}

							pDisp->Release();
						}

						//
						//	Get UV and VertColor
						//
						VARIANT uvIndex;
						uvIndex.vt=VT_I4;
						uvIndex.lVal=-1;
						float UVCacheArray[3][2];
						bool bGotTriUVArray = false;

						VARIANT triIndex;
						triIndex.vt=VT_I4;
						triIndex.lVal=aPolyTriStart[varElem.lVal]; // this is the 1st triangle in polygon #varElem.lVal
						while(uvIndex.lVal==-1)
						{
							Triangle *tri=NULL;
							aTris->get_Item(triIndex,&tri);
							if(tri)
							{
								long triPolyIndex;
								tri->get_PolygonIndex(&triPolyIndex);
								vect3 tvP,tvN;
								tvP.fX=tvP.fY=tvP.fZ=tvN.fX=tvN.fY=tvN.fZ=0.f;
								if(triPolyIndex==varElem.lVal) // if triangle is part of polygon #varElem.lVal
								{
									TrianglePointCollection *triPoints;
									tri->get_Points(&triPoints);
									int triPointIndex;
									VARIANT varTriPointIndex;
									varTriPointIndex.vt=VT_I4;
									
									// for XSI 3.0 -- get the UVArray and compare it --DY
									// (this is a workaround for an apparent bug in TriPoint->get_UV() in
									// XSI 3.0 beta RC2.  If it gets fixed, we can remove this)
									VARIANT varTestUVArray; 
									varTestUVArray.vt=VT_ARRAY | VT_VARIANT;
									varTestUVArray.parray=NULL;
									int nD1Min, nD1Max, nD2Min, nD2Max, testloop, testinnerloop;
									long curr2dindex[2];
									int currCacheIndex1, currCacheIndex2;
									currCacheIndex1 = currCacheIndex2 = 0;
									bGotTriUVArray = false;
									for (testloop = 0; testloop < 3; testloop++)
									{
										UVCacheArray[testloop][0] = 0.0f;
										UVCacheArray[testloop][1] = 0.0f;
									}
									if (SUCCEEDED(tri->get_UVArray(&varTestUVArray)))
									{
										bGotTriUVArray = true;
										// The docs are fairly unclear on this -- UVArray is a 2-D SAFEARRAY of _doubles_
										// where index1 = U or V (0 or 1) and index2 = tripoint index (typically 0-2)
										VARIANT varTestFloat;
										varTestFloat.vt = VT_R8;

										nD1Min = varTestUVArray.parray->rgsabound[0].lLbound;
										nD1Max = varTestUVArray.parray->rgsabound[0].cElements - 1;
										nD2Min = varTestUVArray.parray->rgsabound[1].lLbound;
										nD2Max = varTestUVArray.parray->rgsabound[1].cElements - 1;

										// we get the UVs in tripoint order
										currCacheIndex1 = 0;
										for (testloop = nD1Min; testloop <= nD1Max; testloop++) 
										{
											curr2dindex[1] = testloop;
											currCacheIndex2 = 0;
											for (testinnerloop = nD2Min; testinnerloop <= nD2Max; testinnerloop++)
											{
												curr2dindex[0] = testinnerloop;
												hrTest = SafeArrayGetElement(varTestUVArray.parray,curr2dindex,&varTestFloat);
												UVCacheArray[currCacheIndex1][currCacheIndex2] = (float)varTestFloat.dblVal;
												currCacheIndex2++;
												if (currCacheIndex2 > 1) // sanity check for crap data
													currCacheIndex2 = 1;
											}
											currCacheIndex1++;
											if (currCacheIndex1 > 2) // sanity check for non-tris
												currCacheIndex1 = 2;
										}
									}

									for(triPointIndex=0;(triPointIndex<3) && (uvIndex.lVal==-1);triPointIndex++)
									{
										TrianglePoint *triPoint=NULL;
										varTriPointIndex.lVal = triPointIndex;
										triPoints->get_Item(varTriPointIndex,&triPoint);
										if(triPoint)
										{
											if(SUCCEEDED(triPoint->get_Position(&pDisp)))
											{
												if(SUCCEEDED(pDisp->QueryInterface(IID_ISIVector3,(void **)&pos)))
												{	
													double tempVal;
													pos->get_X(&tempVal);
													tvP.fX=(float)tempVal;
													pos->get_Y(&tempVal);
													tvP.fY=(float)tempVal;
													pos->get_Z(&tempVal);
													tvP.fZ=(float)tempVal;
													pos->Release();
												}

												pDisp->Release();
											}
											VARIANT_BOOL outValid=true;

											if(SUCCEEDED(triPoint->get_Normal(&pDisp)))
											{
												if(SUCCEEDED(pDisp->QueryInterface(IID_ISIVector3,(void **)&norm)))
												{	
													double tempVal;
													norm->get_X(&tempVal);
													tvN.fX=(float)tempVal;
													norm->get_Y(&tempVal);
													tvN.fY=(float)tempVal;
													norm->get_Z(&tempVal);
													tvN.fZ=(float)tempVal;
													norm->Release();
												}

												pDisp->Release();
											}
											//
											//	if the TrianglePoint is the same as the PolygonNode
											//
											if ((FLCOMPARE(tvP.fX, (*vertexPtr)->vertex.x, 0.000001)) &&
												(FLCOMPARE(tvP.fY, (*vertexPtr)->vertex.y, 0.000001)) &&
												(FLCOMPARE(tvP.fZ, (*vertexPtr)->vertex.z, 0.000001)) &&
												(FLCOMPARE(tvN.fX, (*vertexPtr)->vertex.nx, 0.000001)) &&
												(FLCOMPARE(tvN.fY, (*vertexPtr)->vertex.ny, 0.000001)) &&
												(FLCOMPARE(tvN.fZ, (*vertexPtr)->vertex.nz, 0.000001)))
											{
												UV *pTriUV=NULL;
												hrTest = triPoint->get_UV(&pTriUV);
												if(aUV0s)
												{
#define ORIG_WAY_BEFORE_XSI3BUG 
#ifdef ORIG_WAY_BEFORE_XSI3BUG
													if(pTriUV)
													{
														double tempVal;
														hrTest = pTriUV->get_U(&tempVal);
														(*vertexPtr)->vertex.uvs[0].fU = (float)tempVal;
														hrTest = pTriUV->get_V(&tempVal);
														(*vertexPtr)->vertex.uvs[0].fV = (float)tempVal;
													}
#else
													if (bGotTriUVArray)
													{
														(*vertexPtr)->vertex.uvs[0].fU = UVCacheArray[triPointIndex][0];
														(*vertexPtr)->vertex.uvs[0].fV = UVCacheArray[triPointIndex][1];
													}
#endif // ORIG_WAY_BEFORE_XSI3BUG
												}
												Color *vertCol=NULL;
												triPoint->get_Color(&vertCol);
												if(vertCol)
												{
													ZeroColor zCol;
													double temp;
													vertCol->get_Red(&temp);
													zCol.r=(unsigned char)temp;
													vertCol->get_Green(&temp);
													zCol.g=(unsigned char)temp;
													vertCol->get_Blue(&temp);
													zCol.b=(unsigned char)temp;
													vertCol->get_Alpha(&temp);
													zCol.a=(unsigned char)temp;
													(*vertexPtr)->color = (zCol.a << 24) |
														(zCol.r << 16) |
														(zCol.g << 8) |
														(zCol.b);
													if((((*vertexPtr)->color) & 0xFFFFFF)!=0)
														aDoColors=true;

													vertCol->Release();
												}
												uvIndex.lVal=-2;

												if (pTriUV)
													pTriUV->Release();
											}
											triPoint->Release();
										}
									}
									triPoints->Release();
								}
								else
									uvIndex.lVal=-2;

								tri->Release();
							}
							else
								uvIndex.lVal=-2;
							triIndex.lVal++;
						}
						polyNode->Release();
						point->Release();
					}
					(*vertexPtr)++;
				}
			}
			polyNodes->Release();
			facetPoints->Release();
		}

		facet->Release();
		polyFace->Release();
	}
}

void CScene2MSH::SortPolygonsByMaterial(long nClusters, ClusterCollection *pClusters, PolygonFaceCollection *polys, PolygonListByType &polyLists, CZeroData *aData, X3DObject *aModel,ObjClsAttrib *aKey)
{
	bool *bPolyAdded;
	long i,nPolys=0;
	long clsIdx;
	VARIANT varClsIdx;
	VARIANT varPolyIdx;
	Cluster *pCls = 0;

	polys->get_Count(&nPolys);
	bPolyAdded=new bool[nPolys];
	memset(bPolyAdded,0,nPolys*sizeof(bool));

	//
	//	for each poly cluster (that isn't an ATRB cluster), add the polys to polyLists
	//
	varPolyIdx.vt=varClsIdx.vt=VT_I4;
	//	looking for a MSHSegment poly cluster
	for(clsIdx=0;clsIdx<nClusters;clsIdx++)
	{
		varClsIdx.lVal=clsIdx;

		pClusters->get_Item(varClsIdx,&pCls);

		char clsName[32];
		GetSIObjectName(pCls,clsName,32);
		char clsType[32];
		GetSIObjectType(pCls,clsType,32);
		
		//
		// if it's a poly cluster
		//
		if((stricmp(clsType,"poly")==0))
		{
			// it's a MSHSegment cluster
			ClusterElementCollection *pClusterPolys;
			pCls->get_Elements(&pClusterPolys);
			long nClusterPolys,clsPolyIdx;
			pClusterPolys->get_Count(&nClusterPolys);
			VARIANT varClsPolyIdx;
			varClsPolyIdx.vt=VT_I4;
			for(clsPolyIdx=0;clsPolyIdx<nClusterPolys;clsPolyIdx++) // for each polygon
			{
				VARIANT varPolyIdx;
				varPolyIdx.vt=VT_I4;
				varClsPolyIdx.lVal=clsPolyIdx;
				pClusterPolys->get_Item(varClsPolyIdx,&varPolyIdx);

				long numPolyVertices=0;
				PolygonFace *polyFace=NULL;
				polys->get_Item(varPolyIdx,&polyFace);
				PolygonNodeCollection *polyFaceNodes=NULL;
				if(polyFace)
				{
					polyFace->get_Nodes(&polyFaceNodes);
					if(polyFaceNodes)
					{
						polyFaceNodes->get_Count(&numPolyVertices);
						polyFaceNodes->Release();
					}
					polyFace->Release();
				}

				if(numPolyVertices)
				{
					SceneItem *pSIModel=NULL;
					if(SUCCEEDED(aModel->QueryInterface(IID_SceneItem,(void **)&pSIModel)))
					{
						PropertyCollection *modelProps=NULL;
						if(SUCCEEDED(pSIModel->get_Properties(&modelProps)))
						{
							long nModelProps;
							modelProps->get_Count(&nModelProps);
							//	looking for a MAT_ATRB custom property
							Property *modelProp=NULL;
							long mpIdx;
							VARIANT varMPIdx;
							varMPIdx.vt=VT_I4;
							char propName[32];
							for(mpIdx=0;(mpIdx<nModelProps) && (!bPolyAdded[varPolyIdx.lVal]);mpIdx++)
							{
								varMPIdx.lVal=mpIdx;
								modelProps->get_Item(varMPIdx,&modelProp);
								GetSIObjectName(modelProp,propName,32);
								if(strnicmp(propName,"MAT_ATRB",8)==0)
								{
									bool bFoundPoly=false;
									VARIANT polyIDs;
									polyIDs.vt=VT_BSTR;
									polyIDs.bstrVal=::SysAllocString(sBSTRInit);
									GetParameter(modelProp,"PolygonIDs",m_CurrentFrame,&polyIDs);
									_bstr_t tempPolys(polyIDs.bstrVal,true);
									::SysFreeString(polyIDs.bstrVal);
									char *pos = tempPolys;

									if(atoi(pos)==varPolyIdx.lVal)
										bFoundPoly=true;
									while(pos && !bFoundPoly)
									{
										pos = strchr(pos,',');
										if(pos)
										{
											pos++;
											if(atoi(pos)==varPolyIdx.lVal)
												bFoundPoly=true;
										}
									}
									if(bFoundPoly)
									{
										if(!bPolyAdded[varPolyIdx.lVal])
										{
											aKey->clsIdx = clsIdx;
											aKey->attrib = GetMaterialAttribute(aModel,propName);
											aKey->txt1CRC = GetMaterialTexture(aModel, propName, 1);
											aKey->txt2CRC = GetMaterialTexture(aModel, propName, 2);
											aKey->txt3CRC = GetMaterialTexture(aModel, propName, 3);
											polyLists.AddPoly(this,aData,aKey,varPolyIdx.lVal,numPolyVertices);
											bPolyAdded[varPolyIdx.lVal]=true;
										}
									}
								}
								modelProp->Release();
							}
							modelProps->Release();
						}
						pSIModel->Release();
					}

					if(!bPolyAdded[varPolyIdx.lVal])
					{
						aKey->clsIdx = clsIdx;
						aKey->attrib = 0;
						aKey->txt1CRC = 0;
						aKey->txt2CRC = 0;
						aKey->txt3CRC = 0;
						polyLists.AddPoly(this,aData,aKey,varPolyIdx.lVal,numPolyVertices);
						bPolyAdded[varPolyIdx.lVal]=true;
					}
				}
			}

			pClusterPolys->Release();
		}

		if (pCls)
			pCls->Release();
	}
	for(i=0;i<nPolys;i++)
	{
		if(!bPolyAdded[i])
		{
			long numPolyVertices=0;
			PolygonFace *polyFace=NULL;
			PolygonNodeCollection *polyFaceNodes=NULL;
			varPolyIdx.lVal=i;
			polys->get_Item(varPolyIdx,&polyFace);
			if(polyFace)
			{
				polyFace->get_Nodes(&polyFaceNodes);
				if(polyFaceNodes)
				{
					polyFaceNodes->get_Count(&numPolyVertices);
					polyFaceNodes->Release();
				}
				polyFace->Release();
			}

			if(numPolyVertices)
			{
				SceneItem *pSIModel=NULL;
				if(SUCCEEDED(aModel->QueryInterface(IID_SceneItem,(void **)&pSIModel)))
				{
					PropertyCollection *modelProps=NULL;
					if(SUCCEEDED(pSIModel->get_Properties(&modelProps)))
					{
						long nModelProps;
						modelProps->get_Count(&nModelProps);
						//	looking for a MAT_ATRB custom property
						Property *modelProp=NULL;
						long mpIdx;
						VARIANT varMPIdx;
						varMPIdx.vt=VT_I4;
						char propName[32];
						for(mpIdx=0;(mpIdx<nModelProps) && (!bPolyAdded[i]);mpIdx++)
						{
							varMPIdx.lVal=mpIdx;
							modelProps->get_Item(varMPIdx,&modelProp);
							GetSIObjectName(modelProp,propName,32);
							if(strnicmp(propName,"MAT_ATRB",8)==0)
							{
								bool bFoundPoly=false;
								VARIANT polyIDs;
								polyIDs.vt=VT_BSTR;
								polyIDs.bstrVal=::SysAllocString(sBSTRInit);
								GetParameter(modelProp,"PolygonIDs",m_CurrentFrame,&polyIDs);
								_bstr_t tempPolys(polyIDs.bstrVal,true);
								::SysFreeString(polyIDs.bstrVal);
								char *pos = tempPolys;

								if(atoi(pos)==i)
									bFoundPoly=true;
								while(pos && !bFoundPoly)
								{
									pos = strchr(pos,',');
									if(pos)
									{
										pos++;
										if(atoi(pos)==i)
											bFoundPoly=true;
									}
								}
								if(bFoundPoly)
								{
									aKey->clsIdx = -1;
									aKey->attrib = GetMaterialAttribute(aModel,propName);
									aKey->txt1CRC = GetMaterialTexture(aModel, propName, 1);
									aKey->txt2CRC = GetMaterialTexture(aModel, propName, 2);
									aKey->txt3CRC = GetMaterialTexture(aModel, propName, 3);
									polyLists.AddPoly(this,aData,aKey,i,numPolyVertices);
									bPolyAdded[i]=true;
								}
							}
							modelProp->Release();
						}
						modelProps->Release();
					}
					pSIModel->Release();
				}

				if(!bPolyAdded[i])
				{
					aKey->clsIdx = -1;
					aKey->attrib = 0;
					aKey->txt1CRC = 0;
					aKey->txt2CRC = 0;
					aKey->txt3CRC = 0;
					polyLists.AddPoly(this,aData,aKey,i,numPolyVertices);
					bPolyAdded[i]=true;
				}
			}
		}
	}
	if(bPolyAdded)
		delete[] bPolyAdded;
}

void CScene2MSH::WriteDataToMSHSegment(
	CMSHBlockModelSegment *pMSHSegment, 
	UniformPolygonList& segmentPolyList,
	MSH_FLATVERTEX *flatVertexArray,
	PolygonFaceCollection *polys, 
	int iDoUVs, 
	bool bDoColors, 
	bool bDoWeights,
	unsigned modelNameCRC, 
	CMSHBlockModel *aMSHModel, 
	CZeroData *pData) 
{

	pMSHSegment->SetMaterialName(segmentPolyList.m_materialName);
	
	ZeroStack<unsigned> indexMapPool (segmentPolyList.m_flatVertexCount+1);


	//if(aMSHModel->GetModelType()==Cloth_Node)	{
	if(aMSHModel->IsClothEnabled())	{
		int i;
		for (i = 0; i < segmentPolyList.m_flatVertexCount; i ++) {
			flatVertexArray[i].vertex.nx = 0.0;
			flatVertexArray[i].vertex.ny = 1.0;
			flatVertexArray[i].vertex.nz = 0.0;

//flatVertexArray[i].vertex.u0 = 0.0;
//flatVertexArray[i].vertex.v0 = 0.0;
		}
	}


	int vertexCount = VertexListToIndexList(&flatVertexArray[0].vertex.x, sizeof (MSH_FLATVERTEX), segmentPolyList.m_flatVertexCount, &indexMapPool[0], sizeof (MSH_FLOAT_VERTEX), 1e-3f);
	unsigned *indexMap = &indexMapPool[0];

	CMSHBlockPolygonList *mshPolyList;
	mshPolyList = pMSHSegment->GetPolygonList();
	int i = 0;
	int polyCount = 0;
	UniformPolygonList::Iterator polyIter (segmentPolyList);
	VARIANT varPoly;
	varPoly.vt=VT_I4;
	for (polyIter.Begin(); polyIter; polyIter++) 
	{   
		unsigned int j;
		varPoly.lVal = *polyIter;
		unsigned int nVerticesPerPoly = GetNumVertices(polys,varPoly);
		for(j=0;j<nVerticesPerPoly;j++)
		{
			int faceIndex;
			faceIndex = indexMap[i];
			mshPolyList->SetIndex(polyCount, (unsigned short)j, (unsigned short)faceIndex);
			i ++;
		}
		polyCount ++;
	}
	MSH_FLATVERTEX *vertexPtr = &flatVertexArray[0];
	
	//
	// write positions
	//
	pMSHSegment->SetNumPositions(vertexCount);
	for (i = 0; i < vertexCount; i++) 
	{
		pMSHSegment->SetPosition(i, vertexPtr[i].vertex.x, vertexPtr[i].vertex.y, vertexPtr[i].vertex.z);
	}
	
	//
	// write normals
	//
	pMSHSegment ->SetNumNormals(vertexCount);
	for (i = 0; i < vertexCount; i++) 
	{
		pMSHSegment->SetNormal(i, vertexPtr[i].vertex.nx, vertexPtr[i].vertex.ny, vertexPtr[i].vertex.nz);
	}
	
	//
	// write uvs (if any)
	//

	//
	//	the only materials that uses 2 sets of uvs in CloneWars are lightmap and planar_reflection.  lightmaps get created in Modeler
	if(iDoUVs>0)
	{
		CMSHBlockMaterial *pMaterial=pData->GetMaterialSet()->GetMaterialByName(segmentPolyList.m_materialName);
		iDoUVs = 1;
		if(pMaterial)
		{
			unsigned int attrib=pMaterial->GetAttribute();
			//	for planar_reflection, make space for 2 uvs
			if(((attrib & 0x0000FF00)>>8)==15)
				iDoUVs=2;
		}
	}

	for(int j=0;j<iDoUVs;j++) 
	{
		pMSHSegment->SetNumUVs(vertexCount,j);
		for (i = 0; i < vertexCount; i++) 
			pMSHSegment->SetUV (i, vertexPtr[i].vertex.uvs[0].fU, vertexPtr[i].vertex.uvs[0].fV, j);
	}

	//
	// write colors (if any)
	//
	if (bDoColors) 
	{
		unsigned int BaseColor;
		bool bHasBaseColor=true;
		
		BaseColor = vertexPtr[i].color;
		for (i = 1; ((i < vertexCount) && (bHasBaseColor)); i++)	
		{
			unsigned int tmpBaseColor= vertexPtr[i].color;
			if(tmpBaseColor!=BaseColor)
				bHasBaseColor=false;
		}
		
		//	CLRB
		if(bHasBaseColor)	
		{
			rgba rgbaColor;
			pMSHSegment ->ConvertLongToRGBA(BaseColor,&rgbaColor);
			pMSHSegment ->SetColorBase(rgbaColor.cR,rgbaColor.cG,rgbaColor.cB,rgbaColor.cA);
		} 
		else 
		{
			//	CLRL
			pMSHSegment ->SetNumColors(vertexCount);
			for (i = 0; i < vertexCount; i++) 
			{
				pMSHSegment ->SetColor(i, vertexPtr[i].color);
			}
		}
	}

	//
	// write weights (if any)
	//
	if((aMSHModel->GetModelType()==Skin_Node))
	{
		if (bDoWeights) 
		{
			for (i = 0; i < vertexCount; i++) 
			{
				int nWeights=0;
				for (int j = 0; j < MAX_WEIGHT_COUNT; j++) 
				{
					if (vertexPtr[i].weightId[j]) 
					{
						pMSHSegment->AddVertexWeight (i, (unsigned)vertexPtr[i].weightId[j], vertexPtr[i].vertex.weight[j]);
						nWeights++;
					}
				}
				if(!nWeights)
				{
					pMSHSegment->AddVertexWeight(i,modelNameCRC,1.0);
				}
			}
		}
	}
	//else
	{
		//if(aMSHModel->GetModelType()==Cloth_Node)	{
		if(aMSHModel->IsClothEnabled())	{
			int nFixedPoints;
			int nVerticalPoints;
			int nHorizontalPoints;
			ZeroStack<int> fixedIndices(vertexCount);


			nFixedPoints = 0;
			nVerticalPoints = 0;
			nHorizontalPoints = 0;
			for (i = 0; i < vertexCount; i++) {
				if(vertexPtr[i].clothVertexFlags & CLOTH_FIX_POINT) {
					fixedIndices[nFixedPoints]=i;
					nFixedPoints ++;
				}
			}
			aMSHModel->SetClothFixedPoints(nFixedPoints,&fixedIndices[0]); 

			int nUserCloth = 0;
			ZeroStack<int> userIndicesFirst(vertexCount * vertexCount);
			ZeroStack<int> userIndicesSecond(vertexCount * vertexCount);
		
			for (i = 0; i < vertexCount; i++)
			{
				if(vertexPtr[i].numUserCloth > 0)
				{
					for(int index1 = 0; index1 < vertexPtr[i].numUserCloth; index1++)
					{
						for (int j = 0; j < vertexCount; j++)
						{
							for(int index2 = 0; index2 < vertexPtr[j].numUserCloth; index2++)
							{
								if(j != i && vertexPtr[i].userClothNumbers[index1] == vertexPtr[j].userClothNumbers[index2])
									// found a pair
								{
									userIndicesFirst[nUserCloth]=i;
									userIndicesSecond[nUserCloth]=j;
									nUserCloth++;
								}
							}
						} // end for j
					} // end for index1
				} // end for i

				//if(vertexPtr[i].userClothNumber != -1)
				//{
				//	for (int j = 0; j < vertexCount; j++)
				//	{
				//		if(j != i && vertexPtr[j].userClothNumber == vertexPtr[i].userClothNumber) // found a pair
				//		{
				//			userIndicesFirst[nUserCloth]=i;
				//			userIndicesSecond[nUserCloth]=j;
				//			nUserCloth++;
				//		}
				//	}
				//}
			}
			aMSHModel->SetUserCloth(nUserCloth,&userIndicesFirst[0], &userIndicesSecond[0]); 
		}
	}
}

int CScene2MSH::GetAnimationStartFrame(Scene *aScene,char *animName)
{
	VARIANT varRetVal;
	varRetVal.vt=VT_I2;
	varRetVal.iVal=0;

	bool bDone=false;
	SceneItem *pSI=NULL;
	Model *pRoot=NULL;
	aScene->get_Root(&pRoot);
	if(pRoot)
	{
		if(SUCCEEDED(pRoot->QueryInterface(IID_SceneItem,(void **)&pSI)))
		{
			PropertyCollection *props=NULL;
			pSI->get_Properties(&props);
			long i,nCount;
			props->get_Count(&nCount);
			VARIANT varIdx;
			varIdx.vt=VT_I4;
			for(i=0;i<nCount && !bDone;i++)
			{
				varIdx.lVal = i;
				Property *prop=NULL;
				props->get_Item(varIdx,&prop);
				if(prop)
				{
					SIObject *pSIObj=NULL;
					if(SUCCEEDED(prop->QueryInterface(IID_SIObject,(void **)&pSIObj)))
					{
						BSTR sName=::SysAllocString(sBSTRInit);
						pSIObj->get_Name(&sName);
						pSIObj->Release();
						_bstr_t propName(sName,true);
						if(strstr(propName,"Animation_Cycle"))
						{
							VARIANT varName;
							varName.vt=VT_BSTR;
							varName.bstrVal=::SysAllocString(sBSTRInit);
							GetParameter(prop,"Animation_Name",m_CurrentFrame,&varName);
							_bstr_t tempName(varName.bstrVal,true);
							::SysFreeString(varName.bstrVal);
							if(strcmpi(tempName,animName)==0)
							{
								GetParameter(prop,"Start_Frame",m_CurrentFrame,&varRetVal);
								bDone=true;
							}
						}
						::SysFreeString(sName);
						pSIObj->Release();
					}
					prop->Release();
				}
			}
			pSI->Release();
		}
	}
	return varRetVal.iVal;
}

void CScene2MSH::SetObjectFlags(CMSHBlockModel *aMSHModel,X3DObject *aModel)
{
	SceneItem *pSceneItem=NULL;
	if(SUCCEEDED(aModel->QueryInterface(IID_SceneItem,(void **)&pSceneItem)))
	{
		PropertyCollection *props=NULL;
		pSceneItem->get_Properties(&props);
		VARIANT varIdx;
		varIdx.vt=VT_BSTR;
		_bstr_t tempName="FLGS";
		varIdx.bstrVal=tempName;
		Property *prop=NULL;
		props->get_Item(varIdx,&prop);
		if(prop)
		{
			VARIANT varDynamicallyLit;
			varDynamicallyLit.vt=VT_BOOL;
			GetParameter(prop,"Dynamically_Lit",m_CurrentFrame,&varDynamicallyLit);
			if(varDynamicallyLit.boolVal)
				aMSHModel->SetDynamicallyLit(true);

			VARIANT varRetainNormals;
			varRetainNormals.vt=VT_BOOL;
			if(GetParameter(prop,"Retain_Normals",m_CurrentFrame,&varRetainNormals))
			{
				if(varRetainNormals.boolVal)
					aMSHModel->SetRetainNormals(true);
			}

			VARIANT varRenderAfterShadows;
			varRenderAfterShadows.vt=VT_BOOL;
			if(GetParameter(prop,"Render_After_Shadows",m_CurrentFrame,&varRenderAfterShadows))
			{
				if(varRenderAfterShadows.boolVal)
					aMSHModel->SetRenderAfterShadows(true);
			}

			VARIANT varLeaveGeometry;
			varLeaveGeometry.vt=VT_BOOL;
			if(GetParameter(prop,"Don_t_Flatten_Geometry",m_CurrentFrame,&varLeaveGeometry))
			{
				if(varLeaveGeometry.boolVal)
					aMSHModel->SetLeaveChildGeometry(true);
			}

			VARIANT varPS2Optimize;
			varPS2Optimize.vt=VT_BOOL;
			if(GetParameter(prop,"Optimize_For_PS2",m_CurrentFrame,&varPS2Optimize))
			{
				if(varPS2Optimize.boolVal)
					aMSHModel->SetPS2Optimize(true);
			}
		}
		pSceneItem->Release();
	}
}

void CScene2MSH::SetClothData(CMSHBlockModel *aMSHModel,X3DObject *aModel)
{
	MSH_ClothConfig clothData;

	clothData.bClothEnabled=0;
	clothData.Mass=1.0;
	clothData.bAirResist=0;
	clothData.bBend=0;
	clothData.bShear=0;
	clothData.bSpring=1;
	clothData.bWind=0;
	clothData.Ks_Stretch=10.f;
	
	SceneItem *pSIAModel = NULL;

	if(SUCCEEDED(aModel->QueryInterface(IID_SceneItem,(void **)&pSIAModel)))
	{
		PropertyCollection *props=NULL;
		pSIAModel->get_Properties(&props);
		VARIANT varIdx;
		varIdx.vt=VT_BSTR;
		_bstr_t tempName="PandemicCloth";
		varIdx.bstrVal=tempName;
		Property *prop=NULL;
		props->get_Item(varIdx,&prop);
		if(prop)
		{
			VARIANT varClothType;
			varClothType.vt=VT_UI1;
			varClothType.bVal=2; // default to cotton 
			GetParameter(prop,"Cloth_Type",m_CurrentFrame,&varClothType);
			/*
				clothtype=varClothType.bVal
				0=hair
				1=silk
				2=cotton
				3=denim
			*/


			VARIANT varSoftBody;
			varSoftBody.vt=VT_BOOL;
			varSoftBody.boolVal=VARIANT_FALSE; // default to cloth
			GetParameter(prop,"SoftBody",m_CurrentFrame,&varSoftBody);
			/*
				softbody=varSoftBody.boolVal
				VARIANT_TRUE = yes, it's a softbody
				VARIANT_FALSE = no, it's a cloth
			*/


			clothData.bClothEnabled=1;
			//aMSHModel->SetModelType(Cloth_Node);
			aMSHModel->EnableCloth();
			aMSHModel->SetClothData(clothData);
		}
		pSIAModel->Release();
	}

/*
	aModel->get_ActivePrimitive(&prim);
	if(prim)
	{
		ConstructionHistory *constHistory=NULL;
		prim->get_ConstructionHistory(&constHistory);
		if(constHistory)
		{
			OperatorCollection *clothOps=NULL;
			_bstr_t bsClothOp="ClothOp";
			_bstr_t bsEmpty="";
			constHistory->Filter(bsClothOp,varEmpty,bsEmpty,&clothOps);
			if(clothOps)
			{
				long nClothOps;
				clothOps->get_Count(&nClothOps);
				if(nClothOps>0)
				{
					Operator *clothOp=NULL;
					varIdx.lVal=0;
					clothData.bClothEnabled=1;
					clothOps->get_Item(varIdx,&clothOp);
					if(clothOp)
					{
						varParam.vt=VT_I4;
						GetParameter(clothOp,"SimNbIterPerFrame",m_CurrentFrame,&varParam);
						clothData.nCorrections=varParam.lVal;

						varParam.vt=VT_BOOL;
						GetParameter(clothOp,"CollActive",m_CurrentFrame,&varParam);
						clothData.bCollide=varParam.bVal;

						//
						//	initialize cloth values just in case
						clothData.Mass=1.0;
						clothData.bAirResist=0;
						clothData.bBend=0;
						clothData.bShear=0;
						clothData.bSpring=1;
						clothData.bWind=0;
						clothData.Ks_Stretch=10.f;
					}
				}
			}
		}
	}
	if(clothData.bClothEnabled)
	{
		aMSHModel->SetModelType(Cloth_Node);
		SceneItem *pSceneItem=NULL;
		if(SUCCEEDED(aModel->QueryInterface(IID_SceneItem,(void **)&pSceneItem)))
		{
			PropertyCollection *props=NULL;
			pSceneItem->get_Properties(&props);
			VARIANT varIdx;
			varIdx.vt=VT_BSTR;
			_bstr_t tempName="Cloth";
			varIdx.bstrVal=tempName;
			Property *prop=NULL;
			props->get_Item(varIdx,&prop);
			if(prop)
			{
				varParam.vt=VT_R8;
				GetParameter(prop,"Mass",m_CurrentFrame,&varParam);
				clothData.Mass=(float)varParam.dblVal;

				GetParameter(prop,"Shear",m_CurrentFrame,&varParam);
				clothData.Ks_Shear=(float)varParam.dblVal;
				if(clothData.Ks_Shear>1.0001)
					clothData.bShear=1;

				GetParameter(prop,"Bend",m_CurrentFrame,&varParam);
				clothData.Ks_Bend=(float)varParam.dblVal;
				if(clothData.Ks_Bend>1.0001)
					clothData.bBend=1;

				GetParameter(prop,"Stretch",m_CurrentFrame,&varParam);
				clothData.Ks_Stretch=(float)varParam.dblVal;
			}

			_bstr_t tempName2="ClothForces";
			varIdx.bstrVal=tempName2;
			prop=NULL;
			props->get_Item(varIdx,&prop);
			if(prop)
			{
				varParam.vt=VT_BOOL;
				GetParameter(prop,"Wind",m_CurrentFrame,&varParam);
				clothData.bWind=varParam.bVal;

				GetParameter(prop,"AirResistance",m_CurrentFrame,&varParam);
				clothData.bAirResist=varParam.bVal;

				GetParameter(prop,"Spring",m_CurrentFrame,&varParam);
				clothData.bSpring=varParam.bVal;
			}
			pSceneItem->Release();
		}
		aMSHModel->SetClothData(clothData);
	}
*/
}

#define SAFE_RELEASE(x) if( (x) ) { (x)->Release(); (x) = NULL; }

bool CScene2MSH::DetermineCollisionVolume(const char *name, X3DObject *pModel, ClothCollision &volume)
{
	SIObject * pSI = NULL;
	Parameter * pGlobalTransform = NULL;
	Kinematics * pKine = NULL;
	IDispatch * pDispGlobal = NULL;
	Application * pApp = NULL;

	if( FAILED(pModel->QueryInterface(IID_SIObject,(void **)&pSI)) || !pSI ||
		FAILED(pSI->get_Application(&pApp)) || !pApp ||
		FAILED(pModel->get_Kinematics(&pKine)) || !pKine ||
		FAILED(pKine->get_Global(&pDispGlobal)) || !pDispGlobal ||
		FAILED(pDispGlobal->QueryInterface(IID_Parameter,(void **)&pGlobalTransform)) || !pGlobalTransform )
	{
		goto DetermineCollisionVolume_EXIT_ERROR;
	}

	// we have to determine the type of primitive for the collision volume
	HRESULT hr;
	float fWidth,fHeight,fDepth,fRad;
	DISPID dispid;			// id of method/property
	DISPPARAMS dispparams; // arguments for method call
	VARIANT varResult;
	EXCEPINFO ExcepInfo;
	unsigned int argErr;
	VARIANT sclX,sclY,sclZ;
	sclX.vt = sclY.vt = sclZ.vt = VT_R8;	
	GetParameter(pGlobalTransform,"sclx",m_CurrentFrame,&sclX);
	GetParameter(pGlobalTransform,"scly",m_CurrentFrame,&sclY);
	GetParameter(pGlobalTransform,"sclz",m_CurrentFrame,&sclZ);

	// test each possible primitive and use the first one we find
	// test for sphere...
	varResult.vt = VT_R8; // 64-bit float (verified in testing that this is what XSI returns)
	dispparams.rgvarg = new VARIANT[1];
	OLECHAR FAR* szGetValue = OLESTR("GetValue");
	dispparams.rgvarg[0].vt = VT_BSTR;
	dispparams.rgvarg[0].bstrVal = _bstr_t(name) + _bstr_t(".primitive.sphere.radius");
	pApp->GetIDsOfNames(IID_NULL,&szGetValue,1,LOCALE_SYSTEM_DEFAULT,&dispid);
	dispparams.cArgs = 1;
	dispparams.cNamedArgs = 0;
	hr = pApp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,DISPATCH_METHOD,&dispparams,&varResult,&ExcepInfo,&argErr);
	delete[] dispparams.rgvarg;
	if (SUCCEEDED(hr))
	{
		// it's a sphere
		volume.type = CLOTH_COLLISION_SPHERE;
		volume.width = (float)(varResult.dblVal*sclX.dblVal);
		volume.height = (float)(varResult.dblVal*sclY.dblVal);
		volume.depth = (float)(varResult.dblVal*sclZ.dblVal);
		goto DetermineCollisionVolume_EXIT_SUCCESS;
	}

	// test for cylinder
	varResult.vt = VT_R8; // 64-bit float (verified in testing that this is what XSI returns)
	dispparams.rgvarg = new VARIANT[1];
	dispparams.rgvarg[0].vt = VT_BSTR;
	dispparams.rgvarg[0].bstrVal = _bstr_t(name) + _bstr_t(".primitive.cylinder.radius");
	pApp->GetIDsOfNames(IID_NULL,&szGetValue,1,LOCALE_SYSTEM_DEFAULT,&dispid);
	dispparams.cArgs = 1;
	dispparams.cNamedArgs = 0;
	hr = pApp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,DISPATCH_METHOD,&dispparams,&varResult,&ExcepInfo,&argErr);
	delete[] dispparams.rgvarg;
	if (SUCCEEDED(hr))
	{
		fRad = (float)varResult.dblVal;
		// make sure we can also get the cylinder height
		varResult.vt = VT_R8; // 64-bit float (verified in testing that this is what XSI returns)
		dispparams.rgvarg = new VARIANT[1];
		dispparams.rgvarg[0].vt = VT_BSTR;
		dispparams.rgvarg[0].bstrVal = _bstr_t(name) + _bstr_t(".primitive.cylinder.height");
		pApp->GetIDsOfNames(IID_NULL,&szGetValue,1,LOCALE_SYSTEM_DEFAULT,&dispid);
		dispparams.cArgs = 1;
		dispparams.cNamedArgs = 0;
		hr = pApp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,DISPATCH_METHOD,&dispparams,&varResult,&ExcepInfo,&argErr);
		delete[] dispparams.rgvarg;
		if (SUCCEEDED(hr))
		{
			fHeight = (float)varResult.dblVal;
			volume.type = CLOTH_COLLISION_CYLINDER;
			volume.width = fRad*(float)sclX.dblVal;
			volume.height = fHeight*(float)sclY.dblVal;
			volume.depth = fRad*(float)sclZ.dblVal;
			goto DetermineCollisionVolume_EXIT_SUCCESS;
		}
	}

	// test for box
	varResult.vt = VT_R8; // 64-bit float (verified in testing that this is what XSI returns)
	dispparams.rgvarg = new VARIANT[1];
	dispparams.rgvarg[0].vt = VT_BSTR;
	dispparams.rgvarg[0].bstrVal = _bstr_t(name) + _bstr_t(".primitive.cube.length");
	pApp->GetIDsOfNames(IID_NULL,&szGetValue,1,LOCALE_SYSTEM_DEFAULT,&dispid);
	dispparams.cArgs = 1;
	dispparams.cNamedArgs = 0;
	hr = pApp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,DISPATCH_METHOD,&dispparams,&varResult,&ExcepInfo,&argErr);
	delete[] dispparams.rgvarg;
	if( SUCCEEDED(hr) )
	{
		fWidth = fHeight = fDepth = (float)varResult.dblVal;
		fWidth*=(float)sclX.dblVal;
		fHeight*=(float)sclY.dblVal;
		fDepth*=(float)sclZ.dblVal;

		volume.type = CLOTH_COLLISION_BOX;
		volume.width = 0.5f*fWidth;
		volume.height = 0.5f*fHeight;
		volume.depth = 0.5f*fDepth;
		goto DetermineCollisionVolume_EXIT_SUCCESS;
	}

DetermineCollisionVolume_EXIT_ERROR:
	SAFE_RELEASE(pApp);
	SAFE_RELEASE(pSI);
	SAFE_RELEASE(pKine);
	SAFE_RELEASE(pDispGlobal);
	SAFE_RELEASE(pGlobalTransform);
	return false;


DetermineCollisionVolume_EXIT_SUCCESS:
	SAFE_RELEASE(pApp);
	SAFE_RELEASE(pSI);
	SAFE_RELEASE(pKine);
	SAFE_RELEASE(pDispGlobal);
	SAFE_RELEASE(pGlobalTransform);
	return false;

}

void CScene2MSH::SetClothCollisionData(CMSHBlockModel *aMSHModel,X3DObject *aModel, Scene *aScene)
{
	SceneItem *pSIAModel = NULL;
	if(FAILED(aModel->QueryInterface(IID_SceneItem,(void **)&pSIAModel)))
		return;

	PropertyCollection *props=NULL;
	pSIAModel->get_Properties(&props);
	VARIANT varIdx;
	varIdx.vt=VT_BSTR;
	_bstr_t tempName="PandemicCloth";
	varIdx.bstrVal=tempName;
	Property *prop=NULL;
	props->get_Item(varIdx,&prop);

	if( !prop ) {
		pSIAModel->Release();
		return;
	}


	Model *pRootModel;
	if(FAILED(aScene->get_Root(&pRootModel))) {
		pSIAModel->Release();
		return;
	}

	X3DObject *pRoot3DObject;
	if(FAILED(pRootModel->QueryInterface(IID_X3DObject,(void **)&pRoot3DObject))) {
		pRootModel->Release();
		pSIAModel->Release();
		return;
	}

	VARIANT volumes;
	volumes.vt=VT_BSTR;
	volumes.bstrVal=::SysAllocString(sBSTRInit);
	GetParameter(prop,"CollisionVolumes", m_CurrentFrame, &volumes);
	_bstr_t tempVolumes(volumes.bstrVal, true);
	::SysFreeString(volumes.bstrVal);



	char *ptr;
	VARIANT name;
	VARIANT family;
	family.vt = VT_EMPTY;
	// loop through each collision volume
	for (ptr = strtok (tempVolumes, ",\n"); ptr;  ptr = strtok (NULL, ",\n")) {
		// find the volume in the scene graph
		ClothCollision volume;
		X3DObject *collisionVolumeObject = NULL;
		name.vt = VT_BSTR;
		name.bstrVal = _bstr_t(ptr);

		HRESULT hr = pRoot3DObject->FindChild(name,L"polymsh",family,true,&collisionVolumeObject);

		if( collisionVolumeObject ) {
			X3DObject *parent;
			char parentName[128];

			collisionVolumeObject->get_Parent(&parent);
			GetSIObjectName(parent,parentName,128);

			volume.parentCRC = CalcStrCRC(parentName);
			volume.nameCRC = CalcStrCRC(ptr);

			DetermineCollisionVolume(ptr, collisionVolumeObject, volume);
			aMSHModel->AddClothCollisionVolume(volume);
			collisionVolumeObject->Release();
			parent->Release();
		}
	}
	pSIAModel->Release();
	pRootModel->Release();
	pRoot3DObject->Release();

}

void CScene2MSH::deprecatedSetClothCollisionData(CMSHBlockModel *aMSHModel,X3DObject *aModel)
{
	VARIANT varParam;
	
	SceneItem *pSceneItem=NULL;
	if(SUCCEEDED(aModel->QueryInterface(IID_SceneItem,(void **)&pSceneItem)))
	{
		PropertyCollection *props=NULL;
		pSceneItem->get_Properties(&props);
		VARIANT varIdx;
		varIdx.vt=VT_BSTR;
		_bstr_t tempName="ClothCollision";
		varIdx.bstrVal=tempName;
		Property *prop=NULL;
		props->get_Item(varIdx,&prop);
		if(prop)
		{
			varParam.vt=VT_UI1;
			GetParameter(prop,"Type",m_CurrentFrame,&varParam);

			VARIANT owners;
			owners.vt=VT_BSTR;
			owners.bstrVal=::SysAllocString(sBSTRInit);
			GetParameter(prop,"Collides_With", m_CurrentFrame, &owners);
			_bstr_t tempOwners(owners.bstrVal, true);
			::SysFreeString(owners.bstrVal);
	
			char *ptr;
			for (ptr = strtok (tempOwners, ",\n"); ptr;  ptr = strtok (NULL, ",\n")) {
				aMSHModel->AddClothCollisionOwner (ptr);
			}

			switch(varParam.bVal)
			{
				case COLLISION_SPHERE:
				{
					aMSHModel->SetCollisionSphere();
				}
				break;
				
				case COLLISION_CYLINDER:
				{
					aMSHModel ->SetCollisionCylinder();
				}
				break;
				
				case COLLISION_CONE:
				{
					aMSHModel ->SetCollisionCone();
				}
				break;
			}
		}

		pSceneItem->Release();
	}
}

void CScene2MSH::GetTorsoPositionAndOrientation(float aFrameNumber, vect3 &aPos, vect3 &aFront)
{
	aPos.fX=aPos.fY=aPos.fZ=0.f;
	aFront.fX=aFront.fY=aFront.fZ=0.f;
	X3DObject *torsoObj=m_AllModels.mTorsoModel;
	if(torsoObj)
	{

		Parameter *pLocalTransform=NULL;
		Kinematics *kine=NULL;
		if(SUCCEEDED(torsoObj->get_Kinematics(&kine)))
		{
			IDispatch *pDisp;
			HRESULT hr;
			hr=kine->get_Local(&pDisp);
			if(SUCCEEDED(hr))
			{
				pDisp->QueryInterface(IID_Parameter,(void **)&pLocalTransform);
			}
		}

		if(pLocalTransform)
		{
			VARIANT posX,posY,posZ;
			VARIANT rotX,rotY,rotZ;

			posX.vt=posY.vt=posZ.vt=VT_R8;
			rotX.vt=rotY.vt=rotZ.vt=VT_R8;

			GetParameter(pLocalTransform,"posx",aFrameNumber,&posX);
			GetParameter(pLocalTransform,"posy",aFrameNumber,&posY);
			GetParameter(pLocalTransform,"posz",aFrameNumber,&posZ);
			GetParameter(pLocalTransform,"rotx",aFrameNumber,&rotX);
			GetParameter(pLocalTransform,"roty",aFrameNumber,&rotY);
			GetParameter(pLocalTransform,"rotz",aFrameNumber,&rotZ);

			aPos.fX=(float)posX.dblVal;
			aPos.fY=(float)posY.dblVal;
			aPos.fZ=(float)posZ.dblVal;

			ZeroMatrix pitchMatrix(PitchMatrix ((float)rotX.dblVal * DEG2RAD));
			ZeroMatrix yawMatrix (YawMatrix ((float)rotY.dblVal * DEG2RAD));
			ZeroMatrix rollMatrix (RollMatrix ((float)rotZ.dblVal * DEG2RAD));

			ZeroMatrix localMatrix=pitchMatrix *yawMatrix *rollMatrix;
			aFront.fX=localMatrix.front.x;
			aFront.fY=localMatrix.front.y;
			aFront.fZ=localMatrix.front.z;
		}
	}
}

void CScene2MSH::SetAnimTriggerCollision(CMSHBlockModel *aMSHModel,X3DObject *aModel,Scene *aScene)
{
	SceneItem *pSceneItem=NULL;
	if(SUCCEEDED(aModel->QueryInterface(IID_SceneItem,(void **)&pSceneItem)))
	{
		PropertyCollection *props=NULL;
		pSceneItem->get_Properties(&props);
		VARIANT varIdx;
		varIdx.vt=VT_BSTR;
		_bstr_t tempName="COL1";
		varIdx.bstrVal=tempName;
		Property *prop=NULL;
		props->get_Item(varIdx,&prop);
		if(prop)
		{
			VARIANT varAnimName,varDeviation,varTime,varTolMin,varTolMax,
					varRecovery,varHitFrameMin,varHitFrameMax,varType;

			varAnimName.vt=VT_BSTR;
			varAnimName.bstrVal=::SysAllocString(sBSTRInit);
			GetParameter(prop,"Animation_Name",m_CurrentFrame,&varAnimName);
			_bstr_t sAnimName(varAnimName.bstrVal,true);
			::SysFreeString (varAnimName.bstrVal);  

			varType.vt=VT_UI1;
			varDeviation.vt=VT_I4;
			varTime.vt=VT_R8;
			varTolMin.vt=VT_R8;
			varTolMax.vt=VT_R8;
			varRecovery.vt=VT_R8;
			varHitFrameMin.vt=VT_I2;
			varHitFrameMax.vt=VT_I2;


			GetParameter(prop,"Type",m_CurrentFrame,&varType);
			GetParameter(prop,"Late_Start_Frame__ABS_",m_CurrentFrame,&varDeviation);
			GetParameter(prop,"Time",m_CurrentFrame,&varTime);
			GetParameter(prop,"Early_Tolerance",m_CurrentFrame,&varTolMin);
			GetParameter(prop,"Late_Tolerance",m_CurrentFrame,&varTolMax);
			GetParameter(prop,"Recovery_Time",m_CurrentFrame,&varRecovery);
			GetParameter(prop,"Hit_Frame_Min__ABS_",m_CurrentFrame,&varHitFrameMin);
			GetParameter(prop,"Hit_Frame_Max__ABS_",m_CurrentFrame,&varHitFrameMax);

			vect3 torsoPos,torsoFront;
			GetTorsoPositionAndOrientation(varHitFrameMin.iVal,torsoPos,torsoFront);

			MSH_AnimTriggerCollisionConfig animTriggerCollData;
			animTriggerCollData.config.bCollisionEnabled=true;
			animTriggerCollData.config.sType = varType.bVal;
			animTriggerCollData.fTime = (float)varTime.dblVal;
			animTriggerCollData.fToleranceMin = (float)varTolMin.dblVal;
			animTriggerCollData.fToleranceMax = (float)varTolMax.dblVal;
			animTriggerCollData.fRecoveryTime= (float)varRecovery.dblVal;

			int iStart=GetAnimationStartFrame(aScene,sAnimName);
			animTriggerCollData.iFrameMin=varHitFrameMin.iVal-iStart;
			animTriggerCollData.iFrameMax=varHitFrameMax.iVal-iStart;
			animTriggerCollData.iLateStartFrame = varDeviation.iVal-iStart;

			if(animTriggerCollData.config.bCollisionEnabled)
			{
				switch(animTriggerCollData.config.sType)
				{
				case COLLISION_SPHERE:
					{
						MSH_ClothCollisionSphere sphereData;
						aMSHModel ->SetAnimTriggerCollisionSphere(&sphereData,sAnimName,animTriggerCollData.iLateStartFrame,animTriggerCollData.fTime,animTriggerCollData.iFrameMin,animTriggerCollData.iFrameMax, animTriggerCollData.fToleranceMin, animTriggerCollData.fToleranceMax, animTriggerCollData.fRecoveryTime,torsoPos,torsoFront);
					}
					break;
					
				case COLLISION_CYLINDER:
					{
						MSH_ClothCollisionCylinder cylinderData;
						aMSHModel ->SetAnimTriggerCollisionCylinder(&cylinderData,sAnimName,animTriggerCollData.iLateStartFrame,animTriggerCollData.fTime,animTriggerCollData.iFrameMin,animTriggerCollData.iFrameMax, animTriggerCollData.fToleranceMin, animTriggerCollData.fToleranceMax, animTriggerCollData.fRecoveryTime,torsoPos,torsoFront);
					}
					break;
					
				case COLLISION_CONE:
					{
						MSH_ClothCollisionCone coneData;
						aMSHModel ->SetAnimTriggerCollisionCone(&coneData,sAnimName,animTriggerCollData.iLateStartFrame,animTriggerCollData.fTime,animTriggerCollData.iFrameMin,animTriggerCollData.iFrameMax, animTriggerCollData.fToleranceMin, animTriggerCollData.fToleranceMax, animTriggerCollData.fRecoveryTime,torsoPos,torsoFront);
					}
					break;
				}
			}
		}
		pSceneItem->Release();
	}
}

static const int MAX_USER_CLOTH_SPRINGS = 3000;

void CScene2MSH::CreateGeometry(
	Geometry *aGeometry,
	CZeroData *aData, 
	CMSHBlockModel *aMSHModel, 
	X3DObject *aModel,
	Scene *aScene,
	ObjClsAttrib *aKey)
{
	PolygonMesh *pPolyMesh=NULL;
	PolygonFaceCollection *polys=NULL;
	FacetCollection *facets=NULL;
	TriangleCollection *tris=NULL;
	long nPolys=0;
	bool bDoVertColors=false;
	bool bDoFixedClothPoints=true;
	PropertyCollection *props = NULL;


	aMSHModel->SetModelType(Static_Node);
	if(SUCCEEDED(aGeometry->QueryInterface(IID_PolygonMesh,(void **)&pPolyMesh)))
	{
		if(SUCCEEDED(pPolyMesh->get_Polygons(&polys)))
		{
			polys->get_Count(&nPolys);
		}
		aGeometry->get_Facets(&facets);
		aGeometry->get_Triangles(&tris);
		pPolyMesh->Release();
	}


	SceneItem *pSIAModel = NULL;
	if(SUCCEEDED(aModel->QueryInterface(IID_SceneItem,(void **)&pSIAModel)))
	{
		pSIAModel->get_Properties(&props);
		pSIAModel->Release();
	}


	m_ProgDlg->SetProgress(NULL,30);
	if(facets && polys && tris) // oh my
	{
		ClusterCollection *pClusters=NULL;
		ClusterElementCollection *UV0s=NULL;
		ClusterElementCollection *UV1s=NULL;
		
		long triIdx,facetIdx,tmpNFacets,tmpNTris;
		facets->get_Count(&tmpNFacets);
		tris->get_Count(&tmpNTris);
		ZeroStack<long> PolyTriStart(tmpNFacets);
		facetIdx=0;
		VARIANT varTriIdx;
		varTriIdx.vt=VT_I4;

		for(triIdx = 0;triIdx<tmpNTris;triIdx++)
		{
			long tmpPolyIdx=-1;
			Triangle *tri;
			varTriIdx.lVal=triIdx;
			tris->get_Item(varTriIdx,&tri);
			tri->get_PolygonIndex(&tmpPolyIdx);
			if(tmpPolyIdx==facetIdx)
			{
				PolyTriStart[facetIdx]=triIdx;
				facetIdx++;
			}
			tri->Release();
		}

		ClusterElementCollection *pClothFixPointsCls=NULL;
		ClusterElementCollection *pUserCloth[MAX_USER_CLOTH_SPRINGS]={NULL};
		int numUserCloth = 0;

		ZeroVisTree<int, int> clothBoder1Index;
		ZeroVisTree<int, int> clothBoder2Index;


		m_ProgDlg->SetProgress(NULL,35);
		long nClusters=0;
		if(SUCCEEDED(aGeometry->get_Clusters(&pClusters)))
		{
			pClusters->get_Count(&nClusters);

			VARIANT varClsIdx;
			Cluster *pCls;
			varClsIdx.vt=VT_I4;
			long clsIdx;

			//
			//	find UVElems cluster in this geometry
			//
			for(clsIdx=0;clsIdx<nClusters && !UV1s ;clsIdx++)
			{
				varClsIdx.lVal=clsIdx;
				if(SUCCEEDED(pClusters->get_Item(varClsIdx,&pCls)))
				{
					char clsType[32];
					GetSIObjectType(pCls,clsType,32);
					if(strcmpi(clsType,"sample")==0)
					{
						Property *uvProp=NULL;
						GetPropertyByType(pCls,"uvspace",&uvProp,0);
						if(uvProp)
						{
							ClusterProperty *uvClsProp;
							if(SUCCEEDED(uvProp->QueryInterface(IID_ClusterProperty,(void **)&uvClsProp)))
							{
								uvClsProp->get_Elements(&UV0s);
								uvClsProp->Release();
							}
						}
						GetPropertyByType(pCls,"uvspace",&uvProp,1);
						if(uvProp)
						{
							ClusterProperty *uvClsProp;
							if(SUCCEEDED(uvProp->QueryInterface(IID_ClusterProperty,(void **)&uvClsProp)))
							{
								uvClsProp->get_Elements(&UV1s);
								uvClsProp->Release();
							}
						}
						Property *vertColors=NULL;
						GetPropertyByType(pCls,"vertexcolor",&vertColors);
						if(vertColors)
							bDoVertColors=true;
					} else {
						char tmpClsName[128];
						GetSIObjectName(pCls,tmpClsName,128);
						if(strcmp(tmpClsName,"PandemicCloth_Fixed")==0)
						{
							pCls->get_Elements(&pClothFixPointsCls);
						}
						else if(strnicmp(tmpClsName,"spring_",7)==0 && numUserCloth < MAX_USER_CLOTH_SPRINGS-1)
						{
							pCls->get_Elements(&pUserCloth[numUserCloth]);
							numUserCloth++;
						}

						VARIANT varIdx;
						_bstr_t tempName2="ClothBorders";
						varIdx.bstrVal=tempName2;

						Property* prop;

						prop=NULL;
						varIdx.vt = VT_BSTR;

						if(props)
						{
							props->get_Item(varIdx,&prop);
						}
						if(prop)
						{
							char* ptr;
							_bstr_t tempName;
							VARIANT varParam;
							ZeroStack<char> tmp(1024 * 64);

							varParam.vt=VT_BSTR;
							varParam.bstrVal=::SysAllocString(sBSTRInit);
							GetParameter(prop,"Border1_ids",m_CurrentFrame,&varParam);
							tempName=varParam.bstrVal;
							strcpy (&tmp[0], (char*)tempName); 

							for (ptr = strtok (&tmp[0], ","); ptr; ptr = strtok (NULL, ",")) {
								int index = atoi (ptr);
								clothBoder1Index.Insert (index, index);
							}
							::SysFreeString(varParam.bstrVal);


							varParam.vt=VT_BSTR;
							varParam.bstrVal=::SysAllocString(sBSTRInit);
							GetParameter(prop,"Border2_ids",m_CurrentFrame,&varParam);
							tempName=varParam.bstrVal;
							strcpy (&tmp[0], (char*)tempName); 
							for (ptr = strtok (&tmp[0], ","); ptr; ptr = strtok (NULL, ",")) {
								int index = atoi (ptr);
								clothBoder2Index.Insert (index, index);
							}
							::SysFreeString(varParam.bstrVal);

							prop->Release();
						}

					}
					pCls->Release();
				}
			}
		}

		m_ProgDlg->SetProgress(NULL,40);
		//
		//	Sort polygons into segments
		//
		PolygonListByType polyLists;
		SortPolygonsByMaterial(nClusters,pClusters,polys,polyLists,aData,aModel,aKey);

		m_ProgDlg->SetProgress(NULL,50);
		long nPoints=0;
		PointCollection *pPoints=NULL;
		aGeometry->get_Points(&pPoints);
		if(pPoints)
		{
			pPoints->get_Count(&nPoints);
			pPoints->Release();
		}

		X3DObjectCollection *pDeformers=NULL;
		VARIANT varWeightsArray; 
		varWeightsArray.vt=VT_ARRAY|VT_VARIANT;
		varWeightsArray.parray=NULL;
		if(nPoints)
		{
			SceneItem *pSceneItemModel=NULL;
			if(SUCCEEDED(aModel->QueryInterface(IID_SceneItem,(void **)&pSceneItemModel)))
			{
				VARIANT varEnvIdx;
				varEnvIdx.vt=VT_I4;
				varEnvIdx.lVal=0;
				EnvelopeCollection *pEnvelopes=NULL;
				pSceneItemModel->get_Envelopes(&pEnvelopes);
				if(pEnvelopes)
				{
					Envelope *pEnvelope=NULL;
					pEnvelopes->get_Item(varEnvIdx,&pEnvelope);
					if(pEnvelope)
					{
						ClusterElementCollection *weightClusters=NULL;
						pEnvelope->get_Deformers(&pDeformers);
						pEnvelope->get_Weights(varEnvIdx,&weightClusters);
						long nDeformers=0;
						pDeformers->get_Count(&nDeformers);
						if(nDeformers && nPoints)
						{
							weightClusters->get_Array(&varWeightsArray);
							aMSHModel->SetModelType(Skin_Node);
							VARIANT varDefIdx;
							varDefIdx.vt=VT_I4;
							X3DObject *defObject;
							for(varDefIdx.lVal=0;varDefIdx.lVal<nDeformers;varDefIdx.lVal++)
							{
								defObject=NULL;
								pDeformers->get_Item(varDefIdx,&defObject);
								if(defObject)
								{
									m_Deformers.Insert(defObject,GetModelCRC(defObject));
									defObject->Release();
								}
							}
						}
						if (weightClusters)
							weightClusters->Release();
						pEnvelope->Release();
					}
					pEnvelopes->Release();
				}
				pSceneItemModel->Release();
			}
		}

		m_ProgDlg->SetProgress(NULL,55);
		//
		// for each segment, get all the poly data and create a MSHSegment
		//
		bool bShowWeightsWarning=false;
		PolygonListByType::Iterator iter (polyLists);
		unsigned int numPolyLists = polyLists.GetCount();
		unsigned int curPolyLists = 0;
		for (iter.Begin(); iter; iter ++) 
		{
			int baseProg=55+curPolyLists*45/numPolyLists;
			m_ProgDlg->SetProgress(NULL,baseProg);
			UniformPolygonList& segmentPolyList = (*iter)->info;
			bool bDoColors = false;

			// create space to hold all faces
			ZeroStack<MSH_FLATVERTEX> flatVertexArray (segmentPolyList.m_flatVertexCount+1);
			MSH_FLATVERTEX *vertexPtr = &flatVertexArray[0];
			
			UniformPolygonList::Iterator polyIter (segmentPolyList);
			//
			// for each polygon in the segment, get its data
			//
			m_ProgDlg->SetProgress(NULL,baseProg+(int)(0.33f*45/numPolyLists));
			VARIANT varPoly;
			varPoly.vt=VT_I4;

			for (polyIter.Begin(); polyIter; polyIter++) 
			{   
				varPoly.lVal = *polyIter;
//				GetPolygonData(facets,polys,tris,varPoly,&vertexPtr, UV0s,UV1s,&PolyTriStart[0],bDoColors, pDeformers, &varWeightsArray, 
//									pClothFixPointsCls, clothBoder1Index, clothBoder2Index);

				GetPolygonData(facets,polys,tris,varPoly,&vertexPtr, UV0s,UV1s,&PolyTriStart[0],bDoColors, pDeformers, &varWeightsArray, pClothFixPointsCls, pUserCloth, numUserCloth, &bShowWeightsWarning);

			}

			m_ProgDlg->SetProgress(NULL,baseProg+(int)(0.66f*45/numPolyLists));
			//
			//	create CMSHBlockModelSegment here ...
			//
			int iDoUVs=0;
			bool bDoWeights=false;
			if(pDeformers)	{
				bDoWeights=true;
			}
			CMSHBlockModelSegment *pMSHSegment=aMSHModel->CreateAddSegment();
			CMSHBlockMaterial *pMaterial=mZeroData->GetMaterialSet()->GetMaterialByName(segmentPolyList.m_materialName);
			if(UV0s)
			{
				pMaterial->GetTextureFilenames(iDoUVs);
			}

			SetClothData(aMSHModel,aModel);
			WriteDataToMSHSegment(pMSHSegment,segmentPolyList,&flatVertexArray[0],polys, iDoUVs, bDoVertColors, bDoWeights,aKey->objNameCRC,aMSHModel,aData);

			SetObjectFlags(aMSHModel,aModel);

			SetClothCollisionData(aMSHModel,aModel,aScene);

			SetAnimTriggerCollision(aMSHModel,aModel,aScene);
			m_ProgDlg->SetProgress(NULL,baseProg+45/numPolyLists);
			curPolyLists++;
		}
		if(bShowWeightsWarning)
		{
			char s[256];
			sprintf(s,"The skin [%s] has vertices that are not weighted to any bones.",aMSHModel->GetName());
			if(m_bWarningsOn)
				MessageBox(0,s,"Warning!",MB_OK|MB_SYSTEMMODAL);
		}

		if (UV0s)
			UV0s->Release();
		if (UV1s)
			UV1s->Release();

		// Release our main data structures
		facets->Release();
		polys->Release();
		tris->Release();
		props->Release();

		if (pClusters)
			pClusters->Release();

		if (pDeformers)
			pDeformers->Release();
	}


	// if the geomwtry is a shadow volume ...
	Property *custParamSet;
	GetPropertyByType(aModel,"customparamSet", &custParamSet);
	if(custParamSet) {
		VARIANT varShadow;
		varShadow.vt=VT_BOOL;

		if (GetParameter(custParamSet, "shadowvolume", m_CurrentFrame, &varShadow)){
			aMSHModel->SetModelType(Shadow_Volume);
		}

		custParamSet->Release();
	}



}

bool CScene2MSH::GetBasePose(X3DObject *pModel, ZeroMatrix &matrix, VARIANT &sclX, VARIANT &sclY, VARIANT &sclZ,ZeroImportExportNode *pParentNode)
{
	bool retVal=false;
	X3DObject *pParent=NULL;
	pModel->get_Parent(&pParent);

	VARIANT posX,posY,posZ;
	VARIANT rotX,rotY,rotZ;
	Property *pBasePose=NULL;
	SceneItem *pSceneItemModel;
	bool bGotParentBasePose=false;
	posX.vt=posY.vt=posZ.vt=rotX.vt=rotY.vt=rotZ.vt=VT_R8;
	if(SUCCEEDED(pModel->QueryInterface(IID_SceneItem,(void **)&pSceneItemModel)))
	{
		PropertyCollection *modelProps=NULL;
		if(SUCCEEDED(pSceneItemModel->get_Properties(&modelProps)))
		{
			if(modelProps)
			{
				VARIANT varBasePose;
				varBasePose.vt=VT_BSTR;
				_bstr_t bstrBasePose="Static KineState";
				varBasePose.bstrVal=bstrBasePose;
				modelProps->get_Item(varBasePose,&pBasePose);
				if(pBasePose)
				{
					retVal=true;
					GetParameter(pBasePose,"posx",m_CurrentFrame,&posX);
					GetParameter(pBasePose,"posy",m_CurrentFrame,&posY);
					GetParameter(pBasePose,"posz",m_CurrentFrame,&posZ);
					GetParameter(pBasePose,"sclx",m_CurrentFrame,&sclX);
					GetParameter(pBasePose,"scly",m_CurrentFrame,&sclY);
					GetParameter(pBasePose,"sclz",m_CurrentFrame,&sclZ);
					GetParameter(pBasePose,"orix",m_CurrentFrame,&rotX);
					GetParameter(pBasePose,"oriy",m_CurrentFrame,&rotY);
					GetParameter(pBasePose,"oriz",m_CurrentFrame,&rotZ);
					if(pParent)
					{
						VARIANT parPosX,parPosY,parPosZ;
						VARIANT parSclX,parSclY,parSclZ;
						VARIANT parRotX,parRotY,parRotZ;
						parPosX.vt=parPosY.vt=parPosZ.vt=parSclX.vt=parSclY.vt=parSclZ.vt=parRotX.vt=parRotY.vt=parRotZ.vt=VT_R8;
						parPosX.dblVal=parPosY.dblVal=parPosZ.dblVal=parSclX.dblVal=parSclY.dblVal=parSclZ.dblVal=parRotX.dblVal=parRotY.dblVal=parRotZ.dblVal=0.f;
						SceneItem *pSceneItemParent;
						if(SUCCEEDED(pParent->QueryInterface(IID_SceneItem,(void **)&pSceneItemParent)))
						{
							Property *pParentBasePose=NULL;
							PropertyCollection *parentProps=NULL;
							if(SUCCEEDED(pSceneItemParent->get_Properties(&parentProps)))
							{
								if(parentProps)
								{
									VARIANT varParentBasePose;
									varParentBasePose.vt=VT_BSTR;
									_bstr_t bstrParentBasePose="Static KineState";
									varParentBasePose.bstrVal=bstrParentBasePose;
									parentProps->get_Item(varParentBasePose,&pParentBasePose);
									if(pParentBasePose)
									{
										bGotParentBasePose=true;
										GetParameter(pParentBasePose,"posx",m_CurrentFrame,&parPosX);
										GetParameter(pParentBasePose,"posy",m_CurrentFrame,&parPosY);
										GetParameter(pParentBasePose,"posz",m_CurrentFrame,&parPosZ);
										GetParameter(pParentBasePose,"sclx",m_CurrentFrame,&parSclX);
										GetParameter(pParentBasePose,"scly",m_CurrentFrame,&parSclY);
										GetParameter(pParentBasePose,"sclz",m_CurrentFrame,&parSclZ);
										GetParameter(pParentBasePose,"orix",m_CurrentFrame,&parRotX);
										GetParameter(pParentBasePose,"oriy",m_CurrentFrame,&parRotY);
										GetParameter(pParentBasePose,"oriz",m_CurrentFrame,&parRotZ);
									}
								}
							}
							pSceneItemParent->Release();
						}
						if(!bGotParentBasePose)
						{
//							Parameter *pParentGlobalTransform=NULL;
//							Kinematics *kine=NULL;
//							if(SUCCEEDED(pParent->get_Kinematics(&kine)))
//							{
//								IDispatch *pDisp;
//								if(SUCCEEDED(kine->get_Global(&pDisp)))
//								{
//									pDisp->QueryInterface(IID_Parameter,(void **)&pParentGlobalTransform);
//								}
//							}
//							if(!pParentGlobalTransform)
//								retVal=false;
//							else
							{
								ZeroMatrix mGlob=pParentNode->GetWorldMatrix(mDatabase);
								ZeroVector3 euler=mGlob.CalcPitchYawRollEulerAngles();
								parRotX.dblVal=euler.x*RAD2DEG;
								parRotY.dblVal=euler.y*RAD2DEG;
								parRotZ.dblVal=euler.z*RAD2DEG;
								parSclX.dblVal=parSclY.dblVal=parSclZ.dblVal=1.0;
								parPosX.dblVal=mGlob.posit.x;
								parPosY.dblVal=mGlob.posit.y;
								parPosZ.dblVal=mGlob.posit.z;
/*
								GetParameter(pParentGlobalTransform,"posx",m_CurrentFrame,&parPosX);
								GetParameter(pParentGlobalTransform,"posy",m_CurrentFrame,&parPosY);
								GetParameter(pParentGlobalTransform,"posz",m_CurrentFrame,&parPosZ);
								GetParameter(pParentGlobalTransform,"sclx",m_CurrentFrame,&parSclX);
								GetParameter(pParentGlobalTransform,"scly",m_CurrentFrame,&parSclY);
								GetParameter(pParentGlobalTransform,"sclz",m_CurrentFrame,&parSclZ);
								GetParameter(pParentGlobalTransform,"rotx",m_CurrentFrame,&parRotX);
								GetParameter(pParentGlobalTransform,"roty",m_CurrentFrame,&parRotY);
								GetParameter(pParentGlobalTransform,"rotz",m_CurrentFrame,&parRotZ);
*/
							}
						}

						/*
						have model and parent in global space
						get Mglob*PglobInverse=Mloc)
						*/
						if(retVal)
						{
							ZeroMatrix pitchMatrix(PitchMatrix ((float)rotX.dblVal * DEG2RAD));
							ZeroMatrix yawMatrix (YawMatrix ((float)rotY.dblVal * DEG2RAD));
							ZeroMatrix rollMatrix (RollMatrix ((float)rotZ.dblVal * DEG2RAD));
							matrix = pitchMatrix *yawMatrix *rollMatrix;

							ZeroMatrix parPitchMatrix(PitchMatrix ((float)parRotX.dblVal * DEG2RAD));
							ZeroMatrix parYawMatrix (YawMatrix ((float)parRotY.dblVal * DEG2RAD));
							ZeroMatrix parRollMatrix (RollMatrix ((float)parRotZ.dblVal * DEG2RAD));
							ZeroMatrix parMatrix (parPitchMatrix *parYawMatrix *parRollMatrix );
							parMatrix.posit=ZeroVector3((float)parPosX.dblVal,(float)parPosY.dblVal,(float)parPosZ.dblVal);
							ZeroMatrix parInvMatrix=parMatrix.Inverse();

							matrix=matrix*parInvMatrix;
/*
							ZeroVector3 euler=matrix.CalcPitchYawRollEulerAngles();
							rotX.dblVal=RAD2DEG*euler.x;
							rotY.dblVal=RAD2DEG*euler.y;
							rotZ.dblVal=RAD2DEG*euler.z;
*/
							ZeroVector3 modPos((float)posX.dblVal,(float)posY.dblVal,(float)posZ.dblVal);
							modPos=parInvMatrix.TransformVector(modPos);
							matrix.posit.x=modPos.x;
							matrix.posit.y=modPos.y;
							matrix.posit.z=modPos.z;
						}
					}
				}
			}
		}
		pSceneItemModel->Release();
	}
	return retVal;
}

unsigned int CScene2MSH::GetModelCRC(X3DObject *pModel)
{
	VARIANT name;
	name.vt=VT_BSTR;
	name.bstrVal=::SysAllocString(sBSTRInit);
	SIObject *pSIModel=NULL;
	if(SUCCEEDED(pModel->QueryInterface(IID_SIObject,(void **)&pSIModel)))
	{
		pSIModel->get_FullName(&name.bstrVal);
		pSIModel->Release();
	}
	_bstr_t tempName(name.bstrVal,true);
	::SysFreeString (name.bstrVal);  
	unsigned int crc=0;
	crc=CalcStrCRC(tempName,crc);
	return crc;
}


/*
void CScene2MSH::SetPoseConstraint (X3DObject *model, CMSHBlockModel *aMSHModel)
{
	int i;
	long count;
	Kinematics* kinematics;
	ConstraintCollection	*constrains;


	model->get_Kinematics (&kinematics);
	if (!kinematics) {
		return ;
	}

	kinematics->get_Constraints (&constrains);
	if (!constrains) {
		return ;
	}

	constrains->get_Count (&count);
	for (i = 0; i < count; i ++) {
		VARIANT variant;
		Constraint *contraint;
		SIObject *pSiContraint;

		variant.vt = VT_I4;
		variant.lVal = i;
		constrains->get_Item (variant, &contraint);
		if(SUCCEEDED(contraint->QueryInterface(IID_SIObject,(void **)&pSiContraint))) {
			VARIANT name;
			BSTR bstrTemp=::SysAllocString(L"");
			name.vt=VT_BSTR;
			name.bstrVal=bstrTemp;
			pSiContraint->get_FullName (&name.bstrVal);
			_bstr_t tempName=name.bstrVal;

			char *strName = tempName;
			::SysFreeString (bstrTemp);  
			pSiContraint->Release();

			if (strstr (tempName, ".kine.posecns")) {
				long objCount;
				SIObject *pSiModel;
				X3DObject *objModel;
				X3DObjectCollection *objList;
				contraint->get_Constraining (&objList);

				objList->get_Count (&objCount);
				_ASSERTE (objCount == 1);

				VARIANT objVariant;
				objVariant.vt = VT_I4;
				objVariant.lVal = 0;
				objList->get_Item (objVariant, &objModel);

				if(SUCCEEDED(objModel->QueryInterface(IID_SIObject,(void **)&pSiModel))) {
					VARIANT name;
					BSTR bstrTemp=::SysAllocString(L"");
					name.vt=VT_BSTR;
					name.bstrVal=bstrTemp;
					pSiModel->get_Name (&name.bstrVal);
					_bstr_t tempName=name.bstrVal;

					char *strName = tempName;
					aMSHModel->SetConstraint (strName);

					::SysFreeString (bstrTemp);  
					pSiModel->Release();
				}
			}
		}
	}
}
*/

ZeroImportExportNode *CScene2MSH::CreateModel(CZeroData *pData, X3DObject *pModel, Geometry *pGeometry, X3DObject *pParent,ZeroImportExportNode *modelParentDataBase,Scene *aScene)
{
	VARIANT name;
	name.vt=VT_BSTR;
	name.bstrVal=::SysAllocString(sBSTRInit);
	SIObject *pSIModel=NULL;
	if(SUCCEEDED(pModel->QueryInterface(IID_SIObject,(void **)&pSIModel)))
	{
		pSIModel->get_FullName(&name.bstrVal);
		pSIModel->Release();
	}
	_bstr_t tempName(name.bstrVal,true);
	::SysFreeString (name.bstrVal);  
	unsigned int crc=0;
	crc=CalcStrCRC(tempName,crc);


	CMSHBlockModel *pMSHModel = pData ->AddModel();
    ZeroImportExportNode *modelNode;
    modelNode = new (false) ZeroImportExportNode (pMSHModel, (void *)crc);
    modelNode->Attach (modelParentDataBase);
    modelNode->Release();

/*
	SetPoseConstraint (pModel, pMSHModel);
*/


	// start model block
	// MODEL NAME
	name.bstrVal=::SysAllocString(sBSTRInit);
	GetParameter(pModel,"name",m_CurrentFrame,&name);
	tempName=name.bstrVal;
	pMSHModel->SetName((char *)tempName);
	::SysFreeString (name.bstrVal);  
	char msg[256];
	sprintf(msg,"Getting Model (%s)",pMSHModel->GetName());
	m_ProgDlg->SetProgress(msg,10);

	// PARENT NAME
	if (modelParentDataBase->GetMshModel())
	{
		pMSHModel->SetParentName((char *)modelParentDataBase->GetMshModel()->GetName());
	}

	Property *visProp;
	GetPropertyByType(pModel,"visibility",&visProp);
	if(visProp)
	{
		VARIANT varVisible;
		varVisible.vt=VT_BOOL;
		GetParameter(visProp,"viewvis",m_CurrentFrame,&varVisible);
		if(!varVisible.boolVal)
			pMSHModel->SetHidden(true);
	}


	//	LOCAL TRANSFORMATION
	Property *pBasePose=NULL;
	SceneItem *pSceneItemModel=NULL;

	m_ProgDlg->SetProgress(NULL,15);
	Parameter *pLocalTransform=NULL;
	Kinematics *kine=NULL;
	if(SUCCEEDED(pModel->get_Kinematics(&kine)))
	{
		IDispatch *pDisp;
		HRESULT hr;
//		if(!modelParentDataBase->GetMshModel())
//			hr=kine->get_Global(&pDisp);
//		else
		hr=kine->get_Local(&pDisp);
		if(SUCCEEDED(hr))
		{
			pDisp->QueryInterface(IID_Parameter,(void **)&pLocalTransform);
			pDisp->Release();
		}
		kine->Release();
	}

	if(pLocalTransform || pBasePose)
	{
		VARIANT posX,posY,posZ;
		VARIANT sclX,sclY,sclZ;
		VARIANT rotX,rotY,rotZ;
		VARIANT quatX,quatY,quatZ,quatW;
		quatX.vt=quatY.vt=quatZ.vt=quatW.vt=posX.vt=posY.vt=posZ.vt=sclX.vt=sclY.vt=sclZ.vt=rotX.vt=rotY.vt=rotZ.vt=VT_R8;
		sclX.dblVal=sclY.dblVal=sclZ.dblVal=1.f;
		bool bGotBasePose=false;
		bool useBasePose=false;

		if(m_Deformers.Find(crc) && !mSkipBasepose)
			useBasePose=true;


		ZeroMatrix matrix;
		if(useBasePose && !mBaseposeFromCurrentFrame && modelParentDataBase->GetMshModel())
			bGotBasePose=GetBasePose(pModel,matrix,sclX,sclY,sclZ,modelParentDataBase);

		GetParameter(pLocalTransform,"posx",m_CurrentFrame,&posX);
		GetParameter(pLocalTransform,"posy",m_CurrentFrame,&posY);
		GetParameter(pLocalTransform,"posz",m_CurrentFrame,&posZ);
		GetParameter(pLocalTransform,"rotx",m_CurrentFrame,&rotX);
		GetParameter(pLocalTransform,"roty",m_CurrentFrame,&rotY);
		GetParameter(pLocalTransform,"rotz",m_CurrentFrame,&rotZ);
		if(!bGotBasePose)
		{
			GetParameter(pLocalTransform,"sclx",m_CurrentFrame,&sclX);
			GetParameter(pLocalTransform,"scly",m_CurrentFrame,&sclY);
			GetParameter(pLocalTransform,"sclz",m_CurrentFrame,&sclZ);
		}
		ZeroMatrix pitchMatrix(PitchMatrix ((float)rotX.dblVal * DEG2RAD));
		ZeroMatrix yawMatrix (YawMatrix ((float)rotY.dblVal * DEG2RAD));
		ZeroMatrix rollMatrix (RollMatrix ((float)rotZ.dblVal * DEG2RAD));

		ZeroMatrix localMatrix=pitchMatrix *yawMatrix *rollMatrix;
		localMatrix.posit=ZeroVector3((float)posX.dblVal,(float)posY.dblVal,(float)posZ.dblVal);
		if(!bGotBasePose)
		{
			matrix=localMatrix;
		}
		else
		{
			ChainRoot *pChainRoot;
			if(SUCCEEDED(pModel->QueryInterface(IID_ChainRoot,(void **)&pChainRoot)))
			{
				float err=0.f;
				ZeroMatrix testM=localMatrix.Inverse()*matrix;
				err+=fabsf(1.f-testM.right.x);
				err+=fabsf(testM.right.y);
				err+=fabsf(testM.right.z);
				err+=fabsf(testM.up.x);
				err+=fabsf(1.f-testM.up.y);
				err+=fabsf(testM.up.z);
				err+=fabsf(testM.front.x);
				err+=fabsf(testM.front.y);
				err+=fabsf(1.f-testM.front.z);
				err+=fabsf(testM.posit.x);
				err+=fabsf(testM.posit.y);
				err+=fabsf(testM.posit.z);
				if(err>0.1)
				{
					char s[256];
					sprintf(s,"Local transform for [%s] is different from its basepose.\nIf this is not fixed, then exported animations will be different from what you see in XSI.",pMSHModel->GetName());
					if(m_bWarningsOn)
						MessageBox(0,s,"Warning!",MB_OK|MB_SYSTEMMODAL);
				}
				pChainRoot->Release();
			}
		}


		pMSHModel->SetScale((float)sclX.dblVal,(float)sclY.dblVal,(float)sclZ.dblVal);

		m_ProgDlg->SetProgress(NULL,20);

//		ZeroVector4 pos((float)posX.dblVal,(float)posY.dblVal,(float)posZ.dblVal);
/*
		//
		//	convert global to local coordinates
		//
		if(modelParentDataBase->GetMshModel())
		{
			RecursiveTransform(modelParentDataBase,&matrix,&pos);
		}
*/
		ZeroQuaternion q(matrix);
		pMSHModel->SetRotation(q.x ,q.y ,q.z ,q.s);
		pMSHModel->SetTranslation(matrix.posit.x,matrix.posit.y,matrix.posit.z);

		if(pLocalTransform)
			pLocalTransform->Release();
	}

	m_ProgDlg->SetProgress(NULL,25);
	//	GEOMETRY
	if(pGeometry)
	{
		ObjClsAttrib key;
		key.objNameCRC=0;
		key.objNameCRC = CalcStrCRC(pMSHModel->GetName(),key.objNameCRC);
		CreateGeometry(pGeometry,pData,pMSHModel,pModel,aScene,&key);
	}

	// *** Special stuff for SpecWar ***
	// If model name falls under the category of special collision proxy, get the
	// extended procedural primitive information (if we can) --DY

	// First, we need the IDispatch interface so we can call the GetValue command.  But
	// which one?  The global "toplevel" Application one (this is the only one that
	// will call commands...don't try to convert X3DObject into a IDispatch interface).
	// As with any properly formed COM interface, you can get the global App interface
	// from any sub-interface

	// if name starts with "p_", treat it as a proxy.  This is important -- artists need 
	// to be aware of this, of course

	// Also, we can only really get sphere or cylinder primitives from XSI.  For ellipsoids,
	// we assume the artist made a sphere (non-frozen) with non-uniform scale, and process
	// accordingly

	if (::strncmp(pMSHModel->GetName(),"p_", 2) == 0)
	{
		HRESULT hr;
		DISPID dispid;			// id of method/property
		DISPPARAMS dispparams; // arguments for method call
		VARIANT varResult;
		EXCEPINFO ExcepInfo;
		unsigned int argErr;
		VARIANT sclX,sclY,sclZ;
		sclX.vt = sclY.vt = sclZ.vt = VT_R8;

		SIObject * pSI = NULL;
		Parameter * pGlobalTransform = NULL;
		Kinematics * pKine = NULL;
		IDispatch * pDispGlobal;

		if (SUCCEEDED(pModel->QueryInterface(IID_SIObject,(void **)&pSI)) && pSI &&
			SUCCEEDED(pModel->get_Kinematics(&pKine)) && pKine &&
			SUCCEEDED(pKine->get_Global(&pDispGlobal)) && pDispGlobal &&
			SUCCEEDED(pDispGlobal->QueryInterface(IID_Parameter,(void **)&pGlobalTransform)) && pGlobalTransform
			)
		{
			Application * pApp = NULL;
			pSI->get_Application(&pApp);
			if (pApp != NULL)
			{
				bool bGotPrimitive = false;
				pMSHModel->SetSWCICollisionObject(true); // false by default
				// test each possible primitive and use the first one we find

				// test for sphere...
				varResult.vt = VT_R8; // 64-bit float (verified in testing that this is what XSI returns)
				dispparams.rgvarg = new VARIANT[1];
				OLECHAR FAR* szGetValue = OLESTR("GetValue");
				dispparams.rgvarg[0].vt = VT_BSTR;
				dispparams.rgvarg[0].bstrVal = _bstr_t(pMSHModel->GetName()) + _bstr_t(".primitive.sphere.radius");
				pApp->GetIDsOfNames(IID_NULL,&szGetValue,1,LOCALE_SYSTEM_DEFAULT,&dispid);
				dispparams.cArgs = 1;
				dispparams.cNamedArgs = 0;
				hr = pApp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,DISPATCH_METHOD,&dispparams,&varResult,&ExcepInfo,&argErr);
				delete[] dispparams.rgvarg;
				if (SUCCEEDED(hr))
				{
					// if it's a sphere, it could be an ellisoid!  Well, so to speak...
					bool bIsEllipsoid = false;

					// Check for non-uniform scale
					sclX.dblVal = 0.0; // init the variant...will this work?
					
					// ahk, have to use m_CurrentFrame, but what is it?  Well, hopefully this gets called early on...
					if (GetParameter(pGlobalTransform,"sclx",m_CurrentFrame,&sclX) &&
						GetParameter(pGlobalTransform,"scly",m_CurrentFrame,&sclY) &&
						GetParameter(pGlobalTransform,"sclz",m_CurrentFrame,&sclZ))
					{
						if (!(sclX.dblVal == sclY.dblVal && sclX.dblVal == sclZ.dblVal))
						{
							// we appear to have an ellipsoid.  use scale as multiplier on radius
							float fOrigRad = (float)varResult.dblVal;
							pMSHModel->SetSWCICollisionType(CMSHBlockModel::PST_ELLIPSOID);
							pMSHModel->SetSWCIRadius((float)sclX.dblVal * fOrigRad);
							pMSHModel->SetSWCIHeight((float)sclY.dblVal * fOrigRad);
							pMSHModel->SetSWCILength((float)sclZ.dblVal * fOrigRad);
							bGotPrimitive = true;
							bIsEllipsoid = true;
						}
					}
					if (!bIsEllipsoid)
					{
						// it's a sphere
						pMSHModel->SetSWCICollisionType(CMSHBlockModel::PST_SPHERE);
						float fNewRad = (float)varResult.dblVal;
						if (sclX.dblVal != 0.0)
							fNewRad *= (float)sclX.dblVal;
						pMSHModel->SetSWCIRadius(fNewRad);
						pMSHModel->SetSWCIHeight(0.0f);
						pMSHModel->SetSWCILength(0.0f);
						bGotPrimitive = true;
					}
				}

				if (!bGotPrimitive)
				{
					// test for cylinder
					float fRad, fHeight;
					varResult.vt = VT_R8; // 64-bit float (verified in testing that this is what XSI returns)
					dispparams.rgvarg = new VARIANT[1];
					dispparams.rgvarg[0].vt = VT_BSTR;
					dispparams.rgvarg[0].bstrVal = _bstr_t(pMSHModel->GetName()) + _bstr_t(".primitive.cylinder.radius");
					pApp->GetIDsOfNames(IID_NULL,&szGetValue,1,LOCALE_SYSTEM_DEFAULT,&dispid);
					dispparams.cArgs = 1;
					dispparams.cNamedArgs = 0;
					hr = pApp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,DISPATCH_METHOD,&dispparams,&varResult,&ExcepInfo,&argErr);
					delete[] dispparams.rgvarg;
					if (SUCCEEDED(hr))
					{
						fRad = (float)varResult.dblVal;
						// make sure we can also get the cylinder height
						varResult.vt = VT_R8; // 64-bit float (verified in testing that this is what XSI returns)
						dispparams.rgvarg = new VARIANT[1];
						dispparams.rgvarg[0].vt = VT_BSTR;
						dispparams.rgvarg[0].bstrVal = _bstr_t(pMSHModel->GetName()) + _bstr_t(".primitive.cylinder.height");
						pApp->GetIDsOfNames(IID_NULL,&szGetValue,1,LOCALE_SYSTEM_DEFAULT,&dispid);
						dispparams.cArgs = 1;
						dispparams.cNamedArgs = 0;
						hr = pApp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,DISPATCH_METHOD,&dispparams,&varResult,&ExcepInfo,&argErr);
						delete[] dispparams.rgvarg;
						if (SUCCEEDED(hr))
						{
							fHeight = (float)varResult.dblVal;
							pMSHModel->SetSWCICollisionType(CMSHBlockModel::PST_CYLINDER);
							pMSHModel->SetSWCIRadius(fRad);
							pMSHModel->SetSWCIHeight(fHeight);
							pMSHModel->SetSWCILength(0.0f);
							bGotPrimitive = true;
						}
					}
				}

				if (!bGotPrimitive)
				{
					// test for box
					float fWidth,fHeight,fDepth;
					varResult.vt = VT_R8; // 64-bit float (verified in testing that this is what XSI returns)
					dispparams.rgvarg = new VARIANT[1];
					dispparams.rgvarg[0].vt = VT_BSTR;
					dispparams.rgvarg[0].bstrVal = _bstr_t(pMSHModel->GetName()) + _bstr_t(".primitive.cube.length");
					pApp->GetIDsOfNames(IID_NULL,&szGetValue,1,LOCALE_SYSTEM_DEFAULT,&dispid);
					dispparams.cArgs = 1;
					dispparams.cNamedArgs = 0;
					hr = pApp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,DISPATCH_METHOD,&dispparams,&varResult,&ExcepInfo,&argErr);
					delete[] dispparams.rgvarg;
					if (SUCCEEDED(hr))
					{
						fWidth = fHeight = fDepth = (float)varResult.dblVal;
						GetParameter(pGlobalTransform,"sclx",m_CurrentFrame,&sclX);
						GetParameter(pGlobalTransform,"scly",m_CurrentFrame,&sclY);
						GetParameter(pGlobalTransform,"sclz",m_CurrentFrame,&sclZ);

						fWidth*=(float)sclX.dblVal;
						fHeight*=(float)sclY.dblVal;
						fDepth*=(float)sclZ.dblVal;

						pMSHModel->SetSWCICollisionType(CMSHBlockModel::PST_BOX);
						pMSHModel->SetSWCIRadius(0.5f*fWidth);
						pMSHModel->SetSWCIHeight(0.5f*fHeight);
						pMSHModel->SetSWCILength(0.5f*fDepth);
						bGotPrimitive = true;
					}
				}

				if (!bGotPrimitive)
				{
					// none of the above primitive tests succeeded, so assume a collision mesh for now
					pMSHModel->SetSWCICollisionType(CMSHBlockModel::PST_MESH);
					pMSHModel->SetSWCIRadius(0.0f);
					pMSHModel->SetSWCIHeight(0.0f);
					pMSHModel->SetSWCILength(0.0f);
				}
				pApp->Release();
			}
			pSI->Release();
			pKine->Release();
			pGlobalTransform->Release();
			pDispGlobal->Release();
		}
	}

	m_ProgDlg->SetProgress(NULL,100);
	return modelNode;
}

//void CScene2MSH::SplitSegments(CZeroData *pData)
//{
//	int i;
//	int nModels;
//	CMSHBlockModel **ppModels;
//	
//	ppModels = pData ->GetModels(nModels);
//	for( i = 0; i < nModels; i++) 
//	{
//		ppModels[i]->SpliteSegments ();
//	}
//}

void CScene2MSH::RecursiveTransform(ZeroImportExportNode *node,ZeroMatrix *m,ZeroVector4 *v)
{
	if(node->GetParent())
		RecursiveTransform(node->GetParent(),m,v);

	if(node->GetMshModel())
	{
		ZeroMatrix matrix=node->GetMatrix();
		*v=matrix.UntransformVector(*v);


		matrix=matrix.Inverse();
		*m=*m*matrix;
	}
}

void CScene2MSH::CreateSkeletonAndAnimations(Scene *pScene)
{

	CMSHBlockAnimation *animation;

	ParameterCollection *pAnimatedParameters=NULL;
	long nAnimatedParameters=0;
	Model *pRootModel=NULL;
	ProjectItem *pRootProjItem=NULL;

	pScene->get_Root(&pRootModel);
	pRootModel->QueryInterface(IID_ProjectItem,(void **)&pRootProjItem);
	pRootProjItem->AnimatedParameters(siAnySource,&pAnimatedParameters);
	VARIANT_BOOL bAnimated = VARIANT_FALSE;
	pRootProjItem->IsAnimated(siAnySource, &bAnimated);
	pRootProjItem->Release();
	if (pAnimatedParameters)
		pAnimatedParameters->get_Count(&nAnimatedParameters);

	// NOTE: For some reason, in some cases, scenes can fail to return a
	// AnimatedParameters interface, but still be "animated".  Shrug. --DY
	if(!nAnimatedParameters && bAnimated == VARIANT_FALSE)
		return;
	
	// create the skeleton for the object
	m_ProgDlg->SetProgress("Creating Skeleton ...",0);
	CreateSkeleton (mDatabase);
	
	// if the object has skeleton then create all animations
	if (mZeroData->GetSkeleton()) 
	{
		// creata the animation list
		CMSHBlock::BlockHeader hdr;
		hdr.size=0;
		mZeroData->CreateNewAnimation(&hdr);
		animation = mZeroData->GetAnimation();
		AnimationUserDataElem element;
		
		// create the animation
		m_ProgDlg->SetProgress("Creating Animations ...",10);
		CreateAnimations ();
		PropertyCollection *props;
		SceneItem *pRootItem=NULL;
		bool bAddedCycles=false;
		if(SUCCEEDED(pRootModel->QueryInterface(IID_SceneItem,(void **)&pRootItem)))
		{
			pRootItem->get_Properties(&props);
			long nProps=0;
			props->get_Count(&nProps);
			VARIANT varIdx;
			varIdx.vt=VT_I4;
			char sName[256];
			for(long idx=0;idx<nProps;idx++)
			{
				Property *prop=NULL;
				varIdx.lVal = idx;
				props->get_Item(varIdx,&prop);
				if(prop)
				{
					GetSIObjectName(prop,sName,255);
					if(strnicmp(sName,"Animation_Cycle",15)==0)
					{
						VARIANT tempname;
						tempname.vt=VT_BSTR;
						tempname.bstrVal= ::SysAllocString(sBSTRInit);
						GetParameter(prop,"Animation_Name",m_CurrentFrame,&tempname);
						_bstr_t animName(tempname.bstrVal,true);
						::SysFreeString (tempname.bstrVal);

						VARIANT varStart,varEnd;
						varStart.vt=varEnd.vt=VT_I2;
						GetParameter(prop,"Start_Frame",m_CurrentFrame,&varStart);
						GetParameter(prop,"End_Frame",m_CurrentFrame,&varEnd);

						VARIANT varFrameRate;
						varFrameRate.vt=VT_R8;
						GetParameter(prop,"Frames_Second",m_CurrentFrame,&varFrameRate);

						VARIANT varPlayStyle;
						varPlayStyle.vt=VT_UI1;
						GetParameter(prop,"Play_Style",m_CurrentFrame,&varPlayStyle);
						//
						//	if is a looped animation and we want to clip looped animations then
						//	set the end of the cycle to frame number n-1
						if((mClipLoopedAnimations) && (varPlayStyle.bVal==1))
						{
							varEnd.iVal= (unsigned short) (varEnd.iVal-1);
						}
						animation->AddCycle(animName,(float)varFrameRate.dblVal,varStart.iVal,varEnd.iVal,varPlayStyle.bVal);
						bAddedCycles = true;
					}

					prop->Release();
				}
			}
			pRootItem->Release();
		}

		if(!bAddedCycles)
		{
			// save as one big animation
			strcpy(element.m_AnimationName,"FullAnimation");
			animation->AddCycle(element.m_AnimationName, m_FrameRate, 
				(int)m_StartFrame, (int)m_EndFrame, element.m_PlayStyle);
		}
		m_ProgDlg->SetProgress(NULL,100);
	}

}

void CScene2MSH::CreateSkeleton (ZeroImportExportNode *node)
{   
	if (node->GetMshModel()) 
	{
		CreateIKChain (node);
	}
	for (node = node->GetChild(); node; node = node->GetSibling()) 
	{
		CreateSkeleton (node);
	}
}


bool CScene2MSH::IsMeshAnimated(X3DObject *pModel)
{
	VARIANT_BOOL retval = VARIANT_FALSE;
	Kinematics *pKine=NULL;
	pModel->get_Kinematics(&pKine);
	if(pKine)
	{
/*
		IDispatch *disp=NULL;
		pKine->get_Local(&disp);
		if(disp)
		{
			VARIANT valPosX,valPosY,valPosZ,valRotX,valRotY,valRotZ,val;
			valPosX.vt=valPosY.vt=valPosZ.vt=valRotX.vt=valRotY.vt=valRotZ.vt=val.vt=VT_R8;
			GetParameter(disp,"posx",m_StartFrame,&valPosX);
			GetParameter(disp,"posy",m_StartFrame,&valPosY);
			GetParameter(disp,"posz",m_StartFrame,&valPosZ);
			GetParameter(disp,"rotx",m_StartFrame,&valRotX);
			GetParameter(disp,"roty",m_StartFrame,&valRotY);
			GetParameter(disp,"rotz",m_StartFrame,&valRotZ);
			for(int i=m_StartFrame+1;i<=m_EndFrame;i++)
			{
				GetParameter(disp,"posx",(float)i,&val);
				if(fabs(val.dblVal-valPosX.dblVal)>0.005)
					return true;
				GetParameter(disp,"posy",(float)i,&val);
				if(fabs(val.dblVal-valPosY.dblVal)>0.005)
					return true;
				GetParameter(disp,"posz",(float)i,&val);
				if(fabs(val.dblVal-valPosZ.dblVal)>0.005)
					return true;
				GetParameter(disp,"rotx",(float)i,&val);
				if(fabs(val.dblVal-valRotX.dblVal)>0.005)
					return true;
				GetParameter(disp,"roty",(float)i,&val);
				if(fabs(val.dblVal-valRotY.dblVal)>0.005)
					return true;
				GetParameter(disp,"rotz",(float)i,&val);
				if(fabs(val.dblVal-valRotZ.dblVal)>0.005)
					return true;
			}
		}
*/

		ProjectItem *pPIKine=NULL;
		pKine->QueryInterface(IID_ProjectItem,(void **)&pPIKine);
		if(pPIKine)
		{
			
			pPIKine->IsAnimated(siAnySource,&retval);
			pPIKine->Release();
		}
/*
		if(retval==VARIANT_FALSE)
		{
			ConstraintCollection *constraints=NULL;
			pKine->get_Constraints(&constraints);
			long nCns=0;
			constraints->get_Count(&nCns);
			if(nCns>0)
				retval=VARIANT_TRUE;
		}
*/
	}

	if(retval==VARIANT_TRUE)
		return true;
	else
		return false;
}

float CScene2MSH::SolveConstraint (X3DObject* effector)
{
	X3DObject *parentBone=NULL;
	effector->get_Parent(&parentBone);

	VARIANT rotX;
	VARIANT rotY;
	VARIANT rotZ;
	rotZ.dblVal=0.f;
	rotZ.vt=VT_R8;
	if(parentBone)
	{
		Parameter *pLocalTransform=NULL;
		Kinematics *kine=NULL;
		if(SUCCEEDED(parentBone->get_Kinematics(&kine)))
		{
			IDispatch *pDisp;
			if(SUCCEEDED(kine->get_Local(&pDisp)))
			{
				pDisp->QueryInterface(IID_Parameter,(void **)&pLocalTransform);
				if(pLocalTransform)
				{
					for(int i=(int)m_StartFrame; i<m_EndFrame;i++)
					{
						rotX.vt=rotY.vt=rotZ.vt=VT_R8;
						GetParameter(pLocalTransform,"rotx",(float)i,&rotX);
						GetParameter(pLocalTransform,"roty",(float)i,&rotY);
						GetParameter(pLocalTransform,"rotz",(float)i,&rotZ);
						if (fabs (rotZ.dblVal) > 1.0) 
						{
							break;
						}
					}
					pLocalTransform->Release();
				}
			}
		}
	}
	return ((float)rotZ.dblVal >= 0.0) ? 1.0f : -1.0f;
}


void CScene2MSH::CreateIKChain (ZeroImportExportNode *node)
{
	int jointCount;
	float constraintSign;
	CMSHBlockSkeleton* mshSkeleton;
	unsigned modelCRC;
	X3DObject *model=NULL;

	ChainRoot *pChainRoot=NULL;
	ChainBone *pChainBone=NULL;
	ChainEffector *pChainEffector=NULL;
	
	modelCRC = (unsigned)node->GetCgcModel();
	
//	if (IsConstraint(modelCRC)) 
/*
	if(mFilterPoseCNSAnims)
	{
		if (node->GetMshModel() && node->GetMshModel()->GetConstraint()) {
			return; 
		}
	}
*/

	ModelByCRC::ZeroTreeNode *modelnode=m_AllModels.Find(modelCRC);
	if(modelnode) {
		model=modelnode->info;
	}

	if(!model) {
		return;
	}



	constraintSign = 1.0;
	jointCount = 0;
	switch (node->GetMshModel()->GetModelType()) 
	{
	case Static_Node: 
		{
			if (!IsMeshAnimated(model)) 
			{
				return;
			}
			break;
		}
		
	case Null_Node:
		{
			pChainRoot=NULL;
			pChainBone=NULL;
			pChainEffector=NULL;

			model->QueryInterface(IID_ChainRoot,(void **)&pChainRoot);
			model->QueryInterface(IID_ChainBone,(void **)&pChainBone);

			if(pChainRoot || pChainBone)
			{
				if(pChainRoot)
				{
					pChainRoot->Release();
					if(IsMeshAnimated(model))
					{
						// If we're exporting sampled FK only, this warning is unecessary --DY
						if (!mExportFKAnimation)
						{
							char s[256];
							sprintf(s,"[%s] is an animated skeleton root.\nThe MSH file format does not support animation of skeleton roots.\nThis animation will be removed in the exported file.",node->GetMshModel()->GetName());
							if(m_bWarningsOn)
								MessageBox(0,s,"Warning!",MB_OK|MB_SYSTEMMODAL);
						}
					}
				}
				if(pChainBone)
					pChainBone->Release();
				return;
			}

			model->QueryInterface(IID_ChainEffector,(void **)&pChainEffector);
			if(pChainEffector)
			{
				X3DObject *effparent;
				ChainBone *effParentBone;
				if(SUCCEEDED(model->get_Parent(&effparent)))
				{
					if(effparent)
					{
						if(FAILED(effparent->QueryInterface(IID_ChainBone,(void **)&effParentBone)))
						{
							char s[256];
							sprintf(s,"[%s] is an effector, but it's not a child of a bone.\nThe MSH file format does not support animation of effectors when the parent is not a bone.\nThis animation will be removed in the exported file.",node->GetMshModel()->GetName());
							if(m_bWarningsOn)
								MessageBox(0,s,"Warning!",MB_OK|MB_SYSTEMMODAL);
							pChainEffector->Release();
							return;
						}
						else
						{
							effParentBone->Release();
						}
					}
				}
				ChainElement *pChainElement=NULL;
				pChainEffector->QueryInterface(IID_ChainElement,(void **)&pChainElement);
				if(pChainElement)
				{
					ChainBoneCollection *bones=NULL;
					long nBones=0;
					pChainElement->get_Root(&pChainRoot);
					if(pChainRoot)
						pChainRoot->get_Bones(&bones);
					if(bones)
						bones->get_Count(&nBones);

					_ASSERTE (nBones<=3);
					jointCount=nBones;
					if(nBones>=2)
					{
						constraintSign = SolveConstraint(model);
					}
					pChainElement->Release();
				}
				pChainEffector->Release();
				break;
			}
			else
			{
				if(IsMeshAnimated(model))
					break;
			}
			return;

			break;
		}
	default:
		{
			return;
		}
	} 
	
	mshSkeleton = mZeroData->GetSkeleton();
	if (!mExportFKAnimation)
	{
		mshSkeleton->CreateEffector (node, jointCount, constraintSign); 
	}
	else
	{
		// The following is a code change from Julio -- what we effectively do is
		// get animation info from end effectors as normal, but as we export, we 
		// pretend that the one or two bone roots of the end effectors are also
		// "end effectors", all with 0 joints.  This way, we'll just end up 
		// sampling matrix data for each and every joint during export.

		// Perform some error checking on the effector.
		if (jointCount > 0)
		{
			ZeroImportExportNode *pParent = node;
			bool bIncomplete = false;
			// Primitive error checking:
			// The chain has jointCount + 1 nodes above this one (if valid), 
			// and the root should have a parent, so check for jointCount + 2 
			// parent nodes.
			for (int i = 0; i < jointCount + 2; i++)
			{
				pParent = pParent->GetParent();
				if (!pParent)
				{
					bIncomplete = true;
					break;
				}
			}
			
			if (bIncomplete)
			{
				::MessageBox(NULL, "Warning: Incomplete bone chain detected. Make sure all bone chains are selected at the root.", "Exporter Error", MB_OK | MB_ICONERROR);
				return;
			}
		}

		switch (jointCount) {
			case 0:
			{
				mshSkeleton->CreateEffector (node, 0, constraintSign); 
				break;
			}

			case 1:
			{
				mshSkeleton->CreateEffector (node->GetParent(), 0, constraintSign); 
				break;
			}


			case 2:
			{
				mshSkeleton->CreateEffector (node->GetParent()->GetParent(), 0, constraintSign); 
				mshSkeleton->CreateEffector (node->GetParent(), 0, constraintSign); 
				break;
			}

			case 3:
				mshSkeleton->CreateEffector (node->GetParent()->GetParent()->GetParent(), 0, constraintSign); 
				mshSkeleton->CreateEffector (node->GetParent()->GetParent(), 0, constraintSign); 
				mshSkeleton->CreateEffector (node->GetParent(), 0, constraintSign); 
				break;

			default:
				::MessageBox(NULL, "Warning: IK Chain detected with more than 3 bones, will not be exported!", "Exporter Error", MB_OK | MB_ICONWARNING);
				break;
		}
	}
}

void CScene2MSH::CreateAnimations()
{
	CMSHBlockSkeleton::Iterator iter (*mZeroData->GetSkeleton());
	unsigned int nTotal=mZeroData->GetSkeleton()->GetCount();
	unsigned int i=1;
	for (iter.Begin(); iter; iter ++) {
		m_ProgDlg->SetProgress(NULL,10+i*90/nTotal);
		CreateAnimation (&(*iter));
		i++;
	}
}

void CScene2MSH::CreateAnimation(CMSHBlockEndEffector *effector)
{
   ZeroImportExportNode* node;
	
   if (effector) 
   {
	   node = mDatabase->FindByMSHModel (effector->GetModel());
		switch (effector->GetBoneCount()) 
		{
		case 0:
			CreateFreeJointAnimation (node);
			break;
			
		case 1:
		case 2:
			{
				ZeroImportExportNode *child=node->GetChild();
				bool bDone=false;

				while(child && !bDone)
				{
					X3DObject *model=NULL;
					unsigned crc;
					crc=(unsigned)child->GetCgcModel();
					ModelByCRC::ZeroTreeNode *modelnode=m_AllModels.Find(crc);
					if(modelnode)
					{
						model=modelnode->info;
						ChainEffector *pEffector=NULL;
						if(SUCCEEDED(model->QueryInterface(IID_ChainEffector,(void **)&pEffector)))
						{
							if(effector->GetBoneCount()==1)
							{
								CreateSingleIKAnimation (child);
							}
							else if(effector->GetBoneCount()==2)
							{
								CreateDoubleIKAnimation (child);
							}
							pEffector->Release();
							bDone=true;
						}
					}
					child=child->GetSibling();
				}
			}
			break;
		}
   } 
}




ZeroVector4 CScene2MSH::GetEulerFromPitchYawRollMatrix(const ZeroMatrix& matrix)
{
	ZeroVector4 euler (matrix.CalcPitchYawRollEulerAngles());
	return euler.Scale (RAD2DEG);
}

void CScene2MSH::AlignBonesChainRoot(ZeroImportExportNode *node)
{
//	CGCNull *cgcNull;
//	CGCFCurve *boneRot;
//	PMARKER	boneMarker;
//	CGCRotationKey *boneAngles;
//	CGCSceneObject *cgcModel;
	CMSHBlockModel *effector;
	ZeroImportExportNode *root;
	ZeroImportExportNode *bone;
	CMSHBlockModel *mshBoneModel;
	CMSHBlockModel *mshRootModel;

	effector = node->GetMshModel();
	if (effector) 
	{
		X3DObject *model=NULL;
		unsigned crc;
		crc=(unsigned)node->GetCgcModel();
		ModelByCRC::ZeroTreeNode *modelnode=m_AllModels.Find(crc);
		if(modelnode)
		{
			model=modelnode->info;
			ChainEffector *pEffector=NULL;
			if(SUCCEEDED(model->QueryInterface(IID_ChainEffector,(void **)&pEffector)))
			{
				long numBones=1;
				root=NULL;
				bone = node->GetParent();
				while(root==NULL)
				{
					ChainRoot *pRoot=NULL;
					root = bone->GetParent();

					crc=(unsigned)root->GetCgcModel();
					ModelByCRC::ZeroTreeNode *node=m_AllModels.Find(crc);
					model=node->info;
					if(SUCCEEDED(model->QueryInterface(IID_ChainRoot,(void **)&pRoot)))

					{
						ChainBoneCollection *bones;
						pRoot->get_Bones(&bones);
						bones->get_Count(&numBones);
						pRoot->Release();
					}
					else
					{
						root = NULL;
						bone = bone->GetParent();
					}
				}

				if(numBones==2)
				{
					ZeroVector4 p;
					ZeroQuaternion q;

					//
					//	set root transform to M[bone1]*M[root]
					//
					mshRootModel = root->GetMshModel();
					mshRootModel->GetTrans (p.x, p.y, p.z);
					mshRootModel->GetQuatRot (q.x, q.y, q.z, q.s);
					ZeroMatrix rootMatrix (q, p);

					mshBoneModel = bone->GetMshModel();
					mshBoneModel->GetTrans (p.x, p.y, p.z);
					mshBoneModel->GetQuatRot (q.x, q.y, q.z, q.s);
					ZeroMatrix boneMatrix (q, p);

					rootMatrix = boneMatrix * rootMatrix;
					q = ZeroQuaternion (rootMatrix);
					mshRootModel->SetTranslation (rootMatrix.posit.x, rootMatrix.posit.y, rootMatrix.posit.z);
					mshRootModel->SetRotation (q.x, q.y, q.z, q.s);

					//
					//	save M[bone1] inverse to correct keyframes
					//
					boneMatrix = boneMatrix.Inverse();
					unsigned boneCRC=(unsigned)bone->GetCgcModel();
					BoneMatrixByCRC::ZeroTreeNode *bNode=m_BoneMatrices.Find(boneCRC);
					if(bNode)
					{
						ZeroMatrix curMatrix=bNode->info;
						curMatrix = boneMatrix*curMatrix;
						m_BoneMatrices.Replace(curMatrix,boneCRC);
					}
					else
					{
						m_BoneMatrices.Insert(boneMatrix,boneCRC);
					}
					
					//
					//	set root's children transforms to M[child]*M[bone1].inverse
					//
					ZeroImportExportNode *ptr;
					for (ptr = root->GetChild(); ptr; ptr = ptr->GetSibling()) 
					{
						mshBoneModel = ptr->GetMshModel();           

						mshBoneModel->GetTrans (p.x, p.y, p.z);
						mshBoneModel->GetQuatRot (q.x, q.y, q.z, q.s);
						ZeroMatrix matrix (ZeroMatrix (q, p) * boneMatrix);

						q = ZeroQuaternion (matrix);

						mshBoneModel->SetTranslation (matrix.posit.x, matrix.posit.y, matrix.posit.z);
						mshBoneModel->SetRotation (q.x, q.y, q.z, q.s);
						/*
						fixes keyframes in CreateFreeJointAnimation, SIngleIK, DoubleIK
						*/
					}
				}
				pEffector->Release();
			}
		}
	}
	for (node = node->GetChild(); node; node = node->GetSibling()) 
	{
		AlignBonesChainRoot(node);
	}
}

void CScene2MSH::CreateFreeJointAnimation(ZeroImportExportNode* node)
{
	int i;
	int framesCount=0;
	X3DObject* model; 
	unsigned int modelCRC;
	CMSHBlockModel* mshModel;
	
	CMSHBlockAnimation* animation;
	CMSHBlockNodeAnimation* keyFramesArray;
	
	modelCRC = (unsigned int)node->GetCgcModel();
	const char *sModelName = InvertCRC(modelCRC);

	ModelByCRC::ZeroTreeNode *modelNode;
	modelNode = m_AllModels.Find(modelCRC);
	model=modelNode->info;
	mshModel = node->GetMshModel();
	
	framesCount = (int)m_EndFrame-(int)m_StartFrame+1;

	ZeroStack<CMSHBlockPositionFrame> translationKeyframes (framesCount);
	ZeroStack<CMSHBlockRotationFrame> rotationKeyframes (framesCount);

	framesCount=0;

	ZeroMatrix correctionMatrix(identityMatrix);
	BoneMatrixByCRC::ZeroTreeNode *matNode;
	matNode = m_CorrectionMatrices.Find(modelCRC);
	if(matNode)
	{
		correctionMatrix=matNode->info;
	}

	float fx,fy,fz,fs;
	mshModel->GetQuatRot(fx,fy,fz,fs);
	ZeroQuaternion q0(fs,fx,fy,fz);

	for(i=(int)m_StartFrame; i<=m_EndFrame;i++)
	{
		VARIANT posX,posY,posZ;
		posX.vt=posY.vt=posZ.vt=VT_R8;
		//	position
		translationKeyframes[framesCount].key=i;
		GetParameter(model,"posx",(float)i,&posX);
		GetParameter(model,"posy",(float)i,&posY);
		GetParameter(model,"posz",(float)i,&posZ);
		ZeroVector4 p((float)posX.dblVal,(float)posY.dblVal,(float)posZ.dblVal);
		p=correctionMatrix.RotateVector(p);
		translationKeyframes[framesCount].position = p;

		VARIANT rotX,rotY,rotZ;
		rotX.vt=rotY.vt=rotZ.vt=VT_R8;
		//	rotation
		GetParameter(model,"rotx",(float)i,&rotX);
		GetParameter(model,"roty",(float)i,&rotY);
		GetParameter(model,"rotz",(float)i,&rotZ);
		ZeroMatrix rollMatrix (RollMatrix(AngleMod (rotZ.dblVal) * DEG2RAD));
		ZeroMatrix yawMatrix (YawMatrix(AngleMod (rotY.dblVal) * DEG2RAD));
		ZeroMatrix pitchMatrix (PitchMatrix(AngleMod (rotX.dblVal) * DEG2RAD));
		ZeroMatrix pitchYawRollMatrix (pitchMatrix * yawMatrix * rollMatrix*correctionMatrix);
		if(node->GetParent())
		{
			if(node->GetParent()->GetMshModel())
			{
				//
				//	correct hardpoint for free node animations
				if (mExportLeftHanded)
				{
					pitchYawRollMatrix=YawMatrix(PI)*pitchYawRollMatrix;
				}
			}
		}
		rotationKeyframes[framesCount].rotation = ZeroQuaternion(pitchYawRollMatrix);

		rotationKeyframes[framesCount].key=i;
		framesCount++;
	}

	if(!framesCount)
		return;

	animation = mZeroData->GetAnimation();
	
//	int index,arrayIndex;
//	float alpha;
//	int i0,i1;
	CMSHBlock::BlockHeader hdr;
	hdr.size=0;
	keyFramesArray = animation->CreateKeyFrames (&hdr, mshModel, 0);
	for (i = 0; i < framesCount; i++) 
	{
		keyFramesArray->mPositions[i].key = translationKeyframes[i].key;
		keyFramesArray->mPositions[i].position = translationKeyframes[i].position;
		keyFramesArray->mRotations[i].key = rotationKeyframes[i].key;
		keyFramesArray->mRotations[i].rotation = rotationKeyframes[i].rotation;
	}
	keyFramesArray->SetNumPositions(framesCount);
	keyFramesArray->SetNumRotations(framesCount);
/*
	for (i = 0; i < framesCount-1; i++) 
	{
		arrayIndex = i + translationKeyframes[0].key;
		(*keyFramesArray)[i].key = arrayIndex;
		index = ZeroBinarySearch (&translationKeyframes[0], framesCount, arrayIndex);
		i0 = translationKeyframes[index].key;
		i1 = translationKeyframes[index + 1].key;
		ZeroVector4 p0 (translationKeyframes[index].position);
		ZeroVector4 p1 (translationKeyframes[index + 1].position);
		alpha = (float)(arrayIndex - i0) / (float) (i1 - i0);
		(*keyFramesArray)[i].position = p0.Scale (1.f - alpha) + p1.Scale (alpha);

		arrayIndex = i + rotationKeyframes[0].key;
		_ASSERTE (arrayIndex == (*keyFramesArray)[i].key);
		index = ZeroBinarySearch (&rotationKeyframes[0], framesCount, arrayIndex);
		i0 = rotationKeyframes[index].key;
		i1 = rotationKeyframes[index + 1].key;
		ZeroQuaternion r0 (rotationKeyframes[index].rotation);
		ZeroQuaternion r1 (rotationKeyframes[index + 1].rotation);
		alpha = (float)(arrayIndex - i0) / (float) (i1 - i0);
		(*keyFramesArray)[i].rotation = Interpolate (r0, r1, alpha);
	}
	
	arrayIndex = i + translationKeyframes[0].key;
	(*keyFramesArray)[i].key = arrayIndex;
	index = ZeroBinarySearch (&translationKeyframes[0], framesCount, arrayIndex);
	(*keyFramesArray)[i].position = translationKeyframes[index].position;
	
	arrayIndex = i + translationKeyframes[0].key;
	index = ZeroBinarySearch (&rotationKeyframes[0], framesCount, arrayIndex);
	(*keyFramesArray)[i].rotation = rotationKeyframes[index].rotation;

	keyFramesArray->SetKeyframesCount (framesCount);
*/
}

void CScene2MSH::CreateSingleIKAnimation (ZeroImportExportNode* node)
{
	int i;
//	int i0;
//	int i1;
//	int index;
//	int arrayIndex;
	int framesCount;
	int keyframesCount;
	
//	float alpha;
	X3DObject* bone1; 
	
	CMSHBlockAnimation* animation;
	CMSHBlockNodeAnimation* keyFramesArray;
	
	ZeroVector4 effPos;
	node->GetMshModel()->GetTrans (effPos.x, effPos.y, effPos.z);
	effPos.x = sqrtf (DotProduct (effPos, effPos));
	effPos.y = 0; 
	effPos.z = 0; 
	
	unsigned int bone1CRC;
	bone1CRC = (unsigned int)node->GetParent()->GetCgcModel();
	ModelByCRC::ZeroTreeNode *bone1Node;
	bone1Node = m_AllModels.Find(bone1CRC);
	bone1=bone1Node->info;
	
	animation = mZeroData->GetAnimation();
	
	keyframesCount = (int)m_EndFrame-(int)m_StartFrame+1;
	ZeroStack<CMSHBlockPositionFrame> posframes (keyframesCount+1);
	ZeroStack<CMSHBlockRotationFrame> rotframes (keyframesCount+1);
	
	// calculate effector position by using FK
	i = 0; 
	VARIANT varX,varY,varZ;
	varX.vt=varY.vt=varZ.vt=VT_R8;

	ZeroMatrix boneMatrix(identityMatrix);
	BoneMatrixByCRC::ZeroTreeNode *pMatNode=m_BoneMatrices.Find(bone1CRC);
	if(pMatNode)
	{
		boneMatrix = pMatNode->info;
	}

	ZeroMatrix CorrectionMatrix(identityMatrix);
	BoneMatrixByCRC::ZeroTreeNode *matNode = m_CorrectionMatrices.Find(bone1CRC);
	if(matNode)
	{
		CorrectionMatrix=matNode->info;
	}

	ZeroStack<ZeroVector3> boneRots((int)(m_EndFrame-m_StartFrame+1));
	ZeroVector3 *bRotPtr=&boneRots[0];
	for (int frameNum=(int)m_StartFrame;frameNum<=m_EndFrame;frameNum++)
	{
		varX.vt=varY.vt=varZ.vt=VT_R8;
		GetParameter(bone1,"rotx",(float)frameNum,&varX);
		GetParameter(bone1,"roty",(float)frameNum,&varY);
		GetParameter(bone1,"rotz",(float)frameNum,&varZ);

		/*
		Align bone to chain root
		*/
		ZeroMatrix yawMatrix (YawMatrix((float)varY.dblVal * DEG2RAD));
		ZeroMatrix rollMatrix (RollMatrix((float)varZ.dblVal * DEG2RAD));
		ZeroMatrix pitchMatrix (PitchMatrix((float)varX.dblVal * DEG2RAD));

		ZeroMatrix pyrMatrix(pitchMatrix*yawMatrix*rollMatrix*CorrectionMatrix*boneMatrix);
		ZeroVector4 euler (pyrMatrix.CalcPitchYawRollEulerAngles());

		*bRotPtr++=ZeroVector3(euler.x,euler.y,euler.z);
	}
	ZeroImportExportNode::CalculateSingleIKKeyFrames(&boneRots[0],effPos,(int)m_StartFrame,(int)m_EndFrame,&posframes[0],&rotframes[0]);

	framesCount = posframes[keyframesCount-1].key - posframes[0].key + 1;
	
	CMSHBlock::BlockHeader hdr;
	hdr.size=0;
	keyFramesArray = animation->CreateKeyFrames (&hdr, node->GetParent()->GetMshModel(), 1);
	for (i = 0; i < framesCount; i++) 
	{
		keyFramesArray->mPositions[i].key = posframes[i].key;
		keyFramesArray->mPositions[i].position = posframes[i].position;
		keyFramesArray->mRotations[i].key = rotframes[i].key;
		keyFramesArray->mRotations[i].rotation = rotframes[i].rotation;
	}
	keyFramesArray->SetNumPositions (framesCount);
	keyFramesArray->SetNumRotations (framesCount);
/*
	for (i = 0; i < framesCount-1; i ++) {
		
		arrayIndex = i + keyframes[0].key;
		index = ZeroBinarySearch (&keyframes[0], keyframesCount, arrayIndex);
		i0 = keyframes[index].key;
		i1 = keyframes[index + 1].key;
		
		ZeroVector4 p0 (keyframes[index].position);
		ZeroVector4 p1 (keyframes[index + 1].position);
		
		ZeroQuaternion r0 (keyframes[index].rotation);
		ZeroQuaternion r1 (keyframes[index + 1].rotation);
		
		alpha = (float)(arrayIndex - i0) / (float) (i1 - i0);
		
		(*keyFramesArray)[i].key = arrayIndex;
		(*keyFramesArray)[i].position = p0.Scale (1.f - alpha) + p1.Scale (alpha);
		(*keyFramesArray)[i].rotation = Interpolate (r0, r1, alpha);
	}
	
	arrayIndex = i + keyframes[0].key;
	(*keyFramesArray)[i].key = arrayIndex;
	index = ZeroBinarySearch (&keyframes[0], keyframesCount, arrayIndex);
	ZeroVector4 p0 (keyframes[index].position);
	ZeroQuaternion r0 (keyframes[index].rotation);
	(*keyFramesArray)[i].position = p0;
	(*keyFramesArray)[i].rotation = r0;
*/
}

void CScene2MSH::CreateDoubleIKAnimation (ZeroImportExportNode* node)
{
	int i;
//	int i0;
//	int i1;
//	int index;
//	int arrayIndex;
	int framesCount;
	int keyframesCount;
//	float alpha;

	X3DObject* bone1; 
	X3DObject* bone2; 
	
	CMSHBlockAnimation* animation;
	CMSHBlockNodeAnimation* keyFramesArray;

	ZeroVector4 effPos;
	node->GetMshModel()->GetTrans (effPos.x, effPos.y, effPos.z);
	_ASSERTE (fabs (effPos.y) < 0.001); 
	_ASSERTE (fabs (effPos.z) < 0.001); 

	effPos.y = 0;
	effPos.z = 0;
	
	ZeroVector4 bonePos;
	node->GetParent	()->GetMshModel()->GetTrans (bonePos.x, bonePos.y, bonePos.z);
	_ASSERTE (fabs (bonePos.y) < 0.001); 
	_ASSERTE (fabs (bonePos.z) < 0.001); 
	
	unsigned int bone1CRC;
	bone1CRC = (unsigned int)node->GetParent()->GetParent()->GetCgcModel();
	ModelByCRC::ZeroTreeNode *bone1Node;
	bone1Node = m_AllModels.Find(bone1CRC);
	bone1=bone1Node->info;

	unsigned int bone2CRC;
	bone2CRC = (unsigned int)node->GetParent()->GetCgcModel();
	ModelByCRC::ZeroTreeNode *bone2Node;
	bone2Node = m_AllModels.Find(bone2CRC);
	bone2=bone2Node->info;

	animation = mZeroData->GetAnimation();
	
	keyframesCount = (int)m_EndFrame-(int)m_StartFrame+1;
	ZeroStack<CMSHBlockPositionFrame> posframes (keyframesCount+1);
	ZeroStack<CMSHBlockRotationFrame> rotframes (keyframesCount+1);
	
	// calculate effector position by using FK
	i = 0; 
	VARIANT varbone1X,varbone1Y,varbone1Z;
	VARIANT varbone2X,varbone2Y,varbone2Z;
	varbone1X.vt=varbone1Y.vt=varbone1Z.vt=VT_R8;
	varbone2X.vt=varbone2Y.vt=varbone2Z.vt=VT_R8;
	
	CMSHBlockSkeleton *skeleton = mZeroData->GetSkeleton();
	CMSHBlockEndEffector *effector	= skeleton->FindEffector (node->GetParent()->GetMshModel());

	ZeroMatrix boneMatrix;
	BoneMatrixByCRC::ZeroTreeNode *pMatNode=m_BoneMatrices.Find(bone1CRC);
	if(pMatNode)
	{
		boneMatrix = pMatNode->info;
	}

	ZeroStack<ZeroVector3> bone1Rots((int)(m_EndFrame-m_StartFrame+1));
	ZeroStack<ZeroVector3> bone2Rots((int)(m_EndFrame-m_StartFrame+1));
	ZeroVector3 *b1RotPtr=&bone1Rots[0];
	ZeroVector3 *b2RotPtr=&bone2Rots[0];
	for (int frameNum=(int)m_StartFrame;frameNum<=m_EndFrame;frameNum++)
	{
		varbone1X.vt=varbone1Y.vt=varbone1Z.vt=VT_R8;
		varbone2X.vt=varbone2Y.vt=varbone2Z.vt=VT_R8;
		GetParameter(bone1,"rotx",(float)frameNum,&varbone1X);
		GetParameter(bone1,"roty",(float)frameNum,&varbone1Y);
		GetParameter(bone1,"rotz",(float)frameNum,&varbone1Z);
		GetParameter(bone2,"rotx",(float)frameNum,&varbone2X);
		GetParameter(bone2,"roty",(float)frameNum,&varbone2Y);
		GetParameter(bone2,"rotz",(float)frameNum,&varbone2Z);

		/*
		Align bone to chain root
		*/

		ZeroMatrix bone1YawMatrix (YawMatrix(DEG2RAD * (float)varbone1Y.dblVal));
		ZeroMatrix bone1RollMatrix (RollMatrix(DEG2RAD * (float)varbone1Z.dblVal));
		ZeroMatrix bone1PitchMatrix (PitchMatrix(DEG2RAD * (float)varbone1X.dblVal));

		ZeroMatrix pyrMatrix(bone1PitchMatrix*bone1YawMatrix*bone1RollMatrix*boneMatrix);
		ZeroVector4 euler (pyrMatrix.CalcPitchYawRollEulerAngles());

		*b1RotPtr++=ZeroVector3(euler.x,euler.y,euler.z);
		*b2RotPtr++=ZeroVector3(DEG2RAD*(float)varbone2X.dblVal,DEG2RAD*(float)varbone2Y.dblVal,DEG2RAD*(float)varbone2Z.dblVal);
	}
	ZeroImportExportNode::CalculateDoubleIKKeyFrames(&bone1Rots[0],&bone2Rots[0],bonePos,effPos,effector,(int)m_StartFrame,(int)m_EndFrame,&posframes[0],&rotframes[0]);

	framesCount = posframes[keyframesCount-1].key - posframes[0].key + 1;

	CMSHBlock::BlockHeader hdr;
	hdr.size=0;
	keyFramesArray = animation->CreateKeyFrames (&hdr, node->GetParent()->GetMshModel(), 2);
	for (i = 0; i < framesCount; i++) 
	{
		keyFramesArray->mPositions[i].key = posframes[i].key;
		keyFramesArray->mPositions[i].position = posframes[i].position;
		keyFramesArray->mRotations[i].key = rotframes[i].key;
		keyFramesArray->mRotations[i].rotation = rotframes[i].rotation;
	}
	keyFramesArray->SetNumPositions (framesCount);
	keyFramesArray->SetNumRotations (framesCount);
/*
	for (i = 0; i < framesCount-1; i ++) {
		
		arrayIndex = i + keyframes[0].key;
		index = ZeroBinarySearch (&keyframes[0], keyframesCount, arrayIndex);
		i0 = keyframes[index].key;
		i1 = keyframes[index + 1].key;
		
		ZeroVector4 p0 (keyframes[index].position);
		ZeroVector4 p1 (keyframes[index + 1].position);
		
		ZeroQuaternion r0 (keyframes[index].rotation);
		ZeroQuaternion r1 (keyframes[index + 1].rotation);
		
		alpha = (float)(arrayIndex - i0) / (float) (i1 - i0);
		
		(*keyFramesArray)[i].key = arrayIndex;
		(*keyFramesArray)[i].position = p0.Scale (1.f - alpha) + p1.Scale (alpha);
		(*keyFramesArray)[i].rotation = Interpolate (r0, r1, alpha);
	}
	
	arrayIndex = i + keyframes[0].key;
	(*keyFramesArray)[i].key = arrayIndex;
	index = ZeroBinarySearch (&keyframes[0], keyframesCount, arrayIndex);
	ZeroVector4 p0 (keyframes[index].position);
	ZeroQuaternion r0 (keyframes[index].rotation);
	(*keyFramesArray)[i].position = p0;
	(*keyFramesArray)[i].rotation = r0;
*/
}

void CScene2MSH::CorrectGlobalRotationForSkeleton(ZeroImportExportNode *node,ZeroMatrix oldWorldMatrix,ZeroMatrix newWorldMatrix,bool bForceRotation)
{
	bool bCorrectHardpoint=false;
	CMSHBlockModel *model = node->GetMshModel();
	ZeroVector4 p;
	ZeroQuaternion q;

	#ifdef _DEBUG
	MatrixSanityCheck (newWorldMatrix);
	MatrixSanityCheck (oldWorldMatrix);
	#endif


	bool bPerformRotation = false;
	if(node->GetParent())
	{
		if(node->GetParent()->GetMshModel())
		{
			if(model)
			{
				model->GetTrans (p.x, p.y, p.z);
				model->GetQuatRot (q.x, q.y, q.z, q.s);
				ZeroMatrix preMatrix (q, p);
				ZeroMatrix postMatrix (preMatrix);
				unsigned modelCRC = (unsigned int)node->GetCgcModel();
				
				ModelByCRC::ZeroTreeNode *objNode = m_AllModels.Find(modelCRC);

				if(objNode)
				{
					X3DObject *obj=objNode->info;
					ChainRoot *pRoot=NULL;
					ChainBone *pBone=NULL;
					ChainEffector *pEffector=NULL;

					obj->QueryInterface(IID_ChainRoot,(void **)&pRoot);
					obj->QueryInterface(IID_ChainBone,(void **)&pBone);
					obj->QueryInterface(IID_ChainEffector,(void **)&pEffector);
					
					if(mExportAnimations)
					{
						if (!mExportFKAnimation)
						{
							//
							//	ik animations - align y and x
							if(pRoot)
							{
								bPerformRotation=true;
								pRoot->Release();
							}
							
							if(pBone)
							{
								bPerformRotation=true;
								pBone->Release();
							}
							
							if(pEffector)
							{
								bPerformRotation=true;
								pEffector->Release();
							}
						}

						if(!bPerformRotation)
						{
							if(IsMeshAnimated(obj))
							{
								bPerformRotation=true;
								//
								//	free node animation - correct hardpoint
								if(node->GetParent())
								{
									if(node->GetParent()->GetMshModel())
									{
										bCorrectHardpoint=true;
									}
								}
							}
						}
					}

					MSH_ClothCollisionSphere *sphData;
					MSH_ClothCollisionCylinder *cylData;
					MSH_ClothCollisionCone *coneData;
					model->GetAnimTriggerCollision(&sphData, &cylData, &coneData);	
					if(cylData)
					{
						bPerformRotation = true;
					}

					//
					//	hard points need global rotation
					if(!bPerformRotation)
					{
						if(node->GetParent())
							if(node->GetParent()->GetMshModel())
							{
								bPerformRotation=true;
								bCorrectHardpoint=true;
							}
					}
				}

				if(bPerformRotation)
				{
					postMatrix=preMatrix*oldWorldMatrix*YawMatrix(PI)*newWorldMatrix.Inverse();
					
					//
					//	hard points also get a local rotation of 180
					if(bCorrectHardpoint)
						postMatrix=YawMatrix(PI)*postMatrix;

					q=ZeroQuaternion(postMatrix);
					q = q.Scale (1.0f / q.DotProduct(q));
					postMatrix = ZeroMatrix (q, postMatrix.posit);

					model->SetRotation(q.x,q.y,q.z,q.s);
					p=oldWorldMatrix.RotateVector(p);
					p=newWorldMatrix.Inverse().RotateVector(p);
					model->SetTranslation(p.x,p.y,p.z);
					postMatrix.posit=p;

					ZeroMatrix tmp;
					tmp = oldWorldMatrix * YawMatrix(PI) * newWorldMatrix.Inverse();
					m_CorrectionMatrices.Insert(tmp, modelCRC);
				}
				else
				{
		//			postMatrix=YawMatrix(PI)*preMatrix;
		  			postMatrix = preMatrix/**oldWorldMatrix*newWorldMatrix.Inverse()*/;
					q=ZeroQuaternion(postMatrix);
					q = q.Scale (1.0f / q.DotProduct(q));
					postMatrix = ZeroMatrix (q, postMatrix.posit);

					model->SetRotation(q.x,q.y,q.z,q.s);
					p=oldWorldMatrix.RotateVector(p);
					p=newWorldMatrix.Inverse().RotateVector(p);
					model->SetTranslation(p.x,p.y,p.z);
					postMatrix.posit=p;
		//			m_CorrectionMatrices.Insert(oldWorldMatrix*newWorldMatrix.Inverse(),modelCRC);
				}


				#ifdef _DEBUG
				MatrixSanityCheck (preMatrix);
				MatrixSanityCheck (postMatrix);
				#endif

				newWorldMatrix = postMatrix * newWorldMatrix;
				oldWorldMatrix = preMatrix * oldWorldMatrix;



				int count;
				vect3	*points;
				int segmentCount;
				CMSHBlockModelSegment **segments = model->GetSegments(segmentCount);
				for (int i = 0; i < segmentCount; i ++) 
				{
					ZeroMatrix rotNormals (oldWorldMatrix*newWorldMatrix.Inverse());

					points = segments[i]->GetPositions(count);
					rotNormals.TransformVectors (&points[0].fX, sizeof (vect3), &points[0].fX, sizeof (vect3),	count);

					points = segments[i]->GetNormals(count);
					rotNormals.TransformVectors (&points[0].fX, sizeof (vect3), &points[0].fX, sizeof (vect3),	count);
				}
			}
		}
	}

	ZeroImportExportNode *child;
	for(child=node->GetChild();child;child=child->GetSibling())
	{
		CorrectGlobalRotationForSkeleton(child,oldWorldMatrix,newWorldMatrix,bPerformRotation);
	}
}

