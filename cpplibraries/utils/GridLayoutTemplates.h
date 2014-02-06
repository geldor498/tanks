
#if !defined(__GRIDLAYOUTTEMPLATES_H__FCD09A8E_4E97_4B71_AB01_7E7335DFC8BA)
#define __GRIDLAYOUTTEMPLATES_H__FCD09A8E_4E97_4B71_AB01_7E7335DFC8BA

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#pragma warning(push)

#pragma warning(disable:4786)

#include "config.h"
#include "atlmfc.h"

#include <list>
#include <vector>
#include <algorithm>
#include <utility>

#include "utils.h"
#include "cp1251.h"
#include "datahelpers.h"
#include "tracealloc.h"
#include "convert.h"


/**\defgroup GridLayoutTemplates Utilities to support creation of grid data layouts.
This classes support cretation of grid data layouts -- meta-information to output some 
data in grids. 
*/

//@{

/**\brief Column align enumeration
This values used in a setting of grid column attributes.
*/
enum GridAlignEn{
	GA_Null = 0					///< don`t care
	,GA_Left					///< left column align
	,GA_Right					///< right column align
	,GA_Center					///< center colun align
};

/**\brief helpfull function for easy dedug
\param[in] _align -- align value to get a string for
\return string represantation of _align value (of type \ref GridAlignEn).
*/
inline 
CString_ to_str(GridAlignEn _align)
{
	static const GridAlignEn al001[] = {GA_Null,GA_Left,GA_Right,GA_Center};
	static const LPCTSTR sz001[] = {_T("GA_Null"),_T("GA_left"),_T("GA_Right"),_T("GA_Center")};
	int i=0;
	for(i=0;i<sizea(al001);i++)
	{
		if(al001[i]==_align) return sz001[i];
	}
	static const LPCTSTR szempty = _T("_unknown_");
	return szempty;
}

/**\brief interface for one item of a grid.
This item represent one grid item data. Or one column in grid.
\tparam _Struct -- type grid data item.
*/
template<typename _Struct>
interface IGridLayoutItem
{
	typedef _Struct Struct;	///< metaprograming typedef. it`s used in derived classes

	virtual ~IGridLayoutItem() {}	///< virtual destructor (for correct destruction of derived classes).

	/**\brief return string representation for this item of grid data
	\param[in] _struct -- a grid data (from some row of a grig)
	\param[out] _str -- string representation for this memeber
	\return true if data was successfuly converted
	*/
	virtual bool get_string(const Struct& _struct,CString_& _str) const = 0;	

	/**\brief compare two items (same members) of grid data
	\param[in] _struct1 -- first data to compare (from some row of grid)
	\param[in] _struct2 -- second data to comapre (from another row of grid)
	\return value < 0 if _struct1 < _struct2;
		    value > 0 if _struct1 > _struct2;
			value == 0 if _struct1 == _struct2;
	*/
	virtual long compare(const Struct& _struct1,const Struct& _struct2) const = 0;

	/**\brief return caption of this column (for this item of grid data)
	\param[out] _caption -- caption of this column
	\return true if column have a caption
	*/
	virtual bool get_caption(CString_& _caption) const = 0;

	/**\brief return size (default width) of this column
	\return default width (size) of this column
	*/
	virtual long get_size() const = 0;

	/**\brief return id of this column
	\return identifier of this column
	*/
	virtual DWORD get_id() const = 0;

	//virtual CColor get_color() const = 0;

	/**\brief return align for this column
	\return alignment for this column
	*/
	virtual GridAlignEn get_align() const = 0;
};//template<>interface IGridLayoutItem

