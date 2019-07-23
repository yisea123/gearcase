#if !defined(AFX_USERSETDLG_H__F2EBF784_4A3E_4C33_A424_7B32787F2946__INCLUDED_)
#define AFX_USERSETDLG_H__F2EBF784_4A3E_4C33_A424_7B32787F2946__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UserSetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUserSetDlg dialog

class CUserSetDlg : public CDialog
{
// Construction
public:
	CUserSetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUserSetDlg)
	enum { IDD = IDD_USERSET_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	CString m_sUserName;
	CString m_sUserNum;
	CString m_sProductType;
	CString m_sProductNum;
	CString m_sProducer;
	CString m_sTestName;
	CString m_sTestTime;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUserSetDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERSETDLG_H__F2EBF784_4A3E_4C33_A424_7B32787F2946__INCLUDED_)
