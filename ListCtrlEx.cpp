// ListCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "ListCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//CRITICAL_SECTION m_csLock;

/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrlEx
CHeaderCtrlEx::CHeaderCtrlEx()
	: marker_rect(0,0,0,0)
{
	m_pWidth = NULL;
	m_bDragging = FALSE;
	m_bCheckForDrag = FALSE;
	m_fpDragCol = NULL;
	m_pOwnerWnd = NULL;
	m_bDragEnable = false;

	m_bAutofit = false;

	m_nSortCol = -1;	//三角
}

CHeaderCtrlEx::CHeaderCtrlEx(CWnd *pWnd, void (CWnd::*fpDragCol)(int, int))
	: marker_rect(0,0,0,0)
{
	m_pWidth = NULL;
	m_bDragging = FALSE;
	m_bCheckForDrag = FALSE;
	m_fpDragCol = fpDragCol;
	m_pOwnerWnd = pWnd;
	m_bDragEnable = false;

	m_bAutofit = false;
}

CHeaderCtrlEx::~CHeaderCtrlEx()
{
}

BEGIN_MESSAGE_MAP(CHeaderCtrlEx, CHeaderCtrl)
	//{{AFX_MSG_MAP(CHeaderCtrlEx)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(HDN_ENDTRACK, OnEndTrack)
	ON_WM_SIZE()
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//	Public Properties
//
//	EnableDrag(bool bDrag=true):	Enable or disable column header dragging
//
/////////////////////////////////////////////////////////////////////////////

bool CHeaderCtrlEx::EnableDrag(bool bDrag)
{
	bool bOld = m_bDragEnable;
	m_bDragEnable = bDrag;

	return bOld;
}

//End Properties
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrlEx message handlers

void CHeaderCtrlEx::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bDragEnable) 
	{
		if( (MK_LBUTTON & nFlags) == 0)	
		{
			// The left mouse button is not pressed - so reset flags
			m_bCheckForDrag = FALSE;
			m_bDragging = FALSE;
		}
		else if( m_bDragging ) 
		{
			// Get column number that falls under the mouse
			int i=0, cx = 0;
			if( point.x > 0 )
				for( i = 0; i < GetItemCount(); i++ ) 
				{
					if( point.x >= cx && point.x < cx + m_pWidth[i] )
						break;
					cx += m_pWidth[i];
				}

			if( i != m_nDropPos ) 
			{
				m_nDropPos = i;
				CRect rect;
				GetWindowRect( &rect );
				// Invalidate area occupied by previous marker
				InvalidateRect( &marker_rect );

				// Draw a new marker
				CClientDC dc(this);
				POINT pts[3];
				pts[0].x = cx;
				pts[1].x = cx - rect.Height()/4;
				pts[2].x = cx + rect.Height()/4;
				pts[0].y = rect.Height();
				pts[1].y = pts[2].y = rect.Height()/2;
				dc.Polygon( pts, 3 );

				// save marker information
				marker_rect.left = cx - (rect.Height()/4 + 1);
				marker_rect.top = rect.Height() - (rect.Height()/2 + 1);
				marker_rect.right = cx + (rect.Height()/4 + 1);	
				marker_rect.bottom = rect.Height() + 1;
			}
			return;
		}
		else if( m_bCheckForDrag ) 
		{
			// The mouse button was pressed over a column header
			// and now the mouse has moved - so start drag
			m_bCheckForDrag = FALSE;
			m_bDragging = TRUE;
			m_nDropPos = m_nDragCol;
			SetCapture();

			// Store information for later use
			int iCount = GetItemCount();
			HD_ITEM hd_item;
			m_pWidth = new int[iCount];

			for( int i = 0; i < iCount; i++ ) 
			{
				hd_item.mask = HDI_WIDTH;
				GetItem( i, &hd_item );
				m_pWidth[i] = hd_item.cxy;
			}
			return;
		}
	}
	
	CHeaderCtrl::OnMouseMove(nFlags, point);
}

void CHeaderCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_bDragEnable) 
	{
		// Determine if mouse was pressed over a column header
		HD_HITTESTINFO hd_hittestinfo;
		hd_hittestinfo.pt = point;
		
		SendMessage(HDM_HITTEST, 0, (LPARAM)(&hd_hittestinfo));
		
		if( hd_hittestinfo.flags == HHT_ONHEADER )	
		{
			m_nDragCol = hd_hittestinfo.iItem;
			m_bCheckForDrag = TRUE;
		}
	}

	CHeaderCtrl::OnLButtonDown(nFlags, point);
}

void CHeaderCtrlEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bDragEnable) 
	{
		ASSERT( m_pOwnerWnd != NULL && m_fpDragCol != NULL );

		if( m_bDragging ) 
		{
			m_bDragging = FALSE;
			delete[] m_pWidth;
			ReleaseCapture();
			Invalidate();

			// Call the callback function.
			if( m_nDragCol != m_nDropPos && m_nDragCol != m_nDropPos -1 )
				(m_pOwnerWnd->*m_fpDragCol)( m_nDragCol, m_nDropPos );
		}
	}

	CHeaderCtrl::OnLButtonUp(nFlags, point);
}

void CHeaderCtrlEx::SetCallback( CWnd* pWnd, void (CWnd::*fpDragCol)(int, int) )
{
	m_fpDragCol = fpDragCol;
	m_pOwnerWnd = pWnd;
}

//End CHeaderCtrlEx
/////////////////////////////////////////////////////////////////////////////





//Constructor for the list data structure
CListCtrlEx::ListData::ListData(const DWORD dwOld, const CString &strTxt, const COleDateTime Start)
{
	dwOldData = dwOld;
	strText = strTxt;
	BaseTime = Start;
}

CListCtrlEx::CListCtrlEx()
{
	m_bDblMsgBox=TRUE;
	m_iCurSel		=-1;//初始化当前选择

	nSortedCol		= -1; 
	bSortAscending	= TRUE;	
	m_nNumCols		= 0;
	m_pTypes		= NULL;
	m_ctStart.SetStatus(COleDateTime::invalid);
	m_bSort			= false;
	
	m_nHighlight	= HIGHLIGHT_NORMAL;
	

	m_headerctrl.SetCallback( this, (void (CWnd::*)(int, int))DragColumn );

	InitializeCriticalSection(&m_csLock);
}

CListCtrlEx::~CListCtrlEx()
{
	DeleteCriticalSection(&m_csLock);
	delete m_pTypes;
}


BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlEx)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_NOTIFY(HDN_ITEMCLICKA, 0, OnHeaderClicked) 
	ON_NOTIFY(HDN_ITEMCLICKW, 0, OnHeaderClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//Public Properties
//
//	SetHighlightType(int hilite):		Set highlight type
//	EnableSort(bool bSort=true):		Enable sorting by column
//	SetColumnTypes(unsigned char* pColTypes, unsigned nColCnt):
//										Set the column sort types									
//	EnableHeaderDrag(bool bDrag=true}:	Enable column header dragging
//	SetBaseTime(CString strTime):		Set the base time for 24hr clock
//
/////////////////////////////////////////////////////////////////////////////

int CListCtrlEx::SetHighlightType(int hilite)
{
	int oldhilite = m_nHighlight;
	if( hilite <= HIGHLIGHT_ROW )
	{
		m_nHighlight = hilite;
		//set to ownerdraw for full row highlighting
		if(hilite > HIGHLIGHT_NORMAL)
			ModifyStyle(0, LVS_OWNERDRAWFIXED);
		Invalidate();
	}	
	
	return oldhilite;
}

bool CListCtrlEx::EnableSort(bool bSort)
{
	bool bOldSort = m_bSort;
	m_bSort = bSort;

	return bOldSort;
}

bool CListCtrlEx::SetColumnTypes(unsigned char* pColTypes, unsigned nColCnt)
{
	delete m_pTypes;
	m_pTypes = new unsigned char[nColCnt];
	if(m_pTypes == NULL) {
		m_nNumCols = 0;
		return false;
	}

	for(unsigned i=0; i<nColCnt; i++) {
		if(pColTypes[i] > LVEX_LAST) {
			m_nNumCols = 0;
			delete m_pTypes;
			return false;
		}
		m_pTypes[i] = pColTypes[i];
	}

	m_nNumCols = nColCnt;

	return true;
}

bool CListCtrlEx::EnableHeaderDrag(bool bDrag)
{
	return m_headerctrl.EnableDrag(bDrag);
}

bool CListCtrlEx::SetBaseTime(CString strTime)
{
	COleDateTime ctNew;
	//there can only be one base time for all columns
	//this function should only be called once
	ASSERT(m_ctStart.GetStatus() == COleDateTime::invalid);

	ctNew.ParseDateTime(strTime);
	if(ctNew.GetStatus() != COleDateTime::invalid) 
	{
		m_ctStart = ctNew;
		return true;
	}

	return false;
}

//End Properties
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Highlighting
//
/////////////////////////////////////////////////////////////////////////////

void CListCtrlEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rcItem(lpDrawItemStruct->rcItem);	
	int nItem = lpDrawItemStruct->itemID;
	CImageList* pImageList;	// Save dc state	
	int nSavedDC = pDC->SaveDC();
	CBrush WndBrush(::GetSysColor(COLOR_WINDOW));
	CBrush HltBrush(::GetSysColor(COLOR_HIGHLIGHT));

	// Get item image and state info	
	LV_ITEM lvi;
	lvi.mask = LVIF_IMAGE | LVIF_STATE;	
	lvi.iItem = nItem;	
	lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;		// get all state flags	
	GetItem(&lvi);

	// Should the item be highlighted
	BOOL bHighlight =((lvi.state & LVIS_DROPHILITED)
				|| ( (lvi.state & LVIS_SELECTED)			
				&& ((GetFocus() == this)
				|| (GetStyle() & LVS_SHOWSELALWAYS))));

	// Get rectangles for drawing	
	CRect rcBounds, rcLabel, rcIcon;
	GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
	GetItemRect(nItem, rcLabel, LVIR_LABEL);	
	GetItemRect(nItem, rcIcon, LVIR_ICON);
	CRect rcCol( rcBounds ); 
	CString sLabel = GetItemText( nItem, 0 );

	// Labels are offset by a certain amount  
	// This offset is related to the width of a space character
	int offset = pDC->GetTextExtent(_T(" "), 1 ).cx*2;	
	CRect rcHighlight;
	CRect rcWnd;	
	int nExt;	
	
	switch(m_nHighlight)	
	{	
		case HIGHLIGHT_NORMAL: 
			nExt = pDC->GetOutputTextExtent(sLabel).cx + offset;		
			rcHighlight = rcLabel;
			if( rcLabel.left + nExt < rcLabel.right )
				rcHighlight.right = rcLabel.left + nExt;		
			break;	
		case HIGHLIGHT_ALLCOLUMNS:
			rcHighlight = rcBounds;		
			rcHighlight.left = rcLabel.left;		
			break;	
		case HIGHLIGHT_ROW:
			GetClientRect(&rcWnd);		
			rcHighlight = rcBounds;
			rcHighlight.left = rcLabel.left;		
			rcHighlight.right = rcWnd.right;		
			break;
		default:	
			rcHighlight = rcLabel;	
	}	
	
	// Draw the background color
	if( bHighlight )	
	{		
		pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->FillRect(rcHighlight, &HltBrush);	
	}	
	else
	//pDC->FillRect(rcHighlight, &WndBrush);	//原来的没有背景的
	{//	加行背景
		CRect rcClient, rcRow = rcItem;
		GetClientRect(&rcClient);
		rcRow.right = rcClient.right;
		//pDC->FillRect(rcRow, &CBrush(nItem%2?::GetSysColor(COLOR_WINDOW):RGB(200,235,250)));
		pDC->FillRect(rcRow, &CBrush(nItem%2?RGB(245,255,240):RGB(230,240,255)));
	}

	// Set clip region	
	rcCol.right = rcCol.left + GetColumnWidth(0);	
	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rcCol);	
	pDC->SelectClipRgn(&rgn);
	rgn.DeleteObject();	

	//加列背景
	// Draw column background
	if( 0 && !bHighlight )
		pDC->FillRect(rcCol, &CBrush(RGB(215,215,215)));
	

	// Draw state icon	
	if (lvi.state & LVIS_STATEIMAGEMASK)	{
		int nImage = ((lvi.state & LVIS_STATEIMAGEMASK)>>12) - 1;
		pImageList = GetImageList(LVSIL_STATE);		
		if (pImageList) {
			pImageList->Draw(pDC, nImage,
				CPoint(rcCol.left, rcCol.top), ILD_TRANSPARENT);		
		}	
	}	

	// Draw normal and overlay icon	
	pImageList = GetImageList(LVSIL_SMALL);
	if (pImageList)	{		
		UINT nOvlImageMask=lvi.state & LVIS_OVERLAYMASK;
		pImageList->Draw(pDC, lvi.iImage, 			
			CPoint(rcIcon.left, rcIcon.top),
			(bHighlight?ILD_BLEND50:0) | ILD_TRANSPARENT | nOvlImageMask );	
	}		
	// Draw item label - Column 0	
	rcLabel.left += offset/2;
	rcLabel.right -= offset;
	pDC->DrawText(sLabel,-1,rcLabel,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP 
				| DT_VCENTER | DT_END_ELLIPSIS);	
	
	// Draw labels for remaining columns
	LV_COLUMN lvc;	
	lvc.mask = LVCF_FMT | LVCF_WIDTH;
	if(m_nHighlight == HIGHLIGHT_NORMAL)	// Highlight only first column	
	{
		pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));	
	}	
	rcBounds.right = rcHighlight.right > rcBounds.right ? rcHighlight.right :
							rcBounds.right;	rgn.CreateRectRgnIndirect(&rcBounds);
	pDC->SelectClipRgn(&rgn);				   
	for(int nColumn = 1; GetColumn(nColumn, &lvc); nColumn++)	
	{
		rcCol.left = rcCol.right;		
		rcCol.right += lvc.cx;

		/*加列背景
		// Draw column background
		if( !bHighlight || ( bHighlight && m_nHighlight == HIGHLIGHT_NORMAL ) )
			if( (nColumn+1) % 2 )
				pDC->FillRect(rcCol, &CBrush(RGB(225,225,225)));
		*/

		// Draw the background if needed		
		if( m_nHighlight == HIGHLIGHT_NORMAL )
			pDC->FillRect(rcCol, &HltBrush);
		sLabel = GetItemText(nItem, nColumn);		
		if (sLabel.GetLength() == 0)
			continue;		
		
		// Get the text justification		
		UINT nJustify = DT_LEFT;
		switch(lvc.fmt & LVCFMT_JUSTIFYMASK)		
		{		
			case LVCFMT_RIGHT:
				nJustify = DT_RIGHT;			
				break;		
			case LVCFMT_CENTER:			
				nJustify = DT_CENTER;
				break;		
			default:			
				break;		
		}		

		rcLabel = rcCol;		
		rcLabel.left += offset;
		rcLabel.right -= offset;

		pDC->DrawText(sLabel, -1, rcLabel, nJustify | DT_SINGLELINE | 
					DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS);	
	}

	// Draw focus rectangle if item has focus
	if (lvi.state & LVIS_FOCUSED && (GetFocus() == this))
		pDC->DrawFocusRect(rcHighlight);		
	
	// Restore dc	
	pDC->RestoreDC( nSavedDC );
	WndBrush.DeleteObject();
	HltBrush.DeleteObject();
}