/**\brief Simple implementation of IGridLayoutItem interface.
This class is a simple implementation of grid layout item (represent grid data for one column)
\tparam _Struct -- type of data to hold in a grid
\tparam _Item -- type of one item of data to hold in a grid.
\tparam _DataHelper -- data helper class to support base operations for data in grid
\sa DataHelpers
\sa IGridLayoutItem
*/
template<typename _Struct,typename _Item,typename _DataHelper>
struct CGridLayoutItem 
:
	public IGridLayoutItem<_Struct>
{
	/**\brief Constructor
	Initialize \ref CGridLayoutItem items
	\param _sColumnName -- caption for this column
	\param _pmember -- member pointer of item of data to hold in a grid.
	\param _ndefsize -- default width of this column
	\param _align -- align for this column
	\param _id -- identifier od this column 
	\param _datahelper -- data helper for this column item data operations
	*/
	CGridLayoutItem(const CString_& _sColumnName
		,_Item _Struct::* _pmember
		,long _ndefsize
		,GridAlignEn _align
		,DWORD _id
		,const _DataHelper& _datahelper
		)
		:m_sColumnName(_sColumnName)
		,m_defsize(_ndefsize)
		,m_align(_align)
		,m_pmember(_pmember)
		,m_datahelper(_datahelper)
		,m_id(_id)
	{
	}

	/**\brief implementation for \ref IGridLayoutItem::get_caption(CString_& _caption) const
	\param[out] _caption -- caption of this column
	\return true if column is getted successfuly
	\sa IGridLayoutItem::get_caption(CString_& _caption) const 
	*/
	virtual bool get_caption(CString_& _caption) const {_caption = m_sColumnName;return true;}

	/**\brief return default width (size) of this column
	\return default width (size) of this column
	\sa IGridLayoutItem::get_size() const
	*/
	virtual long get_size() const {return m_defsize;}

	/**\brief return align of this column
	\return this column alignment
	\sa IGridLayoutItem::get_align() const
	*/
	virtual GridAlignEn get_align() const {return m_align;}

	/**\brief return string representation of this data item
	\param[in] _struct -- grid data (some row)
	\param[out] _str -- string representation of this column for grid data _struct
	\return true if data item was successfuly converted to string
	\sa IGridLayoutItem::get_string(const Struct& _struct,CString_& _str) const
	*/
	virtual bool get_string(const Struct& _struct,CString_& _str) const
	{
		return m_datahelper.convert(_struct.*m_pmember,_str);
	}

	/**\brief function to compare two rows of grid
	\param[in] _struct1 -- first grid data (some row) to compare
	\param[in] _struct2 -- second grid data (another row) to comapre
	\return value < 0 if _struct1 < _struct2;
			value > 0 if _struct1 . _struct2;
			value == 0 if _struct1 == _struct2;
	\sa IGridLayoutItem::compare(const Struct& _struct1,const Struct& _struct2) const
	*/
	virtual long compare(const Struct& _struct1,const Struct& _struct2) const
	{
		return m_datahelper.compare(_struct1.*m_pmember,_struct2.*m_pmember);
	}

	/**\brief function to get id of column
	\return column identifier
	\sa IGridLayoutItem::get_id() const
	*/
	virtual DWORD get_id() const {return m_id;}

protected:
	_Item _Struct::* m_pmember;		///< member pointer for grid data item
	const _DataHelper m_datahelper;	///< data helper for this column 
									/**<\sa DataHelpers*/
	CString_ m_sColumnName;			///< column name
	long m_defsize;					///< column default size (width)
	GridAlignEn m_align;			///< column alignment 
									/**<\sa GridAlignEn*/
	DWORD m_id;						///< column identifier
};//template<> struct CGridLayoutItem 

/**\brief implementation of \ref IGridLayoutItem interface
\tparam _Struct -- data struct of layout 
\tparam _Expression -- expression 
\tparam _DataHelper -- data helper
\sa DataHelpers
\sa IGridLayoutItem
*/
template<typename _Struct,typename _Expression,typename _DataHelper>
struct CGridLayoutExpressionItem
:
	public IGridLayoutItem<_Struct>
{
	typedef _Struct Struct;	///< metaprograming typedef. it`s used in derived classes

	/**\brief Constructor
	Initialize \ref CGridLayoutExpressionItem items
	\param _sColumnName -- caption for this column
	\param _expression -- expression that used to form output for this layout item.
	\param _ndefsize -- default width of this column
	\param _align -- align for this column
	\param _id -- identifier od this column 
	\param _datahelper -- data helper for this column item data operations
	*/
	CGridLayoutExpressionItem(const CString_& _sColumnName
		,_Expression _expression
		,long _ndefsize
		,GridAlignEn _align
		,DWORD _id
		,const _DataHelper& _datahelper
		)
		:m_sColumnName(_sColumnName)
		,m_defsize(_ndefsize)
		,m_align(_align)
		,m_expression(_expression)
		,m_datahelper(_datahelper)
		,m_id(_id)
	{
	}

	/**\brief return string representation for this item of grid data
	\param[in] _struct -- a grid data (from some row of a grig)
	\param[out] _str -- string representation for this memeber
	\return true if data was successfuly converted
	*/
	virtual bool get_string(const Struct& _struct,CString_& _str) const
	{
		typename _Expression::result res = m_expression(_struct);
		return m_datahelper.convert(res,_str);
	}

	/**\brief compare two items (same members) of grid data
	\param[in] _struct1 -- first data to compare (from some row of grid)
	\param[in] _struct2 -- second data to comapre (from another row of grid)
	\return value < 0 if _struct1 < _struct2;
		    value > 0 if _struct1 > _struct2;
			value == 0 if _struct1 == _struct2;
	*/
	virtual long compare(const Struct& _struct1,const Struct& _struct2) const
	{
		typename _Expression::result r1 = m_expression(_struct1);
		typename _Expression::result r2 = m_expression(_struct2);

		return m_datahelper.compare(r1,r2);
	}

	/**\brief return caption of this column (for this item of grid data)
	\param[out] _caption -- caption of this column
	\return true if column have a caption
	*/
	virtual bool get_caption(CString_& _caption) const
	{
		_caption = m_sColumnName;
		return true;
	}

	/**\brief return size (default width) of this column
	\return default width (size) of this column
	*/
	virtual long get_size() const
	{
		return m_defsize;
	}

	/**\brief return id of this column
	\return identifier of this column
	*/
	virtual DWORD get_id() const 
	{
		return m_id;
	}

	/**\brief return align for this column
	\return alignment for this column
	*/
	virtual GridAlignEn get_align() const
	{
		return m_align;
	}

protected:
	_Expression m_expression;		///< expression of this item
	const _DataHelper m_datahelper; ///< data helper for this column 
									/**<\sa DataHelpers*/
	CString_ m_sColumnName;			///< column name
	long m_defsize;					///< column default size (width)
	GridAlignEn m_align;			///< column alignment 
									/**<\sa GridAlignEn*/
	DWORD m_id;						///< column identifier
};//template<> struct CGridLayoutExpressionItem




