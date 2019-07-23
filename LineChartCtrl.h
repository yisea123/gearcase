#pragma once

#include <vector>

#define MAX_POINTS 2048 //1 pixel per point, 2048 is the ordinary screen pixels count

using namespace std;

class CAxis
{
public:
	BOOL        m_bDraw;     //plot or not
	BOOL        m_bGrid;     //Draw grid line or not
	double      m_dMinValue;
	double      m_dMaxValue;
	double	    m_dValuePerPixel; //how much a pixel represented

	COLORREF	m_color;     //
	int         m_iStyle;     //
	int         m_iThick;     //
	CAxis()
	{
		m_bDraw = TRUE;
		m_bGrid = TRUE;
		m_dMinValue = 0;
		m_dMaxValue = 10000;
		m_color = RGB(0,0,0);
		m_iStyle = PS_SOLID;
		m_iThick = 1;
	}
};
// CLineChartCtrl

#define MAX_POINT_SIZE 5000
#define TITLE_SIZE	64

class CLineEx
{
public:
	CLineEx();
	virtual ~CLineEx();

public:
	BOOL        m_bDraw;
	COLORREF	m_clrColor;
	int         m_iStyle;
	int         m_iThick;
	float		m_fMinValue;
	float		m_fMaxValue;
	float	    m_fValuePerPixel;
	//long		m_lXValue[MAX_POINT_SIZE];
	//float		m_fYValue[MAX_POINT_SIZE];
	long		*m_lXValue;
	float		*m_fYValue;
	char		m_szTitle[TITLE_SIZE];
	void		AddPoint(long x, float y);
	void		DeleteAllPoint();
	int			GetCount();
	void		SetCount(int iCount);
private:
	int			m_iCount;
	CRITICAL_SECTION m_csData;
};

class CLineChartCtrl : public CStatic
{
	DECLARE_DYNAMIC(CLineChartCtrl)

public:
	CLineChartCtrl();
	virtual ~CLineChartCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	void Start();
	void Stop();

	void CalcLayout();

	void SetRate(int nRate);
	void SetBkColor(COLORREF clrBkColor);
	CAxis& GetAxisY();
	CAxis& GetAxisX();

private:
	void DrawAll(CDC* pDC);
	void DrawBasic(CDC *pDC);
	void DrawAxises(CDC* pDC);
	void DrawLines(CDC* pDC);
	void DrawLegends(CDC* pDC);

	void DrawLine(CDC* pDC, CLineEx *line);
	void DrawRev(CDC* pDC);
	void DrawVibration(CDC* pDC);
	void DrawTemperature(CDC* pDC);
	void DrawToraue(CDC* pDC);

	void DrawLegend(CDC* pDC, CLineEx *line, CString name, CRect rect);

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

	CAxis m_axisY;       //Y axis
	CAxis m_axisX;     //X axis

	COLORREF m_clrBkColor; //plot control background color
	int      m_nRate;      //update interval

public:
	CLineEx m_lineRev; //转速
	CLineEx m_lineVibration[3]; //振动
	CLineEx m_lineTemperature[8]; //温度
	CLineEx m_lineTorque; //转矩
};


