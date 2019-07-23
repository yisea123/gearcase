// InputPasswdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "smssender.h"
#include "InputPasswdDlg.h"
#include "SmsSenderDlg.h"
#include "MD5Checksum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputPasswdDlg dialog


CInputPasswdDlg::CInputPasswdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputPasswdDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputPasswdDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CInputPasswdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputPasswdDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputPasswdDlg, CDialog)
	//{{AFX_MSG_MAP(CInputPasswdDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputPasswdDlg message handlers

void CInputPasswdDlg::OnOK() 
{
	// TODO: Add extra validation here
	char sPasswd[64] = {0};
	GetDlgItemText(IDC_EDIT_PASSWD, sPasswd, 64);
	sPasswd[63] = '\0';
	int iLen = strlen(sPasswd);
	CString strPasswd = CMD5Checksum::GetMD5((BYTE*)sPasswd, iLen);
	CSmsSenderDlg *pMainDlg = (CSmsSenderDlg *)AfxGetApp()->m_pMainWnd;
	if(pMainDlg)
	{
		CString strMd5Pd = pMainDlg->m_sPasswd;
		if(strPasswd != strMd5Pd)
		{
			MessageBox(_T("输入的密码不正确!"), _T("密码输入"), MB_OK);
			return;
		}
	}
	CDialog::OnOK();
}
