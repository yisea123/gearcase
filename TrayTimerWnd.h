#if !defined(AFX_TRAYTIMERWND_H__9B301B40_805E_11D5_B614_00B0D07AF514__INCLUDED_)
#define AFX_TRAYTIMERWND_H__9B301B40_805E_11D5_B614_00B0D07AF514__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrayTimerWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrayTimerWnd frame

class CTrayNotifyIcon;


class CTrayTimerWnd : public CFrameWnd
{
	DECLARE_DYNCREATE(CTrayTimerWnd)
	
// Attributes
public:

// Operations
public:
	int m_nCurrentIconIndex;
	UINT m_nTimerID;
	DWORD m_dwDelay;
	int m_nNumIcons;
	HICON* m_phIcons;
	CTrayNotifyIcon* m_pTrayIcon;

public:
	CTrayTimerWnd() {}
	CTrayTimerWnd(CTrayNotifyIcon* pTrayIcon,HICON* phIcons, int nNumIcons,DWORD dwDelay);
	HICON GetCurrentIcon() const { return m_phIcons[m_nCurrentIconIndex]; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrayTimerWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTrayTimerWnd();

	// Generated message map functions
	//{{AFX_MSG(CTrayTimerWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAYTIMERWND_H__9B301B40_805E_11D5_B614_00B0D07AF514__INCLUDED_)
