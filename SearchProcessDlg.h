#if !defined(AFX_SEARCHPROCESSDLG_H__83DEE65C_72C6_4886_9E43_7F76990E1038__INCLUDED_)
#define AFX_SEARCHPROCESSDLG_H__83DEE65C_72C6_4886_9E43_7F76990E1038__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SearchProcessDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSearchProcessDlg dialog
#define	CM_PROCESS_SET_POS	(WM_USER	+ 1000)//ctrl message
class CSearchProcessDlg : public CDialog
{
// Construction
public:
	CSearchProcessDlg(CWnd* pParent = NULL);   // standard constructor
	CEvent	*m_pStopThreadEvent;
// Dialog Data
	//{{AFX_DATA(CSearchProcessDlg)
	enum { IDD = IDD_DIALOG_SERCH };
	CProgressCtrl	m_process;
	CAnimateCtrl	m_searchAnimate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchProcessDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	afx_msg	void OnProcessSetPos(WPARAM wParam,LPARAM lParam);
	// Generated message map functions
	//{{AFX_MSG(CSearchProcessDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHPROCESSDLG_H__83DEE65C_72C6_4886_9E43_7F76990E1038__INCLUDED_)