void CListCtrlEx::OnPaint() 
{	
	// in full row select mode, we need to extend the clipping region
	// so we can paint a selection all the way to the right
	if (	m_nHighlight == HIGHLIGHT_ROW &&
			(GetStyle() & LVS_TYPEMASK) == LVS_REPORT )	
	{
		CRect rcBounds;
		GetItemRect(0, rcBounds, LVIR_BOUNDS);
		CRect rcClient;
		GetClientRect(&rcClient);

		if(rcBounds.right < rcClient.right)		
		{
			CPaintDC dc(this);
			CRect rcClip;
			dc.GetClipBox(rcClip);
			rcClip.left = min(rcBounds.right-1, rcClip.left);
			rcClip.right = rcClient.right;
			InvalidateRect(rcClip, FALSE);
		}
	}
	//CListCtrl::OnPaint();	//跟随上边的...


	//如果没有项目，则显示一句话--空列表
	Default();
    if (GetItemCount() <= 0)
    {
        COLORREF clrText = ::GetSysColor(COLOR_WINDOWTEXT);
        COLORREF clrTextBk = ::GetSysColor(COLOR_WINDOW);

        CDC* pDC = GetDC();
        // Save dc state
        int nSavedDC = pDC->SaveDC();

        CRect rc;
        GetWindowRect(&rc);
        ScreenToClient(&rc);

        CHeaderCtrl* pHC;
        pHC = GetHeaderCtrl();
        if (pHC != NULL)
        {
            CRect rcH;
            pHC->GetItemRect(0, &rcH);
            rc.top += rcH.bottom;
        }
        rc.top += 10;

        CString strText((LPCSTR)"-- blank --"); // The message you want!

        pDC->SetTextColor(clrText);
        pDC->SetBkColor(clrTextBk);
        pDC->FillRect(rc, &CBrush(clrTextBk));
        pDC->SelectStockObject(ANSI_VAR_FONT);
        pDC->DrawText(strText, -1, rc, 
                      DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP);

        // Restore dc
        pDC->RestoreDC(nSavedDC);
        ReleaseDC(pDC);
    }
    // Do not call CListCtrl::OnPaint() for painting messages

	//如果不是空表的话，则画垂直线 -- 不必，把线的颜色变淡了就行了
	//但是没有数据的地方不会自动更新，非常不好!
	if(0)
	{
		//列之间画垂直线
		// First let the control do its default drawing.
		const MSG *msg = GetCurrentMessage();
		DefWindowProc( msg->message, msg->wParam, msg->lParam );

		// Draw the lines only for LVS_REPORT mode
		if( (GetStyle() & LVS_TYPEMASK) == LVS_REPORT )
		{
			// Get the number of columns
			CClientDC dc(this );
			CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
			int nColumnCount = pHeader->GetItemCount();

			//因为没有内容的时候，都是垂直线很难看，所以用浅颜色的笔画
			//准备一个浅颜色的笔
			CPen Pen;
			COLORREF Color;
			DWORD PenStyle=PS_SOLID;
			Color=RGB(230,240,150);
			Pen.DeleteObject();//删除原来的画笔
			Pen.CreatePen(PenStyle,1,Color);
			dc.SelectObject(&Pen);//选择画笔
			
			// The bottom of the header corresponds to the top of the line 
			RECT rect;
			pHeader->GetClientRect( &rect );
			int top = rect.bottom;

			// Now get the client rect so we know the line length and
			// when to stop
			GetClientRect( &rect );

			// The border of the column is offset by the horz scroll
			int borderx = 0 - GetScrollPos( SB_HORZ );
			for( int i = 0; i < nColumnCount; i++ )
			{
				// Get the next border
				borderx += GetColumnWidth( i );

				// if next border is outside client area, break out
				if( borderx >= rect.right ) break;
				
				// Draw the line.
				dc.MoveTo( borderx-1, top);
				dc.LineTo( borderx-1, rect.bottom );
			}
		}
		
		// Do not call CListCtrl::OnPaint() for painting messages
	}//列之间画垂直线

	//CListCtrl::OnPaint();
}

void CListCtrlEx::OnSetFocus(CWnd* pOldWnd) 
{
	CListCtrl::OnSetFocus(pOldWnd);
	
	// check if we are getting focus from label edit box
	if(pOldWnd!=NULL && pOldWnd->GetParent()==this)
		return;
	// repaint items that should change appearance
	if((GetStyle() & LVS_TYPEMASK)==LVS_REPORT)
		RepaintSelectedItems();
}

