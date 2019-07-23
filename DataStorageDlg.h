#if !defined(AFX_DATASTORAGEDLG_H__01890584_A732_4C93_B786_36A8AFF67FF5__INCLUDED_)
#define AFX_DATASTORAGEDLG_H__01890584_A732_4C93_B786_36A8AFF67FF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataStorageDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDataStorageDlg dialog

class CDataStorageDlg : public CDialog
{
// Construction
public:
	CDataStorageDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDataStorageDlg)
	enum { IDD = IDD_DATASTORAGE_DLG };
		// NOTE: the ClassWizard will add data members here
	CString m_sPath;
	//}}AFX_DATA
	static CString s_strDefPath;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataStorageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDataStorageDlg)
	afx_msg void OnBnSelect();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATASTORAGEDLG_H__01890584_A732_4C93_B786_36A8AFF67FF5__INCLUDED_)