typedef std::list<CRect> RectLst;	///< list of rects

bool operator<(const CRect& _rc1,const CRect& _rc2);	
///< declare operator for MSVC 5.0 correct work with \ref RectLst
bool operator>(const CRect& _rc1,const CRect& _rc2);
///< declare operator for MSVC 5.0 correct work with \ref RectLst

struct CGridColumnGroup;

/**\brief library inner class to support column groups creation
*/
struct CGridColumnGroupVec : protected std::vector<CGridColumnGroup*> 
{
private:
	typedef std::vector<CGridColumnGroup*> base;	///< type def base class
public:
	/**\brief constructor.
	*/
	~CGridColumnGroupVec()
	{
		free();
	}
	/**\brief adds new column
	\param _sGroupName -- current group name
	\param _currindex -- current index where group is added
	*/
	CGridColumnGroup* push(const CString_& _sGroupName,long _currindex);

	/**\brief add new column group
	\param _group -- group to add
	*/
	void push(CGridColumnGroup* _group) {base::push_back(_group);}

	/**\brief return group for specified position
	\param _depth -- depth of group 
	\param _index -- column index
	\return constant pointer to \ref CGridColumnGroup
	*/
	const CGridColumnGroup* get(long _depth,long _index) const;

	/**\brief build cells covered list (recurrent function)
	Cells covered list is used to set covered cell in a grid.
	\param _covered -- list of covered items
	\param _height -- current height
	\param _start -- current start column
	\param _end -- current end column
	\param _maxheight -- grid height
	*/
	void get_covered(RectLst& _covered,long _height,long _start,long _end,long _maxheight) const;

	/**\brief frees \ref CGridColumnGroupVec*/
	void free();

	/**\brief verify if \ref CGridColumnGroupVec
	\return true if \ref CGridColumnGroupVec is empty
	*/
	bool empty() const {return base::empty();}

	/**\brief return current top item.
	This function is used for building of column groups
	\return \ref CGridColumnGroup pointer
	*/
	CGridColumnGroup* top() {return base::back();}

	/**\brief return current top item.
	This function is used for building of column groups
	\return \ref CGridColumnGroup constant pointer
	*/
	const CGridColumnGroup* top() const {return base::back();}

	/**\brief pop last item
	*/
	void pop() {base::pop_back();}

	/**\brief return size
	\return size of vector of \ref CGridColumnGroup
	*/
	long get_size() const {return base::size();}

	/**\brief return true if inedx (_index) inside
	\param _index -- index to verify for
	\return true if inedx (_index) inside
	*/
	bool inside(long _index) const; 
};

/**\brief library inner class to support column groups creation.
This class is used to support a columns groups
*/
struct CGridColumnGroup
{
	/**\brief Constructor
	\param _sGroupName -- column group name
	\param _nCurIndex -- current column index (used as start index of group, end index is set to undefined(-1))
	*/
	CGridColumnGroup(const CString_& _sGroupName,long _nCurIndex)
		:m_name(_sGroupName),m_start(_nCurIndex),m_end(-1)
	{
	}

	/**\brief add sub group
	\param[in] _sGroupName -- subgroup name
	\param[in] _currindex -- current column index
	\return pointer to \ref CGridColumnGroup
	*/
	CGridColumnGroup* add(const CString_& _sGroupName,long _currindex)
	{
		return m_subgroups.push(_sGroupName,_currindex);
	}

	/**\brief sets end index for this group
	\param[in] _index -- current column index
	*/
	void set_end(long _index)
	{
		m_end = _index;
		ASSERT_(NEQL(m_end,m_start));//group can`t be empty
	}

	/**\brief verify is index inside of this group
	\param[in] _index -- index of column
	\return true if column (_index) inside this group
	*/
	bool inside(long _index)
	{
		return _index>=m_start
			&& (m_end==-1 || _index<m_end)
			;
	}

	/**\brief return \ref CGridColumnGroup pointer fro specified header position
	\param[in] _depth -- depth to get for
	\param[in] _index -- index to get for
	\return constant \ref CGridColumnGroup pointer
	*/
	const CGridColumnGroup* get(long _depth,long _index) const
	{
		return m_subgroups.get(_depth,_index);
	}

	/**\brief return name of group
	\return name of this group
	*/
	const CString_& get_name() const {return m_name;}

