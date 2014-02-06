// listhelper.cpp: implementation of the listhelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../utils/utils.h"
#include "listhelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//struct CLVColumn: public LVCOLUMN
//{
//	void ZeroObj()
//	{
//		mask = 0;
//		fmt = 0;
//		cx = 0;
//		pszText = NULL;
//		cchTextMax = 0;
//		iSubItem = 0;
//		if(OSVersion::is().)
//#if (_WIN32_IE >= 0x0300)
//		int iImage;
//		int iOrder;
//#endif
//#if _WIN32_WINNT >= 0x0600
//		int cxMin;       // min snap point
//		int cxDefault;   // default snap point
//		int cxIdeal;     // read only. ideal may not eqaul current width if auto sized (LVS_EX_AUTOSIZECOLUMNS) to a lesser width.
//	}
//};

class AutoMemDC: public CDC
{
	CBitmap         m_bmp;	
	CBitmap*        m_poldbmp;
	CDC*            m_pDC;
	CRect           m_rect;
	bool            m_bMemDC;
	CFont*          m_poldfont;
public:
	AutoMemDC(CDC* _pDC,CRect& _rc);
	~AutoMemDC();
	//operator CDC&() {return *this;};
	AutoMemDC* operator->() {return this;};
	operator AutoMemDC*() {return this;}
	operator AutoMemDC&() {return *this;}
};

AutoMemDC::AutoMemDC(CDC* _pDC,CRect& _rc)
:m_pDC(NULL),m_rect(0,0,0,0),m_bMemDC(false),m_poldbmp(NULL),m_poldfont(NULL)
{
	ASSERT(NULL!=_pDC);
	if(NULL==_pDC) return;
	m_pDC = _pDC;
	m_bMemDC = !_pDC->IsPrinting();
	m_rect = _rc;
	if(m_bMemDC)
	{
		CreateCompatibleDC(m_pDC);
		_pDC->LPtoDP(&m_rect);
		m_bmp.CreateCompatibleBitmap(m_pDC,m_rect.Width(),m_rect.Height());
		m_poldbmp = SelectObject(&m_bmp);
		SetMapMode(m_pDC->GetMapMode());
		m_pDC->DPtoLP(&m_rect);
		SetWindowOrg(m_rect.left,m_rect.top);
		BitBlt(m_rect.left,m_rect.top,m_rect.Width(),m_rect.Height(),m_pDC,m_rect.left,m_rect.top,SRCCOPY);
		m_poldfont = SelectObject(m_pDC->GetCurrentFont());
	}
	else
	{
		m_bPrinting = _pDC->m_bPrinting;
		m_hDC = _pDC->m_hDC;
		m_hAttribDC = _pDC->m_hAttribDC;
	}
}

AutoMemDC::~AutoMemDC()
{
	if(m_bMemDC)
	{
		m_pDC->BitBlt(m_rect.left,m_rect.top,m_rect.Width(),m_rect.Height()
			,this,m_rect.left,m_rect.top,SRCCOPY);
		SelectObject(m_poldbmp);
		SelectObject(m_poldfont);
	}
	else
	{
		m_hDC = m_hAttribDC = NULL;
	}
}

BOOL HasFocus(CWnd* wnd)
{
	if(IS_NULL(wnd) || IS_NULL((HWND)*wnd)) return false;
	CWnd* pfocuswnd=wnd->GetFocus();
	return pfocuswnd->GetSafeHwnd()==wnd->GetSafeHwnd()
		|| ::IsChild(wnd->GetSafeHwnd(),pfocuswnd->GetSafeHwnd());
}

// This is the "magic" ROP code used to generate the embossed look for
// a disabled button. It's listed in Appendix F of the Win32 Programmer's
// Reference as PSDPxax (!) which is a cryptic reverse-polish notation for
//
// ((Destination XOR Pattern) AND Source) XOR Pattern
//
// which I leave to you to figure out. In the case where I apply it,
// Source is a monochrome bitmap which I want to draw in such a way that
// the black pixels get transformed to the brush color and the white pixels
// draw transparently--i.e. leave the Destination alone.
//
// black ==> Pattern
// white ==> Destintation (ie, transparent)
//
// 0xb8074a is the ROP code that does this. For more info, see Charles
// Petzold, _Programming Windows_, 2nd Edition, p 622-624.
//
const DWORD		MAGICROP		= 0xb8074a;

//////////////////
// Draw an image with the embossed (disabled) look.
//
//		dc			device context to draw in
//		il			image list containing image
//		i			index of image to draw
//		p			point in dc to draw image at
//    bColor	do color embossing. Default is B/W.
//

void DrawEmbossed(CDC& dc, CImageList& il, int i,
						 CPoint p, BOOL bColor)
{
	IMAGEINFO info;
	VERIFY(il.GetImageInfo(0, &info));
	CRect rc = info.rcImage;
	int cx = rc.Width();
	int cy = rc.Height();

	// create memory dc
	CDC memdc;
	memdc.CreateCompatibleDC(&dc);

	// create mono or color bitmap
	CBitmap bm;
	if (bColor)
		bm.CreateCompatibleBitmap(&dc, cx, cy);
	else
		bm.CreateBitmap(cx, cy, 1, 1, NULL);

	// draw image into memory DC--fill BG white first
	CBitmap* pOldBitmap = memdc.SelectObject(&bm);
	memdc.PatBlt(0, 0, cx, cy, WHITENESS);
	il.Draw(&memdc, i, CPoint(0,0), ILD_TRANSPARENT);

	// This seems to be required. Why, I don't know. ???
	COLORREF colorOldBG = dc.SetBkColor(RGB(255,255,255));

	// Draw using hilite offset by (1,1), then shadow
	CBrush brShadow(GetSysColor(COLOR_3DSHADOW));
	CBrush brHilite(GetSysColor(COLOR_3DHIGHLIGHT));
	CBrush* pOldBrush = dc.SelectObject(&brHilite);
	dc.BitBlt(p.x+1, p.y+1, cx, cy, &memdc, 0, 0, MAGICROP);
	dc.SelectObject(&brShadow);
	dc.BitBlt(p.x, p.y, cx, cy, &memdc, 0, 0, MAGICROP);
	dc.SelectObject(pOldBrush);
	dc.SetBkColor(colorOldBG);				 // restore
	memdc.SelectObject(pOldBitmap);		 // ...
}

struct CScrollInfo:public SCROLLINFO
{
	CScrollInfo() {ZeroObj(*this);cbSize = sizeof(SCROLLINFO);}
	CScrollInfo(UINT _fMask) {ZeroObj(*this);cbSize = sizeof(SCROLLINFO);fMask = _fMask;}
};

struct Counter
{
	int val;
	Counter(int _init):val(_init){}
	int operator()()
	{
		return val++;
	}
};

//using namespace Utils;
using namespace std;
#pragma warning(disable:4800)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define NOTSELECTED              (-1)
// offsets for first and other columns
#define OFFSET_FIRST    (2)
#define OFFSET_OTHER    (6)

void traceListStates(CListCtrl& lc)
{
	TRACE("----traceListStates()-------");
	int i;
	for(i=0;i<lc.GetItemCount();i++)
	{
		DWORD st = lc.GetItemState(i,0xffff);
		TRACE("i=%d  state=%08x\n",i,st);
	}
}


CListHelper::ColorSheme::ColorSheme(CListHelper& _listhelper,CDC* pDC,GridDataSource& _data
									,int _row,int _col,BOOL _bFocus,BOOL _bSelect,BOOL _bHilited
									,ListDrawModeEn _drawmode)
:m_listhelper(_listhelper),m_data(_data),m_pbrush(NULL),m_pDC(pDC) //m_ppen(NULL)
{
	colors[C_NotSelected][C_Background] = 
		EQL(_drawmode,LDM_STANDARD)?::GetSysColor(COLOR_WINDOW)
		:(EQL(_drawmode,LDM_DELPHIGRIG)?::GetSysColor(COLOR_BTNFACE)
			: ::GetSysColor(COLOR_WINDOW));
	colors[C_NotSelected][C_Foreground] = 
		EQL(_drawmode,LDM_STANDARD)?::GetSysColor(COLOR_WINDOWTEXT)
		:(EQL(_drawmode,LDM_DELPHIGRIG)?::GetSysColor(COLOR_BTNTEXT)
			: ::GetSysColor(COLOR_WINDOWTEXT));
		//::GetSysColor(COLOR_WINDOWTEXT);
		//COLOR_BTNTEXT
	colors[C_Selected][C_Background] = ::GetSysColor(COLOR_HIGHLIGHT);
//		EQL(_drawmode,LDM_STANDARD)?::GetSysColor(COLOR_HIGHLIGHT);
//		:(EQL(_drawmode,LDM_DELPHIGRIG)?::GetSysColor(COLOR_BTNFACE)
//			: ::GetSysColor(COLOR_HIGHLIGHT));
		//::GetSysColor(COLOR_HIGHLIGHT);
	colors[C_Selected][C_Foreground] = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
//		EQL(_drawmode,LDM_STANDARD)?::GetSysColor(COLOR_HIGHLIGHTTEXT);
//		:(EQL(_drawmode,LDM_DELPHIGRIG)?::GetSysColor(COLOR_BTNTEXT)
//			: ::GetSysColor(COLOR_HIGHLIGHTTEXT));
		//::GetSysColor(COLOR_HIGHLIGHTTEXT);
	colors[C_DropHilited][C_Background] = ::GetSysColor(COLOR_HIGHLIGHT);
	colors[C_DropHilited][C_Foreground] = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_data.GetColors(_row,_col,colors);
	if(_bHilited) m_icolors = C_DropHilited;
	else if(_bFocus && m_listhelper.m_bShowFocusAsSelection)
	{
		if(m_listhelper.m_bCellFocused)m_icolors=(m_listhelper.m_nFocusedColumn==_col)?C_Selected:C_NotSelected;
		else m_icolors = C_Selected;
	}
	else if(_bSelect && !m_listhelper.m_bShowFocusAsSelection) 
		m_icolors = C_Selected;
	else m_icolors = C_NotSelected;
//	TRACE("_bFocus=%s, _bDpopHilited=%s, _bSelected=%s, "
//		"bShowFocusAsSelect=%s, bCellFocused=%s nFocusedColumn=%d"
//		" icolor=%s\n"
//		,_bFocus?"true":"false",_bHilited?"true":"false"
//		,_bSelect?"true":"false",m_listhelper.m_bShowFocusAsSelection?"true":"false"
//		,m_listhelper.m_bCellFocused?"true":"false",m_listhelper.m_nFocusedColumn
//		,m_icolors==C_NotSelected?"Not selected":m_icolors==C_Selected?"Selected"
//		:m_icolors==C_DropHilited?"Hilited":"------");
	//m_pen.CreatePen(PS_SOLID,0,colors[icolor][C_Foreground]);
	m_brush.CreateSolidBrush(colors[m_icolors][C_Background]);
	//m_ppen = new Utils::AutoGdiObject<CPen>(pDC,m_pen);
	m_pbrush = new AutoGdiObject<CBrush>(pDC,m_brush);
	m_oTxtClr = m_pDC->SetTextColor(colors[m_icolors][C_Foreground]);
	m_oBkgClr = m_pDC->SetBkColor(colors[m_icolors][C_Background]);
}

CListHelper::ColorSheme::~ColorSheme()
{
	//if(NOT_NULL(m_ppen)) delete m_ppen;
	if(NOT_NULL(m_pbrush)) 
	{
		delete m_pbrush;
		m_pbrush = NULL;
	}
	m_pDC->SetTextColor(m_oTxtClr);
	m_pDC->SetBkColor(m_oBkgClr);
}

void CListHelper::set_DrawMode(ListDrawModeEn  _drawmode)
{
	m_drawmode = _drawmode;
	if(NOT_NULL(&GetListCtrl())
		&& ::IsWindow(GetListCtrl().GetSafeHwnd())
		&& ::IsWindowVisible(GetListCtrl().GetSafeHwnd())
		) RedrawList();
	if(EQL(m_drawmode,LDM_DELPHIGRIG))
	{
		//if(IS_NULL((HGDIOBJ)m_hlinepen))
		//	m_hlinepen.CreatePen(PS_DASH,0,::GetSysColor(COLOR_3DSHADOW));
		if(IS_NULL((HGDIOBJ)m_btnshadowpen))
			m_btnshadowpen.CreatePen(PS_SOLID,0,::GetSysColor(COLOR_3DSHADOW));
		if(IS_NULL((HGDIOBJ)m_btnhighlightpen))
			m_btnhighlightpen.CreatePen(PS_SOLID,0,::GetSysColor(COLOR_BTNHIGHLIGHT));
	}
}

