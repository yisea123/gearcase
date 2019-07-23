#if !defined(AFX_DATAQUERYDLG_H__46D8E9DE_C0D8_49A1_B188_8C7F828E88D5__INCLUDED_)
#define AFX_DATAQUERYDLG_H__46D8E9DE_C0D8_49A1_B188_8C7F828E88D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataQueryDlg.h : header file
//
#include "LineChartCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CDataQueryDlg dialog

class CDataQueryDlg : public CDialog
{
// Construction
public:
	CDataQueryDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDataQueryDlg();

// Dialog Data
	//{{AFX_DATA(CDataQueryDlg)
	enum { IDD = IDD_DATAQUERY_DLG };

	CString m_sPath;
	CRect m_rectTitle;
	CRect m_rectDisplay;
	CRect m_rectOpinion;
	CRect m_rectComment;

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

	float		m_fRevValuePerPixel;
	float		m_fVibrationValuePerPixel;
	float		m_fTemperatureValuePerPixel;
	float		m_fTorqueValuePerPixel;
	float		m_fSpeedDownRate;

	CString m_sUserName;
	CString m_sUserNum;
	CString m_sProductType;
	CString m_sProductNum;
	CString m_sProducer;
	CString m_sTestName;
	CString m_sTestTime;

	CString m_sTitle;
	CString m_sOpinion;

	//}}AFX_DATA
	static CString s_strDefPath;
	COLORREF	m_clrRev;
	COLORREF	m_clrVibration[3];
	COLORREF	m_clrTemperature[8];
	COLORREF	m_clrTorque;
	
	CLineEx m_lineRev;
	CLineEx m_lineVibration[3];
	CLineEx m_lineTemperature[8];
	CLineEx m_lineTorque;

	void InitDisplay();
	void DisplayCurve();
	//void LoadData(CString strName, CLineEx *ln);
	void LoadUserFromFile();
	BOOL FolderExist(CString strPath);
	BOOL FileExist(CString strPath);

	void CalcLayout();
	void DrawBasic(CDC *pDC);
	void DrawAxises(CDC* pDC);
	void DrawLegends(CDC* pDC);
	void DrawLegend(CDC* pDC, COLORREF clr, CString name, CRect rect);
	void ReadLineFromFile(CString strName, CLineEx *ln);
	void ReadLines();
	void DrawLine(CDC* pDC, CLineEx *line);
	void DrawRev(CDC* pDC);
	void DrawVibration(CDC* pDC);
	void DrawTemperature(CDC* pDC);
	void DrawToraue(CDC* pDC);
	void DrawLines(CDC *pDC);
	void OnDraw(CDC *pDC);
	//void OnBeginPrinting(CDC* dc, Info);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataQueryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDataQueryDlg)
	afx_msg void OnBnSelect();
	afx_msg void OnBnPrint();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATAQUERYDLG_H__46D8E9DE_C0D8_49A1_B188_8C7F828E88D5__INCLUDED_)
