// listvbase.cpp : implementation file
//

#include "stdafx.h"
#include "listvbase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListVBase

IMPLEMENT_DYNAMIC(CListVBase, CListView)

CListVBase::CListVBase()
{
}

CListVBase::~CListVBase()
{
}


BEGIN_MESSAGE_MAP(CListVBase, CListView)
	//{{AFX_MSG_MAP(CListVBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(HDN_TRACK, OnTrack)
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDITA, OnEndLabelEdit)
	ON_MESSAGE(LVM_SETIMAGELIST, OnSetImageList)
	ON_MESSAGE(LVM_SETTEXTCOLOR, OnSetTextColor)
	ON_MESSAGE(LVM_SETTEXTBKCOLOR, OnSetTextBkColor)
	ON_MESSAGE(LVM_SETBKCOLOR, OnSetBkColor)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGING, OnItemChanging)
	ON_NOTIFY(HDN_BEGINTRACKA,0, OnBeginTrack)
	ON_NOTIFY(HDN_BEGINTRACKW,0, OnBeginTrack)
	ON_NOTIFY(HDN_ITEMCLICK,0, OnHeaderClick)
	ON_NOTIFY_RANGE(TTN_NEEDTEXTW,0,0xffff,OnGetTipTextW)
	ON_NOTIFY_RANGE(TTN_NEEDTEXTA,0,0xffff,OnGetTipTextA)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListVBase drawing

void CListVBase::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CListVBase diagnostics

#ifdef _DEBUG
void CListVBase::AssertValid() const
{
	CListView::AssertValid();
}

void CListVBase::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CListVBase message handlers

int CListVBase::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	m_listhelper.Attach((CWnd*)this);
	m_listhelper.Attach(&GetListCtrl());
	set_GrigProperties();
	GridDataSource* pgds = get_GridDataSource();
	if(NOT_NULL(pgds))
	{
		m_listhelper.InitGrid(*pgds,GetWindowDC(),&m_columns);
	}
	return 0;
}

void CListVBase::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	GridDataSource* pgds = get_GridDataSource();
	if(NOT_NULL(pgds))
		m_listhelper.DrawItem(lpDrawItemStruct,*pgds);
}

BOOL CListVBase::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_listhelper.PreCreateWindow(cs);
	return CListView::PreCreateWindow(cs);
}

void CListVBase::OnSize(UINT nType, int cx, int cy) 
{
	CListView::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	m_listhelper.OnSize(nType,cx,cy);
}

void CListVBase::OnPaint() 
{
	m_listhelper.OnPaint();
	CListView::OnPaint();
//	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CListView::OnPaint() for painting messages
}

void CListVBase::OnSetFocus(CWnd* pOldWnd) 
{
	CListView::OnSetFocus(pOldWnd);
	// TODO: Add your message handler code here
	m_listhelper.OnSetFocus(pOldWnd);
}

void CListVBase::OnKillFocus(CWnd* pNewWnd) 
{
	CListView::OnKillFocus(pNewWnd);
	// TODO: Add your message handler code here
	m_listhelper.OnKillFocus(pNewWnd);
}

LRESULT CListVBase::OnSetImageList(WPARAM wParam, LPARAM lParam)
{
	m_listhelper.OnSetImageList(wParam,lParam);
	return (Default());
}

LRESULT CListVBase::OnSetTextColor(WPARAM wParam, LPARAM lParam)
{
	m_listhelper.OnSetTextColor(wParam,lParam);
	return (Default());
}

LRESULT CListVBase::OnSetTextBkColor(WPARAM wParam, LPARAM lParam)
{
	m_listhelper.OnSetTextBkColor(wParam,lParam);
	return (Default());
}

LRESULT CListVBase::OnSetBkColor(WPARAM wParam, LPARAM lParam)
{
	m_listhelper.OnSetBkColor(wParam,lParam);
	return (Default());
}


