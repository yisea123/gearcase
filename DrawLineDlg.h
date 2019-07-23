#if !defined(AFX_DRAWLINEDLG_H__925624B0_C247_43EA_8F6E_8BB80A732671__INCLUDED_)
#define AFX_DRAWLINEDLG_H__925624B0_C247_43EA_8F6E_8BB80A732671__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DrawLineDlg.h : header file
//
#include "resource.h"
#include "LineChartCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CDrawLineDlg dialog

class CDrawLineDlg : public CDialog
{
// Construction
public:
	CDrawLineDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDrawLineDlg)
	enum { IDD = IDD_DRAWLINE_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CRect m_rectCtrl;                       // the static rect of chart control
	CRect m_rectAxisY;		                // Y axis rect
	CRect m_rectAxisX;		                // X axis rect
	CRect m_rectPlot;                       // m_rectCtrl - margin - axisRect
	CRect m_rectLegend;
	
	int			m_iMarginLeft;			   // left margin in pixels
	int			m_iMarginRight;			   // right margin in pixels
	int			m_iMarginTop;			   // top margin in pixels
	int			m_iMarginBottom;		   // bottom margin in pixels
	int			m_iLegendHeight;		   // height legend in pixels
	int			m_iAxisYWidth;			   // width axisY in pixels
	int			m_iAxisXHeight;			   // height axisX in pixels
	float		m_fXValuePerPixel;
	float		m_fYValuePerPixel;

	COLORREF m_clrBkColor; //plot control background color
	int      m_nRate;      //update interval

	CLineEx m_line;

	void SetLine(CLineEx *line);
	void CalcLayout();
	void DrawBasic(CDC *pDC);
	void DrawAxises(CDC* pDC);
	void OnDraw(CDC *pDC);
	void DrawLine(CDC* pDC, CLineEx *line);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawLineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDrawLineDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAWLINEDLG_H__925624B0_C247_43EA_8F6E_8BB80A732671__INCLUDED_)
