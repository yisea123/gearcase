// SmsSenderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SmsSender.h"
#include "SmsSenderDlg.h"
#include "XString.h"
#include "AboutDlg.h"

//#include "searchprocessdlg.h"

#include "ComSetDlg.h"
#include "UserSetDlg.h"
#include "AlarmSetDlg.h"
#include "SpeedSetDlg.h"
#include "CurveColorDlg.h"
#include "DataQueryDlg.h"
#include "DataStorageDlg.h"
#include "PasswordSetDlg.h"
#include "SpeedDownSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	CONFIG_FILE_NAME	"GearCase.ini"

int g_iTimeout;

//CEvent g_evSmsNext;

void AG_Log(CString z);
CSmsSenderDlg* AG_GetMainDlg();

static TOOLTEXT tooltext[] =
{
	ID_MENU_RELATION_COM,IDS_BN_RELATION_COM,
	ID_MENU_APPLY_RS485CTRL,IDS_BN_APPLY_RS485CTRL,
	ID_MENU_RELEASE_RS485CTRL,IDS_BN_RELEASE_RS485CTRL,
	ID_MENU_ROLL_START_IM,IDS_BN_ROLL_START_IM,
	ID_MENU_REVERSE_START_IM,IDS_BN_REVERSE_START_IM,
	ID_MENU_AUTO_START_IM,IDS_BN_AUTO_START_IM,
	ID_MENU_STOP_IM,IDS_BN_STOP_IM,
	ID_APP_EXIT,IDS_APP_EXIT,
	ID_BUTTON_HIDE,IDS_BUTTON_HIDE,
	ID_MENU_STARTRECORD,IDS_BN_STARTRECORD,
	ID_MENU_STOPRECORD,IDS_BN_STOPRECORD
};
#define TOOLTEXT_NUM (sizeof(tooltext)/sizeof(TOOLTEXT))

static UINT indicators[] =
{
	ID_SEPARATOR, //status line indicator
	ID_SEPARATOR, //UserName
	ID_SEPARATOR, //UserNum
	ID_SEPARATOR, //ProductType
	ID_SEPARATOR, //ProductNum
	ID_SEPARATOR, //Producer
	ID_SEPARATOR, //TestName
	ID_SEPARATOR  //RelationCom
};
#define STATUSBAR_NUM (sizeof(indicators)/sizeof(UINT))

#define WM_TRAYNOTIFY	WM_USER+11
#define WM_SHOWPACKET	WM_USER+12
#define WM_RECVMBINFO	WM_USER+13
#define WM_CALCTORQUE	WM_USER+14
#define WM_VIBRATION_ALARM	WM_USER+15
#define WM_TEMPERATURE_ALARM	WM_USER+16
#define WM_REFRESH_REV WM_USER+17
#define WM_REFRESH_VIBRATION WM_USER+18
#define WM_REFRESH_TEMPERATURE WM_USER+19
#define WM_REFRESH_PLCSM WM_USER+20
#define WM_REFRESH_FREQUENCY_CURRENT WM_USER+21
#define WM_REFRESH_ELECTRICPOWER WM_USER+22
#define WM_REFRESH_TRANSDUCER WM_USER+23
#define WM_REFRESH_GEARCASE_INFO WM_USER+24
#define WM_USABLE	WM_USER+100

/////////////////////////////////////////////////////////////////////////////
// CSmsSenderDlg dialog

UINT FuncModbusRequest(LPVOID lParam)
{
	BOOL bRead = TRUE;
	DWORD dwDelay = 0, dwFront = 0;
	CSmsSenderDlg *pDlg = (CSmsSenderDlg*)lParam;
	while(1)
	{
		DWORD dwRet = 0;
		dwRet = WaitForMultipleObjects(2, pDlg->m_hModbusRequestEventArray, FALSE, g_iTimeout*1000);
		if((dwRet - WAIT_OBJECT_0) == WAIT_OBJECT_0) break;
		bRead = TRUE;
		ModbusPacket *mpWrite = pDlg->m_mqWriteQueue.Top();
		if(mpWrite)
		{
			dwDelay = GetTickCount() - mpWrite->dwStart;
			if(dwDelay < mpWrite->dwDelay)
			{
				bRead = TRUE;
			}
			else
			{
				mpWrite = pDlg->m_mqWriteQueue.Pop();
				if(mpWrite)
				{
					pDlg->m_Port.WriteToPort((char*)mpWrite->data, mpWrite->nSize);
					memcpy(&(pDlg->m_mpPacket), mpWrite, sizeof(ModbusPacket));
					ModbusPacket *mpShow = new ModbusPacket;
					memcpy(mpShow, mpWrite, sizeof(ModbusPacket));
					pDlg->PostMessage(WM_SHOWPACKET, (WPARAM)mpShow);
					delete mpWrite;
					bRead = FALSE;
				}
			}
		}
		if(bRead)
		{
			ModbusPacket *mpRead = pDlg->m_mqReadQueue.Top();
			if(mpRead)
			{
				dwDelay = GetTickCount() - mpRead->dwStart;
				//if(dwDelay < mpRead->dwDelay) continue;
				mpRead = pDlg->m_mqReadQueue.Pop();
				if(mpRead)
				{
					pDlg->m_Port.WriteToPort((char*)mpRead->data, mpRead->nSize);
					memcpy(&(pDlg->m_mpPacket), mpRead, sizeof(ModbusPacket));
					ModbusPacket *mpShow = new ModbusPacket;
					memcpy(mpShow, mpRead, sizeof(ModbusPacket));
					pDlg->PostMessage(WM_SHOWPACKET, (WPARAM)mpShow);
					delete mpRead;
				}
			}
		}
	}
	return 0;
}

UINT FuncModbusRecv(LPVOID lParam)
{
	CSmsSenderDlg *pDlg = (CSmsSenderDlg*)lParam;
	while(1)
	{
		DWORD dwRet = WaitForMultipleObjects(2, pDlg->m_hModbusRecvEventArray, FALSE, INFINITE);
		if((dwRet-WAIT_OBJECT_0) == WAIT_OBJECT_0) break;
		if(dwRet == WAIT_TIMEOUT)
		{
			SetEvent(pDlg->m_hModbusRequestNextEvent);
			continue;
		}
		ModbusPacket *mpRecv = pDlg->m_mqRecvQueue.Pop();
		if(mpRecv)
		{
			int i = 0, j = 0;
			CString sSend, sRecv, sTmp, sErr;
			byte *ucData = mpRecv->data;
			byte *ucRecv = mpRecv->ucRecvBuf;
			sSend = _T("��������: ") + pDlg->Data2String(ucData, mpRecv->nSize);
			sRecv = _T("��������: ") + pDlg->Data2String(ucRecv, mpRecv->nRecvSize);

			WORD wBegin = 0, wEnd = 0;
			if(ucRecv[0] == 0x01) //ת�ٱ�MS5000D
			{
				float fRev = 0.00f;
				for(j=0; j<4; j++)
					((byte *)&fRev)[j] = ucRecv[6-j];
				if(pDlg->m_GCInfo.wStateOrCtrl & 0x0004)
					fRev = -fRev;
				pDlg->m_GCInfo.fRev = fRev;
				fRev = fRev * pDlg->m_fSpeedDownRate; //��ת��ֵ=ת��ֵ*���ٱ�
				pDlg->m_stcDraw.m_lineRev.AddPoint(time(0), fRev);
				wBegin = 0;
				wEnd = 0;
			}
			if(ucRecv[0] == 0x02) //�񶯲���3ͨ�����Ա�MD52U
			{
				float fVibration[3] = {0.00f};
				memset(fVibration, 0, sizeof(fVibration));
				for(i=0; i<3; i++)
				{
					for(j=0; j<4; j++)
						((byte *)&fVibration[i])[j] = ucRecv[4*i+6-j];
					if(fabs(fVibration[i]) > 500.00)
						fVibration[i] = 0.00;
					pDlg->m_GCInfo.fVibration[i] = fVibration[i];
					pDlg->m_stcDraw.m_lineVibration[i].AddPoint(time(0), fVibration[i]);
					if(fVibration[i] > pDlg->m_fAlarmVibration)
					{
						WPARAM wparam = MAKEWPARAM(i, 0);
						pDlg->PostMessage(WM_VIBRATION_ALARM, wparam, (LPARAM)fVibration[i]);
					}
				}
				wBegin = 2;
				wEnd = 4;
			}
			if(ucRecv[0] == 0x03) //�¶�Ѳ����MD52U-8
			{
				float fTemperature[8] = {0.00f};
				memset(fTemperature, 0, sizeof(fTemperature));
				for(j=0; j<4; j++)
					((byte *)&fTemperature[7])[j] = ucRecv[4*7+6-j];
				if(fabs(fTemperature[7]) > 500.00)
					fTemperature[7] = 0.00;
				pDlg->m_GCInfo.fTemperature[7] = fTemperature[7];
				pDlg->m_stcDraw.m_lineTemperature[7].AddPoint(time(0), fTemperature[7]);

				for(i=0; i<7; i++)
				{
					for(j=0; j<4; j++)
						((byte *)&fTemperature[i])[j] = ucRecv[4*i+6-j];
					if(fabs(fTemperature[i]) > 500.00)
						fTemperature[i] = 0.00;
					else
						fTemperature[i] = fTemperature[i] - fTemperature[7]; //����[i]=����ֵ[i]-����[7]
					pDlg->m_GCInfo.fTemperature[i] = fTemperature[i];
					pDlg->m_stcDraw.m_lineTemperature[i].AddPoint(time(0), fTemperature[i]);
					if(fTemperature[i] > pDlg->m_fAlarmTemperature)
					{
						WPARAM wparam = MAKEWPARAM(i, 0);
						pDlg->PostMessage(WM_TEMPERATURE_ALARM, wparam, (LPARAM)fTemperature[i]);
					}
				}
				wBegin = 6;
				wEnd = 13;
			}
			if(ucRecv[0] == 0x04) //PLC FX2N48MT-D
			{
				if(ucData[1] ==0x03) //������
				{
					WORD wSM = 0;
					//((byte *)&wSM)[0] = ucRecv[3];
					//((byte *)&wSM)[1] = ucRecv[4];
					//pDlg->m_GCInfo.wSM0 = wSM;
					//((byte *)&wSM)[0] = ucRecv[5];
					//((byte *)&wSM)[1] = ucRecv[6];
					//pDlg->m_GCInfo.wSM1 = wSM;

					pDlg->m_GCInfo.wSM0 = ucRecv[3]; //0x01 485���ƣ���������ƣ� 0x02 �����ƣ�PU���ƣ� 0x03 �ⲿ����(������ť����)
					pDlg->m_GCInfo.wSM1 = ucRecv[4]; //0x00 ������� 0x01 �������
				}
			}
			if(ucRecv[0] == 0x05) //��Ƶ������Ƶ�ʺ��������2ͨ���������Ա�MB70U
			{
				float fFrequency = 0.00f, fCurrent= 0.00f;
				for(j=0; j<4; j++)
				{
					((byte *)&fFrequency)[j] = ucRecv[6-j];
					((byte *)&fCurrent)[j] = ucRecv[10-j];
				}
				pDlg->m_GCInfo.fFrequency = fFrequency;
				pDlg->m_GCInfo.fOutputCurrent = fCurrent;
				
				wBegin = 15;
				wEnd = 16;
			}
			//wBegin = 17 //��Ƶ�����ת��
			//wEnd = 17

			if(ucRecv[0] == 0x06) //�ۺϵ�����EL2020
			{
				/*
				GearCaseInfo *pGci = &(pDlg->m_GCInfo);
				WORD wValue = 0;
				if(ucData[2] == 0x00 && ucData[3] == 0x00)
				{
					WORD wValue = 0;
					((byte *)&wValue)[0] = ucRecv[4];
					((byte *)&wValue)[1] = ucRecv[3];
					pGci->wUa = wValue; //���ѹUa
				}
				if(ucData[2] == 0x00 && ucData[3] == 0x04)
				{
					WORD wValue = 0;
					((byte *)&wValue)[0] = ucRecv[4];
					((byte *)&wValue)[1] = ucRecv[3];
					pGci->wIa = wValue; //A�����Ia
				}
				if(ucData[2] == 0x00 && ucData[3] == 0x10)
				{
					WORD wValue = 0;
					((byte *)&wValue)[0] = ucRecv[4];
					((byte *)&wValue)[1] = ucRecv[3];
					pGci->wUb = wValue; //���ѹUb
				}
				if(ucData[2] == 0x00 && ucData[3] == 0x14)
				{
					WORD wValue = 0;
					((byte *)&wValue)[0] = ucRecv[4];
					((byte *)&wValue)[1] = ucRecv[3];
					pGci->wIb = wValue; //B�����Ib
				}
				if(ucData[2] == 0x00 && ucData[3] == 0x20)
				{
					WORD wValue = 0;
					((byte *)&wValue)[0] = ucRecv[4];
					((byte *)&wValue)[1] = ucRecv[3];
					pGci->wUc = wValue; //���ѹUc
				}
				if(ucData[2] == 0x00 && ucData[3] == 0x24)
				{
					WORD wValue = 0;
					((byte *)&wValue)[0] = ucRecv[4];
					((byte *)&wValue)[1] = ucRecv[3];
					pGci->wIc = wValue; //C�����Ic
				}
				if(ucData[2] == 0x00 && ucData[3] == 0x36)
				{
					WORD wValue = 0;
					((byte *)&wValue)[0] = ucRecv[4];
					((byte *)&wValue)[1] = ucRecv[3];
					pGci->wF = wValue; //Ƶ��F (��Ƶ)
				}
				*/
				GearCaseInfo *pGci = &(pDlg->m_GCInfo);
				WORD wValue = 0;
				((byte *)&wValue)[0] = ucRecv[4];
				((byte *)&wValue)[1] = ucRecv[3];
				pGci->wUa = wValue;
				((byte *)&wValue)[0] = ucRecv[8];
				((byte *)&wValue)[1] = ucRecv[7];
				pGci->wIa = wValue;
				((byte *)&wValue)[0] = ucRecv[20];
				((byte *)&wValue)[1] = ucRecv[19];
				pGci->wUb = wValue;
				((byte *)&wValue)[0] = ucRecv[24];
				((byte *)&wValue)[1] = ucRecv[23];
				pGci->wIb = wValue;
				((byte *)&wValue)[0] = ucRecv[36];
				((byte *)&wValue)[1] = ucRecv[35];
				pGci->wUc = wValue;
				((byte *)&wValue)[0] = ucRecv[40];
				((byte *)&wValue)[1] = ucRecv[39];
				pGci->wIc = wValue;
				((byte *)&wValue)[0] = ucRecv[58];
				((byte *)&wValue)[1] = ucRecv[57];
				pGci->wF = wValue;
				wBegin = 24;
				wEnd = 30;
			}
			if(ucRecv[0] == 0x08) //��Ƶ��FR-A740-22K
			{
				if(ucData[1] == 0x03)
				{
					if(ucData[2] == 0x00 && ucData[3] == 0x08)
					{
						WORD wValue = 0;
						((byte *)&wValue)[0] = ucRecv[4];
						((byte *)&wValue)[1] = ucRecv[3];
						pDlg->m_GCInfo.wStateOrCtrl = wValue; //״̬/����

						((byte *)&wValue)[0] = ucRecv[6];
						((byte *)&wValue)[1] = ucRecv[5];
						pDlg->m_GCInfo.wModeStateOrSet = wValue; //ģʽ״̬/�趨 0x0000��ʾ�ⲿ���ƣ�0x0004��ʾRS485���ƣ� ������ʾPU(���)����
						wBegin = 40;
						wEnd = 41;
					}
					if(ucData[2] == 0x00 && ucData[3] == 0x0D)
					{
						WORD wValue = 0;
						((byte *)&wValue)[0] = ucRecv[4];
						((byte *)&wValue)[1] = ucRecv[3];
						pDlg->m_wRunSpeed = wValue; //485Ƶ���趨ֵ(ת���趨ֵ)
						wBegin = 19;
						wEnd = 19;
					}
					if(ucData[2] == 0x00 && ucData[3] == 0xC8)
					{
						WORD wValue = 0;
						((byte *)&wValue)[0] = ucRecv[4];
						((byte *)&wValue)[1] = ucRecv[3];
						pDlg->m_GCInfo.wOutputF = wValue; //���Ƶ��

						((byte *)&wValue)[0] = ucRecv[6];
						((byte *)&wValue)[1] = ucRecv[5];
						pDlg->m_GCInfo.wOutputI = wValue; //�������

						((byte *)&wValue)[0] = ucRecv[8];
						((byte *)&wValue)[1] = ucRecv[7];
						pDlg->m_GCInfo.wOutputU = wValue; //�����ѹ
					}
					if(ucData[2] == 0x00 && ucData[3] == 0xCC)
					{
						WORD wValue = 0;
						((byte *)&wValue)[0] = ucRecv[4];
						((byte *)&wValue)[1] = ucRecv[3];
						pDlg->m_GCInfo.wFSetValue = wValue; //���Ƶ���趨ֵ(������ת���趨ֵ)
						wBegin = 18;
						wEnd = 18;

						((byte *)&wValue)[0] = ucRecv[6];
						((byte *)&wValue)[1] = ucRecv[5];
						pDlg->m_GCInfo.wSpeed = wValue; //�����ٶ�
					}
					if(ucData[2] == 0x00 && ucData[3] == 0xD1)
					{
						WORD wValue = 0;
						((byte *)&wValue)[0] = ucRecv[4];
						((byte *)&wValue)[1] = ucRecv[3];
						pDlg->m_GCInfo.wOverILoadRate = wValue; //������������

						((byte *)&wValue)[0] = ucRecv[6];
						((byte *)&wValue)[1] = ucRecv[5];
						pDlg->m_GCInfo.wOutputIMax = wValue; //���������ֵ

						((byte *)&wValue)[0] = ucRecv[8];
						((byte *)&wValue)[1] = ucRecv[7];
						pDlg->m_GCInfo.wOutputUMax = wValue*10; //�����ѹ��ֵ

						((byte *)&wValue)[0] = ucRecv[10];
						((byte *)&wValue)[1] = ucRecv[9];
						pDlg->m_GCInfo.wInputP = wValue; //���빦��

						((byte *)&wValue)[0] = ucRecv[12];
						((byte *)&wValue)[1] = ucRecv[11];
						pDlg->m_GCInfo.wOutputP = wValue; //�������
						wBegin = 20;
						wEnd = 23;

						//����ת��
						float fTorque = 0.00;
						pDlg->m_GCInfo.fTorque = fTorque;
						float fRev = pDlg->m_GCInfo.fRev;
						if(fabs(fRev/pDlg->m_fSpeedDownRate) >= 30.00f)
						{
							fTorque = 9550*pDlg->m_GCInfo.wOutputP/fRev;
							fTorque = fTorque/100;
							pDlg->m_GCInfo.fTorque = fTorque;
							//LPARAM lparam = MAKELPARAM(wBegin, wEnd);
							//pDlg->PostMessage(WM_REFRESH_GEARCASE_INFO, (WPARAM)ucRecv[0], lparam);
						}
						pDlg->m_stcDraw.m_lineTorque.AddPoint(time(0), fTorque);
						//wBegin = 17;
						//wEnd = 17;
					}
					if(ucData[2] == 0x00 && ucData[3] == 0xDB)
					{
						WORD wValue = 0;
						((byte *)&wValue)[0] = ucRecv[4];
						((byte *)&wValue)[1] = ucRecv[3];
						pDlg->m_GCInfo.wTotalRunTime = wValue; //�ۼ�ͨ��ʱ��
					}
					if(ucData[2] == 0x00 && ucData[3] == 0xDE)
					{
						WORD wValue = 0;
						((byte *)&wValue)[0] = ucRecv[4];
						((byte *)&wValue)[1] = ucRecv[3];
						pDlg->m_GCInfo.wRealRunTime = wValue; //ʵ������ʱ��
					}
					if(ucData[2] == 0x00 && ucData[3] == 0xE0)
					{
						WORD wValue = 0;
						((byte *)&wValue)[0] = ucRecv[4];
						((byte *)&wValue)[1] = ucRecv[3];
						pDlg->m_GCInfo.wTotalQ = wValue; //�ۼ��õ���
					}
					if(ucData[2] == 0x03 && ucData[3] == 0xEB) //����1-3
					{
						WORD wValue = 0;
						((byte *)&wValue)[0] = ucRecv[4];
						((byte *)&wValue)[1] = ucRecv[3];
						pDlg->m_wSpeed[2] = wValue; //����3

						((byte *)&wValue)[0] = ucRecv[6];
						((byte *)&wValue)[1] = ucRecv[5];
						pDlg->m_wSpeed[1] = wValue; //����2

						((byte *)&wValue)[0] = ucRecv[8];
						((byte *)&wValue)[1] = ucRecv[7];
						pDlg->m_wSpeed[0] = wValue; //����1
						sTmp.Format(_T("����1=%d ����2=%d ����3=%d"), 
							pDlg->m_wSpeed[0], pDlg->m_wSpeed[1], pDlg->m_wSpeed[2]);
						sRecv = sRecv + sTmp;
						wBegin = 32;
						wEnd = 34;
					}
					if(ucData[2] == 0x03 && ucData[3] == 0xFF) //����4-7
					{
						WORD wValue = 0;
						((byte *)&wValue)[0] = ucRecv[4];
						((byte *)&wValue)[1] = ucRecv[3];
						pDlg->m_wSpeed[3] = wValue; //����4
						
						((byte *)&wValue)[0] = ucRecv[6];
						((byte *)&wValue)[1] = ucRecv[5];
						pDlg->m_wSpeed[4] = wValue; //����5
						
						((byte *)&wValue)[0] = ucRecv[8];
						((byte *)&wValue)[1] = ucRecv[7];
						pDlg->m_wSpeed[5] = wValue; //����6

						((byte *)&wValue)[0] = ucRecv[10];
						((byte *)&wValue)[1] = ucRecv[9];
						pDlg->m_wSpeed[6] = wValue; //����7
						sTmp.Format(_T("����4=%d ����5=%d ����6=%d ����7=%d"), 
							pDlg->m_wSpeed[3], pDlg->m_wSpeed[4], pDlg->m_wSpeed[5], pDlg->m_wSpeed[6]);
						sRecv = sRecv + sTmp;
						wBegin = 35;
						wEnd = 38;
					}
					if(ucData[2] == 0x07 && ucData[3] == 0x5F)
					{
						WORD wLow = 0, wHigh = 0;
						((byte *)&wLow)[0] = ucRecv[4];
						((byte *)&wLow)[1] = ucRecv[3];

						((byte *)&wHigh)[0] = ucRecv[6];
						((byte *)&wHigh)[1] = ucRecv[5];
						pDlg->PostMessage(WM_USABLE, (WPARAM)(wHigh-wLow));
					}
				}
			}
			if(ucData[1] == 0x03)
			{
				LPARAM lparam = MAKELPARAM(wBegin, wEnd);
				pDlg->PostMessage(WM_REFRESH_GEARCASE_INFO, (WPARAM)ucRecv[0], lparam);
			}
			sTmp = _T("�����������");
			//pDlg->m_sMbInfo = sSend + sRecv + sTmp;
			char *pMbInfo = new char[1024];
			memset(pMbInfo, 0, 1024);
			sprintf(pMbInfo, _T("%s %s %s"), sSend, sRecv, sTmp);
			pDlg->PostMessage(WM_RECVMBINFO, (WPARAM)pMbInfo);
			delete mpRecv;
			SetEvent(pDlg->m_hModbusRequestNextEvent);
		}
	}
	return 0;
}

