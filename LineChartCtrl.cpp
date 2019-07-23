// LineChartCtrl.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "LineChartCtrl.h"
#include "MemDC.h"
#include "DrawLineDlg.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// CLineEx
CLineEx::CLineEx()
{
	m_bDraw = TRUE;
	m_iStyle = PS_SOLID;
	m_clrColor = RGB(0x00, 0x00, 0x00);
	m_iThick = 1;
	m_iCount = 0;
	m_fMinValue = 0.00;
	m_fMaxValue = 1.00;
	m_fValuePerPixel = 1.00;
	m_lXValue = new long[MAX_POINT_SIZE];
	m_fYValue = new float[MAX_POINT_SIZE];
	memset(m_lXValue, 0, MAX_POINT_SIZE);
	memset(m_fYValue, 0, MAX_POINT_SIZE);
	memset(m_szTitle, 0, TITLE_SIZE);
	InitializeCriticalSection(&m_csData);
}

CLineEx::~CLineEx()
{
	if(m_lXValue) delete m_lXValue;
	if(m_fYValue) delete m_fYValue;
	DeleteCriticalSection(&m_csData);
}

void CLineEx::AddPoint(long x, float y)
{
	int i = 0;
	EnterCriticalSection(&m_csData);
	if(y>m_fMaxValue) y = m_fMaxValue;
	if(y<m_fMinValue) y = m_fMinValue;
	if(m_iCount < MAX_POINT_SIZE)
	{
		m_lXValue[m_iCount] = x;
		m_fYValue[m_iCount++] = y;
	}
	else
	{
		for(i=0; i<m_iCount-1; i++)
		{
			m_lXValue[i] = m_lXValue[i+1];
			m_fYValue[i] = m_fYValue[i+1];
		}
		m_lXValue[m_iCount-1] = x;
		m_fYValue[m_iCount-1] = y;
	}
	LeaveCriticalSection(&m_csData);
	//TRACE("m_iCount=%d x=%ld y=%.2f\n", m_iCount, x, y);
}

void CLineEx::DeleteAllPoint()
{
	EnterCriticalSection(&m_csData);
	m_iCount = 0;
	LeaveCriticalSection(&m_csData);
}

int CLineEx::GetCount()
{
	int iCount = 0;
	EnterCriticalSection(&m_csData);
	iCount = m_iCount;
	LeaveCriticalSection(&m_csData);
	return iCount;
}

void CLineEx::SetCount(int iCount)
{
	EnterCriticalSection(&m_csData);
	m_iCount = iCount;
	LeaveCriticalSection(&m_csData);
}
//////////////////////////////////////////////////////////////////////////////////////////////
// CLineChartCtrl

IMPLEMENT_DYNAMIC(CLineChartCtrl, CStatic)
CLineChartCtrl::CLineChartCtrl()
{
	m_clrBkColor = RGB(0,0,0);
	m_nRate = 1000;

	m_iMarginLeft = 5;
	m_iMarginRight = 15;
	m_iMarginTop = 10;
	m_iMarginBottom = 5;
	m_iAxisXHeight = 30;
	m_iLegendHeight = 20;
	m_iAxisYWidth = 130;
}

CLineChartCtrl::~CLineChartCtrl()
{
}

BEGIN_MESSAGE_MAP(CLineChartCtrl, CStatic)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CLineChartCtrl message handlers

void CLineChartCtrl::OnTimer(UINT nIDEvent)
{
	Invalidate(TRUE);
	CStatic::OnTimer(nIDEvent);
}

void CLineChartCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rectCtrl;
	GetClientRect (&rectCtrl);

	CMemDC memDC(&dc, &rectCtrl);
	DrawAll(&memDC);
}

BOOL CLineChartCtrl::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}
void CLineChartCtrl::DrawAll(CDC* pDC)
{
	DrawBasic(pDC);
	DrawAxises(pDC);
	DrawLegends(pDC);
	DrawLines(pDC);
}

void CLineChartCtrl::DrawBasic(CDC *pDC) //画整个图区的属性、边框和背景
{
	CRect rect(&m_rectCtrl);
	pDC->SetBkMode(TRANSPARENT); // 设置透明
	COLORREF clr = RGB(0x8F, 0xBC, 0x8F);
	pDC->FillSolidRect(&rect, clr);
}

