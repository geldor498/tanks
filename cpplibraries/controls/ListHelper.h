// listhelper.h: interface for the listhelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTHELPER_H__7D1D05B2_CD41_4D71_877F_19E8814F3397__INCLUDED_)
#define AFX_LISTHELPER_H__7D1D05B2_CD41_4D71_877F_19E8814F3397__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../utils/utils.h"
#include "idhelper.h"
#include "../utils/serialize.h"

#define INT_NULL (-1)
#define INT_UNDEFINED (-2)

template<typename _II>
inline 
bool find_iterator(_II itb,_II ite,const _II it)
{
	for(;it!=itb && itb!=ite;++itb){}
	return it==itb;
}

template<class _Ty>
class ListItem
{
protected:
	std::list<_Ty>& lst;
	typename std::list<_Ty>::iterator it;
public:
	ListItem(std::list<_Ty>& _lst,typename std::list<_Ty>::iterator _it):lst(_lst),it(_it){}
	ListItem(std::list<_Ty>& _lst,_Ty* pdta):lst(_lst)
	{
		if(pdta==NULL) it = lst.end();
		else it = find(lst.begin(),lst.end(),pdta);
	}
	ListItem(std::list<_Ty>& _lst,int _il):lst(_lst)
	{
		if(_il<0 || _il>=_lst.size()) 
		{
			it = _lst.end();
			return;
		}
		it = _lst.begin();
		advance(it,_il);
	}

	operator bool() const
	{
		return it!=lst.end();
	}

	void splice(std::list<_Ty>& _lst,typename std::list<_Ty>::iterator it1,typename std::list<_Ty>::iterator it2)
	{
		if(&lst!=&_lst || &lst==&_lst && !find_iterator(it1,it2,it))
			lst.splice(it,_lst,it1,it2);
	}

	void splice(ListItem<_Ty>& _li)
	{
		if(_li.it==_li.lst.end()) return;
		std::list<_Ty>::iterator ite = _li.it;
		++ite;
		if(&lst!=&_li.lst || &lst==&_li.lst && !find_iterator(_li.it,ite,it))
			lst.splice(it,_li.lst,_li.it,ite);
	}

	typename std::list<_Ty>::iterator get_iterator() {return it;}

	template<typename _Ty> friend class HeapVariable;
};

template<class _Ty>
class HeapVariable
{
protected:
	std::list<_Ty> var;
	void _Allocate()
	{if(*this) return;var.insert(var.end());}

public:
	void copyto(ListItem<_Ty>& _li)
	{
		_li.splice(var,var.begin(),var.end());
	}

	HeapVariable<_Ty>& operator=(ListItem<_Ty>& _li)
	{
		if(&_li.lst==&var) return *this;
		clear();
		if(!_li) return *this;
		std::list<_Ty>::iterator ite;
		++(ite = _li.it); 
		var.splice(var.end(),_li.lst,_li.it,ite);
		return *this;
	}
	HeapVariable<_Ty>& operator=(HeapVariable<_Ty>& _ti)
	{
		if(&_ti==this) return *this;
		clear();
		if(!_ti) return *this;
		var.splice(var.end(),_ti.var.begin(),_ti.var.end());
	}

	operator _Ty&() {if(!*this) _Allocate();return var.back();}
	_Ty& operator->() {if(!*this) _Allocate(); return var.back();}
	void clear() {var.clear();}
	void allocate() {_Allocate();}
	operator bool() const {return var.size()>0;}
	_Ty& get() {return (_Ty&)*this;}
};

template<class _Ty>
class FindInList
{
protected:
	const std::list<_Ty>& lst;
	bool bf;
	bool validate;
public:
	FindInList(const std::list<_Ty>& _lst,bool _validate=true)
		:lst(_lst),bf(false),validate(_validate)
	{}
	int operator()(const _Ty* _ptr)
	{
		if(!_ptr) {bf=true;return INT_NULL;}
		std::list<_Ty>::const_iterator it;
		it = find(lst.begin(),lst.end(),_ptr);
		if(validate) ASSERT(it!=lst.end());
		if(it==lst.end()) return INT_NULL;
		bf=true;
		return distance(lst.begin(),it);
	}