void CListHelper::DrawItem(LPDRAWITEMSTRUCT _lpDrawItemStruct,GridDataSource& _data)
{
	if(IS_NULL(This()) || IS_NULL(&GetListCtrl())
		|| !IsWindow(*This()) || !IsWindow(GetListCtrl())
		) return;
	CListCtrl& lc = GetListCtrl();
	CDC* drawDC = CDC::FromHandle(_lpDrawItemStruct->hDC);
	//CPen* poldpen;
	CRect rcItem(_lpDrawItemStruct->rcItem);
	rcItem.InflateRect(0,0,1,1);
	AutoMemDC pDC(drawDC,rcItem);
	//CDC* pDC = drawDC;
	UINT uiFlags = ILD_TRANSPARENT;
	if(IS_NULL((HGDIOBJ)m_hlinepen))
		m_hlinepen.CreatePen(PS_SOLID,0,::GetSysColor(COLOR_3DSHADOW));
	CImageList* pImageList;
	int nListItem = _lpDrawItemStruct->itemID;
	BOOL bFocus = HasFocus(&lc);
	//COLORREF oldBkclr,oldTextclr;
	if(lc.GetStyle()&WS_DISABLED) return;
	bool bOwnerData = lc.GetStyle()&LVS_OWNERDATA;
	int rowcnt = get_Rows(_data);
	int nItem = get_SourceRow(nListItem);
	bool baddnewline = m_bAddNewLine && (nItem==rowcnt-1);
	//get state
	//LV_ITEM lvi;
	//ZeroMemory(&lvi,sizeof(lvi));
	//lvi.mask = LVIF_STATE;
	//lvi.stateMask = 0xffff;
	UINT fState;
	//lc.GetItem(&lvi);
	//fState = lvi.state;

	#define CHECKSTATE(srcstate,resstate)  (NEQL(0,_lpDrawItemStruct->itemState&(srcstate))?(resstate):0)

	fState = CHECKSTATE(ODS_FOCUS,LVIS_FOCUSED)
		|CHECKSTATE(ODS_SELECTED,LVIS_SELECTED);
		//|CHECKSTATE(ODS_HOTLIGHT,LVIS_DROPHILITED);
//	TRACE("List = %d Item = %d state = %08x\n",nListItem,nItem,fState);
//	TRACE("redraw = %d \t(%d) \t%08x\n",nListItem,nItem,fState);

	if(m_bSaveGridState)
	{
		Utils::ItemID* _id;
		_data.GetRowID(nItem,_id);
		if(NOT_NULL(_id))
		{
			Utils::IDWithData<RowState>* prowstate = m_idhelper.get(_id);
			if(NOT_NULL(prowstate))
				fState = fState&LVIS_FOCUSED | (prowstate->data.state&~LVIS_FOCUSED);
		}
	}
	bool bTopItem = lc.GetTopIndex()==nListItem;
	int nItems = lc.GetTopIndex() + lc.GetCountPerPage();
	nItems = min(nItems,lc.GetItemCount()-1);
	bool bBottomItem = (nListItem==nItems && !m_bAddNewLine)
		|| (m_bAddNewLine && nListItem==nItems-1);

	BOOL bSelected = NEQL(0,fState&LVIS_SELECTED);
	BOOL bDropHilited = NEQL(0,fState&LVIS_DROPHILITED);
	BOOL bFocused = NEQL(0,fState&LVIS_FOCUSED);
	
	CRect rcAllLabels;
	lc.GetItemRect(nListItem,&rcAllLabels,LVIR_BOUNDS);
	CRect rcLabel,rcFocus;
	lc.GetItemRect(nListItem,&rcLabel,LVIR_LABEL);

	//if(bFocused && !m_bShowFocusAsSelection) rcFocus = rcAllLabels;
	pDC->SetBkMode(TRANSPARENT);
	int nColumn;
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT|LVCF_WIDTH|LVCF_SUBITEM;

	rcAllLabels.InflateRect(0,0,1,1);
	pDC->FillRect(rcAllLabels,&CBrush(
		::GetSysColor(
			EQL(LDM_STANDARD,m_drawmode)?COLOR_WINDOW
			:EQL(LDM_DELPHIGRIG,m_drawmode)?COLOR_BTNFACE:COLOR_WINDOW
			)
		));

	rcAllLabels.DeflateRect(0,0,1,1);
	bool bdisabled = lc.GetStyle()&WS_DISABLED;
	if(bdisabled) return;

	int nStateImg;
	if(bOwnerData) _data.GetImage(nItem,nStateImg);
	else nStateImg = ((fState&LVIS_STATEIMAGEMASK)>>12)-1;
	if(nStateImg>=0)
	{
		pImageList = lc.GetImageList(LVSIL_STATE);
		if(pImageList)
		{
			IMAGEINFO ii;
			pImageList->GetImageInfo(0,&ii);
			int ih = CRect(ii.rcImage).Height();
			CPoint pt = CPoint(rcAllLabels.left,ih>rcAllLabels.Height()?rcAllLabels.top
				:(rcAllLabels.Height()-ih)/2+rcAllLabels.top);
			pImageList->Draw(pDC,nStateImg,pt,ILD_TRANSPARENT);
			//rcItem.left += CRect(ii.rcImage).Width();
		}else nStateImg=-1;
	}

	//rcAllLabels.left = rcLabel.left;
	lc.GetItemRect(nListItem, rcItem, LVIR_LABEL);
	if(!m_bCellFocused && (!m_bShowFocusAsSelection
		|| EQL(m_drawmode,LDM_DELPHIGRIG))) 
	{
		rcFocus = rcAllLabels;
		rcFocus.left = rcItem.left;
	}
	//rcItem.left += OFFSET_FIRST;
	int offsCol0 = rcItem.left-rcAllLabels.left;
	//rcItem.right -= OFFSET_FIRST;
	AutoGdiObject<CPen> __gdi0(pDC,m_gridpen);
	rcItem.right = rcItem.left;
	// here i should set rcItem
	nColumn = 0;
	bool bStateImages = NOT_NULL(lc.GetImageList(LVSIL_STATE));
	if(bStateImages)
	{
		lc.GetColumn(nColumn++,&lvc);
		rcItem = rcAllLabels;
		rcItem.right = rcItem.left;
		rcItem.left = (rcItem.right+=lvc.cx);
	}
	CString text;
	if(baddnewline)
	{
		int nItemColumn = get_SourceColumn(0);
		ColorSheme __cs(*this,pDC,_data,nItem,nItemColumn,bFocused,bSelected,bDropHilited,m_drawmode);
		text = _T("...");
		rcAllLabels.InflateRect(0,0,1,1);
		pDC->FillRect(&rcAllLabels,&CBrush(
			::GetSysColor(
				EQL(LDM_STANDARD,m_drawmode)?COLOR_WINDOW
				:EQL(LDM_DELPHIGRIG,m_drawmode)?COLOR_BTNFACE:COLOR_WINDOW
				)
			));

		rcAllLabels.DeflateRect(0,0,1,1);
		rcItem.left = rcItem.right; 
		rcItem.right += lc.GetColumnWidth(nColumn)-(EQL(0,nColumn)?offsCol0:0);
		pDC->FillRect(&rcLabel,pDC->GetCurrentBrush());
		pDC->DrawText(text,rcItem,DT_LEFT|DT_SINGLELINE
				|DT_NOPREFIX|DT_NOCLIP|DT_VCENTER
				|DT_END_ELLIPSIS|DT_WORD_ELLIPSIS);
		if(0!=nListItem)
		{
			//poldpen = (EQL(m_drawmode,LDM_DELPHIGRIG))?pDC->SelectObject(&m_hlinepen):NULL;
			if(EQL(m_drawmode,LDM_STANDARD))
			{
				AutoGdiObject<CPen> __ap(pDC,&m_hlinepen);
				pDC->MoveTo(rcItem.left,rcLabel.top);
				pDC->LineTo(rcAllLabels.right,rcLabel.top);
			}
			else if(EQL(m_drawmode,LDM_DELPHIGRIG))
			{
				int x;
				COLORREF clr = ::GetSysColor(COLOR_3DSHADOW);
				for(x=rcItem.left;x<rcAllLabels.right;x+=2) pDC->SetPixelV(x,rcLabel.top,clr);
			}
			//if(NOT_NULL(poldpen))pDC->SelectObject(poldpen);
		}
		if(bFocused)
		if(!m_bShowFocusAsSelection)
		{
			if(EQL(m_drawmode,LDM_DELPHIGRIG))
				pDC->Draw3dRect(&rcItem
					,::GetSysColor(COLOR_3DDKSHADOW)
					,::GetSysColor(COLOR_BTNHIGHLIGHT)
					);
			else if(EQL(m_drawmode,LDM_STANDARD))
				pDC->DrawFocusRect(rcItem); 
			//rcItem.DeflateRect(1,1,1,1);
			//pDC.DrawFocusRect(rcItem);
		}
		else if(EQL(m_drawmode,LDM_DELPHIGRIG))
		{
			pDC->Draw3dRect(&rcItem
				,::GetSysColor(COLOR_3DDKSHADOW)
				,::GetSysColor(COLOR_BTNHIGHLIGHT)
				);
		}
		else if(EQL(m_drawmode,LDM_STANDARD))
			pDC->DrawFocusRect(rcItem); 
		return;
	}
	//poldpen = (EQL(m_drawmode,LDM_DELPHIGRIG))?pDC->SelectObject(&m_hlinepen):NULL;
	if(EQL(m_drawmode,LDM_STANDARD))
	{
		AutoGdiObject<CPen> __ap(pDC,&m_hlinepen);
		pDC->MoveTo(rcItem.left,rcAllLabels.top);
		pDC->LineTo(rcAllLabels.right,rcAllLabels.top);
		if(bBottomItem)
		{
			pDC->MoveTo(rcItem.left,rcAllLabels.bottom);
			pDC->LineTo(rcAllLabels.right,rcAllLabels.bottom);
		}
	}
	else if(EQL(m_drawmode,LDM_DELPHIGRIG))
	{
		int x;
		COLORREF clr = ::GetSysColor(COLOR_3DSHADOW);
		for(x=rcItem.left;x<rcAllLabels.right;x+=2) pDC->SetPixelV(x,rcLabel.top,clr);
		for(x=rcItem.left;x<rcAllLabels.right;x+=2) pDC->SetPixelV(x,rcLabel.bottom,clr);
	}
	//if(NOT_NULL(poldpen))pDC->SelectObject(poldpen);
	if(EQL(m_drawmode,LDM_STANDARD))
	{
		AutoGdiObject<CPen> __ap(pDC,&m_hlinepen);
		pDC->MoveTo(rcItem.left,rcItem.top);
		pDC->LineTo(rcItem.left,rcItem.bottom);
	}
	else if(EQL(m_drawmode,LDM_DELPHIGRIG))
	{
		CPen* poldpen = pDC->SelectObject(&m_btnshadowpen);
		pDC->MoveTo(rcItem.left-1,rcItem.top);
		pDC->LineTo(rcItem.left-1,rcItem.bottom);
		poldpen = pDC->SelectObject(&m_btnhighlightpen);
		pDC->MoveTo(rcItem.left,rcItem.top);
		pDC->LineTo(rcItem.left,rcItem.bottom);
		pDC->SelectObject(poldpen);
	}

	pImageList = lc.GetImageList(LVSIL_NORMAL);
	IMAGEINFO ii;
	if(NOT_NULL(pImageList)) pImageList->GetImageInfo(0,&ii);

	DrawModeEn drawmode=TDM_NORMAL;
	if(!_data.GetDrawMode(nItem,drawmode)) drawmode=TDM_NORMAL;
	int coli;
	bool blastjoin = false;
	for(coli=0;lc.GetColumn(nColumn,&lvc);nColumn++,coli++)
	{
		int nItemColumn = get_SourceColumn(coli); //todo: add support for draging of headers and support for the 
		ColorSheme __cs(*this,pDC,_data,nItem,nItemColumn,bFocused,bSelected,bDropHilited,m_drawmode);
		ColumnInfoTag cit;
		_data.GetColumnInfo(nItem,nItemColumn,cit);
		CRect rcItem1 = rcItem;
		if(!cit.m_bjoin && !blastjoin) rcItem.left = rcItem.right;
		rcItem.right += lvc.cx-((nColumn==0)?offsCol0:0);
		if(m_bCellFocused && m_nFocusedColumn==nItemColumn && 
			(!m_bShowFocusAsSelection || EQL(m_drawmode,LDM_DELPHIGRIG)))
		{
			rcItem1.left = rcItem1.right;
			rcItem1.right += lvc.cx-((nColumn==0)?offsCol0:0);
			rcFocus = rcItem1;
		}
		blastjoin = cit.m_bjoin;
		if(cit.m_bjoin) continue;
		UINT nJustify = DT_LEFT;
		switch(lvc.fmt&LVCFMT_JUSTIFYMASK)
		{
		case LVCFMT_RIGHT: nJustify = DT_RIGHT; break;
		case LVCFMT_CENTER: nJustify = DT_CENTER; break;
		case LVCFMT_LEFT: nJustify = DT_LEFT; break;
		default: break;
		}
		COLORREF clrImage;
		clrImage = __cs[C_Background];
		if(NEQL(0,fState&LVIS_CUT) || NEQL(__cs.get_Colors(),C_NotSelected)) uiFlags |= ILD_BLEND50;
		else uiFlags &= ~(ILD_BLEND50);

		rcItem.DeflateRect(1,1,0,0);
		pDC->FillRect(&rcItem,pDC->GetCurrentBrush());
		rcItem.InflateRect(1,1,0,0);

		if(EQL(ColumnInfoTag::CT_TEXT,cit.m_type))
		{
			if(bOwnerData) _data.GetString(nItem,nItemColumn,text);
			else text = lc.GetItemText(nListItem,coli);

			CRect rcText = rcItem;
			rcText.right-=2;
			rcText.left++;
			if(EQL(drawmode,TDM_NORMAL))
			{
				pDC->DrawText(text,rcText
					,nJustify|DT_SINGLELINE|DT_NOPREFIX|DT_NOCLIP|DT_VCENTER
					|DT_END_ELLIPSIS|DT_WORD_ELLIPSIS);
			}
			else if(EQL(drawmode,TDM_DISABLED))
			{
				rcText.bottom--;
				rcText.right--;
				rcText.OffsetRect(1,1);
				pDC->SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
				pDC->DrawText(text,rcText,nJustify|DT_SINGLELINE|DT_NOPREFIX|DT_NOCLIP|DT_VCENTER
					|DT_END_ELLIPSIS|DT_WORD_ELLIPSIS);
				rcText.OffsetRect(-1,-1);
				pDC->SetTextColor(::GetSysColor(COLOR_GRAYTEXT));
				pDC->DrawText(text,rcText,nJustify|DT_SINGLELINE|DT_NOPREFIX|DT_NOCLIP|DT_VCENTER
					|DT_END_ELLIPSIS|DT_WORD_ELLIPSIS);
			}
		}
		else if(EQL(ColumnInfoTag::CT_IMAGE,cit.m_type) && NOT_NULL(pImageList))
		{
			int ih,iw;
			ih = CRect(ii.rcImage).Height();
			iw = CRect(ii.rcImage).Width();

			int nImage;
			_data.GetImage(nItem,nItemColumn,nImage);
			CPoint pt;
			switch(nJustify)
			{
			case DT_LEFT:
				pt.x = rcItem.left;
				break;
			case DT_RIGHT:
				pt.x = rcItem.right-iw;
				break;
			case DT_CENTER:
			default:
				pt.x = (iw<rcItem.Width())?rcItem.left+(rcItem.Width()-iw)/2 :rcItem.left;
				break;
			}
			pt.y = (ih<rcItem.Height())?rcItem.top+(rcItem.Height()-ih)/2 : rcItem.top;
			if(EQL(drawmode,TDM_NORMAL))
				ImageList_DrawEx(pImageList->m_hImageList, nImage,pDC->m_hDC,pt.x,pt.y, iw, ih
							,__cs[C_Background],__cs[C_Foreground],uiFlags);
			else if(EQL(drawmode,TDM_DISABLED))
				DrawEmbossed(pDC,*pImageList,nImage,pt,TRUE);
		}
		//if(coli!=0 || (coli==0 && nStateImg<0))
		//{
		if(EQL(m_drawmode,LDM_STANDARD))
		{
			AutoGdiObject<CPen> __ap(pDC,&m_hlinepen);
			pDC->MoveTo(rcItem.right,rcItem.top);
			pDC->LineTo(rcItem.right,rcItem.bottom);
		}
		else if(EQL(m_drawmode,LDM_DELPHIGRIG))
		{
			CPen* poldpen = pDC->SelectObject(&m_btnshadowpen);
			pDC->MoveTo(rcItem.right-1,rcItem.top);
			pDC->LineTo(rcItem.right-1,rcItem.bottom);
			poldpen = pDC->SelectObject(&m_btnhighlightpen);
			pDC->MoveTo(rcItem.right,rcItem.top);
			pDC->LineTo(rcItem.right,rcItem.bottom);
			pDC->SelectObject(poldpen);
		}
	}
	if(bFocused)
	if(!m_bShowFocusAsSelection)
	{
		if(EQL(m_drawmode,LDM_DELPHIGRIG))
			pDC->Draw3dRect(&rcFocus
				,::GetSysColor(COLOR_3DDKSHADOW)
				,::GetSysColor(COLOR_BTNHIGHLIGHT));
		else if(EQL(m_drawmode,LDM_STANDARD))
			pDC->DrawFocusRect(rcFocus); 
		//rcFocus.DeflateRect(1,1,1,1);
		//pDC.DrawFocusRect(rcFocus);
	}
	else if(EQL(m_drawmode,LDM_DELPHIGRIG))
	{
		pDC->Draw3dRect(&rcFocus
			,::GetSysColor(COLOR_3DDKSHADOW)
			,::GetSysColor(COLOR_BTNHIGHLIGHT)
			);
	}
	else if(EQL(m_drawmode,LDM_STANDARD))
		pDC->DrawFocusRect(rcFocus); 
}

