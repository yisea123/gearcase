#if !defined(AFX_COMSETDLG_H__AE61C4CB_F0A4_404C_BA33_CF151F5013EB__INCLUDED_)
#define AFX_COMSETDLG_H__AE61C4CB_F0A4_404C_BA33_CF151F5013EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComSetDlg.h : header file
//

#include "BtnST.h"

#define DEFAULT_TIMEOUT 3
#define MIN_TIMEOUT 1
#define MAX_TIMEOUT 60
#define SERIALPORT_STARTBITS 1
/////////////////////////////////////////////////////////////////////////////
// CComSetDlg dialog

class CComSetDlg : public CDialog
{
// Construction
public:
	CComSetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CComSetDlg)
	enum { IDD = IDD_COMSET_DLG };
	CButtonST	m_ctrlBtnST_search;
	//}}AFX_DATA
	int m_nSerialPortNum; //0,1,2,3 = 1, 2, 3, 4
	int m_nBaudRate; //0,1 = 9600, 19200
	int m_nStartBits; //0,1 = 1, 2
	int m_nDataBits; //0,1,2,3,4 = 4, 5, 6, 7, 8
	int m_nVerifyBits; //0,1,2,3,4 = NONE, ODD, EVEN, MARK, SPACE
	int m_nStopBits; //0,1,2 = 1, 1.5, 2
	BOOL m_bAutoRelation;
	int m_nTimeOut;

	void SetCtrlValue();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CComSetDlg)
	afx_msg void OnBnSearchPort();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMSETDLG_H__AE61C4CB_F0A4_404C_BA33_CF151F5013EB__INCLUDED_)
