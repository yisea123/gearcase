// SmsSender.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SmsSender.h"
#include "SmsSenderDlg.h"
#include "XString.h"
#include "ListCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define UNIQUE_NAME "{06B48739-1599-43e1-8E4F-81375D658BA2}"
#define REG_COMPANY_KEY "ZHAOLH"

/////////////////////////////////////////////////////////////////////////////
// CSmsSenderApp

BEGIN_MESSAGE_MAP(CSmsSenderApp, CWinApp)
	//{{AFX_MSG_MAP(CSmsSenderApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmsSenderApp construction

CSmsSenderApp::CSmsSenderApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSmsSenderApp object

CSmsSenderApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSmsSenderApp initialization

BOOL CSmsSenderApp::InitInstance()
{
	m_hOneInstance=CreateMutex(NULL,FALSE,UNIQUE_NAME);

	//InitializeCriticalSection(&XString::m_csLock);
//	InitializeCriticalSection(&CListCtrlEx::m_csLock);

	if(m_hOneInstance==NULL||ERROR_ALREADY_EXISTS==::GetLastError())
	{
		HWND hwnd = ::FindWindow(NULL, "GearCase");
		if(hwnd)
		{
			::ShowWindow(hwnd, SW_RESTORE);
			::SetForegroundWindow(hwnd);
		}
		return FALSE;
	}


	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	SetRegistryKey(_T(REG_COMPANY_KEY));
	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	CSmsSenderDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CSmsSenderApp::ExitInstance() 
{
	try
	{
		CloseHandle(m_hOneInstance);
		//g_dbLog.Close();
	}
	catch(...)
	{
	}
	
//	DeleteCriticalSection(&CListCtrlEx::m_csLock);
	//DeleteCriticalSection(&XString::m_csLock);

	return CWinApp::ExitInstance();
}