	/**\brief build covered cells (recursive)
	\param[in,out] _covered -- grid covered cells list
	\param[in] _height -- height to build for
	\param[in] _start -- start column index to build for
	\param[in] _maxheight -- grid header height
	*/
	void get_covered(RectLst& _covered,long _height,long _start,long _end,long _maxheight) const
	{
		_covered.push_back(CRect(m_start,_maxheight-_height,m_end>=0?m_end:_end,_maxheight-_height+1));
		m_subgroups.get_covered(_covered,_height-1,m_start,m_end>=0?m_end:_end,_maxheight);
	}

protected:
	CGridColumnGroupVec m_subgroups;	///< vector of inner column groups
	long m_start;						///< start column of this group
	long m_end;							///< end column of this group
	CString_ m_name;						///< name of this column group
};

inline
CGridColumnGroup* CGridColumnGroupVec::push(const CString_& _sGroupName,long _currindex)
{
	push_back(trace_alloc(new CGridColumnGroup(_sGroupName,_currindex)));
	return back();
}

inline
const CGridColumnGroup* CGridColumnGroupVec::get(long _depth,long _index) const
{
	const_iterator it,ite;
	it = begin();
	ite = end();
	for(;it!=ite;++it)
	{
		if(!(*it)->inside(_index)) continue;
		if(EQL(_depth,0)) return *it;
		else return (*it)->get(_depth-1,_index);
	}
	return NULL;
}

inline
void CGridColumnGroupVec::free()
{
	iterator it,ite;
	it = begin();
	ite = end();
	for(;it!=ite;++it) delete trace_free(*it);
	clear();
}

inline
void CGridColumnGroupVec::get_covered(RectLst& _covered,long _height,long _start,long _end,long _maxheight) const
{
	const_iterator it,ite;
	it = begin();
	ite = end();
	for(;it!=ite;++it)
	{
		(*it)->get_covered(_covered,_height,_start,_end,_maxheight);
	}
	if(_height>1)
	{
		long index = 0;
		for(index=_start;index<_end;index++)
		{
			if(inside(index)) continue;
			_covered.push_back(CRect(index,_maxheight-_height,index+1,_maxheight));
		}
	}
}

inline
bool CGridColumnGroupVec::inside(long _index) const
{
	const_iterator it,ite;
	it = begin();
	ite = end();
	for(;it!=ite;++it) 
	{
		if((*it)->inside(_index)) return true;
	}
	return false;
}


/**\brief Grid layout base class.
This class is used to create layout (description of columns) to output 
grid data to grid.
\tparam _Struct -- data type to display at grid.
*/
template<typename _Struct>
struct CGridLayoutBase
{
protected:
	typedef std::vector<IGridLayoutItem<_Struct>* > GridLayoutItemsLst; ///< column items vector

public:	

	/**\brief Constructor.
	*/
	CGridLayoutBase():m_groupdepth(0)
	{
	}

	/**\brief Destruuctor.
	*/
	virtual ~CGridLayoutBase()
	{
		free();
	}

	/**\brief Template function to add column to layout.
	This fucntion add column with default data helper
	\tparam _Item -- data member type
	\param _sColumnName -- name of the column
	\param _ndefsize -- default size (width) of the column
	\param _pmember -- pointer to member variable of a grid data.
	\param _id -- column identifier
	\sa DataHelpers
	*/
	template<typename _Item>
		void add_column(const CString_& _sColumnName
			,long _ndefsize
			,GridAlignEn _align
			,_Item _Struct::* _pmember
			,DWORD _id = -1
			)
	{
		if(EQL(_id,-1)) _id = get_fldUnique(_pmember);
		m_items.push_back(
			trace_alloc(
					(
						new CGridLayoutItem<_Struct,_Item,CGridDefaultDataHelper>
						(
							_sColumnName
							,_pmember
							,_ndefsize
							,_align
							,_id
							,CGridDefaultDataHelper()
						)
					)
				)
			);
	}

	/**\brief Template function to add column expression to layout.
	This function add column expression with default data helper
	\tparam _Expression -- expression functor (class or structure that define operator()(const _Struct& _struct))
	\param _sColumnName -- name of the column
	\param _ndefsize -- default size (width) of the column
	\param _expression -- expression functor
	\param _id -- column identifier
	\sa DataHelpers
	*/
	template<typename _Expression>
		void add_column_expression(
			const CString_& _sColumnName
			,long _ndefsize
			,GridAlignEn _align
			,_Expression _expression
			,DWORD _id = -1
		)
	{
		//if(EQL(_id,-1)) _id = get_fldUnique(_pmember);
		m_items.push_back(
			trace_alloc(
					(
						new CGridLayoutExpressionItem<_Struct,_Expression,CGridDefaultDataHelper>
						(
							_sColumnName
							,_expression
							,_ndefsize
							,_align
							,-1
							,CGridDefaultDataHelper()
						)
					)
				)
			);
	}