	const _Ty* operator()(int _ti)
	{
		if(_ti==INT_NULL) {bf=true;return NULL;}
		if(validate) ASSERT(_ti>=0 && _ti<lst.size());
		if(_ti<0 || _ti>=lst.size()) return NULL;
		std::list<_Ty>::const_iterator it = lst.begin();
		advance(it,_ti);
		bf=true;
		return &*it;
	}
	operator bool() const {return bf;}
};

template<class GdiObj>
class AutoGdiObject
{
protected:
	GdiObj* m_poldobj;
	CDC* m_pdc;
public:
	AutoGdiObject(CDC* _pdc,GdiObj& _gdiobj)
	{
		m_pdc = _pdc;
		m_poldobj = m_pdc->SelectObject(&_gdiobj);
	}

	AutoGdiObject(CDC* _pdc,GdiObj* _pgdiobj)
	{
		m_pdc = _pdc;
		if(NOT_NULL(_pgdiobj))	
			m_poldobj = m_pdc->SelectObject(_pgdiobj);
		else m_poldobj = NULL;
	}

	~AutoGdiObject()
	{
		if(NOT_NULL(m_poldobj))
			m_pdc->SelectObject(m_poldobj);
	}
};

class CEditCont: public CEdit
{
	DECLARE_DYNCREATE(CEditCont)
protected:
	CRichEditCtrl* m_pouteredit;
public:
	CEdit* GetWnd(CWnd* pParent,UINT ctrlID);
	~CEditCont();
	void set_OuterEdit(CRichEditCtrl* _pouteredit)
	{
		m_pouteredit = _pouteredit;
	}
	//{{AFX_VIRTUAL(CEditCont)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	//{{AFX_MSG(CEditCont)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg void OnTextChanged();
	DECLARE_MESSAGE_MAP()
};

class CComboCont: public CComboBox
{
	DECLARE_DYNCREATE(CComboCont);
public:
	CComboCont* GetWnd(CWnd* pParent,UINT ctrlID);
	~CComboCont();
	
	//{{AFX_VIRTUAL(CEditCont)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	//{{AFX_MSG(CEditCont)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

struct ColumnInfoTag
{
	enum ColumnTypeEn{CT_TEXT,CT_IMAGE,CT_COMBOBOX};
	ColumnTypeEn    m_type;
	bool m_bjoin;
	
	ColumnInfoTag():m_type(CT_TEXT),m_bjoin(false){}
};

struct GridCellInfoTag
{
	bool m_beditable;
	bool m_bredirectalledit;
	bool m_bmultiline;
	CRichEditCtrl* m_outereditor;
	GridCellInfoTag():m_beditable(false)
		,m_bredirectalledit(false)
		,m_bmultiline(false)
		,m_outereditor(NULL)
	{}
};

enum ColorsSNS{C_NotSelected=0,C_Selected=1,C_DropHilited=2,ColorsSNS_MAX=3};
enum ColorsBF{C_Background=0,C_Foreground=1,ColorsBF_MAX=2};
enum DrawModeEn{TDM_NORMAL,TDM_DISABLED};

interface GridDataSource
{
	virtual void GetString(int _row,int _col,CString& _str) = 0;
	virtual void GetImage(int _row,int _col,int& _nImage) = 0;
	virtual void GetColumnHeader(int _col,CString& _str,int& _fmt) = 0;
	virtual int GetRows() = 0;
	virtual int GetCols() = 0;
	virtual void GetColumnInfo(int _rowi,int _coli,ColumnInfoTag& cit) = 0;
	virtual void GetParam(int _row,LPARAM& lParam) = 0;
	virtual void GetImage(int _row,int& nStateImg) = 0;
	virtual bool GetColors(int _row,int _col,COLORREF _clrs[ColorsSNS_MAX][ColorsBF_MAX]) = 0;
	virtual bool GetDrawMode(int _row,DrawModeEn& _drawmode) = 0;
	virtual void GetRowID(int _row,Utils::ItemID*& _id) = 0;
	virtual bool GetDescription(int _row,int _col,CString& _str,bool& _bshowalways) = 0;
};

interface GridDataSetter
{
	virtual void SetString(int _row,int _col,const CString& _str) = 0;
	virtual bool Validate(int _row,int _col,const CString& _nval,CString& errmess) = 0;
};

interface GridCellInformer
{
	virtual void GetCellInfo(int _row,int _col,GridCellInfoTag& gcit) const = 0;
};

#define  ID_HELPER_EDITCTRL				(0x1001)
//#define  IMAGEWIDTH                     16
//#define  IMAGEHEIGHT                    16
class CListHelper;

struct VisibleArea
{
protected:
	int top,bottom;
	bool bxscroll;
	int xscroll;

