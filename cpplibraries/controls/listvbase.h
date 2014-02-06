#if !defined(AFX_LISTVBASE_H__C69D6E13_4698_11D9_A797_97922892BD71__INCLUDED_)
#define AFX_LISTVBASE_H__C69D6E13_4698_11D9_A797_97922892BD71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// listvbase.h : header file
//
#include "listhelper.h"
#include <comutil.h>
#include <afxcview.h>

/////////////////////////////////////////////////////////////////////////////
// CListVBase view

class CListVBase : public CListView
{
protected:
	CListVBase();           // protected constructor used by dynamic creation
	DECLARE_DYNAMIC(CListVBase)

// Attributes
protected:
	ColumnParamsLst m_columns;
	CListHelper m_listhelper;
	_bstr_t m_tipstr;
public:

// Operations
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListVBase)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CListVBase();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	virtual void set_GrigProperties() {};
	virtual GridDataSource* get_GridDataSource() = 0;
	virtual GridDataSetter* get_GridDataSetter() {return NULL;}
	virtual GridCellInformer* get_GridCellInformer() {return NULL;}
	//{{AFX_MSG(CListVBase)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginTrack(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHeaderClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTrack(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	//afx_msg void OnEndtrack(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnSetBkColor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetImageList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetTextColor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetTextBkColor(WPARAM wParam, LPARAM lParam);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetTipTextA(UINT id,NMHDR* pNMHDR,LRESULT* pResult);
	afx_msg void OnGetTipTextW(UINT id,NMHDR* pNMHDR,LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CListCBase list control

class CListCBase : public CListCtrl
{
protected:
	CListCBase();           // protected constructor used by dynamic creation
	DECLARE_DYNAMIC(CListCBase)

// Attributes
protected:
	ColumnParamsLst m_columns;
	CListHelper m_listhelper;
	_bstr_t m_tipstr;
public:

// Operations
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCBase)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CListCBase();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	void InitCtrl();
	// Generated message map functions
protected:
	virtual void set_GrigProperties() {};
	virtual GridDataSource* get_GridDataSource() = 0;
	virtual GridDataSetter* get_GridDataSetter() {return NULL;}
	virtual GridCellInformer* get_GridCellInformer() {return NULL;}
	//{{AFX_MSG(CListCBase)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginTrack(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHeaderClick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg LRESULT OnSetBkColor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetImageList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetTextColor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetTextBkColor(WPARAM wParam, LPARAM lParam);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetTipTextA(UINT id,NMHDR* pNMHDR,LRESULT* pResult);
	afx_msg void OnGetTipTextW(UINT id,NMHDR* pNMHDR,LRESULT* pResult);
	afx_msg void OnTrack(NMHDR* pNMHDR, LRESULT* pResult); 
	DECLARE_MESSAGE_MAP()
public:
	void refresh();
};


class CListCBaseImpl:public CListCBase
{
protected:
	GridDataSource* m_pgriddatasource;
public:
	CListCBaseImpl();

	void Attach(GridDataSource* _pgriddatasource);
	void refresh();
	virtual GridDataSource* get_GridDataSource();
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTVBASE_H__C69D6E13_4698_11D9_A797_97922892BD71__INCLUDED_)