	/**\brief template fucntion to add column to layout (overload to specify data helper for column)
	\tparam _Item -- data member type
	\tparam _DataHelper -- data helper type
	\param _sColumnName -- name of the column
	\param _ndefsize -- default size (width) of the column
	\param _pmember -- pointer to member variable of a grid data.
	\param _datahelper -- data helper object which will be used for this column
	\param _id -- column identifier
	\sa DataHelpers
	*/
	template<typename _Item,typename _DataHelper>
		void add_column(const CString_& _sColumnName
			,long _ndefsize
			,GridAlignEn _align
			,_Item _Struct::* _pmember
			,const _DataHelper& _datahelper
			,DWORD _id = -1
			)
	{
		if(EQL(_id,-1)) _id = get_fldUnique(_pmember);
		m_items.push_back(
				trace_alloc(
					(
						new CGridLayoutItem<_Struct,_Item,_DataHelper>
						(
							_sColumnName
							,_pmember
							,_ndefsize
							,_align
							,_id
							,_datahelper
						)
					)
				)
			);
	}

	/**\brief Template function to add column expression to layout (overload function to specify data helper ).
	This function add column expression with default data helper
	\tparam _Expression -- expression functor (class or structure that define operator()(const _Struct& _struct)) type
	\tparam _DataHelper -- data helper type
	\param _sColumnName -- name of the column
	\param _ndefsize -- default size (width) of the column
	\param _expression -- expression functor
	\param _datahelper -- data helper object which will be used for this column
	\param _id -- column identifier
	\sa DataHelpers
	*/
	template<typename _Expression,typename _DataHelper>
		void add_column_expression(
			const CString_& _sColumnName
			,long _ndefsize
			,GridAlignEn _align
			,_Expression _expression
			,const _DataHelper& _datahelper
			,DWORD _id = -1
		)
	{
		//if(EQL(_id,-1)) _id = get_fldUnique(_pmember);
		m_items.push_back(
			trace_alloc(
					(
						new CGridLayoutExpressionItem<_Struct,_Expression,_DataHelper>
						(
							_sColumnName
							,_expression
							,_ndefsize
							,_align
							,-1
							,_datahelper()
						)
					)
				)
			);
	}

	/**\brief fucntion to start columns group
	\param _groupname -- froup name
	*/
	void start_group(const CString_& _groupname)
	{
		CGridColumnGroup* pgroup = NULL;
		if(m_groupstack.empty()) 
		{
			pgroup = m_groups.push(_groupname,m_items.size());
		}
		else 
		{
			pgroup = m_groupstack.top();
			pgroup = pgroup->add(_groupname,m_items.size());
		}
		m_groupstack.push(pgroup);
		m_groupdepth = std::_cpp_max(m_groupstack.get_size(),m_groupdepth);
	}

	/**\brief ends the current group
	*/
	void end_group()
	{
		VERIFY_EXIT(!m_groupstack.empty());// call end_group() without start_group() call
		m_groupstack.top()->set_end(m_items.size());
		m_groupstack.pop();
	}

	/**\brief return caption for specified header position
	\param[in] _row -- row of grid
	\param[in] _col -- column of grid
	\param[out] _str -- caption for this position
	\return true if caption exists.
	*/
	bool get_caption(long _row,long _col,CString_& _str) const 
	{
		if(_col>=get_columnsCount())
			TRACE_(_T("Row = %d Col = %d MaxCol = %d\n"),_row,_col,get_columnsCount());

		VERIFY_EXIT1(
			_col>=0 && _col<get_columnsCount()
				&& _row>=0 && _row<get_titlerows()
			,false
			);
		const CGridColumnGroup* pgroup = m_groups.get(_row,_col);
		if(NOT_NULL(pgroup))
		{
			_str = pgroup->get_name();
			return true;
		}
		GridLayoutItemsLst::const_iterator it = m_items.begin();
		std::advance(it,_col);
		return (*it)->get_caption(_str);
	}

	/**\brief return height of header
	\return height of grid header for this layout
	*/
	long get_titlerows() const
	{
		return m_groupdepth+1;
	}

	/**\brief build and return list of covered cells for header
	\param[out] _covered -- list of covered cells
	\return true if returned list is not empty
	*/
	bool get_gridCovered(RectLst& _covered) const
	{
		_covered.clear();
		if(EQL(m_groupdepth,0)) return !_covered.empty();
		m_groups.get_covered(_covered,get_titlerows(),0,get_columnsCount(),get_titlerows());
		return !_covered.empty();
	}

	/**\brief return size (width) of column
	\param[in]  _col -- column index
	\param[out] _ndefsize -- default size
	\return true if there is a default size for the column
	*/
	bool get_size(long _col,long& _ndefsize) const
	{
		VERIFY_EXIT1(_col>=0 && _col<get_columnsCount(),false);
		GridLayoutItemsLst::const_iterator it = m_items.begin();
		std::advance(it,_col);
		_ndefsize = (*it)->get_size();
		return true;
	}

