// XSI2MSHProgressDialog.cpp : implementation file
//

#include "stdafx.h"
#include "xsi2mshexporter.h"
#include "XSI2MSHProgressDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXSI2MSHProgressDialog dialog


CXSI2MSHProgressDialog::CXSI2MSHProgressDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CXSI2MSHProgressDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXSI2MSHProgressDialog)
	//}}AFX_DATA_INIT
}


void CXSI2MSHProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXSI2MSHProgressDialog)
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressBar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CXSI2MSHProgressDialog, CDialog)
	//{{AFX_MSG_MAP(CXSI2MSHProgressDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXSI2MSHProgressDialog message handlers

BOOL CXSI2MSHProgressDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// TODO: Add your specialized creation code here
	m_ProgressBar.SetRange(0,100);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CXSI2MSHProgressDialog::SetProgress(char *Message,int pos)
{
	if(Message)
		m_ProgressBar.SetWindowText(Message);
	if(pos>-1)
		m_ProgressBar.SetPos(pos);
	RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_NOERASE);
	m_ProgressBar.RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_NOERASE);
}

