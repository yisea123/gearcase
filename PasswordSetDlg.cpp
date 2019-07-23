// PasswordSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "smssender.h"
#include "PasswordSetDlg.h"
#include "MD5Checksum.h"
#include "SmsSenderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPasswordSetDlg dialog


CPasswordSetDlg::CPasswordSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPasswordSetDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPasswordSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordSetDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPasswordSetDlg, CDialog)
	//{{AFX_MSG_MAP(CPasswordSetDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswordSetDlg message handlers

void CPasswordSetDlg::OnOK() 
{
	// TODO: Add extra validation here
	char oldpd[64] = {0}, newpd[64] = {0}, confirmpd[64] = {0};
	GetDlgItemText(IDC_EDIT_OLDPD, oldpd, 64);
	GetDlgItemText(IDC_EDIT_NEWPD, newpd, 64);
	GetDlgItemText(IDC_EDIT_CONFIRMPD, confirmpd, 64);
	oldpd[63] = '\0';
	newpd[63] = '\0';
	confirmpd[63] = '\0';
	int iOldLen = 0, iNewLen = 0, iConfirmLen = 0;
	iOldLen = strlen(oldpd);
	iNewLen = strlen(newpd);
	iConfirmLen = strlen(confirmpd);
	if(iOldLen<4 || iNewLen<4 || iConfirmLen<4)
	{
		MessageBox(_T("密码过短，请重新输入"), _T("密码设置"), MB_OK);
		return;
	}
	if(16<iOldLen || 16<iNewLen || 16<iConfirmLen)
	{
		MessageBox(_T("密码过长，请重新输入"), _T("密码设置"), MB_OK);
		return;
	}
	CSmsSenderDlg *pMainDlg = (CSmsSenderDlg *)AfxGetApp()->m_pMainWnd;
	if(pMainDlg)
	{
		CString strMd5Pd = pMainDlg->m_sPasswd;
		if(strMd5Pd.IsEmpty())
		{
			MessageBox(_T("原有密码丢失，不能进行密码设置!"), _T("密码设置"), MB_OK);
			return;
		}
		CString strOldPd = CMD5Checksum::GetMD5((BYTE*)oldpd, iOldLen);
		if(strOldPd != strMd5Pd)
		{
			MessageBox(_T("密码有误，请重新输入!"), _T("密码设置"), MB_OK);
			return;
		}
		if(strcmp(newpd, confirmpd))
		{
			MessageBox(_T("密码输入不一致，请重新输入!"), _T("密码设置"), MB_OK);
			return;
		}

		CString strNewPd = CMD5Checksum::GetMD5((BYTE*)newpd, iNewLen);
		pMainDlg->m_sPasswd.Format(_T("%s"), newpd);
	}
	CDialog::OnOK();
}