	/**\brief return string for column from grid data
	\param[in] _col - column index
	\param[in] _struct -- grid data (for some row)
	\param[out] _str -- string representation for grid data for this column
	\return true if conversion was successfull
	*/
	bool get_string(long _col,const _Struct& _struct,CString_& _str) const
	{
		VERIFY_EXIT1(_col>=0 && _col<get_columnsCount(),false);
		GridLayoutItemsLst::const_iterator it = m_items.begin();
		std::advance(it,_col);
		return (*it)->get_string(_struct,_str);
	}
	
	/**\brief return string for column from grid data
	\param[in] _col - column index
	\param[in] _pstruct -- grid data (for some row)
	\param[out] _str -- string representation for grid data for this column
	\return true if conversion was successfull
	*/
	bool get_string(long _col,const _Struct* _pstruct,CString_& _str) const
	{
		VERIFY_EXIT1(NOT_NULL(_pstruct),false);
		VERIFY_EXIT1(_col>=0 && _col<get_columnsCount(),false);
		GridLayoutItemsLst::const_iterator it = m_items.begin();
		std::advance(it,_col);
		return (*it)->get_string(*_pstruct,_str);
	}

	/**\brief return identifier for a column.
	\param[in] _col -- column index
	\param[out] _id -- column identifier
	\return true if there is identifier for this column
	*/
	bool get_id(long _col,DWORD& _id) const 
	{
		VERIFY_EXIT1(_col>=0 && _col<get_columnsCount(),false);
		GridLayoutItemsLst::const_iterator it = m_items.begin();
		std::advance(it,_col);
		_id = (*it)->get_id();
		return true;
	}

	/**\brief search column index for specified column identity
	\param[in] _id -- identifier to search
	\return column index of -1 if column identifier was not founded
	*/
	long find_id(DWORD _id) const
	{
		GridLayoutItemsLst::const_iterator 
			it = m_items.begin()
			,ite = m_items.end()
			;
		long col=0;
		for(col=0;it!=ite;++it,col++)
		{
			if((*it)->get_id()==_id) return col;
		}
		return -1;
	}

	/**\brief return alignment for specified column
	\param[in] _col -- column index
	\param[out] _align -- column alignment
	\return true if specified alignment was founded
	*/
	bool get_align(long _col,GridAlignEn& _align) const 
	{
		VERIFY_EXIT1(_col>=0 && _col<get_columnsCount(),false);
		GridLayoutItemsLst::const_iterator it = m_items.begin();
		std::advance(it,_col);
		_align =  (*it)->get_align();
		return NEQL(GA_Null,_align);
	}

	/**\brief compare to grid data objects (from two row)
	\param[in] _col -- column to compare data for
	\param[in] _struct1 -- first grid data object (from some row)
	\param[in] _struct2 -- second grid data object (from another row)
	\return value < 0 if _struct1 < _struct2;
		    value > 0 if _struct1 > _struct2;
			value == 0 if _struct1 == _struct2;
	*/
	long compare(long _col,const _Struct& _struct1,const _Struct& _struct2) const
	{
		VERIFY_EXIT1(_col>=0 && _col<get_columnsCount(),0);
		GridLayoutItemsLst::const_iterator it = m_items.begin();
		std::advance(it,_col);
		return (*it)->compare(_struct1,_struct2);
	}

	/**\brief compare to grid data objects (from two row)
	\param[in] _col -- column to compare data for
	\param[in] _pstruct1 -- first grid data object (from some row)
	\param[in] _pstruct2 -- second grid data object (from another row)
	\return value < 0 if _struct1 < _struct2;
		    value > 0 if _struct1 > _struct2;
			value == 0 if _struct1 == _struct2;
	*/
	long compare(long _col,const _Struct* _pstruct1,const _Struct* _pstruct2) const
	{
		VERIFY_EXIT1(NOT_NULL(_pstruct1) && NOT_NULL(_pstruct2),0);
		VERIFY_EXIT1(_col>=0 && _col<get_columnsCount(),0);
		GridLayoutItemsLst::const_iterator it = m_items.begin();
		std::advance(it,_col);
		return (*it)->compare(*_pstruct1,*_pstruct2);
	}

	/**\brief return columns count
	\return columns count for this layout
	*/
	long get_columnsCount() const {return m_items.size();}

protected:
	/**\brief frees layout data*/
	void free()
	{
		GridLayoutItemsLst::iterator it,ite;
		it = m_items.begin();
		ite = m_items.end();
		for(;it!=ite;++it)
		{
			delete trace_free(*it);
		}
		m_items.clear();
	}
protected:
	GridLayoutItemsLst m_items;			///> list of columns 
	CGridColumnGroupVec m_groupstack;	///> groups stack
										/**< don`t used anywhere except initialization time, 
										but we can`t define end of initialization time*/
	long m_groupdepth;					///> groups depth. is is used in group definition
	CGridColumnGroupVec m_groups;		///> list of top groups
};//template<> struct CGridLayoutBase

/**\brief Get layout of some struct.
If struct don`t have layout then will be rised complire error. This fucntion 
helps relieve of long time start -- object of layout is created on demand and 
once for one data class/struct (thats difines layout for itself).
\return \ref CGridLayoutBase for specified struct
\sa CGridLayoutBase
*/
template<typename _Struct>
inline const CGridLayoutBase<_Struct>& GetGridLayout()
{
	typename _Struct::CGridLayout;
	static _Struct::CGridLayout _;
	return _;
}