	Utils::ItemID* m_focusrowid;
	int m_focusedrow;
	int m_yscroll;
public:
	VisibleArea() :m_focusrowid(NULL),m_focusedrow(-1){}

	bool operator==(const VisibleArea& va)
	{
		return top==va.top && bottom==va.bottom;
	}

	friend class CListHelper;
};

struct ColumnParams
{
protected:
	bool m_binitialized;
	int m_width;
public:
	ColumnParams():m_binitialized(false),m_width(-1){}

	int get_Width()
	{
		ASSERT((bool)*this);
		return m_width;
	}

	void set_Width(int _width)
	{
		m_binitialized = true;
		m_width = _width;
	}

	operator bool()
	{
		return m_binitialized;
	}

	class DefaultLayout: public Layout<ColumnParams>
	{
	public:
		DefaultLayout()
		{
			add_simple(_T("Initialized"),&ColumnParams::m_binitialized);
			add_simple(_T("width"),&ColumnParams::m_width);
		}
	};
};

typedef std::list<ColumnParams> ColumnParamsLst;

inline
void insertColumn(ColumnParamsLst& _lst,int _col)
{
	ASSERT(_col>=0 && _col<=(int)_lst.size());
	if(_col<0) return;
	_col = min(_col,(int)_lst.size());
	ColumnParamsLst::iterator it = _lst.begin();
	std::advance(it,_col);
	_lst.insert(it,ColumnParams());
}

inline
void removeColumn(ColumnParamsLst& _lst,int _col)
{
	ASSERT(_col<(int)_lst.size());
	if(_col>=(int)_lst.size()) return;
	_col = max(0,_col);
	ColumnParamsLst::iterator it = _lst.begin();
	std::advance(it,_col);
	_lst.erase(it);
}

inline 
ColumnParams* getColumn(ColumnParamsLst& _lst,int _col)
{
	//ASSERT(_col>=0 && _col<_lst.size());
	if(_col<0 || _col>=(int)_lst.size()) return NULL;
	ColumnParamsLst::iterator it = _lst.begin();
	std::advance(it,_col);
	return &*it;
}

inline
ListItem<ColumnParams> getListItem(ColumnParamsLst& _lst,int _col)
{
	ASSERT(_col>=0 && _col<=(int)_lst.size());
	if(_col<0 || _col>(int)_lst.size()) return ListItem<ColumnParams>(_lst,_lst.end());
	ColumnParamsLst::iterator it = _lst.begin();
	std::advance(it,_col);
	return ListItem<ColumnParams>(_lst,it);
}

#define FULL_ROW                     (-1)
#define STATE_COLUMN                 (-2)
#define HEADER_ROW                   (-2)
#define SPACE_ROW                    (-3)
#define NOSELECTION                  (-1)
#define DEFOCUSED                    (-1)

typedef std::vector<int> SortedRowsLst;
typedef std::vector<int> SortImagesLst;

interface IListSort
{
	virtual void sort(int _col,int _sortnum,SortedRowsLst& _sortedrows,SortImagesLst& _sortimages) = 0;
};

struct RowState
{
	DWORD state;
	int irow;

