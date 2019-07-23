// DataQueryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "smssender.h"
#include "DataQueryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataQueryDlg dialog

CString CDataQueryDlg::s_strDefPath = _T("");

int CALLBACK DirDlgCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)   
{
	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		::SendMessage(hWnd, BFFM_SETSELECTION, 1, (long)CDataQueryDlg::s_strDefPath.GetBuffer(MAX_PATH));
		CDataQueryDlg::s_strDefPath.ReleaseBuffer();
		break;
	default:
		break;
	}
	return 0;
}

CDataQueryDlg::CDataQueryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDataQueryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDataQueryDlg)
		// NOTE: the ClassWizard will add member initialization here
	m_sPath = _T("");
	
	m_iMarginLeft = 5;
	m_iMarginRight = 15;
	m_iMarginTop = 10;
	m_iMarginBottom = 5;
	m_iAxisXHeight = 30;
	m_iLegendHeight = 20;
	m_iAxisYWidth = 130;

	m_sUserName = _T("");
	m_sUserNum = _T("");
	m_sProductType = _T("");
	m_sProductNum = _T("");
	m_sProducer = _T("");
	m_sTestName = _T("");
	m_sTestTime = _T("");
	
	m_sTitle = _T("");
	m_sOpinion = _T("");
	//}}AFX_DATA_INIT
	m_fSpeedDownRate = 1.0000;

	int i = 0;
	CLineEx *line = NULL;
	//Rev
	line = &m_lineRev;
	line->m_bDraw = FALSE;
	line->m_fMaxValue = 5000.00;
	line->m_fMinValue = -5000.00;
	line->m_clrColor = RGB(0xDB, 0x93, 0x70);
	//Vibration
	for(i=0; i<3; i++)
	{
		line = &m_lineVibration[i];
		line->m_bDraw = FALSE;
		line->m_fMaxValue = 20.00;
		line->m_fMinValue = 0.00;
	}
	m_lineVibration[0].m_clrColor = RGB(0xDC, 0x14, 0x3C);
	m_lineVibration[1].m_clrColor = RGB(0x41, 0x69, 0xE1);
	m_lineVibration[2].m_clrColor = RGB(0x20, 0xB2, 0xAA);
	//Temperature
	for(i=0; i<8; i++)
	{
		line = &m_lineTemperature[i];
		line->m_bDraw = FALSE;
		line->m_fMaxValue = 130.00;
		line->m_fMinValue = -10.00;
	}
	m_lineTemperature[0].m_clrColor = RGB(0xEE, 0x82, 0xEE);
	m_lineTemperature[1].m_clrColor = RGB(0xFF, 0x00, 0xFF);
	m_lineTemperature[2].m_clrColor = RGB(0x8B, 0x00, 0x8B);
	m_lineTemperature[3].m_clrColor = RGB(0x4B, 0x00, 0x82);
	m_lineTemperature[4].m_clrColor = RGB(0x78, 0x6B, 0xEE);
	m_lineTemperature[5].m_clrColor = RGB(0x48, 0x3D, 0x8B);
	m_lineTemperature[6].m_clrColor = RGB(0x00, 0x00, 0xFF);
	m_lineTemperature[7].m_clrColor = RGB(0x19, 0x19, 0x70);
	//Torque
	long fValue = floor(802.50/(10*m_fSpeedDownRate))*10;
	line = &m_lineTorque;
	line->m_bDraw = FALSE;
	line->m_fMaxValue = fValue;//120.00;
	line->m_fMinValue = -fValue;//-120.00;
	line->m_clrColor = RGB(0xFF, 0xA5, 0x00);
}

CDataQueryDlg::~CDataQueryDlg()
{

}

void CDataQueryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataQueryDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDataQueryDlg, CDialog)
	//{{AFX_MSG_MAP(CDataQueryDlg)
	ON_BN_CLICKED(IDC_BN_SELECT, OnBnSelect)
	ON_BN_CLICKED(IDC_BN_PRINT, OnBnPrint)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataQueryDlg message handlers

