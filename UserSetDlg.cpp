// UserSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "smssender.h"
#include "UserSetDlg.h"
#include "SmsSenderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserSetDlg dialog


CUserSetDlg::CUserSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUserSetDlg)
		// NOTE: the ClassWizard will add member initialization here
	m_sUserName = _T("");
	m_sUserNum = _T("");
	m_sProductType = _T("");
	m_sProductNum = _T("");
	m_sProducer = _T("");
	m_sTestName = _T("");
	m_sTestTime = _T("");
	//}}AFX_DATA_INIT
}


void CUserSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserSetDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserSetDlg, CDialog)
	//{{AFX_MSG_MAP(CUserSetDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserSetDlg message handlers

BOOL CUserSetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetDlgItemText(IDC_EDIT_USERNAME, m_sUserName);
	SetDlgItemText(IDC_EDIT_USERNUM, m_sUserNum);
	SetDlgItemText(IDC_EDIT_PRODUCTTYPE, m_sProductType);
	SetDlgItemText(IDC_EDIT_PRODUCTNUM, m_sProductNum);
	SetDlgItemText(IDC_EDIT_PRODUCER, m_sProducer);
	SetDlgItemText(IDC_EDIT_TESTNAME, m_sTestName);
	SetDlgItemText(IDC_EDIT_TESTTIME, m_sTestTime);
	SetTimer(1, 1000, NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUserSetDlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	CTime t = CTime::GetCurrentTime();
	SetDlgItemText(IDC_EDIT_TESTTIME, t.Format("%Y-%m-%d %H:%M:%S"));
	
	CDialog::OnTimer(nIDEvent);
}

void CUserSetDlg::OnOK() 
{
	// TODO: Add extra validation here
	GetDlgItemText(IDC_EDIT_USERNAME, m_sUserName);
	GetDlgItemText(IDC_EDIT_USERNUM, m_sUserNum);
	GetDlgItemText(IDC_EDIT_PRODUCTTYPE, m_sProductType);
	GetDlgItemText(IDC_EDIT_PRODUCTNUM, m_sProductNum);
	GetDlgItemText(IDC_EDIT_PRODUCER, m_sProducer);
	GetDlgItemText(IDC_EDIT_TESTNAME, m_sTestName);
	GetDlgItemText(IDC_EDIT_TESTTIME, m_sTestTime);

	CSmsSenderDlg *pMainDlg = (CSmsSenderDlg *)AfxGetApp()->m_pMainWnd;
	if(pMainDlg)
	{
		pMainDlg->m_sUserName = m_sUserName;
		pMainDlg->m_sUserNum = m_sUserNum;
		pMainDlg->m_sProductType = m_sProductType;
		pMainDlg->m_sProductNum = m_sProductNum;
		pMainDlg->m_sProducer = m_sProducer;
		pMainDlg->m_sTestName = m_sTestName;
		pMainDlg->m_sTestTime = m_sTestTime;
		CString sLog;
		sLog.Format(_T("[ÓÃ»§ÉèÖÃ]m_sUserName=%s m_sUserNum=%s m_sProductType=%s m_sProductNum=%s m_sProducer=%s m_sTestName=%s m_sTestTime=%s"),
			m_sUserName, m_sUserNum, m_sProductType, m_sProductNum, m_sProducer, m_sTestName, m_sTestTime);
		pMainDlg->P_ShowLog(sLog);
	}
	CDialog::OnOK();
}
