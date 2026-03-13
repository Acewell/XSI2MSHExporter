#if !defined(AFX_XSI2MSHDIALOG_H__C05CAC5B_36FA_41D3_912F_4FCFA0D9E122__INCLUDED_)
#define AFX_XSI2MSHDIALOG_H__C05CAC5B_36FA_41D3_912F_4FCFA0D9E122__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XSI2MSHDialog.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CXSI2MSHDialog dialog

class CXSI2MSHDialog : public CDialog
{
// Construction
public:
	CXSI2MSHDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CXSI2MSHDialog)
	enum { IDD = IDD_DIALOG1 };
	CButton	m_BtnExportLeftHanded;
	CButton	m_BtnSkipBasepose;
	CButton	m_BtnExportFKAnimation;
	CButton	m_BtnCameraAnims;
	CButton	m_BtnFlattenGeometry;
	CButton	m_BtnBaseposeFromCurrentFrame;
	CButton	m_BtnIgnoreWarnings;
	CButton	m_BtnLights;
	CButton	m_BtnSelected;
	CButton	m_BtnClipLoopedAnims;
	CButton	m_BtnAnimations;
	CButton	m_BtnTextures;
	CStatic	m_TxtStatic;
	CComboBox	m_CBoxTxtFormat;
	CEdit	mPathEdit;
	CString	mPathName;
	BOOL	mExportLights;
	BOOL	mExportAnimations;
	BOOL	mExportTextures;
	BOOL	mExportSelectedOnly;
	int		m_TxtFormat;
	BOOL	m_IgnoreWarnings;
	BOOL	m_ClipLoopedAnims;
	BOOL	m_BaseposeFromCurrentFrame;
	BOOL	m_FlattenGeometry;
	BOOL	m_CameraAnims;
	BOOL	m_ExportFKAnimation;
	BOOL	m_SkipBasepose;
	BOOL	m_ExportLeftHanded;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXSI2MSHDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CXSI2MSHDialog)
	afx_msg void OnBrowseButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnExportTextures();
	afx_msg void OnAnimations();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XSI2MSHDIALOG_H__C05CAC5B_36FA_41D3_912F_4FCFA0D9E122__INCLUDED_)
