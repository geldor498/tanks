#if !defined(__GRIDTEMPLATES_H__4F6C3B42_38A1_47E1_91E1_3C1C3D48E3F8)
#define __GRIDTEMPLATES_H__4F6C3B42_38A1_47E1_91E1_3C1C3D48E3F8

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "config.h"
#include "atlmfc.h"

#include <list>
#include <vector>
#include <algorithm>
#include <utility>

#include "utils.h"
#include "thread.h"
#include "NullType.h"
#include "typelist.h"
#include "PriorSqlTemplates.h"
#include "GridLayoutTemplates.h"
#include "IUtilitiesUnknown.h"
#include "synchronize.h"

/**\defgroup GridTemplates Utilities support of grid data.
*/
//@{

/**\brief interface for grid data holder to catch on data grid updated event*/
interface IGridDataRecipient
{
	/**\brief call back for data loaded event
	\param _lpParam -- parameters
	*/
	virtual void OnGridDataLoaded(LPVOID _lpParam) = 0;
};//interface IGridDataRecipient

/**\struct to reperesent grid column identifier
*/
struct GridFieldID
{
	/**\brief Constructor
	\param _id -- identifier
	*/
	GridFieldID(DWORD _id):id(_id){}

	DWORD id; ///< identifier
};

interface IGridColumns
{
	/**\brief return columns count
	\return columns count
	*/
	virtual long get_columnCount() const = 0;

	/**\brief return header rows count
	\return header rows count
	*/
	virtual long get_titleRowsCount() const = 0;

	/**\brief get column caption for specified position in grid header.
	\param[in] _row -- row to get column header for.
	\param[in] _col -- column to get column header for.
	\param[out] _sColumnName -- column name for specified cell of the header.
	\return true if column name was getted successfully.
	*/
	virtual bool get_columnCaption(long _row,long _col,CString_& _sColumnName) const = 0;

	/**\brief retry coulmn alignment.
	\param[in] _col -- column index to get alignment for.
	\param[out] _align -- alignment of specified column.
	\return true if aligment was getted successfully.
	*/
	virtual bool get_columnAlign(long _col,GridAlignEn& _align) const = 0;

	/**\brief return coulnm default width (size)
	\param _col -- column index to get default width for.
	\return default width for specified coulmn.
	*/
	virtual long get_columnSize(long _col) const = 0;

	/**\brief return column identifier
	\param[in] _col -- grid column
	\param[out] _id -- returned identifier
	\return true if identifier was successfully getted.
	*/
	virtual bool get_id(long _col,DWORD& _id) const = 0;

	/**\brief return list of header covered cells
	\param[out] _covered -- rectangles list of covered cells for grid header
	\return true if list is not empty
	*/
	virtual bool get_titleCoveredCells(RectLst& _covered) const = 0;
};

DECLARE_INTERFACE_NAME(IGridColumns);

interface IGridData
{
	/**\brief clear all data of grid*/
	virtual void clear() = 0;

	/**\brief verify data loaded state for grid
	\return true if grid data already loaded
	*/
	virtual bool is_dataReady() const = 0;

	/**\brief return rows count (already loaded)
	\return rows count 
	*/
	virtual long get_rowCount() const = 0;

	/**\brief return string for specified grid cell
	\param[in] _row -- row of grid
	\param[in] _col -- column index
	\param[out] _str -- retusrned string of column data
	\return true if string was getted successfully
	*/
	virtual bool get_string(long _row,long _col,CString_& _str) const = 0;

	/**\brief starts data retreing
	\param _bstop -- if true then if data retrieing process is already running then
	       it stops first; if false function just exit if data retriving process 
		   is already running.
	*/
	virtual void start_retryData(bool _bstop = false) = 0;
};

DECLARE_INTERFACE_NAME(IGridData);

interface IGridSort
{
	/**\brief clear sort list.*/
	virtual void clear_sortList() = 0;

	/**\brief add sort description
	\param[in] _col -- column to sort by in grid
	\param[in] _binvers -- inverse sort order
	*/
	virtual void add_toSortList(long _col,bool _binvers) = 0;

	/**\brief add sort description (used column identifier insted of column index)
	\param[in] _id -- identifier
	\param[in] _binvers -- inverse sort order
	*/
	virtual void add_FieldID2SortList(DWORD _id,bool _binvers) = 0;

	/**\brief sort grid data
	*/
	virtual void sort_data() = 0;
};

DECLARE_INTERFACE_NAME(IGridSort);

interface IGridNotify
{
	/**\brief set window notifier to get messages from grid data
	\param[in] _hwnd -- window handle which should recive notifications 
	\param[in] _msg -- window notification message to send
	\param[in] _lpParam -- params to send with notification message
	*/
	virtual void initialize_notifier(HWND _hwnd,UINT _msg,LPVOID _lpParam) = 0;
};

DECLARE_INTERFACE_NAME(IGridNotify);

interface IGridPosition
{
	/**\brief save grid row id for letter use 
	This function helps to save and restore grid position after some operations
	that changes grid data item order.
	\param[in] _row -- row index to save.
	\return true if row id was saved successfully
	\sa IGridPosition::restore_savedrow(long& _row)
	*/
	virtual bool save_rowid(long _row) = 0;

	/**\brief return row of saved id (see \ref IGridDataInterface::save_rowid(long _row) )
	\param[out] _row -- row index of saved row id
	\return true if row was successfully got.
	\sa IGridPosition::save_rowid(long _row)
	*/
	virtual bool restore_savedrow(long& _row) = 0;
};

DECLARE_INTERFACE_NAME(IGridPosition)

typedef TYPELIST_5(IGridColumns,IGridData,IGridSort,IGridNotify,IGridPosition) GridInterfacesLst;

/**\brief Interface that supports base data of grid operations.
This interface used to realize programming pattern bridge.
*/
interface IGridDataInterface
: 
	public IUtilitiesUnknownImpl<IGridDataInterface,GridInterfacesLst>
	,public IGridColumns
	,public IGridData
	,public IGridSort
	,public IGridNotify
	,public IGridPosition
{
};//interface IGridDataInterface

