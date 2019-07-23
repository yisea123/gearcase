// SmsSenderDlg.h : header file
//

#if !defined(AFX_SMSSENDERDLG_H__F392537A_727F_447D_937C_A368691ED9D8__INCLUDED_)
#define AFX_SMSSENDERDLG_H__F392537A_727F_447D_937C_A368691ED9D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <afxcoll.h>
#include "TrayNotifyIcon.h"	// Added by ClassView
#include "ToolBar_Add_Word.h"	// Added by ClassView
#include "BtnST.h"
#include "ListCtrlEx.h"
#include "SerialPort.h"
#include "ModbusQueue.h"
#include "LineChartCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CSmsSenderDlg dialog

#define COMMAND_COUNT 500

typedef struct tagGearCaseInfo //�ܺ�����������(61������)
{
	//0x01 ת�ٱ�MS5000D
	float fRev;

	//0x02 �񶯲���3ͨ�����Ա�MD52U
	float fVibration[3];

	//0x03 �¶�Ѳ����MD52U-8
	float fTemperature[8];

	//ת��=9550*���Ƶ��/ת��
	float fTorque;

	//0x04 PLC FX2N48MT-D
	WORD wSM0; //����״̬�Ĵ��� 0x01 485���ƣ���������ƣ� 0x02 �����ƣ�PU���ƣ� 0x03 �ⲿ����(������ť����)
	WORD wSM1; //����״̬�Ĵ��� 0x00 ������� 0x01 �������

	//0x05 ��Ƶ������Ƶ�ʺ��������2ͨ���������Ա�MB70U
	float fFrequency;
	float fOutputCurrent;

	//0x06 �ۺϵ�����EL2020
	WORD wUa; //���ѹUa
	WORD wUca; //�ߵ�ѹUca
	WORD wIa; //A�����
	WORD wReserveda; //����a
	WORD wPa; //A���й�����
	WORD wPfa; //A�๦������
	WORD wQa; //A���޹�����
	WORD wSa; //A�����ڹ���

	WORD wUb; //���ѹUb
	WORD wUab; //�ߵ�ѹUab
	WORD wIb; //B�����
	WORD wReservedb; //����b
	WORD wPb; //B���й�����
	WORD wPfb; //B�๦������
	WORD wQb; //B���޹�����
	WORD wSb; //B�����ڹ���

	WORD wUc; //���ѹUc
	WORD wUbc; //�ߵ�ѹUbc
	WORD wIc; //C�����
	WORD wReservedc; //����c
	WORD wPc; //C���й�����
	WORD wPfc; //C�๦������
	WORD wQc; //C���޹�����
	WORD wSc; //C�����ڹ���

	WORD wI0; //�������
	WORD wUav; //����ƽ�����ѹ
	WORD wIav; //����ƽ�������
	WORD wF; //Ƶ��

	//0x08 ��Ƶ��FR-A740-22K
	WORD wStateOrCtrl; //״̬/��������  ����0x0002��ʾ��ת���� ����0x0004��ʾ��ת���� ����0x0100��ʾ���ֹ��� ������ʾ����
	WORD wModeStateOrSet; //ģʽ״̬/�趨 0x0000�ⲿ���� 0x0004�����(RS485)���� ����PU����
	WORD wFRam; //����Ƶ��RAMֵ
	WORD wOutputF; //���Ƶ��
	WORD wOutputI; //�������
	WORD wOutputU; //�����ѹ
	WORD wFSetValue; //Ƶ���趨ֵ
	WORD wSpeed; //�����ٶ�
	WORD wOverILoadRate; //������������
	WORD wOutputIMax; //���������ֵ
	WORD wOutputUMax; //�����ѹ��ֵ
	WORD wInputP; //���빦��
	WORD wOutputP; //�������
	WORD wTotalRunTime; //�ۼ�ͨ��ʱ��
	WORD wRealRunTime; //ʵ������ʱ��
	WORD wTotalQ; //�ۼ��õ��� kwh
}GearCaseInfo;

class CSmsSenderDlg : public CDialog
{
// Construction
public:
	CSmsSenderDlg(CWnd* pParent = NULL);
	void OnOK();
	void P_ShowLog(CString z);
	static CString GetExePath();

