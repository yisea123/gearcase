// SmsSender.h : main header file for the SMSSENDER application
//

#if !defined(AFX_SMSSENDER_H__763E32C7_A9CC_49F1_B2B0_4F0801E77456__INCLUDED_)
#define AFX_SMSSENDER_H__763E32C7_A9CC_49F1_B2B0_4F0801E77456__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "ado.h"
/////////////////////////////////////////////////////////////////////////////
// CSmsSenderApp:
// See SmsSender.cpp for the implementation of this class
//
//static CADODatabase	g_dbLog;
class CSmsSenderApp : public CWinApp
{
public:
	CSmsSenderApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmsSenderApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSmsSenderApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	HANDLE m_hOneInstance;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMSSENDER_H__763E32C7_A9CC_49F1_B2B0_4F0801E77456__INCLUDED_)
