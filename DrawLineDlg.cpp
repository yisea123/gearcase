// DrawLineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "smssender.h"
#include "DrawLineDlg.h"
//#include "MemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawLineDlg dialog


CDrawLineDlg::CDrawLineDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDrawLineDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDrawLineDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_clrBkColor = RGB(0,0,0);
	m_nRate = 1000;
	
	m_iMarginLeft = 5;
	m_iMarginRight = 15;
	m_iMarginTop = 10;
	m_iMarginBottom = 5;
	m_iAxisXHeight = 20;
	m_iLegendHeight = 0;
	m_iAxisYWidth = 40;

	m_fXValuePerPixel = 1.00;
	m_fYValuePerPixel = 1.00;
}


void CDrawLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrawLineDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDrawLineDlg, CDialog)
	//{{AFX_MSG_MAP(CDrawLineDlg)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawLineDlg message handlers

void CDrawLineDlg::SetLine(CLineEx *line)
{
	int i = 0, iCnt = 0;
	m_line.m_bDraw = line->m_bDraw;
	m_line.m_clrColor = line->m_clrColor;
	m_line.m_iStyle = line->m_iStyle;
	m_line.m_iThick = line->m_iThick;
	m_line.m_fMaxValue = line->m_fMaxValue;
	m_line.m_fMinValue = line->m_fMinValue;
	m_line.m_fValuePerPixel = line->m_fValuePerPixel;
	iCnt = line->GetCount();
	m_line.SetCount(iCnt);
	for(i=0; i<iCnt; i++)
	{
		m_line.m_lXValue[i] = line->m_lXValue[i];
		m_line.m_fYValue[i] = line->m_fYValue[i];
	}
	strcpy(m_line.m_szTitle, line->m_szTitle);
}

void CDrawLineDlg::CalcLayout()
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
	
	m_fYValuePerPixel = (float)(5000.00 - (-5000.00)) / m_rectPlot.Height();
	m_fXValuePerPixel = (float)(5000.00 - 0.00) / m_rectPlot.Width();
	TRACE("m_fYValuePerPixel=%.1f m_fXValuePerPixel=%.1f\n", m_fYValuePerPixel, m_fXValuePerPixel);

	m_line.m_fValuePerPixel = (m_line.m_fMaxValue-m_line.m_fMinValue)/m_rectPlot.Height();
}

BOOL CDrawLineDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strTitle;
	strTitle.Format(_T("曲线: %s"), m_line.m_szTitle);
	SetWindowText(strTitle);

	CRect rect;
	GetWindowRect(&rect);
	//delete the showing window's caption
	LONG style = GetWindowLong(m_hWnd, GWL_STYLE);
	//style &= ~WS_CAPTION;
	//set the window's show sytle
	SetWindowLong(m_hWnd, GWL_STYLE, style);
	//get the screen's pixel
	int screenX = GetSystemMetrics(SM_CXSCREEN);
	int screenY = GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(NULL, 0, 0, screenX, screenY, SWP_NOZORDER);

	CalcLayout();	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDrawLineDlg::DrawBasic(CDC *pDC) //画整个图区的属性、边框和背景
{
	CRect rect(&m_rectCtrl);
	pDC->SetBkMode(TRANSPARENT); // 设置透明
	COLORREF clr = RGB(0x8F, 0xBC, 0x8F);
	pDC->FillSolidRect(&rect, clr);
}