BOOL CDataQueryDlg::FolderExist(CString strPath)
{
	BOOL bRet = FALSE;
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(strPath, &wfd);
	if(hFind != INVALID_HANDLE_VALUE && (wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
		bRet = TRUE;
	FindClose(hFind);
	return bRet;
}

BOOL CDataQueryDlg::FileExist(CString strPath)
{
	BOOL bRet = FALSE;
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(strPath, &wfd);
	if(hFind != INVALID_HANDLE_VALUE)
		bRet = TRUE;
	FindClose(hFind);
	
	//CFileFind ff;
	//bRet = ff.FindFile(strPath);
	return bRet;
}

void CDataQueryDlg::LoadUserFromFile()
{
	CString sFile = m_sPath + _T("\\") + _T("UserInfo") + _T(".gc");
	if(FileExist(sFile))
	{
		char line[128] = {0};
		FILE *fp = fopen(sFile, "r");
		if(fp)
		{
			char *rn = NULL, *sValue = NULL;
			//UserName
			memset(line, 0, 128);
			fgets(line, 128, fp);
			rn = strchr(line, '\n');
			if(rn) *rn = '\0';
			TRACE(_T("%s\n"), line);
			sValue = strchr(line, '=');
			sValue++;
			m_sUserName.Format(_T("%s"), sValue);
			TRACE(_T("UserName=%s\n"), m_sUserName);

			//UserNum
			memset(line, 0, 128);
			fgets(line, 128, fp);
			rn = strchr(line, '\n');
			if(rn) *rn = '\0';
			TRACE(_T("%s\n"), line);
			sValue = strchr(line, '=');
			sValue++;
			m_sUserNum.Format(_T("%s"), sValue);
			TRACE(_T("UserNum=%s\n"), m_sUserNum);
			
			//ProductType
			memset(line, 0, 128);
			fgets(line, 128, fp);
			rn = strchr(line, '\n');
			if(rn) *rn = '\0';
			TRACE(_T("%s\n"), line);
			sValue = strchr(line, '=');
			sValue++;
			m_sProductType.Format(_T("%s"), sValue);
			TRACE(_T("ProductType=%s\n"), m_sProductType);

			//ProductNum
			memset(line, 0, 128);
			fgets(line, 128, fp);
			rn = strchr(line, '\n');
			if(rn) *rn = '\0';
			TRACE(_T("%s\n"), line);
			sValue = strchr(line, '=');
			sValue++;
			m_sProductNum.Format(_T("%s"), sValue);
			TRACE(_T("ProductNum=%s\n"), m_sProductNum);

			//Producer
			memset(line, 0, 128);
			fgets(line, 128, fp);
			rn = strchr(line, '\n');
			if(rn) *rn = '\0';
			TRACE(_T("%s\n"), line);
			sValue = strchr(line, '=');
			sValue++;
			m_sProducer.Format(_T("%s"), sValue);
			TRACE(_T("Producer=%s\n"), m_sProducer);

			//TestName
			memset(line, 0, 128);
			fgets(line, 128, fp);
			rn = strchr(line, '\n');
			if(rn) *rn = '\0';
			TRACE(_T("%s\n"), line);
			sValue = strchr(line, '=');
			sValue++;
			m_sTestName.Format(_T("%s"), sValue);
			TRACE(_T("TestName=%s\n"), m_sTestName);

			fclose(fp);
		}
	}
}

void CDataQueryDlg::ReadLineFromFile(CString strName, CLineEx *ln)
{
	if(FolderExist(m_sPath))
	{
		CString sFile = m_sPath + _T("\\") + strName + _T(".gc");
		if(FileExist(sFile))
		{
			char line[128] = {0};
			FILE *fp = fopen(sFile, "r");
			if(fp)
			{
				char *rn = NULL, *sValue = NULL;
				BOOL bIsDraw = 0;
				int iStyle = PS_SOLID, iThick = 1, iColor = 0;
				float fMaxValue = 1.00, fMinValue = 0.00;
				int iCount = 0, i = 0;
				
				//[Section]
				memset(line, 0, 128);
				fgets(line, 128, fp);
				rn = strchr(line, '\n');
				if(rn) *rn = '\0';
				TRACE(_T("%s\n"), line);

				//Time
				memset(line, 0, 128);
				fgets(line, 128, fp);
				rn = strchr(line, '\n');
				if(rn) *rn = '\0';
				TRACE(_T("%s\n"), line);
				sValue = strchr(line, '=');
				sValue++;
				TRACE(_T("Time=%s\n"), sValue);
				m_sTestTime.Format(_T("%s"), sValue);

				//IsDraw
				memset(line, 0, 128);
				fgets(line, 128, fp);
				rn = strchr(line, '\n');
				if(rn) *rn = '\0';
				TRACE(_T("%s\n"), line);
				sValue = strchr(line, '=');
				sValue++;
				//TRACE(_T("IsDraw=%s\n"), sValue);
				//ln->m_bDraw = atoi(sValue);

				//Style
				memset(line, 0, 128);
				fgets(line, 128, fp);
				rn = strchr(line, '\n');
				if(rn) *rn = '\0';
				TRACE(_T("%s\n"), line);
				sValue = strchr(line, '=');
				sValue++;
				TRACE(_T("Style=%s\n"), sValue);
				ln->m_iStyle = atoi(sValue);

				//Thick
				memset(line, 0, 128);
				fgets(line, 128, fp);
				rn = strchr(line, '\n');
				if(rn) *rn = '\0';
				TRACE(_T("%s\n"), line);
				sValue = strchr(line, '=');
				sValue++;
				TRACE(_T("Thick=%s\n"), sValue);
				ln->m_iThick = atoi(sValue);

				//Color
				memset(line, 0, 128);
				fgets(line, 128, fp);
				rn = strchr(line, '\n');
				if(rn) *rn = '\0';
				TRACE(_T("%s\n"), line);
				sValue = strchr(line, '=');
				sValue++;
				TRACE(_T("Color=%s\n"), sValue);
				ln->m_clrColor = atoi(sValue);

				//MaxValue
				memset(line, 0, 128);
				fgets(line, 128, fp);
				rn = strchr(line, '\n');
				if(rn) *rn = '\0';
				TRACE(_T("%s\n"), line);
				sValue = strchr(line, '=');
				sValue++;
				TRACE(_T("MaxValue=%s\n"), sValue);
				ln->m_fMaxValue = atof(sValue);

				//MinValue
				memset(line, 0, 128);
				fgets(line, 128, fp);
				rn = strchr(line, '\n');
				if(rn) *rn = '\0';
				TRACE(_T("%s\n"), line);
				sValue = strchr(line, '=');
				sValue++;
				TRACE(_T("MinValue=%s\n"), sValue);
				ln->m_fMinValue = atof(sValue);

				//ValuePerPixel
				ln->m_fValuePerPixel = (ln->m_fMaxValue-ln->m_fMinValue)/m_rectPlot.Height();
				TRACE("ln->m_fValuePerPixel=%.2f\n", ln->m_fValuePerPixel);

				//Count
				memset(line, 0, 128);
				fgets(line, 128, fp);
				rn = strchr(line, '\n');
				if(rn) *rn = '\0';
				TRACE(_T("%s\n"), line);
				sValue = strchr(line, '=');
				sValue++;
				TRACE(_T("Count=%s\n"), sValue);
				iCount = atoi(sValue);
				//ln->m_iCount = 0;
				ln->DeleteAllPoint();
				for(i=0; i<iCount; i++)
				{
					//XValue YValue
					char szX[32] = {0}, szY[32] = {0};
					memset(szX, 0, 32);
					memset(szY, 0, 32);
					memset(line, 0, 128);
					fgets(line, 128, fp);
					char *rn = strchr(line, '\n');
					if(rn) *rn = '\0';
					char *l = strchr(line, '=');
					l++;
					char *r = strchr(line, ' ');
					memcpy(szX, l, r-l);
					r++;
					strcpy(szY, r);
					TRACE(_T("szX=%s szY=%s\n"), szX, szY);
					long lx = atol(szX);
					float fy = atof(szY);
					ln->AddPoint(lx, fy);
				}
				//ln->m_iCount = iCount;
				fclose(fp);
			}
		}
	}
}

void CDataQueryDlg::CalcLayout()
{
	//GetClientRect(&m_rectCtrl);
	m_rectCtrl.left = m_rectDisplay.left;
	m_rectCtrl.right = m_rectDisplay.right;
	m_rectCtrl.top = m_rectDisplay.top;
	m_rectCtrl.bottom = m_rectDisplay.bottom;
	
	m_rectLegend.left = m_rectCtrl.left + m_iMarginLeft + 30;
	m_rectLegend.right = m_rectCtrl.right - m_iMarginRight;
	m_rectLegend.top = m_rectCtrl.bottom - m_iLegendHeight - m_iMarginBottom;
	m_rectLegend.bottom = m_rectCtrl.bottom - m_iMarginBottom;
	
	m_rectAxisY.left = m_rectCtrl.left + m_iMarginLeft;
	m_rectAxisY.right = m_rectAxisY.left + m_iAxisYWidth;
	m_rectAxisY.top   = m_rectCtrl.top + m_iMarginTop;
	m_rectAxisY.bottom = m_rectCtrl.bottom - m_iMarginBottom - m_iAxisXHeight - m_iLegendHeight;
	
	m_rectAxisX.left = m_rectAxisY.right;
	m_rectAxisX.right = m_rectCtrl.right - m_iMarginRight;
	m_rectAxisX.top = m_rectAxisY.bottom;
	m_rectAxisX.bottom = m_rectAxisX.top + m_iAxisXHeight;
	
	m_rectPlot.left   = m_rectAxisY.right;
	m_rectPlot.right  = m_rectAxisX.right;
	m_rectPlot.top    = m_rectAxisY.top;
	m_rectPlot.bottom = m_rectAxisX.top;
	
	m_fYValuePerPixel = (5000.00 - (-5000.00)) / m_rectPlot.Height();
	m_fXValuePerPixel = (5000.00 - 0) / m_rectPlot.Width();
	TRACE("m_fYValuePerPixel=%.1f m_fXValuePerPixel=%.1f\n", m_fYValuePerPixel, m_fXValuePerPixel);
	int i = 0;
	//Rev
	m_fRevValuePerPixel = (5000.00 - (-5000.00))/m_rectPlot.Height();
	TRACE("m_fRevValuePerPixel=%.2f\n", m_fRevValuePerPixel);
	CLineEx *line = NULL;
	line = &m_lineRev;
	line->m_fValuePerPixel = (line->m_fMaxValue - line->m_fMinValue)/m_rectPlot.Height();
	//Vibration
	m_fVibrationValuePerPixel = (20.00 - 0.00)/m_rectPlot.Height();
	TRACE("m_fVibrationValuePerPixel=%.2f\n",m_fVibrationValuePerPixel);
	for(i=0; i<3; i++)
	{
		line = &m_lineVibration[i];
		line->m_fValuePerPixel = (line->m_fMaxValue - line->m_fMinValue)/m_rectPlot.Height();
	}
	//Temperature
	m_fTemperatureValuePerPixel = (130.00 - (-10.00))/m_rectPlot.Height();
	TRACE("m_fTemperatureValuePerPixel=%.2f\n",m_fTemperatureValuePerPixel);
	for(i=0; i<8; i++)
	{
		line = &m_lineTemperature[i];
		line->m_fValuePerPixel = (line->m_fMaxValue - line->m_fMinValue)/m_rectPlot.Height();
	}
	//Torque
	//m_fTorqueValuePerPixel = (120.00 - (-120.00))/m_rectPlot.Height();
	m_fTorqueValuePerPixel = (802.50 - (-802.50))/m_rectPlot.Height();
	m_fTorqueValuePerPixel = m_fTorqueValuePerPixel/m_fSpeedDownRate;
	TRACE("m_fTorqueValuePerPixel=%.2f\n", m_fTorqueValuePerPixel);
	line = &m_lineTorque;
	line->m_fValuePerPixel = (line->m_fMaxValue - line->m_fMinValue)/m_rectPlot.Height();
}

BOOL CDataQueryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CDataQueryDlg::s_strDefPath = m_sPath;
	SetDlgItemText(IDC_EDIT_PATH, m_sPath);
	CWnd *pWnd = NULL;
	pWnd = GetDlgItem(IDC_STATIC_TITLE);
	pWnd->GetWindowRect(&m_rectTitle);
	pWnd = GetDlgItem(IDC_STATIC_DISPLAY);
	pWnd->GetWindowRect(&m_rectDisplay);
	pWnd = GetDlgItem(IDC_STATIC_OPINION);
	pWnd->GetWindowRect(&m_rectOpinion);
	pWnd->GetWindowText(m_sOpinion);
	pWnd = GetDlgItem(IDC_EDIT_COMMENT);
	pWnd->GetWindowRect(&m_rectComment);
	ScreenToClient(&m_rectTitle);
	ScreenToClient(&m_rectDisplay);
	ScreenToClient(&m_rectOpinion);
	ScreenToClient(&m_rectComment);
	CalcLayout();

	int ri = m_sPath.ReverseFind('\\');
	if(ri>=0)
	{
		m_sTestTime = m_sPath.Right(m_sPath.GetLength()- ri -1);
	}
	LoadUserFromFile();
	ReadLines();
	m_sTitle.Format(_T("%s %s %s %s %s %s %s"), 
		m_sUserName, m_sUserNum, m_sProductType, m_sProductNum, m_sProducer, m_sTestName, m_sTestTime);
	SetDlgItemText(IDC_STATIC_TITLE, m_sTitle);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDataQueryDlg::OnBnSelect() 
{
	// TODO: Add your control notification handler code here
	char szDir[MAX_PATH] = {0};
	BROWSEINFO bi;
	ITEMIDLIST *pidl;
	bi.hwndOwner = m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDir;
	bi.lpszTitle = _T("请选择数据文件的目录");
	bi.ulFlags = BIF_STATUSTEXT | BIF_RETURNONLYFSDIRS;
	bi.lpfn = DirDlgCallbackProc;
	bi.lParam = 0;
	bi.iImage = 0;
	pidl = SHBrowseForFolder(&bi);
	if(pidl == NULL) return;
	if(!SHGetPathFromIDList(pidl, szDir)) return;
	else m_sPath = szDir;
	SetDlgItemText(IDC_EDIT_PATH, m_sPath);
	CDataQueryDlg::s_strDefPath = m_sPath;
	int ri = m_sPath.ReverseFind('\\');
	if(ri>=0)
	{
		m_sTestTime = m_sPath.Right(m_sPath.GetLength()- ri -1);
	}
	LoadUserFromFile();
	ReadLines();
	m_sTitle.Format(_T("%s %s %s %s %s %s %s"), 
		m_sUserName, m_sUserNum, m_sProductType, m_sProductNum, m_sProducer, m_sTestName, m_sTestTime);
	SetDlgItemText(IDC_STATIC_TITLE, m_sTitle);
	Invalidate();
}

void CDataQueryDlg::OnBnPrint() 
{
	// TODO: Add your control notification handler code here
	CDC dc;
    CPrintDialog printDlg(FALSE);
    if (printDlg.DoModal() == IDCANCEL) //获取用户对打印机的设置
        return;
    dc.Attach(printDlg.GetPrinterDC()); //获取打印机设备环境，赋予CDC
    dc.m_bPrinting = TRUE;
    CString strTitle; //得到应用程序名称
    strTitle.LoadString(AFX_IDS_APP_TITLE);
	TRACE(_T("strTitle=%s\n"), strTitle);
    DOCINFO doci; //初始化打印文档内容
    ::ZeroMemory (&doci, sizeof (DOCINFO));
    doci.cbSize = sizeof (DOCINFO);
    doci.lpszDocName = strTitle;
    BOOL bPrintingOK = dc.StartDoc(&doci); //开始一个打印作业
	TRACE(_T("bPrintingOK=%d\n"), bPrintingOK);
    CPrintInfo Info; //获取打印范围，存入m_rectDraw中
	CRect rect;
	rect.left = m_rectTitle.left;
	rect.right = m_rectTitle.right;
	rect.top = m_rectTitle.top;
	rect.bottom = m_rectComment.bottom;
    Info.m_rectDraw.SetRect(0, 0, dc.GetDeviceCaps(HORZRES), dc.GetDeviceCaps(VERTRES));
	//Info.m_rectDraw.SetRect(0, 0, rect.Width(), rect.Height());
	//OnBeginPrinting(&dc, &Info);

	dc.SetMapMode(MM_ANISOTROPIC); //转化坐标映射模式
	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN); //得到屏幕分辨率的宽和高(我的是1280*1024)
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	CSize size = CSize(nScreenWidth, nScreenHeight);
	dc.SetWindowExt(size); //设置视窗大小
	int xLogPixPerInch = dc.GetDeviceCaps(LOGPIXELSX);  //得到设备每逻辑英寸的像素数量
	int yLogPixPerInch = dc.GetDeviceCaps(LOGPIXELSY);
	float fWidth = (float)xLogPixPerInch / 96 ; //得到电脑屏幕映射到视窗大小比率
	float fHeight = (float)yLogPixPerInch / 96; //一般得到的fWidth = fHeight 
	long xExt = (long)(fWidth * size.cx); //得到视窗大小
	long yExt = (long)(fHeight * size.cy) ;
	dc.SetViewportExt((int)xExt, (int)yExt); //设置视窗大小(由电脑屏幕映射到视窗大小)
	//Info.m_rectDraw.SetRect(0, 0, xExt, yExt);

    //for (UINT uiPage = Info.GetMinPage(); uiPage <= Info.GetMaxPage() && bPrintingOK; uiPage++)
    {
        dc.StartPage(); //开始打印新的页
        //Info.m_nCurPage = uiPage;
		Info.m_nCurPage = 1;
		dc.DrawText(m_sTitle, m_rectTitle, DT_LEFT|DT_TOP);
        OnDraw(&dc);  //打印
		dc.DrawText(m_sOpinion, m_rectOpinion, DT_LEFT|DT_TOP);
		CString strComment;
		GetDlgItemText(IDC_EDIT_COMMENT, strComment);
		CEdit *pEdit = (CEdit*)(GetDlgItem(IDC_EDIT_COMMENT));
		int iLineCount = pEdit->GetLineCount();
		int i = 0;
		int iHeight = 0;
		for(i=0; i<iLineCount; i++)
		{
			iHeight = iHeight + 16;
		}
		if(iHeight>m_rectComment.Height())
			m_rectComment.bottom = m_rectComment.top + iHeight;
		dc.DrawText(strComment, m_rectComment, DT_LEFT|DT_TOP);
        bPrintingOK = dc.EndPage(); //结束页
    }
    if (bPrintingOK)
        dc.EndDoc(); //停止打印作业
    else
        dc.AbortDoc(); //中断打印作业
   dc.DeleteDC(); //删除打印设备环境
}

void CDataQueryDlg::DrawBasic(CDC *pDC) //画整个图区的属性、边框和背景
{
	CRect rect(&m_rectDisplay);
	pDC->SetBkMode(TRANSPARENT); // 设置透明
	COLORREF clr = RGB(0x8F, 0xBC, 0x8F);
	pDC->FillSolidRect(&rect, clr);
}

void CDataQueryDlg::DrawAxises(CDC* pDC)
{
	int i = 0;
	int nInterval = 10;
	
	//Draw Y axis
	CPen pen(PS_SOLID, 1, RGB(192, 192, 192));
	CPen *oldPen = pDC->SelectObject(&pen);
	//Draw border of plot
	pDC->MoveTo(m_rectPlot.left,   m_rectPlot.top);  
	pDC->LineTo(m_rectPlot.right,  m_rectPlot.top);  
	pDC->LineTo(m_rectPlot.right,  m_rectPlot.bottom);  
	pDC->LineTo(m_rectPlot.left,   m_rectPlot.bottom);  
	pDC->LineTo(m_rectPlot.left,   m_rectPlot.top); 

	int iYGridCount = 20;
	int iYGrid =(int)floor((5000.00 - (-5000.00))/iYGridCount);
	if(iYGrid>0)
	{
		CString str;
		int textWidth = 0;
		int textHeight = 16;
		CSize sizeStr(0, 0);
		sizeStr = pDC->GetTextExtent("-5000");
		textHeight = sizeStr.cy;
		int y0=(int) (-5000.00);
		for(i=0;i<iYGridCount+1;i++)
		{
			//AxisY
			int y = (int)(m_rectPlot.bottom - (i*iYGrid)/m_fYValuePerPixel);
			pDC->MoveTo(m_rectPlot.left,y);
			pDC->LineTo(m_rectPlot.right,y);

			//Rev(-5000, 5000)
			CSize sizeRev(0, 0);
			sizeRev = pDC->GetTextExtent("-5000");
			textWidth = sizeRev.cx;
			textHeight = sizeRev.cy;
			int yRev0 = (-5000.00);
			float fRevValuePerPixel = m_fRevValuePerPixel;
			float fRevRange = (5000.00 - (-5000.00));
			int iRevGrid =(int)floor(fRevRange/iYGridCount);
			CString strRev;
			int yRev = (int)(m_rectPlot.bottom - (i*iRevGrid)/fRevValuePerPixel);
			strRev.Format(_T("%d"), yRev0+i*iRevGrid);
			CRect rectRev(0, 0, 0, 0);
			rectRev.left = m_rectAxisY.right - textWidth;
			rectRev.right = m_rectAxisY.right;
			rectRev.top = yRev - textHeight/2;
			rectRev.bottom = yRev + textHeight/2;
			pDC->DrawText(strRev, rectRev, DT_CENTER|DT_VCENTER);
			CRect rtRev(rectRev.left, m_rectAxisY.bottom+8, rectRev.right-6, m_rectAxisX.bottom);
			pDC->DrawText(_T("转速"), rtRev, DT_CENTER|DT_VCENTER);

			//Vibration(0, 20)
			CSize sizeVibration(0, 0);
			sizeVibration = pDC->GetTextExtent("20");
			textWidth = sizeVibration.cx;
			textHeight = sizeVibration.cy;
			int yVibration0 = 0;
			float fVibrationValuePerPixel = m_fVibrationValuePerPixel;
			float fVibrationRange = 20.00 - 0.00;
			int iVibrationGrid =(int)floor(fVibrationRange/iYGridCount);
			CString strVibration;
			int yVibration = (int)(m_rectPlot.bottom - (i*iVibrationGrid)/fVibrationValuePerPixel);
			strVibration.Format(_T("%d"), yVibration0+i*iVibrationGrid);
			CRect rectVibration(0, 0, 0, 0);
			rectVibration.left = rectRev.left - nInterval - textWidth;
			rectVibration.right = rectRev.left - nInterval;
			rectVibration.top = yVibration - textHeight/2;
			rectVibration.bottom = yVibration + textHeight/2;
			pDC->DrawText(strVibration, rectVibration, DT_CENTER|DT_VCENTER);
			CRect rtVibration(rectVibration.left-6, m_rectAxisY.bottom+8, rectVibration.right+6, m_rectAxisX.bottom);
			pDC->DrawText(_T("振动"), rtVibration, DT_CENTER|DT_VCENTER);

			//Temperature(-10, 130)
			CSize sizeTemperature(0, 0);
			sizeTemperature = pDC->GetTextExtent("120");
			textWidth = sizeTemperature.cx;
			textHeight = sizeTemperature.cy;
			int yTemperature0 = -10.00;
			float fTemperatureRange = (130.00 - (-10.00));
			float fTemperatureValuePerPixel = m_fTemperatureValuePerPixel;
			int iTemperatureGrid =(int)floor(fTemperatureRange/iYGridCount);
			CString strTemperature;
			int yTemperature = (int)(m_rectPlot.bottom - (i*iTemperatureGrid)/fTemperatureValuePerPixel);
			strTemperature.Format(_T("%d"), yTemperature0+i*iTemperatureGrid);
			CRect rectTemperature(0, 0, 0, 0);
			rectTemperature.left = rectVibration.left - nInterval - textWidth;
			rectTemperature.right = rectVibration.left - nInterval;
			rectTemperature.top = yTemperature - textHeight/2;
			rectTemperature.bottom = yTemperature + textHeight/2;
			pDC->DrawText(strTemperature, rectTemperature, DT_CENTER|DT_VCENTER);
			CRect rtTemperature(rectTemperature.left-6, m_rectAxisY.bottom+8, rectTemperature.right, m_rectAxisX.bottom);
			pDC->DrawText(_T("温升"), rtTemperature, DT_CENTER|DT_VCENTER);

			//Torque(-120, 120)
			CSize sizeTorque(0, 0);
			sizeTorque = pDC->GetTextExtent("-120");
			textWidth = sizeTorque.cx;
			textHeight = sizeTorque.cy;
			long fValue = floor(802.50/(10*m_fSpeedDownRate))*10;
			int yTorque0 = -fValue;//-120.00;
			float fTorqueRange = fValue - (-fValue);//120.00 - (-120.00);
			float fTorqueValuePerPixel = m_fTorqueValuePerPixel;
			int iTorqueGrid =(int)floor(fTorqueRange/iYGridCount);
			CString strTorque;
			int yTorque = (int)(m_rectPlot.bottom - (i*iTorqueGrid)/fTorqueValuePerPixel);
			strTorque.Format(_T("%d"), yTorque0+i*iTorqueGrid);
			CRect rectTorque(0, 0, 0, 0);
			rectTorque.left = rectTemperature.left - nInterval - textWidth;
			rectTorque.right = rectTemperature.left - nInterval;
			rectTorque.top = yTorque - textHeight/2;
			rectTorque.bottom = yTorque + textHeight/2;
			pDC->DrawText(strTorque, rectTorque, DT_CENTER|DT_VCENTER);
			CRect rtTorque(rectTorque.left-6, m_rectAxisY.bottom+8, rectTorque.right, m_rectAxisX.bottom);
			pDC->DrawText(_T("转矩"), rtTorque, DT_CENTER|DT_VCENTER);
		}
	}
	//pDC->SelectObject(oldPen);

	
	//Draw X axis
	//CPen pen(PS_SOLID, 1, RGB(192, 192, 192));
	//CPen *oldPen = pDC->SelectObject(&pen);
	int iXGridCount = 20;
	int iXGrid =(int)floor((5000.00 - 0.00)/iXGridCount);
	if(iXGrid>0)
	{
		CString str;
		int textWidth = 0;
		int textHeight =16;
		CSize sizeStr(0, 0);
		sizeStr = pDC->GetTextExtent("5000");
		textHeight = sizeStr.cy;
		textWidth = sizeStr.cx;
		int y0 = 0.00; 
		for(i=0;i<iXGridCount+1;i++)
		{
			int y = (int)(m_rectPlot.left + (i*iXGrid)/m_fXValuePerPixel);
			str.Format(_T("%d"), y0+i*iXGrid);
			CRect rectText(0, 0, 0, 0);
			rectText.left = y-textWidth/2;
			rectText.right = y+textWidth/2;
			rectText.top = m_rectAxisX.top;
			rectText.bottom = m_rectAxisX.bottom;
			if(i%2 == 0)
				pDC->DrawText(str, rectText, DT_CENTER|DT_VCENTER);
			
			pDC->MoveTo(y, m_rectPlot.top);
			pDC->LineTo(y, m_rectPlot.bottom);
		}
	}
	pDC->SelectObject(oldPen);
}

void CDataQueryDlg::DrawLegends(CDC* pDC)
{
	int i = 0;
	int dx = m_rectLegend.Width()/13, iCount = 0;
	int left = m_rectLegend.left;
	int top = m_rectLegend.top;
	int iLegendHeight = m_rectLegend.Height();
	CString str;
	CSize sizeStr(0, 0);
	CRect rect(0, 0, 0, 0);
	CLineEx *line = NULL;
	str = _T("图例");
	sizeStr = pDC->GetTextExtent(str);
	
	rect.left = left + iCount * dx;
	rect.right = rect.left + dx;
	rect.top = top;
	rect.bottom = rect.top + iLegendHeight;
	CRect rectRev(rect);
	str = _T("转速");
	line = &m_lineRev;
	if(line->m_bDraw)
	{
		DrawLegend(pDC, line->m_clrColor, str, rectRev);
		iCount++;
	}
	
	for(i=0; i<3; i++)
	{
		rect.left = left + iCount * dx;
		rect.right = rect.left + dx;
		rect.top = top;
		rect.bottom = rect.top + iLegendHeight;
		CRect rectVibration(rect);
		str.Format(_T("振动%d"), i+1);
		line = &m_lineVibration[i];
		if(line->m_bDraw)
		{
			DrawLegend(pDC, line->m_clrColor, str, rectVibration);
			iCount++;
		}
	}
	
	for(i=0; i<7; i++)
	{
		rect.left = left + iCount * dx;
		rect.right = rect.left + dx;
		rect.top = top;
		rect.bottom = rect.top + iLegendHeight;
		CRect rectTemperature(rect);
		str.Format(_T("温升%d"), i+1);
		line = &m_lineTemperature[i];
		if(line->m_bDraw)
		{
			DrawLegend(pDC, line->m_clrColor, str, rectTemperature);
			iCount++;
		}
	}
	rect.left = left + iCount * dx;
	rect.right = rect.left + dx;
	rect.top = top;
	rect.bottom = rect.top + iLegendHeight;
	CRect rectTemperature(rect);
	str.Format(_T("室温"));
	line = &m_lineTemperature[i];
	if(line->m_bDraw)
	{
		DrawLegend(pDC, line->m_clrColor, str, rectTemperature);
		iCount++;
	}

	rect.left = left + iCount * dx;
	rect.right = rect.left + dx;
	rect.top = top;
	rect.bottom = rect.top + iLegendHeight;
	CRect rectTorque(rect);
	str.Format(_T("转矩"));
	line = &m_lineTorque;
	if(line->m_bDraw)
	{
		DrawLegend(pDC, line->m_clrColor, str, rectTorque);
		iCount++;
	}
}

void CDataQueryDlg::DrawLegend(CDC* pDC, COLORREF clr, CString name, CRect rect)
{
	int textWidth = 0;
	int textHeight = 0;
	CSize sizeStr(0, 0);
	sizeStr = pDC->GetTextExtent(name);
	textWidth = sizeStr.cx;
	textHeight = sizeStr.cy;
	CRect rectText(0, 0, 0, 0);
	int y = rect.left;
	rectText.left = y;
	rectText.right = y + textWidth;
	rectText.top = rect.top;
	rectText.bottom = rect.bottom;
	pDC->DrawText(name, rectText, DT_CENTER|DT_VCENTER);
	pDC->FillSolidRect(rectText.right, rectText.top+2, rect.Width()-textWidth-2, rectText.Height()-8, clr);
}

void CDataQueryDlg::ReadLines()
{
	int i = 0;
	CString strName;
	strName.Format(_T("Rev"));
	ReadLineFromFile(strName, &m_lineRev);

	for(i=0; i<3; i++)
	{
		strName.Format(_T("Vibration%d"), i);
		ReadLineFromFile(strName, &m_lineVibration[i]);
	}
	for(i=0; i<8; i++)
	{
		strName.Format(_T("Temperature%d"), i);
		ReadLineFromFile(strName, &m_lineTemperature[i]);
	}
	strName.Format(_T("Torque"));
	ReadLineFromFile(strName, &m_lineTorque);
}

void CDataQueryDlg::DrawLine(CDC* pDC, CLineEx *line)
{
	if(line == NULL) return;
	int i = 0;
	int iCnt = line->GetCount(); //line->m_iCount;
	BOOL bDraw = line->m_bDraw;
	CPoint *pt = NULL;
	if(iCnt > 0 && bDraw) 
	{
		pt = new CPoint[iCnt];
		if(pt == NULL) return;
		for(i=0;i<iCnt;i++)
		{
			pt[i].x = m_rectPlot.left + i/m_fXValuePerPixel;
			pt[i].y = (int)(m_rectPlot.bottom - ((line->m_fYValue[i]-line->m_fMinValue)/line->m_fValuePerPixel));
		}
		CPen pen(line->m_iStyle, line->m_iThick, line->m_clrColor);
		CPen *oldPen = pDC->SelectObject(&pen);
		pDC->Polyline(pt, iCnt);
		pDC->SelectObject(oldPen);
		if(pt) delete pt;
	}
}

void CDataQueryDlg::DrawRev(CDC* pDC)
{
	DrawLine(pDC, &m_lineRev);
}

void CDataQueryDlg::DrawVibration(CDC* pDC)
{
	int i = 0;
	for(i=0; i<3; i++)
		DrawLine(pDC, &m_lineVibration[i]);
}
void CDataQueryDlg::DrawTemperature(CDC* pDC)
{
	int i = 0;
	for(i=0; i<8; i++)
		DrawLine(pDC, &m_lineTemperature[i]);
}
void CDataQueryDlg::DrawToraue(CDC* pDC)
{
	DrawLine(pDC, &m_lineTorque);
}

void CDataQueryDlg::DrawLines(CDC *pDC)
{
	DrawRev(pDC);
	DrawVibration(pDC);
	DrawTemperature(pDC);
	DrawToraue(pDC);
}

void CDataQueryDlg::OnDraw(CDC *pDC)
{
	CFont font;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 12;
	strcpy(lf.lfFaceName, "宋体");
	font.CreateFontIndirect(&lf);
	CFont* oldfont = pDC->SelectObject(&font);

	DrawBasic(pDC);
	DrawAxises(pDC);
	DrawLegends(pDC);
	DrawLines(pDC);

	pDC->SelectObject(oldfont);
	font.DeleteObject();
}

void CDataQueryDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	OnDraw(&dc);

	// Do not call CDialog::OnPaint() for painting messages
}