	DWORD m_dwStart;
	DWORD m_dwAutoDelay;
	BOOL m_bAutoStartIM;
	CString m_strAllRecvData;
	CSerialPort m_Port;
	CModbusQueue m_mqReadQueue;
	CModbusQueue m_mqWriteQueue;
	CModbusQueue m_mqRecvQueue;

	int m_nSerialPortNum;
	int m_nBaudRate;
	int m_nStartBits;
	int m_nDataBits;
	int m_nVerifyBits;
	int m_nStopBits;
	BOOL m_bAutoRelation;
	BOOL m_bRelationPort;
	int m_nTimeOut;

	int m_nReadUsableNum;
	BOOL m_bUsable;
	
	CString m_sUserName;
	CString m_sUserNum;
	CString m_sProductType;
	CString m_sProductNum;
	CString m_sProducer;
	CString m_sTestName;
	CString m_sTestTime;
	CString m_sPasswd;

	CString m_sUserDir;
	CString m_sDataStorageDir;
	CString m_sLogDir;
	CString m_sLogPath;
	BOOL m_bStorage;
	
	WORD m_wSpeed[7];
	int m_nInterval[7];
	WORD m_wRunSpeed;
	WORD m_wLastSpeed; //����ֹͣ����ʱ���ת��
	
	BOOL m_bIsAlarmTemperature;
	BOOL m_bIsAlarmVibration;
	float m_fAlarmTemperature;
	float m_fAlarmVibration;
	float m_fSpeedDownRate; //���ٱ�

	BOOL m_bRev;
	BOOL m_bVibration[3];
	BOOL m_bTemperature[8];
	BOOL m_bTorque;
	COLORREF m_clrRev;
	COLORREF m_clrVibration[3];
	COLORREF m_clrTemperature[8];
	COLORREF m_clrTorque;

	
	CWinThread *m_pModbusRequestThread;
	HANDLE m_hModbusRequestExitEvent;
	HANDLE m_hModbusRequestNextEvent;
	HANDLE m_hModbusRequestEventArray[2];

	CWinThread *m_pModbusRecvThread;
	HANDLE m_hModbusRecvExitEvent;
	HANDLE m_hModbusRecvNextEvent;
	HANDLE m_hModbusRecvEventArray[2];

	ModbusPacket m_mpPacket;
	CString m_sMbInfo;
	int m_nTransducerStatus;

	GearCaseInfo m_GCInfo; //�ܺ�����������
// Dialog Data
	//{{AFX_DATA(CSmsSenderDlg)
	enum { IDD = IDD_SMSSENDER_DIALOG };
	CLineChartCtrl m_stcDraw;
	CListCtrlEx	m_l1;
	CToolBar_Add_Word m_wndToolBar;
	CTrayNotifyIcon m_TrayIcon;
	CStatusBar m_wndStatusBar;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmsSenderDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	HICON m_hIcons[2];
	HACCEL m_hAccel;

