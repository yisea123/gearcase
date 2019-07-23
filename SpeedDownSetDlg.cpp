// SpeedDownSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "smssender.h"
#include "SpeedDownSetDlg.h"
#include "SmsSenderDlg.h"
#include "InputPasswdDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpeedDownSetDlg dialog


CSpeedDownSetDlg::CSpeedDownSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpeedDownSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpeedDownSetDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_fSpeedDownRate = 1.0000;
}


void CSpeedDownSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpeedDownSetDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpeedDownSetDlg, CDialog)
	//{{AFX_MSG_MAP(CSpeedDownSetDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpeedDownSetDlg message handlers

BOOL CSpeedDownSetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString sRate;
	sRate.Format(_T("%.04f"), m_fSpeedDownRate);
	SetDlgItemText(IDC_EDIT_SPEEDDOWNRATE, sRate);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSpeedDownSetDlg::OnOK() 
{
	// TODO: Add extra validation here
	CString sRate;
	GetDlgItemText(IDC_EDIT_SPEEDDOWNRATE, sRate);
 	float fRate = atof(sRate);
 	if(MIN_SPEEDDWONRATE<=fRate && fRate<=MAX_SPEEDDWONRATE)
 		m_fSpeedDownRate = fRate;

 	CSmsSenderDlg *pMainDlg = (CSmsSenderDlg *)AfxGetApp()->m_pMainWnd;
 	if(pMainDlg)
 	{
 		CInputPasswdDlg dlg;
 		int iRet = 0;
 		iRet = dlg.DoModal();
 		if(iRet != IDOK) return;
 		iRet = MessageBox(_T("确认要重新设置减速比吗?"), _T("减速比设置"), MB_OKCANCEL | MB_ICONQUESTION);
 		if(iRet != IDOK) return;
 		
		pMainDlg->m_fSpeedDownRate = m_fSpeedDownRate;
 		CString sRateSet;
 		sRateSet.Format(_T("[减速比设置] m_fSpeedDownRate=%.04f"), m_fSpeedDownRate);
 		pMainDlg->P_ShowLog(sRateSet);
 	}
	
	CDialog::OnOK();
}
