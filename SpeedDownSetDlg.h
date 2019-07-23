#if !defined(AFX_SPEEDDOWNSETDLG_H__5ACB86D5_D5F6_4466_ACA4_93937F48398A__INCLUDED_)
#define AFX_SPEEDDOWNSETDLG_H__5ACB86D5_D5F6_4466_ACA4_93937F48398A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpeedDownSetDlg.h : header file
//

const float DEFAULT_SPEEDDWONRATE = 5.0000;
const float MIN_SPEEDDWONRATE = 1.0000;
const float MAX_SPEEDDWONRATE = 10.0000;

/////////////////////////////////////////////////////////////////////////////
// CSpeedDownSetDlg dialog

class CSpeedDownSetDlg : public CDialog
{
// Construction
public:
	CSpeedDownSetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSpeedDownSetDlg)
	enum { IDD = IDD_SPEEDDOWNSET_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	float m_fSpeedDownRate;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpeedDownSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpeedDownSetDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPEEDDOWNSETDLG_H__5ACB86D5_D5F6_4466_ACA4_93937F48398A__INCLUDED_)
