// CurveColorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "smssender.h"
#include "CurveColorDlg.h"
#include "SmsSenderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCurveColorDlg dialog


CCurveColorDlg::CCurveColorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCurveColorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCurveColorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	int i = 0;
	m_bRev = FALSE;
	for(i=0; i<3; i++)
	{
		m_bVibration[i] = FALSE;
	}
	for(i=0; i<8; i++)
	{
		m_bTemperature[i] = FALSE;
	}
	m_bTorque = FALSE;

	//转速
	m_clrRev = RGB(0xDB, 0x93, 0x70);
	//振动
	m_clrVibration[0] = RGB(0xDC, 0x14, 0x3C);
	m_clrVibration[1] = RGB(0x41, 0x69, 0xE1);
	m_clrVibration[2] = RGB(0x20, 0xB2, 0xAA);
	//温度
	m_clrTemperature[0] = RGB(0xEE, 0x82, 0xEE);
	m_clrTemperature[1] = RGB(0xFF, 0x00, 0xFF);
	m_clrTemperature[2] = RGB(0x8B, 0x00, 0x8B);
	m_clrTemperature[3] = RGB(0x4B, 0x00, 0x82);
	m_clrTemperature[4] = RGB(0x78, 0x6B, 0xEE);
	m_clrTemperature[5] = RGB(0x48, 0x3D, 0x8B);
	m_clrTemperature[6] = RGB(0x00, 0x00, 0xFF);
	m_clrTemperature[7] = RGB(0x19, 0x19, 0x70);
	//转矩
	m_clrTorque = RGB(0xFF, 0xA5, 0x00);
}


void CCurveColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCurveColorDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCurveColorDlg, CDialog)
	//{{AFX_MSG_MAP(CCurveColorDlg)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(IDC_BN_REVCOLOR, IDC_BN_TORQUE, OnSelectColor)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCurveColorDlg message handlers

BOOL CCurveColorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int i = 0;
	CButton *pBn = NULL;
	pBn = (CButton *)GetDlgItem(IDC_CHECK_REV);
	pBn->SetCheck(m_bRev);
	for(i=0; i<3; i++)
	{
		pBn = (CButton *)GetDlgItem(IDC_CHECK_VIBRATION1+i);
		pBn->SetCheck(m_bVibration[i]);
	}
	for(i=0; i<8; i++)
	{
		pBn = (CButton *)GetDlgItem(IDC_CHECK_TEMPERATURE1+i);
		pBn->SetCheck(m_bTemperature[i]);
	}
	pBn = (CButton *)GetDlgItem(IDC_CHECK_TORQUE);
	pBn->SetCheck(m_bTorque);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCurveColorDlg::OnSelectColor(UINT nID)
{
	COLORREF clr = 0;
	CColorDialog dlg;
	if(nID == IDC_BN_REVCOLOR)
		clr = m_clrRev;
	if(IDC_BN_VIBRATION1<=nID && nID<=IDC_BN_VIBRATION3)
		clr = m_clrVibration[nID-IDC_BN_VIBRATION1];
	if(IDC_BN_TEMPERATURE1<=nID && nID<=IDC_BN_TEMPERATURE8)
		clr = m_clrTemperature[nID-IDC_BN_TEMPERATURE1];
	if(nID == IDC_BN_TORQUE)
		clr = m_clrTorque;
	dlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;
	dlg.m_cc.rgbResult = clr;
	int iRet = dlg.DoModal();
	if(iRet == IDOK)
	{
		clr = dlg.GetColor();
		if(nID == IDC_BN_REVCOLOR)
			m_clrRev = clr;
		if(IDC_BN_VIBRATION1<=nID && nID<=IDC_BN_VIBRATION3)
			m_clrVibration[nID-IDC_BN_VIBRATION1] = clr;
		if(IDC_BN_TEMPERATURE1<=nID && nID<=IDC_BN_TEMPERATURE8)
			m_clrTemperature[nID-IDC_BN_TEMPERATURE1] = clr;
		if(nID == IDC_BN_TORQUE)
			m_clrTorque = clr;
		Invalidate();
		UpdateWindow();
	}
}

void CCurveColorDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	int i = 0;
	CRect rect;
	CBrush brush, *old = NULL;
	CDC * pDC = NULL;
	CWnd * pWnd = NULL;
	//转速颜色
	pWnd = GetDlgItem(IDC_STC_REVCOLOR);
	pWnd->GetClientRect(&rect);
	pDC = pWnd->GetDC();
	pWnd->Invalidate();
	pWnd->UpdateWindow();
	brush.CreateSolidBrush(m_clrRev);
	old = pDC->SelectObject(&brush);
	pDC->Rectangle(&rect);
	pDC->SelectObject(old);
	pWnd->ReleaseDC(pDC);
	brush.DeleteObject();
	//振动颜色
	for(i=0; i<3; i++)
	{
		pWnd = GetDlgItem(IDC_STC_VIBRATION1+i);
		pWnd->GetClientRect(&rect);
		pDC = pWnd->GetDC();
		pWnd->Invalidate();
		pWnd->UpdateWindow();
		brush.CreateSolidBrush(m_clrVibration[i]);
		old = pDC->SelectObject(&brush);
		pDC->Rectangle(&rect);
		pDC->SelectObject(old);
		pWnd->ReleaseDC(pDC);
		brush.DeleteObject();
	}
	//温度颜色
	for(i=0; i<8; i++)
	{
		pWnd = GetDlgItem(IDC_STC_TEMPERATURE1+i);
		pWnd->GetClientRect(&rect);
		pDC = pWnd->GetDC();
		pWnd->Invalidate();
		pWnd->UpdateWindow();
		brush.CreateSolidBrush(m_clrTemperature[i]);
		old = pDC->SelectObject(&brush);
		pDC->Rectangle(&rect);
		pDC->SelectObject(old);
		pWnd->ReleaseDC(pDC);
		brush.DeleteObject();
	}
	//转矩颜色
	pWnd = GetDlgItem(IDC_STC_TORQUE);
	pWnd->GetClientRect(&rect);
	pDC = pWnd->GetDC();
	pWnd->Invalidate();
	pWnd->UpdateWindow();
	brush.CreateSolidBrush(m_clrTorque);
	old = pDC->SelectObject(&brush);
	pDC->Rectangle(&rect);
	pDC->SelectObject(old);
	pWnd->ReleaseDC(pDC);
	brush.DeleteObject();
	// Do not call CDialog::OnPaint() for painting messages
}

void CCurveColorDlg::OnOK() 
{
	// TODO: Add extra validation here
	CSmsSenderDlg *pMainDlg = (CSmsSenderDlg *)AfxGetApp()->m_pMainWnd;
	if(pMainDlg)
	{
		int i = 0;
		CButton *pBn = NULL;
		CLineEx *line = NULL;
		pBn = (CButton *)GetDlgItem(IDC_CHECK_REV);
		m_bRev = pBn->GetCheck();
		pMainDlg->m_bRev = m_bRev;
		pMainDlg->m_clrRev = m_clrRev;
		line = &(pMainDlg->m_stcDraw.m_lineRev);
		line->m_bDraw = m_bRev;
		line->m_clrColor = m_clrRev;

 		for(i=0; i<3; i++)
 		{
			pBn = (CButton *)GetDlgItem(IDC_CHECK_VIBRATION1+i);
			m_bVibration[i] = pBn->GetCheck();
			pMainDlg->m_bVibration[i] = m_bVibration[i];
			pMainDlg->m_clrVibration[i] = m_clrVibration[i];
			line = &(pMainDlg->m_stcDraw.m_lineVibration[i]);
			line->m_bDraw = m_bVibration[i];
			line->m_clrColor = m_clrVibration[i];
 		}
 		for(i=0; i<8; i++)
 		{
			pBn = (CButton *)GetDlgItem(IDC_CHECK_TEMPERATURE1+i);
			m_bTemperature[i] = pBn->GetCheck();
			pMainDlg->m_bTemperature[i] = m_bTemperature[i];
			pMainDlg->m_clrTemperature[i] = m_clrTemperature[i];
			line = &(pMainDlg->m_stcDraw.m_lineTemperature[i]);
			line->m_bDraw = m_bTemperature[i];
			line->m_clrColor = m_clrTemperature[i];
 		}
		pBn = (CButton *)GetDlgItem(IDC_CHECK_TORQUE);
		m_bTorque = pBn->GetCheck();
		pMainDlg->m_bTorque = m_bTorque;
		pMainDlg->m_clrTorque = m_clrTorque;
		line = &(pMainDlg->m_stcDraw.m_lineTorque);
		line->m_bDraw = m_bTorque;
		line->m_clrColor = m_clrTorque;
	}
	CDialog::OnOK();
}
