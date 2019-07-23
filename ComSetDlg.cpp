// ComSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "smssender.h"
#include "ComSetDlg.h"
#include "SearchProcessDlg.h"
#include "SerialPort.h"
#include "SmsSenderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEvent	g_evStopSearchPort;
//extern CEvent	g_evSmsNext;
extern int		g_iTimeout;
/////////////////////////////////////////////////////////////////////////////
// CComSetDlg dialog


CComSetDlg::CComSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CComSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CComSetDlg)
		// NOTE: the ClassWizard will add member initialization here
	m_nSerialPortNum = 1;
	m_nBaudRate = CBR_19200;
	m_nStartBits = 1;
	m_nDataBits = 8;
	m_nVerifyBits = EVENPARITY;
	m_nStopBits = ONESTOPBIT;
	m_bAutoRelation = FALSE;
	m_nTimeOut = DEFAULT_TIMEOUT;
	//}}AFX_DATA_INIT
}


void CComSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComSetDlg)
	DDX_Control(pDX, IDC_BN_SEARCH_PORT, m_ctrlBtnST_search);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CComSetDlg, CDialog)
	//{{AFX_MSG_MAP(CComSetDlg)
	ON_BN_CLICKED(IDC_BN_SEARCH_PORT, OnBnSearchPort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComSetDlg message handlers

UINT FuncSearchPort(LPVOID lParam)
{
	int i = 0, j = 0;
	CSerialPort Port;
	Sleep(100);
	CDialog	*pDlg = (CDialog*)lParam;
	CComSetDlg *pComDlg = (CComSetDlg *)pDlg->GetParent();
	//CSmsSenderDlg *pMainDlg = (CSmsSenderDlg *)AfxGetApp()->m_pMainWnd;
	int	avgPos = 100/4;
	for(i=1; i<4; i++)
	{
		::PostMessage(pDlg->GetSafeHwnd(), CM_PROCESS_SET_POS, i*avgPos, 0);
		BOOL bRight = FALSE;
		UINT nBaudRate = 19200;
		CString sBandRate;
		pComDlg->GetDlgItem(IDC_EDIT_BAUDRATE)->GetWindowText(sBandRate);
		if(!sBandRate.IsEmpty())
			nBaudRate = atoi(sBandRate);
		if(nBaudRate == 0)
			nBaudRate = 19200;
		//BOOL bPort = pMainDlg->m_Port.InitPort(AfxGetApp()->m_pMainWnd, i, nBaudRate);
		BOOL bPort = Port.InitPort(pComDlg, i, nBaudRate);
		if(bPort)
		{
			//pMainDlg->m_Port.StartMonitoring();
			//Port.StartMonitoring();
			bRight = TRUE;
		}
		//pMainDlg->m_Port.CloseSerialPort();
		Port.CloseSerialPort();
		if(bRight)
		{
			pComDlg->m_nSerialPortNum = i;
			pComDlg->m_nBaudRate = Port.GetDCB().BaudRate;
			pComDlg->m_nStartBits = SERIALPORT_STARTBITS;
			pComDlg->m_nDataBits = Port.GetDCB().ByteSize;
			pComDlg->m_nVerifyBits = Port.GetDCB().Parity;
			pComDlg->m_nStopBits = Port.GetDCB().StopBits;
			CString strTemp;
			strTemp.Format(_T("找到串口:%d"), i);
			AfxMessageBox(strTemp);
			::SendMessage(pDlg->GetSafeHwnd(), CM_PROCESS_SET_POS, 100, 0);
			::SendMessage(pDlg->GetSafeHwnd(), WM_CLOSE, 0, 0);
			return 1;
		}
	}
	::SendMessage(pDlg->GetSafeHwnd(),CM_PROCESS_SET_POS,100,0);
	AfxMessageBox(_T("没有找到串口,请手动查找!"));
	::SendMessage(pDlg->GetSafeHwnd(),WM_CLOSE,0,0);
	return 1;
}

void CComSetDlg::OnBnSearchPort() 
{
	// TODO: Add your control notification handler code here
	CSearchProcessDlg dlg;
	g_evStopSearchPort.ResetEvent();
	dlg.m_pStopThreadEvent = &g_evStopSearchPort;
	AfxBeginThread(FuncSearchPort, &dlg);
	dlg.DoModal();
	SetCtrlValue();
}

void CComSetDlg::SetCtrlValue()
{
	CString s;
	CComboBox *pBox = NULL;
	pBox = (CComboBox *)GetDlgItem(IDC_COMBO_NUM);
	pBox->SetCurSel(m_nSerialPortNum-1);
	s.Format(_T("%d"), m_nBaudRate);
	SetDlgItemText(IDC_EDIT_BAUDRATE, s);
	pBox = (CComboBox *)GetDlgItem(IDC_COMBO_BAUDRATE);
	if(m_nBaudRate == 9600)
		pBox->SetCurSel(0);
	else
		pBox->SetCurSel(1);
	pBox = (CComboBox *)GetDlgItem(IDC_COMBO_STARTBITS);
	pBox->SetCurSel(m_nStartBits-1);
	pBox = (CComboBox *)GetDlgItem(IDC_COMBO_DATABITS);
	pBox->SetCurSel(m_nDataBits-4);
	pBox = (CComboBox *)GetDlgItem(IDC_COMBO_VERIFYBITS);
	pBox->SetCurSel(m_nVerifyBits);
	pBox = (CComboBox *)GetDlgItem(IDC_COMBO_STOPBITS);
	pBox->SetCurSel(m_nStopBits);
	if(m_bAutoRelation)
	{
		CButton *pBn = (CButton *)GetDlgItem(IDC_CHECK_AUTORELATION);
		if(pBn) pBn->SetCheck(TRUE);
	}
	s.Format(_T("%d"), m_nTimeOut);
	SetDlgItemText(IDC_EDIT_TIMEOUT, s);
}

BOOL CComSetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ctrlBtnST_search.SetActiveFgColor(RGB(255, 0, 0));
	SetCtrlValue();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CComSetDlg::OnOK() 
{
	// TODO: Add extra validation here
	CSmsSenderDlg *pMainDlg = (CSmsSenderDlg *)AfxGetApp()->m_pMainWnd;
	if(pMainDlg)
	{
		CComboBox *pBox = NULL;
		pBox = (CComboBox *)GetDlgItem(IDC_COMBO_NUM);
		m_nSerialPortNum = pBox->GetCurSel() + 1;
		pBox = (CComboBox *)GetDlgItem(IDC_COMBO_BAUDRATE);
		if(pBox->GetCurSel() == 0)
			m_nBaudRate = 9600;
		else
			m_nBaudRate = 19200;
		pBox = (CComboBox *)GetDlgItem(IDC_COMBO_STARTBITS);
		m_nStartBits = pBox->GetCurSel() + 1;
		pBox = (CComboBox *)GetDlgItem(IDC_COMBO_DATABITS);
		m_nDataBits = pBox->GetCurSel() + 4;
		pBox = (CComboBox *)GetDlgItem(IDC_COMBO_VERIFYBITS);
		m_nVerifyBits = pBox->GetCurSel();
		pBox = (CComboBox *)GetDlgItem(IDC_COMBO_STOPBITS);
		m_nStopBits = pBox->GetCurSel();
		CButton *pBn = (CButton *)GetDlgItem(IDC_CHECK_AUTORELATION);
		m_bAutoRelation = pBn->GetCheck();
		CString sTimeOut;
		GetDlgItemText(IDC_EDIT_TIMEOUT, sTimeOut);
		m_nTimeOut = atoi(sTimeOut);
		if(m_nTimeOut<MIN_TIMEOUT) m_nTimeOut = MIN_TIMEOUT;
		if(m_nTimeOut>MAX_TIMEOUT) m_nTimeOut = MAX_TIMEOUT;

		pMainDlg->m_nSerialPortNum = m_nSerialPortNum;
		pMainDlg->m_nBaudRate = m_nBaudRate;
		pMainDlg->m_nStartBits = m_nStartBits;
		pMainDlg->m_nDataBits = m_nDataBits;
		pMainDlg->m_nVerifyBits = m_nVerifyBits;
		pMainDlg->m_nStopBits =m_nStopBits;
		pMainDlg->m_bAutoRelation = m_bAutoRelation;
		pMainDlg->m_nTimeOut = m_nTimeOut;

		CString sLog;
		sLog.Format(_T("[串口设置]m_nSerialPortNum=%d m_nBaudRate=%d m_nStartBits=%d m_nDataBits=%d m_nVerifyBits=%d m_nStopBits=%d m_bAutoRelation=%d m_nTimeOut=%d"),
			m_nSerialPortNum, m_nBaudRate, m_nStartBits, m_nDataBits, m_nVerifyBits, m_nStopBits, m_bAutoRelation, m_nTimeOut);
		pMainDlg->P_ShowLog(sLog);
	}
	CDialog::OnOK();
}