void CDrawLineDlg::DrawAxises(CDC* pDC)
{
	int i = 0;
	int nInterval = 10;
	
	//Draw Y axis
	CPen pen(PS_SOLID, 1, RGB(192, 192, 192));
	CPen *oldPen = pDC->SelectObject(&pen);
	//Draw border of plot
	pDC->MoveTo(m_rectPlot.left,   m_rectPlot.top);  
	pDC->LineTo(m_rectPlot.right,  m_rectPlot.top);  
	pDC->LineTo(m_rectPlot.right,  m_rectPlot.bottom);  
	pDC->LineTo(m_rectPlot.left,   m_rectPlot.bottom);  
	pDC->LineTo(m_rectPlot.left,   m_rectPlot.top); 

	int iYGridCount = 20;
	int iYGrid =(int)floor((5000.00 - (-5000.00))/iYGridCount);
	if(iYGrid>0)
	{
		CString str;
		int textWidth = 0;
		int textHeight = 16;
		CSize sizeStr(0, 0);
		sizeStr = pDC->GetTextExtent("-5000");
		textHeight = sizeStr.cy;
		int y0=(int) (-5000.00);
		for(i=0;i<iYGridCount+1;i++)
		{
			//AxisY
			int y = (int)(m_rectPlot.bottom - (i*iYGrid)/m_fYValuePerPixel);
			pDC->MoveTo(m_rectPlot.left,y);
			pDC->LineTo(m_rectPlot.right,y);

			//Rev(-5000, 5000)
			CSize sizeRev(0, 0);
			sizeRev = pDC->GetTextExtent("-5000");
			textWidth = sizeRev.cx;
			textHeight = sizeRev.cy;
			int yRev0 = (int)(m_line.m_fMinValue);
			float fRevValuePerPixel = m_line.m_fValuePerPixel;
			float fRevRange = (m_line.m_fMaxValue-m_line.m_fMinValue);
			int iRevGrid =(int)floor(fRevRange/iYGridCount);
			CString strRev;
			int yRev = (int)(m_rectPlot.bottom - (i*iRevGrid)/fRevValuePerPixel);
			strRev.Format(_T("%d"), yRev0+i*iRevGrid);
			CRect rectRev(0, 0, 0, 0);
			rectRev.left = m_rectAxisY.right - textWidth;
			rectRev.right = m_rectAxisY.right;
			rectRev.top = yRev - textHeight/2;
			rectRev.bottom = yRev + textHeight/2;
			pDC->DrawText(strRev, rectRev, DT_CENTER|DT_VCENTER);
			//CRect rtRev(rectRev.left, m_rectAxisY.bottom+8, rectRev.right-6, m_rectAxisX.bottom);
			//pDC->DrawText(_T("转速"), rtRev, DT_CENTER|DT_VCENTER);
		}
	}

	//Draw X axis
	int iXGridCount = 20;
	int iXGrid =(int)floor((5000.00 - 0.00)/iXGridCount);
	if(iXGrid>0)
	{
		CString str;
		int textWidth = 0;
		int textHeight =16;
		CSize sizeStr(0, 0);
		sizeStr = pDC->GetTextExtent("5000");
		textHeight = sizeStr.cy;
		textWidth = sizeStr.cx;
		int y0 = (int)0.00;
		for(i=0;i<iXGridCount+1;i++)
		{
			int y = (int)(m_rectPlot.left + (i*iXGrid)/m_fXValuePerPixel);
			str.Format(_T("%d"), y0+i*iXGrid);
			CRect rectText(0, 0, 0, 0);
			rectText.left = y-textWidth/2;
			rectText.right = y+textWidth/2;
			rectText.top = m_rectAxisX.top;
			rectText.bottom = m_rectAxisX.bottom;
			if(i%2 == 0)
				pDC->DrawText(str, rectText, DT_CENTER|DT_VCENTER);

			pDC->MoveTo(y, m_rectPlot.top);
			pDC->LineTo(y, m_rectPlot.bottom);
		}
	}
	pDC->SelectObject(oldPen);
}

void CDrawLineDlg::DrawLine(CDC* pDC, CLineEx *line)
{
	if(line == NULL) return;
	int i = 0;
	int iCnt = line->GetCount();
	BOOL bDraw = line->m_bDraw;
	CPoint *pt = NULL;
	if(iCnt > 0) 
	{
		pt = new CPoint[iCnt];
		if(pt == NULL) return;
		for(i=0;i<iCnt;i++)
		{
			pt[i].x = m_rectPlot.left + i/m_fXValuePerPixel;
			pt[i].y = (int)(m_rectPlot.bottom - ((line->m_fYValue[i]-line->m_fMinValue)/line->m_fValuePerPixel));
		}
		CPen pen(line->m_iStyle, line->m_iThick, line->m_clrColor);
		CPen *oldPen = pDC->SelectObject(&pen);
		pDC->Polyline(pt, iCnt);
		pDC->SelectObject(oldPen);
		if(pt) delete pt;
	}
}

void CDrawLineDlg::OnDraw(CDC *pDC)
{
// 	CFont font;
// 	LOGFONT lf;
// 	memset(&lf, 0, sizeof(LOGFONT));
// 	lf.lfHeight = 12;
// 	strcpy(lf.lfFaceName, "宋体");
// 	font.CreateFontIndirect(&lf);
// 	CFont* oldfont = pDC->SelectObject(&font);

	DrawBasic(pDC);
	DrawAxises(pDC);
	//DrawLegends(pDC);
	//DrawLines(pDC);
	DrawLine(pDC, &m_line);
	
// 	pDC->SelectObject(oldfont);
// 	font.DeleteObject();
}

void CDrawLineDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	OnDraw(&dc);
	
	// Do not call CDialog::OnPaint() for painting messages
}
