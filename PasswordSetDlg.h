#if !defined(AFX_PASSWORDSETDLG_H__10FFF064_FB87_4614_8C61_ECE7B2199DE7__INCLUDED_)
#define AFX_PASSWORDSETDLG_H__10FFF064_FB87_4614_8C61_ECE7B2199DE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PasswordSetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPasswordSetDlg dialog

class CPasswordSetDlg : public CDialog
{
// Construction
public:
	CPasswordSetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPasswordSetDlg)
	enum { IDD = IDD_PASSWORDSET_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPasswordSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPasswordSetDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSWORDSETDLG_H__10FFF064_FB87_4614_8C61_ECE7B2199DE7__INCLUDED_)