void CListCtrlEx::OnKillFocus(CWnd* pNewWnd) 
{
	CListCtrl::OnKillFocus(pNewWnd);
	
	// check if we are losing focus to label edit box
	if(pNewWnd != NULL && pNewWnd->GetParent() == this)
		return;
	// repaint items that should change appearance
	if((GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
		RepaintSelectedItems();
}

void CListCtrlEx::RepaintSelectedItems()
{
	CRect rcBounds, rcLabel;

	// Invalidate focused item so it can repaint 
	int nItem = GetNextItem(-1, LVNI_FOCUSED);

	if(nItem != -1)
	{
		GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
		GetItemRect(nItem, rcLabel, LVIR_LABEL);
		rcBounds.left = rcLabel.left;

		InvalidateRect(rcBounds, FALSE);
	}

	// Invalidate selected items depending on LVS_SHOWSELALWAYS
	if(!(GetStyle() & LVS_SHOWSELALWAYS))
	{
		for(nItem = GetNextItem(-1, LVNI_SELECTED);
			nItem != -1; nItem = GetNextItem(nItem, LVNI_SELECTED))
		{
			GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
			GetItemRect(nItem, rcLabel, LVIR_LABEL);
			rcBounds.left = rcLabel.left;

			InvalidateRect(rcBounds, FALSE);
		}
	}

	UpdateWindow();
}

//places check marks in state icon boxes when clicked
void CListCtrlEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	UINT uFlags = 0;
	int nHitItem = HitTest(point, &uFlags);

	m_iCurSel=nHitItem;//当前正在选择的号码 0,1,...

	BOOL bHit = FALSE;
	if (uFlags & LVHT_ONITEMSTATEICON)
		bHit = TRUE;

	if (bHit) 
	{
		UINT nState = GetItemState(nHitItem, LVIS_STATEIMAGEMASK);

		if(nState == INDEXTOSTATEIMAGEMASK(1))
			SetItemState(nHitItem, INDEXTOSTATEIMAGEMASK(2), LVIS_STATEIMAGEMASK);
		else
			SetItemState(nHitItem, INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK);
	}
	
	CListCtrl::OnLButtonDown(nFlags, point);
}

//End Hilighting
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//Sorting by columns
//
//////////////////////////////////////////////////////////////////////

void CListCtrlEx::OnHeaderClicked(NMHDR* pNMHDR, LRESULT* pResult)
{
	//原来的排序方法，没具体看，因为有时候它判断成不排序，不太好
	{
		HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;

		if(phdn->iButton == 0 && m_bSort)// Button is the left mouse button 
		{
			// User clicked on header using left mouse button
			if(phdn->iItem == nSortedCol)
				bSortAscending = !bSortAscending;
			else
				bSortAscending = TRUE;

			nSortedCol = phdn->iItem;
			int NumItems = GetItemCount();	

			// replace Item data with pointer to ListData structure
			for (int i = 0; i < NumItems; i++)	
			{
				DWORD dwData = GetItemData(i); // save current data to restore it later
				CString strText = GetItemText(i, nSortedCol);
				
				SetItemData(i, (DWORD)new ListData(dwData, strText, m_ctStart));	
			}

			SortItems(Compare, bSortAscending | (m_pTypes[nSortedCol] << 8));

			for(i=0; i<NumItems; i++) 
			{
				ListData* pData = (ListData*)GetItemData(i);
				SetItemData(i, pData->dwOldData);
				delete pData;
			}
		}
		*pResult = 0;
	}

	//排序方法 2 
	//根据字符串来排序，所以 66 < 7 ，对数字不好。
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;

    if( phdn->iButton == 0 )
    {
            // User clicked on header using left mouse button
            if( phdn->iItem == nSortedCol )
                    bSortAscending = !bSortAscending;
            else
                    bSortAscending = TRUE;

            nSortedCol = phdn->iItem;
            SortTextItems( nSortedCol, bSortAscending );

    }
    *pResult = 0;

}

int CALLBACK CListCtrlEx::Compare(LPARAM lParam1, LPARAM lParam2, LPARAM lParSortAsc)
{
	CString str1 = ((ListData*)lParam1)->strText;
	CString str2 = ((ListData*)lParam2)->strText;

	unsigned char Type = unsigned char(lParSortAsc >> 8);

	// restore data type and sort order from lParamSort
	// if lParamSort positive - ascending sort order, negative - descending
	int  sOrder = (lParSortAsc & 0xFF) ? 1 : -1; 
	COleDateTime t1, t2, bTime = ((ListData*)lParam1)->BaseTime;	
	
	switch (Type)	
	{
		//float number columns
		case  LVEX_NUM_FLOAT:
			return (atof(str1) < atof(str2) ? -1 : 1)*sOrder;

		//Date time column(s) with 24 hr base time	
		case  LVEX_TIME_BASE://most people will never use this		
			if (t1.ParseDateTime(str1) && t2.ParseDateTime(str2))
			{
				if(bTime.GetStatus() != COleDateTime::invalid && 
					t1 < bTime)
				{
					t1 += COleDateTimeSpan(1, 0, 0, 0);
				}
				if(bTime.GetStatus() != COleDateTime::invalid &&
					t2 < bTime)
				{
					t2 += COleDateTimeSpan(1, 0, 0, 0);
				}
			
				return (t1 < t2 ? -1 : 1 )*sOrder;
			}
			else			
				return 0;

		//Date time column(s) no base time
		case  LVEX_TIME:
			if (t1.ParseDateTime(str1) && t2.ParseDateTime(str2))
				return (t1 < t2 ? -1 : 1 )*sOrder;		
			else			
				return 0;

		//integer number columns
		case  LVEX_NUM:		
			return (atol(str1) - atol(str2))*sOrder;

		//text columns (no case)
		case  LVEX_NOCASE:		
			return str1.CompareNoCase(str2)*sOrder;

		//text columns (case sensitive)
		case LVEX_CASE:
			return (str1.Compare(str2))*sOrder;	
	}

	return 0;
}

//End Sorting
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//Drag Column Headers
//
//////////////////////////////////////////////////////////////////////

void CListCtrlEx::DragColumn(int source, int dest)
{	
	TCHAR sColText[160];

	// Insert a column at dest
	LV_COLUMN lv_col;
	lv_col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv_col.pszText = sColText;
	lv_col.cchTextMax = 159;
	GetColumn( source, &lv_col );
	lv_col.iSubItem = dest;
	InsertColumn( dest, &lv_col );

	// Adjust source col number since it might have changed�

	// because a new column was inserted
	if( source > dest )
		source++;

	// Moving a col to position 0 is a special case	
	if( dest == 0 )
		for( int i = GetItemCount()-1; i > -1 ; i-- )
			SetItemText(i, 1, GetItemText( i, 0) );	
		
	// Copy sub item from source to dest
	for( int i = GetItemCount()-1; i > -1 ; i-- )
		SetItemText(i, dest, GetItemText( i, source ) );

	// Delete the source column, but not if it is the first	
	if( source != 0 )
		DeleteColumn( source );
	else {
		// If source col is 0, then copy col# 1 to col#0
		// and then delete col# 1
		GetColumn( 1, &lv_col );
		lv_col.iSubItem = 0;
		SetColumn( 0, &lv_col );
		for( int i = GetItemCount()-1; i > -1 ; i-- )
			SetItemText(i, 0, GetItemText( i, 1) );
		DeleteColumn( 1 );
	}
	
	Invalidate();
}

void CListCtrlEx::PreSubclassWindow() 
{
	CListCtrl::PreSubclassWindow();
	
	// Add initialization code
	m_headerctrl.SubclassWindow( ::GetDlgItem(m_hWnd,0) );

	// Add initialization code
	m_tooltip.Create( this );
}

//End Drag Headers
//////////////////////////////////////////////////////////////////////////

void CHeaderCtrlEx::Autofit(int nOverrideItemData /*= -1*/, int nOverrideWidth /*= 0*/)
{
	int nItemCount = GetItemCount();
	int nTotalWidthOfColumns = 0;
	int nDifferenceInWidht;
	int nItem;
	HD_ITEM hi;
	CRect rClient;

	if (!m_bAutofit)
		return;

	SetRedraw(FALSE);

	GetClientRect(&rClient);
	if (-1 != nOverrideItemData)
		rClient.right -= nOverrideWidth;

	// Get total width of all columns
	for (nItem = 0; nItem < nItemCount; nItem++)
	{
		if (nItem == nOverrideItemData)	// Don't mess with the item being resized by the user
			continue;

		hi.mask = HDI_WIDTH;
		GetItem(nItem, &hi);

		nTotalWidthOfColumns += hi.cxy;
	}

	if (nTotalWidthOfColumns != rClient.Width())
	{
		nDifferenceInWidht = abs(nTotalWidthOfColumns-rClient.Width());	// We need to shrink/expand all columns!
		
		// Shrink/expand all columns proportionally based on their current size
		for (nItem = 0; nItem < nItemCount; nItem++)
		{
			if (nItem == nOverrideItemData)	// Skip the overrride column if there is one!
				continue;
			
			hi.mask = HDI_WIDTH;
			GetItem(nItem, &hi);

			hi.mask = HDI_WIDTH;
			hi.cxy = (hi.cxy * rClient.Width()) / nTotalWidthOfColumns;

			SetItem(nItem, &hi);
		}
	}

	SetRedraw(TRUE);
	Invalidate();
}

void CHeaderCtrlEx::OnEndTrack(NMHDR * pNotifyStruct, LRESULT* result)
{
	HD_NOTIFY* pHDN = (HD_NOTIFY*)pNotifyStruct;

	Autofit(pHDN->iItem, pHDN->pitem->cxy);
}

void CHeaderCtrlEx::OnSize(UINT nType, int cx, int cy) 
{
	CHeaderCtrl::OnSize(nType, cx, cy);

	Autofit();
}

//在最右边加入新的列，比原来的方便!
// AddColumn		- 在最右边的列右边再增加一列
// 返回值			- 如果成功，返回新列的索引；失败，返回 -1 
// 参数:sColHeading - 列的标题
// 参数:nWidth		- 以象素为单位的列宽。如果参数为 -1 (不输入本参数),则和前一列同样宽
// 参数:nFormat		- 列的对齐方式：LVCFMT_LEFT(缺省), LVCFMT_RIGHT, or LVCFMT_CENTER.
int CListCtrlEx::AddColumn(LPCTSTR sColHeading, int nWidth  = -1, int nFormat  = LVCFMT_LEFT)
{
    CHeaderCtrlEx* pHeader = (CHeaderCtrlEx*)GetDlgItem(0);
    int nColumnCount = pHeader->GetItemCount();

    if( nWidth == -1 )	//和以前的列一样宽
    {
        // The line below return zero for ICON views
        //nWidth = GetColumnWidth( nColumnCount - 1 );

        // Get the column width of the previous column from header control
        HD_ITEM hd_item;
        hd_item.mask = HDI_WIDTH;               //indicate that we want the width
        pHeader->GetItem( nColumnCount - 1, &hd_item );
        nWidth = hd_item.cxy;
    }
    return InsertColumn( nColumnCount, sColHeading, nFormat, nWidth, nColumnCount );
}

//具有最小尺寸的自动适应内容的列宽
//If you don't supply a column number, it will resize all columns
void CListCtrlEx::AutoSizeColumns(int col)
{
     // Call this after your list control is filled
     SetRedraw(false);
     int mincol = col < 0 ? 0 : col;
     int maxcol = col < 0 ? GetColumnCount()-1 : col;

     for (col = mincol; col <= maxcol; col++) 
	 {
          SetColumnWidth(col,LVSCW_AUTOSIZE);
          int wc1 = GetColumnWidth(col);
          SetColumnWidth(col,LVSCW_AUTOSIZE_USEHEADER);
          int wc2 = GetColumnWidth(col);
          int wc = MAX(MINCOLWIDTH,MAX(wc1,wc2));
          SetColumnWidth(col,wc+20);	//这里我给加了+20，不然最后几个字显示成"..."了.
     }
     
	 // RecalcHeaderTips(); //*** uncomment this if you use my header tips method
     SetRedraw(true);
     
	 // Invalidate(); //*** uncomment this if you don't use my SetRedraw function}
}

int CListCtrlEx::GetColumnCount() const
{
     // get the header control
     CHeaderCtrlEx* pHeader = (CHeaderCtrlEx*)GetDlgItem(0);

     // return the number of items in it - ie. the number of columns
     return pHeader->GetItemCount();
}

//选择一个范围
// SelItemRange - Selects/Deselect a range of items
// Returns              - The number of new items selected
// bSelect              - TRUE to select, FALSE to deselect
// nFirstItem           - index of first item to select
// nLastItem            - index of last item to select
int CListCtrlEx::SelItemRange(BOOL bSelect, int nFirstItem, int nLastItem)
{
		// make sure nFirstItem and nLastItem are valid
        if( nFirstItem >= GetItemCount() || nLastItem >= GetItemCount() )
                return 0;

        int nItemsSelected = 0;
        int nFlags = bSelect ? 0 : LVNI_SELECTED;
        int nItem = nFirstItem - 1;
        while( (nItem = GetNextItem( nItem, nFlags )) >=0 && nItem <= nLastItem )
        {
                nItemsSelected++;
                SetItemState(nItem, bSelect ? LVIS_SELECTED : 0, LVIS_SELECTED );
        }
        return nItemsSelected;
}

//选择所有东西
void CListCtrlEx::SelectAllItems()
{
	LV_ITEM theItem;

	theItem.mask		= LVIF_STATE;
	theItem.iItem		= -1;
	theItem.iSubItem	= 0;
	theItem.state		= LVIS_SELECTED;
	theItem.stateMask	= 2;

	this->SetItemState(-1, &theItem);

	return;
}

BOOL CListCtrlEx::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent( pMsg );	
	return CListCtrl::PreTranslateMessage(pMsg);
}