	// Generated message map functions
	//{{AFX_MSG(CSmsSenderDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnButtonHide();
	afx_msg void OnButtonShow();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LONG OnCommunication(UINT, LONG);
	afx_msg LONG OnCTSDetected(UINT, LONG);
	afx_msg void OnAppAbout();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMenuAlarmset();
	afx_msg void OnMenuSpeedset();
	afx_msg void OnMenuRelationCom();
	afx_msg void OnMenuDataStorage();
	afx_msg void OnMenuCurveColor();
	afx_msg void OnMenuComset();
	afx_msg void OnMenuUserset();
	afx_msg void OnMenuApplyRs485ctrl();
	afx_msg void OnMenuReleaseRs485ctrl();
	afx_msg void OnMenuRollStartIm();
	afx_msg void OnMenuReverseStartIm();
	afx_msg void OnMenuAutoStartIm();
	afx_msg void OnMenuStopIm();
	afx_msg LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCommRecieveData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowPacket(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRecvMbInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCalcTorque(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRefreshGearCaseInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnVibrationAlarm(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUsable(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTemperatureAlarm(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMenuDataQuery();
	afx_msg void OnMenuStartRecord();
	afx_msg void OnMenuStopRecord();
	afx_msg void OnMenuPasswdset();
	afx_msg void OnMenuSpeeddownset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CString _Get_Current_Time(int t_iFlag=0);
	void _WINDOW_SHOW();
	void _WINDOW_HIDE();
	void WriteFileLog(CString log);
	void WriteStorageFile(CString strName, CLineEx *line);
	void SaveUserToFile();

	void _RegSave();
	void _RegLoad();
	void _INIT();

	BOOL FolderExist(CString strPath);
	BOOL FileExist(CString strPath);
	DWORD GetFileSize(CString strPath);
	DWORD GetDirSize(CString strPath);
	BOOL DeleteDirectory(CString strPath);
public:
	void LoadConfig(CString strFileName);
	void SaveConfig(CString strFileName);
	void SetStatusBarText();
	void SetListCtrlText();
	void CleanListCtrlText();
	void InitDrawCurve();

	void RollStart(); //��ת����
	void RollStartWithSpeed(int nIndex=0); //�Զ���nIndex��ת����
	void RollStop(); //��תֹͣ
	void ReverseStart();  //��ת����
	void ReverseStartWithSpeed(int nIndex=0); //�Զ���nIndex��ת����
	void ReverseStop(); //��תֹͣ

	void ReadRev(); //��ת�ٱ�MS5000D
	void ReadVibration(); //�񶯲���3ͨ�����Ա�MD52U
	void ReadTemperature(); //���¶�Ѳ����MD52U-8
	void ReadPlcSM(); //��PLCϵͳģʽ
	void ReadFrequencyAndOutputCurrent(); //����Ƶ������Ƶ�ʺ��������2ͨ���������Ա�MB70U
	void ReadElectricPower(); //���ۺϵ�����EL20200
	void ReadUa(); //���ۺϵ��������ѹUa
	void ReadIa(); //���ۺϵ�����A�����Ia
	void ReadUb(); //���ۺϵ��������ѹUb
	void ReadIb(); //���ۺϵ�����B�����Ib
	void ReadUc(); //���ۺϵ��������ѹUc
	void ReadIc(); //���ۺϵ�����C�����Ic
	void ReadF(); //���ۺϵ�����Ƶ��F
	
	void ReadTransducer(); //����Ƶ����Ϣ
	void ReadControlStatus(); //����Ƶ��ģʽ����״̬
	void ReadOutputFIU(); //����Ƶ�����Ƶ�ʡ���������������ѹ����
	void ReadRunTimeFS(); //����Ƶ�����Ƶ���趨ֵ�������ٶ�����
	void Read485F(); //����Ƶ��485Ƶ���趨ֵ
	void ReadLoadRateMaxUIAndPower(); //����Ƶ�������������ʡ����������ֵ�������ѹ��ֵ�����빦�ʡ������������
	void ReadOutputP(); //����Ƶ���������
	void ReadTotalRunTime(); //����Ƶ���ۼ�ͨ��ʱ������
	void ReadRealRunTime(); //����Ƶ��ʵ������ʱ������
	void ReadTotalQ(); //����Ƶ���ۼ��õ�������
	void ReadMultiSpeeds(); //����Ƶ����ζ���
	void ReadUsable(); //����Ƶ������Ƿ����

	void WriteRunSpeed(); //д��Ƶ�����������ٶ�

	void WriteApplyRs485Start(); //����485���Ƴɹ�֮��д��Ƶ����ת/��ת����
	void WriteReleaseRs485Stop(); //�ͷ�485���Ƴɹ�֮��д��Ƶ����ת/��תֹͣ


	//void RefreshRev(); //ˢ��ת�ٱ�MS5000D
	//void RefreshVibration(); //ˢ���񶯲���3ͨ�����Ա�MD52U
	//void RefreshTemperature(); //ˢ���¶�Ѳ����MD52U-8
	//void RefreshPlcSm(); //ˢ��PLCϵͳģʽ
	//void RefreshFrequencyAndOutputCurrent(); //ˢ�±�Ƶ������Ƶ�ʺ��������2ͨ���������Ա�MB70U
	//void RefreshElectricPower(); //ˢ���ۺϵ�����EL2020
	//void RefreshTransducer(); //ˢ�±�Ƶ����Ϣ

	unsigned short FormCrc16(unsigned char * startaddress, unsigned char bytecount); //crc16У����
	CString Data2String(byte *ucData, int nSize);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMSSENDERDLG_H__F392537A_727F_447D_937C_A368691ED9D8__INCLUDED_)
