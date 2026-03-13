#if !defined(AFX_XSI2MSHPROGRESSDIALOG_H__4658FD9A_5264_4549_860B_9F7105EA47F0__INCLUDED_)
#define AFX_XSI2MSHPROGRESSDIALOG_H__4658FD9A_5264_4549_860B_9F7105EA47F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XSI2MSHProgressDialog.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CXSI2MSHProgressDialog dialog

class CXSI2MSHProgressDialog : public CDialog
{
// Construction
public:
	CXSI2MSHProgressDialog(CWnd* pParent = NULL);   // standard constructor

	void SetProgress(char *Message,int pos);
// Dialog Data
	//{{AFX_DATA(CXSI2MSHProgressDialog)
	enum { IDD = IDD_DIALOG2 };
	CProgressCtrl	m_ProgressBar;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXSI2MSHProgressDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CXSI2MSHProgressDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XSI2MSHPROGRESSDIALOG_H__4658FD9A_5264_4549_860B_9F7105EA47F0__INCLUDED_)