void CListVBase::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_listhelper.OnLButtonDblClk(nFlags,point,*get_GridDataSource()
		,get_GridDataSetter(),get_GridCellInformer())) return;
	CListView::OnLButtonDblClk(nFlags, point);
}

void CListVBase::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_listhelper.OnLButtonDown(nFlags,point,*get_GridDataSource()
		,get_GridDataSetter(),get_GridCellInformer())) return;
	CListView::OnLButtonDown(nFlags, point);
//	CListCtrl& lc = GetListCtrl();
//	int i;
//	TRACE("CListVBase::OnLButtonDown()\n");
//	for(i=0;i<lc.GetItemCount();i++)
//	{
//		DWORD state = lc.GetItemState(i,0xffff);
//		TRACE("%d = %08x\n",i,state);
//	}
}

void CListVBase::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(NOT_NULL(get_GridDataSetter()))
		m_listhelper.OnEndlabelEdit((LPCTSTR)TRUE,*get_GridDataSetter());
	CListView::OnLButtonUp(nFlags, point);
}

void CListVBase::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_listhelper.OnKeyDown(nChar,nRepCnt,nFlags,*get_GridDataSource()
		,get_GridDataSetter(),get_GridCellInformer())) return;
	CListView::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CListVBase::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	if(NOT_NULL(get_GridDataSetter()))
		m_listhelper.OnEndlabelEdit((LPCTSTR)TRUE,*get_GridDataSetter());
	return CListView::OnMouseWheel(nFlags, zDelta, pt);
}

void CListVBase::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	LockWindowUpdate();
	m_listhelper.OnHScroll(nSBCode, nPos, pScrollBar,*get_GridDataSource(),get_GridDataSetter(),GetWindowDC(),&m_columns);
	CListView::OnHScroll(nSBCode, nPos, pScrollBar);
	UnlockWindowUpdate();
}

void CListVBase::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	LockWindowUpdate();
	// TODO: Add your message handler code here and/or call default
	m_listhelper.OnVScroll(nSBCode, nPos, pScrollBar,*get_GridDataSource(),get_GridDataSetter(),GetWindowDC(),&m_columns);
	CListView::OnVScroll(nSBCode, nPos, pScrollBar);
	//m_listhelper.PostVScroll(*get_GridDataSource());
	UnlockWindowUpdate();
}

void CListVBase::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(NOT_NULL(get_GridDataSetter()))
		m_listhelper.OnEndlabelEdit((LPCTSTR)TRUE,*get_GridDataSetter());
	CListView::OnRButtonDown(nFlags, point);
}

void CListVBase::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CListView::OnRButtonUp(nFlags, point);
}

BOOL CListVBase::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_listhelper.OnEraseBkgnd(pDC)) return TRUE;
	return CListView::OnEraseBkgnd(pDC);
}

void CListVBase::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_listhelper.OnItemChanged(pNMHDR,*get_GridDataSource());
}

void CListVBase::OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult)
{
}

void CListVBase::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	if(NOT_NULL(get_GridDataSetter()))
		m_listhelper.OnEndlabelEdit(pDispInfo->item.pszText,*get_GridDataSetter());
	*pResult = 0;
}

void CListVBase::OnBeginTrack(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
	if(NOT_NULL(get_GridDataSetter()))
		m_listhelper.OnEndlabelEdit((LPCTSTR)TRUE,*get_GridDataSetter());
	*pResult = 0;
}

void CListVBase::OnHeaderClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
	m_listhelper.OnHeaderClicked(phdn->iItem,*get_GridDataSource()
		,get_GridDataSetter(),GetWindowDC(),&m_columns);
	*pResult = 0;
}
///////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CListCBase

IMPLEMENT_DYNAMIC(CListCBase, CListCtrl)

CListCBase::CListCBase()
{
}

CListCBase::~CListCBase()
{
}