void CListHelper::redraw_EditRow()
{
	CListCtrl& lc = GetListCtrl();
	lc.RedrawItems(m_edrow,m_edrow);
}

void CListHelper::BeginLabelEdit(int _row,int _col,GridDataSource& _data,GridCellInformer* _pgci)
{
	CListCtrl& lc = GetListCtrl();
	CEdit* pedit = m_editctrl.GetWnd(&lc,ID_HELPER_EDITCTRL);
	pedit->ModifyStyle(ES_MULTILINE|ES_WANTRETURN|ES_AUTOVSCROLL,ES_AUTOHSCROLL);
	int irow,icol;
	irow = get_SourceRow(_row);
	icol = get_SourceColumn(_col);
	if(_pgci)
	{
		GridCellInfoTag gcit;
		_pgci->GetCellInfo(irow,icol,gcit);
		if(NOT_NULL(gcit.m_outereditor))
		{
			if(m_pitemselect) 
				m_pitemselect->edit_ListItem(irow,icol,true);
			if(gcit.m_bredirectalledit)
			{
				m_edrow = _row;
				m_edcol = _col;
				TRACE("m_edrow = %d m_edcol = %d\n",m_edrow,m_edcol);
				gcit.m_outereditor->SetFocus();
				return;
			}
		}
		pedit->ModifyStyle(gcit.m_bmultiline?ES_AUTOHSCROLL:0,gcit.m_bmultiline?ES_MULTILINE|ES_WANTRETURN|ES_AUTOVSCROLL:0);
		m_editctrl.set_OuterEdit(gcit.m_outereditor);
	}
	BeginLabelProcess(_row,_col,pedit,_data);
}

void CListHelper::BeginLabelProcess(int _row, int _col, CWnd *pwnd,GridDataSource& _data)
{
	//EndLabelEdit(NULL,pwnd,_data);
	CListCtrl& lc = GetListCtrl();
	int irow,icol;
	irow = get_SourceRow(_row);
	icol = get_SourceColumn(_col);
	if(m_pitemselect) m_pitemselect->focused_ListItem(irow,icol);
	
	bool bMultiSelect = EQL(0,lc.GetStyle()&LVS_SINGLESEL);
	if(!m_bShowFocusAsSelection || !bMultiSelect)
	{
		DWORD state = lc.GetItemState(_row,LVIS_FOCUSED|LVIS_SELECTED);
		state &= ~(m_bShowFocusAsSelection?LVIS_SELECTED:LVIS_FOCUSED);
		lc.SetItemState(_row,state,LVIS_FOCUSED|LVIS_SELECTED);
	}

	if(_row!=HEADER_ROW && !EnsureVisible(_row,FALSE)) return; 
	if(m_bWillScroll)
	{
		//m_postscrollmsg.m_type = PostScrollMessage::T_BEGINEDIT;
		//m_postscrollmsg.set_Pos(_row,_col);
		return;
	}
	CHeaderCtrl *pHeader = lc.GetHeaderCtrl(); //(CHeaderCtrl*)GetDlgItem(0);
	bool bStateImages = NOT_NULL(lc.GetImageList(LVSIL_STATE));
	int colscnt = pHeader->GetItemCount();
	if(bStateImages) colscnt--;
	if(_col>colscnt || lc.GetColumnWidth(_col+(bStateImages?1:0))<5) return;
	if(m_bCellFocused) m_nFocusedColumn = _col;

	int i,offset;
	offset = 0;
	int coli = _col;
	if(bStateImages) coli++;
	for(i=0;i<coli;i++)
		offset+=lc.GetColumnWidth(i);
	CRect rc,rcclient,rc1;
	lc.GetClientRect(&rcclient);

	int nStateIconcx = 0;
	int scrollx = lc.GetScrollPos(SB_HORZ);
	if(_row!=HEADER_ROW)
	{
		//lc.SetItemState(_row,0,0xff);
		lc.GetItemRect(_row,&rc1,LVIR_BOUNDS);
		lc.GetItemRect(_row,&rc,LVIR_LABEL);
		nStateIconcx = rc.left-rc1.left;
		//if(_col) offset -= nStateIconcx;
		//rc.right -= nStateIconcx;
		//rc.left += offset;
		rc.left = offset;
		rc.right = rc.left + lc.GetColumnWidth(_col+(bStateImages?1:0));
	}
	else // _row==HEADER_ROW
	{
		pHeader->GetItemRect(_col+(bStateImages?1:0),rc);
	}
	CScrollInfo si;
	if(rc.left<scrollx || rc.left>=scrollx+rcclient.right)
	{
		CSize sz;
		sz.cy = 0;
		sz.cx = rc.left;
		lc.GetScrollInfo(SB_HORZ,&si,SIF_ALL);
		sz.cx-=si.nPos;
		if(sz.cx>si.nMax/*-si.nPage*/) 
			sz.cx = si.nMax/*-si.nPage*/;
		si.fMask = SIF_POS;
		lc.Scroll(sz);
		scrollx = lc.GetScrollPos(SB_HORZ);
	}
	if(rc.right>scrollx+rcclient.right)
	{
		CSize sz;
		sz.cy = 0;
		int width = max(rc.Width(),rcclient.Width());
		sz.cx = rc.right - width;
		lc.GetScrollInfo(SB_HORZ,&si,SIF_ALL);
		sz.cx -= si.nPos;
		si.fMask = SIF_POS;
		lc.Scroll(sz);
		scrollx = lc.GetScrollPos(SB_HORZ);
	}
		//rc.left = rc1.left;
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	lc.GetColumn(_col+(bStateImages?1:0),&lvcol);
	rc.right = rc.left + lc.GetColumnWidth(_col+(bStateImages?1:0));
	if(!_col) rc.right -= nStateIconcx;

	rc.left -= scrollx;
	rc.right -= scrollx;
	if(rc.right>rcclient.right) rc.right=rcclient.right;
	pwnd->SetWindowPos(&CWnd::wndTop,
		rc.left,rc.top,rc.Width(),rc.Height(),
		0);

	CFont* pfnt = lc.GetFont();
	pwnd->SetFont(pfnt);
	CString txt;
	if(!m_bAddNewLine || _row!=get_Rows(_data))
	{
		if(lc.GetStyle()&LVS_OWNERDATA)
		{
			int irow = get_SourceRow(_row);
			int icol = get_SourceColumn(_col);
			_data.GetString(irow,icol,txt);
		}
		else 
			txt = lc.GetItemText(_row,_col);
	}
	txt.TrimLeft();
	txt.TrimRight();
	pwnd->SetWindowText(txt);
	m_edrow = _row;
	m_edcol = _col;
	pwnd->ShowWindow(SW_SHOW);
	pwnd->SetFocus();
}

void CListHelper::EndLabelEdit(LPCTSTR sztxt,CWnd *pwnd,GridDataSetter* _psetter)
{
	CListCtrl& lc = GetListCtrl();
	if(!pwnd->IsWindowVisible()) 
	{
		//m_edrow = SPACE_ROW;
		return;
	}
	if(pwnd->ShowWindow(SW_HIDE)!=SW_HIDE && sztxt!=NULL && NOT_NULL(_psetter))
	{
		CString txt;
		pwnd->GetWindowText(txt);
		CString errmess;
		int irow,icol;
		irow = get_SourceRow(m_edrow);
		icol = get_SourceColumn(m_edcol);
		if(!_psetter->Validate(irow,icol,txt,errmess))
		{
			AfxMessageBox(errmess,MB_OK|MB_ICONEXCLAMATION);
			pwnd->ShowWindow(SW_SHOW);
			pwnd->SetFocus();
			return;
		}
		if(m_pitemselect) 
			m_pitemselect->edit_ListItem(irow,icol,false);
		if(!(GetListCtrl().GetStyle()&LVS_OWNERDATA))
			lc.SetItemText(m_edrow,m_edcol,txt);
		_psetter->SetString(irow,icol,txt);
		if(m_edrow==HEADER_ROW && lc.GetHeaderCtrl()) SetColumnText(m_edcol,txt);
		RedrawList();
	}
	pwnd->SetWindowText(_T(""));
	lc.SetFocus();
	if(m_edrow!=HEADER_ROW)
	{
		set_Focused(m_edrow,m_edcol);
		lc.RedrawItems(m_edrow,m_edrow);
	}
	m_edrow = SPACE_ROW;
}

void CListHelper::SetColumnText(int _col,const CString& _txt)
{
	CListCtrl& lc = GetListCtrl();
	LVCOLUMN lvc;
	ZeroMemory(&lvc,sizeof(lvc));
	lvc.mask = LVCF_TEXT;
	lvc.pszText = (LPTSTR)(LPCTSTR)_txt;
	bool bStateImages = NOT_NULL(lc.GetImageList(LVSIL_STATE));
	lc.SetColumn(_col+(bStateImages?1:0),&lvc);
}
/////////////////////////////////////////////////////////////////////////////
// CEditCont

IMPLEMENT_DYNCREATE(CEditCont,CEdit)

BEGIN_MESSAGE_MAP(CEditCont,CEdit)
	//{{AFX_MSG_MAP(CEditCont)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	ON_CONTROL_REFLECT(EN_CHANGE,OnTextChanged)
END_MESSAGE_MAP()



CEdit* CEditCont::GetWnd(CWnd* pParent,UINT ctrlID)
{
	if((HWND)*this==NULL)
	{
		CRect rc(0,0,0,0);
		Create(ES_AUTOHSCROLL|ES_LEFT|WS_CHILD|WS_BORDER,rc,pParent,ctrlID);
	}
	return this;
}

CEditCont::~CEditCont()
{
	if((HWND)*this!=NULL)
		this->DestroyWindow();
}

void CEditCont::OnTextChanged()
{
	if(IS_NULL(m_pouteredit)) return;
	if(!HasFocus(this)) return;
	CString txt;
	GetWindowText(txt);
	m_pouteredit->SetWindowText(txt);
}

