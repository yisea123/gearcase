#if !defined(AFX_CURVECOLORDLG_H__34CCC3DD_8258_4E6D_87BA_35DF7CA0EC9F__INCLUDED_)
#define AFX_CURVECOLORDLG_H__34CCC3DD_8258_4E6D_87BA_35DF7CA0EC9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CurveColorDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCurveColorDlg dialog

class CCurveColorDlg : public CDialog
{
// Construction
public:
	CCurveColorDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCurveColorDlg)
	enum { IDD = IDD_CURVECOLOR_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	BOOL m_bRev;
	BOOL m_bVibration[3];
	BOOL m_bTemperature[8];
	BOOL m_bTorque;

	COLORREF m_clrRev;
	COLORREF m_clrVibration[3];
	COLORREF m_clrTemperature[8];
	COLORREF m_clrTorque;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCurveColorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCurveColorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual void OnOK();
	//}}AFX_MSG
	afx_msg void OnSelectColor(UINT nID);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CURVECOLORDLG_H__34CCC3DD_8258_4E6D_87BA_35DF7CA0EC9F__INCLUDED_)
