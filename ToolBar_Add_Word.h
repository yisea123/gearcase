#if !defined(AFX_TOOLBAR_ADD_WORD_H__5339CD97_3E46_40A4_B2DB_266190B9DEC7__INCLUDED_)
#define AFX_TOOLBAR_ADD_WORD_H__5339CD97_3E46_40A4_B2DB_266190B9DEC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolBar_Add_Word.h : header file
//


//工具栏的资源 ID 和 工具栏的文字的字符串资源 相关联
typedef struct t_TOOLTEXT {
	UINT idCommand;
	UINT idString;
} TOOLTEXT;

/////////////////////////////////////////////////////////////////////////////
// CToolBar_Add_Word window

class CToolBar_Add_Word : public CToolBar
{
// Construction
public:
	CToolBar_Add_Word();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolBar_Add_Word)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL LoadToolBarEx(UINT nID,TOOLTEXT *pToolText,int nCnt);
	virtual ~CToolBar_Add_Word();

	// Generated message map functions
protected:
	//{{AFX_MSG(CToolBar_Add_Word)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLBAR_ADD_WORD_H__5339CD97_3E46_40A4_B2DB_266190B9DEC7__INCLUDED_)
