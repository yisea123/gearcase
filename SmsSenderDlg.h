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

typedef struct tagGearCaseInfo //跑合箱所有数据(61个数据)
{
	//0x01 转速表MS5000D
	float fRev;

	//0x02 振动测量3通道数显表MD52U
	float fVibration[3];

	//0x03 温度巡检仪MD52U-8
	float fTemperature[8];

	//转矩=9550*输出频率/转速
	float fTorque;

	//0x04 PLC FX2N48MT-D
	WORD wSM0; //控制状态寄存器 0x01 485控制（计算机控制） 0x02 面板控制（PU控制） 0x03 外部控制(电器柜按钮控制)
	WORD wSM1; //运行状态寄存器 0x00 电机空闲 0x01 电机运行

	//0x05 变频器运行频率和输出电流2通道监视数显表MB70U
	float fFrequency;
	float fOutputCurrent;

	//0x06 综合电量表EL2020
	WORD wUa; //相电压Ua
	WORD wUca; //线电压Uca
	WORD wIa; //A相电流
	WORD wReserveda; //保留a
	WORD wPa; //A相有功功率
	WORD wPfa; //A相功率因数
	WORD wQa; //A相无功功率
	WORD wSa; //A相视在功率

	WORD wUb; //相电压Ub
	WORD wUab; //线电压Uab
	WORD wIb; //B相电流
	WORD wReservedb; //保留b
	WORD wPb; //B相有功功率
	WORD wPfb; //B相功率因数
	WORD wQb; //B相无功功率
	WORD wSb; //B相视在功率

	WORD wUc; //相电压Uc
	WORD wUbc; //线电压Ubc
	WORD wIc; //C相电流
	WORD wReservedc; //保留c
	WORD wPc; //C相有功功率
	WORD wPfc; //C相功率因数
	WORD wQc; //C相无功功率
	WORD wSc; //C相视在功率

	WORD wI0; //零序电流
	WORD wUav; //三相平均相电压
	WORD wIav; //三相平均相电流
	WORD wF; //频率

	//0x08 变频器FR-A740-22K
	WORD wStateOrCtrl; //状态/控制命令  相与0x0002表示正转运行 相与0x0004表示反转运行 相与0x0100表示出现故障 其它表示空闲
	WORD wModeStateOrSet; //模式状态/设定 0x0000外部控制 0x0004计算机(RS485)控制 其它PU控制
	WORD wFRam; //运行频率RAM值
	WORD wOutputF; //输出频率
	WORD wOutputI; //输出电流
	WORD wOutputU; //输出电压
	WORD wFSetValue; //频率设定值
	WORD wSpeed; //运行速度
	WORD wOverILoadRate; //过电流负载率
	WORD wOutputIMax; //输出电流峰值
	WORD wOutputUMax; //输出电压峰值
	WORD wInputP; //输入功率
	WORD wOutputP; //输出功率
	WORD wTotalRunTime; //累计通电时间
	WORD wRealRunTime; //实际运行时间
	WORD wTotalQ; //累计用电量 kwh
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
	WORD m_wLastSpeed; //发送停止命令时电机转速
	
	BOOL m_bIsAlarmTemperature;
	BOOL m_bIsAlarmVibration;
	float m_fAlarmTemperature;
	float m_fAlarmVibration;
	float m_fSpeedDownRate; //减速比

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

	GearCaseInfo m_GCInfo; //跑合箱所有数据
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

	void RollStart(); //正转启动
	void RollStartWithSpeed(int nIndex=0); //以段速nIndex正转运行
	void RollStop(); //正转停止
	void ReverseStart();  //反转启动
	void ReverseStartWithSpeed(int nIndex=0); //以段速nIndex反转运行
	void ReverseStop(); //反转停止

	void ReadRev(); //读转速表MS5000D
	void ReadVibration(); //振动测量3通道数显表MD52U
	void ReadTemperature(); //读温度巡检仪MD52U-8
	void ReadPlcSM(); //读PLC系统模式
	void ReadFrequencyAndOutputCurrent(); //读变频器运行频率和输出电流2通道监视数显表MB70U
	void ReadElectricPower(); //读综合电量表EL20200
	void ReadUa(); //读综合电量表相电压Ua
	void ReadIa(); //读综合电量表A相电流Ia
	void ReadUb(); //读综合电量表相电压Ub
	void ReadIb(); //读综合电量表B相电流Ib
	void ReadUc(); //读综合电量表相电压Uc
	void ReadIc(); //读综合电量表C相电流Ic
	void ReadF(); //读综合电量表频率F
	
	void ReadTransducer(); //读变频器信息
	void ReadControlStatus(); //读变频器模式控制状态
	void ReadOutputFIU(); //读变频器输出频率、输出电流、输出电压数据
	void ReadRunTimeFS(); //读变频器面板频率设定值、运行速度数据
	void Read485F(); //读变频器485频率设定值
	void ReadLoadRateMaxUIAndPower(); //读变频器过电流负载率、输出电流峰值、输出电压峰值、输入功率、输出功率数据
	void ReadOutputP(); //读变频器输出功率
	void ReadTotalRunTime(); //读变频器累计通电时间数据
	void ReadRealRunTime(); //读变频器实际运行时间数据
	void ReadTotalQ(); //读变频器累计用电量数据
	void ReadMultiSpeeds(); //读变频器多段段速
	void ReadUsable(); //读变频器软件是否可用

	void WriteRunSpeed(); //写变频器设置运行速度

	void WriteApplyRs485Start(); //申请485控制成功之后，写变频器正转/反转启动
	void WriteReleaseRs485Stop(); //释放485控制成功之后，写变频器正转/反转停止


	//void RefreshRev(); //刷新转速表MS5000D
	//void RefreshVibration(); //刷新振动测量3通道数显表MD52U
	//void RefreshTemperature(); //刷新温度巡检仪MD52U-8
	//void RefreshPlcSm(); //刷新PLC系统模式
	//void RefreshFrequencyAndOutputCurrent(); //刷新变频器运行频率和输出电流2通道监视数显表MB70U
	//void RefreshElectricPower(); //刷新综合电量表EL2020
	//void RefreshTransducer(); //刷新变频器信息

	unsigned short FormCrc16(unsigned char * startaddress, unsigned char bytecount); //crc16校验码
	CString Data2String(byte *ucData, int nSize);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMSSENDERDLG_H__F392537A_727F_447D_937C_A368691ED9D8__INCLUDED_)
