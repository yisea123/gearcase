#if !defined(AFX_INPUTPASSWDDLG_H__6BB395C3_05E4_4A5F_9519_E84B1E6C5375__INCLUDED_)
#define AFX_INPUTPASSWDDLG_H__6BB395C3_05E4_4A5F_9519_E84B1E6C5375__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputPasswdDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputPasswdDlg dialog

class CInputPasswdDlg : public CDialog
{
// Construction
public:
	CInputPasswdDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputPasswdDlg)
	enum { IDD = IDD_INPUTPASSWD_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputPasswdDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputPasswdDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTPASSWDDLG_H__6BB395C3_05E4_4A5F_9519_E84B1E6C5375__INCLUDED_)
