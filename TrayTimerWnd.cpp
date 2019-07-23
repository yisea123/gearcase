// TrayTimerWnd.cpp : implementation file
//

#include "stdafx.h"
#include "TrayTimerWnd.h"
#include "TrayNotifyIcon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrayTimerWnd

IMPLEMENT_DYNCREATE(CTrayTimerWnd, CFrameWnd)

//析构函数
CTrayTimerWnd::~CTrayTimerWnd()
{
	if(m_phIcons)
	{
		delete [] m_phIcons;
		m_phIcons=NULL;
	}
}


BEGIN_MESSAGE_MAP(CTrayTimerWnd, CFrameWnd)
	//{{AFX_MSG_MAP(CTrayTimerWnd)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrayTimerWnd message handlers

//重载构造函数
CTrayTimerWnd::CTrayTimerWnd(CTrayNotifyIcon *pTrayIcon, HICON *phIcons, int nNumIcons, DWORD dwDelay)
{
	m_nCurrentIconIndex=0;

	//必须包含一个托盘实例
	ASSERT(pTrayIcon);

	//至少有一个ICON
	ASSERT(nNumIcons);

	//保存ICON的数组必须有效
	ASSERT(phIcons);

	//计时器的间隔非0
	ASSERT(dwDelay);

	//保持成员变量
	m_pTrayIcon=pTrayIcon;
	m_phIcons=new HICON[nNumIcons];

	//将构造函数参数引入的ICON对象拷贝到m_phIcons指定的内存区
	CopyMemory(m_phIcons,phIcons,nNumIcons*sizeof(HICON));

	m_nNumIcons=nNumIcons;
	m_dwDelay=dwDelay;
}

//Windows消息响应函数
int CTrayTimerWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	//时间触发器
	//m_nTimerID=SetTimer(8,m_dwDelay,NULL);
	
	return 0;
}

//响应时间触发器
void CTrayTimerWnd::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	//增加Icon的索引值
	++m_nCurrentIconIndex;	//这样处理的话,不是越来越大了吗?能作为服务一直运行吗?
							//不会超出整数表示范围吗?

	m_nCurrentIconIndex=m_nCurrentIconIndex % m_nNumIcons;

	//更新NOTIFYICONDATA结构数据
	m_pTrayIcon->m_NotifyIconData.uFlags=NIF_ICON;
	m_pTrayIcon->m_NotifyIconData.hIcon=m_phIcons[m_nCurrentIconIndex];

	//动态改变ICON
	AfxGetMainWnd()->SetIcon(m_pTrayIcon->m_NotifyIconData.hIcon,FALSE);
	Shell_NotifyIcon(NIM_MODIFY,&m_pTrayIcon->m_NotifyIconData);

	CFrameWnd::OnTimer(nIDEvent);
}

//响应Windows消息
void CTrayTimerWnd::OnDestroy() 
{
	CFrameWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	KillTimer(m_nTimerID);
	CFrameWnd::OnDestroy();
}