CSmsSenderDlg::CSmsSenderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSmsSenderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSmsSenderDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIcons[0]=AfxGetApp()->LoadIcon(IDI_ICON_1);
	m_hIcons[1]=AfxGetApp()->LoadIcon(IDI_ICON_2);

	m_bRelationPort = FALSE;
	m_nSerialPortNum = 1;
	m_nBaudRate = CBR_19200;
	m_nStartBits = 1;
	m_nDataBits = 8;
	m_nVerifyBits = NOPARITY;
	m_nStopBits = ONESTOPBIT;
	m_bAutoRelation = FALSE;
	m_nTimeOut = 1;
	m_nReadUsableNum = 0;
	m_bUsable = FALSE;
	
	m_sUserName = _T("");
	m_sUserNum = _T("");
	m_sProductType = _T("");
	m_sProductNum = _T("");
	m_sProducer = _T("");
	m_sTestName = _T("");
	m_sTestTime = _T("");
	m_sPasswd = _T("");

	m_sUserDir = _T("");
	m_sDataStorageDir = _T("");
	m_sLogDir = _T("");
	m_sLogPath = _T("");
	m_bStorage = FALSE;

	int i = 0;
	for(i=0; i<7; i++)
	{
		m_wSpeed[i] = 0;
		m_nInterval[i] = 0;
	}
	m_wRunSpeed = 0;
	m_dwStart = 0;
	m_dwAutoDelay = 0;
	m_bAutoStartIM = FALSE;

	m_bIsAlarmTemperature = FALSE;
	m_bIsAlarmVibration = FALSE;
	m_fAlarmTemperature = 0.00;
	m_fAlarmVibration = 0.00;
	m_fSpeedDownRate = 1.0000;

	m_bRev = FALSE;
	m_clrRev = RGB(0xDB, 0x93, 0x70);
	for(i=0; i<3; i++)
	{
		m_bVibration[i] = FALSE;
	}
	m_clrVibration[0] = RGB(0xDC, 0x14, 0x3C);
	m_clrVibration[1] = RGB(0x41, 0x69, 0xE1);
	m_clrVibration[2] = RGB(0x20, 0xB2, 0xAA);
	for(i=0; i<8; i++)
	{
		m_bTemperature[i] = FALSE;
	}
	m_clrTemperature[0] = RGB(0xEE, 0x82, 0xEE);
	m_clrTemperature[1] = RGB(0xFF, 0x00, 0xFF);
	m_clrTemperature[2] = RGB(0x8B, 0x00, 0x8B);
	m_clrTemperature[3] = RGB(0x4B, 0x00, 0x82);
	m_clrTemperature[4] = RGB(0x78, 0x6B, 0xEE);
	m_clrTemperature[5] = RGB(0x48, 0x3D, 0x8B);
	m_clrTemperature[6] = RGB(0x00, 0x00, 0xFF);
	m_clrTemperature[7] = RGB(0x19, 0x19, 0x70);
	m_bTorque = FALSE;
	m_clrTorque = RGB(0xFF, 0xA5, 0x00);

	m_pModbusRequestThread = NULL;
	m_hModbusRequestExitEvent = NULL;
	m_hModbusRequestNextEvent = NULL;
	m_hModbusRequestEventArray[0] = NULL;
	m_hModbusRequestEventArray[1] = NULL;

	m_pModbusRecvThread = NULL;
	m_hModbusRecvExitEvent = NULL;
	m_hModbusRecvNextEvent = NULL;
	m_hModbusRecvEventArray[0] = NULL;
	m_hModbusRecvEventArray[1] = NULL;

	m_nTransducerStatus = 0; //0��ʾ�ⲿ���ƣ�1��ʾRS485����
}

void CSmsSenderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSmsSenderDlg)
	DDX_Control(pDX, IDC_STATIC_CURVE, m_stcDraw);
	DDX_Control(pDX, IDC_LIST_CACHE, m_l1);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSmsSenderDlg, CDialog)
	//{{AFX_MSG_MAP(CSmsSenderDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_COMMAND(ID_BUTTON_HIDE, OnButtonHide)
	ON_COMMAND(ID_BUTTON_SHOW, OnButtonShow)
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_COMM_RXCHAR, OnCommunication)
	ON_MESSAGE(WM_COMM_CTS_DETECTED, OnCTSDetected)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_WM_TIMER()
	ON_COMMAND(ID_MENU_ALARMSET, OnMenuAlarmset)
	ON_COMMAND(ID_MENU_SPEEDSET, OnMenuSpeedset)
	ON_COMMAND(ID_MENU_RELATION_COM, OnMenuRelationCom)
	ON_COMMAND(ID_MENU_DATA_STORAGE, OnMenuDataStorage)
	ON_COMMAND(ID_MENU_CURVE_COLOR, OnMenuCurveColor)
	ON_COMMAND(ID_MENU_COMSET, OnMenuComset)
	ON_COMMAND(ID_MENU_USERSET, OnMenuUserset)
	ON_COMMAND(ID_MENU_APPLY_RS485CTRL, OnMenuApplyRs485ctrl)
	ON_COMMAND(ID_MENU_RELEASE_RS485CTRL, OnMenuReleaseRs485ctrl)
	ON_COMMAND(ID_MENU_ROLL_START_IM, OnMenuRollStartIm)
	ON_COMMAND(ID_MENU_REVERSE_START_IM, OnMenuReverseStartIm)
	ON_COMMAND(ID_MENU_AUTO_START_IM, OnMenuAutoStartIm)
	ON_COMMAND(ID_MENU_STOP_IM, OnMenuStopIm)
	ON_MESSAGE(WM_TRAYNOTIFY, OnTrayNotification)
	ON_MESSAGE(WM_COMM_RECIEVE_DATA_DETECTED, OnCommRecieveData)
	ON_MESSAGE(WM_SHOWPACKET, OnShowPacket)
	ON_MESSAGE(WM_RECVMBINFO, OnRecvMbInfo)
	ON_MESSAGE(WM_CALCTORQUE, OnCalcTorque)
	ON_MESSAGE(WM_REFRESH_GEARCASE_INFO, OnRefreshGearCaseInfo)
	ON_MESSAGE(WM_VIBRATION_ALARM, OnVibrationAlarm)
	ON_MESSAGE(WM_USABLE, OnUsable)
	ON_MESSAGE(WM_TEMPERATURE_ALARM, OnTemperatureAlarm)
	ON_COMMAND(ID_MENU_DATA_QUERY, OnMenuDataQuery)
	ON_COMMAND(ID_MENU_STARTRECORD, OnMenuStartRecord)
	ON_COMMAND(ID_MENU_STOPRECORD, OnMenuStopRecord)
	ON_COMMAND(ID_MENU_PASSWDSET, OnMenuPasswdset)
	ON_COMMAND(ID_MENU_SPEEDDOWNSET, OnMenuSpeeddownset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmsSenderDlg message handlers
void CSmsSenderDlg::SetStatusBarText()
{
	UINT nID = 0;
	CString sText(_T(""));
	CDC *pDC = GetDC();
	CSize sizeText;
	int iWidth = 0;
	sText.Format(_T("����Ա��%s"), m_sUserName);
	sizeText = pDC->GetTextExtent(sText);
	iWidth = sizeText.cx;
	m_wndStatusBar.SetPaneInfo(0, nID, SBPS_NORMAL, iWidth);
	m_wndStatusBar.SetPaneText(0, sText);
	
	sText.Format(_T("����Ա��ţ�%s"), m_sUserNum);
	sizeText = pDC->GetTextExtent(sText);
	iWidth = sizeText.cx;
	m_wndStatusBar.SetPaneInfo(1, nID, SBPS_NORMAL, iWidth);
	m_wndStatusBar.SetPaneText(1, sText);
	
	sText.Format(_T("��Ʒ�ͺţ�%s"), m_sProductType);
	sizeText = pDC->GetTextExtent(sText);
	iWidth = sizeText.cx;
	m_wndStatusBar.SetPaneInfo(2, nID, SBPS_NORMAL, iWidth);
	m_wndStatusBar.SetPaneText(2, sText);
	
	sText.Format(_T("��Ʒ��ţ�%s"), m_sProductNum);
	sizeText = pDC->GetTextExtent(sText);
	iWidth = sizeText.cx;
	m_wndStatusBar.SetPaneInfo(3, nID, SBPS_NORMAL, iWidth);
	m_wndStatusBar.SetPaneText(3, sText);
	
	sText.Format(_T("�������ң�%s"), m_sProducer);
	sizeText = pDC->GetTextExtent(sText);
	iWidth = sizeText.cx;
	m_wndStatusBar.SetPaneInfo(4, nID, SBPS_NORMAL, iWidth);
	m_wndStatusBar.SetPaneText(4, sText);
	
	sText.Format(_T("�������ƣ�%s"), m_sTestName);
	sizeText = pDC->GetTextExtent(sText);
	iWidth = sizeText.cx;
	m_wndStatusBar.SetPaneInfo(5, nID, SBPS_NORMAL, iWidth);
	m_wndStatusBar.SetPaneText(5, sText);

	if(m_bRelationPort)
		sText.Format(_T("�����ѹ���"));
	else
		sText.Format(_T("�����ѶϿ�"));
	sizeText = pDC->GetTextExtent(sText);
	iWidth = sizeText.cx;
	m_wndStatusBar.SetPaneInfo(6, nID, SBPS_NORMAL, iWidth);
	m_wndStatusBar.SetPaneText(6, sText);
}

void CSmsSenderDlg::SetListCtrlText()
{
	XString x;
	CString sParam, sValue, sUnit;
	int i = 0, inter = 0, fraction =0;
	//0x01 ת�ٱ�MS5000D
	sParam.Format(_T("��������ת��")); //ת�� (0)
	sValue.Format(_T("%.02f"), m_GCInfo.fRev);
	sUnit.Format(_T("r/min"));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);

	sParam.Format(_T("")); // (1)
	sValue.Format(_T(""));
	sUnit.Format(_T(""));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);

	//0x02 �񶯲���3ͨ�����Ա�MD52U
	for(i=0; i<3; i++) //(2-4)
	{
		sParam.Format(_T("��%d"), i+1);
		sValue.Format(_T("%.02f"), m_GCInfo.fVibration[i]);
		sUnit.Format(_T("mm/s"));
		x = Concat(sParam, sValue, sUnit);
		m_l1.AddItemFullRowXS(x);
	}

	sParam.Format(_T("")); // (5)
	sValue.Format(_T(""));
	sUnit.Format(_T(""));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);

	//0x03 �¶�Ѳ����MD52U-8
	for(i=0; i<7; i++) //(6-12)
	{
		sParam.Format(_T("����%d"), i+1);
		sValue.Format(_T("%.02f"), m_GCInfo.fTemperature[i]);
		sUnit.Format(_T("��"));
		x = Concat(sParam, sValue, sUnit);
		m_l1.AddItemFullRowXS(x);
	}
	sParam.Format(_T("����")); // (13)
	sValue.Format(_T("%.02f"), m_GCInfo.fTemperature[i]);
	sUnit.Format(_T("��"));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);

	sParam.Format(_T("")); // (14)
	sValue.Format(_T(""));
	sUnit.Format(_T(""));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);

	//0x05 ��Ƶ������Ƶ�ʺ��������2ͨ���������Ա�MB70U
	sParam.Format(_T("��Ƶ������Ƶ��")); //����Ƶ�� (15)
	sValue.Format(_T("%.02f"), m_GCInfo.fFrequency);
	sUnit.Format(_T("Hz"));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);

	sParam.Format(_T("��Ƶ���������")); // (16)
	sValue.Format(_T("%.02f"), m_GCInfo.fOutputCurrent);
	sUnit.Format(_T("A"));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);

	//ת�� (9550*�������/ת��)
	sParam.Format(_T("��Ƶ�����ת��")); //ת�� (17)
	sValue.Format(_T("%.02f"), m_GCInfo.fTorque);
	sUnit.Format(_T("N��m"));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);

	//0x08 ��Ƶ��FR-A740-22K
// 	inter = m_GCInfo.wOutputF/100;
// 	fraction = m_GCInfo.wOutputF%100;
// 	sParam.Format(_T("���Ƶ��"));
// 	sValue.Format(_T("%d.%02d"), inter, fraction);
// 	sUnit.Format(_T("Hz"));
// 	x = Concat(sParam, sValue, sUnit);
// 	m_l1.AddItemFullRowXS(x);
// 
// 	inter = m_GCInfo.wOutputI/100;
// 	fraction = m_GCInfo.wOutputI%100;
// 	sParam.Format(_T("�������"));
// 	sValue.Format(_T("%d.%02d"), inter, fraction);
// 	sUnit.Format(_T("A"));
// 	x = Concat(sParam, sValue, sUnit);
// 	m_l1.AddItemFullRowXS(x);