BEGIN_MESSAGE_MAP(CListCBase, CListCtrl)
	//{{AFX_MSG_MAP(CListCBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(HDN_TRACK, OnTrack)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDITA, OnEndLabelEdit)
	ON_MESSAGE(LVM_SETIMAGELIST, OnSetImageList)
	ON_MESSAGE(LVM_SETTEXTCOLOR, OnSetTextColor)
	ON_MESSAGE(LVM_SETTEXTBKCOLOR, OnSetTextBkColor)
	ON_MESSAGE(LVM_SETBKCOLOR, OnSetBkColor)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGING, OnItemChanging)
	ON_NOTIFY(HDN_BEGINTRACKA,0, OnBeginTrack)
	ON_NOTIFY(HDN_BEGINTRACKW,0, OnBeginTrack)
	ON_NOTIFY(HDN_ITEMCLICK,0, OnHeaderClick)
	ON_NOTIFY_RANGE(TTN_NEEDTEXTW,0,0xffff,OnGetTipTextW)
	ON_NOTIFY_RANGE(TTN_NEEDTEXTA,0,0xffff,OnGetTipTextA)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCBase drawing

/////////////////////////////////////////////////////////////////////////////
// CListCBase diagnostics

#ifdef _DEBUG
void CListCBase::AssertValid() const
{
	CListCtrl::AssertValid();
}

void CListCBase::Dump(CDumpContext& dc) const
{
	CListCtrl::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CListCBase message handlers

int CListCBase::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	m_listhelper.Attach((CWnd*)this);
	m_listhelper.Attach((CListCtrl*)this);
	set_GrigProperties();
	GridDataSource* pgds = get_GridDataSource();
	if(NOT_NULL(pgds))
	{
		m_listhelper.InitGrid(*pgds,GetWindowDC(),&m_columns);
	}
	return 0;
}

void CListCBase::InitCtrl()
{
	if(IS_NULL(&m_listhelper.GetListCtrl()))
	{
		m_listhelper.Attach((CWnd*)this);
		m_listhelper.Attach((CListCtrl*)this);
		set_GrigProperties();
		GridDataSource* pgds = get_GridDataSource();
		if(NOT_NULL(pgds))
		{
			m_listhelper.InitGrid(*pgds,GetWindowDC(),&m_columns);
		}
	}
}

void CListCBase::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	GridDataSource* pgds = get_GridDataSource();
	if(NOT_NULL(pgds))
		m_listhelper.DrawItem(lpDrawItemStruct,*pgds);
}

void CListCBase::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	//m_listhelper.PreSubcclassWindow();
	//m_listhelper.Attach((CWnd*),this);
	//m_listhelper.Attach((CListCtrl*)this);
	CListCtrl::PreSubclassWindow();
}

void CListCBase::OnSize(UINT nType, int cx, int cy) 
{
	CListCtrl::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	m_listhelper.OnSize(nType,cx,cy);
}

void CListCBase::OnPaint() 
{
	m_listhelper.OnPaint();
	CListCtrl::OnPaint();
//	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CListCtrl::OnPaint() for painting messages
}

void CListCBase::OnSetFocus(CWnd* pOldWnd) 
{
	CListCtrl::OnSetFocus(pOldWnd);
	// TODO: Add your message handler code here
	m_listhelper.OnSetFocus(pOldWnd);
}

void CListCBase::OnKillFocus(CWnd* pNewWnd) 
{
	CListCtrl::OnKillFocus(pNewWnd);
	// TODO: Add your message handler code here
	m_listhelper.OnKillFocus(pNewWnd);
}

LRESULT CListCBase::OnSetImageList(WPARAM wParam, LPARAM lParam)
{
	m_listhelper.OnSetImageList(wParam,lParam);
	return (Default());
}

LRESULT CListCBase::OnSetTextColor(WPARAM wParam, LPARAM lParam)
{
	m_listhelper.OnSetTextColor(wParam,lParam);
	return (Default());
}

LRESULT CListCBase::OnSetTextBkColor(WPARAM wParam, LPARAM lParam)
{
	m_listhelper.OnSetTextBkColor(wParam,lParam);
	return (Default());
}

