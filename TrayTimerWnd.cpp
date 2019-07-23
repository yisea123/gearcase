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

//��������
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

//���ع��캯��
CTrayTimerWnd::CTrayTimerWnd(CTrayNotifyIcon *pTrayIcon, HICON *phIcons, int nNumIcons, DWORD dwDelay)
{
	m_nCurrentIconIndex=0;

	//�������һ������ʵ��
	ASSERT(pTrayIcon);

	//������һ��ICON
	ASSERT(nNumIcons);

	//����ICON�����������Ч
	ASSERT(phIcons);

	//��ʱ���ļ����0
	ASSERT(dwDelay);

	//���ֳ�Ա����
	m_pTrayIcon=pTrayIcon;
	m_phIcons=new HICON[nNumIcons];

	//�����캯�����������ICON���󿽱���m_phIconsָ�����ڴ���
	CopyMemory(m_phIcons,phIcons,nNumIcons*sizeof(HICON));

	m_nNumIcons=nNumIcons;
	m_dwDelay=dwDelay;
}

//Windows��Ϣ��Ӧ����
int CTrayTimerWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	//ʱ�䴥����
	//m_nTimerID=SetTimer(8,m_dwDelay,NULL);
	
	return 0;
}

//��Ӧʱ�䴥����
void CTrayTimerWnd::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	//����Icon������ֵ
	++m_nCurrentIconIndex;	//��������Ļ�,����Խ��Խ������?����Ϊ����һֱ������?
							//���ᳬ��������ʾ��Χ��?

	m_nCurrentIconIndex=m_nCurrentIconIndex % m_nNumIcons;

	//����NOTIFYICONDATA�ṹ����
	m_pTrayIcon->m_NotifyIconData.uFlags=NIF_ICON;
	m_pTrayIcon->m_NotifyIconData.hIcon=m_phIcons[m_nCurrentIconIndex];

	//��̬�ı�ICON
	AfxGetMainWnd()->SetIcon(m_pTrayIcon->m_NotifyIconData.hIcon,FALSE);
	Shell_NotifyIcon(NIM_MODIFY,&m_pTrayIcon->m_NotifyIconData);

	CFrameWnd::OnTimer(nIDEvent);
}

//��ӦWindows��Ϣ
void CTrayTimerWnd::OnDestroy() 
{
	CFrameWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	KillTimer(m_nTimerID);
	CFrameWnd::OnDestroy();
}