// 	inter = m_GCInfo.wOutputU/100;
// 	fraction = m_GCInfo.wOutputU%100;
// 	sParam.Format(_T("�����ѹ"));
// 	sValue.Format(_T("%d.%02d"), inter, fraction);
// 	sUnit.Format(_T("V"));
// 	x = Concat(sParam, sValue, sUnit);
// 	m_l1.AddItemFullRowXS(x);

	//inter = m_GCInfo.wFSetValue/100;
	//fraction = m_GCInfo.wFSetValue%100;
	sParam.Format(_T("��ǰ���ת���趨ֵ")); //���Ƶ���趨ֵ (18)
	sValue.Format(_T("%d"), m_GCInfo.wFSetValue);
	sUnit.Format(_T("rpm"));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);

	sParam.Format(_T("485���ת���趨ֵ")); //485Ƶ���趨ֵ (19)
	sValue.Format(_T("%d"), m_wRunSpeed);
	sUnit.Format(_T("rpm"));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);

// 	inter = m_GCInfo.wSpeed/100;
// 	fraction = m_GCInfo.wSpeed%100;
// 	sParam.Format(_T("�����ٶ�"));
// 	sValue.Format(_T("%d.%02d"), inter, fraction);
// 	sUnit.Format(_T("r/min"));
// 	x = Concat(sParam, sValue, sUnit);
// 	m_l1.AddItemFullRowXS(x);

// 	inter = m_GCInfo.wOverILoadRate/100;
// 	fraction = m_GCInfo.wOverILoadRate%100;
// 	sParam.Format(_T("������������"));
// 	sValue.Format(_T("%d.%02d"), inter, fraction);
// 	sUnit.Format(_T("%%"));
// 	x = Concat(sParam, sValue, sUnit);
// 	m_l1.AddItemFullRowXS(x);

	inter = m_GCInfo.wOutputIMax/100; // (20)
	fraction = m_GCInfo.wOutputIMax%100;
	sParam.Format(_T("���������ֵ"));
	sValue.Format(_T("%d.%02d"), inter, fraction);
	sUnit.Format(_T("I"));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);

	inter = m_GCInfo.wOutputUMax/100; // (21)
	fraction = m_GCInfo.wOutputUMax%100;
	sParam.Format(_T("�����ѹ��ֵ"));
	sValue.Format(_T("%d.%02d"), inter, fraction);
	sUnit.Format(_T("V"));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);

	inter = m_GCInfo.wInputP/100; // (22)
	fraction = m_GCInfo.wInputP%100;
	sParam.Format(_T("���빦��"));
	sValue.Format(_T("%d.%02d"), inter, fraction);
	sUnit.Format(_T("KW"));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);

	inter = m_GCInfo.wOutputP/100; //(23)
	fraction = m_GCInfo.wOutputP%100;
	sParam.Format(_T("�������"));
	sValue.Format(_T("%d.%02d"), inter, fraction);
	sUnit.Format(_T("KW"));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);

// 	inter = m_GCInfo.wTotalRunTime/100;
// 	fraction = m_GCInfo.wTotalRunTime%100;
// 	sParam.Format(_T("�ۼ�ͨ��ʱ��"));
// 	sValue.Format(_T("%d.%02d"), inter, fraction);
// 	sUnit.Format(_T("H"));
// 	x = Concat(sParam, sValue, sUnit);
// 	m_l1.AddItemFullRowXS(x);
// 
// 	inter = m_GCInfo.wRealRunTime/100;
// 	fraction = m_GCInfo.wRealRunTime%100;
// 	sParam.Format(_T("ʵ������ʱ��"));
// 	sValue.Format(_T("%d.%02d"), inter, fraction);
// 	sUnit.Format(_T("H"));
// 	x = Concat(sParam, sValue, sUnit);
// 	m_l1.AddItemFullRowXS(x);
// 
// 	inter = m_GCInfo.wTotalQ/100;
// 	fraction = m_GCInfo.wTotalQ%100;
// 	sParam.Format(_T("�ۼ��õ���"));
// 	sValue.Format(_T("%d.%02d"), inter, fraction);
// 	sUnit.Format(_T("KWH"));
// 	x = Concat(sParam, sValue, sUnit);
// 	m_l1.AddItemFullRowXS(x);

	//0x06 �ۺϵ�����EL2020
	inter = m_GCInfo.wUa/100; //(24)
	fraction = m_GCInfo.wUa%100;
	sParam.Format(_T("A���ѹUa"));
	sValue.Format(_T("%d.%02d"), inter, fraction);
	sUnit.Format(_T("V"));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);
	
	inter = m_GCInfo.wUb/100; //(25)
	fraction = m_GCInfo.wUb%100;
	sParam.Format(_T("B���ѹUb"));
	sValue.Format(_T("%d.%02d"), inter, fraction);
	sUnit.Format(_T("V"));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);
	
	inter = m_GCInfo.wUc/100; //(26)
	fraction = m_GCInfo.wUc%100;
	sParam.Format(_T("C���ѹUc"));
	sValue.Format(_T("%d.%02d"), inter, fraction);
	sUnit.Format(_T("V"));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);
	
	inter = m_GCInfo.wIa/100; //(27)
	fraction = m_GCInfo.wIa%100;
	sParam.Format(_T("A�����Ia"));
	sValue.Format(_T("%d.%02d"), inter, fraction);
	sUnit.Format(_T("A"));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);
	
	
	inter = m_GCInfo.wIb/100; //(28)
	fraction = m_GCInfo.wIb%100;
	sParam.Format(_T("B�����Ib"));
	sValue.Format(_T("%d.%02d"), inter, fraction);
	sUnit.Format(_T("A"));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);
	
	inter = m_GCInfo.wIc/100; //(29)
	fraction = m_GCInfo.wIc%100;
	sParam.Format(_T("C�����Ic"));
	sValue.Format(_T("%d.%02d"), inter, fraction);
	sUnit.Format(_T("A"));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);
	
	inter = m_GCInfo.wF/100; //(30)
	fraction = m_GCInfo.wF%100;
	sParam.Format(_T("��Ƶ")); //����Ƶ��
	sValue.Format(_T("%d.%02d"), inter, fraction);
	sUnit.Format(_T("Hz"));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);

	sParam.Format(_T("")); //(31)
	sValue.Format(_T(""));
	sUnit.Format(_T(""));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);

	//08 ��Ƶ������1--����7
	for(i=0; i<7; i++) //(32-38)
	{
		sParam.Format(_T("����%d"), i+1);
		sValue.Format(_T("%d"), 0);
		sUnit.Format(_T("rpm"));
		x = Concat(sParam, sValue, sUnit);
		m_l1.AddItemFullRowXS(x);
	}

	sParam.Format(_T("")); //(39)
	sValue.Format(_T(""));
	sUnit.Format(_T(""));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);

	//WORD wMss = m_GCInfo.wModeStateOrSet;
	sParam.Format(_T("ϵͳ״̬")); //ϵͳ״̬ (40)
	sValue.Format(_T("%s"), _T("PU����"));
	sUnit.Format(_T(""));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);

	//float fRev = m_GCInfo.fRev;
	sParam.Format(_T("����״̬")); //�������״̬ (41)
	sValue.Format(_T("%s"), _T("����"));
	sUnit.Format(_T(""));
	x = Concat(sParam, sValue, sUnit);
	m_l1.AddItemFullRowXS(x);
}

void CSmsSenderDlg::CleanListCtrlText()
{
	int iCnt = m_l1.GetItemCount();
	int i = 0;
	for(i=0; i<iCnt; i++)
	{
		CString sText = _T("0.00");
		if(i == 1 || i == 5 || i == 14 || i == 23 || i == 31) sText = _T("");
		if(i == 18) sText = _T("0");
		if(i == 32) sText = _T("δ֪");
		if(i == 33) sText = _T("δ֪");
		m_l1.SetItemText(i, 1, sText);
	}
}

void CSmsSenderDlg::InitDrawCurve()
{
	m_stcDraw.GetAxisY().m_dMinValue=-5000.00;
	m_stcDraw.GetAxisY().m_dMaxValue=5000.00;
	m_stcDraw.GetAxisX().m_dMaxValue=0.00;
	m_stcDraw.GetAxisX().m_dMaxValue=5000.00;

	int i = 0;
	CString strTitle;
	CLineEx *line = NULL;
	//Rev
	line = &(m_stcDraw.m_lineRev);
	line->m_bDraw = m_bRev;
	line->m_clrColor = m_clrRev;
	line->m_fMinValue = -5000.00;
	line->m_fMaxValue = 5000.00;
	strTitle.Format(_T("%s"), _T("ת��"));
	strcpy(line->m_szTitle, strTitle);
	//Vibration
	line = m_stcDraw.m_lineVibration;
	for(i=0; i<3; i++)
	{
		line[i].m_bDraw = m_bVibration[i];
		line[i].m_clrColor = m_clrVibration[i];
		line[i].m_fMinValue = 0.00;
		line[i].m_fMaxValue = 20.00;
		strTitle.Format(_T("%s%d"), _T("��"), i+1);
		strcpy(line[i].m_szTitle, strTitle);
	}
	//Temperature
	line = m_stcDraw.m_lineTemperature;
	for(i=0; i<7; i++)
	{
		line[i].m_bDraw = m_bTemperature[i];
		line[i].m_clrColor = m_clrTemperature[i];
		line[i].m_fMinValue = -10.00;
		line[i].m_fMaxValue = 130.00;
		strTitle.Format(_T("%s%d"), _T("����"), i+1);
		strcpy(line[i].m_szTitle, strTitle);
	}
	line[i].m_bDraw = m_bTemperature[i];
	line[i].m_clrColor = m_clrTemperature[i];
	line[i].m_fMinValue = -10.00;
	line[i].m_fMaxValue = 130.00;
	strTitle.Format(_T("%s"), _T("����"));
	strcpy(line[i].m_szTitle, strTitle);

	//Torque
	line = &(m_stcDraw.m_lineTorque);
	line->m_bDraw = m_bTorque;
	line->m_clrColor = m_clrTorque;
	long fValue = floor(802.50/(10*m_fSpeedDownRate))*10;
	line->m_fMinValue = -fValue; //-120.00
	line->m_fMaxValue = fValue;
	strTitle.Format(_T("%s"), _T("ת��"));
	strcpy(line->m_szTitle, strTitle);
	
	m_stcDraw.CalcLayout();
	//m_stcDraw.SetRate(200);
	m_stcDraw.Start();
}