LRESULT CListCBase::OnSetBkColor(WPARAM wParam, LPARAM lParam)
{
	m_listhelper.OnSetBkColor(wParam,lParam);
	return (Default());
}


void CListCBase::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_listhelper.OnLButtonDblClk(nFlags,point,*get_GridDataSource()
		,get_GridDataSetter(),get_GridCellInformer())) return;
	CListCtrl::OnLButtonDblClk(nFlags, point);
}

void CListCBase::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_listhelper.OnLButtonDown(nFlags,point,*get_GridDataSource()
		,get_GridDataSetter(),get_GridCellInformer())) return;
	CListCtrl::OnLButtonDown(nFlags, point);
//	CListCtrl& lc = GetListCtrl();
//	int i;
//	TRACE("CListCBase::OnLButtonDown()\n");
//	for(i=0;i<lc.GetItemCount();i++)
//	{
//		DWORD state = lc.GetItemState(i,0xffff);
//		TRACE("%d = %08x\n",i,state);
//	}
}

void CListCBase::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(NOT_NULL(get_GridDataSetter()))
		m_listhelper.OnEndlabelEdit((LPCTSTR)TRUE,*get_GridDataSetter());
	CListCtrl::OnLButtonUp(nFlags, point);
}

void CListCBase::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_listhelper.OnKeyDown(nChar,nRepCnt,nFlags,*get_GridDataSource()
		,get_GridDataSetter(),get_GridCellInformer())) return;
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CListCBase::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	if(NOT_NULL(get_GridDataSetter()))
		m_listhelper.OnEndlabelEdit((LPCTSTR)TRUE,*get_GridDataSetter());
	return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

void CListCBase::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	LockWindowUpdate();
	m_listhelper.OnHScroll(nSBCode, nPos, pScrollBar,*get_GridDataSource(),get_GridDataSetter(),GetWindowDC(),&m_columns);
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
	UnlockWindowUpdate();
}

void CListCBase::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	LockWindowUpdate();
	// TODO: Add your message handler code here and/or call default
	m_listhelper.OnVScroll(nSBCode, nPos, pScrollBar,*get_GridDataSource(),get_GridDataSetter(),GetWindowDC(),&m_columns);
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
	//m_listhelper.PostVScroll(*get_GridDataSource());
	UnlockWindowUpdate();
}

void CListCBase::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(NOT_NULL(get_GridDataSetter()))
		m_listhelper.OnEndlabelEdit((LPCTSTR)TRUE,*get_GridDataSetter());
	CListCtrl::OnRButtonDown(nFlags, point);
}

void CListCBase::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CListCtrl::OnRButtonUp(nFlags, point);
}

BOOL CListCBase::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_listhelper.OnEraseBkgnd(pDC)) return TRUE;
	return CListCtrl::OnEraseBkgnd(pDC);
}

void CListCBase::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_listhelper.OnItemChanged(pNMHDR,*get_GridDataSource());
}

void CListCBase::OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult)
{
}

void CListCBase::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	if(NOT_NULL(get_GridDataSetter()))
		m_listhelper.OnEndlabelEdit(pDispInfo->item.pszText,*get_GridDataSetter());
	*pResult = 0;
}

void CListCBase::OnBeginTrack(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
	if(NOT_NULL(get_GridDataSetter()))
		m_listhelper.OnEndlabelEdit((LPCTSTR)TRUE,*get_GridDataSetter());
	*pResult = 0;
}

void CListCBase::OnHeaderClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
	m_listhelper.OnHeaderClicked(phdn->iItem,*get_GridDataSource()
		,get_GridDataSetter(),GetWindowDC(),&m_columns);
	*pResult = 0;
}

BOOL CListVBase::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_listhelper.PreTranslateMessage(pMsg)) return TRUE;
	return CListView::PreTranslateMessage(pMsg);
}