//增加header的提示，可以用来告诉用户这一列的报警范围等信息
BOOL CListCtrlEx::AddHeaderToolTip(int nCol, LPCTSTR sTip)
{
const int TOOLTIP_LENGTH = 80;
	char buf[TOOLTIP_LENGTH+1];

	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if( nCol >= nColumnCount)
		return FALSE;

	if( (GetStyle() & LVS_TYPEMASK) != LVS_REPORT )
		return FALSE;

	// Get the header height
	RECT rect;
	pHeader->GetClientRect( &rect );
	int height = rect.bottom;

	RECT rctooltip;
	rctooltip.top = 0;
	rctooltip.bottom = rect.bottom;

	// Now get the left and right border of the column
	rctooltip.left = 0 - GetScrollPos( SB_HORZ );
	for( int i = 0; i < nCol; i++ )
		rctooltip.left += GetColumnWidth( i );
	rctooltip.right = rctooltip.left + GetColumnWidth( nCol );

	if( sTip == NULL )
	{
		// Get column heading
		LV_COLUMN lvcolumn;
		lvcolumn.mask = LVCF_TEXT;
		lvcolumn.pszText = buf;
		lvcolumn.cchTextMax = TOOLTIP_LENGTH;
		if( !GetColumn( nCol, &lvcolumn ) )
			return FALSE;
	}


	m_tooltip.AddTool( GetDlgItem(0), sTip ? sTip : buf, &rctooltip, nCol+1 );
	return TRUE;
}

BOOL CListCtrlEx::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
HD_NOTIFY	*pHDN = (HD_NOTIFY*)lParam;

	if((pHDN->hdr.code == HDN_ENDTRACKA || pHDN->hdr.code == HDN_ENDTRACKW))
	{
		// Update the tooltip info
		CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
		int nColumnCount = pHeader->GetItemCount();

		CToolInfo toolinfo;
		toolinfo.cbSize = sizeof( toolinfo );

		// Cycle through the tooltipinfo for each effected column
		for( int i = pHDN->iItem; i <= nColumnCount; i++ )
		{
			m_tooltip.GetToolInfo( toolinfo, pHeader, i + 1 );

			int dx;				// store change in width
			if( i == pHDN->iItem )
				dx = pHDN->pitem->cxy - 
					(toolinfo.rect.right - toolinfo.rect.left);
			else 
				toolinfo.rect.left += dx;
			toolinfo.rect.right += dx;
			m_tooltip.SetToolInfo( &toolinfo );
		}
	}
	
	return CListCtrl::OnNotify(wParam, lParam, pResult);
}

//将参数数组 aryCols 中的 arySize 个元素一起加到一个列表控件,
//用来初始化
//运用AddColumn()函数，一次加到右边             
//根据字数自动调整列宽	
void CListCtrlEx::AddColumnArray(char *aryCols[], int arySize)
{
	int iLenStr=MINCOLWIDTH;
	for(int i=0;i<arySize;i++)
	{
		iLenStr=strlen(aryCols[i]);
		AddColumn(aryCols[i],iLenStr*6+20);	//自动适应列宽
	}
}

