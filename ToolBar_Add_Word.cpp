// ToolBar_Add_Word.cpp : implementation file
//

#include "stdafx.h"
#include "ToolBar_Add_Word.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolBar_Add_Word

CToolBar_Add_Word::CToolBar_Add_Word()
{
}

CToolBar_Add_Word::~CToolBar_Add_Word()
{
}


BEGIN_MESSAGE_MAP(CToolBar_Add_Word, CToolBar)
	//{{AFX_MSG_MAP(CToolBar_Add_Word)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolBar_Add_Word message handlers

BOOL CToolBar_Add_Word::LoadToolBarEx(UINT nID, TOOLTEXT *pToolText, int nCnt)
{
	BOOL bRet;
	if (bRet=LoadToolBar(nID))
	{
		// loop thru tooltext adding text to buttons
		for (int i=0;i<nCnt;i++)
		{
			// find button
			TBBUTTON tb;
			int inx=CommandToIndex(pToolText[i].idCommand);
			GetToolBarCtrl().GetButton(inx,&tb);

			// get text for button
			CString str;
			str.LoadString(pToolText[i].idString);

			// add a second NULL to string for AddStrings()
			int nLen = str.GetLength() + 1;
			TCHAR * pStr = str.GetBufferSetLength(nLen);
			pStr[nLen] = 0;

			// add new button using AddStrings
			tb.iString=GetToolBarCtrl().AddStrings(pStr);
			str.ReleaseBuffer();

			// (no ModifyButton() function)
			GetToolBarCtrl().DeleteButton(inx);
			GetToolBarCtrl().InsertButton(inx,&tb);
		}

		//注意，要给 下拉按钮 留出地方
		CSize sizeImage(50,31);	//原来按钮的大小
		CSize sizeButton(57,51);//加入文字后，按钮的大小
		SetSizes(sizeButton, sizeImage);
	}
	return bRet;
}