/**\brief inner interface of id field item.
\tparam _Struct -- grid data item
*/
template<typename _Struct>
interface IIDSaveRestoreField
{
	/**\brief this fucntion save state for some struct
	\param _struct -- data item
	\return true if data was successfully saved
	*/
	virtual bool save(const _Struct& _struct) = 0;

	/**\brief compare some struct with saved data
	\param _struct -- data item
	\return true if saved data equal to specified data item
	*/
	virtual bool compare(const _Struct& _struct) const = 0;
};//template<> interface IIDSaveRestoreField

/**\brief implementation of \ref IIDSaveRestoreField
This class is the one from classes that supports base functionality to 
search grid rows by identity field. This class supports field to 
serach by.
\tparam _Struct -- grid data item
\tparam _FieldType -- field type 
*/
template<typename _Struct,typename _FieldType>
struct IIDSaveRestoreFieldImpl : public IIDSaveRestoreField<_Struct>
{
	/**\brief Constructor
	\param _pfld -- member pointer of grid data that is used as a identity field.
	*/
	IIDSaveRestoreFieldImpl(_FieldType _Struct::* _pfld)
		:m_pfld(_pfld)
	{
		m_saveddata = _FieldType();
		VERIFY_EXIT(NOT_NULL(_pfld));
	}

	/**\brief implementation of \ref IIDSaveRestoreField::save(const _Struct& _struct)
	\param _struct -- row of data item to save state for
	\return true if state of this row grid data was saved successfully
	\sa IIDSaveRestoreField::save(const _Struct& _struct)
	*/
	virtual bool save(const _Struct& _struct)
	{
		VERIFY_EXIT1(NOT_NULL(m_pfld),false);
		m_saveddata = _struct.*m_pfld;
		return true;
	}

	/**\brief implementation of \ref IIDSaveRestoreField::compare(const _Struct& _struct) const
	\param _struct -- data item
	\return true if saved data equal to specified data item
	\sa IIDSaveRestoreField::compare(const _Struct& _struct) const
	*/
	virtual bool compare(const _Struct& _struct) const
	{
		VERIFY_EXIT1(NOT_NULL(m_pfld),false);
		return _struct.*m_pfld==m_saveddata;
	}
protected:
	_FieldType _Struct::* m_pfld;	///< field that used as identifier
	_FieldType m_saveddata;			///< saved value of identifier
};//template<> struct IIDSaveRestoreFieldImpl


/**\brief Default implementation of save position strategy
tparam _Struct -- grid data type
*/
template<typename _Struct>
struct CDefaultRowIDSaveRestore
{
protected:
	IIDSaveRestoreField<_Struct>* m_pFieldImpl;	///< identity field interface

public:
	/**\brief Constructor.
	*/
	CDefaultRowIDSaveRestore():m_pFieldImpl(NULL)
	{
	}

	/**\brief Destructor.
	*/
	~CDefaultRowIDSaveRestore()
	{
		free();
	}


	/**\brief template method to set identity field
	\tparam _FieldType -- field type
	\param _pfld -- grid data struct member pointer
	*/
	template<typename _Field>
		void set_idfield(_Field _Struct::* _pfield)
	{
		if(NOT_NULL(m_pFieldImpl)) free();
		m_pFieldImpl = trace_alloc( (new IIDSaveRestoreFieldImpl<_Struct,_Field>(_pfield)) );
	}
protected:
	/**\brief free class member
	*/
	void free()
	{
		if(NOT_NULL(m_pFieldImpl))
		{
			delete trace_free(m_pFieldImpl);
			m_pFieldImpl = NULL;
		}
	}

	/**\brief function that save value of grid data struct field 
			  for specified row.
	\tparam _Container -- container
	\param[in] _row -- row index
	\param[in] _cont -- container reference
	\return true if data was successfully saved
	*/
	template<typename _Container>
		bool save_rowid(long _row,const _Container& _cont)
	{
		if(IS_NULL(m_pFieldImpl) || _row<0 || _row>_cont.size()) return false;
		_Container::const_iterator it = _cont.begin();
		std::advance(it,_row);
		const _Struct& rowitem = Private::to_type<_Struct>(*it);
		return m_pFieldImpl->save(rowitem);
	}

	/**\brief function to get row number of saved field 
	\tparam _Container -- container type of the grid
	\param[out] _row -- row index
	\param[in] _cont -- grid data container 
	*/
	template<typename _Container>
		bool restore_savedrow(long& _row,const _Container& _cont)
	{
		if(IS_NULL(m_pFieldImpl)) return false;
		_Container::const_iterator 
			it = _cont.begin()
			,ite = _cont.end()
			;
		long row = 0;
		for(row=0;it!=ite;++it,row++)
		{
			const _Struct& rowitem = Private::to_type<_Struct>(*it);
			if(m_pFieldImpl->compare(rowitem)) 
			{
				_row = row;
				return true;
			}
		}
		return false;
	}
};

/**\brief Grid data implementation.
\tparam _Struct -- data type of one row of a grid.
\tparam _PriorSql -- ST DB access class (PriorSql, PriorSqlSwitch, PriorsqlEx..)
\tparam _OutParams -- list of params reciving by call to grid procedure on every 
        data retry. 
\tparam _OutParamsHolder -- type to get outparam type from this pointer
*/
template<
	typename _Struct
	,typename _PriorSql
	,typename _OutParams = NullType
	,typename _OutParamsHolder = NullType
	,typename _Container = std::list<_Struct>
	>