void CLineChartCtrl::DrawAxises(CDC* pDC)
{
	int i = 0;
	int nInterval = 10;
	
	//Draw Y axis
	if(m_axisY.m_bDraw)
	{
		CPen pen(m_axisY.m_iStyle, m_axisY.m_iThick, RGB(192, 192, 192));
		CPen *oldPen = pDC->SelectObject(&pen);
		//Draw border of plot
		pDC->MoveTo(m_rectPlot.left,   m_rectPlot.top);  
		pDC->LineTo(m_rectPlot.right,  m_rectPlot.top);  
		pDC->LineTo(m_rectPlot.right,  m_rectPlot.bottom);  
		pDC->LineTo(m_rectPlot.left,   m_rectPlot.bottom);  
		pDC->LineTo(m_rectPlot.left,   m_rectPlot.top); 

		int iGridCount = 20;
		int iGrid =(int)floor((m_axisY.m_dMaxValue-m_axisY.m_dMinValue)/iGridCount);
		if(iGrid>0)
		{
			CString str;
			int textWidth = 0;
			int textHeight = 16;
			CSize sizeStr(0, 0);
			sizeStr = pDC->GetTextExtent("-5000");
			textHeight = sizeStr.cy;
			int y0=(int)m_axisY.m_dMinValue;
			for(i=0;i<iGridCount+1;i++)
			{
				//AxisY
				int y = (int)(m_rectPlot.bottom - (y0+i*iGrid-m_axisY.m_dMinValue)/m_axisY.m_dValuePerPixel);
				if(m_axisY.m_bGrid)
				{
					pDC->MoveTo(m_rectPlot.left,y);
					pDC->LineTo(m_rectPlot.right,y);
				}

				//Rev(-5000, 5000)
				CSize sizeRev(0, 0);
				sizeRev = pDC->GetTextExtent("-5000");
				textWidth = sizeRev.cx;
				textHeight = sizeRev.cy;
				int yRev0 = m_lineRev.m_fMinValue;
				float fRevValuePerPixel = m_lineRev.m_fValuePerPixel;
				float fRevRange = m_lineRev.m_fMaxValue - m_lineRev.m_fMinValue;
				int iRevGrid =(int)floor(fRevRange/iGridCount);
				CString strRev;
				int yRev = (int)(m_rectPlot.bottom - (yRev0+i*iRevGrid - m_lineRev.m_fMinValue)/fRevValuePerPixel);
				strRev.Format(_T("%d"), yRev0+i*iRevGrid);
				CRect rectRev(0, 0, 0, 0);
				rectRev.left = m_rectAxisY.right - textWidth;
				rectRev.right = m_rectAxisY.right;
				rectRev.top = yRev - textHeight/2;
				rectRev.bottom = yRev + textHeight/2;
				pDC->DrawText(strRev, rectRev, DT_CENTER|DT_VCENTER);
				CRect rtRev(rectRev.left, m_rectAxisY.bottom+8, rectRev.right-6, m_rectAxisX.bottom);
				pDC->DrawText(_T("转速"), rtRev, DT_CENTER|DT_VCENTER);

				//Vibration(0, 20)
				CSize sizeVibration(0, 0);
				sizeVibration = pDC->GetTextExtent("20");
				textWidth = sizeVibration.cx;
				textHeight = sizeVibration.cy;
				int yVibration0 = m_lineVibration[0].m_fMinValue;
				float fVibrationValuePerPixel = m_lineVibration[0].m_fValuePerPixel;
				float fVibrationRange = m_lineVibration[0].m_fMaxValue - m_lineVibration[0].m_fMinValue;
				int iVibrationGrid =(int)floor(fVibrationRange/iGridCount);
				CString strVibration;
				int yVibration = (int)(m_rectPlot.bottom - (i*iVibrationGrid)/fVibrationValuePerPixel);
				strVibration.Format(_T("%d"), yVibration0+i*iVibrationGrid);
				CRect rectVibration(0, 0, 0, 0);
				rectVibration.left = rectRev.left - nInterval - textWidth;
				rectVibration.right = rectRev.left - nInterval;
				rectVibration.top = yVibration - textHeight/2;
				rectVibration.bottom = yVibration + textHeight/2;
				pDC->DrawText(strVibration, rectVibration, DT_CENTER|DT_VCENTER);
				CRect rtVibration(rectVibration.left-6, m_rectAxisY.bottom+8, rectVibration.right+6, m_rectAxisX.bottom);
				pDC->DrawText(_T("振动"), rtVibration, DT_CENTER|DT_VCENTER);

				//Temperature(-10, 120)
				CSize sizeTemperature(0, 0);
				sizeTemperature = pDC->GetTextExtent("120");
				textWidth = sizeTemperature.cx;
				textHeight = sizeTemperature.cy;
				int yTemperature0 = m_lineTemperature[0].m_fMinValue;
				float fTemperatureRange = m_lineTemperature[0].m_fMaxValue - m_lineTemperature[0].m_fMinValue;
				float fTemperatureValuePerPixel = m_lineTemperature->m_fValuePerPixel;
				int iTemperatureGrid =(int)floor(fTemperatureRange/iGridCount);
				CString strTemperature;
				int yTemperature = (int)(m_rectPlot.bottom - (i*iTemperatureGrid)/fTemperatureValuePerPixel);
				strTemperature.Format(_T("%d"), yTemperature0+i*iTemperatureGrid);
				CRect rectTemperature(0, 0, 0, 0);
				rectTemperature.left = rectVibration.left - nInterval - textWidth;
				rectTemperature.right = rectVibration.left - nInterval;
				rectTemperature.top = yTemperature - textHeight/2;
				rectTemperature.bottom = yTemperature + textHeight/2;
				pDC->DrawText(strTemperature, rectTemperature, DT_CENTER|DT_VCENTER);
				CRect rtTemperature(rectTemperature.left-6, m_rectAxisY.bottom+8, rectTemperature.right, m_rectAxisX.bottom);
				pDC->DrawText(_T("温升"), rtTemperature, DT_CENTER|DT_VCENTER);

				//Torque(0, 120)
				CSize sizeTorque(0, 0);
				sizeTorque = pDC->GetTextExtent("-120");
				textWidth = sizeTorque.cx;
				textHeight = sizeTorque.cy;
				int yTorque0 = m_lineTorque.m_fMinValue;
				float fTorqueRange = m_lineTorque.m_fMaxValue - m_lineTorque.m_fMinValue;
				float fTorqueValuePerPixel = m_lineTorque.m_fValuePerPixel;
				int iTorqueGrid =(int)floor(fTorqueRange/iGridCount);
				CString strTorque;
				int yTorque = (int)(m_rectPlot.bottom - (i*iTorqueGrid)/fTorqueValuePerPixel);
				strTorque.Format(_T("%d"), yTorque0+i*iTorqueGrid);
				CRect rectTorque(0, 0, 0, 0);
				rectTorque.left = rectTemperature.left - nInterval - textWidth;
				rectTorque.right = rectTemperature.left - nInterval;
				rectTorque.top = yTorque - textHeight/2;
				rectTorque.bottom = yTorque + textHeight/2;
				pDC->DrawText(strTorque, rectTorque, DT_CENTER|DT_VCENTER);
				CRect rtTorque(rectTorque.left-6, m_rectAxisY.bottom+8, rectTorque.right, m_rectAxisX.bottom);
				pDC->DrawText(_T("转矩"), rtTorque, DT_CENTER|DT_VCENTER);
			}
		}
		pDC->SelectObject(oldPen);
	}
	
	//Draw X axis
	if(m_axisX.m_bDraw)
	{
		CPen pen(m_axisX.m_iStyle, m_axisX.m_iThick, RGB(192, 192, 192));
		CPen *oldPen = pDC->SelectObject(&pen);
		int iGridCount = 20;
		int iGrid =(int)floor((m_axisX.m_dMaxValue-m_axisX.m_dMinValue)/iGridCount);
		if(iGrid>0)
		{
			CString str;
			int textWidth = 0;
			int textHeight =16;
			CSize sizeStr(0, 0);
			sizeStr = pDC->GetTextExtent("5000");
			textHeight = sizeStr.cy;
			textWidth = sizeStr.cx;
			int y0=(int)m_axisX.m_dMinValue; 
			for(i=0;i<iGridCount+1;i++)
			{
				int y = (int)(m_rectPlot.left + (y0+i*iGrid-m_axisX.m_dMinValue)/m_axisX.m_dValuePerPixel);
				str.Format(_T("%d"), y0+i*iGrid);
				CRect rectText(0, 0, 0, 0);
				rectText.left = y-textWidth/2;
				rectText.right = y+textWidth/2;
				rectText.top = m_rectAxisX.top;
				rectText.bottom = m_rectAxisX.bottom;
				if(i%2 == 0)
					pDC->DrawText(str, rectText, DT_CENTER|DT_VCENTER);
				
				if(m_axisX.m_bGrid)
				{
					pDC->MoveTo(y, m_rectPlot.top);
					pDC->LineTo(y, m_rectPlot.bottom);
				}
			}
		}
		pDC->SelectObject(oldPen);
	}
}

