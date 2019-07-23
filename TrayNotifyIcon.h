// TrayNotifyIcon.h: interface for the CTrayNotifyIcon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRAYNOTIFYICON_H__740A4E80_804C_11D5_B614_00B0D07AF514__INCLUDED_)
#define AFX_TRAYNOTIFYICON_H__740A4E80_804C_11D5_B614_00B0D07AF514__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//����CTrayTimerWnd��,�Ա�CTrayNotifyIcon��ʶ��.
class CTrayTimerWnd;

//����CTrayNotifyIcon��
class CTrayNotifyIcon : public CObject  
{
public:
	CTrayNotifyIcon();
	virtual ~CTrayNotifyIcon();

	BOOL IsShowing() const { return !IsHidden(); }
	BOOL IsHidden() const { return m_bHidden; }
	BOOL UsingAnimatedIcon() const { return m_bAnimated; }
	
public:
	BOOL m_bIsWindowNormal;

protected:
	
	BOOL m_bAnimated;
	CTrayTimerWnd* m_pTimerWnd;
	CWnd* m_pNotificationWnd;
	BOOL m_bHidden;
	BOOL m_bCreated;
	NOTIFYICONDATA m_NotifyIconData;
	void DestroyTimerWindow();
	
//����CTrayTimerWnd��Ϊ��Ԫ��
public:
	LRESULT OnTrayNotification(WPARAM wID,LPARAM lEvent);
	void ShowIcon();
	BOOL SetNotificationWnd(CWnd* pNotifyWnd);
	BOOL SetStandardIcon(UINT nIDResource);
	BOOL SetStandardIcon(LPCTSTR lpIconName);
	BOOL SetIcon(HICON* phIcons,int nNumIcons, DWORD dwDelay);
	BOOL SetIcon(LPCTSTR lpIconName);
	BOOL SetIcon(HICON hIcon);
	void RemoveIcon();
	void MoveToExtremeRight();
	BOOL IsAnimated();
	void HideIcon();
	BOOL SetTooltipText(UINT nID);
	BOOL SetTooltipText(LPCTSTR pszTooltipText);
	CString GetTooltipText() const;
	CWnd* GetNotificationWnd() const;
	HICON GetIcon() const;

	BOOL CreateTimerWindow(HICON* phIcons, int nNumIcons, DWORD dwDelay);
	BOOL Create(CWnd* pNotifyWnd, UINT uID, LPCTSTR pszTooltipText, HICON* phIcons, int nNumIcons, DWORD dwDelay, UINT nNotifyMessage);
	BOOL Create(CWnd* pNotifyWnd, UINT nID, LPCTSTR pszTooltipText, HICON hIcon, UINT nNotifyMessage);
	
	friend class CTrayTimerWnd;
	
	DECLARE_DYNAMIC(CTrayNotifyIcon)

};

//��̬����CTrayNotifyIcon��Ĺؼ��궨��

#endif // !defined(AFX_TRAYNOTIFYICON_H__740A4E80_804C_11D5_B614_00B0D07AF514__INCLUDED_)

//��NOTIFYICONDATA�ṹ���������������,�Ա����
/*
typedef struct _NOTIFYICONDATA
{
	DWORD	cbSize;				//��λ����ýṹ�洢�����С
	HWND	hWnd;				//��������,ICON��Ϣ���͵Ĵ�����
	UINT	uID;				//Ӧ�ó������ICON��ID
	UINT	uFlags;				//��־λ
								//NIF_ICON:		hIcon��Ա���Է���
								//NIF_MESSAGE:	uCallbackMessage������Ч
								//NIF_TIP:		szTip������Ч
	UINT	uCallbackMessage;	//�û��������ϢID
	HICON	hIcon;				//���,ɾ��,�޸ĵ�ͼ����
	char	szTip[64];			//���ָ��ICONʱ��ʾ������˵��
}NOTIFYICONDATA,*PNOTIFYICONDATA;
*/