	RowState():state(0),irow(-1){}
	void trace() {TRACE("irow=%d,  state=%08x",irow,state);}
};

interface IItemSelect
{
	virtual void focused_ListItem(int _row,int _col) = 0;
	virtual bool can_SelectListItem(int _row,int _col) = 0;
	virtual bool can_FocusedListItem(int _orow,int _ocol,int _nrow,int _ncol) = 0;
	virtual void edit_ListItem(int _row,int _col,bool _bActivateEdit) = 0;
};

enum ListDrawModeEn{LDM_STANDARD,LDM_DELPHIGRIG};

class CListHelper
{
	class ColorSheme
	{
		COLORREF colors[3][2];
		//Utils::AutoGdiObject<CPen>* m_ppen;
		AutoGdiObject<CBrush>* m_pbrush;
		//CPen m_pen;
		CBrush m_brush;
		CListHelper& m_listhelper;
		COLORREF m_oTxtClr;
		COLORREF m_oBkgClr;
		GridDataSource& m_data;
		CDC* m_pDC;
		ColorsSNS m_icolors;
	public:	
		ColorSheme(CListHelper& _listhelper,CDC* pDC,GridDataSource& _data
			,int _row,int _col,BOOL _bFocus,BOOL _bSelect,BOOL _bHilited
			,ListDrawModeEn _drawmode);
		~ColorSheme();
		ColorsSNS get_Colors() const {return m_icolors;}
		COLORREF operator[](ColorsBF _cbf) const {return colors[get_Colors()][_cbf];}
	};
	friend class ColorSheme;
/*	struct PostScrollMessage
	{
		enum TypeEn{T_NOTHING,T_BEGINEDIT};
		TypeEn m_type;
		int m_row,m_col;
		PostScrollMessage():m_type(T_NOTHING){}
		void clear(){m_type=T_NOTHING;}
		void set_Pos(int _row,int _col)
		{
			m_row = _row;
			m_col = _col;
		}
	};
*/
protected:
	bool can_Edit(int _row,int _col,GridCellInformer* _gci);

	void BeginLabelEdit(int _row,int _col,GridDataSource& _data,GridCellInformer* _pgci);
	void BeginLabelProcess(int _row,int _col,CWnd* pwnd,GridDataSource& _data);
	void EndLabelEdit(LPCTSTR sztxt,CWnd *pwnd,GridDataSetter* _psetter);

	CWnd*           m_pwnd;
	CListCtrl*      m_plistctrl;

	BOOL m_bFullRowSel;
	BOOL m_bClientWidthSel;

	void RepaintSelectedItems();
	int             m_cxClient;
	int             m_cxStateImageOffset;
	//int             m_noffsheader;
	//COLORREF        m_clrText;
	//COLORREF        m_clrTextBk;
	//COLORREF        m_clrBkgnd;
	COLORREF        m_clrCursor;
	int             m_edrow,
		            m_edcol;
	CEditCont       m_editctrl;
	CPen            m_blkpen;
	CPen            m_gridpen;
	bool            m_bAddNewLine;
	bool            m_bCellFocused;
	int             m_nFocusedColumn;

	bool            m_bShowNoItemMessage;
	CString         m_sNoItemMessage;

	IListSort*      m_pdatasort;
	int             m_sortcolumn;
	int             m_sortnum;
	bool            m_bsorted;
	bool            m_bresorted;
	SortedRowsLst   m_sortedrows;
	SortImagesLst   m_sortimages;

	VisibleArea     m_lastva;
	BOOL            m_bSaveGridState;
	Utils::IDHelper<RowState> m_idhelper;

	bool            m_bShowFocusAsSelection;
	bool            m_bWillScroll;
	//PostScrollMessage m_postscrollmsg;

	IItemSelect*    m_pitemselect;

	void resort(int _col,int _sortnum,GridDataSource& _data);
	int get_Row(int _row,int _rowcnt);
	int get_Rows(GridDataSource& _data);
	void SetHeaderSortImages();
	int find_RowbyID(GridDataSource* _pdata,Utils::ItemID* _rowid);
	CEdit* GetEditCtrl0() {return m_editctrl.GetWnd(&GetListCtrl(),ID_HELPER_EDITCTRL);}
	void set_Tooltips(GridDataSource& _data,CDC* _pDC,ColumnParamsLst* _columnparams=NULL);

