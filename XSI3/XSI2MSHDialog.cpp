// XSI2MSHDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "XSI2MSHDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXSI2MSHDialog dialog


CXSI2MSHDialog::CXSI2MSHDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CXSI2MSHDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXSI2MSHDialog)
	mPathName = _T("");
	mExportLights = FALSE;
	mExportAnimations = FALSE;
	mExportTextures = FALSE;
	mExportSelectedOnly = FALSE;
	m_TxtFormat = -1;
	m_IgnoreWarnings = FALSE;
	m_ClipLoopedAnims = FALSE;
	m_BaseposeFromCurrentFrame = FALSE;
	m_FlattenGeometry = FALSE;
	m_CameraAnims = FALSE;
	m_ExportFKAnimation = FALSE;
	m_SkipBasepose = FALSE;
	m_ExportLeftHanded = FALSE;
	//}}AFX_DATA_INIT
	HKEY hKey;
	if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\Pandemic Studios\\XSI2MSHExporter",0,KEY_ALL_ACCESS,&hKey))
	{
		DWORD dwBool;
		DWORD datasize;
		DWORD dwType;
		char pathname[2048];

		RegQueryValueEx(hKey,"Path",0,&dwType,(unsigned char *)pathname,&datasize);
		mPathName=pathname;
		RegQueryValueEx(hKey,"Lights",0,&dwType,(unsigned char *)&dwBool,&datasize);
		mExportLights=(dwBool==1);
		RegQueryValueEx(hKey,"Animations",0,&dwType,(unsigned char *)&dwBool,&datasize);
		mExportAnimations=(dwBool==1);
		RegQueryValueEx(hKey,"Textures",0,&dwType,(unsigned char *)&dwBool,&datasize);
		mExportTextures=(dwBool==1);
		RegQueryValueEx(hKey,"Selected",0,&dwType,(unsigned char *)&dwBool,&datasize);
		mExportSelectedOnly=(dwBool==1);
		RegQueryValueEx(hKey,"TxtFormat",0,&dwType,(unsigned char *)&dwBool,&datasize);
		m_TxtFormat=dwBool;
		RegQueryValueEx(hKey,"IgnoreWarnings",0,&dwType,(unsigned char *)&dwBool,&datasize);
		m_IgnoreWarnings=(dwBool==1);
		RegQueryValueEx(hKey,"ClipLoopedAnims",0,&dwType,(unsigned char *)&dwBool,&datasize);
		m_ClipLoopedAnims=(dwBool==1);
		RegQueryValueEx(hKey,"BaseposeFromCurrentFrame",0,&dwType,(unsigned char *)&dwBool,&datasize);
		m_BaseposeFromCurrentFrame=(dwBool==1);
		RegQueryValueEx(hKey,"CameraAnims",0,&dwType,(unsigned char *)&dwBool,&datasize);
		m_CameraAnims=(dwBool==1);
		RegQueryValueEx(hKey,"FlattenGeometry",0,&dwType,(unsigned char *)&dwBool,&datasize);
		m_FlattenGeometry=(dwBool==1);
		RegQueryValueEx(hKey,"ExportFKAnimation",0,&dwType,(unsigned char *)&dwBool,&datasize);
		m_ExportFKAnimation=(dwBool==1);
		RegQueryValueEx(hKey,"SkipBasepose",0,&dwType,(unsigned char *)&dwBool,&datasize);
		m_SkipBasepose=(dwBool==1);
		RegQueryValueEx(hKey,"ExportLeftHanded",0,&dwType,(unsigned char *)&dwBool,&datasize);
		m_ExportLeftHanded=(dwBool==1);
		RegCloseKey(hKey);
	}
	else
	{
		DWORD disposition;
		if(ERROR_SUCCESS==RegCreateKeyEx(HKEY_CURRENT_USER,"Software\\Pandemic Studios\\XSI2MSHExporter",0,"",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&disposition))
		{
			DWORD dwFalse=DWORD(0);
			DWORD dwNegOne= DWORD(-1);
			
			RegSetValueEx(hKey,"Path",0,REG_SZ,(unsigned char *)"",1);
			RegSetValueEx(hKey,"Lights",0,REG_DWORD,(unsigned char *)&dwFalse,sizeof(DWORD));
			RegSetValueEx(hKey,"Animations",0,REG_DWORD,(unsigned char *)&dwFalse,sizeof(DWORD));
			RegSetValueEx(hKey,"Textures",0,REG_DWORD,(unsigned char *)&dwFalse,sizeof(DWORD));
			RegSetValueEx(hKey,"Selected",0,REG_DWORD,(unsigned char *)&dwFalse,sizeof(DWORD));
			RegSetValueEx(hKey,"TxtFormat",0,REG_DWORD,(unsigned char *)&dwNegOne,sizeof(DWORD));
			RegSetValueEx(hKey,"IgnoreWarnings",0,REG_DWORD,(unsigned char *)&dwFalse,sizeof(DWORD));
			RegSetValueEx(hKey,"ClipLoopedAnims",0,REG_DWORD,(unsigned char *)&dwFalse,sizeof(DWORD));
			RegSetValueEx(hKey,"BaseposeFromCurrentFrame",0,REG_DWORD,(unsigned char *)&dwFalse,sizeof(DWORD));
			RegSetValueEx(hKey,"ExportFKAnimation",0,REG_DWORD,(unsigned char *)&dwFalse,sizeof(DWORD));
			RegSetValueEx(hKey,"SkipBasepose",0,REG_DWORD,(unsigned char *)&dwFalse,sizeof(DWORD));
			RegSetValueEx(hKey,"ExportLeftHanded",0,REG_DWORD,(unsigned char *)&dwFalse,sizeof(DWORD));

			RegCloseKey(hKey);
		}
	}
}