struct CGridData 
:
	public CWorkedThreadImpl<CGridData<_Struct,_PriorSql,_OutParams,_OutParamsHolder,_Container> > ///< thread implement
	,public IGridDataInterface						///< this class implement \ref IGridDataInterface interface
	,public CDefaultRowIDSaveRestore<_Struct>	///< row id save and restore strategy
{
	typedef _Container Container;			///< typedef for container
	typedef _Struct Struct;					///< typedef for grid row data
	typedef _PriorSql PriorSqlType;			///< typedef for priorsql
	typedef CDefaultRowIDSaveRestore<_Struct> RowIDSaveRestoreStrategy;	///< typedef for row id save and restore strategy

	/**\brief Constructor.
	*/
	CGridData()
		:m_bloaded(true)
		,m_hNotifierWnd(NULL),m_lpParam(NULL),m_NotifyMsg(0)
	{
	}

	/**\brief Destructor.
	*/
	virtual ~CGridData()
	{
		predestruct();
	}

	/**\brief virtual function to prepare call for grid data retry process
	\param _sql -- sql connection to initialize for data retry call
	*/
	virtual bool prepare_call(PriorSqlType& _sql) = 0;

	/**\brief implementation of \ref IGridDataInterface::initialize_notifier(HWND _hwnd,UINT _msg,LPVOID _lpParam)
	Save notify information.
	\param[in] _hwnd -- window handle to send notify message to.
	\param[in] _msg -- notify message to send
	\param[in] _lpParam -- data to send with notify message
	*/
	virtual void initialize_notifier(HWND _hwnd,UINT _msg,LPVOID _lpParam)
	{
		m_hNotifierWnd = _hwnd;
		m_NotifyMsg = _msg;
		m_lpParam = _lpParam;
	}

	/**\brief virtual fuction called after grid data was loaded or grid data loading was stated.
	\param _bloaded -- true if data was loaded; false if data loading was just started (in that
	       state container was already cleared).
	*/
	virtual void data_loaded(bool _bloaded)
	{
		m_bloaded = _bloaded;
		if(NOT_NULL(m_hNotifierWnd) && ::IsWindow(m_hNotifierWnd))
		{
			::PostMessage(m_hNotifierWnd,m_NotifyMsg,0,(LPARAM)m_lpParam);
		}
	}

protected:
	/**\brief function to load grid data and output data from grid procedure
	\tparam _OutputParams -- output params type
	*/
	template<typename _OutputParams>
	void process(_OutputParams*)
	{
		try
		{
			{
			CAutoLock __lock(*this);
			m_bloaded = false;
			}

			PriorSqlType sql;
			if(!prepare_call(sql)) return;

			int ret = 0;
			{
			CAutoLock __lock(*this);
			m_list.clear();
			}
			this->data_loaded(false);

			_OutputParams& outparams 
				= static_cast<_OutputParams&>(
					static_cast<_OutParamsHolder&>(*this)
					);

			ret = CallSqlProcOS(
				CString_()			// proc name already was set
				,outparams			// output params
				,m_list				// stream
				,sql				// sql connection
				,false				// sql connection was already created
				,_get_eventhandle()	// thread stop event handle
				,static_cast<CCriticalSection_&>(m_critsect)
				);
			this->sort_data();
			this->data_loaded(true);
		}catch(...){return;}//just for not exiting the proccess 
	}

	/**\brief process grid data load process when no output params.
	Parameter is used just for metaprogramming (to distinguish situation when
	there is no output params).
	*/
	void process(NullType*)
	{
		try
		{
			{
			CAutoLock __lock(*this);
			m_bloaded = false;
			}

			PriorSqlType sql;
			if(!prepare_call(sql)) return;

			int ret = 0;
			{
			CAutoLock __lock(*this);
			m_list.clear();
			}
			this->data_loaded(false);

			ret = CallSqlProcS(
				CString_()		// proc name was already sets
				,m_list			// stream
				,sql			// connection
				,false			// sql connection was already created
				,_get_eventhandle()	// thread stop event handle
				,static_cast<CCriticalSection_&>(m_critsect)
				);

			this->sort_data();
			this->data_loaded(true);
		}catch(...){return;}//just for not exiting the proccess 

	}

public:

	/**\brief thread main function. 
	This function used by \ref CWorkedThreadImpl class and presents thread main 
	function.
	\return therad exit code.
	*/
	DWORD thread_main()
	{
		_OutParams* p = NULL;
		process(p);
		return 0;
	}

	/**\brief Clear grid data. If data loading proccess is running 
	          Function try to stop it.
	This function is a implementation of \ref IGridDataInterface::clear().
	\sa IGridDataInterface::clear()
	*/
	virtual void clear()
	{
		if(is_running() && !stop(2000)) return;

		CAutoLock __lock(*this);
		m_list.clear();
		m_bloaded = true;
	}

	/**\brief return true if grid data was loaded 
	This function implements  \ref IGridDataInterface::is_dataReady() const
	\return true if data was loaded. 
	\sa IGridDataInterface::is_dataReady() const
	*/
	virtual bool is_dataReady() const
	{
		CAutoLock __lock(*this);
		return /*!is_running() &&*/ m_bloaded;
	}

	/**\brief return count of columns for this grid
	This function implements \ref IGridDataInterface::get_columnCount() const
	\return number of columns for this grid.
	\sa IGridDataInterface::get_columnCount() const
	*/
	virtual long get_columnCount() const
	{
		const CGridLayoutBase<Struct>& layout = GetGridLayout<Struct>();
		return layout.get_columnsCount();
	}

	/**\brief return number of grid data rows
	This function implements \ref IGridDataInterface::get_titleRowsCount() const
	\return number of grid data rows
	\sa \ref IGridDataInterface::get_titleRowsCount() const
	*/
	virtual long get_titleRowsCount() const
	{
		const CGridLayoutBase<Struct>& layout = GetGridLayout<Struct>();
		return layout.get_titlerows();
	}

	/**\brief return column name for a specified header position
	This function impements \ref IGridDataInterface::get_columnCaption(long _row,long _col,CString_& _sColumnName) const
	\param[in] _row -- row number to get caption
	\param[in] _col -- column number to get caption
	\param[out] _sColumnName -- column name
	\return true if column name was getted successfully
	\sa IGridDataInterface::get_columnCaption(long _row,long _col,CString_& _sColumnName) const
	*/
	virtual bool get_columnCaption(long _row,long _col,CString_& _sColumnName) const
	{
		const CGridLayoutBase<Struct>& layout = GetGridLayout<Struct>();
		return layout.get_caption(_row,_col,_sColumnName);
	}

	/**\brief return column aligment
	This fucntion implements \ref IGridDataInterface::get_columnAlign(long _col,GridAlignEn& _align) const
	\param[in] _col -- column index
	\param[out] _align -- column aligment
	\return true if column alignment was getted successfully.
	\sa IGridDataInterface::get_columnAlign(long _col,GridAlignEn& _align) const
	*/
	virtual bool get_columnAlign(long _col,GridAlignEn& _align) const
	{
		const CGridLayoutBase<Struct>& layout = GetGridLayout<Struct>();
		return layout.get_align(_col,_align);
	}

	/**\brief return column default width (size)
	This function implements IGridDataInterface::get_columnSize(long _col) const
	\param[in] _col -- column index
	\return column default width (size)
	\sa IGridDataInterface::get_columnSize(long _col) const
	*/
	virtual long get_columnSize(long _col) const
	{
		const CGridLayoutBase<Struct>& layout = GetGridLayout<Struct>();
		long nColSize = 0;
		if(!layout.get_size(_col,nColSize)) return 0;
		return nColSize;
	}

	/**\brief return number of grid data rows.
	This function implements \ref IGridDataInterface::get_rowCount() const .
	\return number of rows (already loaded)
	\sa IGridDataInterface::get_rowCount() const .
	*/
	virtual long get_rowCount() const
	{
		try
		{
			CAutoTryLock __al(*this);
			return m_list.size();
		}
		catch(CantEnterCriticalSection)
		{
			return 0;
		}
	}

	/**\brief return grid data cell string
	This function implements \ref IGridDataInterface::get_string(long _row,long _col,CString_& _str) const
	\param[in] _row -- row index of grid cell to get data for
	\param[in] _col -- column index of grid cell to get data for
	\param[out] _str -- string of data for a specified cell
	\return true if cell data was successfully got and converted to string.
	\sa IGridDataInterface::get_string(long _row,long _col,CString_& _str) const
	*/
	virtual bool get_string(long _row,long _col,CString_& _str) const
	{
		try
		{
			CAutoTryLock __al(*this);
			_str.Empty();
			if(!is_exist(_row)) return false;
			typename _Container::const_iterator;
			_Container::const_iterator it = m_list.begin();
			std::advance(it,_row);
			const CGridLayoutBase<Struct>& layout = GetGridLayout<Struct>();
			return layout.get_string(_col,*it,_str);
		}
		catch(CantEnterCriticalSection)
		{
			_str.Empty();
			return false;
		}
	}
	
	/**\brief return column id.
	This function implements \ref IGridDataInterface::get_id(long _col,DWORD& _id) const
	\param[in] _col -- column index
	\param[out] _id -- column identifier
	\return true if column identifier was successfully got
	\sa IGridDataInterface::get_id(long _col,DWORD& _id) const
	*/
	virtual bool get_id(long _col,DWORD& _id) const
	{
		const CGridLayoutBase<Struct>& layout = GetGridLayout<Struct>();
		return layout.get_id(_col,_id);
	}

	/**\brief function to clear sort list (column list to sort by)
	This function implements \ref IGridDataInterface::clear_sortList()
	\sa IGridDataInterface::clear_sortList()
	*/
	virtual void clear_sortList()
	{
		try
		{
			CAutoTryLock __al(*this);
			m_opsort.clear();
		}
		catch(CantEnterCriticalSection)
		{
		}
	}

	/**\brief add column to the sort list.
	This function implements \ref IGridDataInterface::add_toSortList(long _col,bool _binvers)
	\param[in] _col -- column index to sort
	\param[in] _binvers -- flag to invers sort order for specified column
	\sa IGridDataInterface::add_toSortList(long _col,bool _binvers)
	*/
	virtual void add_toSortList(long _col,bool _binvers)
	{
		try
		{
			CAutoTryLock __al(*this);
			m_opsort.sort_by(_col,_binvers);
		}
		catch(CantEnterCriticalSection)
		{
		}
	}

	/**brief add column to sort list (by column identifier).
	This function implements \ref IGridDataInterface::add_FieldID2SortList(DWORD _id,bool _binvers)
	\param[in] _id -- column indentifier to sort
	\param[in] _binvers -- flag to invers sort order for specified column
	\sa IGridDataInterface::add_FieldID2SortList(DWORD _id,bool _binvers)
	*/
	virtual void add_FieldID2SortList(DWORD _id,bool _binvers)
	{
		try
		{
			CAutoTryLock __al(*this);
			const CGridLayoutBase<Struct>& layout = GetGridLayout<Struct>();
			long col = layout.find_id(_id);
			if(col>=0) m_opsort.sort_by(col,_binvers);
		}
		catch(CantEnterCriticalSection)
		{
		}
	}

	/**\brief sort grid data.
	This function implements \ref IGridDataInterface::sort_data()
	\sa IGridDataInterface::sort_data()
	*/
	virtual void sort_data()
	{
		try
		{
			CAutoTryLock __al(*this);
			m_opsort.sort(m_list);
		}
		catch(CantEnterCriticalSection)
		{
		}
	}

	/**\brief starts grid data retrying.
	This function implements \ref IGridDataInterface::start_retryData(bool _bstop)
	\param[in] _bstop -- flag to stop current grid data process retrying if it is
	           running in this function call time.
	\sa IGridDataInterface::start_retryData(bool _bstop)
	*/
	virtual void start_retryData(bool _bstop = false)
	{
		if(is_running()) 
		{
			if(_bstop) stop();
			else return;
		}
		m_bloaded = false;
		start();
	}

	/**\brief refresh grid data.
	*/
	void refresh()
	{
		start_retryData(true);
	}

	/**\brief gets grid header covered columns.
	This fucntion implements \ref IGridDataInterface::get_titleCoveredCells(RectLst& _covered) const
	\param[out] _covered -- header covered cells list.
	\return true if covered cells list is not empty.
	\sa IGridDataInterface::get_titleCoveredCells(RectLst& _covered) const
	*/
	virtual bool get_titleCoveredCells(RectLst& _covered) const
	{
		const CGridLayoutBase<Struct>& layout = GetGridLayout<Struct>();
		return layout.get_gridCovered(_covered);
	}

	/**\brief function to add data item to grid data
	\param[in] _item -- item to add
	\sa CGridData::insert(long _i,const Struct& _item)
	\sa CGridData::erase(long _i)
	\sa CGridData::find(const Struct& _item) const
	\sa CGridData::get_ptr()
	*/
	void push_back(const Struct& _item)
	{
		CAutoLock __al(*this);
		insert(m_list.size(),_item);
	}

	/**\brief fucntion to insert grid data item to grid data list at specified position.
	\param[in] _i -- index to insert before
	\param[in] _item -- data item to insert
	\sa CGridData::push_back(const Struct& _item)
	\sa CGridData::erase(long _i)
	\sa CGridData::find(const Struct& _item) const
	\sa CGridData::get_ptr()
	*/
	void insert(long _i,const Struct& _item)
	{
		CAutoLock __al(*this);
		_i = std::_cpp_min(_i,(long)m_list.size());
		_i = std::_cpp_max(_i,0l);
		_Container::const_iterator it = m_list.begin();
		std::advance(it,_i);
		m_list.insert(it,_item);
	}

	/**\brief erase grid data item at specified index.
	\param[in] _i -- row index to erase
	\return true if data was successfully erased
	\sa CGridData::insert(long _i,const Struct& _item)
	\sa CGridData::push_back(const Struct& _item)
	\sa CGridData::find(const Struct& _item) const
	\sa CGridData::get_ptr()
	*/
	bool erase(long _i)
	{
		CAutoLock __al(*this);
		if(!is_exist(_i)) return false;
		_Container::const_iterator it = m_list.begin();
		std::advance(it,_i);
		m_list.erase(it);
		return true;
	}

	/**\brief find item in the grid.
	\param[in] _item -- item to find
	\return true if item was successsfully founded
	\sa CGridData::insert(long _i,const Struct& _item)
	\sa CGridData::push_back(const Struct& _item)
	\sa CGridData::erase(long _i)
	\sa CGridData::get_ptr()
	*/
	bool find(const Struct& _item) const
	{
		CAutoLock __al(*this);
		_Container::const_iterator fit = std::find(m_list.begin(),m_list.end(),_item);
		return NEQL(m_list.end(),fit);
	}
	
	template<typename _SearchData>
		long find(const _SearchData& _data)
	{
		CAutoLock __al(*this);
		_Container::iterator fit = std::find(m_list.begin(),m_list.end(),_data);
		if(EQL(m_list.end(),fit)) return -1;
		return std::distance(m_list.begin(),fit);
	}

	template<typename _SearchData>
		long find(const _SearchData& _data) const
	{
		CAutoLock __al(*this);
		_Container::const_iterator fit = std::find(m_list.begin(),m_list.end(),_data);
		if(EQL(m_list.end(),fit)) return -1;
		return std::distance(m_list.begin(),fit);
	}

	/**\brief return grid data item field 
	\tparam _Type -- field type
	\param[in] _nrow -- row number
	\param[in] _pvar -- class field pointer to identity field to get
	\return field value
	*/
	template<typename _Type>
	_Type get(ROWCOL _nrow,_Type _Struct::* _pvar) const
	{
		CAutoLock __lock(*this);

		_Container::const_iterator it = m_list.begin();
		if(!is_exist(_nrow)) return _Type();
		std::advance(it,_nrow);
		return (*it).*_pvar;
	}

	/**\brief set grid data field value
	\tparam _Type -- field type
	\param[in] _nrow -- row number
	\param[in] _pvar -- grid data field class member pointer
	\param[in] _val - value to set
	*/
	template<typename _Type>
	void set(ROWCOL _nrow,_Type _Struct::* _pvar,const _Type& _val)
	{
		CAutoLock __lock(*this);

		_Container::iterator it = m_list.begin();
		if(!is_exist(_nrow)) return;
		std::advance(it,_nrow);
		(*it).*_pvar = _val;
	}

	/**\brief search for field value in grid data
	\param[in] _pvar -- grid data field class membre pointer
	\param[in] _val -- value to search for
	*/
	template<typename _Type>
	long find(_Type _Struct::* _pvar,const _Type& _val) const
	{
		CAutoLock __al(*this);

		_Container::const_iterator 
			it = m_list.begin()
			,ite = m_list.end()
			;

		long i=0;
		for(i=0;it!=ite;++it,i++)
		{
			if((*it).*_pvar == _val) return i;
		}
		return -1;
	}

	/**\brief grid size (row count)
	\return row count of grid data
	*/
	size_t size() const 
	{
		return get_rowCount();
	}
	
	bool empty() const
	{
		try
		{
			CAutoTryLock __al(*this);
			return m_list.empty();
		}
		catch(CantEnterCriticalSection)
		{
			return true;
		}
	}

	/**\brief return grid data item reference 
	\param _i -- row index to get data
	\return grid item constant reference
	*/
	const Struct& operator[] (long _i) const
	{
		CAutoLock __al(*this);
		if(!is_exist(_i)) return Fish<Struct>::get();
		_Container::const_iterator it = m_list.begin();
		std::advance(it,_i);
		return *it;
	}

	/**\brief return grid data item reference
	\param _i -- row index to get data
	\return grid item refernce
	*/
	Struct& operator[] (long _i)
	{
		CAutoLock __al(*this);
		if(!is_exist(_i)) return Fish<Struct>::get();
		_Container::iterator it = m_list.begin();
		std::advance(it,_i);
		return *it;
	}

	/**\brief return item pointer (constant)
	This function return grid item pointer if specified index is in the grid
	and NULL if index out of grid data range.
	\param[in] _nrow -- row index
	\return pointer to grid item  (constant) or NULL if index out of range.
	*/
	const Struct* get_ptr(ROWCOL _nrow) const
	{
		CAutoLock __al(*this);

		if(!is_exist(_nrow)) return NULL;
		_Container::const_iterator it = m_list.begin();
		std::advance(it,_nrow);
		return &*it;
	}

	/**\brief return item pointer.
	This function return grid item pointer if specified index is in the grid
	and NULL if index out of grid data range.
	\param[in] _nrow -- row index
	\return pointer to grid item or NULL if index out of range.
	*/
	Struct* get_ptr(ROWCOL _nrow)
	{
		return const_cast<Struct*>(
			const_cast<const CGridData*>(this)->get_ptr(_nrow)
			)
			;
	}

	/** return true if row in the grid items range.
	\param _nrow -- row index
	\return true if index in the grid.
	*/
	bool is_exist(long _nrow) const
	{
		CAutoLock __lock(*this);

		return _nrow>=0 && _nrow<m_list.size();
	}

	/**\brief return \ref CCriticalSection_ to use in grid data synchronization.
	*/
	operator CCriticalSection_& () {return m_critsect;}
	/**\brief return \ref CCriticalSection_ to use in grid data synchronization.
	*/
	operator const CCriticalSection_& () const {return m_critsect;}

	/**\brief save grid row id for letter use 
	This function helps to save and restore grid position after some operations
	that changes grid data item order.
	\param[in] _row -- row index to save.
	\return true if row id was saved successfully
	\sa IGridDataInterface::restore_savedrow(long& _row)
	*/
	virtual bool save_rowid(long _row)
	{
		CAutoLock __lock(*this);
		return RowIDSaveRestoreStrategy::save_rowid(_row,m_list);
	}

	/**\brief return row of saved id (see \ref IGridDataInterface::save_rowid(long _row) )
	\param[out] _row -- row index of saved row id
	\return true if row was successfully got.
	\sa IGridDataInterface::save_rowid(long _row)
	*/
	virtual bool restore_savedrow(long& _row)
	{
		CAutoLock __lock(*this);
		return RowIDSaveRestoreStrategy::restore_savedrow(_row,m_list);
	}

protected:
	_Container m_list;					///< drid data
	op_GridSort<Struct> m_opsort;		///< sort structure
	CSharedObject<CCriticalSection_> m_critsect;	///< synchronize object
	bool m_bloaded;						///< loaded flag
protected:
	HWND m_hNotifierWnd;				///< windows handle to notify
	UINT m_NotifyMsg;					///< notify message
	LPVOID m_lpParam;					///< notify param
};//template<> struct CGridData

