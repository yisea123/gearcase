#if !defined(AFX_ALARMSETDLG_H__9F16F03F_76B3_479B_9260_D18F78A41E88__INCLUDED_)
#define AFX_ALARMSETDLG_H__9F16F03F_76B3_479B_9260_D18F78A41E88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AlarmSetDlg.h : header file
//

const float DEFAULT_ALARM_TEMPERATURE = 50.00; //50.00
const float MIN_ALARM_TEMPERATURE = 0.00;
const float MAX_ALARM_TEMPERATURE = 100.00;

const float DEFAULT_ALARM_VIBRATION = 10.00; //10.00
const float MIN_ALARM_VIBRATION = 0.00;
const float MAX_ALARM_VIBRATION = 20.00;

/////////////////////////////////////////////////////////////////////////////
// CAlarmSetDlg dialog

class CAlarmSetDlg : public CDialog
{
// Construction
public:
	CAlarmSetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAlarmSetDlg)
	enum { IDD = IDD_ALARMSET_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	BOOL m_bIsAlarmTemperature;
	BOOL m_bIsAlarmVibration;
	float m_fAlarmTemperature;
	float m_fAlarmVibration;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlarmSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAlarmSetDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARMSETDLG_H__9F16F03F_76B3_479B_9260_D18F78A41E88__INCLUDED_)