typedef std::vector<pairex<long,bool> >	GridSortedColumnsLst; ///< typedef for sort purpose

template<typename _Type>
struct IsType
{
	typedef typename _Type::value_type value;
	enum{
		Vector = Conversion<_Type*,std::vector<value>* >::exists
		,List  = Conversion<_Type*,std::list<value>* >::exists
	};
};

template<typename _Struct> struct op_GridSort;

template<typename _Struct,typename _Cont> inline
	void sort_cont(_Cont& _vec,op_GridSort<_Struct>* _pthis,Int2Type<0>);
template<typename _Struct,typename _Cont> inline
	void sort_cont(_Cont& _list,op_GridSort<_Struct>* _pthis,Int2Type<1>);
template<typename _Struct,typename _Container> inline
	void sort_vec(_Container& _vec,op_GridSort<_Struct>* _pthis);
template<typename _Struct,typename _Type> inline
	void sort_list(std::list<_Type>& _lst,op_GridSort<_Struct>* _pthis);

/**\brief class to suport sorting of grid data
\tparam _Struct -- grid data type
*/
template<typename _Struct>
struct op_GridSort
{
	/**\brief sort pointers to data
	\param _pd1 -- first data pointrer (some row)
	\param _pd2 -- second data pointer (another row)
	\return true if *_pd1 <= *_pd2
	*/
	bool operator()(const _Struct* _pd1,const _Struct* _pd2)
	{
		VERIFY_EXIT1(NOT_NULL(_pd1)	&& NOT_NULL(_pd2),false);
		const CGridLayoutBase<_Struct>& layout = GetGridLayout<_Struct>();
		long i = 0;
		long ret = 0;
		for(i=0;i<m_sortedcolumns.size();i++)
		{
			long col = m_sortedcolumns[i].first;
			ret = layout.compare(col,*_pd1,*_pd2);
			if(m_sortedcolumns[i].second) ret = -ret;
			if(ret) return ret>0;
		}
		return ret>0;
	}

	/**\brief sort references to grid data
	\param _d1 -- first data reference (some row)
	\param _d2 -- second data reference (another row)
	\return true if _d1 <= _d2
	*/
	bool operator()(const _Struct& _d1,const _Struct& _d2)
	{
		return operator()(&_d1,&_d2);
	}

	/**\brief clear sort conditions
	*/
	void clear()
	{
		m_sortedcolumns.clear();
	}

	/**\brief set sort condition
	\param _col -- column to sort by
	\param _binvers -- sort in invers order
	*/
	void sort_by(long _col,bool _binvers)
	{
		GridSortedColumnsLst::iterator it = std::find(
			m_sortedcolumns.begin()
			,m_sortedcolumns.end()
			,GridSortedColumnsLst::value_type(_col,false)
			);
		if(it!= m_sortedcolumns.end())
		{
			it->second = true;
			return;
		}
		it = std::find(
			m_sortedcolumns.begin()
			,m_sortedcolumns.end()
			,GridSortedColumnsLst::value_type(_col,true)
			);
		if(it!=m_sortedcolumns.end())
		{
			it->second = false;
			return;
		}
		m_sortedcolumns.push_back(GridSortedColumnsLst::value_type(_col,_binvers));
	}


	/**\brief start sorting of grid data container (default implementaion fos std::vector() data)
	\param _vec -- container to sort data of it
	*/
	template<typename _Container>
		void sort(_Container& _data)
	{
		if(m_sortedcolumns.empty()) return;
		sort_cont(_data,this,Int2Type<IsType<_Container>::List>());
	}
protected:
	GridSortedColumnsLst m_sortedcolumns;	///< list of column to sort
};//struct op_GridSort

template<typename _Struct,typename _Cont> inline
	void sort_cont(_Cont& _vec,op_GridSort<_Struct>* _pthis,Int2Type<0>)
{
	sort_vec(_vec,_pthis);
}


template<typename _Struct,typename _Cont> inline
	void sort_cont(_Cont& _list,op_GridSort<_Struct>* _pthis,Int2Type<1>)
{
	sort_list(_list,_pthis);
}

template<typename _Struct,typename _Container> inline
	void sort_vec(_Container& _vec,op_GridSort<_Struct>* _pthis)
{
	typedef std::list<_Container::value_type>* ContAsList;
	std::sort(_vec.begin(),_vec.end(),*_pthis);
}

