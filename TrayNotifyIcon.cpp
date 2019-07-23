// TrayNotifyIcon.cpp: implementation of the CTrayNotifyIcon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TrayNotifyIcon.h"
#include "TrayTimerWnd.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//��̬����CTrayNotifyIcon��Ķ�̬�ؼ��궨��
IMPLEMENT_DYNAMIC(CTrayNotifyIcon,CObject)

//���캯��
CTrayNotifyIcon::CTrayNotifyIcon()
{
	//��ʼ����������

	//����m_NotifyIconData�е�����Ϊ0
	memset(&m_NotifyIconData,0,sizeof(m_NotifyIconData));

	m_bCreated=FALSE;
	m_bHidden=FALSE;
	m_pNotificationWnd=NULL;
	m_pTimerWnd=NULL;
	m_bAnimated=FALSE;

}

//��������
CTrayNotifyIcon::~CTrayNotifyIcon()
{
	DestroyTimerWindow();
	RemoveIcon();
}

//�������̵�ͼ��
//pNotifyWndָ��ָ����CMainFrame
BOOL CTrayNotifyIcon::Create(CWnd *pNotifyWnd, UINT uID, LPCTSTR pszTooltipText, HICON hIcon, UINT nNotifyMessage)
{
	//ȷ��pNotifyWndָ��Ĵ�����Ч
	ASSERT(pNotifyWnd && ::IsWindow(pNotifyWnd->GetSafeHwnd()));
	m_pNotificationWnd=pNotifyWnd;

	//nNotifyMessage��������Ϣ�Ƿ񳬹���Windows������û���Ϣ
	ASSERT(nNotifyMessage>=WM_USER);

	//�����е�ͼ����ʾ��Ϣ������64���ַ�
	ASSERT(_tcslen(pszTooltipText)<=64);

	//���ñ�Ҫ��NOTIFYICONDATA�ṹ������
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

//���غ���Create(...)
//�������̵�ͼ��
//������ʱ��Ͷ����Ĵ���
BOOL CTrayNotifyIcon::Create(CWnd *pNotifyWnd, UINT uID, LPCTSTR pszTooltipText, HICON *phIcons, int nNumIcons, DWORD dwDelay, UINT nNotifyMessage)
{
	//�������ʾ�Ƿ�����
	//m_bIsWindowNormal=true;

	//�漰��������ʾ,���ٱ�����2��ͼ��
	ASSERT(nNumIcons>=2);

	//����ʱ�䴥��������,�ô����з�װ�������������Ĵ���
	if(!CreateTimerWindow(phIcons,nNumIcons,dwDelay))
		return FALSE;

	//ʹ�����ص�Create()����������������ͼ��
	//�ô�������������ʼͼ��ʹ�õ���ͼ�������еĵ�һ��
	BOOL bSuccess=Create(pNotifyWnd,uID,pszTooltipText,phIcons[0],nNotifyMessage);
	m_bAnimated=TRUE;

	return bSuccess;
}

BOOL CTrayNotifyIcon::CreateTimerWindow(HICON *phIcons, int nNumIcons, DWORD dwDelay)
{
	//��������CTimer��ʱ������ʽ����
	ASSERT(m_pTimerWnd==NULL);
	m_pTimerWnd=new CTrayTimerWnd(this,phIcons,nNumIcons,dwDelay);

	if(!m_pTimerWnd)
		return FALSE;

	//����CWnd���ȱʡ����Create()
	if(!m_pTimerWnd->Create(NULL,_T("")))
		return FALSE;	

	return TRUE;
}

void CTrayNotifyIcon::DestroyTimerWindow()
{
	//������ڵĻ�,ɾ����ʱ������
	if(m_pTimerWnd)
	{
		m_pTimerWnd->SendMessage(WM_CLOSE);
		m_pTimerWnd=NULL;
	}
}

//�õ���ǰʹ�õ�ICON���
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

//�õ�����Notify��Ϣ�Ĵ���
CWnd* CTrayNotifyIcon::GetNotificationWnd() const
{
	//����CTrayNotifyIcon���е�CWnd��Ա����
	return m_pNotificationWnd;
}

//�õ�CToolTip��ʾ����ʾ����
CString CTrayNotifyIcon::GetTooltipText() const
{
	CString sText;
	if(m_bCreated)
		sText=m_NotifyIconData.szTip;

	return sText;
}

//������ʾ����
BOOL CTrayNotifyIcon::SetTooltipText(LPCTSTR pszTooltipText)
{
	//���û�д���,���˳�
	if(!m_bCreated)
		return FALSE;

	m_bIsWindowNormal=true;
	m_NotifyIconData.uFlags=NIF_TIP;
	_tcscpy(m_NotifyIconData.szTip,pszTooltipText);

	//��Shell_NotifyIconˢ����ʾ
	return Shell_NotifyIcon(NIM_MODIFY,&m_NotifyIconData);
}

//��uID������ʾ����
BOOL CTrayNotifyIcon::SetTooltipText(UINT nID)
{
	CString sToolTipText;
	VERIFY(sToolTipText.LoadString(nID));

	return SetTooltipText(sToolTipText);
}

//����ͼ��
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

//�õ��Ƿ񶯻���ʾ
BOOL CTrayNotifyIcon::IsAnimated()
{
	return m_bAnimated;
}

//ˢ�������е�ͼ��,�ɾ���
void CTrayNotifyIcon::MoveToExtremeRight()
{
	HideIcon();
	ShowIcon();
}

//�Ƴ�ͼ��
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

	//ȷ��������Ч
	ASSERT(pNotifyWnd && ::IsWindow(pNotifyWnd->GetSafeHwnd()));

	m_pNotificationWnd=pNotifyWnd;
	m_NotifyIconData.hWnd=pNotifyWnd->GetSafeHwnd();
	m_NotifyIconData.uFlags=0;

	return Shell_NotifyIcon(NIM_MODIFY,&m_NotifyIconData);
}

//��ʾͼ��
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

//��Ӧ��Ϣ
LRESULT CTrayNotifyIcon::OnTrayNotification(WPARAM wID, LPARAM lEvent)
{
	//�����ݲ��Ƿ��ظ���Ԫ��ʱ,Ѹ�ٷ���
	if(wID!=m_NotifyIconData.uID)
		return 0L;

	//���ز˵�
	CMenu menu;
	if(!menu.LoadMenu(m_NotifyIconData.uID))
		return 0;

	CMenu* pSubMenu=menu.GetSubMenu(0);
	if(!pSubMenu)
		return 0;

	//������Ϣ
	//�ڴ������������Ϣ��Ӧ����

	if(lEvent==WM_RBUTTONUP)
	{
		//���Ҽ�����ͼ��ʱ,�����˵���ʹ���һ��ѡ���Ϊ����
		::SetMenuDefaultItem(pSubMenu->m_hMenu,0,TRUE);

		//��ʾ��׷�������Ϣ
		CPoint pos;
		GetCursorPos(&pos);
		::SetForegroundWindow(m_NotifyIconData.hWnd);
		::TrackPopupMenu(pSubMenu->m_hMenu,0,pos.x,pos.y,0,m_NotifyIconData.hWnd,NULL);
	}
	else if(lEvent==WM_LBUTTONDBLCLK)
	{
		//������岻�ɼ�,�����弤��
		if(!m_bIsWindowNormal)
			::SendMessage(m_NotifyIconData.hWnd,WM_COMMAND,pSubMenu->GetMenuItemID(0),0);
		else
			::SendMessage(m_NotifyIconData.hWnd,WM_COMMAND,pSubMenu->GetMenuItemID(1),0);
	}
	
	return 1;
}

