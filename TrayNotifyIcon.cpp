// TrayNotifyIcon.cpp: implementation of the CTrayNotifyIcon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TrayNotifyIcon.h"
#include "TrayTimerWnd.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//动态加载CTrayNotifyIcon类的动态关键宏定义
IMPLEMENT_DYNAMIC(CTrayNotifyIcon,CObject)

//构造函数
CTrayNotifyIcon::CTrayNotifyIcon()
{
	//初始化各种数据

	//设置m_NotifyIconData中的数据为0
	memset(&m_NotifyIconData,0,sizeof(m_NotifyIconData));

	m_bCreated=FALSE;
	m_bHidden=FALSE;
	m_pNotificationWnd=NULL;
	m_pTimerWnd=NULL;
	m_bAnimated=FALSE;

}

//析构函数
CTrayNotifyIcon::~CTrayNotifyIcon()
{
	DestroyTimerWindow();
	RemoveIcon();
}

//创建托盘的图标
//pNotifyWnd指针指向窗体CMainFrame
BOOL CTrayNotifyIcon::Create(CWnd *pNotifyWnd, UINT uID, LPCTSTR pszTooltipText, HICON hIcon, UINT nNotifyMessage)
{
	//确保pNotifyWnd指向的窗体有效
	ASSERT(pNotifyWnd && ::IsWindow(pNotifyWnd->GetSafeHwnd()));
	m_pNotificationWnd=pNotifyWnd;

	//nNotifyMessage包含的消息是否超过了Windows定义的用户消息
	ASSERT(nNotifyMessage>=WM_USER);

	//托盘中的图标提示信息最多包含64个字符
	ASSERT(_tcslen(pszTooltipText)<=64);

	//设置必要的NOTIFYICONDATA结构的数据
	m_NotifyIconData.cbSize=sizeof(m_NotifyIconData);
	m_NotifyIconData.hWnd=pNotifyWnd->GetSafeHwnd();
	m_NotifyIconData.uID=uID;
	m_NotifyIconData.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
	m_NotifyIconData.uCallbackMessage=nNotifyMessage;
	m_NotifyIconData.hIcon=hIcon;
	_tcscpy(m_NotifyIconData.szTip,pszTooltipText);

	BOOL rVal=Shell_NotifyIcon(NIM_ADD,&m_NotifyIconData);
	m_bCreated=rVal;

	return rVal;
}

//重载函数Create(...)
//创建托盘的图标
//加入了时间和动画的处理
BOOL CTrayNotifyIcon::Create(CWnd *pNotifyWnd, UINT uID, LPCTSTR pszTooltipText, HICON *phIcons, int nNumIcons, DWORD dwDelay, UINT nNotifyMessage)
{
	//窗体的显示是否正常
	//m_bIsWindowNormal=true;

	//涉及到动画演示,至少必须有2个图标
	ASSERT(nNumIcons>=2);

	//创建时间触发器窗体,该窗体中封装了任务栏动画的代码
	if(!CreateTimerWindow(phIcons,nNumIcons,dwDelay))
		return FALSE;

	//使用重载的Create()函数来创建任务栏图标
	//该创建的任务栏初始图标使用的是图标数组中的第一个
	BOOL bSuccess=Create(pNotifyWnd,uID,pszTooltipText,phIcons[0],nNotifyMessage);
	m_bAnimated=TRUE;

	return bSuccess;
}

BOOL CTrayNotifyIcon::CreateTimerWindow(HICON *phIcons, int nNumIcons, DWORD dwDelay)
{
	//创建包含CTimer计时器的隐式窗口
	ASSERT(m_pTimerWnd==NULL);
	m_pTimerWnd=new CTrayTimerWnd(this,phIcons,nNumIcons,dwDelay);

	if(!m_pTimerWnd)
		return FALSE;

	//调用CWnd类的缺省函数Create()
	if(!m_pTimerWnd->Create(NULL,_T("")))
		return FALSE;	

	return TRUE;
}

void CTrayNotifyIcon::DestroyTimerWindow()
{
	//如果存在的话,删除计时器窗体
	if(m_pTimerWnd)
	{
		m_pTimerWnd->SendMessage(WM_CLOSE);
		m_pTimerWnd=NULL;
	}
}

//得到当前使用的ICON句柄
HICON CTrayNotifyIcon::GetIcon() const
{
	HICON hIcon=NULL;
	if(m_bCreated)
	{
		if(m_bAnimated)
			hIcon=m_pTimerWnd->GetCurrentIcon();
		else
			hIcon=m_NotifyIconData.hIcon;
	}

	return hIcon;
}

//得到接收Notify消息的窗体
CWnd* CTrayNotifyIcon::GetNotificationWnd() const
{
	//返回CTrayNotifyIcon类中的CWnd成员函数
	return m_pNotificationWnd;
}

//得到CToolTip显示的提示文字
CString CTrayNotifyIcon::GetTooltipText() const
{
	CString sText;
	if(m_bCreated)
		sText=m_NotifyIconData.szTip;

	return sText;
}

//设置提示文字
BOOL CTrayNotifyIcon::SetTooltipText(LPCTSTR pszTooltipText)
{
	//如果没有创建,则退出
	if(!m_bCreated)
		return FALSE;

	m_bIsWindowNormal=true;
	m_NotifyIconData.uFlags=NIF_TIP;
	_tcscpy(m_NotifyIconData.szTip,pszTooltipText);

	//用Shell_NotifyIcon刷新显示
	return Shell_NotifyIcon(NIM_MODIFY,&m_NotifyIconData);
}

