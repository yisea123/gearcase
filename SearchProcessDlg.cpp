// SearchProcessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "smssender.h"
#include "SearchProcessDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchProcessDlg dialog


CSearchProcessDlg::CSearchProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchProcessDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSearchProcessDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pStopThreadEvent = NULL;
}


void CSearchProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchProcessDlg)
	DDX_Control(pDX, IDC_PROGRESS_SEARCH, m_process);
	DDX_Control(pDX, IDC_ANIMATE_SEARCH, m_searchAnimate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSearchProcessDlg, CDialog)
	//{{AFX_MSG_MAP(CSearchProcessDlg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(CM_PROCESS_SET_POS,OnProcessSetPos)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchProcessDlg message handlers

BOOL CSearchProcessDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	this->m_process.SetRange(0,100);
	this->m_process.SetPos(0);
	this->m_searchAnimate.Open(IDR_SERCH_AVI);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSearchProcessDlg::OnProcessSetPos(WPARAM wParam,LPARAM lParam)
{
	this->m_process.SetPos(wParam);
}

void CSearchProcessDlg::OnButtonCancel() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add extra cleanup here
	if(this->m_pStopThreadEvent == NULL)
		CDialog::OnCancel();
	else this->m_pStopThreadEvent->SetEvent();
}

BOOL CSearchProcessDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_F4)
	{
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CSearchProcessDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
}