/**\todo need sort refactoring as in ForexDeals class CForexClientGridData sort change pointers to data
*/

/**\brief Grid data class over container
\tparam _Container -- container type
*/
template<
	typename _Container
	,typename _SyncObject = NullType
	,typename _Struct = _Container::value_type
	>
struct CGridDataOnContainer 
:
	public IGridDataInterface						///< this class implement \ref IGridDataInterface interface
	,public CDefaultRowIDSaveRestore<_Struct>	///< row id save and restore strategy
{
public:
	typedef _Container Container;
	typedef CDefaultRowIDSaveRestore<_Struct> RowIDSaveRestoreStrategy;	///< typedef for row id save and restore strategy
protected:
	_Container* m_pcontainer;			///< container pointer
	_SyncObject* m_psyncobj;			///< syncronize object
	op_GridSort<_Struct> m_opsort;		///< sort structure
public:
	/**\brief Constructor.
	*/
	CGridDataOnContainer()
		:m_pcontainer(NULL)
		,m_psyncobj(NULL)
	{
	}

	/**\brief function that specify container fro this grid data
	\param _pcont -- container to set
	*/
	void set_data(_Container* _pcont)
	{
		VERIFY_EXIT(IS_NULL(m_pcontainer) && NOT_NULL(_pcont));
		m_pcontainer = _pcont;
	}

	/**\brief function to set syncronize object
	\tparam _Struct -- object type to get sync object from
	\param _struct -- object to set sync object from
	*/
	template<typename _Object>
		void set_syncobject(const _Object& _struct)
	{
		m_psyncobj = &get_syncobject(_struct,_SyncObject());
	}

	/**\brief Clear grid data. If data loading proccess is running 
	          Function try to stop it.
	This function is a implementation of \ref IGridDataInterface::clear().
	\sa IGridDataInterface::clear()
	*/
	virtual void clear()
	{
		CAutoLockT<_SyncObject> __lock(*m_psyncobj);

		VERIFY_EXIT(NOT_NULL(m_pcontainer));
		m_pcontainer->clear();
	}

	/**\brief return true if grid data was loaded 
	This function implements  \ref IGridDataInterface::is_dataReady() const
	\return true if data was loaded. 
	\sa IGridDataInterface::is_dataReady() const
	*/
	virtual bool is_dataReady() const
	{
		return true;
	}

	/**\brief return count of columns for this grid
	This function implements \ref IGridDataInterface::get_columnCount() const
	\return number of columns for this grid.
	\sa IGridDataInterface::get_columnCount() const
	*/
	virtual long get_columnCount() const
	{
		const CGridLayoutBase<_Struct>& layout = GetGridLayout<_Struct>();
		return layout.get_columnsCount();
	}

	/**\brief return number of grid data rows
	This function implements \ref IGridDataInterface::get_titleRowsCount() const
	\return number of grid data rows
	\sa \ref IGridDataInterface::get_titleRowsCount() const
	*/
	virtual long get_titleRowsCount() const
	{
		const CGridLayoutBase<_Struct>& layout = GetGridLayout<_Struct>();
		return layout.get_titlerows();
	}

	/**\brief return column name for a specified header position
	This function impements \ref IGridDataInterface::get_columnCaption(long _row,long _col,CString_& _sColumnName) const
	\param[in] _row -- row number to get caption
	\param[in] _col -- column number to get caption
	\param[out] _sColumnName -- column name
	\return true if column name was getted successfully
	\sa IGridDataInterface::get_columnCaption(long _row,long _col,CString_& _sColumnName) const
	*/
	virtual bool get_columnCaption(long _row,long _col,CString_& _sColumnName) const
	{
		const CGridLayoutBase<_Struct>& layout = GetGridLayout<_Struct>();
		return layout.get_caption(_row,_col,_sColumnName);
	}

	/**\brief return column aligment
	This fucntion implements \ref IGridDataInterface::get_columnAlign(long _col,GridAlignEn& _align) const
	\param[in] _col -- column index
	\param[out] _align -- column aligment
	\return true if column alignment was getted successfully.
	\sa IGridDataInterface::get_columnAlign(long _col,GridAlignEn& _align) const
	*/
	virtual bool get_columnAlign(long _col,GridAlignEn& _align) const
	{
		const CGridLayoutBase<_Struct>& layout = GetGridLayout<_Struct>();
		return layout.get_align(_col,_align);
	}

	/**\brief return column default width (size)
	This function implements IGridDataInterface::get_columnSize(long _col) const
	\param[in] _col -- column index
	\return column default width (size)
	\sa IGridDataInterface::get_columnSize(long _col) const
	*/
	virtual long get_columnSize(long _col) const
	{
		const CGridLayoutBase<_Struct>& layout = GetGridLayout<_Struct>();
		long nColSize = 0;
		if(!layout.get_size(_col,nColSize)) return 0;
		return nColSize;
	}

	/**\brief return number of grid data rows.
	This function implements \ref IGridDataInterface::get_rowCount() const .
	\return number of rows (already loaded)
	\sa IGridDataInterface::get_rowCount() const .
	*/
	virtual long get_rowCount() const
	{
		try
		{
			CAutoTryLockT<_SyncObject> __lock(*m_psyncobj);
			VERIFY_EXIT1(NOT_NULL(m_pcontainer),0);
			return m_pcontainer->size();
		}
		catch(CantEnterCriticalSection)
		{
			return 0;
		}
	}

	/**\brief return grid data cell string
	This function implements \ref IGridDataInterface::get_string(long _row,long _col,CString_& _str) const
	\param[in] _row -- row index of grid cell to get data for
	\param[in] _col -- column index of grid cell to get data for
	\param[out] _str -- string of data for a specified cell
	\return true if cell data was successfully got and converted to string.
	\sa IGridDataInterface::get_string(long _row,long _col,CString_& _str) const
	*/
	virtual bool get_string(long _row,long _col,CString_& _str) const
	{
		try
		{
			CAutoTryLockT<_SyncObject> __lock(*m_psyncobj);
			_str.Empty();
			VERIFY_EXIT1(NOT_NULL(m_pcontainer),false);
			if(!is_exist(_row)) return false;
			typename _Container::const_iterator;
			_Container::const_iterator it = m_pcontainer->begin();
			std::advance(it,_row);
			const CGridLayoutBase<_Struct>& layout = GetGridLayout<_Struct>();
			return layout.get_string(_col,*it,_str);
		}
		catch(CantEnterCriticalSection)
		{
			return false;
		}
	}
	
	/**\brief return column id.
	This function implements \ref IGridDataInterface::get_id(long _col,DWORD& _id) const
	\param[in] _col -- column index
	\param[out] _id -- column identifier
	\return true if column identifier was successfully got
	\sa IGridDataInterface::get_id(long _col,DWORD& _id) const
	*/
	virtual bool get_id(long _col,DWORD& _id) const
	{
		const CGridLayoutBase<_Struct>& layout = GetGridLayout<_Struct>();
		return layout.get_id(_col,_id);
	}

	/**\brief function to clear sort list (column list to sort by)
	This function implements \ref IGridDataInterface::clear_sortList()
	\sa IGridDataInterface::clear_sortList()
	*/
	virtual void clear_sortList()
	{
		VERIFY_EXIT(NOT_NULL(m_pcontainer));
		m_opsort.clear();
	}

	/**\brief add column to the sort list.
	This function implements \ref IGridDataInterface::add_toSortList(long _col,bool _binvers)
	\param[in] _col -- column index to sort
	\param[in] _binvers -- flag to invers sort order for specified column
	\sa IGridDataInterface::add_toSortList(long _col,bool _binvers)
	*/
	virtual void add_toSortList(long _col,bool _binvers)
	{
		m_opsort.sort_by(_col,_binvers);
	}

	/**brief add column to sort list (by column identifier).
	This function implements \ref IGridDataInterface::add_FieldID2SortList(DWORD _id,bool _binvers)
	\param[in] _id -- column indentifier to sort
	\param[in] _binvers -- flag to invers sort order for specified column
	\sa IGridDataInterface::add_FieldID2SortList(DWORD _id,bool _binvers)
	*/
	virtual void add_FieldID2SortList(DWORD _id,bool _binvers)
	{
		const CGridLayoutBase<_Struct>& layout = GetGridLayout<_Struct>();
		long col = layout.find_id(_id);
		if(col>=0) m_opsort.sort_by(col,_binvers);
	}

	/**\brief sort grid data.
	This function implements \ref IGridDataInterface::sort_data()
	\sa IGridDataInterface::sort_data()
	*/
	virtual void sort_data()
	{
		try
		{
			CAutoTryLockT<_SyncObject> __lock(*m_psyncobj);
			VERIFY_EXIT(NOT_NULL(m_pcontainer));
			m_opsort.sort(*m_pcontainer);
		}
		catch(CantEnterCriticalSection)
		{
			return;
		}
	}

	/**\brief starts grid data retrying.
	This function implements \ref IGridDataInterface::start_retryData(bool _bstop)
	\param[in] _bstop -- flag to stop current grid data process retrying if it is
	           running in this function call time.
	\sa IGridDataInterface::start_retryData(bool _bstop)
	*/
	virtual void start_retryData(bool _bstop = false)
	{
		ASSERT_(FALSE); // you shoul not to call this function
	}

	/**\brief gets grid header covered columns.
	This fucntion implements \ref IGridDataInterface::get_titleCoveredCells(RectLst& _covered) const
	\param[out] _covered -- header covered cells list.
	\return true if covered cells list is not empty.
	\sa IGridDataInterface::get_titleCoveredCells(RectLst& _covered) const
	*/
	virtual bool get_titleCoveredCells(RectLst& _covered) const
	{
		const CGridLayoutBase<_Struct>& layout = GetGridLayout<_Struct>();
		return layout.get_gridCovered(_covered);
	}
	/** return true if row in the grid items range.
	\param _nrow -- row index
	\return true if index in the grid.
	*/
	bool is_exist(long _nrow) const
	{
		try
		{
			CAutoTryLockT<_SyncObject> __lock(*m_psyncobj);
			VERIFY_EXIT1(NOT_NULL(m_pcontainer),false);
			return _nrow>=0 && _nrow<m_pcontainer->size();
		}
		catch(CantEnterCriticalSection)
		{
			return false;
		}
	}

	/**\brief save grid row id for letter use 
	This function helps to save and restore grid position after some operations
	that changes grid data item order.
	\param[in] _row -- row index to save.
	\return true if row id was saved successfully
	\sa IGridDataInterface::restore_savedrow(long& _row)
	*/
	virtual bool save_rowid(long _row)
	{
		CAutoLockT<_SyncObject> __lock(*m_psyncobj);

		VERIFY_EXIT1(NOT_NULL(m_pcontainer),false);
		return RowIDSaveRestoreStrategy::save_rowid(_row,*m_pcontainer);
	}

	/**\brief return row of saved id (see \ref IGridDataInterface::save_rowid(long _row) )
	\param[out] _row -- row index of saved row id
	\return true if row was successfully got.
	\sa IGridDataInterface::save_rowid(long _row)
	*/
	virtual bool restore_savedrow(long& _row)
	{
		CAutoLockT<_SyncObject> __lock(*m_psyncobj);

		VERIFY_EXIT1(NOT_NULL(m_pcontainer),false);
		return RowIDSaveRestoreStrategy::restore_savedrow(_row,*m_pcontainer);
	}

	/**\brief implementation of \ref IGridDataInterface::initialize_notifier(HWND _hwnd,UINT _msg,LPVOID _lpParam)
	Save notify information.
	\param[in] _hwnd -- window handle to send notify message to.
	\param[in] _msg -- notify message to send
	\param[in] _lpParam -- data to send with notify message
	*/
	virtual void initialize_notifier(HWND _hwnd,UINT _msg,LPVOID _lpParam)
	{
		//ASSERT_(FALSE); // you should not call to this function 
	}
};//template<> struct CGridDataOnContainer 