BOOL CListCBase::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_listhelper.PreTranslateMessage(pMsg)) return TRUE;
	return CListCtrl::PreTranslateMessage(pMsg);
}

void CListVBase::OnGetTipTextA(UINT id,NMHDR* pNMHDR,LRESULT* pResult)
{
	LPNMTTDISPINFO pNMHDDispInfo = (LPNMTTDISPINFO)pNMHDR;
	*pResult = TRUE;
    UINT nID =pNMHDR->idFrom;
	CString str;
    if (pNMHDDispInfo->uFlags & TTF_IDISHWND)
		nID = ::GetDlgCtrlID((HWND)nID);
	m_listhelper.get_Tooltip(nID,*get_GridDataSource(),str);
	m_tipstr = str;
	pNMHDDispInfo->lpszText = (LPTSTR)(LPCSTR)m_tipstr;
}

void CListVBase::OnGetTipTextW(UINT id,NMHDR* pNMHDR,LRESULT* pResult)
{
	LPNMTTDISPINFO pNMHDDispInfo = (LPNMTTDISPINFO)pNMHDR;
	*pResult = TRUE;
    UINT nID =pNMHDR->idFrom;
	CString str;
    if (pNMHDDispInfo->uFlags & TTF_IDISHWND)
		nID = ::GetDlgCtrlID((HWND)nID);
	m_listhelper.get_Tooltip(nID,*get_GridDataSource(),str);
	m_tipstr = str;
	pNMHDDispInfo->lpszText = (LPTSTR)(LPCWSTR)m_tipstr;
}

void CListCBase::OnGetTipTextA(UINT id,NMHDR* pNMHDR,LRESULT* pResult)
{
	LPNMTTDISPINFO pNMHDDispInfo = (LPNMTTDISPINFO)pNMHDR;
	*pResult = TRUE;
    UINT nID =pNMHDR->idFrom;
	CString str;
    if (pNMHDDispInfo->uFlags & TTF_IDISHWND)
		nID = ::GetDlgCtrlID((HWND)nID);
	m_listhelper.get_Tooltip(nID,*get_GridDataSource(),str);
	m_tipstr = (LPCTSTR)str;
	pNMHDDispInfo->lpszText = (LPTSTR)(LPCSTR)m_tipstr;
}

void CListCBase::OnGetTipTextW(UINT id,NMHDR* pNMHDR,LRESULT* pResult)
{
	LPNMTTDISPINFO pNMHDDispInfo = (LPNMTTDISPINFO)pNMHDR;
	*pResult = TRUE;
    UINT nID =pNMHDR->idFrom;
	CString str;
    if (pNMHDDispInfo->uFlags & TTF_IDISHWND)
		nID = ::GetDlgCtrlID((HWND)nID);
	m_listhelper.get_Tooltip(nID,*get_GridDataSource(),str);
	m_tipstr = (LPCTSTR)str;
	pNMHDDispInfo->lpszText = (LPTSTR)(LPCWSTR)m_tipstr;
}

void CListVBase::OnTrack(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
	*pResult = FALSE;
	m_listhelper.OnTrack(pNMHDR,pResult);
}

void CListCBase::OnTrack(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
	*pResult = FALSE;
	m_listhelper.OnTrack(pNMHDR,pResult);
}

CListCBaseImpl::CListCBaseImpl():m_pgriddatasource(NULL)
{
}

void CListCBaseImpl::Attach(GridDataSource* _pgriddatasource)
{
	m_pgriddatasource = _pgriddatasource;
}

GridDataSource* CListCBaseImpl::get_GridDataSource()
{
	return m_pgriddatasource;
}

void CListCBaseImpl::refresh()
{
	VERIFY_EXIT(NOT_NULL(m_pgriddatasource));
	m_listhelper.SetGridData(*m_pgriddatasource);
}

void CListCBase::refresh()
{
	VERIFY_EXIT(NOT_NULL(get_GridDataSource()));
	m_listhelper.SetGridData(*get_GridDataSource());
}