void CLineChartCtrl::DrawLegends(CDC* pDC)
{
	int i = 0;
	int dx = m_rectLegend.Width()/13, iCount = 0;
	int left = m_rectLegend.left;
	int top = m_rectLegend.top;
	int iLegendHeight = m_rectLegend.Height();
	CString str;
	CSize sizeStr(0, 0);
	CRect rect(0, 0, 0, 0);
	CLineEx *line = NULL;
	str = _T("图例");
	sizeStr = pDC->GetTextExtent(str);

	rect.left = left + iCount * dx;
	rect.right = rect.left + dx;
	rect.top = top;
	rect.bottom = rect.top + iLegendHeight;
	CRect rectRev(rect);
	str = _T("转速");
	line = &m_lineRev;
	//if(line->m_bDraw)
	{
		DrawLegend(pDC, line, str, rectRev);
		iCount++;
	}

	for(i=0; i<3; i++)
	{
		rect.left = left + iCount * dx;
		rect.right = rect.left + dx;
		rect.top = top;
		rect.bottom = rect.top + iLegendHeight;
		CRect rectVibration(rect);
		str.Format(_T("振动%d"), i+1);
		line = &m_lineVibration[i];
		//if(line->m_bDraw)
		{
			DrawLegend(pDC, line, str, rectVibration);
			iCount++;
		}
	}
	
	for(i=0; i<7; i++)
	{
		rect.left = left + iCount * dx;
		rect.right = rect.left + dx;
		rect.top = top;
		rect.bottom = rect.top + iLegendHeight;
		CRect rectTemperature(rect);
		str.Format(_T("温升%d"), i+1);
		line = &m_lineTemperature[i];
		//if(line->m_bDraw)
		{
			DrawLegend(pDC, line, str, rectTemperature);
			iCount++;
		}
	}
	rect.left = left + iCount * dx;
	rect.right = rect.left + dx;
	rect.top = top;
	rect.bottom = rect.top + iLegendHeight;
	CRect rectTemperature(rect);
	str.Format(_T("室温"));
	line = &m_lineTemperature[i];
	//if(line->m_bDraw)
	{
		DrawLegend(pDC, line, str, rectTemperature);
		iCount++;
	}


	rect.left = left + iCount * dx;
	rect.right = rect.left + dx;
	rect.top = top;
	rect.bottom = rect.top + iLegendHeight;
	CRect rectTorque(rect);
	str.Format(_T("转矩"));
	line = &m_lineTorque;
	//if(line->m_bDraw)
	{
		DrawLegend(pDC, line, str, rectTorque);
		iCount++;
	}
}