void CXSI2MSHDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXSI2MSHDialog)
	DDX_Control(pDX, IDC_CHECKLEFTHANDED, m_BtnExportLeftHanded);
	DDX_Control(pDX, IDC_CHECKSKIPBP, m_BtnSkipBasepose);
	DDX_Control(pDX, IDC_CHECKEXPFK, m_BtnExportFKAnimation);
	DDX_Control(pDX, IDC_CHECK11, m_BtnCameraAnims);
	DDX_Control(pDX, IDC_CHECK10, m_BtnFlattenGeometry);
	DDX_Control(pDX, IDC_CHECK8, m_BtnBaseposeFromCurrentFrame);
	DDX_Control(pDX, IDC_CHECK6, m_BtnIgnoreWarnings);
	DDX_Control(pDX, IDC_CHECK5, m_BtnLights);
	DDX_Control(pDX, IDC_CHECK3, m_BtnSelected);
	DDX_Control(pDX, IDC_CHECK7, m_BtnClipLoopedAnims);
	DDX_Control(pDX, IDC_CHECK2, m_BtnAnimations);
	DDX_Control(pDX, IDC_CHECK1, m_BtnTextures);
	DDX_Control(pDX, IDC_TXT_STATIC, m_TxtStatic);
	DDX_Control(pDX, IDC_COMBO1, m_CBoxTxtFormat);
	DDX_Control(pDX, IDC_EDIT1, mPathEdit);
	DDX_Text(pDX, IDC_EDIT1, mPathName);
	DDX_Check(pDX, IDC_CHECK5, mExportLights);
	DDX_Check(pDX, IDC_CHECK2, mExportAnimations);
	DDX_Check(pDX, IDC_CHECK1, mExportTextures);
	DDX_Check(pDX, IDC_CHECK3, mExportSelectedOnly);
	DDX_CBIndex(pDX, IDC_COMBO1, m_TxtFormat);
	DDX_Check(pDX, IDC_CHECK6, m_IgnoreWarnings);
	DDX_Check(pDX, IDC_CHECK7, m_ClipLoopedAnims);
	DDX_Check(pDX, IDC_CHECK8, m_BaseposeFromCurrentFrame);
	DDX_Check(pDX, IDC_CHECK10, m_FlattenGeometry);
	DDX_Check(pDX, IDC_CHECK11, m_CameraAnims);
	DDX_Check(pDX, IDC_CHECKEXPFK, m_ExportFKAnimation);
	DDX_Check(pDX, IDC_CHECKSKIPBP, m_SkipBasepose);
	DDX_Check(pDX, IDC_CHECKLEFTHANDED, m_ExportLeftHanded);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CXSI2MSHDialog, CDialog)
	//{{AFX_MSG_MAP(CXSI2MSHDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBrowseButton)
	ON_BN_CLICKED(IDC_CHECK1, OnExportTextures)
	ON_BN_CLICKED(IDC_CHECK2, OnAnimations)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXSI2MSHDialog message handlers

