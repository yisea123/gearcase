#if !defined(AFX_LISTCTRLEX_H__8F7ECD33_C404_4479_95A6_89C6A9B543DC__INCLUDED_)
#define AFX_LISTCTRLEX_H__8F7ECD33_C404_4479_95A6_89C6A9B543DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrlEx.h : header file
//

#include "XString.h"


#define HIGHLIGHT_NORMAL		0
#define HIGHLIGHT_ALLCOLUMNS	1
#define HIGHLIGHT_ROW			2

#define LVEX_CASE				0
#define LVEX_NOCASE				1
#define LVEX_NUM				2	//integer
#define LVEX_TIME				3
#define LVEX_NOSORT				4
#define LVEX_NUM_FLOAT			5
#define LVEX_TIME_BASE			6	//probably will never use this
#define LVEX_LAST				6	//increment this if you add more types

#define MAXLINELENGTH 4096


#define MINCOLWIDTH 10  /* or whatever */
#define MAX(x,y)	(x)>(y)?(x):(y)

#define NUM_LOGCOLUMNS	4

static unsigned char _gnLogColTypes[NUM_LOGCOLUMNS] = 
{
	LVEX_NUM, LVEX_CASE, LVEX_TIME, LVEX_NOCASE
};


/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx window




class CHeaderCtrlEx : public CHeaderCtrl
{
	// Construction
public:	
	CHeaderCtrlEx();
	CHeaderCtrlEx(CWnd* pWnd, void (CWnd::*fpDragCol)(int, int));
	
	// Attributes
	bool EnableDrag(bool bDrag=true);

public:
	// Operations

public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHeaderCtrlEx)
	//}}AFX_VIRTUAL
	
	// Implementation


protected:
	int	m_nSortCol;		//小三角
	BOOL	m_bSortAsc;	//小三角

public:
	virtual ~CHeaderCtrlEx();
	void SetCallback(CWnd* pWnd, void (CWnd::*fpDragCol)(int, int));

protected:
	bool	m_bDragEnable;

	BOOL	m_bCheckForDrag;
	BOOL	m_bDragging;
	int		*m_pWidth;
	int		m_nDragCol;
	int		m_nDropPos;
	CRect	marker_rect;
	void	(CWnd::*m_fpDragCol)(int, int);
	CWnd	*m_pOwnerWnd;

private:
	bool m_bAutofit;

private:
	void Autofit(int nOverrideItemData = -1, int nOverrideWidth = 0);

public:
	int SetSortImage( int nCol, BOOL bAsc );
	void SetAutofit(bool bAutofit = true) { m_bAutofit = bAutofit; Autofit(); }

	// Generated message map functions
protected:
	//{{AFX_MSG(CHeaderCtrlEx)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEndTrack(NMHDR * pNotifyStruct, LRESULT* result);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////



class CListCtrlEx : public CListCtrl
{
// Construction
public:	
	CListCtrlEx();
	
	// Attributes
	int SetHighlightType(int hilite);
	bool EnableSort(bool bSort=true);
	bool SetColumnTypes(unsigned char* pColTypes, unsigned nColCnt);
	bool EnableHeaderDrag(bool bDrag=true);
	bool SetBaseTime(CString strTime);
	
	CToolTipCtrl	m_tooltip;	//header 提示，用来显示给用户报警范围等信息:)

	CRITICAL_SECTION m_csLock;

public:
	// Operations	
	
protected:	
	int  m_nHighlight;		// Indicate type of selection highlighting
	bool m_bSort;			//set to false if you don't want to sort
	unsigned char* m_pTypes;//column sort types
	int m_nNumCols;			//number of columns to sort
	COleDateTime m_ctStart;	//base time for sorting on 24hr clock
	
	CMutex m_mutex;

	int nSortedCol;	
	BOOL bSortAscending;
	CHeaderCtrlEx	m_headerctrl;
	
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void RepaintSelectedItems();
	void DragColumn(int source, int dest);
	
	static int CALLBACK Compare(LPARAM lParam1, LPARAM lParam2, LPARAM 
		lParamSort);

	//data structure for setdata
	struct ListData
	{
		ListData(const DWORD dwOld, const CString &strTxt, const COleDateTime Start);
		DWORD dwOldData;
		CString strText;
		COleDateTime BaseTime;
	public:
	};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlEx)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	void AddItemFullRowXS(const XS xsbuf,char sep);
	void P_Set_DlgMsgBox(BOOL t_bFlag);
	XString P_Get_Line_Of(int t_iNum);
	XString P_Get_Current_Line();
	int P_Get_Current_Sel();
	BOOL DeleteItem(int nItem);
	void DeleteAllColumns();
	int m_iCurSel;
	void AddItemFullRowXS(const XS xsbuf);
	void AddColumnXS(XS xsBuf,int xLen);
	BOOL SortTextItems( int nCol, BOOL bAscending, int low = 0, int high = -1);
	void AddItemFullRowStrLen(char *str, int iFromLen[],int);
	void AddItemFullRowStr(char *str, int iFrom[], int iLastLen = -1);
	void AddItemFullRow(char aryFullRow[][MAXLINELENGTH]);
	void AddColumnArray(char *aryCols[],int arySize);
	BOOL AddHeaderToolTip(int nCol, LPCTSTR sTip);
	void SelectAllItems();
	int SelItemRange(BOOL bSelect, int nFirstItem, int nLastItem);
	int GetColumnCount() const;
	void AutoSizeColumns(int col = -1);
	int AddColumn(LPCTSTR sColHeading, int nWidth, int nFormat);
	virtual ~CListCtrlEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlEx)
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg void OnHeaderClicked(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bDblMsgBox;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLEX_H__8F7ECD33_C404_4479_95A6_89C6A9B543DC__INCLUDED_)
