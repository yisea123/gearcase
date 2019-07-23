// AlarmSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "smssender.h"
#include "AlarmSetDlg.h"
#include "SmsSenderDlg.h"
#include "InputPasswdDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlarmSetDlg dialog


CAlarmSetDlg::CAlarmSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlarmSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAlarmSetDlg)
		// NOTE: the ClassWizard will add member initialization here
	m_bIsAlarmTemperature = FALSE;
	m_bIsAlarmVibration = FALSE;
	m_fAlarmTemperature = 0.00;
	m_fAlarmVibration = 0.00;
	//}}AFX_DATA_INIT
}


void CAlarmSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlarmSetDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlarmSetDlg, CDialog)
	//{{AFX_MSG_MAP(CAlarmSetDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlarmSetDlg message handlers

BOOL CAlarmSetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString sAlarm;
	if(m_bIsAlarmTemperature) 
	{
		CButton *pBn = (CButton *)GetDlgItem(IDC_CHECK_TEMPERATURE);
		pBn->SetCheck(TRUE);
	}
	sAlarm.Format(_T("%.02f"), m_fAlarmTemperature);
	SetDlgItemText(IDC_EDIT_TEMPERATURE, sAlarm);
	if(m_bIsAlarmVibration)
	{
		CButton *pBn = (CButton *)GetDlgItem(IDC_CHECK_VIBRATION);
		pBn->SetCheck(TRUE);
	}
	sAlarm.Format(_T("%.02f"), m_fAlarmVibration);
	SetDlgItemText(IDC_EDIT_VIBRATION, sAlarm);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAlarmSetDlg::OnOK() 
{
	// TODO: Add extra validation here
	CString sAlarm;
	int nAlarm = 0, inter = 0, fraction = 0;
	CButton *pBn = (CButton *)GetDlgItem(IDC_CHECK_TEMPERATURE);
	m_bIsAlarmTemperature = pBn->GetCheck();
	GetDlgItemText(IDC_EDIT_TEMPERATURE, sAlarm);
	float fAlarmTemperature = atof(sAlarm);
	if(MIN_ALARM_TEMPERATURE<=fAlarmTemperature && fAlarmTemperature<=MAX_ALARM_TEMPERATURE)
			m_fAlarmTemperature = fAlarmTemperature;

	pBn = (CButton *)GetDlgItem(IDC_CHECK_VIBRATION);
	m_bIsAlarmVibration = pBn->GetCheck();
	GetDlgItemText(IDC_EDIT_VIBRATION, sAlarm);
	float fAlarmVibration = atof(sAlarm);
	if(MIN_ALARM_VIBRATION<=fAlarmVibration && fAlarmVibration<=MAX_ALARM_VIBRATION)
			m_fAlarmVibration = fAlarmVibration;

	CSmsSenderDlg *pMainDlg = (CSmsSenderDlg *)AfxGetApp()->m_pMainWnd;
	if(pMainDlg)
	{
		CInputPasswdDlg dlg;
		int iRet = 0;
		iRet = dlg.DoModal();
		if(iRet != IDOK) return;
		iRet = MessageBox(_T("确认要重新设置报警值吗?"), _T("报警设置"), MB_OKCANCEL | MB_ICONQUESTION);
		if(iRet != IDOK) return;

		pMainDlg->m_bIsAlarmTemperature = m_bIsAlarmTemperature;
		pMainDlg->m_bIsAlarmVibration = m_bIsAlarmVibration;
		pMainDlg->m_fAlarmTemperature = m_fAlarmTemperature;
		pMainDlg->m_fAlarmVibration = m_fAlarmVibration;
		CString sAlarmLog;
		sAlarmLog.Format(_T("[报警设置]m_bIsAlarmTemperature=%d m_fAlarmTemperature=%.02f m_bIsAlarmVibration=%d m_fAlarmVibration=%.02f"),
			m_bIsAlarmTemperature, m_fAlarmTemperature, m_bIsAlarmVibration, m_fAlarmVibration);
		pMainDlg->P_ShowLog(sAlarmLog);
	}

	CDialog::OnOK();
}
