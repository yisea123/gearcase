// SpeedSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "smssender.h"
#include "SpeedSetDlg.h"
#include "SmsSenderDlg.h"
#include "InputPasswdDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpeedSetDlg dialog


CSpeedSetDlg::CSpeedSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpeedSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpeedSetDlg)
	//}}AFX_DATA_INIT
	int i = 0;
	for(i=0; i<7; i++)
	{
		m_wSpeed[i] = 0;
		m_nInterval[i] = 0;
	}
	m_wRunSpeed = 0;
}


void CSpeedSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpeedSetDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpeedSetDlg, CDialog)
	//{{AFX_MSG_MAP(CSpeedSetDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpeedSetDlg message handlers

BOOL CSpeedSetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int i = 0;
	for(i=0; i<7; i++)
	{
		CString sSpeed;
		sSpeed.Format(_T("%d"), m_wSpeed[i]);
		SetDlgItemText(IDC_EDIT_SPEED1+i, sSpeed);
		SetDlgItemInt(IDC_EDIT_INTERVAL1+i, m_nInterval[i]);
	}
	CString sRunSpeed;
	sRunSpeed.Format(_T("%d"), m_wRunSpeed);
	SetDlgItemText(IDC_EDIT_RUNSPEED, sRunSpeed);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSpeedSetDlg::OnOK() 
{
	// TODO: Add extra validation here
	CString sSpeed;
	WORD wSpeed = 0;
	int i = 0;
	for(i=0; i<7; i++)
	{
		int inter = 0, fraction = 0;
		GetDlgItemText(IDC_EDIT_SPEED1+i, sSpeed);
		int nRet = sscanf(sSpeed, _T("%d"), &wSpeed);
		if(nRet == 1)
		{
			if(MIN_SPEED_RPM<=wSpeed && wSpeed<=MAX_SPEED_RPM)
				m_wSpeed[i] = wSpeed;
		}

		CString sInterval;
		int nInterval = 0;
		GetDlgItemText(IDC_EDIT_INTERVAL1+i, sInterval);
		nInterval = atoi(sInterval);
		if(MIN_INTERVAL<=nInterval && nInterval<=MAX_INTERVAL)
			m_nInterval[i] = nInterval;
	}

	CString sRunSpeed;
	WORD wRunSpeed = 0;
	GetDlgItemText(IDC_EDIT_RUNSPEED, sRunSpeed);
	int nRet = sscanf(sRunSpeed, _T("%d"), &wRunSpeed);
	if(nRet == 1)
	{
		if(MIN_SPEED_RPM<=wRunSpeed && wRunSpeed<=MAX_SPEED_RPM)
			m_wRunSpeed = wRunSpeed;
	}

	CSmsSenderDlg *pMainDlg = (CSmsSenderDlg *)AfxGetApp()->m_pMainWnd;
	if(pMainDlg)
	{
		CInputPasswdDlg dlg;
		int iRet = 0;
		iRet = dlg.DoModal();
		if(iRet != IDOK) return;
		iRet = MessageBox(_T("确认要重新设置段速吗?"), _T("段速设置"), MB_OKCANCEL | MB_ICONQUESTION);
		if(iRet != IDOK) return;

		CString sSpeedLog, sIntervalLog, sTmp;
		for(i=0; i<7; i++)
		{
			pMainDlg->m_wSpeed[i] = m_wSpeed[i];
			pMainDlg->m_nInterval[i] = m_nInterval[i];
			sTmp.Format(_T("m_sSpeed[%d]=%d "), i, m_wSpeed[i]);
			sSpeedLog = sSpeedLog + sTmp;
			sTmp.Format(_T("m_nInterval[%d]=%d "), i, m_nInterval[i]);
			sIntervalLog = sIntervalLog + sTmp;
		}
		pMainDlg->m_wRunSpeed = m_wRunSpeed;
		CString sLog;
		sLog.Format(_T("段速和运行速度设置 %s %s m_wRunSpeed=%d"), sSpeedLog, sIntervalLog, m_wRunSpeed);
		pMainDlg->P_ShowLog(sLog);
	}

	CDialog::OnOK();
}