//用uID设置提示文字
BOOL CTrayNotifyIcon::SetTooltipText(UINT nID)
{
	CString sToolTipText;
	VERIFY(sToolTipText.LoadString(nID));

	return SetTooltipText(sToolTipText);
}

//隐藏图标
void CTrayNotifyIcon::HideIcon()
{
	ASSERT(m_bCreated);
	if(!m_bHidden)
	{
		m_NotifyIconData.uFlags=NIF_ICON;
		Shell_NotifyIcon(NIM_DELETE,&m_NotifyIconData);
		m_bHidden=TRUE;
	}
}

//得到是否动画显示
BOOL CTrayNotifyIcon::IsAnimated()
{
	return m_bAnimated;
}

//刷新托盘中的图标,可纠错
void CTrayNotifyIcon::MoveToExtremeRight()
{
	HideIcon();
	ShowIcon();
}

//移除图标
void CTrayNotifyIcon::RemoveIcon()
{
	if(m_bCreated)
	{
		m_NotifyIconData.uFlags=0;
		Shell_NotifyIcon(NIM_DELETE,&m_NotifyIconData);
		m_bCreated=FALSE;
	}
}

//SetIcon(HICON hIcon)
BOOL CTrayNotifyIcon::SetIcon(HICON hIcon)
{
	if(!m_bCreated)
		return FALSE;

	DestroyTimerWindow();
	m_bAnimated=FALSE;
	m_NotifyIconData.uFlags=NIF_ICON;
	m_NotifyIconData.hIcon=hIcon;

	return Shell_NotifyIcon(NIM_MODIFY,&m_NotifyIconData);
}

//SetIcon(LPCTSTR lpIconName)
BOOL CTrayNotifyIcon::SetIcon(LPCTSTR lpIconName)
{
	HICON hIcon=AfxGetApp()->LoadIcon(lpIconName);
	return SetIcon(hIcon);
}

//SetIcon(HICON *phIcons, int nNumIcons, DWORD dwDelay)
BOOL CTrayNotifyIcon::SetIcon(HICON *phIcons, int nNumIcons, DWORD dwDelay)
{
	ASSERT(nNumIcons>=2);
	ASSERT(phIcons);

	if(!SetIcon(phIcons[0]))
		return FALSE;

	DestroyTimerWindow();
	if(!CreateTimerWindow(phIcons,nNumIcons,dwDelay))
		return FALSE;
	m_bAnimated=TRUE;

	return TRUE;

}

//SetStandardIcon(LPCTSTR lpIconName)
BOOL CTrayNotifyIcon::SetStandardIcon(LPCTSTR lpIconName)
{
	HICON hIcon=LoadIcon(NULL,lpIconName);
	return SetIcon(hIcon);
}

//SetStandardIcon(UINT nIDResource)
BOOL CTrayNotifyIcon::SetStandardIcon(UINT nIDResource)
{
	HICON hIcon=LoadIcon(NULL,MAKEINTRESOURCE(nIDResource));
	return SetIcon(hIcon);
}

//SetNotificationWnd(CWnd *pNotifyWnd)
BOOL CTrayNotifyIcon::SetNotificationWnd(CWnd *pNotifyWnd)
{
	if(!m_bCreated)
		return FALSE;

	//确保窗体有效
	ASSERT(pNotifyWnd && ::IsWindow(pNotifyWnd->GetSafeHwnd()));

	m_pNotificationWnd=pNotifyWnd;
	m_NotifyIconData.hWnd=pNotifyWnd->GetSafeHwnd();
	m_NotifyIconData.uFlags=0;

	return Shell_NotifyIcon(NIM_MODIFY,&m_NotifyIconData);
}

//显示图标
void CTrayNotifyIcon::ShowIcon()
{
	ASSERT(m_bCreated);
	
	if(m_bHidden)
	{
		m_NotifyIconData.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP;
		Shell_NotifyIcon(NIM_ADD,&m_NotifyIconData);
		m_bHidden=FALSE;
	}
}

//响应消息
LRESULT CTrayNotifyIcon::OnTrayNotification(WPARAM wID, LPARAM lEvent)
{
	//当数据不是返回给该元素时,迅速返回
	if(wID!=m_NotifyIconData.uID)
		return 0L;

	//加载菜单
	CMenu menu;
	if(!menu.LoadMenu(m_NotifyIconData.uID))
		return 0;

	CMenu* pSubMenu=menu.GetSubMenu(0);
	if(!pSubMenu)
		return 0;

	//测试消息
	//在此添加其它的消息响应函数

	if(lEvent==WM_RBUTTONUP)
	{
		//当右键单击图标时,弹出菜单并使其第一个选项变为粗体
		::SetMenuDefaultItem(pSubMenu->m_hMenu,0,TRUE);

		//显示并追踪鼠标信息
		CPoint pos;
		GetCursorPos(&pos);
		::SetForegroundWindow(m_NotifyIconData.hWnd);
		::TrackPopupMenu(pSubMenu->m_hMenu,0,pos.x,pos.y,0,m_NotifyIconData.hWnd,NULL);
	}
	else if(lEvent==WM_LBUTTONDBLCLK)
	{
		//如果窗体不可见,将窗体激活
		if(!m_bIsWindowNormal)
			::SendMessage(m_NotifyIconData.hWnd,WM_COMMAND,pSubMenu->GetMenuItemID(0),0);
		else
			::SendMessage(m_NotifyIconData.hWnd,WM_COMMAND,pSubMenu->GetMenuItemID(1),0);
	}
	
	return 1;
}