	CToolTipCtrl    m_tooltipctrl;
	bool            m_bShowTooltips;

	ListDrawModeEn  m_drawmode;
	CPen            m_hlinepen;
	CPen            m_btnshadowpen;
	CPen            m_btnhighlightpen;
	//std::vector<UINT> m_states;
public:

	void reset_Headers(GridDataSource& _data);
	int get_SourceRow(int _lstrow);
	int get_SourceColumn(int _col);
	void BeginLabelEdit(int _irow,int _icol,GridDataSource& _data,GridDataSetter& _setter,GridCellInformer* _gridinfo,bool _sellectall=false);
	void SetGridData(GridDataSource& _data,CDC* _pDC=NULL,ColumnParamsLst* _columnparams=NULL);
	void InitColumns(GridDataSource& _data,CDC* pDC,ColumnParamsLst* _columnparams=NULL);
	void Attach(CWnd* _pwnd);
	void Attach(CListCtrl* _plistctrl);
	void set_AddNewLineMode(bool enable=true) {m_bAddNewLine=enable;}
	void set_CellSelectMode(bool enable=true) {if(!(m_bCellFocused=enable)){m_nFocusedColumn=-1;RedrawList();}}
	void set_SortedMode(bool _enable=true,IListSort* _plistsort=NULL);
	void enable_ShowNoItemMessage(bool _enable=true) {m_bShowNoItemMessage = _enable;}
	void set_NoItemMessage(const CString& _MsgId) {m_sNoItemMessage = _MsgId;}
	void enable_SaveGridState(bool _en=true) {m_bSaveGridState=_en;}
	void enable_MultiSelect(bool _en=true);
	void enable_ShowFocusAsSelection(bool _en=true);
	void enable_ToolTips(bool _en=true);
	void enable_CellEdit(bool _en=true,GridDataSetter* _psetter=NULL);
	void set_ListItemSelectNotify(IItemSelect* _pitemselect) {m_pitemselect = _pitemselect;}
	void set_DrawMode(ListDrawModeEn  _drawmode);

	CListCtrl& GetListCtrl() {return *m_plistctrl;}
	CWnd* This() {return m_pwnd;}
	BOOL SetFullRowSel(BOOL bFullRowSel=TRUE);
	BOOL GetFullRowSel() {return m_bFullRowSel;}
	CEdit* GetEditCtrl() {return IsControlActive()?GetEditCtrl0():NULL;}

	CListHelper();
	virtual ~CListHelper();

	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct,GridDataSource& _data);
	void PreCreateWindow(CREATESTRUCT& cs);
	void RedrawList();
	void redraw_EditRow();
	void InitGrid(GridDataSource& _data,CDC* pDC,ColumnParamsLst* _columnparams=NULL);

	BOOL OnSetImageList(WPARAM wParam, LPARAM lParam);
	BOOL OnSetTextColor(WPARAM wParam, LPARAM lParam);
	BOOL OnSetTextBkColor(WPARAM wParam, LPARAM lParam);
	BOOL OnSetBkColor(WPARAM wParam, LPARAM lParam);
	void OnSize(UINT nType, int cx, int cy);
	void OnPaint();
	void OnSetFocus(CWnd* pOldWnd);
	void OnKillFocus(CWnd* pNewWnd);

	BOOL OnLButtonDown(UINT nFlags, CPoint point,GridDataSource& _data,GridDataSetter* _spetter=NULL,GridCellInformer* _gridinfo=NULL);
	BOOL OnLButtonDblClk(UINT nFlags, CPoint point,GridDataSource& _data,GridDataSetter* _psetter=NULL,GridCellInformer* _gridinfo=NULL);
	void OnEndlabelEdit(LPCTSTR szText,GridDataSetter& _setter);
	BOOL PreTranslateMessage(MSG* pMsg);
	void OnItemChanged(NMHDR* pNMHDR,GridDataSource& _data);
	BOOL OnRowLostFocus(NM_LISTVIEW* pNMListView,int& row,GridDataSource& _data);