void CLineChartCtrl::DrawLegend(CDC* pDC, CLineEx *line, CString name, CRect rect)
{
	int textWidth = 0;
	int textHeight = 0;
	CSize sizeStr(0, 0);
	sizeStr = pDC->GetTextExtent(name);
	textWidth = sizeStr.cx;
	textHeight = sizeStr.cy;
	CRect rectText(0, 0, 0, 0);
	int y = rect.left;
	rectText.left = y;
	rectText.right = y + textWidth;
	rectText.top = rect.top;
	rectText.bottom = rect.bottom;
	pDC->DrawText(name, rectText, DT_CENTER|DT_VCENTER);
	COLORREF clr = line->m_clrColor;
	pDC->FillSolidRect(rectText.right, rectText.top+2, rect.Width()-textWidth-2, rectText.Height()-8, clr);
}

void CLineChartCtrl::DrawLines(CDC *pDC)
{
	DrawRev(pDC);
	DrawVibration(pDC);
	DrawTemperature(pDC);
	DrawToraue(pDC);
}

void CLineChartCtrl::DrawLine(CDC* pDC, CLineEx *line)
{
	if(line == NULL) return;
	int i = 0;
	int iCnt = line->GetCount(); //line->m_iCount;
	BOOL bDraw = line->m_bDraw;
	CPoint *pt = NULL;
	if(iCnt > 0 && bDraw) 
	{
		pt = new CPoint[iCnt];
		if(pt == NULL) return;
		for(i=0;i<iCnt;i++)
		{
			pt[i].x = m_rectPlot.left + i/m_axisX.m_dValuePerPixel;
			pt[i].y = (int)(m_rectPlot.bottom - ((line->m_fYValue[i]-line->m_fMinValue)/line->m_fValuePerPixel));
		}
		CPen pen(line->m_iStyle, line->m_iThick, line->m_clrColor);
		CPen *oldPen = pDC->SelectObject(&pen);
		pDC->Polyline(pt, iCnt);
		pDC->SelectObject(oldPen);
		if(pt) delete pt;
	}
}