//一次加入一整行 aryFullRow
//行元素的个数为列数
void CListCtrlEx::AddItemFullRow(char aryFullRow[][MAXLINELENGTH])
{
	/*
	//加入每行的项
	LV_ITEM lvItem;
	lvItem.mask=LVIF_TEXT|LVIF_STATE;
	lvItem.state=0;
	lvItem.stateMask=0;
	lvItem.iImage=0;
	lvItem.iSubItem=0;
	int i=InsertItem(&lvItem);

	for(int jj=0;jj<GetColumnCount();jj++)
	{
		SetItemText(i,jj,aryFullRow[jj]);	
	}
	*/
	//原来的方式InsertItem(&lvItem)时总出错;现改为如下代码没问题.houjun 20020919
	//houjun 20020919
	//CSingleLock sLock(&(m_mutex));
	//sLock.Lock();

	EnterCriticalSection(&m_csLock);
	LV_ITEM lvItem;
	lvItem.mask=LVIF_TEXT|LVIF_STATE;
	lvItem.state=0;
	lvItem.stateMask=0;
	lvItem.iImage=0;
	lvItem.iItem = GetItemCount();
	for(int jj=0;jj<GetColumnCount();jj++)
	{
		//SetItemText(i,jj,aryFullRow[jj]);	
		lvItem.iSubItem=jj;
		lvItem.pszText = aryFullRow[jj];
		if(jj==0)
		{
			if (InsertItem(&lvItem) == -1)
			{
				TRACE("InsertItem fail!");
			}
		}
		else 
		{
			if (SetItem(&lvItem) == 0)
				TRACE("SetItem fail!");
		}
	}
	LeaveCriticalSection(&m_csLock);
	//sLock.Unlock();

}

//根据 aryFrom 代表的每一个 Item 开始的位置，
//将 str 分成一个个的 Item 加入到 列表控件中
void CListCtrlEx::AddItemFullRowStr(char *str, int iFrom[], int iLastLen)
{
	//加入每行的项
	/*
	LV_ITEM lvItem;
	lvItem.mask=LVIF_TEXT|LVIF_STATE;
	lvItem.state=0;
	lvItem.stateMask=0;
	lvItem.iImage=0;
	lvItem.iSubItem=0;
	int i=InsertItem(&lvItem);
	
	char buf[2048] = "";	//缓冲长度，如果不够会出错
	int iLenBuf=0;
	for(int jj=0;jj<GetColumnCount()-1;jj++)	//先不考虑最后一个 Get..()-1
	{
		memset((char *)buf,0,sizeof(buf));
		iLenBuf=iFrom[jj+1]-iFrom[jj];
		strncpy(buf, &str[iFrom[jj]],iLenBuf);
		buf[iLenBuf]='\0';
		SetItemText(i,jj,buf);
	}

	//最后一个 Item 要单独考虑 jj已经+1了 
	if(iLastLen==-1)	//最后一列没有长度限制
		SetItemText(i,jj,&str[iFrom[jj]]);
	else				//将最后一列加上长度限制
	{
		memset((char *)buf,0,sizeof(buf));
		strncpy(buf,&str[iFrom[jj]],iLastLen);
		buf[iLastLen]='\0';
		SetItemText(i,jj,buf);
	}
	*/
	//houjun 20020919

	//原来的方式InsertItem(&lvItem)时总出错;现改为如下代码没问题.houjun 20020919
	//XString 还有点问题
	//CSingleLock sLock(&(m_mutex));
	//sLock.Lock();
	
	EnterCriticalSection(&m_csLock);
	
	LV_ITEM lvItem;
	lvItem.mask=LVIF_TEXT|LVIF_STATE;
	lvItem.state=0;
	lvItem.stateMask=0;
	lvItem.iImage=0;
	lvItem.iItem = GetItemCount();
	char buf[MAXLINELENGTH + 1] = "";	//缓冲长度，如果不够会出错
	int iLenBuf=0;
	int nColCnt = GetColumnCount();
	for(int jj=0;jj < nColCnt;jj++)
	{
		//SetItemText(i,jj,aryFullRow[jj]);	

		if (jj < nColCnt - 1)
		{
			memset((char *)buf,0,sizeof(buf));
			iLenBuf=iFrom[jj+1]-iFrom[jj];
			if (iLenBuf > MAXLINELENGTH)
				iLenBuf = MAXLINELENGTH;
			strncpy(buf, &str[iFrom[jj]],iLenBuf);
			buf[iLenBuf]='\0';
		}
		else
		{
			if(iLastLen==-1)
			{//最后一列没有长度限制
				//SetItemText(i,jj,&str[iFrom[jj]]);
				iLastLen = strlen(&str[iFrom[jj]]);
				if (iLenBuf > MAXLINELENGTH)
					iLenBuf = MAXLINELENGTH;
				strncpy(buf,&str[iFrom[jj]],iLastLen);
				buf[iLastLen]='\0';
			}
			else				//将最后一列加上长度限制
			{
				memset((char *)buf,0,sizeof(buf));
				if (iLastLen > MAXLINELENGTH)
					iLastLen = MAXLINELENGTH;
				strncpy(buf,&str[iFrom[jj]],iLastLen);
				buf[iLastLen]='\0';
			}

		}
		lvItem.iSubItem=jj;
		lvItem.pszText = buf;
		if(jj==0)
		{
			if (InsertItem(&lvItem) == -1)
			{
				TRACE("InsertItem fail!");
			}
		}
		else 
		{
			if (SetItem(&lvItem) == 0)
				TRACE("SetItem fail!");
		}
	}
	
	//sLock.Unlock();
	LeaveCriticalSection(&m_csLock);

}

//根据每一个元素的长度 iFromLen[i]，将 str 分别放到一行的每一项中
//就是用一下AddItemFullRowStr()，为了去掉空格方便而已
//AddItemFullRowStr()最后一列有了长度限制，可以重用它了:)
void CListCtrlEx::AddItemFullRowStrLen(char *str, int iFromLen[], int size)
{
	int *t;
	t = new int[size];

	t[0]=0;
	for(int jj=1;jj<GetColumnCount();jj++)
	{
		t[jj]=t[jj-1]+iFromLen[jj-1];
	}
	AddItemFullRowStr(str,t,iFromLen[jj-1]);
	delete []t;
}

