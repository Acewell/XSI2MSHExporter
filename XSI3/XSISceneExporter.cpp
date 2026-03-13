// XSISceneExporter.cpp : Implementation of CXSISceneExporter
#include "stdafx.h"
#include "XSI2MSHExporter.h"
#include "XSISceneExporter.h"
#include "XSI2MSHDialog.h"
#include "XSI2MSHProgressDialog.h"

#import "Libraries/XSISDK/SIobjectmodel.tlb"
#import "Libraries/XSISDK/SI3dobjectmodel.tlb"

#include "Scene2MSH.h"
#include "ZeroData.h"
#include "ZeroExporter.h"
#include <string.h>

/////////////////////////////////////////////////////////////////////////////
// CXSISceneExporter

//CZeroData* gZeroData = NULL;

STDMETHODIMP CXSISceneExporter::ExportScene(IDispatch *pProject, IDispatch *pScene)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CXSI2MSHDialog dlg;
	if(dlg.DoModal()==IDOK)
	{
		Scene *pRootScene;
		Project *pRootProject;
		if(pScene->QueryInterface(IID_Scene,(void **)&pRootScene)==S_OK)
		{
			if(pProject->QueryInterface(IID_Project,(void **)&pRootProject)==S_OK)
			{
				bool bDoIt=true;
				char msg[1024];
				if(_access((LPCTSTR)dlg.mPathName,0)==0)	//	if file exists
				{
					if(_access((LPCTSTR)dlg.mPathName,2)==-1)	//	if write-protected
					{
						MessageBox(0,"The output file is Write-Protected.","Error",MB_OK);
						bDoIt=false;
					}
				}

				if(bDoIt)
				{
					CScene2MSH converter;
					CZeroData mshData;

					converter.SetFilters(dlg.mExportSelectedOnly,dlg.mExportTextures,dlg.mExportAnimations,dlg.mExportLights,false,dlg.m_ClipLoopedAnims,dlg.m_BaseposeFromCurrentFrame,dlg.m_FlattenGeometry,dlg.m_CameraAnims,dlg.m_ExportFKAnimation,dlg.m_SkipBasepose,dlg.m_ExportLeftHanded);
					converter.SetDestinationPath((LPCTSTR)dlg.mPathName);
					converter.SetTextureFormat(dlg.m_TxtFormat);
					converter.SetWarningsOn(!dlg.m_IgnoreWarnings);

					CXSI2MSHProgressDialog progressDlg;

					progressDlg.Create(IDD_DIALOG2);
					progressDlg.ShowWindow(SW_SHOW);

					sprintf(msg,"Exporting %s ...",(LPCTSTR)dlg.mPathName);
					progressDlg.SetProgress(msg,0);

					converter.CreateZeroStructure(&mshData,pRootProject,pRootScene,NULL,&progressDlg);

					gZeroData = &mshData;

					#ifdef _DEBUG_LOG
					progressDlg.SetProgress("Debug output",0);
					mshData.DebugOutput((LPCTSTR)dlg.mPathName);
					progressDlg.SetProgress("Debug output",100);
					#endif


					sprintf(msg,"Writing to file %s ...",(LPCTSTR)dlg.mPathName);
					progressDlg.SetProgress(msg,50);
					FILE *f;
					f=fopen((LPCTSTR)dlg.mPathName, "wb");
					if(f)
					{
						CZeroExporter exporter;
						exporter.ExportFile(f,&mshData);
						fclose(f);
					}
					progressDlg.SetProgress(msg,100);
					progressDlg.DestroyWindow();
				}
				pRootProject ->Release();
			}
			pRootScene ->Release();
		}
	}

	return S_OK;
}


//
// ExportSceneA -- same as ExportScene, but takes a destination filename and
//                 doesn't open the dialog.  Provides a way to use the exporter in "batch" mode
//
STDMETHODIMP CXSISceneExporter::ExportSceneA(IDispatch *pProject, IDispatch *pScene, BSTR in_bstrDestFilename)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;

	CHAR * l_szTemp = OLE2A( in_bstrDestFilename );

	CXSI2MSHDialog dlg;

	// trickery, we have an instance of the CDialog, but we never call DoModal --
	// it will in turn read in all the defaults from the registry in OnInit()

	Scene *pRootScene;
	Project *pRootProject;
	if(pScene->QueryInterface(IID_Scene,(void **)&pRootScene)==S_OK)
	{
		if(pProject->QueryInterface(IID_Project,(void **)&pRootProject)==S_OK)
		{
			bool bDoIt=true;
			char msg[1024];
			if(_access((LPCTSTR)l_szTemp,0)==0)	//	if file exists
			{
				if(_access((LPCTSTR)l_szTemp,2)==-1)	//	if write-protected
				{
					MessageBox(0,"The output file is Write-Protected.","Error",MB_OK);
					bDoIt=false;
				}
			}

			if(bDoIt)
			{
				CScene2MSH converter;
				CZeroData mshData;

				converter.SetFilters(dlg.mExportSelectedOnly,dlg.mExportTextures,dlg.mExportAnimations,dlg.mExportLights,false,dlg.m_ClipLoopedAnims,dlg.m_BaseposeFromCurrentFrame,dlg.m_FlattenGeometry,dlg.m_CameraAnims,dlg.m_ExportFKAnimation,dlg.m_SkipBasepose,dlg.m_ExportLeftHanded);
				converter.SetDestinationPath((LPCTSTR)l_szTemp);
				converter.SetTextureFormat(dlg.m_TxtFormat);
				converter.SetWarningsOn(!dlg.m_IgnoreWarnings);

				CXSI2MSHProgressDialog progressDlg;

				progressDlg.Create(IDD_DIALOG2);
				progressDlg.ShowWindow(SW_SHOW);

				sprintf(msg,"Exporting %s ...",(LPCTSTR)l_szTemp);
				progressDlg.SetProgress(msg,0);

				converter.CreateZeroStructure(&mshData,pRootProject,pRootScene,NULL,&progressDlg);


				#ifdef _DEBUG_LOG
				progressDlg.SetProgress("Debug output",0);
				mshData.DebugOutput((LPCTSTR)l_szTemp);
				progressDlg.SetProgress("Debug output",100);
				#endif


				sprintf(msg,"Writing to file %s ...",(LPCTSTR)l_szTemp);
				progressDlg.SetProgress(msg,50);
				FILE *f;
				f=fopen((LPCTSTR)l_szTemp, "wb");
				if(f)
				{
					CZeroExporter exporter;
					exporter.ExportFile(f,&mshData);
					fclose(f);
				}
				progressDlg.SetProgress(msg,100);
				progressDlg.DestroyWindow();
			}
			pRootProject ->Release();
		}
		pRootScene ->Release();
	}

	return S_OK;
}