void CLineChartCtrl::DrawRev(CDC* pDC)
{
	DrawLine(pDC, &m_lineRev);
}

void CLineChartCtrl::DrawVibration(CDC* pDC)
{
	int i = 0;
	for(i=0; i<3; i++)
		DrawLine(pDC, &m_lineVibration[i]);
}
void CLineChartCtrl::DrawTemperature(CDC* pDC)
{
	int i = 0;
	for(i=0; i<8; i++)
		DrawLine(pDC, &m_lineTemperature[i]);
}
void CLineChartCtrl::DrawToraue(CDC* pDC)
{
	DrawLine(pDC, &m_lineTorque);
}

void CLineChartCtrl::Start()
{
	if(m_nRate > 0)
	{
		//SetTimer(1, m_nRate, NULL);
		SetTimer(1, 1000, NULL);
	}
}
void CLineChartCtrl::Stop()
{
	KillTimer(1);
}

void CLineChartCtrl::CalcLayout()
{
	GetClientRect(&m_rectCtrl);
	
	m_rectLegend.left = m_rectCtrl.left + m_iMarginLeft + 30;
	m_rectLegend.right = m_rectCtrl.right - m_iMarginRight;
	m_rectLegend.top = m_rectCtrl.bottom - m_iLegendHeight - m_iMarginBottom;
	m_rectLegend.bottom = m_rectCtrl.bottom - m_iMarginBottom;
	
	m_rectAxisY.left = m_rectCtrl.left + m_iMarginLeft;
	m_rectAxisY.right = m_rectAxisY.left + m_iAxisYWidth;
	m_rectAxisY.top   = m_rectCtrl.top + m_iMarginTop;
	m_rectAxisY.bottom = m_rectCtrl.bottom - m_iMarginBottom - m_iAxisXHeight - m_iLegendHeight;
	
	m_rectAxisX.left = m_rectAxisY.right;
	m_rectAxisX.right = m_rectCtrl.right - m_iMarginRight;
	m_rectAxisX.top = m_rectAxisY.bottom;
	m_rectAxisX.bottom = m_rectAxisX.top + m_iAxisXHeight;
		
	m_rectPlot.left   = m_rectAxisY.right;
	m_rectPlot.right  = m_rectAxisX.right;
	m_rectPlot.top    = m_rectAxisY.top;
	m_rectPlot.bottom = m_rectAxisX.top;
	
	m_axisY.m_dValuePerPixel = (m_axisY.m_dMaxValue- m_axisY.m_dMinValue) / m_rectPlot.Height();
	m_axisX.m_dValuePerPixel = (m_axisX.m_dMaxValue- m_axisX.m_dMinValue) / m_rectPlot.Width();
	TRACE("m_axisY.m_dValuePerPixel=%.1f m_axisX.m_dValuePerPixel=%.1f\n",m_axisY.m_dValuePerPixel, m_axisX.m_dValuePerPixel);
	int i = 0;
	CLineEx *line = NULL;
	//Rev
	line = &m_lineRev;
	line->m_fValuePerPixel = (line->m_fMaxValue-line->m_fMinValue)/m_rectPlot.Height();
	TRACE("m_lineRev.m_dValuePerPixel=%.2f\n",line->m_fValuePerPixel);
	//Vibration
	line = m_lineVibration;
	for(i=0; i<3; i++)
		line[i].m_fValuePerPixel = (line[i].m_fMaxValue-line[i].m_fMinValue)/m_rectPlot.Height();
	TRACE("m_lineVibration[0].m_dValuePerPixel=%.2f\n",m_lineVibration[0].m_fValuePerPixel);
	//Temperature
	line = m_lineTemperature;
	for(i=0; i<8; i++)
		line[i].m_fValuePerPixel = (line[i].m_fMaxValue-line[i].m_fMinValue)/m_rectPlot.Height();
	TRACE("m_lineTemperature[0].m_dValuePerPixel=%.2f\n",m_lineTemperature[0].m_fValuePerPixel);
	//Rev
	line = &m_lineTorque;
	line->m_fValuePerPixel = (line->m_fMaxValue-line->m_fMinValue)/m_rectPlot.Height();
	TRACE("m_lineTorque.m_dValuePerPixel=%.2f\n",m_lineTorque.m_fValuePerPixel);
}