void CListHelper::UpdateListStates(int _row)
{
	CListCtrl& lc = GetListCtrl();
	int ilast = lc.GetItemCount()-1;
	if(_row<0 || _row>ilast) return;
	UINT curselflg = m_bShowFocusAsSelection?LVIS_FOCUSED:LVIS_SELECTED;
	if(m_bAddNewLine) ilast--;
	int lpos = lc.GetNextItem(-1,LVNI_FOCUSED);
	if(lpos>=0)
		lc.SetItemState(lpos,0,LVIS_FOCUSED);
	lc.SetItemState(_row,LVIS_FOCUSED,LVIS_FOCUSED);
	bool bMultiSelect = EQL(0,lc.GetStyle()&LVS_SINGLESEL);
	while(bMultiSelect)
	{
		UINT selby = m_bShowFocusAsSelection?LVNI_SELECTED:LVNI_FOCUSED;
		bool bCtrlKey = NEQL(0,0x8000&GetKeyState(VK_CONTROL));
		if(NEQL(0,0x8000&GetKeyState(VK_SHIFT)))
		{//select list items 
			int lpos = lc.GetNextItem(-1,selby);
			if(lpos==-1) break;
			int i1,i2,i;
			i1 = min(lpos,_row);
			i2 = max(lpos,_row);
			i2 = min(ilast,i2);
			if(!bCtrlKey)
				for(i=0;i<i1;i++)
					lc.SetItemState(i,0,LVIS_SELECTED);
			for(;i1<=i2;i1++)
			{
				int irow = get_SourceRow(i1);
				bool bcanselect;
				if(m_pitemselect) 
					bcanselect = m_pitemselect->can_SelectListItem(irow,SPACE_ROW);
				else bcanselect = true;
				if(bcanselect) lc.SetItemState(i1,LVIS_SELECTED,LVIS_SELECTED);
			}
			if(!bCtrlKey)
			{
				i2 = ilast;
				for(i1;i1<i2;i1++)
					lc.SetItemState(i1,0,LVIS_SELECTED);
			}
		}
		else if(bCtrlKey)
		{
			toggle_Selection(_row);
		}
		else
		{
			int lpos = -1;
			while((lpos=lc.GetNextItem(lpos,LVNI_SELECTED))>=0)
				lc.SetItemState(lpos,0,LVIS_SELECTED);
			bool bcanselect;
			if(m_pitemselect) 
			{
				int irow = get_SourceRow(_row);
				bcanselect = m_pitemselect->can_SelectListItem(irow,SPACE_ROW);
			}
			else bcanselect = true;
			if(bcanselect) lc.SetItemState(_row,LVIS_SELECTED,LVIS_SELECTED);
		}
		return;
	}
	if(NEQL(0,curselflg&LVIS_SELECTED))
	{
		int lpos = lc.GetNextItem(-1,LVNI_SELECTED);
		if(lpos>=0)
			lc.SetItemState(lpos,0,LVIS_SELECTED);
		bool bcanselect;
		if(m_pitemselect) 
		{
			int irow = get_SourceRow(_row);
			bcanselect = m_pitemselect->can_SelectListItem(irow,SPACE_ROW);
		}
		else bcanselect = true;
		if(bcanselect) lc.SetItemState(_row,LVIS_SELECTED,LVIS_SELECTED);
		//lc.SetItemState(_row,LVIS_SELECTED,LVIS_SELECTED);
	}
}

bool CListHelper::can_Focused(int _row,int _col,bool _bapllynewpos/*=false*/)
{
	if(IS_NULL(m_pitemselect)) return true;
	int inrow,incol,iorow,iocol;
	inrow = get_SourceRow(_row);
	incol = get_SourceColumn(_col);
	iorow = get_SourceRow(get_FocusedRow(true));
	iocol = get_SourceColumn(m_nFocusedColumn);
	if(!m_pitemselect->can_FocusedListItem(iorow,iocol,inrow,incol)) return false;
	if(!_bapllynewpos) return true;
	m_pitemselect->focused_ListItem(inrow,incol);
	return true;
}

BOOL CListHelper::OnLButtonDown(UINT nFlags, CPoint point,GridDataSource& _data,GridDataSetter* _psetter/*=NULL*/,GridCellInformer* _gridinfo/*=NULL*/)
{
	int row,col;
	bool bEditable = NEQL(0,This()->GetStyle()&LVS_EDITLABELS);
	CListCtrl& lc = GetListCtrl();
	if(IsControlActive())
		EndLabelEdit((LPCTSTR)TRUE,GetEditCtrl0(),_psetter);
	int nOldSelCol = m_nFocusedColumn;
	UINT curselflg = m_bShowFocusAsSelection?LVIS_FOCUSED:LVIS_SELECTED;
	if(!FindRowCol(point,row,col)) 
	{
		if(m_bCellFocused) m_nFocusedColumn = -1;	//should select all row;
		RedrawList();
		if(m_pitemselect)
			m_pitemselect->focused_ListItem(-1,-1);
		return FALSE;
	}
	else if(!m_bCellFocused) col=-1;
	if(!can_Focused(row,col,true)) return TRUE;
	m_nFocusedColumn = col;
	UINT state = lc.GetItemState(row,curselflg);
	if(!bEditable) 
	{
		UpdateListStates(row);
		return TRUE;
	}
	if(EQL(state,0)) 
	{
		UpdateListStates(row);
		return TRUE; 
	}
	lc.RedrawItems(row,row);
	if(m_bCellFocused && nOldSelCol!=m_nFocusedColumn)
		return TRUE;
	if(_gridinfo)
	{
		if(!can_Edit(row,m_nFocusedColumn,_gridinfo)) return TRUE;
	}
	BeginLabelEdit(row,m_nFocusedColumn,_data,_gridinfo);
	return TRUE;
}

BOOL CListHelper::OnLButtonDblClk(UINT nFlags, CPoint point,GridDataSource& _data,GridDataSetter* _psetter/*=NULL*/,GridCellInformer* _gridinfo/*=NULL*/)
{
	int row,col;
	CListCtrl& lc = GetListCtrl();
	bool bEditable = NEQL(0,This()->GetStyle()&LVS_EDITLABELS);
	if(IsControlActive())
		EndLabelEdit((LPCTSTR)TRUE,GetEditCtrl0(),_psetter);
	int nOldSelCol = m_nFocusedColumn;
	UINT curselflg = m_bShowFocusAsSelection?LVIS_FOCUSED:LVIS_SELECTED;
	if(!FindRowCol(point,row,col)) 
	{
		if(m_bCellFocused) m_nFocusedColumn = -1;	//should select all row;
		RedrawList();
		return FALSE;
	}else m_nFocusedColumn = m_bCellFocused?col:-1;
	lc.RedrawItems(row,row);
	if(!bEditable) return FALSE;
	if(_gridinfo)
	{
		if(!can_Edit(row,m_nFocusedColumn,_gridinfo)) return TRUE;
	}
	BeginLabelEdit(row,m_nFocusedColumn,_data,_gridinfo);
	return TRUE;
}

void CListHelper::BeginLabelEdit(int _irow,int _icol,GridDataSource& _data,GridDataSetter& _setter,GridCellInformer* _gridinfo,bool _sellectall/*=false*/)
{
	CEdit* pedit = GetEditCtrl0();
	EndLabelEdit((LPCTSTR)TRUE,pedit,&_setter);
	int row = get_ListRow(_irow);
	int col = get_ListColumn(_icol);
	BeginLabelEdit(row,col,_data,_gridinfo);
	if(_sellectall && pedit) pedit->SetSel(0,-1);
}

int CListHelper::get_SourceColumn(int _col)
{
	return _col;
}

bool CListHelper::FindRowCol(const CPoint& pt,int& _row,int& _col,bool _bheader/*=false*/,bool _hstatecol/*=false*/,bool _blistpos/*=true*/)
{
	CListCtrl& lc = GetListCtrl();

	UINT hitflags;
	_row = SPACE_ROW;
	_col = SPACE_ROW;
	if(_bheader && lc.GetHeaderCtrl()!=NULL)//test if point belong to header control
	{
		CHeaderCtrl& hc = *lc.GetHeaderCtrl();
		CRect hrc;
		hc.GetWindowRect(hrc);
		CPoint pt1 = pt;
		lc.ClientToScreen(&pt1);
		//int scrollx = lc.GetScrollPos(SB_HORZ);
		//pt1.x += scrollx;
		if(hrc.PtInRect(pt1)) _row=HEADER_ROW;
	}

	if(_row!=HEADER_ROW)
	{
		_row = lc.HitTest(pt,&hitflags);
		if(_row<0 || (hitflags & LVHT_ONITEMICON)==0) return false;
		if(!_blistpos)_row = get_SourceRow(_row);
	}

	LV_COLUMN lvc;
	lvc.mask = LVCF_WIDTH;
	int nColumn;
	CRect rcLabel,rcAll;
	lc.GetItemRect(0,&rcLabel,LVIR_LABEL);
	int offset = rcLabel.left;
	lc.GetItemRect(0,&rcAll,LVIR_BOUNDS);
	CRect rect(0,0,rcLabel.left-lc.GetScrollPos(SB_HORZ),0);
	if(_hstatecol
		&& pt.x>rcAll.left-lc.GetScrollPos(SB_HORZ)
		&& pt.x<rcLabel.left-lc.GetScrollPos(SB_HORZ))
	{
		_col = STATE_COLUMN;
		return true;
	}
	int coli=0;
	bool bStateImages = NOT_NULL(lc.GetImageList(LVSIL_STATE));
	if(bStateImages) 
	{
		rect.right = lc.GetColumnWidth(coli++) - lc.GetScrollPos(SB_HORZ);
	}
	for(nColumn=0;lc.GetColumn(coli,&lvc);nColumn++,coli++)
	{
		rect.left = rect.right;
		rect.right = rect.left+lvc.cx-((coli==0)?offset:0);
		if(pt.x<rect.right) 
		{
			_col = nColumn;
			if(!_blistpos)_col=get_SourceColumn(_col);
			return true;
		}
	}
	return false;
}

void CListHelper::OnEndlabelEdit(LPCTSTR szText,GridDataSetter& _setter)
{
	EndLabelEdit(szText ,GetEditCtrl0(),&_setter);
}