	void get_Visible(VisibleArea& _va,bool _bxscroll=false);
	void set_Visible(const VisibleArea& _va,GridDataSource* _pdata=NULL);
	BOOL OnEraseBkgnd(CDC* pDC);
	int  get_FocusedRow(bool _blistrow);
	int  get_FocusedColumn(bool _blistcol);
	void EnableWindow(BOOL bEnable);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags,GridDataSource& _data,GridDataSetter* _psetter,GridCellInformer* _pgsi=NULL);
	bool IsControlActive();
	int  get_SelectedCount();
	bool IsAnySelected();
	void selectAll();
	void deselectAll();
	BOOL EnsureVisible(int _row,BOOL _bPartialOK);
	void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar,GridDataSource& _data,GridDataSetter* _griddatasetter,CDC* _pDC=NULL,ColumnParamsLst* _columnparams=NULL);
	void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar,GridDataSource& _data,GridDataSetter* _griddatasetter,CDC* _pDC=NULL,ColumnParamsLst* _columnparams=NULL); 
	void OnTrack(NMHDR* pNMHDR, LRESULT* pResult); 

	//void AdjustHeader();
	//void PostVScroll(GridDataSource& _data);
//	void reset_ID(Utils::ItemID*& _id);
	void OnEndtrack(NMHDR* pNMHDR, LRESULT* pResult,ColumnParamsLst& _cpl,GridDataSource& _data,CDC* _pDC=NULL);

	bool get_Tooltip(UINT _id,GridDataSource& _data,CString& _str);
	bool get_SortColumn(int& _col,int& _sortnum);
	// _row and _col in data grid coordinates not in a list position as it sorted;
	void toggle_Selection(int _row);
	void SetColumnText(int _col,const CString& _txt);
	void select_Row(int _row,bool _addtoselection=false,bool _ensurevisible=false);
	void ensureVisible(int _row,int _col=SPACE_ROW);
	void restore_LastFocused(bool _ensurevisible=true);
	bool set_Focused(int _row,int _col=SPACE_ROW,bool _ensurevisible=true);
	bool set_FocusedData(int _row,int _col=SPACE_ROW,bool _ensurevisible=true);
	void OnHeaderClicked(int _col,GridDataSource& _data,GridDataSetter* _psetter,CDC* _pDC=NULL,ColumnParamsLst* _columnparams=NULL);
	bool FindRowCol(const CPoint& pt,int& _row,int& _col,bool _bheader=false,bool _hstatecol=false,bool _blistpos=true);
	void UpdateListStates(int _row);
	bool get_EditPos(int& _irow,int& _icol);
	void next_Item(GridDataSource& _data,GridDataSetter* _psetter,GridCellInformer* _pgsi=NULL);
	void prev_Item(GridDataSource& _data,GridDataSetter* _psetter,GridCellInformer* _pgsi=NULL);

	bool can_Focused(int _row,int _col,bool _bapllynewpos=false);
	int get_ListRow(int _irow);
	int get_ListColumn(int _icol);

	void traceStates();
};

inline
int CListHelper::get_SourceRow(int _lstrow)
{
	if(!m_bsorted || EQL(0,m_sortedrows.size())) return _lstrow;
	if(_lstrow<0 || _lstrow>=(int)m_sortedrows.size()) return _lstrow;
	return m_sortedrows[_lstrow];
}

inline
int CListHelper::get_ListRow(int _irow)
{
	if(!m_bsorted || EQL(0,m_sortedrows.size())) return _irow;
	SortedRowsLst::iterator it = std::find(m_sortedrows.begin(),m_sortedrows.end(),_irow);
	return (it!=m_sortedrows.end())?(int)std::distance(m_sortedrows.begin(),it):_irow<0?_irow:0;
}

inline
int CListHelper::get_ListColumn(int _icol)
{
	return _icol;
}

#endif // !defined(AFX_LISTHELPER_H__7D1D05B2_CD41_4D71_877F_19E8814F3397__INCLUDED_)