BOOL CSmsSenderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	m_TrayIcon.Create(this, IDR_MENU_TRAY_ICON, _T("GearCase"), m_hIcons, 2, 1000, WM_TRAYNOTIFY);
	m_hAccel=::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MENU_MAIN));

	// TODO: Add extra initialization here
	_INIT();

	//Data storage and log path
	int i = 0;
	if(m_sUserDir.IsEmpty())
		m_sUserDir = GetExePath() + m_sUserName;
	if(!FolderExist(m_sUserDir))
		CreateDirectory(m_sUserDir, NULL);
	m_sDataStorageDir = m_sUserDir + _T("\\") + _Get_Current_Time(3);
	CreateDirectory(m_sDataStorageDir, NULL);
	m_sLogDir = GetExePath() + m_sUserName;
	if(!FolderExist(m_sLogDir))
		CreateDirectory(m_sLogDir, NULL);
	m_sLogPath = m_sLogDir + _T("\\") + _Get_Current_Time(3) + _T(".log");

	//�ܺ���61��������Ϣ
	memset(&m_GCInfo, 0, sizeof(GearCaseInfo));

	//delete the showing window's caption
	//LONG style = GetWindowLong(m_hWnd, GWL_STYLE);
	//style &= ~WS_CAPTION;
	//set the window's show sytle
	//SetWindowLong(m_hWnd, GWL_STYLE, style);
	//get the screen's pixel
	int screenX = GetSystemMetrics(SM_CXSCREEN);
	int screenY = GetSystemMetrics(SM_CYSCREEN);
	//set the window full screen
	SetWindowPos(NULL, 0, 0, screenX, screenY, SWP_NOZORDER);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD|WS_VISIBLE | CBRS_TOP| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBarEx(IDR_TOOLBAR_MAIN, (TOOLTEXT*)&tooltext, TOOLTEXT_NUM))	
	{
		return FALSE;
	}
	m_wndToolBar.ShowWindow(SW_SHOW);

	if (!m_wndStatusBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM) ||
		!m_wndStatusBar.SetIndicators(indicators, STATUSBAR_NUM))
	{
		return FALSE;
	}

	//m_stcDraw.ModifyStyle(0, SS_NOTIFY);
	
	UpdateData(FALSE);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	//full screen controls
	CRect rectToolBar, rectStatusBar, rectSplit;
	CRect rectl1Box, rectl1, rectStcDrawBox, rectStcDraw;
	m_wndToolBar.GetWindowRect(&rectToolBar);
	m_wndStatusBar.GetWindowRect(&rectStatusBar);
	ScreenToClient(&rectToolBar);
	ScreenToClient(&rectStatusBar);

	rectSplit.left = rectToolBar.left;
	rectSplit.right = rectToolBar.right;
	rectSplit.top = rectToolBar.bottom + 1;
	rectSplit.bottom = rectSplit.top + 1;
	CWnd *pWnd = NULL;
	pWnd = GetDlgItem(IDC_STC_SPLIT);
	pWnd->MoveWindow(&rectSplit);

	rectl1Box.left = rectSplit.left + 2;
	rectl1Box.right = rectl1Box.left + 350;
	rectl1Box.top = rectSplit.bottom + 7;
	rectl1Box.bottom = rectStatusBar.top;
	pWnd = GetDlgItem(IDC_STC_LISTBOX);
	pWnd->MoveWindow(&rectl1Box);

	rectl1.left = rectl1Box.left + 6;
	rectl1.right = rectl1Box.right - 6;
	rectl1.top = rectl1Box.top + 16;
	rectl1.bottom = rectl1Box.bottom - 6;
	m_l1.MoveWindow(&rectl1);

	rectStcDrawBox.left = rectl1Box.right + 10;
	rectStcDrawBox.right = rectSplit.right - 2;
	rectStcDrawBox.top = rectSplit.bottom + 7;
	rectStcDrawBox.bottom = rectStatusBar.top;
	pWnd = GetDlgItem(IDC_STC_DRAWBOX);
	pWnd->MoveWindow(&rectStcDrawBox);

	rectStcDraw.left = rectStcDrawBox.left + 6;
	rectStcDraw.right = rectStcDrawBox.right - 6;
	rectStcDraw.top = rectStcDrawBox.top + 16;
	rectStcDraw.bottom = rectStcDrawBox.bottom - 6;
	m_stcDraw.MoveWindow(&rectStcDraw);

	//modify list ctrl item height
	int cy = (rectl1.Height()-30)/42 - 1; //����42��������Ҫ��ʾ
	CImageList m_l;
	m_l.Create(1, cy, TRUE|ILC_COLOR32, 1, 0);
	m_l1.SetImageList(&m_l, LVSIL_SMALL);

	SetStatusBarText();
	SetListCtrlText();
	InitDrawCurve();

	g_iTimeout = m_nTimeOut>1 ? m_nTimeOut:1;

	m_hModbusRequestExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hModbusRequestNextEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hModbusRequestEventArray[0] = m_hModbusRequestExitEvent;
	m_hModbusRequestEventArray[1] = m_hModbusRequestNextEvent;
	m_pModbusRequestThread = AfxBeginThread(FuncModbusRequest, this);
	
	m_hModbusRecvExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hModbusRecvNextEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	m_hModbusRecvEventArray[0] = m_hModbusRecvExitEvent;
	m_hModbusRecvEventArray[1] = m_hModbusRecvNextEvent;
	m_pModbusRecvThread = AfxBeginThread(FuncModbusRecv, this);

	if(m_bAutoRelation)
		OnMenuRelationCom();

	SetTimer(1, 1000, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSmsSenderDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSmsSenderDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSmsSenderDlg::OnOK()
{
	
}

BOOL CSmsSenderDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(::TranslateAccelerator(GetSafeHwnd(), m_hAccel, pMsg))
		return true;

	if(pMsg->message==WM_KEYDOWN)
	{
		if((int)pMsg->wParam==VK_RETURN)	return TRUE;
		if((int)pMsg->wParam==VK_ESCAPE)
		{
			_WINDOW_HIDE();			
			return TRUE;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CSmsSenderDlg::_INIT()
{
	_RegLoad();

	m_l1.SetHighlightType(HIGHLIGHT_ALLCOLUMNS);
	CString zHead;
	zHead.Format("��������|����|��λ");
	m_l1.AddColumnXS(zHead, 3);
	m_l1.SetColumnWidth(0, 140);
	m_l1.SetColumnWidth(1, 100);
	m_l1.SetColumnWidth(2, 60);

	//CImageList m_l;
	//m_l.Create(1, 13, TRUE|ILC_COLOR32, 1, 0);
	//m_l1.SetImageList(&m_l, LVSIL_SMALL);
}

void CSmsSenderDlg::_RegLoad()
{
	this->LoadConfig(GetExePath() + CONFIG_FILE_NAME);
	g_iTimeout=(m_nTimeOut>1 ? m_nTimeOut:1);
	UpdateData(FALSE);
}

void CSmsSenderDlg::_RegSave()
{
	this->SaveConfig(GetExePath() + CONFIG_FILE_NAME);
}

BOOL CSmsSenderDlg::FolderExist(CString strPath)
{
	BOOL bRet = FALSE;
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(strPath, &wfd);
	if(hFind != INVALID_HANDLE_VALUE && (wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
		bRet = TRUE;
	FindClose(hFind);
	TRACE("bRet=%d\n", bRet);
	return bRet;
}

BOOL CSmsSenderDlg::FileExist(CString strPath)
{
	BOOL bRet = FALSE;
 	WIN32_FIND_DATA wfd;
 	HANDLE hFind = FindFirstFile(strPath, &wfd);
 	if(hFind != INVALID_HANDLE_VALUE)
 		bRet = TRUE;
 	FindClose(hFind);

	//CFileFind ff;
	//bRet = ff.FindFile(strPath);
	TRACE("bRet=%d\n", bRet);
	return bRet;
}

DWORD CSmsSenderDlg::GetFileSize(CString strPath)
{
	DWORD dwSize = 0;
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(strPath, &wfd);
	if(hFind != INVALID_HANDLE_VALUE)
		dwSize = wfd.nFileSizeLow;
	FindClose(hFind);
	return dwSize;
}

DWORD CSmsSenderDlg::GetDirSize(CString strPath)
{
	DWORD dwDirSize = 0;
	CString strFilePath = strPath + _T("\\*.*");
	CFileFind ff;
	BOOL bFind = ff.FindFile(strFilePath);
	while(bFind)
	{
		bFind = ff.FindNextFile();
		if(!ff.IsDots())
		{
			CString strTempPath = ff.GetFilePath();
			if(!ff.IsDirectory())
			{
				dwDirSize += ff.GetLength();
			}
			else
			{
				dwDirSize += GetDirSize(strTempPath);
			}
		}
	}
	ff.Close();
	return dwDirSize;
}

BOOL CSmsSenderDlg::DeleteDirectory(CString strPath)
{
	BOOL bRet = FALSE;
	CFileFind ff;
	CString strFilePath = strPath + _T("\\*.*");
	BOOL bFind = ff.FindFile(strFilePath);
	while(bFind)
	{
		bFind = ff.FindNextFile();
		if(!ff.IsDots())
		{
			CString strFileName = ff.GetFileName();
			if(ff.IsDirectory())
			{
				CString strDir = strPath + _T("\\") + strFileName;
				DeleteDirectory(strDir);	
			}
			else
			{
				CString strTempFile = strPath + _T("\\") + strFileName;
				DeleteFile(strTempFile);
			}
		}
	}
	ff.Close();
	if(RemoveDirectory(strPath))
		bRet = TRUE;
	return bRet;
}

void CSmsSenderDlg::OnClose() 
{
	int iRet = MessageBox(_T("�˳�������,��ʧȥ��������!\nȷ���˳���?"), 
		_T("��Ϣ"), MB_OKCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2);
	if( iRet == IDCANCEL ) return;	

// 	if(m_bRelationPort == FALSE)
// 	{
// 		ResumeThread(m_pModbusRequestThread->m_hThread);
// 		ResumeThread(m_pModbusRecvThread->m_hThread);
// 	}

	SetEvent(m_hModbusRequestExitEvent);
	SetEvent(m_hModbusRecvExitEvent);

	if(m_bRelationPort) m_Port.CloseSerialPort();
	WaitForSingleObject(m_pModbusRequestThread->m_hThread, INFINITE);
	WaitForSingleObject(m_pModbusRecvThread->m_hThread, INFINITE);
	
	int i = 0;
	for(i=0; i<2; i++)
	{
		CloseHandle(m_hModbusRequestEventArray[i]);
		CloseHandle(m_hModbusRecvEventArray[i]);
	}

	UpdateData();
	_RegSave();
	CDialog::OnClose();
}

void CSmsSenderDlg::_WINDOW_HIDE()
{
	ShowWindow(SW_SHOWMINIMIZED);
	ShowWindow(SW_HIDE);
	UpdateWindow();
	m_TrayIcon.m_bIsWindowNormal=false;
}

void CSmsSenderDlg::_WINDOW_SHOW()
{
	ShowWindow(SW_RESTORE);
	SetForegroundWindow();
	m_TrayIcon.m_bIsWindowNormal=true;
}

void CSmsSenderDlg::OnButtonHide() 
{
	_WINDOW_HIDE();	
}

void CSmsSenderDlg::OnButtonShow() 
{
	_WINDOW_SHOW();	
}

LRESULT CSmsSenderDlg::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	return m_TrayIcon.OnTrayNotification(wParam, lParam);
}

void CSmsSenderDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	PostMessage( WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM( point.x, point.y));	
	CDialog::OnLButtonDown(nFlags, point);
}

void CSmsSenderDlg::P_ShowLog(CString z)
{
	//CString sTime = _Get_Current_Time();
	//CString sLog = sTime + " " + z;
	//WriteFileLog(sLog);
}

LONG CSmsSenderDlg::OnShowPacket(WPARAM wParam,LPARAM lParam)
{
	CString s;
	int i = 0;
	ModbusPacket *mpShow = (ModbusPacket *)wParam;
	if(mpShow == NULL) return 1;
	//s.Format(_T("����͵�����: size=%d dwStart=%u dwDelay=%u "),
	//	m_mpPacket.nSize, m_mpPacket.dwStart, m_mpPacket.dwDelay);
	//s += Data2String(m_mpPacket.data, m_mpPacket.nSize);
	s.Format(_T("����͵�����: size=%d dwStart=%u dwDelay=%u "),
		mpShow->nSize, mpShow->dwStart, mpShow->dwDelay);
	s += Data2String(m_mpPacket.data, m_mpPacket.nSize);
	P_ShowLog(s);
	delete mpShow;
	
	return 0;
}

LONG CSmsSenderDlg::OnRecvMbInfo(WPARAM wParam,LPARAM lParam)
{
	if(wParam)
	{
		char *pMbInfo = (char *)wParam;
		CString sLog(pMbInfo);
		P_ShowLog(sLog);
		delete pMbInfo;
	}
	return 0;
}

LRESULT CSmsSenderDlg::OnCalcTorque(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

CString CSmsSenderDlg::_Get_Current_Time(int t_iFlag)
{
	char buf[1024];
	time_t Second;
	struct tm *now;

	Second = time(0);
	now = localtime(&Second);
	now->tm_year += 1900;
	
	if(t_iFlag==0)
		sprintf(buf, _T("%4i-%02i-%02i %02i:%02i:%02i"), now->tm_year, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
	else if(t_iFlag == 1)
		sprintf(buf, _T("%02i:%02i:%02i"), now->tm_hour, now->tm_min, now->tm_sec);
	else if(t_iFlag == 2)
		sprintf(buf, _T("%02i/%02i/%4i"), now->tm_mday, now->tm_mon+1, now->tm_year);
	else if(t_iFlag == 3)
		sprintf(buf, _T("%4i-%02i-%02i %02iH%02iM%02iS"), now->tm_year, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
	else
		sprintf(buf, _T("%02i:%02i:%02i"), now->tm_hour, now->tm_min, now->tm_sec);
	return (CString)buf;
}

CSmsSenderDlg* AG_GetMainDlg()
{
	CSmsSenderDlg* pDlg=(CSmsSenderDlg*)AfxGetApp()->m_pMainWnd;
	return pDlg;
}

void AG_Log(CString z)
{
	AG_GetMainDlg()->P_ShowLog(z);
}

LONG CSmsSenderDlg::OnCommunication(WPARAM ch, LPARAM port)
{
	return 0;
}

LONG CSmsSenderDlg::OnCTSDetected(WPARAM, LPARAM port)
{
	return 0;
}

LONG CSmsSenderDlg::OnCommRecieveData(WPARAM wParam, LPARAM lParam)
{
	CString sSend, sRecv, sTmp;
	byte *ucData = m_mpPacket.data;
	byte *ucRecv = m_mpPacket.ucRecvBuf;

	WORD wByteCnt = LOWORD(lParam);
	WORD wPortNum = HIWORD(lParam);
	byte *data = (byte *)wParam;

	int i = 0;
	int b = m_mpPacket.nRecvSize;
	int e = b + wByteCnt;
	for(i=b; i<e; i++)
		ucRecv[i] = data[i-b];
	m_mpPacket.nRecvSize = e;

	if(ucData[0] != ucRecv[0])
	{
		sSend = _T("��������: ") + Data2String(ucData, m_mpPacket.nSize);
		sRecv = _T("��������: ") + Data2String(ucRecv, m_mpPacket.nRecvSize);
		sTmp = _T("���������뷢�������豸��ַ��һ��");
		char *pMbInfo = new char[1024];
		memset(pMbInfo, 0, 1024);
		sprintf(pMbInfo, _T("%s %s %s"), sSend, sRecv, sTmp);
		PostMessage(WM_RECVMBINFO, (WPARAM)pMbInfo);
		return 0;
	}
	if(ucRecv[1] & 0x80) //ͨ�ų���
	{
		if(ucRecv[2] == 0x01)
			sTmp = _T("���ܴ����");
		else if(ucRecv[2] == 0x02)
			sTmp = _T("���ݵ�ַ��");
		else if(ucRecv[2] == 0x03)
			sTmp = _T("����ֵ��");
		else if(ucRecv[2] == 0x04)
			sTmp = _T("CRC��");
		else
			sTmp = _T("�������ݳ���");
		sSend = _T("��������: ") + Data2String(ucData, m_mpPacket.nSize);
		sRecv = _T("��������: ") + Data2String(ucRecv, m_mpPacket.nRecvSize);
		char *pMbInfo = new char[1024];
		memset(pMbInfo, 0, 1024);
		sprintf(pMbInfo, _T("%s %s %s"), sSend, sRecv, sTmp);
		PostMessage(WM_RECVMBINFO, (WPARAM)pMbInfo);
		return 0;
	}

	if(ucRecv[1] == 0x03) //�������
	{
		int nSize = ucRecv[2];
		nSize = nSize + 2 + 1 + 2;
		if(m_mpPacket.nRecvSize>= nSize)
		{
			ModbusPacket *packet = new ModbusPacket;
			memset(packet, 0, sizeof(ModbusPacket));
			m_mpPacket.next = NULL;
			memcpy(packet, &m_mpPacket, sizeof(ModbusPacket));
			packet->nRecvSize = nSize;
			m_mqRecvQueue.Push(packet);
			sSend = _T("��������: ") + Data2String(ucData, m_mpPacket.nSize);
			sRecv = _T("��������: ") + Data2String(ucRecv, m_mpPacket.nRecvSize);
			sTmp = _T("����������������");
			char *pMbInfo = new char[1024];
			memset(pMbInfo, 0, 1024);
			sprintf(pMbInfo, _T("%s %s %s"), sSend, sRecv, sTmp);
			PostMessage(WM_RECVMBINFO, (WPARAM)pMbInfo);
			memset(&m_mpPacket, 0, sizeof(ModbusPacket));
			SetEvent(m_hModbusRecvNextEvent);
			return 0;
		}
		else
		{
			sSend = _T("��������: ") + Data2String(ucData, m_mpPacket.nSize);
			sRecv = _T("��������: ") + Data2String(ucRecv, m_mpPacket.nRecvSize);
			sTmp = _T("������ص����ݲ�����");
			m_sMbInfo = sSend + sRecv + sTmp;
			char *pMbInfo = new char[1024];
			memset(pMbInfo, 0, 1024);
			sprintf(pMbInfo, _T("%s %s %s"), sSend, sRecv, sTmp);
			PostMessage(WM_RECVMBINFO, (WPARAM)pMbInfo);
			return 0;
		}
	}
	else if(ucRecv[1] == 0x06 || ucRecv[1] == 0x10) //д�����
	{
		if(m_mpPacket.nRecvSize >= 8)
		{
			ModbusPacket *packet = new ModbusPacket;
			memset(packet, 0, sizeof(ModbusPacket));
			m_mpPacket.next = NULL;
			memcpy(packet, &m_mpPacket, sizeof(ModbusPacket));
			//packet->nRecvSize = nSize;
			m_mqRecvQueue.Push(packet);
			sSend = _T("��������: ") + Data2String(ucData, m_mpPacket.nSize);
			sRecv = _T("��������: ") + Data2String(ucRecv, m_mpPacket.nRecvSize);
			sTmp = _T("д��������������");
			if(ucRecv[1] == 0x06)
			{
				BOOL bFlag = TRUE;
				for(i=0; i<m_mpPacket.nRecvSize; i++)
				{
					if(ucData[i] != ucRecv[i])
					{
						bFlag = FALSE;
						break;
					}
				}
				if(bFlag)
				{
					sTmp = _T("д�����Ĵ�������--�ɹ�");
					if(ucRecv[0] == 0x04 && ucRecv[2] == 0x00 && ucRecv[3] == 0x00)
						WriteApplyRs485Start();
					if(ucRecv[0] == 0x04 && ucRecv[2] == 0x00 && ucRecv[3] == 0x01)
						WriteReleaseRs485Stop();
				}
				else
					sTmp = _T("д�����Ĵ�������--ʧ��");
			}
			if(ucRecv[1] == 0x10)
			{
				if(ucData[2] == ucRecv[2] && ucData[3] == ucRecv[3] && ucData[4] == ucRecv[4] && ucData[5] == ucRecv[5])
					sTmp = _T("д����Ĵ�������--�ɹ�");
				else
					sTmp = _T("д����Ĵ�������--ʧ��");
			}

			m_sMbInfo = sSend + sRecv + sTmp;
			char *pMbInfo = new char[1024];
			memset(pMbInfo, 0, 1024);
			sprintf(pMbInfo, _T("%s %s %s"), sSend, sRecv, sTmp);
			PostMessage(WM_RECVMBINFO, (WPARAM)pMbInfo);
			memset(&m_mpPacket, 0, sizeof(ModbusPacket));
			SetEvent(m_hModbusRecvNextEvent);
			return 0;
		}
		else
		{
			sTmp = _T("д����ص����ݲ�����");
			sSend = _T("��������: ") + Data2String(ucData, m_mpPacket.nSize);
			sRecv = _T("��������: ") + Data2String(ucRecv, m_mpPacket.nRecvSize);
			m_sMbInfo = sSend + sRecv + sTmp;
			char *pMbInfo = new char[1024];
			memset(pMbInfo, 0, 1024);
			sprintf(pMbInfo, _T("%s %s %s"), sSend, sRecv, sTmp);
			PostMessage(WM_RECVMBINFO, (WPARAM)pMbInfo);
			return 0;
		}
	}
	else
	{
		sSend = _T("��������: ") + Data2String(ucData, m_mpPacket.nSize);
		sRecv = _T("��������: ") + Data2String(ucRecv, m_mpPacket.nRecvSize);
		sTmp = _T("δ֪����");
		m_sMbInfo = sSend + sRecv + sTmp;
		char *pMbInfo = new char[1024];
		memset(pMbInfo, 0, 1024);
		sprintf(pMbInfo, _T("%s %s %s"), sSend, sRecv, sTmp);
		PostMessage(WM_RECVMBINFO, (WPARAM)pMbInfo);
	}
	return 0;
}

void CSmsSenderDlg::OnAppAbout() 
{
	CAboutDlg dlg;
	dlg.DoModal();
}

void CSmsSenderDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent == 1)
	{
		if(m_bRelationPort)
		{
			CString sLog;
			int iCnt = 0;
			iCnt = m_mqWriteQueue.GetCount();
			if(iCnt == 0 && m_bAutoStartIM && fabs(m_GCInfo.fRev)<1.00)
			{
				m_bAutoStartIM = FALSE;
				OnMenuStopRecord();
			}
			iCnt = m_mqReadQueue.GetCount();
			sLog.Format(_T("OnTimer m_mqReadQueue.GetCount=%d"), iCnt);
			P_ShowLog(sLog);
			//if(iCnt > COMMAND_COUNT)
			if(iCnt > 0)
			{
				int i = 0, iLineCnt = 0;
				long lx = time(0);
				float fy = 0.00;
				CLineEx *line = 0;
				line = &m_stcDraw.m_lineRev;
				iLineCnt = line->GetCount();
				if(iLineCnt>0)
				{
					fy = line->m_fYValue[iLineCnt-1];
					line->AddPoint(lx, fy);
				}

				for(i=0; i<3; i++)
				{
					line = &m_stcDraw.m_lineVibration[i];
					iLineCnt = line->GetCount();
					if(iLineCnt>0)
					{
						fy = line->m_fYValue[iLineCnt-1];
						line->AddPoint(lx, fy);
					}
				}

				for(i=0; i<8; i++)
				{
					line = &m_stcDraw.m_lineTemperature[i];
					iLineCnt = line->GetCount();
					if(iLineCnt>0)
					{
						fy = line->m_fYValue[iLineCnt-1];
						line->AddPoint(lx, fy);
					}
				}

				line = &m_stcDraw.m_lineTorque;
				iLineCnt = line->GetCount();
				if(iLineCnt>0)
				{
					fy = line->m_fYValue[iLineCnt-1];
					line->AddPoint(lx, fy);
				}
				m_nReadUsableNum++;
				return;
			}
			if(m_nReadUsableNum%600000 == 0) //10���ӷ���һ��
			{
				ReadUsable(); //08 ����Ƶ������Ƿ����
				m_nReadUsableNum++;
				if(m_nReadUsableNum >= 600000)
					m_nReadUsableNum = 0;
			}
			ReadElectricPower(); //06 �ۺϵ�����EL2020
			ReadRev(); //01 ת�ٱ�MS5000D
			ReadVibration(); //02 �񶯲���3ͨ�����Ա�MD52U
			ReadTemperature();  //03 �¶�Ѳ����MD52U-8
			ReadFrequencyAndOutputCurrent(); //05 ��Ƶ������Ƶ�ʺ��������2ͨ���������Ա�MB70U
			ReadTransducer(); //08 ��Ƶ��FR-A740-22K
			//ReadPlcSM(); //04 PLC FX2N48MT-D
			iCnt = m_mqReadQueue.GetCount();
			SetEvent(m_hModbusRequestNextEvent);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

CString CSmsSenderDlg::GetExePath()
{
	char strFilePath[512];
	char *temp;
	int ch = '\\';
	memset(strFilePath,0,sizeof(strFilePath));

	GetModuleFileName(NULL, strFilePath, MAX_PATH);
	temp = strrchr(strFilePath,ch);
	*(temp+1) = '\0';

	CString strFileAllPath;
	strFileAllPath = strFilePath;

	return strFileAllPath;
}

void CSmsSenderDlg::WriteFileLog(CString log)
{
	char sLog[1024] = {0};
	memset(sLog, 0, 1024);
	CString sPath = m_sLogPath;
	DWORD dwSize = GetFileSize(sPath);
	if(dwSize > (2 * 1024 * 1024))
	{
		m_sLogPath = m_sLogDir + _T("\\") + _Get_Current_Time(3) + _T(".log");
		sPath = m_sLogPath;
	}
	FILE *fp = fopen(sPath, _T("a+"));
	if(fp)
	{
		sprintf(sLog, _T("%s\n"), log);
		fwrite(sLog, strlen(sLog), 1, fp);
		if(fp) fclose(fp);
	}
}

void CSmsSenderDlg::SaveUserToFile()
{
	CString sDir = m_sDataStorageDir;
	CString sFile = sDir + _T("\\") + _T("UserInfo") + _T(".gc");
	FILE *fp = fopen(sFile, "w+");
	if(fp)
	{
		int i = 0;
		char sData[1024] = {0};
		
		//UserName
		memset(sData, 0, 1024);
		sprintf(sData, _T("UserName=%s\n"), m_sUserName);
		fwrite(sData, strlen(sData), 1, fp);
		//UserNum
		memset(sData, 0, 1024);
		sprintf(sData, _T("UserNum=%s\n"), m_sUserNum);
		fwrite(sData, strlen(sData), 1, fp);
		//ProductType
		memset(sData, 0, 1024);
		sprintf(sData, _T("ProductType=%s\n"), m_sProductType);
		fwrite(sData, strlen(sData), 1, fp);
		//ProductNum
		memset(sData, 0, 1024);
		sprintf(sData, _T("ProductNum=%s\n"), m_sProductNum);
		fwrite(sData, strlen(sData), 1, fp);
		//Producer
		memset(sData, 0, 1024);
		sprintf(sData, _T("Producer=%s\n"), m_sProducer);
		fwrite(sData, strlen(sData), 1, fp);
		//TestName
		memset(sData, 0, 1024);
		sprintf(sData, _T("TestName=%s\n"), m_sTestName);
		fwrite(sData, strlen(sData), 1, fp);
		
		fclose(fp);
	}
}

void CSmsSenderDlg::WriteStorageFile(CString strName, CLineEx *line)
{
	CString sDir = m_sDataStorageDir;
	CString sFile = sDir + _T("\\") + strName + _T(".gc");
	FILE *fp = fopen(sFile, "w+");
	if(fp)
	{
		int i = 0, iLineCnt = 0;
		iLineCnt = line->GetCount();
		char sData[1024] = {0};
		memset(sData, 0, 1024);
		sprintf(sData, _T("[%s]\n"), strName);
		fwrite(sData, strlen(sData), 1, fp);
		memset(sData, 0, 1024);
		struct tm * lt = localtime(&line->m_lXValue[0]);
		sprintf(sData, _T("Time=%4i-%02i-%02i %02i:%02i:%02i\n"), lt->tm_year+1900, lt->tm_mon+1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
		fwrite(sData, strlen(sData), 1, fp);
		memset(sData, 0, 1024);
		sprintf(sData, _T("IsDraw=%d\nStyle=%d\nThick=%d\nColor=%d\n"), line->m_bDraw, line->m_iStyle, line->m_iThick, line->m_clrColor);
		fwrite(sData, strlen(sData), 1, fp);
		memset(sData, 0, 1024);
		sprintf(sData, _T("MaxValue=%.02f\nMinValue=%.02f\nCount=%d\n"), line->m_fMaxValue, line->m_fMinValue, iLineCnt);
		fwrite(sData, strlen(sData), 1, fp);
		for(i=0; i<iLineCnt; i++)
		{
			memset(sData, 0, 1024);
			sprintf(sData, _T("%d=%d %.02f\n"), i, line->m_lXValue[i], line->m_fYValue[i]);
			fwrite(sData, strlen(sData), 1, fp);
		}
		fclose(fp);
	}
}

void CSmsSenderDlg::SaveConfig(CString strFileName)
{
	CStdioFile file;
	CString strLine;
	CString	strValue;
	int i = 0;
	try
	{
		if(!file.Open(strFileName,CFile::modeReadWrite|CFile::modeCreate))
			return;

		file.WriteString("[COMSET]\n");
		strLine.Format("SERIALPORTNUM=%d\n", m_nSerialPortNum);
		file.WriteString(strLine);
		strLine.Format("BAUDRATE=%d\n", m_nBaudRate);
		file.WriteString(strLine);
		strLine.Format("STARTBITS=%d\n", m_nStartBits);
		file.WriteString(strLine);
		strLine.Format("DATABITS=%d\n", m_nDataBits);
		file.WriteString(strLine);
		strLine.Format("VERIFYBITS=%d\n", m_nVerifyBits);
		file.WriteString(strLine);
		strLine.Format("STOPBITS=%d\n", m_nStopBits);
		file.WriteString(strLine);
		strLine.Format("AUTORELATION=%d\n", m_bAutoRelation);
		file.WriteString(strLine);
		strLine.Format("TIMEOUT=%d\n", m_nTimeOut);
		file.WriteString(strLine);
		strLine.Format("\n");
		file.WriteString(strLine);

		file.WriteString(_T("[USERSET]\n"));
		strLine.Format(_T("USERNAME=%s\n"), m_sUserName);
		file.WriteString(strLine);
		strLine.Format(_T("USERNUM=%s\n"), m_sUserNum);
		file.WriteString(strLine);
		strLine.Format(_T("PRODUCTTYPE=%s\n"), m_sProductType);
		file.WriteString(strLine);
		strLine.Format(_T("PRODUCTNUM=%s\n"), m_sProductNum);
		file.WriteString(strLine);
		strLine.Format(_T("PRODUCER=%s\n"), m_sProducer);
		file.WriteString(strLine);
		strLine.Format(_T("TESTNAME=%s\n"), m_sTestName);
		file.WriteString(strLine);
		strLine.Format(_T("PASSWD=%s\n"), m_sPasswd);
		file.WriteString(strLine);
		strLine.Format("\n");
		file.WriteString(strLine);

		file.WriteString(_T("[SPEEDSET]\n"));
		for(i=0; i<7; i++)
		{
			strLine.Format(_T("SPEED%d=%d\n"), i, m_wSpeed[i]);
			file.WriteString(strLine);
			strLine.Format(_T("INTERVAL%d=%d\n"), i, m_nInterval[i]);
			file.WriteString(strLine);
		}
		strLine.Format(_T("RUNSPEED=%d\n"), m_wRunSpeed);
		file.WriteString(strLine);
		strLine.Format("\n");
		file.WriteString(strLine);

		file.WriteString(_T("[ALARMSET]\n"));
		strLine.Format(_T("ISALARMTEMPERATURE=%d\n"), m_bIsAlarmTemperature);
		file.WriteString(strLine);
		strLine.Format(_T("ALARMTEMPERATURE=%.02f\n"), m_fAlarmTemperature);
		file.WriteString(strLine);
		strLine.Format(_T("ISALARMVIBRATION=%d\n"), m_bIsAlarmVibration);
		file.WriteString(strLine);
		strLine.Format(_T("ALARMVIBRATION=%.02f\n"), m_fAlarmVibration);
		file.WriteString(strLine);
		strLine.Format("\n");
		file.WriteString(strLine);

		file.WriteString("[DATASTORAGE]\n");
		strLine.Format("USERDIR=%s\n", m_sUserDir);
		file.WriteString(strLine);
		strLine.Format("\n");
		file.WriteString(strLine);

		file.WriteString(_T("[CURVECOLOR]\n"));
		strLine.Format(_T("ISREV=%d\n"), m_bRev);
		file.WriteString(strLine);
		strLine.Format(_T("REVCOLOR=%d\n"), m_clrRev);
		file.WriteString(strLine);
		for(i=0; i<3; i++)
		{
			strLine.Format(_T("ISVIBRATION%d=%d\n"), i, m_bVibration[i]);
			file.WriteString(strLine);
			strLine.Format(_T("VIBRATIONCOLOR%d=%d\n"), i, m_clrVibration[i]);
			file.WriteString(strLine);
		}
		for(i=0; i<8; i++)
		{
			strLine.Format(_T("ISTEMPERATURE%d=%d\n"), i, m_bTemperature[i]);
			file.WriteString(strLine);
			strLine.Format(_T("TEMPERATURECOLOR%d=%d\n"), i, m_clrTemperature[i]);
			file.WriteString(strLine);
		}
		strLine.Format(_T("ISTORQUE=%d\n"), m_bTorque);
		file.WriteString(strLine);
		strLine.Format(_T("TORQUECOLOR=%d\n"), m_clrTorque);
		file.WriteString(strLine);
		strLine.Format("\n");
		file.WriteString(strLine);

		file.WriteString(_T("[SPEEDDOWNSET]\n"));
		strLine.Format(_T("SPEEDDOWNRATE=%.04f\n"), m_fSpeedDownRate);
		file.WriteString(strLine);
		strLine.Format("\n");
		file.WriteString(strLine);

		file.Close();
	}
	catch(...)
	{}
}

void CSmsSenderDlg::LoadConfig(CString strFileName)
{
	CStdioFile	file;
	CString strLine;
	CString	strValue;
	int nIndex ;
	int i = 0;
	try
	{
		if(!file.Open(strFileName,CFile::modeRead))
		{
			this->SaveConfig(strFileName);
			return;
		}
		while(file.ReadString(strLine))
		{
			strLine.TrimLeft();
			strLine.TrimRight();
			if(strLine.IsEmpty()) continue;
			if(strLine.GetAt(0) == '#') continue;
			if(strLine == _T("[COMSET]")) continue;
			if(strLine == _T("[USERSET]")) continue;
			if(strLine == _T("[SPEEDSET]")) continue;
			if(strLine == _T("[ALARMSET]")) continue;
			if(strLine == _T("[DATASTORAGE]")) continue;
			if(strLine == _T("[CURVECOLOR]")) continue;
			if(strLine == _T("[SPEEDDOWNSET]")) continue;

			nIndex = strLine.Find('=');
			strValue = strLine.Right(strLine.GetLength() - nIndex - 1);
			strLine = strLine.Left(nIndex);
			//ComSet
			if(strLine == "AUTORELATION")
			{
				m_bAutoRelation = atoi(strValue);
			}
			else if(strLine == "SERIALPORTNUM")
			{
				m_nSerialPortNum = atoi(strValue);
			}
			else if(strLine == "BAUDRATE")
			{
				m_nBaudRate = atoi(strValue);
			}
			else if(strLine == "STARTBITS")
			{
				m_nStartBits = atoi(strValue);
			}
			else if(strLine == "DATABITS")
			{
				m_nDataBits = atoi(strValue);
			}
			else if(strLine == "VERIFYBITS")
			{
				m_nVerifyBits = atoi(strValue);
			}
			else if(strLine == "STOPBITS")
			{
				m_nStopBits = atoi(strValue);
			}
			else if(strLine == "TIMEOUT")
			{
				m_nTimeOut = atoi(strValue);
			}
			//UserSet
			else if(strLine == _T("USERNAME"))
			{
				m_sUserName = strValue;
			}
			else if(strLine == _T("USERNUM"))
			{
				m_sUserNum = strValue;
			}
			else if(strLine == _T("PRODUCTTYPE"))
			{
				m_sProductType = strValue;
			}
			else if(strLine == _T("PRODUCTNUM"))
			{
				m_sProductNum = strValue;
			}
			else if(strLine == _T("PRODUCER"))
			{
				m_sProducer = strValue;
			}
			else if(strLine == _T("TESTNAME"))
			{
				m_sTestName = strValue;
			}
			else if(strLine == _T("PASSWD"))
			{
				m_sPasswd = strValue;
			}
			//SpeedDownSet
			else if(strLine == _T("SPEEDDOWNRATE"))
			{
				m_fSpeedDownRate = atof(strValue);
			}
			//SpeedSet
			else if(strLine == _T("RUNSPEED"))
			{
				m_wRunSpeed = (WORD)atoi(strValue);
			}
			else if(strLine.Find(_T("SPEED")) != -1)
			{
				for(i=0; i<7; i++)
				{
					CString s;
					s.Format(_T("SPEED%d"), i);
					if(strLine == s)
					{
						m_wSpeed[i] = (WORD)atoi(strValue);
						break;
					}
				}
			}
			else if(strLine.Find(_T("INTERVAL")) != -1)
			{
				for(i=0; i<7; i++)
				{
					CString s;
					s.Format(_T("INTERVAL%d"), i);
					if(strLine == s)
					{
						m_nInterval[i] = atoi(strValue);
						break;
					}
				}
			}
			//AlarmSet
			else if(strLine == _T("ISALARMTEMPERATURE"))
			{
				m_bIsAlarmTemperature = atoi(strValue);
			}
			else if(strLine == _T("ALARMTEMPERATURE"))
			{
				m_fAlarmTemperature = atof(strValue);
			}
			else if(strLine == _T("ISALARMVIBRATION"))
			{
				m_bIsAlarmVibration = atoi(strValue);
			}
			else if(strLine == _T("ALARMVIBRATION"))
			{
				m_fAlarmVibration = atof(strValue);
			}
			//DataStorage
			else if(strLine == _T("USERDIR"))
			{
				m_sUserDir = strValue;
			}
			//CURVECOLOR
			else if(strLine == _T("ISREV"))
			{
				m_bRev = atoi(strValue);
			}
			else if(strLine == _T("REVCOLOR"))
			{
				m_clrRev = atoi(strValue);
			}
			else if(strLine.Find(_T("ISVIBRATION")) != -1)
			{
				for(i=0; i<3; i++)
				{
					CString s;
					s.Format(_T("ISVIBRATION%d"), i);
					if(strLine == s)
					{
						m_bVibration[i] = atoi(strValue);
						break;
					}
				}
			}
			else if(strLine.Find(_T("VIBRATIONCOLOR")) != -1)
			{
				for(i=0; i<3; i++)
				{
					CString s;
					s.Format(_T("VIBRATIONCOLOR%d"), i);
					if(strLine == s)
					{
						m_clrVibration[i] = atoi(strValue);
						break;
					}
				}
			}
			else if(strLine.Find(_T("ISTEMPERATURE")) != -1)
			{
				for(i=0; i<8; i++)
				{
					CString s;
					s.Format(_T("ISTEMPERATURE%d"), i);
					if(strLine == s)
					{
						m_bTemperature[i] = atoi(strValue);
						break;
					}
				}
			}
			else if(strLine.Find(_T("TEMPERATURECOLOR")) != -1)
			{
				for(i=0; i<8; i++)
				{
					CString s;
					s.Format(_T("TEMPERATURECOLOR%d"), i);
					if(strLine == s)
					{
						m_clrTemperature[i] = atoi(strValue);
						break;
					}
				}
			}
			else if(strLine == _T("ISTORQUE"))
			{
				m_bTorque = atoi(strValue);
			}
			else if(strLine == _T("TORQUECOLOR"))
			{
				m_clrTorque = atoi(strValue);
			}
		}
		file.Close();
	}
	catch(...)
	{}
}

void CSmsSenderDlg::OnMenuAlarmset() 
{
	// TODO: Add your command handler code here
	CAlarmSetDlg dlgAlarmSet;
	dlgAlarmSet.m_bIsAlarmTemperature = m_bIsAlarmTemperature;
	dlgAlarmSet.m_bIsAlarmVibration = m_bIsAlarmVibration;
	dlgAlarmSet.m_fAlarmTemperature = m_fAlarmTemperature;
	dlgAlarmSet.m_fAlarmVibration = m_fAlarmVibration;
	dlgAlarmSet.DoModal();
}

void CSmsSenderDlg::WriteRunSpeed() //д��Ƶ���������������ٶ�
{
	int i = 0;
	unsigned short usCrc16 = 0;
	CString s;
	ModbusPacket *packet = new ModbusPacket;
	memset(packet, 0, sizeof(ModbusPacket));
	byte regCnt = 1;
	byte *data = NULL;
	packet->nSize = regCnt*2 + 6;
	data = packet->data;
	data[0] = 0x08; //��Ƶ��
	data[1] = 0x06; //д�����Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x0D;
	s.Format(_T("%04X"), m_wRunSpeed);
	sscanf(s, _T("%2X%02X"), &data[4], &data[5]);
	usCrc16 = FormCrc16((unsigned char *)data, packet->nSize-2);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	
	packet->dwStart = GetTickCount();
	packet->dwDelay = 0;
	m_mqWriteQueue.Push(packet);
	//m_mqReadQueue.Push(packet);
	
	s.Format(_T("[д��Ƶ�����������ٶ�]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s += Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::WriteApplyRs485Start() //����485���Ƴɹ�֮��д��Ƶ����ת/��ת����
{
	int i = 0;
	unsigned short usCrc16 = 0;
	CString s;
	ModbusPacket *packet = new ModbusPacket;
	memset(packet, 0, sizeof(ModbusPacket));
	byte regCnt = 1;
	byte *data = NULL;
	packet->nSize = regCnt*2 + 6;
	data = packet->data;
	data[0] = 0x08; //��Ƶ��
	data[1] = 0x06; //д�����Ĵ���
	data[2] = 0x05; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x3A;
	data[4] = 0x00;
	data[5] = 0x00;
	usCrc16 = FormCrc16((unsigned char *)data, packet->nSize-2);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	
	packet->dwStart = GetTickCount();
	packet->dwDelay = 0;
	//m_mqWriteQueue.Push(packet);
	m_mqReadQueue.Push(packet);
	
	s.Format(_T("[����485���Ƴɹ�֮��д��Ƶ����ת/��ת����]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s += Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::WriteReleaseRs485Stop() //�ͷ�485���Ƴɹ�֮��д��Ƶ����ת/��תֹͣ
{
	int i = 0;
	unsigned short usCrc16 = 0;
	CString s;
	ModbusPacket *packet = new ModbusPacket;
	memset(packet, 0, sizeof(ModbusPacket));
	byte regCnt = 1;
	byte *data = NULL;
	packet->nSize = regCnt*2 + 6;
	data = packet->data;
	data[0] = 0x08; //��Ƶ��
	data[1] = 0x06; //д�����Ĵ���
	data[2] = 0x05; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x3A;
	data[4] = 0x00;
	data[5] = 0x01;
	usCrc16 = FormCrc16((unsigned char *)data, packet->nSize-2);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	
	packet->dwStart = GetTickCount();
	packet->dwDelay = 0;
	//m_mqWriteQueue.Push(packet);
	m_mqReadQueue.Push(packet);
	
	s.Format(_T("[�ͷ�485���Ƴɹ�֮��д��Ƶ����ת/��תֹͣ]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s += Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::OnMenuSpeedset() 
{
	// TODO: Add your command handler code here
	if(m_bRelationPort == FALSE)
	{
		MessageBox(_T("����δ����"), _T("��������"), MB_OK);
		return;
	}
	if(m_bUsable != 1)
	{
		MessageBox(_T("�Ѳ�����ʹ�ô�����ˣ����˳�!"), _T("GearCase"), MB_OK);
		return;
	}
 	int iCnt = m_mqWriteQueue.GetCount();
	if(iCnt > COMMAND_COUNT)
	{
		MessageBox(_T("�����������������࣬������ִ�д�������Ժ���ִ�д�����"), _T("��������"), MB_OK);
		return;
	}
	if(iCnt > 0 || m_bAutoStartIM)
	{
		MessageBox(_T("�����Զ������У��������ö��٣����Ժ���ִ�д�����"), _T("��������"), MB_OK);
		return;
	}

	int i = 0;
	CSpeedSetDlg dlgSpeedSet;
	for(i=0; i<7; i++)
	{
		dlgSpeedSet.m_wSpeed[i] = m_wSpeed[i];
		dlgSpeedSet.m_nInterval[i] = m_nInterval[i];
	}
	dlgSpeedSet.m_wRunSpeed = m_wRunSpeed;

	int nRet = dlgSpeedSet.DoModal();
	if(nRet == IDOK)
	{
		CString s, sTmp;
		m_dwStart = GetTickCount();
		ModbusPacket *packet = new ModbusPacket;
		memset(packet, 0, sizeof(ModbusPacket));
		byte regCnt = 3;
		byte *data = NULL;
		packet->nSize = regCnt*2 + 9;
		data = packet->data;
		data[0] = 0x08; //��Ƶ��
		data[1] = 0x10; //д����Ĵ���
		data[2] = 0x03; //2�ֽ�ƫ�Ƶ�ַ
		data[3] = 0xEB;
		data[4] = 0x00; //2�ֽڼĴ������������ö���1������2������3
		data[5] = 0x03;
		data[6] = regCnt*2; //1�ֽ��ֽ������Ĵ�������*2
		for(i=0; i<regCnt; i++)
		{
			s.Format(_T("%04X"), m_wSpeed[2-i]);
			sscanf(s, _T("%02X%02X"), &data[2*i+7], &data[2*i+8]);
		}
		unsigned short usCrc16 = 0;
		usCrc16 = FormCrc16((unsigned char *)data, packet->nSize-2);
		data[(regCnt-1)*2+9] = ((char*)&usCrc16)[0];
		data[(regCnt-1)*2+10] = ((char*)&usCrc16)[1];

		packet->dwStart = m_dwStart;
		packet->dwDelay = 0;
		m_mqWriteQueue.Push(packet);
		//m_mqReadQueue.Push(packet);
		
		s.Format(_T("[���ö���1-3]size=%d dwStart=%u dwDelay=%u "),
			packet->nSize, packet->dwStart, packet->dwDelay);
		s = s + Data2String(packet->data, packet->nSize);
		P_ShowLog(s);

		ModbusPacket *packet2 = new ModbusPacket;
		memset(packet2, 0, sizeof(ModbusPacket));
		byte regCnt2 = 4;
		byte *data2 = NULL;
		packet2->nSize = regCnt2*2 + 9;
		data2 = packet2->data;
		data2[0] = 0x08; //��Ƶ��
		data2[1] = 0x10; //д����Ĵ���
		data2[2] = 0x03; //2�ֽ�ƫ�Ƶ�ַ
		data2[3] = 0xFF;
		data2[4] = 0x00; //2�ֽڼĴ������������ö���4������5������6������7
		data2[5] = 0x04;
		data2[6] = regCnt2*2; //1�ֽ��ֽ���
		for(i=0; i<regCnt2; i++)
		{
			s.Format(_T("%04X"), m_wSpeed[i+3]);
			sscanf(s, _T("%2X%02X"), &data2[2*i+7], &data2[2*i+8]);
		}
		usCrc16 = FormCrc16((unsigned char *)data2, packet2->nSize-2);
		data2[(regCnt2-1)*2+9] = ((char*)&usCrc16)[0];
		data2[(regCnt2-1)*2+10] = ((char*)&usCrc16)[1];

		packet2->dwStart = m_dwStart;
		packet2->dwDelay = 0;
		m_mqWriteQueue.Push(packet2);
		//m_mqReadQueue.Push(packet2);

		s.Format(_T("[���ö���4-7]size=%d dwStart=%u dwDelay=%u "),
			packet2->nSize, packet2->dwStart, packet2->dwDelay);
		s = s + Data2String(packet2->data, packet2->nSize);
		P_ShowLog(s);

		WriteRunSpeed();

		iCnt = m_mqWriteQueue.GetCount();
		//iCnt = m_mqReadQueue.GetCount();
		if(iCnt == 3) SetEvent(m_hModbusRequestNextEvent);
	}
}

void CSmsSenderDlg::OnMenuRelationCom()
{
	// TODO: Add your command handler code here
	if(!m_bRelationPort)
	{
		//if(!UpdateData())
		//	m_nTimeOut = 10;
		if(m_nSerialPortNum<1 || m_nSerialPortNum>4)
		{
			CString sLog;
			sLog.Format(_T("��������COM%dʧ��, ���ںŲ��Ϸ�"), m_nSerialPortNum);
			P_ShowLog(sLog);
			return;
		}
		char parity = 'E';
		if(m_nVerifyBits == 0)
			parity = 'N';
		else if(m_nVerifyBits == 1)
			parity = 'O';
		else if(m_nVerifyBits == 2)
			parity = 'E';
		else if(m_nVerifyBits == 3)
			parity = 'M';
		else if(m_nVerifyBits == 4)
			parity = 'S';

		if(m_Port.InitPort(this, m_nSerialPortNum, m_nBaudRate, parity, 8, 1, EV_RXCHAR| EV_CTS, 256))
		{
			m_Port.StartMonitoring();
		}
		else
		{
			CString sLog;
			sLog.Format(_T("��������COM%dʧ�ܣ����ڿ����ѱ���������ռ�ã����ߴ���û��������"), m_nSerialPortNum);
			P_ShowLog(sLog);
			return;
		}

		m_bRelationPort = TRUE;
		P_ShowLog(_T("�������ڣ��ȴ���������"));
		if(m_pModbusRequestThread) ResumeThread(m_pModbusRequestThread->m_hThread);
		if(m_pModbusRecvThread) ResumeThread(m_pModbusRecvThread->m_hThread);
	}
	else
	{
		CString sLog;
		sLog.Format(_T("����COM%d�Ѿ�������"), m_nSerialPortNum);
		P_ShowLog(sLog);
	}
	SetStatusBarText();
}

void CSmsSenderDlg::OnMenuDataStorage() 
{
	// TODO: Add your command handler code here
	CDataStorageDlg dlg;
	dlg.m_sPath = m_sUserDir;
	int iRet = dlg.DoModal();
	if(iRet == IDOK)
	{
		if(m_sUserDir != dlg.m_sPath)
		{
			m_sUserDir = dlg.m_sPath;
			m_sDataStorageDir = m_sUserDir + _T("\\") + _Get_Current_Time(3);
			CreateDirectory(m_sDataStorageDir, NULL);
		}
	}
	TRACE(_T("m_sDataStorageDir=%s\n"), m_sDataStorageDir);
}

void CSmsSenderDlg::OnMenuDataQuery() 
{
	// TODO: Add your command handler code here
	CDataQueryDlg dlg;
	dlg.m_sPath = m_sDataStorageDir;
	dlg.m_sUserName = m_sUserName;
	dlg.m_sUserNum = m_sUserNum;
	dlg.m_sProductType = m_sProductType;
	dlg.m_sProductNum = m_sProductNum;
	dlg.m_sProducer = m_sProducer;
	dlg.m_sTestName = m_sTestName;
	dlg.m_sTestTime = m_sTestTime;
	dlg.m_fSpeedDownRate = m_fSpeedDownRate;
	CLineEx *line = NULL;
	int i = 0;
	line = &(dlg.m_lineRev);
	line->m_bDraw = m_bRev;
	line->m_clrColor = m_clrRev;
	for(i=0; i<3; i++)
	{
		line = &(dlg.m_lineVibration[i]);
		line->m_bDraw = m_bVibration[i];
		line->m_clrColor = m_clrVibration[i];
	}
	for(i=0; i<8; i++)
	{
		line = &(dlg.m_lineTemperature[i]);
		line->m_bDraw = m_bTemperature[i];
		line->m_clrColor = m_clrTemperature[i];
	}
	line = &(dlg.m_lineTorque);
	line->m_bDraw = m_bTorque;
	line->m_clrColor = m_clrTorque;
	int iRet = dlg.DoModal();
}

void CSmsSenderDlg::OnMenuCurveColor() 
{
	// TODO: Add your command handler code here
	int i = 0;
	CCurveColorDlg dlg;
	dlg.m_bRev = m_bRev;
	dlg.m_clrRev = m_clrRev;
	for(i=0; i<3; i++)
	{
		dlg.m_bVibration[i] = m_bVibration[i];
		dlg.m_clrVibration[i] = m_clrVibration[i];
	}
	for(i=0; i<8; i++)
	{
		dlg.m_bTemperature[i] = m_bTemperature[i];
		dlg.m_clrTemperature[i] = m_clrTemperature[i];
	}
	dlg.m_bTorque = m_bTorque;
	dlg.m_clrTorque = m_clrTorque;
	int iRet = dlg.DoModal();
}

void CSmsSenderDlg::OnMenuComset()
{
	// TODO: Add your command handler code here
	CComSetDlg dlgComSet;
	dlgComSet.m_nSerialPortNum = m_nSerialPortNum;
	dlgComSet.m_nBaudRate = m_nBaudRate;
	dlgComSet.m_nStartBits = m_nStartBits;
	dlgComSet.m_nDataBits = m_nDataBits;
	dlgComSet.m_nVerifyBits = m_nVerifyBits;
	dlgComSet.m_nStopBits = m_nStopBits;
	dlgComSet.m_bAutoRelation = m_bAutoRelation;
	dlgComSet.m_nTimeOut = m_nTimeOut;
	dlgComSet.DoModal();
}

void CSmsSenderDlg::OnMenuUserset() 
{
	// TODO: Add your command handler code here
	CUserSetDlg dlgUserSet;
	dlgUserSet.m_sUserName = m_sUserName;
	dlgUserSet.m_sUserNum = m_sUserNum;
	dlgUserSet.m_sProductType = m_sProductType;
	dlgUserSet.m_sProductNum = m_sProductNum;
	dlgUserSet.m_sProducer = m_sProducer;
	dlgUserSet.m_sTestName = m_sTestName;
	dlgUserSet.DoModal();
	SetStatusBarText();
}

void CSmsSenderDlg::OnMenuApplyRs485ctrl() 
{
	// TODO: Add your command handler code here
	CString sText, sCaption, sLog;
	if(m_bRelationPort == FALSE)
	{
		sText.Format(_T("����δ����"));
		sCaption.Format(_T("�������"));
		sLog.Format(_T("[%s]%s"), sCaption, sText);
		P_ShowLog(sLog);
		MessageBox(sText, sCaption, MB_OK);
		return;
	}
	if(m_bUsable != 1)
	{
		MessageBox(_T("�Ѳ�����ʹ�ô�����ˣ����˳�!"), _T("GearCase"), MB_OK);
		return;
	}
	//if(m_GCInfo.wSM0 == 0x01)
	if(m_GCInfo.wModeStateOrSet == 0x0004)
	{
		sText.Format(_T("�Ѿ��Ǽ�������ƣ�����ִ�в���"));
		sCaption.Format(_T("�������"));
		sLog.Format(_T("[%s]%s"), sCaption, sText);
		P_ShowLog(sLog);
		MessageBox(sText, sCaption, MB_OK);
		return;
	}
	//int iCnt = m_mqWriteQueue.GetCount();
	int iCnt = m_mqReadQueue.GetCount();
	if(iCnt > COMMAND_COUNT)
	{
		sText.Format(_T("�����������������࣬������ִ�д�������Ժ���ִ�д�����"));
		sCaption.Format(_T("�������"));
		sLog.Format(_T("[%s]%s"), sCaption, sText);
		P_ShowLog(sLog);
		MessageBox(sText, sCaption, MB_OK);
		return;
	}
	m_dwStart = GetTickCount();
	ModbusPacket *packet = new ModbusPacket;
	memset(packet, 0, sizeof(ModbusPacket));
	byte ucSize = 4;
	byte *data = NULL;
	packet->nSize = ucSize + 4;
	data = packet->data;
	data[0] = 0x04; //PLC
	data[1] = 0x06; //д�����Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x00;
	data[4] = 0x00; //2�ֽڼĴ������ݣ�����RS485����
	data[5] = 0x00;
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	//m_mqWriteQueue.Push(packet);
	m_mqReadQueue.Push(packet);
	iCnt = m_mqReadQueue.GetCount();
	if(iCnt == 1) SetEvent(m_hModbusRequestNextEvent);
	CString s, sTmp;
	int i = 0;
	s.Format(_T("[����RS485����]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	sTmp = Data2String(data, packet->nSize);
	s = s + sTmp;
	P_ShowLog(s);
}

void CSmsSenderDlg::OnMenuReleaseRs485ctrl() 
{
	// TODO: Add your command handler code here
	CString sText, sCaption, sLog;
	if(m_bRelationPort == FALSE)
	{
		MessageBox(_T("����δ����"), _T("�ͷſ���"), MB_OK);
		return;
	}
	if(m_bUsable != 1)
	{
		MessageBox(_T("�Ѳ�����ʹ�ô�����ˣ����˳�!"), _T("GearCase"), MB_OK);
		return;
	}
	if(m_GCInfo.wModeStateOrSet != 0x0004)
	{
		sText.Format(_T("�Ǽ�������ƣ�����ִ�в���"));
		sCaption.Format(_T("�ͷſ���"));
		sLog.Format(_T("[%s]%s"), sCaption, sText);
		P_ShowLog(sLog);
		MessageBox(sText, sCaption, MB_OK);
		return;
	}
	if(m_mqReadQueue.GetCount() > COMMAND_COUNT)
	{
		MessageBox(_T("�����������������࣬������ִ�д�������Ժ���ִ�д�����"), _T("�ͷſ���"), MB_OK);
		return;
	}
	m_dwStart = GetTickCount();
	ModbusPacket *packet = new ModbusPacket;
	memset(packet, 0, sizeof(ModbusPacket));
	byte ucSize = 4;
	byte *data = NULL;
	packet->nSize = ucSize + 4;
	data = packet->data;
	data[0] = 0x04; //PLC
	data[1] = 0x06; //д�����Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x01;
	data[4] = 0x00; //2�ֽڼĴ������ݣ��ͷ�RS485���ƣ����ⲿ����
	data[5] = 0x00;
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	//m_mqWriteQueue.Push(packet);
	m_mqReadQueue.Push(packet);
	int iCnt = m_mqReadQueue.GetCount();
	if(iCnt == 1) SetEvent(m_hModbusRequestNextEvent);
	CString s, sTmp;
	int i = 0;
	s.Format(_T("[�ͷ�RS485����]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::RollStart() //��ת����
{
	ModbusPacket *packet = NULL;
	byte ucSize = 4;
	byte *data = NULL;
	CString s, sTmp;
	int i = 0;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpRollStart = new ModbusPacket;
	packet = mpRollStart;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = ucSize + 4;
	data = packet->data;
	data[0] = 0x08; //��Ƶ��
	data[1] = 0x06; //д�����Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x08;
	data[4] = 0x00; //2�ֽڼĴ������ݣ���ת��������
	data[5] = 0x02;
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	//m_mqWriteQueue.Push(packet);
	m_mqReadQueue.Push(packet);
	s.Format(_T("[��ת����]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::RollStartWithSpeed(int nIndex) //�Զ���nIndex��ת����
{
	ModbusPacket *packet = NULL;
	int nSize = 4;
	byte *data = NULL;
	CString s, sTmp;
	int nInterval = 0;
	nInterval = m_nInterval[nIndex];

	m_dwStart = GetTickCount();
	ModbusPacket *mpSpeed = new ModbusPacket;
	packet = mpSpeed;
	memset(packet, 0, sizeof(ModbusPacket));
	nSize = 4;
	packet->nSize = nSize + 4;
	data = packet->data;
	data[0] = 0x08; //��Ƶ��
	data[1] = 0x06; //д�����Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x08;
	data[4] = 0x00;
	data[5] = 0x22;
	switch (nIndex)
	{
	case 0:
		data[5] = 0x22; //�Զ���1��ת����
		break;
	case 1:
		data[5] = 0x12; //�Զ���2��ת����
		break;
	case 2:
		data[5] = 0x0A; //�Զ���3��ת����
		break;
	case 3:
		data[5] = 0x32; //�Զ���4��ת����
		break;
	case 4:
		data[5] = 0x2A; //�Զ���5��ת����
		break;
	case 5:
		data[5] = 0x1A; //�Զ���6��ת����
		break;
	case 6:
		data[5] = 0x3A; //�Զ���7��ת����
		break;
	}
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = m_dwAutoDelay * 1000;
	m_mqWriteQueue.Push(packet);
	s.Format(_T("[�Զ���%d��ת����]size=%d dwStart=%u dwDelay=%u "),
		nIndex+1, packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::RollStop() //��תֹͣ
{
	m_dwStart = GetTickCount();
	ModbusPacket *packet = new ModbusPacket;
	memset(packet, 0, sizeof(ModbusPacket));
	byte ucSize = 4;
	byte *data = NULL;
	packet->nSize = ucSize + 4;
	data = packet->data;
	data[0] = 0x08; //��Ƶ��
	data[1] = 0x06; //д�����Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x08;
	data[4] = 0x00; //2�ֽڼĴ������ݣ�ֹͣ��������
	data[5] = 0x01;
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = m_dwAutoDelay * 1000;
	if(m_dwAutoDelay) //�Զ�ֹͣ
		m_mqWriteQueue.Push(packet);
	else //�ֹ�ֹͣ
		m_mqReadQueue.Push(packet);
	CString s, sTmp;
	int i = 0;
	s.Format(_T("[ֹͣ]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReverseStart()  //��ת����
{
	ModbusPacket *packet = NULL;
	byte ucSize = 4;
	byte *data = NULL;
	CString s;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpReverseStart = new ModbusPacket;
	packet = mpReverseStart;
	memset(packet, 0, sizeof(ModbusPacket));
	ucSize = 4;
	packet->nSize = ucSize + 4;
	data = packet->data;
	data[0] = 0x08; //��Ƶ��
	data[1] = 0x06; //д�����Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x08;
	data[4] = 0x00; //2�ֽڼĴ������ݣ���ת��������
	data[5] = 0x04;
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	//m_mqWriteQueue.Push(packet);
	m_mqReadQueue.Push(packet);
	s.Format(_T("[��ת����]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReverseStartWithSpeed(int nIndex) //�Զ���nIndex��ת����
{
	ModbusPacket *packet = NULL;
	int nSize = 4;
	byte *data = NULL;
	CString s;
	int nInterval = 0;
	nInterval = m_nInterval[nIndex];
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpSpeed = new ModbusPacket;
	packet = mpSpeed;
	memset(packet, 0, sizeof(ModbusPacket));
	nSize = 4;
	packet->nSize = nSize + 4;
	data = packet->data;
	data[0] = 0x08; //��Ƶ��
	data[1] = 0x06; //д�����Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x08;
	data[4] = 0x00;
	data[5] = 0x24;
	switch (nIndex)
	{
	case 0:
		data[5] = 0x24; //�Զ���1��ת����
		break;
	case 1:
		data[5] = 0x14; //�Զ���2��ת����
		break;
	case 2:
		data[5] = 0x0C; //�Զ���3��ת����
		break;
	case 3:
		data[5] = 0x34; //�Զ���4��ת����
		break;
	case 4:
		data[5] = 0x2C; //�Զ���5��ת����
		break;
	case 5:
		data[5] = 0x1C; //�Զ���6��ת����
		break;
	case 6:
		data[5] = 0x3C; //�Զ���7��ת����
		break;
	}
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = m_dwAutoDelay * 1000;
	m_mqWriteQueue.Push(packet);
	s.Format(_T("[�Զ���%d��ת����]size=%d dwStart=%u dwDelay=%u "),
		nIndex+1, packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReverseStop() //��תֹͣ
{
	if(m_bRelationPort == FALSE)
	{
		MessageBox(_T("����δ����"), _T("��תֹͣ"), MB_OK);
		return;
	}
	if(m_bUsable != 1)
	{
		MessageBox(_T("�Ѳ�����ʹ�ô�����ˣ����˳�!"), _T("GearCase"), MB_OK);
		return;
	}
	RollStop();
}

void CSmsSenderDlg::OnMenuRollStartIm() 
{
	// TODO: Add your command handler code here
	CString sText, sCaption, sLog;
	if(m_bRelationPort == FALSE)
	{
		MessageBox(_T("����δ����"), _T("��ת����"), MB_OK);
		return;
	}
	if(m_bUsable != 1)
	{
		MessageBox(_T("�Ѳ�����ʹ�ô�����ˣ����˳�!"), _T("GearCase"), MB_OK);
		return;
	}
	if(m_GCInfo.wModeStateOrSet != 0x0004)
	{
		sText.Format(_T("�Ǽ�������ƣ�����ִ�в���"));
		sCaption.Format(_T("��ת����"));
		sLog.Format(_T("[%s]%s"), sCaption, sText);
		P_ShowLog(sLog);
		MessageBox(sText, sCaption, MB_OK);
		return;
	}
	if(m_mqReadQueue.GetCount() > COMMAND_COUNT)
	{
		MessageBox(_T("�����������������࣬������ִ�д�������Ժ���ִ�д�����"), _T("��ת����"), MB_OK);
		return;
	}
	RollStart();
	int iCnt = m_mqReadQueue.GetCount();
	if(iCnt == 1) SetEvent(m_hModbusRequestNextEvent);
}

void CSmsSenderDlg::OnMenuReverseStartIm() 
{
	// TODO: Add your command handler code here
	CString sText, sCaption, sLog;
	if(m_bRelationPort == FALSE)
	{
		MessageBox(_T("����δ����"), _T("��ת����"), MB_OK);
		return;
	}
	if(m_bUsable != 1)
	{
		MessageBox(_T("�Ѳ�����ʹ�ô�����ˣ����˳�!"), _T("GearCase"), MB_OK);
		return;
	}
	if(m_GCInfo.wModeStateOrSet != 0x0004)
	{
		sText.Format(_T("�Ǽ�������ƣ�����ִ�в���"));
		sCaption.Format(_T("��ת����"));
		sLog.Format(_T("[%s]%s"), sCaption, sText);
		P_ShowLog(sLog);
		MessageBox(sText, sCaption, MB_OK);
		return;
	}
	if(m_mqReadQueue.GetCount() > COMMAND_COUNT)
	{
		MessageBox(_T("�����������������࣬������ִ�д�������Ժ���ִ�д�����"), _T("��ת����"), MB_OK);
		return;
	}
	ReverseStart();
}

void CSmsSenderDlg::OnMenuAutoStartIm() 
{
	// TODO: Add your command handler code here
	CString sText, sCaption, sLog;
	if(m_bRelationPort == FALSE)
	{
		MessageBox(_T("����δ����"), _T("�Զ�����"), MB_OK);
		return;
	}
	if(m_bUsable != 1)
	{
		MessageBox(_T("�Ѳ�����ʹ�ô�����ˣ����˳�!"), _T("GearCase"), MB_OK);
		return;
	}
	if(m_GCInfo.wModeStateOrSet != 0x0004)
	{
		sText.Format(_T("�Ǽ�������ƣ�����ִ�в���"));
		sCaption.Format(_T("�Զ�����"));
		sLog.Format(_T("[%s]%s"), sCaption, sText);
		P_ShowLog(sLog);
		MessageBox(sText, sCaption, MB_OK);
		return;
	}
	if(m_bAutoStartIM)
	{
		sText.Format(_T("�Ѿ������Զ�����״̬������ִ�в���"));
		sCaption.Format(_T("�Զ�����"));
		sLog.Format(_T("[%s]%s"), sCaption, sText);
		P_ShowLog(sLog);
		MessageBox(sText, sCaption, MB_OK);
		return;
	}
	if(m_mqWriteQueue.GetCount() > COMMAND_COUNT)
	{
		MessageBox(_T("�����������������࣬������ִ�д�������Ժ���ִ�д�����"), _T("�Զ�����"), MB_OK);
		return;
	}
	m_bAutoStartIM = TRUE;
	OnMenuStartRecord(); //ÿ���Զ�����ʱ���������¼�¼���ݣ����»�ͼ
	int i = 0;
	m_wLastSpeed = 0;
	m_dwAutoDelay = 0;
	for(i=0; i<7; i++)
	{
		if(m_nInterval[i])
		{
			RollStartWithSpeed(i);
			m_wLastSpeed = m_wSpeed[i];
			m_dwAutoDelay += m_nInterval[i];
		}
	}
	RollStop(); //ֹͣ����
	DWORD dwDelay = m_wLastSpeed * 15 / 500; //Time = Speed * 150/5000 ����ֹͣ��������ֹͣ����ʱ��
	m_dwAutoDelay += dwDelay;
	for(i=0; i<7; i++)
	{
		if(m_nInterval[i])
		{
			ReverseStartWithSpeed(i);
			m_dwAutoDelay += m_nInterval[i];
		}
	}
	RollStop(); //ֹͣ����
	m_wLastSpeed = 0;
	m_dwAutoDelay = 0;
}

void CSmsSenderDlg::OnMenuStopIm() 
{
	// TODO: Add your command handler code here
	CString sText, sCaption, sLog;
	if(m_bRelationPort == FALSE)
	{
		MessageBox(_T("����δ����"), _T("ֹͣ"), MB_OK);
		return;
	}
	if(m_bUsable != 1)
	{
		MessageBox(_T("�Ѳ�����ʹ�ô�����ˣ����˳�!"), _T("GearCase"), MB_OK);
		return;
	}
	if(m_GCInfo.wModeStateOrSet != 0x0004)
	{
		sText.Format(_T("�Ǽ�������ƣ�����ִ�в���"));
		sCaption.Format(_T("ֹͣ"));
		sLog.Format(_T("[%s]%s"), sCaption, sText);
		P_ShowLog(sLog);
		MessageBox(sText, sCaption, MB_OK);
		return;
	}
	if(m_mqReadQueue.GetCount() > COMMAND_COUNT)
	{
		MessageBox(_T("�����������������࣬������ִ�д�������Ժ���ִ�д�����"), _T("ֹͣ"), MB_OK);
		return;
	}
	m_mqWriteQueue.DeleteAll();
	RollStop();
	int iCnt = m_mqReadQueue.GetCount();
	if(iCnt == 1) SetEvent(m_hModbusRequestNextEvent);
}

void CSmsSenderDlg::ReadRev() //0x01 ��ת�ٱ�MS5000D
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s;

	m_dwStart = GetTickCount();
	ModbusPacket *mpRev = new ModbusPacket;
	packet = mpRev;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x01; //ת�ٱ�MS5000D
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x10;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡ1·ת������
	data[5] = 0x02; //2���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);

	s.Format(_T("[��ת�ٱ�MS5000D]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadVibration() //0x02 �񶯲���3ͨ�����Ա�MD52U
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	int i = 0;

	m_dwStart = GetTickCount();
	ModbusPacket *mpVibr = new ModbusPacket;
	packet = mpVibr;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x02; //3ͨ�����Ա�MD52U
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x10;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡ3·������
	data[5] = 0x06; //6���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	
	s.Format(_T("[���񶯲���3ͨ�����Ա�MD52U]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadTemperature() //0x03 ���¶�Ѳ����MD52U-8
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	int i = 0;

	m_dwStart = GetTickCount();
	ModbusPacket *mpTemp = new ModbusPacket;
	packet = mpTemp;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x03; //�¶�Ѳ����MD52U-8
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x10;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡ8·�¶�����
	data[5] = 0x10; //16���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	
	s.Format(_T("[���¶�Ѳ����MD52U-8]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadPlcSM() //0x04 ��PLCϵͳģʽ
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	int i = 0;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpPlcSm = new ModbusPacket;
	packet = mpPlcSm;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x04; //PLC
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x00;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡPLC�Ŀ���״̬�Ĵ���SM0������״̬�Ĵ���SM1
	data[5] = 0x02;
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);

	s.Format(_T("[��PLC��SM]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadFrequencyAndOutputCurrent() //0x05 ����Ƶ������Ƶ�ʺ��������2ͨ���������Ա�MB70U
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	int i = 0;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpFreqCur = new ModbusPacket;
	packet = mpFreqCur;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x05; //2ͨ���������Ա�MB70U
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x10;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡ��Ƶ������Ƶ�ʺ������������
	data[5] = 0x04; //4���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);

	s.Format(_T("[����Ƶ������Ƶ�ʺ��������2ͨ���������Ա�MB70U]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadUa() //0x06 ���ۺϵ�����EL2020���ѹUa
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpUa = new ModbusPacket;
	packet = mpUa;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x06; //�ۺϵ�����EL2020
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x00;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡ���ѹUa
	data[5] = 0x01; //1���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	
	s.Format(_T("[���ۺϵ��������ѹUa]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadIa() //0x06 ���ۺϵ�����EL2020 A�����Ia
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpIa = new ModbusPacket;
	packet = mpIa;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x06; //�ۺϵ�����EL2020
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x04;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡA�����Ia
	data[5] = 0x01; //1���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	
	s.Format(_T("[���ۺϵ�����A�����Ia]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadUb() //0x06 ���ۺϵ�����EL2020���ѹUb
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpUa = new ModbusPacket;
	packet = mpUa;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x06; //�ۺϵ�����EL2020
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x10;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡ���ѹUb
	data[5] = 0x01; //1���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	
	s.Format(_T("[���ۺϵ��������ѹUb]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadIb() //0x06 ���ۺϵ�����EL2020 A�����Ib
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpIb = new ModbusPacket;
	packet = mpIb;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x06; //�ۺϵ�����EL2020
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x14;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡB�����Ib
	data[5] = 0x01; //1���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	
	s.Format(_T("[���ۺϵ�����B�����Ib]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadUc() //0x06 ���ۺϵ�����EL2020���ѹUc
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpUc = new ModbusPacket;
	packet = mpUc;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x06; //�ۺϵ�����EL2020
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x20;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡ���ѹUc
	data[5] = 0x01; //1���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	
	s.Format(_T("[���ۺϵ��������ѹUc]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadIc() //0x06 ���ۺϵ�����EL2020 A�����Ic
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpIc = new ModbusPacket;
	packet = mpIc;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x06; //�ۺϵ�����EL2020
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x24;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡC�����Ic
	data[5] = 0x01; //1���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	
	s.Format(_T("[���ۺϵ�����C�����Ic]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadF() //0x06 ���ۺϵ�����EL2020 Ƶ��F
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpF = new ModbusPacket;
	packet = mpF;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x06; //�ۺϵ�����EL2020
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x36;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡƵ��F
	data[5] = 0x01; //1���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	
	s.Format(_T("[���ۺϵ�����Ƶ��F]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadElectricPower() //0x06 ���ۺϵ�����EL2020
{
	/*
	ReadUa(); //���ۺϵ��������ѹUa
	ReadIa(); //���ۺϵ�����A�����Ia
	ReadUb(); //���ۺϵ��������ѹUb
	ReadIb(); //���ۺϵ�����B�����Ib
	ReadUc(); //���ۺϵ��������ѹUc
	ReadIc(); //���ۺϵ�����C�����Ic
	ReadF(); //���ۺϵ�����Ƶ��F
	*/

	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	int i = 0;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpPlcSm = new ModbusPacket;
	packet = mpPlcSm;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x06; //�ۺϵ�����EL2020
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x00;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡ�ۺϵ�����EL2020
	data[5] = 0x1C; //28���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	
	s.Format(_T("[���ۺϵ�����EL2020]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadControlStatus() //0x08 ����Ƶ��ģʽ����״̬
{
	ModbusPacket *packet = NULL;
	byte ucSize = 4;
	byte *data = NULL;
	CString s, sTmp;
	int i = 0;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpStatus = new ModbusPacket;
	packet = mpStatus;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x08; //��Ƶ��
	data[1] = 0x03; //��1���Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x08;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡ��Ƶ������״̬
	data[5] = 0x02; //1���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);

	s.Format(_T("[����Ƶ������״̬]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadOutputFIU() //0x08 ����Ƶ�����Ƶ�ʡ���������������ѹ����
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	int i = 0;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpFiu = new ModbusPacket;
	packet = mpFiu;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x08; //��Ƶ��
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0xC8;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡ���Ƶ�ʡ���������������ѹ����
	data[5] = 0x03; //3���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	s.Format(_T("[����Ƶ�����Ƶ�ʡ���������������ѹ]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadRunTimeFS() //0x08 ����Ƶ�����Ƶ���趨ֵ�������ٶ�����
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	int i = 0;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpFs = new ModbusPacket;
	packet = mpFs;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x08; //��Ƶ��
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0xCC;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡƵ���趨ֵ�������ٶ�����
	data[5] = 0x02; //2���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];

	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	s.Format(_T("[����Ƶ�����Ƶ���趨ֵ�������ٶ�]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::Read485F() //����Ƶ��485Ƶ���趨ֵ
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	int i = 0;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpFs = new ModbusPacket;
	packet = mpFs;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x08; //��Ƶ��
	data[1] = 0x03; //��1���Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x0D;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡ485Ƶ���趨ֵ
	data[5] = 0x01; //1���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	s.Format(_T("[����Ƶ��485Ƶ���趨ֵ�������ٶ�]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadLoadRateMaxUIAndPower() //0x08 ����Ƶ�������������ʡ����������ֵ�������ѹ��ֵ�����빦�ʡ������������
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	int i = 0;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpRuip = new ModbusPacket;
	packet = mpRuip;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x08; //��Ƶ��
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0xD1;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡ�����������ʡ����������ֵ�������ѹ��ֵ�����빦�ʡ������������
	data[5] = 0x05; //5���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	s.Format(_T("[����Ƶ�������������ʡ����������ֵ�������ѹ��ֵ�����빦�ʡ��������]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadOutputP() //0x08 ����Ƶ���������
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	int i = 0;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpOutput = new ModbusPacket;
	packet = mpOutput;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x08; //��Ƶ��
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0xD5;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡ�����������
	data[5] = 0x01; //1���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	s.Format(_T("[����Ƶ���������]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadTotalRunTime() //0x08 ����Ƶ���ۼ�ͨ��ʱ������
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	int i = 0;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpTrt = new ModbusPacket;
	packet = mpTrt;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x08; //��Ƶ��
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0xDB;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡ�ۼ�ͨ��ʱ������
	data[5] = 0x01; //1���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	s.Format(_T("[����Ƶ���ۼ�ͨ��ʱ��]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadRealRunTime() //0x08 ����Ƶ��ʵ������ʱ������
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	int i = 0;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpRrt = new ModbusPacket;
	packet = mpRrt;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x08; //��Ƶ��
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0xDE;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡʵ������ʱ������
	data[5] = 0x01; //1���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	s.Format(_T("[����Ƶ��ʵ������ʱ��]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadTotalQ() //0x08 ����Ƶ���ۼ��õ�������
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	int i = 0;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpTransducer6 = new ModbusPacket;
	packet = mpTransducer6;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x08; //��Ƶ��
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0xE0;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡ�ۼ��õ�������
	data[5] = 0x01; //1���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	s.Format(_T("[����Ƶ���ۼ��õ���]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadTransducer() //0x08 ����Ƶ����Ϣ
{
	ReadControlStatus(); //����Ƶ��ģʽ����״̬
	ReadOutputFIU(); //��ȡ���Ƶ�ʡ���������������ѹ����
	ReadRunTimeFS(); //��ȡƵ���趨ֵ�������ٶ�����
	Read485F(); //����Ƶ��485Ƶ���趨ֵ
	ReadLoadRateMaxUIAndPower(); //��ȡ�����������ʡ����������ֵ�������ѹ��ֵ�����빦�ʡ������������
	//ReadTotalRunTime(); //��ȡ�ۼ�ͨ��ʱ������
	//ReadRealRunTime(); //��ȡʵ������ʱ������
	//ReadTotalQ(); //��ȡ�ۼ��õ�������
	ReadMultiSpeeds(); //����Ƶ����ζ���
}

void CSmsSenderDlg::ReadMultiSpeeds() //����Ƶ����ζ���
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s;
	int i = 0;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpSpeed1 = new ModbusPacket;
	packet = mpSpeed1;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x08; //��Ƶ��
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x03; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0xEB;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡ����1������2������3
	data[5] = 0x03; //3���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	s.Format(_T("[����Ƶ����ȡ��ζ���1-3]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
	
	
	ModbusPacket *mpSpeed2= new ModbusPacket;
	packet = mpSpeed2;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x08; //��Ƶ��
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x03; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0xFF;
	data[4] = 0x00; //2�ֽڼĴ�����������ȡ����4������5������6������7
	data[5] = 0x04; //3���Ĵ���
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	s.Format(_T("[����Ƶ����ȡ��ζ���4-7]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

void CSmsSenderDlg::ReadUsable() //����Ƶ������Ƿ����
{
	ModbusPacket *packet = NULL;
	byte *data = NULL;
	CString s, sTmp;
	int i = 0;
	
	m_dwStart = GetTickCount();
	ModbusPacket *mpUsable = new ModbusPacket;
	packet = mpUsable;
	memset(packet, 0, sizeof(ModbusPacket));
	packet->nSize = 8;
	data = packet->data;
	data[0] = 0x08; //��Ƶ��
	data[1] = 0x03; //������Ĵ���
	data[2] = 0x07; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x5F;
	data[4] = 0x00; //2�ֽڼĴ���������������Ƿ����
	data[5] = 0x02; //2���Ĵ���
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	m_mqReadQueue.Push(packet);
	s.Format(_T("[����Ƶ�������ж�����Ƿ����]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	s = s + Data2String(packet->data, packet->nSize);
	P_ShowLog(s);
}

LRESULT CSmsSenderDlg::OnRefreshGearCaseInfo(WPARAM wParam, LPARAM lParam)
{
	BYTE ucAddr = (BYTE)wParam;
	int iBegin = LOWORD(lParam);
	int iEnd = HIWORD(lParam);
	CString sValue;
	int i = 0, inter = 0, fraction =0;
	if(ucAddr == 0x01)
	{
		//ת�� (0)
		i = iBegin;
		sValue.Format(_T("%.02f"), m_GCInfo.fRev);
		m_l1.SetItemText(i, 1, sValue);
		if(m_bStorage)
			WriteStorageFile(_T("Rev"), &(m_stcDraw.m_lineRev));
	}
	// (1)
	if(ucAddr == 0x02)
	{
		//�� (2-4)
		for(i=iBegin; i<=iEnd; i++)
		{
			sValue.Format(_T("%.02f"), m_GCInfo.fVibration[i-iBegin]);
			m_l1.SetItemText(i, 1, sValue);
			CString strName;
			strName.Format(_T("Vibration%d"), i-iBegin);
			if(m_bStorage)
				WriteStorageFile(strName, &(m_stcDraw.m_lineVibration[i-iBegin]));
		}
	}
	// (5)
	if(ucAddr == 0x03)
	{
		//�¶� (6-13)
		for(i=iBegin; i<=iEnd; i++)
		{
			sValue.Format(_T("%.02f"), m_GCInfo.fTemperature[i-iBegin]);
			m_l1.SetItemText(i, 1, sValue);
			CString strName;
			strName.Format(_T("Temperature%d"), i-iBegin);
			if(m_bStorage)
				WriteStorageFile(strName, &(m_stcDraw.m_lineTemperature[i-iBegin]));
		}
	}
	//(14)
// 	if(ucAddr == 0x04)
// 	{
// 		//������ϵͳ״̬
// 		WORD wSM0 = m_GCInfo.wSM0;
// 		CString sMss;
// 		sMss = _T("PU����");
// 		if(wSM0 == 0x01)
// 			sMss = _T("���������"); //RS485����
// 		else if(wSM0 == 0x02)
// 			sMss = _T("PU����"); //������
// 		else if(wSM0 == 0x03)
// 			sMss = _T("�ⲿ����");
// 		sValue.Format(_T("%s"), sMss);
// 		m_l1.SetItemText(33, 1, sValue);
// 		
// 		//�������״̬
// 		WORD wSM1 = m_GCInfo.wSM1;
// 		CString sRev;
// 		sRev = _T("����");
// 		if(wSM1 == 0x00)
// 			sRev = _T("����");
// 		else if(wSM1 == 0x01)
// 			sRev = _T("����");
// 		sValue.Format(_T("%s"), sRev);
// 		m_l1.SetItemText(34, 1, sValue);
// 	}
	if(ucAddr == 0x05)
	{
		//����Ƶ�� (15)
		i = iBegin;
		sValue.Format(_T("%.02f"), m_GCInfo.fFrequency);
		m_l1.SetItemText(i, 1, sValue);

		//������� (16)
		i = iBegin+1;
		sValue.Format(_T("%.02f"), m_GCInfo.fOutputCurrent);
		m_l1.SetItemText(i, 1, sValue);
	}
	if(ucAddr == 0x06)
	{
		//A���ѹUa (24)
		i = iBegin;
		inter = m_GCInfo.wUa/100;
		fraction = m_GCInfo.wUa%100;
		sValue.Format(_T("%d.%02d"), inter, fraction);
		m_l1.SetItemText(i++, 1, sValue);

		//B���ѹUb (25)
		inter = m_GCInfo.wUb/100;
		fraction = m_GCInfo.wUb%100;
		sValue.Format(_T("%d.%02d"), inter, fraction);
		m_l1.SetItemText(i++, 1, sValue);
		
		//C���ѹUc (26)
		inter = m_GCInfo.wUc/100;
		fraction = m_GCInfo.wUc%100;
		sValue.Format(_T("%d.%02d"), inter, fraction);
		m_l1.SetItemText(i++, 1, sValue);

		//A�����Ia (27)
		inter = m_GCInfo.wIa/10000;
		fraction = m_GCInfo.wIa%10000;
		sValue.Format(_T("%d.%02d"), inter, fraction);
		m_l1.SetItemText(i++, 1, sValue);

		//B�����Ib (28)
		inter = m_GCInfo.wIb/10000;
		fraction = m_GCInfo.wIb%10000;
		sValue.Format(_T("%d.%02d"), inter, fraction);
		m_l1.SetItemText(i++, 1, sValue);

		//C�����Ic (29)
		inter = m_GCInfo.wIc/10000;
		fraction = m_GCInfo.wIc%10000;
		sValue.Format(_T("%d.%02d"), inter, fraction);
		m_l1.SetItemText(i++, 1, sValue);

		//��Ƶ(����Ƶ��) (30)
		inter = (m_GCInfo.wF*1.068)/1000;
		fraction = (int)(m_GCInfo.wF*1.068) % 1000;
		sValue.Format(_T("%d.%02d"), inter, fraction);
		m_l1.SetItemText(i++, 1, sValue);
	}

	if(ucAddr == 0x08)
	{	
		//ת�� (17)
		sValue.Format(_T("%.02f"), m_GCInfo.fTorque);
		m_l1.SetItemText(17, 1, sValue);
		if(m_bStorage)
			WriteStorageFile(_T("Torque"), &(m_stcDraw.m_lineTorque));

		i = iBegin;
// 		//���Ƶ��
// 		inter = m_GCInfo.wOutputF/100;
// 		fraction = m_GCInfo.wOutputF%100;
// 		sValue.Format(_T("%d.%02d"), inter, fraction);
// 		m_l1.SetItemText(i++, 1, sValue);

// 		//�������
// 		inter = m_GCInfo.wOutputI/100;
// 		fraction = m_GCInfo.wOutputI%100;
// 		sValue.Format(_T("%d.%02d"), inter, fraction);
// 		m_l1.SetItemText(i++, 1, sValue);

// 		//�����ѹ
// 		inter = m_GCInfo.wOutputU/100;
// 		fraction = m_GCInfo.wOutputU%100;
// 		sValue.Format(_T("%d.%02d"), inter, fraction);
// 		m_l1.SetItemText(i++, 1, sValue);

		//������ת���趨ֵ (18)
		if(iBegin == 18)
		{
			sValue.Format(_T("%d"), m_GCInfo.wFSetValue);
			m_l1.SetItemText(i++, 1, sValue);
		}

		if(iBegin == 19)
		{
			//485���ת���趨ֵ (19)
			sValue.Format(_T("%d"), m_wRunSpeed);
			m_l1.SetItemText(i++, 1, sValue);
		}
	
// 		//�����ٶ�
// 		inter = m_GCInfo.wSpeed/100;
// 		fraction = m_GCInfo.wSpeed%100;
// 		sValue.Format(_T("%d.%02d"), inter, fraction);
// 		m_l1.SetItemText(i++, 1, sValue);
		
// 		//������������
// 		inter = m_GCInfo.wOverILoadRate/100;
// 		fraction = m_GCInfo.wOverILoadRate%100;
// 		sValue.Format(_T("%d.%02d"), inter, fraction);
// 		m_l1.SetItemText(i++, 1, sValue);
		if(20 <=iBegin && iEnd<=23)
		{
			//���������ֵ (20)
			inter = m_GCInfo.wOutputIMax/100;
			fraction = m_GCInfo.wOutputIMax%100;
			sValue.Format(_T("%d.%02d"), inter, fraction);
			m_l1.SetItemText(i++, 1, sValue);
			
			//�����ѹ��ֵ (21)
			inter = m_GCInfo.wOutputUMax/100;
			fraction = m_GCInfo.wOutputUMax%100;
			sValue.Format(_T("%d.%02d"), inter, fraction);
			m_l1.SetItemText(i++, 1, sValue);
			
			//���빦�� (22)
			inter = m_GCInfo.wInputP/100;
			fraction = m_GCInfo.wInputP%100;
			sValue.Format(_T("%d.%02d"), inter, fraction);
			m_l1.SetItemText(i++, 1, sValue);
			
			//������� (23)
			inter = m_GCInfo.wOutputP/100;
			fraction = m_GCInfo.wOutputP%100;
			sValue.Format(_T("%d.%02d"), inter, fraction);
			m_l1.SetItemText(i++, 1, sValue);
		}
	
// 		//�ۼ�ͨ��ʱ��
// 		inter = m_GCInfo.wTotalRunTime/100;
// 		fraction = m_GCInfo.wTotalRunTime%100;
// 		sValue.Format(_T("%d.%02d"), inter, fraction);
// 		m_l1.SetItemText(i++, 1, sValue);
		
// 		//ʵ������ʱ��
// 		inter = m_GCInfo.wRealRunTime/100;
// 		fraction = m_GCInfo.wRealRunTime%100;
// 		sValue.Format(_T("%d.%02d"), inter, fraction);
// 		m_l1.SetItemText(i++, 1, sValue);
		
// 		//�ۼ��õ���
// 		inter = m_GCInfo.wTotalQ/100;
// 		fraction = m_GCInfo.wTotalQ%100;
// 		sValue.Format(_T("%d.%02d"), inter, fraction);
// 		m_l1.SetItemText(i++, 1, sValue);

		//��Ƶ������1--����7
		if(32<=iBegin && iEnd<=38)
		{
			for(i=iBegin; i<=iEnd; i++) //(32-38)
			{
				sValue.Format(_T("%d"), m_wSpeed[i-32]);
				m_l1.SetItemText(i, 1, sValue);
			}
		}

		if(iBegin == 40)
		{
			//������ϵͳ״̬
			WORD wMSS = m_GCInfo.wModeStateOrSet;
			CString sMss;
			sMss = _T("PU����"); //������
			if(wMSS == 0x0004)
				sMss = _T("���������"); //RS485����
			else if(wMSS == 0x0000)
				sMss = _T("�ⲿ����");
			sValue.Format(_T("%s"), sMss);
			m_l1.SetItemText(40, 1, sValue);
			
			//�������״̬
			WORD wSC = m_GCInfo.wStateOrCtrl;
			CString sSC;
			sSC = _T("����");
			if(wSC & 0x0002)
				sSC = _T("��ת����");
			else if(wSC & 0x0004)
				sSC = _T("��ת����");
			else if(wSC & 0x0100) //���ֹ���
			{
				WORD wTrouble = wSC & 0x00F0;
				wTrouble = wTrouble >> 4;
				sSC.Format(_T("���Ϻ�: %d"), wTrouble);
			}
			sValue.Format(_T("%s"), sSC);
			m_l1.SetItemText(41, 1, sValue);
		}
	}
	return 0;
}

LRESULT CSmsSenderDlg::OnVibrationAlarm(WPARAM wParam, LPARAM lParam)  //�񶯱���
{
	int nIndex = LOWORD(wParam);
	float fVibration = (float)lParam;
	nIndex = 0;
	CString sAlarm;
	//sAlarm.Format(_T("��%d·��(%.02f)����"), nIndex, m_GCInfo.fVibration[nIndex]);
	sAlarm.Format(_T("��%d·��(%.02f)����"), nIndex, fVibration);
	P_ShowLog(sAlarm);

	m_dwStart = GetTickCount();
	ModbusPacket *packet = new ModbusPacket;
	memset(packet, 0, sizeof(ModbusPacket));
	byte ucSize = 4;
	byte *data = NULL;
	packet->nSize = ucSize + 4;
	data = packet->data;
	data[0] = 0x04; //PLC
	data[1] = 0x06; //д�����Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x02;
	data[4] = 0x00; //2�ֽڼĴ������ݣ�����RS485����
	data[5] = 0x00;
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	//m_mqWriteQueue.Push(packet);
	m_mqReadQueue.Push(packet);
	CString s, sTmp;
	int i = 0;
	s.Format(_T("[Ҫ��PLC����(��)����]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	sTmp = Data2String(data, packet->nSize);
	s = s + sTmp;
	P_ShowLog(s);

	return 0;
}

LRESULT CSmsSenderDlg::OnTemperatureAlarm(WPARAM wParam, LPARAM lParam)
{
	int nIndex = LOWORD(wParam);
	float fTemperature = (float)lParam;
	nIndex = 0;
	CString sAlarm;
	//sAlarm.Format(_T("��%d·�¶�(%.02f)����"), nIndex, m_GCInfo.fTemperature[nIndex]);
	sAlarm.Format(_T("��%d·����(%.02f)����"), nIndex, fTemperature);
	P_ShowLog(sAlarm);

	m_dwStart = GetTickCount();
	ModbusPacket *packet = new ModbusPacket;
	memset(packet, 0, sizeof(ModbusPacket));
	byte ucSize = 4;
	byte *data = NULL;
	packet->nSize = ucSize + 4;
	data = packet->data;
	data[0] = 0x04; //PLC
	data[1] = 0x06; //д�����Ĵ���
	data[2] = 0x00; //2�ֽ�ƫ�Ƶ�ַ
	data[3] = 0x02;
	data[4] = 0x00; //2�ֽڼĴ������ݣ�����RS485����
	data[5] = 0x00;
	unsigned short usCrc16 = 0;
	usCrc16 = FormCrc16((unsigned char *)data, 6);
	data[6] = ((char*)&usCrc16)[0];
	data[7] = ((char*)&usCrc16)[1];
	packet->dwStart = m_dwStart;
	packet->dwDelay = 0;
	//m_mqWriteQueue.Push(packet);
	m_mqReadQueue.Push(packet);
	CString s, sTmp;
	int i = 0;
	s.Format(_T("[Ҫ��PLC����(����)����]size=%d dwStart=%u dwDelay=%u "),
		packet->nSize, packet->dwStart, packet->dwDelay);
	sTmp = Data2String(data, packet->nSize);
	s = s + sTmp;
	P_ShowLog(s);
	return 0;
}

LRESULT CSmsSenderDlg::OnUsable(WPARAM wParam, LPARAM lParam)
{
	m_bUsable = wParam;
	if(m_bUsable != 1)
	{
		KillTimer(1);
		MessageBox(_T("�Ѳ�����ʹ�ô�����ˣ����˳�!"), _T("GearCase"), MB_OK);
	}
	return 0;
}

unsigned short CSmsSenderDlg::FormCrc16(unsigned char * startaddress, unsigned char bytecount)
{
	unsigned char i, j ;
	unsigned short int crc16value = 0xffff;
	unsigned short int polynomial = 0xa001;
	for ( i = 0 ; i < bytecount ; i ++ )
	{
		crc16value = crc16value ^ ( * ( startaddress + i ) ) ;
		for ( j = 0 ; j < 8 ; j ++ )
		{
			if ( ( crc16value & 0x0001 ) == 0x00001 )
				crc16value = ( crc16value >> 1 ) ^ polynomial ;
			else
				crc16value = crc16value >> 1 ;
		}
	}
	return  crc16value ;
}

CString CSmsSenderDlg::Data2String(byte *ucData, int nSize)
{
	int i = 0;
	CString sRet, sTmp;
	for(i=0; i<nSize; i++)
	{
		sTmp.Format(_T("%02X "), ucData[i]);
		sRet = sRet + sTmp;
	}
	return sRet;
}

void CSmsSenderDlg::OnMenuStartRecord() 
{
	// TODO: Add your command handler code here
	int i = 0;
	m_bStorage = FALSE;
	//m_stcDraw.m_lineRev.m_iCount = 0;
	m_stcDraw.m_lineRev.DeleteAllPoint();
	for(i=0; i<3; i++)
		//m_stcDraw.m_lineVibration[i].m_iCount = 0;
		m_stcDraw.m_lineVibration[i].DeleteAllPoint();
	for(i=0; i<8; i++)
		//m_stcDraw.m_lineTemperature[i].m_iCount = 0;
		m_stcDraw.m_lineTemperature[i].DeleteAllPoint();
	//m_stcDraw.m_lineTorque.m_iCount = 0;
	m_stcDraw.m_lineTorque.DeleteAllPoint();
	m_bStorage = TRUE;
	SaveUserToFile();
}

void CSmsSenderDlg::OnMenuStopRecord() 
{
	// TODO: Add your command handler code here
	m_bStorage = FALSE;
}

void CSmsSenderDlg::OnMenuPasswdset() 
{
	// TODO: Add your command handler code here
	CPasswordSetDlg dlg;
	dlg.DoModal();
}

void CSmsSenderDlg::OnMenuSpeeddownset() 
{
	// TODO: Add your command handler code here
	CSpeedDownSetDlg dlg;
	dlg.m_fSpeedDownRate = m_fSpeedDownRate;
	int iRet = dlg.DoModal();
	if(iRet == IDOK)
	{
		//Torque
		CLineEx *line = NULL;
		line = &(m_stcDraw.m_lineTorque);
		long fValue = floor(802.50/(10*m_fSpeedDownRate))*10;
		line->m_fMinValue = -fValue; //-120.00
		line->m_fMaxValue = fValue;
		m_stcDraw.CalcLayout();
	}
}