void CLineChartCtrl::SetRate(int nRate)
{
	m_nRate = nRate;
}
void CLineChartCtrl::SetBkColor(COLORREF clrBkColor)
{
	m_clrBkColor = clrBkColor;
}

CAxis& CLineChartCtrl::GetAxisY()
{
	return m_axisY;
}
CAxis& CLineChartCtrl::GetAxisX()
{
	return m_axisX;
}

void CLineChartCtrl::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	CalcLayout();
	Invalidate();
}

void CLineChartCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CString strPoint;
	long x = point.x;
	long y = point.y;
	float fYValue = 0.00;
	int i = 0, iIndex = 0;
	CLineEx *line = NULL;
	BOOL bShow = FALSE;
	iIndex = (int)((x - m_rectPlot.left) * m_axisX.m_dValuePerPixel);
	if(iIndex<0 || iIndex>=5000) return;

	line = &m_lineRev;
	fYValue = (m_rectPlot.bottom - y) * line->m_fValuePerPixel + line->m_fMinValue;
	if(!bShow && fabs(fYValue - line->m_fYValue[iIndex])<0.01)
		bShow = TRUE;
	
	for(i=0; i<3 && !bShow; i++)
	{
		line = &m_lineVibration[i];
		fYValue = (m_rectPlot.bottom - y) * line->m_fValuePerPixel + line->m_fMinValue;
		if(fabs(fYValue - line->m_fYValue[iIndex])<0.01)
		{
			bShow = TRUE;
			break;
		}
	}

	for(i=0; i<8 && !bShow; i++)
	{
		line = &m_lineTemperature[i];
		fYValue = (m_rectPlot.bottom - y) * line->m_fValuePerPixel + line->m_fMinValue;
		if(fabs(fYValue - line->m_fYValue[iIndex])<0.01)
		{
			bShow = TRUE;
			break;
		}
	}
	
	line = &m_lineTorque;
	fYValue = (m_rectPlot.bottom - y) * line->m_fValuePerPixel + line->m_fMinValue;
	if(!bShow && fabs(fYValue - line->m_fYValue[iIndex])<0.001)
		bShow = TRUE;

	if(bShow)
	{
		CDrawLineDlg dlg;
		dlg.SetLine(line);
		dlg.DoModal();
	}
	CStatic::OnLButtonDblClk(nFlags, point);
}
