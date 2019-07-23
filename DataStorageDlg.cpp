// DataStorageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "smssender.h"
#include "DataStorageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataStorageDlg dialog

CString CDataStorageDlg::s_strDefPath = _T("");

int CALLBACK DirDlgCallback(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)   
{
	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		::SendMessage(hWnd, BFFM_SETSELECTION, 1, (long)CDataStorageDlg::s_strDefPath.GetBuffer(MAX_PATH));
		CDataStorageDlg::s_strDefPath.ReleaseBuffer();
		break;
	default:
		break;
	}
	return 0;
}

CDataStorageDlg::CDataStorageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDataStorageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDataStorageDlg)
		// NOTE: the ClassWizard will add member initialization here
	m_sPath = _T("");
	//}}AFX_DATA_INIT
}


void CDataStorageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataStorageDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDataStorageDlg, CDialog)
	//{{AFX_MSG_MAP(CDataStorageDlg)
	ON_BN_CLICKED(IDC_BN_SELECT, OnBnSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataStorageDlg message handlers

BOOL CDataStorageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CDataStorageDlg::s_strDefPath = m_sPath;
	SetDlgItemText(IDC_EDIT_DIR, m_sPath);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDataStorageDlg::OnBnSelect() 
{
	// TODO: Add your control notification handler code here
	char szDir[MAX_PATH] = {0};
	BROWSEINFO bi;
	ITEMIDLIST *pidl;
	bi.hwndOwner = m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDir;
	bi.lpszTitle = _T("请选择存储数据文件的目录");
	bi.ulFlags = BIF_STATUSTEXT | BIF_RETURNONLYFSDIRS;
	bi.lpfn = DirDlgCallback;
	bi.lParam = 0;
	bi.iImage = 0;
	pidl = SHBrowseForFolder(&bi);
	if(pidl == NULL) return;
	if(!SHGetPathFromIDList(pidl, szDir)) return;
	SetDlgItemText(IDC_EDIT_DIR, szDir);
}

void CDataStorageDlg::OnOK() 
{
	// TODO: Add extra validation here
	GetDlgItemText(IDC_EDIT_DIR, m_sPath);
	CDataStorageDlg::s_strDefPath = m_sPath;
	
	CDialog::OnOK();
}