void CEditCont::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nChar)
	{
	case VK_RETURN:
	case VK_ESCAPE:
		{
			bool bEsc = nChar==VK_ESCAPE;
			LV_DISPINFO dispinfo;
			dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
			dispinfo.hdr.idFrom = GetDlgCtrlID();
			dispinfo.hdr.code = LVN_ENDLABELEDITA;

			dispinfo.item.mask = LVIF_TEXT;
			dispinfo.item.iItem = -1;
			dispinfo.item.iSubItem = -1;
			dispinfo.item.pszText = bEsc ? NULL : (LPTSTR)TRUE;
			dispinfo.item.cchTextMax = 0;
			//GetParent()->
			SendMessage(WM_NOTIFY, GetDlgCtrlID(),(LPARAM)&dispinfo);
		}
		break;
	case VK_TAB:
		{
			GetParent()->SendMessage(WM_KEYDOWN,(WPARAM)nChar,MAKELONG(nRepCnt,nFlags));
			break;
		}
	default:
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

//requared SetGridData
//function not automaticaly call SetGridData so 
//on fly call SetGridData to apply filter to the grid data
//call this function in initialization before SetGridData or InitGrid
void CListHelper::set_SortedMode(bool _enable/*=true*/,IListSort* _plistsort/*=NULL*/)
{
	m_bsorted = _enable;
	GetListCtrl().ModifyStyle(m_bsorted?0:LVS_NOSORTHEADER,m_bsorted?LVS_NOSORTHEADER:0
		,SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE
		);
	ASSERT((_enable && NOT_NULL(_plistsort)) || !_enable);
	m_pdatasort = _plistsort;
	m_sortedrows.clear();
	m_sortimages.clear();
}

void CListHelper::InitColumns(GridDataSource &_data, CDC *_pDC,ColumnParamsLst* _columnparams/*=NULL*/)
{
	This()->LockWindowUpdate();
	ASSERT(NOT_NULL(_pDC));
	if(IS_NULL(_pDC)) return;
	resort(m_sortcolumn,m_sortnum,_data);
	CListCtrl& lc = GetListCtrl();

	int coli,colcnt,coli1;
	colcnt = _data.GetCols();
	LVCOLUMN lvc;
	ZeroObj(lvc);
	lvc.mask = LVCF_WIDTH;
	if(::IsWindow(lc.GetSafeHwnd()))
		for(;lc.GetColumn(0,&lvc);)lc.DeleteColumn(0);

	AutoGdiObject<CFont> fnt(_pDC,*m_plistctrl->GetFont());

	std::vector<int> colswidth;
	colswidth.resize(colcnt,0);

	ColumnInfoTag cit;

	ZeroObj(lvc);
	lvc.mask = LVCF_FMT|LVCF_TEXT|LVCF_SUBITEM;
	bool bdelfisrt = false;
	coli1 =0;
	CImageList* pImageList;
	pImageList = lc.GetImageList(LVSIL_STATE);
	bool bStateImages = NOT_NULL(pImageList);
	if(bStateImages)
	{
		lvc.iSubItem = colcnt+1;
		lvc.pszText = (LPTSTR)_T("");
		lvc.fmt = LVCFMT_LEFT;
		lc.InsertColumn(0,&lvc);
		coli1++;
	}
	
	for(coli=0;coli<colcnt;coli++,coli1++)
	{
		CString header;
		int fmt;
		_data.GetColumnHeader(coli,header,fmt);
		lvc.iSubItem = coli;
		lvc.pszText = (LPTSTR)(LPCTSTR)header;
		lvc.fmt = fmt;
		if(EQL(coli1,0) && NEQL(LVCFMT_LEFT,fmt&LVCFMT_JUSTIFYMASK))
		{//bug of the list ctrl. It set first column alignment always to the left
			// so we need to set first column and then delete it
			bdelfisrt = true;
			lvc.iSubItem = colcnt;
			lvc.fmt = LVCFMT_LEFT;
			lc.InsertColumn(0,&lvc);
			lvc.iSubItem = coli;
			lvc.fmt = fmt;
			coli1++;
		}
		CSize txtsz = _pDC->GetTextExtent(header);
		colswidth[coli] = txtsz.cx;
		lvc.cx = colswidth[coli];
		lc.InsertColumn(coli1,&lvc);
		lvc.mask &= ~LVCF_IMAGE;
	}
	if(bdelfisrt) lc.DeleteColumn(0);

	SetHeaderSortImages();
	int rowcnt,rowi;
	rowcnt = get_Rows(_data);
	if(m_bAddNewLine) rowcnt--;
	pImageList = lc.GetImageList(LVSIL_NORMAL);
	int nImgWidth;
	if(NOT_NULL(pImageList))
	{
		IMAGEINFO ii;
		pImageList->GetImageInfo(0,&ii);
		nImgWidth = CRect(ii.rcImage).Width();
	}
	else nImgWidth = 16;

	for(coli=0;coli<colcnt;coli++)
	{
		if(_columnparams)
		{
			ColumnParams* cp = getColumn(*_columnparams,coli);
			if(cp && (bool)*cp) continue;
		}
		for(rowi=0;rowi<rowcnt;rowi++)
		{
			_data.GetColumnInfo(-1,coli,cit);
			if(cit.m_type==ColumnInfoTag::CT_TEXT)
			{
				CString txt;
				_data.GetString(rowi,coli,txt);
				CSize txtsz = _pDC->GetTextExtent(txt);
				colswidth[coli] = max(colswidth[coli],txtsz.cx);
			}
			else if(cit.m_type==ColumnInfoTag::CT_IMAGE)
			{
				colswidth[coli] = max(nImgWidth,colswidth[coli]);
			}
		}
	}

	coli1 = 0;
	pImageList = lc.GetImageList(LVSIL_STATE);
	if(bStateImages)
	{
		IMAGEINFO ii;
		pImageList->GetImageInfo(0,&ii);
		int w = CRect(ii.rcImage).Width() + 2;
		lc.SetColumnWidth(coli1,w);
		coli1++;
	}
	int maxcellcx = ::GetSystemMetrics(SM_CXFULLSCREEN)/3;
	for(coli=0;coli<colcnt;coli++)
		colswidth[coli] = min(colswidth[coli],maxcellcx);
	if(_columnparams)
	{
		for(coli=0;coli<colcnt;coli++,coli1++)
		{
			ColumnParams* cp = getColumn(*_columnparams,coli);
			if(cp)
			{
				if(!(bool)*cp)cp->set_Width(colswidth[coli]+40);
				lc.SetColumnWidth(coli1,cp->get_Width());
			}
			else lc.SetColumnWidth(coli1,colswidth[coli]+40);
		}
	}
	else
		for(coli=0;coli<colcnt;coli++,coli1++)
		{
			lc.SetColumnWidth(coli1,colswidth[coli]+40);
		}
	This()->UnlockWindowUpdate();
}

void CListHelper::SetGridData(GridDataSource &_data,CDC* _pDC/*=NULL*/,ColumnParamsLst* _columnparams/*=NULL*/)
{
	if(!m_bresorted && m_bSaveGridState) get_Visible(m_lastva,true);
	resort(m_sortcolumn,m_sortnum,_data);

	CListCtrl& lc = GetListCtrl();
	int coli,colcnt;
	colcnt = _data.GetCols();
	bool bownerdata = lc.GetStyle()&LVS_OWNERDATA;
	
	if(!bownerdata)
	{
		lc.DeleteAllItems();
		LV_ITEM lvi;
		ZeroMemory(&lvi,sizeof(lvi));

		int rowcnt,rowi;
		rowcnt = get_Rows(_data);
		lvi.mask = LVIF_TEXT|LVIF_STATE;
		for(rowi=0;rowi<rowcnt;rowi++)
		{
			lvi.iSubItem = 0;
			lvi.iItem = rowi;
			CString txt;
			int rowi1 = get_Row(rowi,rowcnt);
			_data.GetString(rowi1,0,txt);
			lvi.pszText = (LPTSTR)(LPCTSTR)txt;
			lc.InsertItem(&lvi);
			LPARAM lparam;
			_data.GetParam(rowi1,lparam);
			lc.SetItemData(rowi1,lparam);

			for(coli=1;coli<colcnt;coli++)
			{
				_data.GetString(rowi1,coli,txt);
				lvi.iSubItem = coli;
				lvi.pszText = (LPTSTR)(LPCTSTR)txt;
				lc.SetItem(&lvi);
			}
		}
		RedrawList();
	}
	else
	{
		lc.SetItemCount(get_Rows(_data));
		lc.Invalidate();
	}
	if(m_bSaveGridState)
	{
		int i,rowi,rowcnt;
		rowcnt = get_Rows(_data);
		for(i=0;i<rowcnt;i++)
		{
			rowi = get_SourceRow(i);
			Utils::ItemID* _id;
			_data.GetRowID(rowi,_id);
			Utils::IDWithData<RowState>* prowstate = m_idhelper.reset_ID(_id);
			if(IS_NULL(prowstate) || !(bool)*prowstate) continue;
			prowstate->data.irow = i;
			lc.SetItemState(i,prowstate->data.state,0xff);
		}
		m_idhelper.eraseold();
		set_Visible(m_lastva,&_data);
	}else
	{
		int i,rowcnt;
		rowcnt = get_Rows(_data);
		for(i=0;i<rowcnt;i++)
		{
			lc.SetItemState(i,0,0xff);
		}
	}
	set_Tooltips(_data,_pDC,_columnparams);
}

BOOL CEditCont::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message==WM_KEYDOWN /*&& m_bSendNotification*/)
	{
		SHORT sCtrlKey = GetKeyState(VK_CONTROL);
		if( pMsg->wParam==VK_RETURN
			|| pMsg->wParam==VK_ESCAPE
			|| sCtrlKey
			)
		{
			::TranslateMessage(pMsg);
			//if(pMsg->wParam!=VK_ESCAPE)
			{
				::DispatchMessage(pMsg);
			}
			return TRUE;
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}

BOOL CListHelper::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)	
	{
		if(pMsg->wParam==VK_RETURN 
			|| pMsg->wParam==VK_TAB
			)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;
		}
	}
	if(m_bShowTooltips && IsWindow(m_tooltipctrl))
	{
		m_tooltipctrl.RelayEvent(pMsg);
	}
	return FALSE;
}

void CListHelper::RedrawList()
{
	int ti,bi;
	CListCtrl& lc = GetListCtrl();
	if(!::IsWindow(lc.GetSafeHwnd())) return;
	ti = lc.GetTopIndex();
	bi = ti + lc.GetCountPerPage();
	bi = min(lc.GetItemCount(),bi);
	CScrollInfo si;
	lc.GetScrollInfo(SB_HORZ,&si);
	lc.RedrawItems(ti,bi);
	//lc.SetScrollPos(SB_HORZ,si.nPos);
	lc.Scroll(CSize(0,0));
}

void CListHelper::InitGrid(GridDataSource& _data,CDC* pDC,ColumnParamsLst* _columnparams/*=NULL*/)
{
	resort(m_sortcolumn,m_sortnum,_data);
	if(m_bSaveGridState) get_Visible(m_lastva,true);
	m_bresorted = true;
	InitColumns(_data,pDC,_columnparams);
	SetGridData(_data,pDC,_columnparams);
	m_bresorted = false;
}

CListHelper::CListHelper()
{
	m_bFullRowSel = FALSE;
	m_bClientWidthSel = TRUE;

	m_cxClient = 0;
	m_cxStateImageOffset = 0;
	//m_noffsheader = 0;

	//m_clrText = ::GetSysColor(COLOR_WINDOWTEXT);
	//m_clrTextBk = ::GetSysColor(COLOR_WINDOW);
	//m_clrBkgnd = ::GetSysColor(COLOR_WINDOW);
	m_clrCursor = RGB(0x0,0x0,0xa0);
	m_pwnd = NULL;
	m_plistctrl = NULL;
	m_edrow = m_edcol = -1;

	m_blkpen.CreatePen(PS_SOLID,1,GetSysColor(COLOR_WINDOWTEXT));
	m_gridpen.CreatePen(PS_SOLID,1,GetSysColor(COLOR_3DSHADOW));
	m_bAddNewLine = false;
	m_nFocusedColumn = NOTSELECTED;
	m_bCellFocused = false;
	m_pdatasort = NULL;
	m_bsorted = m_bresorted = false;
	m_sortcolumn = 0;
	m_sortnum = 0;
	m_bShowNoItemMessage = true;
	m_sNoItemMessage = _T("---");
	m_bSaveGridState = false;
	m_bShowFocusAsSelection = false;
	m_pitemselect = NULL;
	m_edrow = SPACE_ROW;
	m_bShowTooltips = false;
	m_drawmode = LDM_STANDARD;
}

CListHelper::~CListHelper()
{
}

void CListHelper::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~LVS_TYPEMASK;
	cs.style |= LVS_REPORT|LVS_OWNERDRAWFIXED|LVS_SINGLESEL|LVS_OWNERDATA|LVS_SHOWSELALWAYS;
}

BOOL CListHelper::OnSetImageList(WPARAM wParam, LPARAM lParam)
{
	OSVERSIONINFO info;
	info.dwOSVersionInfoSize = sizeof(info);
	VERIFY(::GetVersionEx(&info));

	if((int)wParam==LVSIL_STATE)
	{
		if(info.dwMajorVersion<4)
		{
			int cx, cy;
	
			if(::ImageList_GetIconSize((HIMAGELIST)lParam, &cx, &cy))
				m_cxStateImageOffset = cx;
			else
				m_cxStateImageOffset = 0;
		}
		/*CHeaderCtrl* phc = GetListCtrl().GetHeaderCtrl();
		if(NOT_NULL(phc) && NEQL(0,lParam))
		{
			CImageList il;
			il.Attach((HIMAGELIST)lParam);
			IMAGEINFO ii;
			il.GetImageInfo(0,&ii);
			m_noffsheader = CRect(ii.rcImage).Width() + 2;
			il.Detach();
			CRect rcHeader,rcList;
			phc->GetWindowRect(&rcHeader);
			GetListCtrl().GetClientRect(&rcList);
			GetListCtrl().ClientToScreen(&rcList);
			phc->GetParent()->ScreenToClient(&rcHeader);
			phc->GetParent()->ScreenToClient(&rcList);
			rcHeader.left = m_noffsheader+rcHeader.left-rcList.left;
			phc->SetWindowPos(NULL,rcHeader.left,rcHeader.top,0,0
				,SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE);
		}*/
	}
	return FALSE;
}
/*
void CListHelper::AdjustHeader()
{
	CListCtrl& lc = GetListCtrl();
	CHeaderCtrl* phc = lc.GetHeaderCtrl();
	if(IS_NULL(phc)) return;
	CRect rcHeader,rcList;
	phc->GetWindowRect(&rcHeader);
	lc.GetClientRect(&rcList);
	lc.ClientToScreen(&rcList);
	phc->GetParent()->ScreenToClient(&rcHeader);
	phc->GetParent()->ScreenToClient(&rcList);
	rcHeader.left = m_noffsheader+rcHeader.left-rcList.left;
	phc->SetWindowPos(NULL,rcHeader.left,rcHeader.top,0,0
		,SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE);
}*/

BOOL CListHelper::OnSetTextColor(WPARAM wParam, LPARAM lParam)
{
	//m_clrText = (COLORREF)lParam;
	return FALSE;
}

BOOL CListHelper::OnSetTextBkColor(WPARAM wParam, LPARAM lParam)
{
	//m_clrTextBk = (COLORREF)lParam;
	return FALSE;
}

BOOL CListHelper::OnSetBkColor(WPARAM wParam, LPARAM lParam)
{
	//m_clrBkgnd = (COLORREF)lParam;
	return FALSE;
}

void CListHelper::OnSize(UINT nType, int cx, int cy)
{
	m_cxClient = cx;
}

void CListHelper::OnPaint()
{
	if(IS_NULL(This()) || !IsWindow(*This())) return;
	if (m_bClientWidthSel &&
		(This()->GetStyle() & LVS_TYPEMASK) == LVS_REPORT &&
		GetFullRowSel())
	{
		CRect rcAllLabels;
		GetListCtrl().GetItemRect(0, rcAllLabels, LVIR_BOUNDS);

		if(rcAllLabels.right < m_cxClient)
		{
			// need to call BeginPaint (in CPaintDC c-tor)
			// to get correct clipping rect
			CPaintDC dc(This());

			CRect rcClip;
			dc.GetClipBox(rcClip);

			rcClip.left = min(rcAllLabels.right-1, rcClip.left);
			rcClip.right = m_cxClient;

			This()->InvalidateRect(rcClip, FALSE);
			// EndPaint will be called in CPaintDC d-tor
		}
	}
/*	CListCtrl& lc = GetListCtrl();
	if(m_states.size()!=lc.GetItemCount())
		m_states.resize(lc.GetItemCount(),0);
	//vector<UINT> states;
	//states.resize(lc.GetItemCount(),0);
	int i;
	bool eql=true;
	for(i=0;i<lc.GetItemCount();i++)
	{
		DWORD state=lc.GetItemState(i,0xffff);
		eql &= (state==m_states[i]);
		m_states[i]=state;
	}
	if(!eql)
	{
		TRACE("void CListHelper::OnPaint()\n");
		for(i=0;i<lc.GetItemCount();i++)
		{
			TRACE("%i %08x\n",i,m_states[i]);
		}
	}
*/
}

void CListHelper::OnSetFocus(CWnd* pOldWnd)
{
	// check if we are getting focus from label edit box
	if(pOldWnd!=NULL && pOldWnd->GetParent()==This())
		return;

	// repaint items that should change appearance
	if(m_bFullRowSel && (This()->GetStyle() & LVS_TYPEMASK)==LVS_REPORT)
		RepaintSelectedItems();
}