//@}

/**\page Page_QuickStart_GridTemplates Quick start: "Creating grid data".

Template \ref CGridData support you simple way to create grid data. You need 
just to:
	-# Create you class of grid data.
	-# Derive it from \ref CGridData 
	   (may be specify identity field in derived class constructor)
	-# reload \ref CGridData::prepare_call() function.
	.

You can see simple exampl of this steps next.

\code

// declare grid data class
typedef CGridData<CClientRow,PriorSqlSwitch> CClientGridDataBase;	/// define Grid Data base class

struct CClientsGridData : public CClientGridDataBase	// create you own class and derive from grid data base
{
	CClientsGridData()
	{
		set_idfield(&CClientRow::m_fClientID);		/// this will be used to restore cursor position in grid
													/// after some operations
	}

	virtual bool prepare_call(PriorSqlType& _sql)	/// reload \ref CGridData::prepare_call() function
	{
		CAutoLock __lock(*this);

		_sql.CreateConnect();
		_sql.SetProcCmd(_T("eBankClientsList"));
		return true;
	}
};//struct CClientsGridData

\endcode

Now show simple usage of the grid data class (not in grid)

\code
struct get_client_data()
{
	CClientsGridData data;
	data.refresh();
	data.wait();
	std::cout << _T("Loaded ") << data.get_rowCount() 
		<< _T(" rows") << std::endl
		;

	long i = 0;
	for(i=0;i<data.get_rowCount();i++)
	{
		CAutoLock __lock(data);
		const CClientRow& row = data[i];
		std::cout << _T("row[") << i << _T("] = ")	// think we have operator << (_StreamType& stream,const CClientRow& _row);
			<< row << std::endl
			;
	}
}

\endcode

Also you can create you grid data over some container (see \ref CGridDataOnContainer). 
This is the class that support just view of data in grid, but not a refresh or load.
So if you have some data you need to view in grid use \ref CGridDataOnContainer to create
grid data from some type of container (std::vector std::list) just to view container data
in grid (of cause you should declare layout for container item type).

Now show usage in grid (using grid class \ref CBaseGridOnTemplWnd).

\code
// h file

class CClientsWnd : public CBaseGridOnTemplWnd
{
private:
	typedef CBaseGridOnTemplWnd base;
// Construction
public:
	CClientsWnd(){};

// Attributes
public:
protected:
	CClientsGridData m_griddata;

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientsWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CClientsWnd(){};

public:
	void initialize(UINT _id,CWnd* _pParent);
protected:
	virtual void OnGridDataLoaded(bool _bGridDataReady);
	// Generated message map functions
protected:
	//{{AFX_MSG(CClientsWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};//class CClientsWnd

// cpp file

void CClientsWnd::initialize(UINT _id,CWnd* _pParent)
{
	base::initialize(&m_griddata,_id,_pParent);	
}

void CClientsWnd::OnGridDataLoaded(bool _bGridDataReady)
{
	if(!_bGridDataReady) return;
	GetParent()->SendMessage(WUM_DATALOADED);
}

\endcode

And then you just need place in CFormView of into CDialog this window.



See also \ref Page_QuickStart_GridLayout \n
See also \ref Page_QuickStart_CBaseGridOnTempl \n
See also \ref Page_QuickStart_UtilitiesAttributes \n

*/



#endif//#if !defined(__GRIDTEMPLATES_H__4F6C3B42_38A1_47E1_91E1_3C1C3D48E3F8)