//郭鹏提出的要求，鼠标左键双击，弹出相应行的内容，他同意模式的对话框。
//模式对话框并不影响程序内部的运行，
//模式对话框只是阻止用户操作，不阻止程序自己刷新界面
void CListCtrlEx::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CListCtrlEx::OnLButtonDown(nFlags, point);

	//如果m_bDblMsgBox为FALSE，则退出，不显示对话框行内容
	if(!m_bDblMsgBox)
		return;

	XString* x_sz=new XString;
	x_sz->Empty();

	int index;	//所点击的行
	if( ( index = HitTest( point)) != -1 )
	{
		m_iCurSel=index;//当前正在选择的号码 0,1,...

		UINT flag = LVIS_FOCUSED;
		if( (GetItemState( index, flag ) & flag) == flag )
		{
			*x_sz="第";
			*x_sz+=itoa(index+1);
			*x_sz+="行\n\n";

			//取每一类的标题
			
			int nMaxHeaderLength=MAXLINELENGTH;         
			XString Caption;
			HDITEM HdItem;
			HdItem.mask = HDI_TEXT; // just get the text
			HdItem.pszText = Caption.GetBuffer(nMaxHeaderLength);
			Caption.ReleaseBuffer();
			HdItem.cchTextMax = nMaxHeaderLength;
			
			for(int i=0;i<GetColumnCount();i++)
			{	
				/*
				//取回每列的标题
				//m_headerctrl.GetItem(i,&HdItem);
                 */
				//yuwc改动-------------2002.12.28
				//原来程序此处m_headerctrl在list控件不是用手动添加的,即用create函数动态创建
				//的情况下,获得不到headerctrl句柄,从而无法得到colomn的名称;

				//CHeaderCtrl* pheader = (CHeaderCtrl*)GetDlgItem(0);//这句话也一样有效
				CHeaderCtrl* pheader = GetHeaderCtrl();
				pheader->GetItem(i,&HdItem);
				/*
				//感觉下面的程序更好一些;
				if( m_headerctrl.m_hWnd == NULL )
				{
					CHeaderCtrl* pheader = (CHeaderCtrlEx*)GetDlgItem(0);;
					gBool = pheader->GetItem(i,&HdItem);
					//
					AfxMessageBox( "新的headerctrl" );
				}
				else
				{
					gBool = m_headerctrl.GetItem(i,&HdItem);
					AfxMessageBox( "旧的headerctrl\n" );
				}
				*/
/* 简易说明:
when the list view control is in the report it usually displays a header control to label the
 columns. often times we need to get hold of the header control. this header control is
 a child window of the list view control and always has the id 0. 
 cheaderctrl* pheader =(cheaderctrl*)m_listctrl.getdlgitem(0);
 the header control exists even if the list view is not in the report mode. when the list
 view mode is other that the lvs_report, the dimensions of the header control is zero. 
 */
				*x_sz+="(";
				*x_sz+=itoa(i+1);
				*x_sz+=") ";
				
				*x_sz+=Caption.GetBuffer(Caption.GetLength());	//不可以用 *x_sz+=Caption;
				Caption.ReleaseBuffer();
				*x_sz+=":";
				
				*x_sz+=GetItemText(index,i);
				*x_sz+="\n";
			}

			//一行的内容 全部显示
			AfxMessageBox(*x_sz);
		}
		else
			SetItemState( index, LVIS_SELECTED | LVIS_FOCUSED, 
					LVIS_SELECTED | LVIS_FOCUSED);
	}

	delete x_sz;
}

// SortTextItems	- Sort the list based on column text
// Returns		- Returns true for success
// nCol			- column that contains the text to be sorted
// bAscending		- indicate sort order
// low			- row to start scanning from - default row is 0
// high			- row to end scan. -1 indicates last row
BOOL CListCtrlEx::SortTextItems(int nCol, BOOL bAscending, int low /*= 0*/, int high /*= -1*/)
{
	if( nCol >= ((CHeaderCtrlEx*)GetDlgItem(0))->GetItemCount() )
		return FALSE;

	if( high == -1 ) high = GetItemCount() - 1;

	int lo = low;
	int hi = high;
	CString midItem;

	if( hi <= lo ) return FALSE;

	midItem = GetItemText( (lo+hi)/2, nCol );

	// loop through the list until indices cross
	while( lo <= hi )
	{
		// rowText will hold all column text for one row
		CStringArray rowText;

		// find the first element that is greater than or equal to 
		// the partition element starting from the left Index.
		if( bAscending )
			while( ( lo < high ) && ( GetItemText(lo, nCol) < midItem ) )
				++lo;
		else
			while( ( lo < high ) && ( GetItemText(lo, nCol) > midItem ) )
				++lo;

		// find an element that is smaller than or equal to 
		// the partition element starting from the right Index.
		if( bAscending )
			while( ( hi > low ) && ( GetItemText(hi, nCol) > midItem ) )
				--hi;
		else
			while( ( hi > low ) && ( GetItemText(hi, nCol) < midItem ) )
				--hi;

		// if the indexes have not crossed, swap
		// and if the items are not equal
		if( lo <= hi )
		{
			// swap only if the items are not equal
			if( GetItemText(lo, nCol) != GetItemText(hi, nCol))
			{
				// swap the rows
				LV_ITEM lvitemlo, lvitemhi;
				int nColCount = 
					((CHeaderCtrl*)GetDlgItem(0))->GetItemCount();
				rowText.SetSize( nColCount );
				int i;
				for( i=0; i<nColCount; i++)
					rowText[i] = GetItemText(lo, i);
				lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
				lvitemlo.iItem = lo;
				lvitemlo.iSubItem = 0;
				lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED | 
						LVIS_FOCUSED |  LVIS_SELECTED | 
						LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;

				lvitemhi = lvitemlo;
				lvitemhi.iItem = hi;

				GetItem( &lvitemlo );
				GetItem( &lvitemhi );

				for( i=0; i<nColCount; i++)
					SetItemText(lo, i, GetItemText(hi, i));

				lvitemhi.iItem = lo;
				SetItem( &lvitemhi );

				for( i=0; i<nColCount; i++)
					SetItemText(hi, i, rowText[i]);

				lvitemlo.iItem = hi;
				SetItem( &lvitemlo );
			}

			++lo;
			--hi;
		}
	}

	// If the right index has not reached the left side of array
	// must now sort the left partition.
	if( low < hi )
		SortTextItems( nCol, bAscending , low, hi);

	// If the left index has not reached the right side of array
	// must now sort the right partition.
	if( lo < high )
		SortTextItems( nCol, bAscending , lo, high );

	//设置可以在列头显示小三角，但是名字就没了：（
	//m_headerctrl.SetSortImage( nCol, bAscending );

	return TRUE;
}

int CHeaderCtrlEx::SetSortImage(int nCol, BOOL bAsc)
{
	int nPrevCol = m_nSortCol;

	m_nSortCol = nCol;
	m_bSortAsc = bAsc;

	// Change the item to ownder drawn
	HD_ITEM hditem;

	hditem.mask = HDI_FORMAT;
	GetItem( nCol, &hditem );
	hditem.fmt |= HDF_OWNERDRAW;
	SetItem( nCol, &hditem );

	// Invalidate header control so that it gets redrawn
	Invalidate();
	return nPrevCol;
}

//一次加入一个XS一列,共xLen列
void CListCtrlEx::AddColumnXS(XS xsbuf, int xLen)
{
//	CSingleLock sLock(&(m_mutex));
//	sLock.Lock();
	EnterCriticalSection(&m_csLock);

	//最小列宽
	int iLenStr=MINCOLWIDTH;
	char cbuf[MAXLINELENGTH+1] = "";

	for(int i=0;i<xLen;i++)
	{
		//strcpy(cbuf,xsbuf.Element(i).GetBuffer(2047));
		strcpy(cbuf,xsbuf.Element(i).GetBuffer(xsbuf.Element(i).GetLength()));
		iLenStr=strlen(xsbuf.Element(i));
		AddColumn(xsbuf.Element(i),iLenStr*6+20);	//自动适应列宽
		xsbuf.Element(i).ReleaseBuffer();
	}
	LeaveCriticalSection(&m_csLock);
	//sLock.Unlock();
}