/**\brief sort of list of \ref _Struct data
\param _lst -- container list to sort
*/
template<typename _Struct,typename _Type> inline
	void sort_list(std::list<_Type>& _lst,op_GridSort<_Struct>* _pthis)
{
	typedef std::list<_Type> StructLst;
	typedef std::vector<_Type*> StructPtrVec;
	StructPtrVec spv;
	{
		StructLst::iterator it,ite;
		it = _lst.begin();
		ite = _lst.end();
		spv.clear();
		for(;it!=ite;++it) spv.push_back(&*it);
	}
	std::sort(spv.begin(),spv.end(),*_pthis);
	StructPtrVec::iterator it,ite;
	it = spv.begin();
	ite = spv.end();
	StructLst res;
	for(;it!=ite;++it)
	{
		StructLst::iterator itemit,itemite;
		itemit = _lst.begin();
		_Struct* p0 = *it;
		for(;itemit!=_lst.end();++itemit) 
		{
			_Struct* p1 = &*itemit;
			if(EQL(p0,p1)) break;
		}
		itemite = itemit;
		if(itemite!=_lst.end()) ++itemite;
		res.splice(res.end(),_lst,itemit,itemite);
	}
	_lst = res;
}

//#pragma warning(pop)


//@}

/**\page Page_QuickStart_GridLayout Quick start: "Grid layout templates".

Grid layout classes are used to link some struct member variables with grid columns and
define it output. 

Think we have data struct like this.

\code
struct CClientRow
{
	double m_fClientID;	
	CString_ m_sClientName;
	long m_nClientType;
	CString_ m_sClientTypeStr;
	long m_nClientStatus;
	CString_ m_sClientStatusStr;		
	long m_nClientIdentifierType;
	CString_ m_sClientIdentifierTypeStr;
	CString_ m_sClientIdentifier1;
	CString_ m_sClientIdentifier2;
	long m_nCBU;
	Date m_RegDate;
	double m_fForexClientID;
	double m_fHBClientID;
	double m_fSTCashClientID;
	double m_fERKID;
	double m_fERKRegNum;
	CString_ m_sHBName;
};
\endcode

So write grid layout for this structure.

\code
struct CClientRow
{
	double m_fClientID;	
	CString_ m_sClientName;
	long m_nClientType;					
	CString_ m_sClientTypeStr;
	long m_nClientStatus;				
	CString_ m_sClientStatusStr;		
	long m_nClientIdentifierType;
	CString_ m_sClientIdentifierTypeStr;
	CString_ m_sClientIdentifier1;
	CString_ m_sClientIdentifier2;
	long m_nCBU;
	Date m_RegDate;
	double m_fForexClientID;
	double m_fHBClientID;
	double m_fSTCashClientID;
	double m_fERKID;
	double m_fERKRegNum;
	CString_ m_sHBName;

	struct CGridLayout : public CGridLayoutBase<CClientRow>
	{
		CGridLayout()
		{
			// specify name, default width, aligment, variable member pointer [,data helper] [,column identifier]
			add_column(_T("Имя"),10,GA_Left,&CClientRow::m_sClientName);
			add_column(_T("Тип"),10,GA_Center,&CClientRow::m_nClientType
				,AnalyticDataHelper<PriorSqlSwitch>(AT_SDBO_ClienType)
				);
			add_column(_T("Статус"),10,GA_Center,&CClientRow::m_nClientStatus
				,AnalyticDataHelper<PriorSqlSwitch>(AT_SDBO_ClientStatus)
				);
			start_group(_T("Идентификатор"));
				add_column(_T("Тип"),20,GA_Center,&CClientRow::m_nClientIdentifierType
					,AnalyticDataHelper<PriorSqlSwitch>(AT_SDBO_ClientIdentifierType)
					);
				add_column(_T("Значение"),10,GA_Left,&CClientRow::m_sClientIdentifier1);
				//add_column(_T(""),10,GA_Left,&CClientRow::m_sClientIdentifier2);
			end_group();
			add_column(_T("ЦБУ"),10,GA_Left,&CClientRow::m_nCBU,CBUDataHelper());
			add_column(_T("Дата регистрации"),10,GA_Center,&CClientRow::m_RegDate);
		}
	};// struct CGridLayout
};
\endcode

As you can see, we should declare class with name CGridLayout in a scope of class 
for which we define layout and derive it from\ref CGridLayoutBase with template param 
of type of our source class. Then in the constructor of class CGridLayout we should 
declare (create) grid layout for ouw class. Layout is declared by calling of function 
\ref CGridLayoutBase::add_column() , \ref CGridLayoutBase::start_group() and 
\ref CGridLayoutBase::end_group(), declaring column, columns group or columns group end.
Columns group can be nested, but you can not create group with out any column (it is cause 
rinetime exception). 

To create column you should specify:
	-# coulmn name 
	-# default column width (size)
	-# columnn alignment
	-# data fileld member pointer
	-# [may be] data helper 
	-# [may be] column identifier


To cretae group you should:
	-# call \ref CGridLayoutBase::start_group() with group name
	-# call one or more times \ref CGridLayoutBase::add_column(), to specify
	   columns which should enter this group
	-# and call \ref CGridLayoutBase::end_group() to close group.


See also \ref Page_QuickStart_Datahelpers \n
See also \ref Page_QuickStart_GridTemplates \n

*/


#endif//#if !defined(__GRIDLAYOUTTEMPLATES_H__FCD09A8E_4E97_4B71_AB01_7E7335DFC8BA)