void CListHelper::OnKillFocus(CWnd* pNewWnd)
{
	if(pNewWnd != NULL && pNewWnd->GetParent() == This())
		return;

	// repaint items that should change appearance
	if(m_bFullRowSel && (This()->GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
		RepaintSelectedItems();
}


BOOL CListHelper::SetFullRowSel(BOOL bFullRowSel)
{
	// no painting during change
	m_bFullRowSel = bFullRowSel;

	BOOL bRet;

	if (m_bFullRowSel)
		bRet = m_pwnd->ModifyStyle(0L, LVS_OWNERDRAWFIXED);
	else
		bRet = m_pwnd->ModifyStyle(LVS_OWNERDRAWFIXED, 0L);

	// repaint window if we are not changing view type
	if (bRet && (m_pwnd->GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
		m_pwnd->Invalidate();

	// repaint changes
	return(bRet);
}

void CListHelper::RepaintSelectedItems()
{
	CListCtrl& ListCtrl = GetListCtrl();
	CRect rcItem, rcLabel;

// invalidate focused item so it can repaint properly
	int nItem = ListCtrl.GetNextItem(-1, LVNI_FOCUSED);

	if(nItem != -1)
	{
		ListCtrl.GetItemRect(nItem, rcItem, LVIR_BOUNDS);
		ListCtrl.GetItemRect(nItem, rcLabel, LVIR_LABEL);
		rcItem.left = rcLabel.left;

		m_pwnd->InvalidateRect(rcItem, FALSE);
	}
	
// if selected items should not be preserved, invalidate them
/*	if(!(m_pwnd->GetStyle() & LVS_SHOWSELALWAYS))
	{
		for(nItem = ListCtrl.GetNextItem(-1, LVNI_SELECTED);
			nItem != -1; nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED))
		{
			ListCtrl.GetItemRect(nItem, rcItem, LVIR_BOUNDS);
			ListCtrl.GetItemRect(nItem, rcLabel, LVIR_LABEL);
			rcItem.left = rcLabel.left;

			m_pwnd->InvalidateRect(rcItem, FALSE);
		}
	}
*/
// update changes
	m_pwnd->UpdateWindow();
}

void CListHelper::get_Visible(VisibleArea& _va,bool _bxscroll/*=false*/)
{
	CListCtrl& lc = GetListCtrl();
	_va.bottom = _va.top = lc.GetTopIndex();
	_va.bottom += lc.GetCountPerPage();
	if(_va.bxscroll = _bxscroll)
	{
		_va.xscroll = lc.GetScrollPos(SB_HORZ);
	}
	if(NOT_NULL(_va.m_focusrowid))
	{
		_va.m_yscroll = _va.m_focusedrow-_va.top;
	}
}

int CListHelper::find_RowbyID(GridDataSource* _pdata,Utils::ItemID* _rowid)
{
	if(IS_NULL(_pdata) || IS_NULL(_rowid)) return -1;
	Utils::IDWithData<RowState>* prs = m_idhelper.get(_rowid);
	if(IS_NULL(prs)) return -1;
	return prs->data.irow;
}

void CListHelper::set_Visible(const VisibleArea& _va,GridDataSource* _pdata/*=NULL*/)
{
	CListCtrl& lc = GetListCtrl();
	int nFocusedRow = find_RowbyID(_pdata,_va.m_focusrowid);
	int top,bottom;
	if(nFocusedRow>=0)
	{
		set_Focused(nFocusedRow,m_nFocusedColumn,false);
		top = nFocusedRow-_va.m_yscroll;
		top = max(top,0);
	}
	else
	{
		//bottom = _va.bottom;
		top = _va.top;
	}
	bottom = top+lc.GetCountPerPage();
	EnsureVisible(_va.bottom,TRUE);
	EnsureVisible(_va.top,FALSE);
	if(_va.bxscroll)
	{
		//lc.SetScrollPos(SB_HORZ,_va.xscroll);
		CScrollInfo si;
		lc.GetScrollInfo(SB_HORZ,&si);
		lc.Scroll(CSize(_va.xscroll-si.nPos,0));
	}
}

BOOL CListHelper::OnEraseBkgnd(CDC* pDC)
{
	if(IS_NULL(This()) || !IsWindow(*This()) ||
		!This()->IsWindowEnabled()) return FALSE;
	CListCtrl& lc = GetListCtrl();
	CRect rc;
	CBrush /* *poldbr,*/bkgrndbr;
	//bkgrndbr.Attach((HGDIOBJ)GetClassLong(m_pwnd->GetSafeHwnd(),GCL_HBRBACKGROUND));
	if(EQL(m_drawmode,LDM_STANDARD))
	{
		bkgrndbr.CreateSolidBrush(GetSysColor(COLOR_WINDOW));
		//bkgrndbr.Attach((HGDIOBJ)GetClassLong(m_pwnd->GetSafeHwnd(),GCL_HBRBACKGROUND));
		//bkgrndbr.UnrealizeObject();
	}
	else if(EQL(m_drawmode,LDM_DELPHIGRIG))
		bkgrndbr.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	//bkgrndbr.UnrealizeObject();
	m_pwnd->GetClientRect(&rc);
	if(lc.GetItemCount()<2)
	{
		pDC->FillRect(&rc,&bkgrndbr);
		if(m_bShowNoItemMessage && EQL(0,lc.GetItemCount()))
		{
			CRect wrc;
			lc.GetClientRect(&wrc);
			CString noitemtxt;
			noitemtxt = m_sNoItemMessage;
			AutoGdiObject<CFont> __af(pDC,lc.GetFont());

			pDC->SetBkMode(TRANSPARENT);
			COLORREF clrtxtold = pDC->SetTextColor(
				::GetSysColor(EQL(m_drawmode,LDM_STANDARD)?COLOR_WINDOWTEXT
					:(EQL(m_drawmode,LDM_DELPHIGRIG)?COLOR_BTNTEXT:COLOR_WINDOWTEXT)
					)
				);
				
			CHeaderCtrl* pheader = lc.GetHeaderCtrl();
			int scrollx = lc.GetScrollPos(SB_HORZ);
			int left = -scrollx;
			int right = left;
			LV_COLUMN lvc;
			lvc.mask = LVCF_WIDTH;
			int nColumn;
			for(nColumn=0;lc.GetColumn(nColumn,&lvc);nColumn++)
				right += lvc.cx;
			wrc.left = left;
			wrc.right = right;
			if(pheader)
			{
				CRect rcheader;
				pheader->GetWindowRect(rcheader);
				pheader->GetParent()->ScreenToClient(&rcheader);
				wrc.top = rcheader.bottom+1;
			}
			pDC->DrawText(noitemtxt,&wrc,DT_TOP|DT_CENTER|DT_WORDBREAK);
			pDC->SetTextColor(clrtxtold);
		}
	}
	else
	{
		CRect rcitem,rcitem1;
		int topi,bottomi;
		topi = lc.GetTopIndex();
		//if(!EnsureVisible(topi,FALSE)) ++topi;
		lc.GetItemRect(topi,&rcitem,LVIR_BOUNDS);
		CRect frc(rcitem.right+1,rc.top,rc.right,rc.bottom);
		pDC->FillRect(frc,&bkgrndbr);
		bottomi = lc.GetTopIndex() + lc.GetCountPerPage();
		if(bottomi>lc.GetItemCount()) bottomi = lc.GetItemCount() - 1;
		lc.GetItemRect(bottomi,&rcitem,LVIR_BOUNDS);
		--bottomi;
		lc.GetItemRect(bottomi,&rcitem1,LVIR_BOUNDS);
		frc = CRect(rc.left,max(rcitem.bottom+1,rcitem1.bottom+1),rc.right,rc.bottom);
		pDC->FillRect(frc,&bkgrndbr);
	}

	//if(EQL(m_drawmode,LDM_STANDARD)) bkgrndbr.Detach();
	return TRUE;
}

void CListHelper::ensureVisible(int _row,int _col/*=SPACE_ROW*/)
{
	CListCtrl& lc = GetListCtrl();
	if(_row>=lc.GetItemCount()) _row = lc.GetItemCount()-1;
	if(_row<0) _row = 0;
	EnsureVisible(_row,FALSE);
	bool bStateImages = NOT_NULL(lc.GetImageList(LVSIL_STATE));
	if(!(m_bCellFocused && _col>=0 && lc.GetHeaderCtrl() 
		&& _col<lc.GetHeaderCtrl()->GetItemCount()-(bStateImages?1:0))
		) return;
	CRect rc,rcclient;
	int nStateIconCx;
	//rc.left = 0;
	int offset;
	lc.GetItemRect(lc.GetTopIndex(),&rc,LVIR_LABEL);
	offset = rc.left;
	lc.GetItemRect(lc.GetTopIndex(),&rc,LVIR_BOUNDS);
	int i;
	int coli = _col;
	if(bStateImages) coli++;
	for(i=0;i<coli;i++)	rc.left += lc.GetColumnWidth(i);
	rc.right=rc.left+lc.GetColumnWidth(coli)-((coli==0)?offset:0);
	{
		CRect rc1,rc2;
		lc.GetItemRect(_row,&rc1,LVIR_BOUNDS);
		lc.GetItemRect(_row,&rc2,LVIR_LABEL);
		nStateIconCx = rc2.left - rc1.left;
	}
	int scrollx = lc.GetScrollPos(SB_HORZ);
	lc.GetClientRect(&rcclient);
	CScrollInfo si;
	if(rc.left<scrollx || rc.left>=scrollx+rcclient.right)
	{
		CSize sz;
		sz.cy = 0;
		sz.cx = rc.left;
		lc.GetScrollInfo(SB_HORZ,&si,SIF_ALL);
		sz.cx-=si.nPos;
		if(sz.cx>si.nMax/*-si.nPage*/) 
			sz.cx = si.nMax/*-si.nPage*/;
		si.fMask = SIF_POS;
		lc.Scroll(sz);
		scrollx = lc.GetScrollPos(SB_HORZ);
	}
	if(rc.right>scrollx+rcclient.right)
	{
		CSize sz;
		sz.cy = 0;
		int width = max(rc.Width(),rcclient.Width());
		sz.cx = rc.right - width;
		lc.GetScrollInfo(SB_HORZ,&si,SIF_ALL);
		sz.cx -= si.nPos;
		si.fMask = SIF_POS;
		lc.Scroll(sz);
	}
}


void CListHelper::select_Row(int _row,bool _addtoselection/*=false*/,bool _ensurevisible/*=false*/)
{
	CListCtrl& lc = GetListCtrl();
	if(!_addtoselection) deselectAll();
	lc.SetItemState(_row,LVIS_SELECTED,LVIS_SELECTED);
	if(_ensurevisible) ensureVisible(_row,m_nFocusedColumn);
}

void CListHelper::EnableWindow(BOOL bEnable)
{
	if(m_plistctrl==NULL) return;
	if(::IsWindow(m_plistctrl->GetSafeHwnd()))
	{
		m_plistctrl->EnableWindow(bEnable);
		//m_plistctrl->ModifyStyle(bEnable?0:WS_DISABLED,bEnable?WS_DISABLED:0);
	}
	CHeaderCtrl* phc = m_plistctrl->GetHeaderCtrl();
	if(phc && ::IsWindow(phc->GetSafeHwnd()))
	{
		//phc->EnableWindow(bEnable);
		//phc->ShowWindow(bEnable?SW_SHOW:SW_HIDE);
		phc->ModifyStyle(bEnable?WS_DISABLED:0,bEnable?0:WS_DISABLED);
		phc->InvalidateRect(NULL);
		phc->UpdateWindow();
	}
	RedrawList();
}

int CListHelper::get_FocusedRow(bool _blistrow)
{
	int irow = GetListCtrl().GetNextItem(-1,LVNI_FOCUSED);
	if(!_blistrow) irow = get_SourceRow(irow);
	return irow>=0?irow:DEFOCUSED;
}

int CListHelper::get_FocusedColumn(bool _blistcol)
{
	int icol = (m_bCellFocused)?m_nFocusedColumn:-1;
	if(!_blistcol) icol = get_SourceColumn(icol);
	return icol>=0?icol:DEFOCUSED;
}

BOOL CListHelper::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags,GridDataSource& _data,GridDataSetter* _psetter,GridCellInformer* _pgsi/*=NULL*/)
{
	CListCtrl& lc = GetListCtrl();
	int row = get_FocusedRow(true),col=m_nFocusedColumn;
	switch(nChar)
	{
	case VK_HOME:
		{
			if(m_bCellFocused) col = 0;
			else row = 0; 
			set_Focused(row,col,true);
			return TRUE;
		}
	case VK_END:
		{
			if(m_bCellFocused)
			{
				if(m_bAddNewLine && row==lc.GetItemCount()-1) return TRUE;
				col = _data.GetCols()-1;
				set_Focused(row,col,true);
			}
			else
			{
				row = get_Rows(_data)-1;
				set_Focused(row,SPACE_ROW,true);
			}
			return TRUE;
		}
	case VK_PRIOR://page up
		{
			int ti = lc.GetTopIndex();
			int page = lc.GetCountPerPage();
			if(row==ti) row-= page;
			else row = ti;
			row = max(row,0);
			set_Focused(row,col,true);
			return TRUE;
		}
	case VK_NEXT://page down
		{
			int ti = lc.GetTopIndex();
			int page = lc.GetCountPerPage();
			if(row==ti+page) row+=page;
			else row=ti+page;
			int maxrow = get_Rows(_data)-1;
			if(m_bAddNewLine && m_bCellFocused && NEQL(m_nFocusedColumn,0))--maxrow;
			row = min(row,maxrow);
			set_Focused(row,col,true);
			return TRUE;
		}
	case VK_INSERT:
		{
			if(row==DEFOCUSED) return FALSE;
			toggle_Selection(row);
			if(row>=lc.GetItemCount()-1) return TRUE;
			if(m_bAddNewLine && row==lc.GetItemCount()-2
				&& m_bCellFocused && m_nFocusedColumn!=0 
				) return TRUE;
			set_Focused(row+1,SPACE_ROW,true);
			return TRUE;
		}
	case VK_SPACE:
		{
			if(row==DEFOCUSED) return FALSE;
			toggle_Selection(row);
			return TRUE;
		}
	case VK_UP:
		{
			if(row==DEFOCUSED || row==0) 
				return TRUE;
			set_Focused(row-1,SPACE_ROW,true);
			return TRUE;
		}
	case VK_DOWN:
		{
			if(row==DEFOCUSED || row>=lc.GetItemCount()-1)
				return TRUE;
			if(m_bAddNewLine && row==lc.GetItemCount()-2
				&& m_bCellFocused && m_nFocusedColumn!=0 
				) 
				return TRUE;
			set_Focused(row+1,SPACE_ROW,true);
			return TRUE;
		}
	case VK_LEFT:
		{
			if(!m_bCellFocused) return TRUE; 
			if(m_bAddNewLine && row==lc.GetItemCount()-1) return TRUE;
			if(col>0) col--;
			else col = 0;
			set_Focused(row,col,true);
			return TRUE;
		}
	case VK_RIGHT:
		{
			if(!m_bCellFocused) return TRUE;
			if(m_bAddNewLine && row==lc.GetItemCount()-1) return TRUE;
			int mx = _data.GetCols();
			if(col>=mx-1 || col==NOTSELECTED) col=mx-1;
			else col++;
			set_Focused(row,col,true);
			return TRUE;
		}
	case VK_RETURN:
		{
			if(!_pgsi || !_psetter) return FALSE;
			if(!m_bCellFocused) col=0;
			if(!can_Focused(row,col)) return TRUE;
			if(!can_Edit(row,col,_pgsi)) return TRUE;
			BeginLabelEdit(row,col,_data,*_psetter,_pgsi);
			return TRUE;
		}
	case VK_TAB:
		{
			if((signed short)GetAsyncKeyState(VK_SHIFT)>=0)
				next_Item(_data,_psetter,_pgsi);//simple TAB pressed
			else 
				prev_Item(_data,_psetter,_pgsi);//shift-TAB pressed
			return TRUE;
		}
	}
	return TRUE;
}

bool CListHelper::IsControlActive()
{
	CEdit* pedit = GetEditCtrl0();
	return pedit->IsWindowVisible();
}

void CListHelper::resort(int _col,int _sortnum,GridDataSource& _data)
{
	if(m_bresorted || IS_NULL(m_pdatasort)) return;
	m_sortedrows.resize(_data.GetRows()+(m_bAddNewLine?-1:0));
	m_sortimages.resize(_data.GetCols());
	std::generate(m_sortedrows.begin(),m_sortedrows.end(),Counter(0));
	m_pdatasort->sort(_col,_sortnum,m_sortedrows,m_sortimages);
//	ASSERT(m_bAddNewLine && m_sortedrows.size()==_data.GetRows()-1
//		|| !m_bAddNewLine && m_sortedrows.size()==_data.GetRows());
}

void CListHelper::OnHeaderClicked(int _col,GridDataSource& _data,GridDataSetter* _psetter,CDC* _pDC/*=NULL*/,ColumnParamsLst* _columnparams/*=NULL*/)
{
	CListCtrl& lc = GetListCtrl();
	bool bStateImages = NOT_NULL(lc.GetImageList(LVSIL_STATE));
	if(bStateImages)_col--;
	if(_col==m_sortcolumn)++m_sortnum;
	else m_sortnum = 0;
	m_sortcolumn = _col;
	resort(m_sortcolumn,m_sortnum,_data);
	m_bresorted = true;
	if(m_bSaveGridState) get_Visible(m_lastva,true);
	if(NOT_NULL(_psetter))
		OnEndlabelEdit((LPCTSTR)TRUE,*_psetter);

	SetHeaderSortImages();
	SetGridData(_data,_pDC,_columnparams);
	m_bresorted = false; 
}

int CListHelper::get_Row(int _row,int _rowcnt)
{
	if(!m_bsorted || _row>=(int)m_sortedrows.size()
		|| _row==_rowcnt && m_bAddNewLine
		)return _row;
	return m_sortedrows[_row];
}

void CListHelper::reset_Headers(GridDataSource& _data)
{
	if(!m_bsorted) return;
	CListCtrl& lc = GetListCtrl();
	CHeaderCtrl* pheader = lc.GetHeaderCtrl();
	ASSERT(NOT_NULL(pheader));
	ASSERT(NEQL(0,m_sortimages.size()));
	if(IS_NULL(pheader) || EQL(0,m_sortimages.size())
		|| IS_NULL(pheader->GetImageList())
		)return;

	CString buf;
	HDITEM hi;
	ZeroObj(hi);
	LPTSTR szbuf = buf.GetBuffer(1024);
	hi.mask = HDI_FORMAT|HDI_WIDTH|HDI_TEXT;
	int coli,cnt,colilst;
	cnt = min(pheader->GetItemCount(),(int)m_sortimages.size());
	colilst = 0;
	bool bStateImages = NOT_NULL(lc.GetImageList(LVSIL_STATE));
	if(bStateImages) colilst++;
	for(coli=0;coli<cnt;++coli,++colilst)
	{
		//hi.mask &= ~(HDI_TEXT);
		hi.pszText = szbuf;
		hi.cchTextMax = 1024;
		hi.mask |= HDI_IMAGE;
		pheader->GetItem(colilst,&hi);
		//hi.mask |= HDI_TEXT;
		CString hstr;
		int fmt;
		_data.GetColumnHeader(coli,hstr,fmt);
		hi.pszText = (LPTSTR)(LPCTSTR)hstr;
		hi.cchTextMax = hstr.GetLength()+1;
		hi.fmt &= ~(HDF_CENTER|HDF_LEFT|HDF_RIGHT);
		hi.fmt |= 
			EQL(fmt,LVCFMT_CENTER)?HDF_CENTER
			:EQL(fmt,LVCFMT_LEFT)?HDF_LEFT
			:EQL(fmt,LVCFMT_RIGHT)?HDF_RIGHT:HDF_LEFT;
		if(m_sortimages[coli]>=0)
		{
			hi.fmt |= HDF_IMAGE;
			hi.iImage = m_sortimages[coli];
		}else 
		{
			hi.fmt &= ~(HDF_IMAGE);
			hi.mask &= ~(HDI_IMAGE);
		}
		pheader->SetItem(colilst,&hi);
	}
	buf.ReleaseBuffer(0);
	//AdjustHeader();
}

void CListHelper::SetHeaderSortImages()
{
	if(!m_bsorted) return;
	CListCtrl& lc = GetListCtrl();
	CHeaderCtrl* pheader = lc.GetHeaderCtrl();
	ASSERT(NOT_NULL(pheader));
	ASSERT(NEQL(0,m_sortimages.size()));
	if(IS_NULL(pheader) || EQL(0,m_sortimages.size())
		|| IS_NULL(pheader->GetImageList())
		)return;

	CString buf;
	HDITEM hi;
	ZeroObj(hi);
	hi.mask = HDI_FORMAT|HDI_TEXT|HDI_WIDTH;
	int i,cnt,colilst;
	bool bStateImages = NOT_NULL(lc.GetImageList(LVSIL_STATE));
	cnt = min(pheader->GetItemCount(),(int)m_sortimages.size());
	colilst = 0;
	if(bStateImages) colilst++;
	for(i=0;i<cnt;++i,++colilst)
	{
		hi.mask |= HDI_IMAGE;
		hi.pszText = buf.GetBuffer(1024);
		hi.cchTextMax = 1024;
		pheader->GetItem(colilst,&hi);
		if(m_sortimages[i]>=0)
		{
			hi.fmt |= HDF_IMAGE;
			hi.iImage = m_sortimages[i];
		}else 
		{
			hi.fmt &= ~(HDF_IMAGE);
			hi.mask &= ~(HDI_IMAGE);
		}
		pheader->SetItem(colilst,&hi);
		buf.ReleaseBuffer(0);
	}
	//AdjustHeader();
}

bool CListHelper::set_FocusedData(int _irow,int _icol/*=SPACE_ROW*/,bool _ensurevisible/*=true*/)
{
	CListCtrl& lc = GetListCtrl();
	int lrow,lcol;
	lrow = get_ListRow(_irow);
	lcol = get_ListColumn(_icol);
	return set_Focused(lrow,lcol,_ensurevisible);
}

bool CListHelper::set_Focused(int _row,int _col/*=SPACE_ROW*/,bool _ensurevisible/*=true*/)
{
	CListCtrl& lc = GetListCtrl();
	if(!can_Focused(_row,_col,true)) return false;
	if(m_bCellFocused && _col!=SPACE_ROW) m_nFocusedColumn = _col;
	int lastfrow = get_FocusedRow(true);
	if(lastfrow>=0 && lastfrow<lc.GetItemCount())
	{
		lc.SetItemState(lastfrow,0,LVIS_FOCUSED);
		lc.RedrawItems(lastfrow,lastfrow);
	}
	if(_row<0) return true;
	lc.SetItemState(_row,LVIS_FOCUSED,LVIS_FOCUSED);
	lc.RedrawItems(_row,_row);
	if(_ensurevisible) ensureVisible(_row,m_nFocusedColumn);
	//traceListStates(lc);
	return true;
}

int CListHelper::get_SelectedCount()
{
	CListCtrl& lc = GetListCtrl();
	//POSITION pos = lc.GetFirstSelectedItemPosition();
	int pos,cnt;
	for(pos=-1,cnt=0;(pos=lc.GetNextItem(pos,LVNI_SELECTED))>=0;cnt++)
	{}
	return cnt;
}

bool CListHelper::IsAnySelected()
{
	CListCtrl& lc = GetListCtrl();
	int pos = lc.GetNextItem(-1,LVNI_SELECTED);
	return pos>=0;
}


void CListHelper::toggle_Selection(int _row)
{
	CListCtrl& lc = GetListCtrl();
	if(_row<0 || _row>=lc.GetItemCount()
		|| (m_bAddNewLine && _row==lc.GetItemCount()-1)
		) return;
	TRACE("toggle_Selection(_row=%d)\n",_row);
	lc.SetItemState(_row
		,NEQL(0,lc.GetItemState(_row,LVIS_SELECTED))?0:LVIS_SELECTED
		,LVIS_SELECTED);
	lc.RedrawItems(_row,_row);
}

void CListHelper::selectAll()
{
	CListCtrl& lc = GetListCtrl();
	int irow,cnt;
	cnt = lc.GetItemCount() - (m_bAddNewLine?1:0);
	for(irow=0;irow<cnt;irow++)
		lc.SetItemState(irow,LVIS_SELECTED,LVIS_SELECTED);
}

void CListHelper::deselectAll()
{
	CListCtrl& lc = GetListCtrl();
	//POSITION pos = lc.GetFirstSelectedItemPosition();
	int row;
	for(row=-1;(row=lc.GetNextItem(row,LVNI_SELECTED))>=0;)
	{
		lc.SetItemState(row,0,LVIS_SELECTED);
	}
}

void CListHelper::OnItemChanged(NMHDR* pNMHDR,GridDataSource& _data)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(m_bSaveGridState)
	{
//		TRACE("NM_LISTVIEW = [iItem=%d, uChanged=%08x, uOldState=%08x uNewState=%08x]\n"
//			,pNMListView->iItem,pNMListView->uChanged
//			,pNMListView->uOldState,pNMListView->uNewState
//			);
		int rowcnt = get_Rows(_data);
		if(EQL(0,(pNMListView->uChanged&LVIF_STATE))
			|| pNMListView->iItem<0 
			|| pNMListView->iItem>=rowcnt
			/*|| EQL(pNMListView->uChanged&0xff,0)*/
			) return;
		int rowi = get_SourceRow(pNMListView->iItem);
		Utils::ItemID* _id;
		_data.GetRowID(rowi,_id);
		if(IS_NULL(_id)) return;
		Utils::IDWithData<RowState>* prowstate = m_idhelper.get(_id);
		if(IS_NULL(prowstate) || !(bool)*prowstate) return;
		prowstate->data.state &= ~(pNMListView->uNewState^pNMListView->uOldState);
		prowstate->data.state |= pNMListView->uNewState;
		if(NEQL(0,pNMListView->uNewState&LVIS_FOCUSED))
		{
			m_lastva.m_focusrowid = _id;
		}
	}
	CListCtrl& lc = GetListCtrl();
//	if(NEQL(prowstate->data.state,pNMListView->uOldState))
//	{
//		TRACE("----------- data state = %08x\n",prowstate->data.state);
//		//ASSERT(EQL(prowstate->data.state,pNMListView->uOldState));
//	}
//	prowstate->data.state = pNMListView->uNewState;
	if(NEQL(0,pNMListView->uNewState&LVIS_FOCUSED))
	{
		m_lastva.m_focusedrow = pNMListView->iItem;
	}
}

BOOL CListHelper::OnRowLostFocus(NM_LISTVIEW* pNMListView,int& row,GridDataSource& _data)
{
	row = DEFOCUSED;
	if(EQL(0,pNMListView->uChanged&LVIF_STATE)
		|| EQL(0,(pNMListView->uNewState^pNMListView->uOldState)&LVIS_FOCUSED)
		|| NEQL(0,pNMListView->uNewState&LVIS_FOCUSED)
		) return FALSE;
	if(!m_bSaveGridState) return TRUE;
	row = m_lastva.m_focusedrow;
	int rowcnt = get_Rows(_data);
	if(row<0 || row>=rowcnt) {row=DEFOCUSED;return TRUE;}
	int rowi = get_SourceRow(row);
	Utils::ItemID* _id;
	_data.GetRowID(rowi,_id);
	if(IS_NULL(_id)) return TRUE;
	Utils::IDWithData<RowState>* prowstate = m_idhelper.get(_id);
	if(IS_NULL(prowstate) || !(bool)*prowstate) return TRUE;
	CListCtrl& lc = GetListCtrl();
	if(pNMListView->iItem<0) prowstate->data.state &= ~LVIS_FOCUSED;
	return TRUE;
}

void CListHelper::enable_MultiSelect(bool _en/*=true*/)
{
	CListCtrl& lc = GetListCtrl();
	lc.ModifyStyle(_en?LVS_SINGLESEL:0,_en?0:LVS_SINGLESEL
		,SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
	if(!_en) deselectAll();
}

void CListHelper::enable_ShowFocusAsSelection(bool _en/*=true*/)
{
	m_bShowFocusAsSelection = _en;
	RedrawList();
}

void CListHelper::enable_CellEdit(bool _en/*=true*/,GridDataSetter* _psetter/*=NULL*/)
{
	CListCtrl& lc = GetListCtrl();
	lc.ModifyStyle(_en?0:LVS_EDITLABELS,_en?LVS_EDITLABELS:0
		,SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
	if(!_en && IsControlActive())
		EndLabelEdit((LPCTSTR)TRUE,GetEditCtrl0(),_psetter);
}

/*
void CListHelper::reset_ID(Utils::ItemID*& _id)
{
	m_idhelper.reset_ID(_id);
}*/

void CListHelper::traceStates()
{
	m_idhelper.traceIDList();
}

//template<>
//void Utils::IDHelper<RowState>::traceIDList()
//{
//	IDLst::iterator it,ite;
//	it = m_idlst.begin();
//	ite = m_idlst.end();
//	for(;it!=ite;++it)
//	{
//		Utils::IDWithData<RowState>& idwd = *it;
//		idwd.data.trace();TRACE("\n");
//	}
//}

void CListHelper::OnEndtrack(NMHDR* pNMHDR, LRESULT* pResult,ColumnParamsLst& _cpl,GridDataSource& _data,CDC* _pDC/*=NULL*/)
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;

	CListCtrl& lc = GetListCtrl();
	bool bStateImages = NOT_NULL(lc.GetImageList(LVSIL_STATE));
	int col = phdn->iItem;
	if(bStateImages) col--;
	if(col<0 || col>=(int)_cpl.size() 
		|| EQL(0,phdn->pitem->mask&HDI_WIDTH)) return;
	ColumnParamsLst::iterator it = _cpl.begin();
	std::advance(it,col);
	it->set_Width(phdn->pitem->cxy);
	set_Tooltips(_data,_pDC,&_cpl);
	This()->InvalidateRect(NULL);
	This()->UpdateWindow();
}

BOOL CListHelper::EnsureVisible(int _row,BOOL _bPartialOK)
{
	m_bWillScroll = false;
	CListCtrl& lc = GetListCtrl();
	int topi = lc.GetTopIndex();
	int bottomi = lc.GetCountPerPage()+topi-1;
	if(_bPartialOK) bottomi++;
	if(_row>=topi && _row<=bottomi) return TRUE;
	int oscrolly = lc.GetScrollPos(SB_VERT);
	//int scrollx = lc.GetScrollPos(SB_HORZ);
	BOOL ret;
	UINT scrolly=0;
	if(_row<topi)
	{ 
		ret = _row>=0;
		_row = max(0,_row);
		scrolly += _row-topi;
	}
	else if(_row>bottomi)
	{
		ret = _row<lc.GetItemCount();
		_row = min(_row,lc.GetItemCount()-1);
		scrolly += _row-bottomi;
	}
	CRect rcitem;
	lc.GetItemRect(topi,&rcitem,LVIR_BOUNDS);
	scrolly *= rcitem.Height();
	lc.Scroll(CSize(0,scrolly));
	m_bWillScroll = true;
	return ret;
}

void CListHelper::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar,GridDataSource& _data,GridDataSetter* _griddatasetter,CDC* _pDC/*=NULL*/,ColumnParamsLst* _columnparams/*=NULL*/)
{
	CListCtrl& lc = GetListCtrl();
	if(pScrollBar!=lc.GetScrollBarCtrl(SB_VERT)) return;
	if(IS_NULL(_griddatasetter)) return;
	OnEndlabelEdit((LPCTSTR)TRUE,*_griddatasetter);
	set_Tooltips(_data,_pDC,_columnparams);
}

/*
void CListHelper::PostVScroll(GridDataSource& _data)
{
	if(m_postscrollmsg.m_type==PostScrollMessage::T_BEGINEDIT)
	{
		CListCtrl& lc = GetListCtrl();
		BeginLabelEdit(m_postscrollmsg.m_row,m_postscrollmsg.m_col
			,m_editctrl.GetWnd(&lc,ID_HELPER_EDITCTRL),_data);
		m_postscrollmsg.clear();
	}
}
*/
void CListHelper::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar,GridDataSource& _data,GridDataSetter* _griddatasetter,CDC* _pDC/*=NULL*/,ColumnParamsLst* _columnparams/*=NULL*/)
{
	CListCtrl& lc = GetListCtrl();
	if(pScrollBar!=lc.GetScrollBarCtrl(SB_HORZ)) return;
	if(IS_NULL(_griddatasetter)) return;
	OnEndlabelEdit((LPCTSTR)TRUE,*_griddatasetter);
	set_Tooltips(_data,_pDC,_columnparams);
} 

bool CListHelper::can_Edit(int _row,int _col, GridCellInformer* _gci)
{
	if(IS_NULL(_gci)) return false;
	GridCellInfoTag gcit;
	int irow,icol;
	irow = get_SourceRow(_row);
	icol = get_SourceColumn(_col);
	_gci->GetCellInfo(irow,icol,gcit);
	return gcit.m_beditable;
}

bool CListHelper::get_EditPos(int& _irow,int& _icol)
{
	if(SPACE_ROW==m_edrow) return false;
	_irow = get_SourceRow(m_edrow);
	_icol = get_SourceColumn(m_edcol);
	return true;
}

void CListHelper::next_Item(GridDataSource& _data,GridDataSetter* _psetter,GridCellInformer* _pgsi/*=NULL*/)
{
	CListCtrl& lc = GetListCtrl();
	int row = get_FocusedRow(true),col=get_FocusedColumn(true);
	bool beditmode = IsControlActive();
	if(beditmode)
	{
		ASSERT(EQL(row,m_edrow));
		row = m_edrow;
	}
	bool blastedline = (m_bAddNewLine && row==lc.GetItemCount()-1);
	int maxcol = blastedline?0:_data.GetCols()-1;
	if(m_bCellFocused)
	{
		if(col<0) col=0; 
		else if(col<maxcol || (beditmode && blastedline))
			col++;
		else
		{
			col = 0;
			if(row<lc.GetItemCount()-1) row++;
			else row = 0;
		}
		if(!can_Focused(row,col)) return;
		if(beditmode && _psetter && can_Edit(row,col,_pgsi)) 
		{
			set_Focused(row,col);
			int irow,icol;
			irow = get_SourceRow(row);
			icol = get_SourceColumn(col);
			BeginLabelEdit(irow,icol,_data,*_psetter,_pgsi);
		}
		else 
		{
			if(beditmode) 
				EndLabelEdit((LPCTSTR)TRUE,GetEditCtrl0(),_psetter);
			set_Focused(row,col,true);
		}
	}
	else
	{
		if(row<lc.GetItemCount()-1) ++row;
		else row = 0;
		set_Focused(row,SPACE_ROW,true);
	}
}

void CListHelper::prev_Item(GridDataSource& _data,GridDataSetter* _psetter,GridCellInformer* _pgsi/*=NULL*/)
{
	CListCtrl& lc = GetListCtrl();
	int row = get_FocusedRow(true),col=get_FocusedColumn(true);
	bool beditmode = IsControlActive();
	if(beditmode)
	{
		ASSERT(EQL(row,m_edrow));
		row = m_edrow;
	}
	bool blastedline = (m_bAddNewLine && row==lc.GetItemCount()-1);
	int maxcol = blastedline?0:_data.GetCols()-1;
	if(m_bCellFocused)
	{
		if(col<0) col = maxcol;
		else if(col>0)
			col--;
		else
		{
			if(row>0) {row--;col = _data.GetCols()-1;}
			else {row = lc.GetItemCount()-1;col = m_bAddNewLine?0:_data.GetCols()-1;}
		}
		if(!can_Focused(row,col)) return;
		if(beditmode && _psetter && can_Edit(row,col,_pgsi)) 
		{
			set_Focused(row,col);
			int irow,icol;
			irow = get_SourceRow(row);
			icol = get_SourceColumn(col);
			BeginLabelEdit(irow,icol,_data,*_psetter,_pgsi);
		}
		else 
		{
			if(beditmode)
				EndLabelEdit((LPCTSTR)TRUE,GetEditCtrl0(),_psetter);
			set_Focused(row,col,true);
		}
	}
	else
	{
		if(row>0)--row;
		else row = lc.GetItemCount()-1;
		set_Focused(row,SPACE_ROW,true);
	}
}

int CListHelper::get_Rows(GridDataSource& _data)
{
	if(!m_bsorted || IS_NULL(m_pdatasort)) 
		return _data.GetRows() + (m_bAddNewLine?1:0);
	else
		return (int)m_sortedrows.size() + (m_bAddNewLine?1:0);
}

bool CListHelper::get_SortColumn(int& _col,int& _sortnum)
{
	if(IS_NULL(m_pdatasort) || !m_bsorted) return false;
	_col = m_sortcolumn;
	_sortnum = m_sortnum;
	return true;
}

void CListHelper::Attach(CWnd* _pwnd) 
{
	m_pwnd=_pwnd;
	ASSERT(_pwnd);
}

void CListHelper::Attach(CListCtrl* _plistctrl) 
{
	m_plistctrl=_plistctrl;
	ASSERT(m_plistctrl);
	if(NOT_NULL(m_tooltipctrl.GetSafeHwnd()))
		m_tooltipctrl.DestroyWindow();
	if(IS_NULL(m_plistctrl)) return;
	m_tooltipctrl.Create(_plistctrl,TTS_ALWAYSTIP);
}

void CListHelper::enable_ToolTips(bool _en/*=true*/)
{
	m_bShowTooltips = _en;
	if(!IsWindow(m_tooltipctrl.GetSafeHwnd())) return;
	m_tooltipctrl.Activate(m_bShowTooltips);
}

void CListHelper::set_Tooltips(GridDataSource& _data,CDC* _pDC/*=NULL*/,ColumnParamsLst* _columnparams/*=NULL*/)
{
	if(!m_bShowTooltips || !IsWindow(m_tooltipctrl)
		|| IS_NULL(m_plistctrl)) return;
	//clear tooltip ctrl
	CListCtrl& lc = GetListCtrl();
	UINT i,cnt;
	m_tooltipctrl.Activate(FALSE);
	cnt = m_tooltipctrl.GetToolCount();
	for(i=0;i<cnt;i++)
	{
		CToolInfo ti;
		if(!m_tooltipctrl.GetToolInfo(ti,&lc,i+1)) continue;
		m_tooltipctrl.DelTool(&lc,i+1);
	}
	//add new list items;
	int topi,bottomi,row;
	topi = lc.GetTopIndex();
	bottomi = topi + lc.GetCountPerPage()+1;
	bottomi = min(lc.GetItemCount()-1,bottomi);
	CRect rcitem;
	lc.GetItemRect(topi,&rcitem,LVIR_LABEL);
	int scrollx = lc.GetScrollPos(SB_HORZ);
	vector<int> columns;
	int col;
	LVCOLUMN lvc;
	lvc.mask = LVCF_WIDTH;
	int stateoffs = rcitem.left;
	rcitem.right = rcitem.left - scrollx;
	int coli = 0;
	bool bStateImages = NOT_NULL(lc.GetImageList(LVSIL_STATE));
	if(bStateImages) 
	{
		lc.GetItemRect(topi,&rcitem,LVIR_BOUNDS);
		lc.GetColumn(coli++,&lvc);
		rcitem.right = (rcitem.left+=lvc.cx-scrollx);
	}
	columns.push_back(rcitem.right);
	for(;lc.GetColumn(coli,&lvc);coli++)
	{
		rcitem.left	= rcitem.right;
		rcitem.right += lvc.cx -((0==coli)?stateoffs:0);
		columns.push_back(rcitem.right);
	}
	ASSERT(columns.size()-1==_data.GetCols());
	UINT id;
	CRect crc;
	lc.GetClientRect(crc);
	for(id=1,row=topi;row<=bottomi;row++)
	{
		lc.GetItemRect(row,&rcitem,LVIR_LABEL);
		for(col=1;col<(int)columns.size();col++,id++)
		{
			int irow,icol;
			irow = get_SourceRow(row);
			icol = get_SourceColumn(col-1);
			CString str;
			bool bshowalways;
			if(!_data.GetDescription(irow,icol,str,bshowalways)) continue;
			if(!bshowalways && NOT_NULL(_columnparams))
			{
				ColumnParams* pcolpar = getColumn(*_columnparams,icol);
				if(NOT_NULL(pcolpar) && (bool)*pcolpar)
				{
					CString celltext;
					_data.GetString(irow,icol,celltext);
					CSize txtsz = _pDC->GetTextExtent(celltext);
					if(txtsz.cx<pcolpar->get_Width()) continue;
				}
			}
			CRect rc(max(columns[col-1],crc.left),rcitem.top,min(columns[col],crc.right),rcitem.bottom);
			if(rc.left>=rc.right) continue;
			m_tooltipctrl.AddTool(&lc,LPSTR_TEXTCALLBACK,&rc,id);
		}
	}
	m_tooltipctrl.Activate(TRUE);
}

bool CListHelper::get_Tooltip(UINT _id,GridDataSource& _data,CString& _str)
{
	--_id;
	int row,col;
	row = _id/_data.GetCols();
	col = _id%_data.GetCols();
	int irow,icol;
	CListCtrl& lc = GetListCtrl();
	int topi = lc.GetTopIndex();
	row += topi;
	irow = get_SourceRow(row);
	icol = get_SourceColumn(col);
	bool bshowallways;
	return _data.GetDescription(irow,icol,_str,bshowallways) && bshowallways;
}

void CListHelper::OnTrack(NMHDR* pNMHDR, LRESULT* pResult)
{
	if(IS_NULL(pResult)) return;
	*pResult = FALSE;
	LPNMHEADER pNMHeader = (LPNMHEADER)pNMHDR;
	CListCtrl& lc = GetListCtrl();
	CImageList* pStateImages = lc.GetImageList(LVSIL_STATE);
	if(IS_NULL(pStateImages) || pNMHeader->iItem!=0
		|| IS_NULL(pNMHeader->pitem)
		|| EQL(0,pNMHeader->pitem->fmt&HDI_WIDTH)) return;
	IMAGEINFO ii;
	pStateImages->GetImageInfo(0,&ii);
	int w = CRect(ii.rcImage).Width()+2;
	*pResult = w<pNMHeader->pitem->cxy;
}