//////////////////////////////////////////////////////////////////////
//一次加入一行，根据已知的列数，一次增加，最后如果还有的话，则忽略。
//我终于解决了一个月来最严重问题：为什么在win98下不能插入某些记录
//就是因为"|"的个数不够,如果元素为空的话。
//我要在这里把“|”补上，-- 太可怕了，留下了这么大的隐患。
void CListCtrlEx::AddItemFullRowXS(const XS xsbuf)
{
/*
	//加入每行的项
	LV_ITEM lvItem;
	lvItem.mask=LVIF_TEXT|LVIF_STATE;
	lvItem.state=0;
	lvItem.stateMask=0;
	lvItem.iImage=0;
	lvItem.iSubItem=0;

	//lvItem.iItem = GetItemCount();
	//InsertItem(&lvItem);
	int i=InsertItem(&lvItem);
	if(i<0)
		i++;

	//如果缓冲区设置大了（如：8192），则会出现 InsertItem()返回-1，出错的情况!!!
	char cbuf[8192]="";
	for(int jj=0;jj<GetColumnCount();jj++)
	{	
		if(!xsbuf(jj).IsEmpty())
		{
			strcpy(cbuf,xsbuf(jj).GetBuffer(xsbuf(jj).GetLength()));
		}
		else
		{	
			strcpy(cbuf,"");
		}
		lvItem.pszText = cbuf;
		//lvItem.iSubItem = jj;
		//InsertItem(&lvItem);
		SetItemText(i,jj,cbuf);
		xsbuf(jj).ReleaseBuffer();
	}
	*/

	//加入每行的项
	//原来的方式InsertItem(&lvItem)时总出错;现改为如下代码没问题.houjun 20020919
//	CSingleLock sLock(&(m_mutex));
	
//	sLock.Lock();
	EnterCriticalSection(&m_csLock);
	LV_ITEM lvItem;
	lvItem.mask=LVIF_TEXT|LVIF_STATE;
	lvItem.state=0;
	lvItem.stateMask=0;
	lvItem.iImage=0;
	lvItem.iSubItem=0;
	lvItem.iItem = GetItemCount();

	//char cbuf[1024] = "";
	CString strBuff;
	int nBuffLeng = 0;
	for(int jj=0;jj<GetColumnCount();jj++)
	{
		//SetItemText(i,jj,aryFullRow[jj]);	
		//memset(cbuf,0,1024);
		if(!xsbuf(jj).IsEmpty())
		{
			strBuff = xsbuf(jj);
		}
		else
		{	
			strBuff = "";
		}
		nBuffLeng = strBuff.GetLength();
		if (nBuffLeng > MAXLINELENGTH)
			nBuffLeng = MAXLINELENGTH;

		lvItem.iSubItem=jj;
		lvItem.pszText = strBuff.GetBuffer(nBuffLeng);
		int nRet = 0;
		if(jj==0)
		{
			nRet = InsertItem(&lvItem);
			if (nRet == -1)
			{
				printf("InsertItem fail!");
			}
		}
		else 
		{
			nRet = SetItem(&lvItem);
			if (nRet == 0)
				printf("SetItem fail!");
		}
		xsbuf(jj).ReleaseBuffer();
	}
	//sLock.Unlock();
	LeaveCriticalSection(&m_csLock);
}

//删除所有的字段
void CListCtrlEx::DeleteAllColumns()
{
	int iNum=GetColumnCount();
	
	for(int i=0;i<iNum;i++)
		DeleteColumn(0);
}

BOOL CListCtrlEx::DeleteItem(int nItem)
{
	//CSingleLock sLock(&(m_mutex));
	//sLock.Lock();
	EnterCriticalSection(&m_csLock);
	BOOL bFlag=CListCtrl::DeleteItem(nItem);
	m_iCurSel=-1;

	//sLock.Unlock();
	LeaveCriticalSection(&m_csLock);
	return bFlag;
}

//鼠标右键单击落下－－相当于左键单击
void CListCtrlEx::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CListCtrlEx::OnLButtonDown(nFlags, point);
}

//鼠标右键双击－－相当于左键单击
void CListCtrlEx::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	CListCtrlEx::OnLButtonDown(nFlags, point);
}

//得到当前选定的行的编号，从0开始，-1表示没有选中
int CListCtrlEx::P_Get_Current_Sel()
{
	if(m_iCurSel<0)
		return -1;
	else
		return m_iCurSel;
}

//返回当前行的全部内容
XString CListCtrlEx::P_Get_Current_Line()
{
	int iSel=m_iCurSel;
	XString Buf,Ret("");
	if(iSel<0)
		return Ret;
	
	for(int i=0;i<GetColumnCount();i++)
	{
		Buf.Format("%s|",GetItemText(iSel,i));
		Ret+=Buf;
	}
	
	Ret--;
	
	return Ret;
}

//返回 t_iNum 行的全部内容
XString CListCtrlEx::P_Get_Line_Of(int t_iNum)
{
	XString Buf,Ret("");
	if(t_iNum<0)
		return Ret;
	
	for(int i=0;i<GetColumnCount();i++)
	{
		Buf.Format("%s|",GetItemText(t_iNum,i));
		Ret+=Buf;
	}
	
	Ret--;
	
	return Ret;	
}

//设置是否双击显示行内容
void CListCtrlEx::P_Set_DlgMsgBox(BOOL t_bFlag)
{
	m_bDblMsgBox=t_bFlag;
}

void CListCtrlEx::AddItemFullRowXS(const XS xsbuf, char sep)
{
	/*
	xsbuf.SetSeparator(sep);
	//加入每行的项
	LV_ITEM lvItem;
	lvItem.mask=LVIF_TEXT|LVIF_STATE;
	lvItem.state=0;
	lvItem.stateMask=0;
	lvItem.iImage=0;
	lvItem.iSubItem=0;
	int i=InsertItem(&lvItem);
	if(i<0)
		i++;

	//如果缓冲区设置大了（如：8192），则会出现 InsertItem()返回-1，出错的情况!!!
	char cbuf[8192] = "";
	for(int jj=0;jj<GetColumnCount();jj++)
	{	
		if(!xsbuf(jj).IsEmpty())
		{
			strcpy(cbuf,xsbuf(jj).GetBuffer(xsbuf(jj).GetLength()));
		}
		else
		{	
			strcpy(cbuf,"");
		}
		SetItemText(i,jj,cbuf);
		xsbuf(jj).ReleaseBuffer();
	}
	xsbuf.RestoreSeparator();
	*/
	
	//原来的方式InsertItem(&lvItem)时总出错;现改为如下代码没问题.houjun 20020919

	//CSingleLock sLock(&(m_mutex));
	//sLock.Lock();
	EnterCriticalSection(&m_csLock);
	//xsbuf.SetSeparator(sep);
	//加入每行的项
	LV_ITEM lvItem;
	lvItem.mask=LVIF_TEXT|LVIF_STATE;
	lvItem.state=0;
	lvItem.stateMask=0;
	lvItem.iImage=0;
	lvItem.iItem = GetItemCount();

	char cbuf[MAXLINELENGTH+1] = "";
	int lBuffLeng = 0;
	for(int jj=0;jj<GetColumnCount();jj++)
	{
		//SetItemText(i,jj,aryFullRow[jj]);	
		memset(cbuf,0,MAXLINELENGTH);
		if(!xsbuf(jj,sep).IsEmpty())
		{
			lBuffLeng = xsbuf(jj,sep).GetLength();
			if (lBuffLeng > MAXLINELENGTH)
				lBuffLeng = MAXLINELENGTH;
			strcpy(cbuf,xsbuf(jj,sep).GetBuffer(lBuffLeng));
		}
		else
		{	
			strcpy(cbuf,"");
		}
		cbuf[lBuffLeng] = 0;
		lvItem.iSubItem=jj;
		lvItem.pszText = cbuf;
		if(jj==0)
		{
			if (InsertItem(&lvItem) == -1)
			{
				TRACE("InsertItem fail!");
			}
		}
		else 
		{
			if (SetItem(&lvItem) == 0)
				TRACE("SetItem fail!");
		}
		xsbuf(jj,sep).ReleaseBuffer();
	}
	//xsbuf.RestoreSeparator();
	
	//sLock.Unlock();
	LeaveCriticalSection(&m_csLock);
}
