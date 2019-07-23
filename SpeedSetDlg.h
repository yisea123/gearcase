#if !defined(AFX_SPEEDSETDLG_H__08D630CB_BAE5_4933_936A_C1FB9DB46A9E__INCLUDED_)
#define AFX_SPEEDSETDLG_H__08D630CB_BAE5_4933_936A_C1FB9DB46A9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpeedSetDlg.h : header file
//

#include "BtnST.h"

const int DEF_SPEED_RPM = 1850;
const int MAX_SPEED_RPM = 5000;
const int MIN_SPEED_RPM = 0;

const int DEF_INTERVAL = 100;
const int MAX_INTERVAL = 5000;
const int MIN_INTERVAL = 0;

/////////////////////////////////////////////////////////////////////////////
// CSpeedSetDlg dialog

class CSpeedSetDlg : public CDialog
{
// Construction
public:
	CSpeedSetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSpeedSetDlg)
	enum { IDD = IDD_SPEEDSET_DLG };
	//}}AFX_DATA
	WORD m_wSpeed[7];
	int m_nInterval[7];
	WORD m_wRunSpeed;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpeedSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpeedSetDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPEEDSETDLG_H__08D630CB_BAE5_4933_936A_C1FB9DB46A9E__INCLUDED_)