void CXSI2MSHDialog::OnBrowseButton() 
{
	// TODO: Add your control notification handler code here
	char szFilter[] = "MSH Files (*.msh)|*.msh|All Files (*.*)|*.*||";
	CFileDialog fileDlg(false,"msh", (LPCTSTR)mPathName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN, szFilter);
	if(fileDlg.DoModal()==IDOK)
	{
		CString sPath=fileDlg.GetPathName();
		mPathEdit.SetWindowText((LPCTSTR)sPath);
	}
}

BOOL CXSI2MSHDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	mPathEdit.SetWindowText((LPCTSTR)mPathName);
	switch(m_TxtFormat)
	{
	case -1:
	case 0:
		m_CBoxTxtFormat.SelectString(-1,"Don't Convert");
		break;
	case 1:
		m_CBoxTxtFormat.SelectString(-1,"TARGA");
		break;
	case 2:
		m_CBoxTxtFormat.SelectString(-1,"PIC");
		break;
	}

	if(mExportLights)
		m_BtnLights.SetCheck(1);

	if(m_FlattenGeometry)
		m_BtnFlattenGeometry.SetCheck(1);

	if(mExportAnimations)
	{
		m_BtnAnimations.SetCheck(1);
		OnAnimations();
	}
	if(mExportTextures)
	{
		m_BtnTextures.SetCheck(1);
		OnExportTextures();
	}

	if(mExportSelectedOnly)
		m_BtnSelected.SetCheck(1);
	
	if(m_IgnoreWarnings)
		m_BtnIgnoreWarnings.SetCheck(1);

	if(m_ClipLoopedAnims)
		m_BtnClipLoopedAnims.SetCheck(1);

	if(m_CameraAnims)
		m_BtnCameraAnims.SetCheck(1);

	if(m_BaseposeFromCurrentFrame)
		m_BtnBaseposeFromCurrentFrame.SetCheck(1);

	if (m_ExportFKAnimation)
		m_BtnExportFKAnimation.SetCheck(1);

	if (m_SkipBasepose)
		m_BtnSkipBasepose.SetCheck(1);

	if (m_ExportLeftHanded)
		m_BtnExportLeftHanded.SetCheck(1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CXSI2MSHDialog::OnExportTextures() 
{
	if(m_BtnTextures.GetCheck())
	{
		m_CBoxTxtFormat.ShowWindow(SW_SHOW);
		m_TxtStatic.ShowWindow(SW_SHOW);
	}
	else
	{
		m_CBoxTxtFormat.ShowWindow(SW_HIDE);
		m_TxtStatic.ShowWindow(SW_HIDE);
	}
}

void CXSI2MSHDialog::OnAnimations() 
{
	if(m_BtnAnimations.GetCheck())
	{
		m_BtnCameraAnims.EnableWindow();
		m_BtnClipLoopedAnims.EnableWindow();
		m_BtnExportFKAnimation.EnableWindow();
		m_BtnSkipBasepose.EnableWindow();
	}
	else
	{
		m_BtnCameraAnims.SetCheck(0);
		m_BtnCameraAnims.EnableWindow(FALSE);
		m_BtnClipLoopedAnims.SetCheck(0);
		m_BtnClipLoopedAnims.EnableWindow(FALSE);
		m_BtnExportFKAnimation.SetCheck(0);
		m_BtnExportFKAnimation.EnableWindow(FALSE);
		m_BtnSkipBasepose.SetCheck(0);
		m_BtnSkipBasepose.EnableWindow(FALSE);
	}
}

void CXSI2MSHDialog::OnOK() 
{
	CDialog::OnOK();
	// TODO: Add extra validation here
	HKEY hKey;
	if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\Pandemic Studios\\XSI2MSHExporter",0,KEY_ALL_ACCESS,&hKey))
	{
		DWORD dwVal=0;
		
		RegSetValueEx(hKey,"Path",0,REG_SZ,(unsigned char *)(LPCTSTR)mPathName,strlen((LPCTSTR)mPathName)+1);
		if(mExportLights)
			dwVal=1;
		else
			dwVal=0;
		RegSetValueEx(hKey,"Lights",0,REG_DWORD,(unsigned char *)&dwVal,sizeof(DWORD));

		if(mExportAnimations)
			dwVal=1;
		else
			dwVal=0;
		RegSetValueEx(hKey,"Animations",0,REG_DWORD,(unsigned char *)&dwVal,sizeof(DWORD));

		if(mExportTextures)
			dwVal=1;
		else
			dwVal=0;
		RegSetValueEx(hKey,"Textures",0,REG_DWORD,(unsigned char *)&dwVal,sizeof(DWORD));

		if(mExportSelectedOnly)
			dwVal=1;
		else
			dwVal=0;
		RegSetValueEx(hKey,"Selected",0,REG_DWORD,(unsigned char *)&dwVal,sizeof(DWORD));

		dwVal=m_TxtFormat;
		RegSetValueEx(hKey,"TxtFormat",0,REG_DWORD,(unsigned char *)&dwVal,sizeof(DWORD));
	
		if(m_IgnoreWarnings)
			dwVal=1;
		else
			dwVal=0;
		RegSetValueEx(hKey,"IgnoreWarnings",0,REG_DWORD,(unsigned char *)&dwVal,sizeof(DWORD));

		if(m_ClipLoopedAnims)
			dwVal=1;
		else
			dwVal=0;
		RegSetValueEx(hKey,"ClipLoopedAnims",0,REG_DWORD,(unsigned char *)&dwVal,sizeof(DWORD));

		if(m_CameraAnims)
			dwVal=1;
		else
			dwVal=0;
		RegSetValueEx(hKey,"CameraAnims",0,REG_DWORD,(unsigned char *)&dwVal,sizeof(DWORD));

		if(m_BaseposeFromCurrentFrame)
			dwVal=1;
		else
			dwVal=0;
		RegSetValueEx(hKey,"BaseposeFromCurrentFrame",0,REG_DWORD,(unsigned char *)&dwVal,sizeof(DWORD));

		if(m_FlattenGeometry)
			dwVal=1;
		else
			dwVal=0;
		RegSetValueEx(hKey,"FlattenGeometry",0,REG_DWORD,(unsigned char *)&dwVal,sizeof(DWORD));

		if(m_ExportFKAnimation)
			dwVal=1;
		else
			dwVal=0;
		RegSetValueEx(hKey,"ExportFKAnimation",0,REG_DWORD,(unsigned char *)&dwVal,sizeof(DWORD));

		if(m_SkipBasepose)
			dwVal=1;
		else
			dwVal=0;
		RegSetValueEx(hKey,"SkipBasepose",0,REG_DWORD,(unsigned char *)&dwVal,sizeof(DWORD));

		if(m_ExportLeftHanded)
			dwVal=1;
		else
			dwVal=0;
		RegSetValueEx(hKey,"ExportLeftHanded",0,REG_DWORD,(unsigned char *)&dwVal,sizeof(DWORD));

		RegCloseKey(hKey);
	}
}
