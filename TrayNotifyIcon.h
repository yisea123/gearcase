// TrayNotifyIcon.h: interface for the CTrayNotifyIcon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRAYNOTIFYICON_H__740A4E80_804C_11D5_B614_00B0D07AF514__INCLUDED_)
#define AFX_TRAYNOTIFYICON_H__740A4E80_804C_11D5_B614_00B0D07AF514__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//声明CTrayTimerWnd类,以备CTrayNotifyIcon类识别.
class CTrayTimerWnd;

//声明CTrayNotifyIcon类
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
	
//声明CTrayTimerWnd类为友元类
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

//动态声明CTrayNotifyIcon类的关键宏定义

#endif // !defined(AFX_TRAYNOTIFYICON_H__740A4E80_804C_11D5_B614_00B0D07AF514__INCLUDED_)

//将NOTIFYICONDATA结构及其意义表述如下,以备后查
/*
typedef struct _NOTIFYICONDATA
{
	DWORD	cbSize;				//按位计算该结构存储对象大小
	HWND	hWnd;				//任务栏中,ICON消息发送的窗体句柄
	UINT	uID;				//应用程序定义的ICON的ID
	UINT	uFlags;				//标志位
								//NIF_ICON:		hIcon成员可以访问
								//NIF_MESSAGE:	uCallbackMessage参数有效
								//NIF_TIP:		szTip参数有效
	UINT	uCallbackMessage;	//用户定义的消息ID
	HICON	hIcon;				//添加,删除,修改的图标句柄
	char	szTip[64];			//鼠标指向ICON时显示的文字说明
}NOTIFYICONDATA,*PNOTIFYICONDATA;
*/