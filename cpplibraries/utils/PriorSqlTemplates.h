#if !defined(__PRIORSQLTEMPLATES_H__EF2970ED_08F3_4378_8F10_84B76D15D784)
#define __PRIORSQLTEMPLATES_H__EF2970ED_08F3_4378_8F10_84B76D15D784

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/*
	это не нормальна€ библитека какой бы она могла быть, а только тот минимальный 
	набор, который можно реализовать в MSVC 5.0
	в, уже, MSVC 6.0 можно сделать данную библитеку очень гибкой и удобной
*/

//#pragma warning(push)

#pragma warning(disable:4786)

#include "config.h"
#include "atlmfc.h"

#include <list>
#include <algorithm>
#include <xutility>
#include <xmemory>

#include "utils.h"
#include "tracealloc.h"
#include "nulltype.h"
#include "attributes.h"
#include "convert.h"
#include "synchronize.h"
#include "errors.h"

/**\defgroup PriorSqlTemplates Utilities of Data Bases support.
*/
//@{


static const long g_SqlOk = 1;	///< sql procedure success flag

/**\brief return true if return value of PriorSql ok
\return true if return value of PriorSql ok
\sa sql_failed()
\sa sql_success()
*/
inline bool sql_success(int _ret)
{
	return EQL(g_SqlOk,_ret);
}

/**\brief return true if PriorSql object was executed successfully
\tparam _PriorSql -- DB access class (ST)
\param _sql -- DB access object
\return true if PriorSql object was executed successfully
\sa sql_failed()
\sa sql_success()
*/
template<typename _PriorSql>
inline bool sql_success(_PriorSql& _sql)
{
	return sql_success(_sql.GetReturnValue());
}

/**\brief return true if return value is failed
\param _ret -- return value from PriorSql object
\sa sql_failed()
\sa sql_success()
*/
inline bool sql_failed(int _ret)
{
	return !sql_success(_ret);
}

/**\brief return true if sql call was failed
\tparam _PriorSql -- DB access class (ST)
\param _sql -- _PriorSql object to verify state of
\sa sql_failed()
\sa sql_success()
*/
template<typename _PriorSql>
inline bool sql_failed(_PriorSql& _sql)
{
	return sql_failed(_sql.GetReturnValue());
}

/**\brief function that return extended error information
\tparam _PriorSql -- DB access class (ST)
\param[in] _ncode -- return code to get error inforamtion for
\param[out] _message -- error string from code
\param[in] _bwithcode -- optional flag to add to error message a code value.
\return true if errror message was getted successfully.
*/
template<typename _PriorSql> inline
bool sql_get_errormessage(long _ncode,CString_& _message,bool _bwithcode=false)
{
	TCHAR buf[512]; //XXX
	ZeroObj(buf);
	_PriorSql sql;
	sql.CreateConnect();
	sql.SetProcCmd(_T("ErrorsSelect"));
	sql.SetParam(_T("@ErrNo"),&_ncode);
	if(sql.Send())
	{
		while(sql.Exec())
		{
			if(sql.GetResult()==CS_ROW_RESULT)
			{
				sql.Flush();
				sql.Bind(buf,sizea(buf));
				while(sql.Fetch()) {}
			}
		}
	}
	if(sql_failed(sql.GetReturnValue())) return false;
	_message.Empty();
	if(_bwithcode) _message = format(_T("%d -- \""),_ncode);
	_message += buf;
	if(_bwithcode) _message +=_T("\"");	
	return true;
}


/**\brief function that return extended error information
\tparam _PriorSql -- DB access class (ST)
\param[in] _sql -- sql connection to verify
\param[out] _message -- error string from code
\param[in] _bwithcode -- optional flag to add to error message a code value.
\return true if errror message was getted successfully.
*/
template<typename _PriorSql> inline
bool sql_get_errormessage(_PriorSql& _sql,CString_& _message,bool _bwithcode=false)
{
	long ncode = _sql.GetReturnValue();
	TCHAR buf[512]; //XXX
	ZeroObj(buf);
	if(_sql.IsADO())
	{
		_sql.GetReturnMess(buf);			
	}
	else
	{
		_sql.SetProcCmd(_T("ErrorsSelect"));
		_sql.SetParam(_T("@ErrNo"),&ncode);
		if(_sql.Send())
		{
			while(_sql.Exec())
			{
				if(_sql.GetResult()==CS_ROW_RESULT)
				{
					_sql.Flush();
					_sql.Bind(buf,sizea(buf));
					while(_sql.Fetch()) {}
				}
			}
		}
		if(sql_failed(_sql.GetReturnValue())) return false;
	}
	_message.Empty();
	if(_bwithcode) _message = format(_T("%d -- \""),ncode);
	_message += buf;
	if(_bwithcode) _message +=_T("\"");	
	return true;
}

static const LPCTSTR g_szPriorSqlErrorCode = _T("PriorSqlErrorCode");
static const LPCTSTR g_szPriorSqlErrorMessage = _T("PriorSqlErrorMessage");

template<typename _PriorSql>
	struct CPriorSqlException : public utils::CUtilitiesExceptionBase
{
	CPriorSqlException(
		LPCTSTR _szSourceInfo
		,_PriorSql& _sql
		)
		:utils::CUtilitiesExceptionBase(_szSourceInfo)
	{
		long nErrorCode = _sql.GetReturnValue();
		CString sErropMessage;
		sql_get_errormessage(_sql,sErropMessage);
		utils::CErrorTag __ets;
		__ets
			<< cnamedvalue(g_szPriorSqlErrorCode,nErrorCode)
			<< cnamedvalue(g_szPriorSqlErrorMessage,sErropMessage);
			;
		initialize_exception();
	}

	CPriorSqlException(
		LPCTSTR _szSourceInfo
		,long _nSqlErrorCode
		)
		:utils::CUtilitiesExceptionBase(_szSourceInfo)
	{
		CString sErropMessage;
		sql_get_errormessage(_nSqlErrorCode,sErropMessage);
		utils::CErrorTag __ets;
		__ets
			<< cnamedvalue(g_szPriorSqlErrorCode,_nSqlErrorCode)
			<< cnamedvalue(g_szPriorSqlErrorMessage,sErropMessage);
			;
		initialize_exception();
	}
};//template<> struct CPriorSqlException

template<typename _PriorSql> inline 
	void throw_priorsql_exceptionF(LPCTSTR _szSourceInfo,_PriorSql& _sql)
{
#if defined(USE_EXCEPTION_REFS)
		throw CPriorSqlException<_PriorSql>(_szSourceInfo,_sql);
#else
		throw trace_alloc(
			new CPriorSqlException<_PriorSql>(_szSourceInfo,_sql);
			);
#endif
}

template<typename _PriorSql> inline 
	void check_priorsql_error_throwF(LPCTSTR _szSourceInfo,_PriorSql& _sql)
{
	if(sql_success(_sql)) return;
#if defined(USE_EXCEPTION_REFS)
		throw CPriorSqlException<_PriorSql>(_szSourceInfo,_sql);
#else
		throw trace_alloc(
			new CPriorSqlException<_PriorSql>(_szSourceInfo,_sql);
			);
#endif
}

// template<typename _PriorSql> inline 
// 	void throw_priorsql_exceptionF(LPCTSTR _szSourceInfo,long _nSqlErrorCode,_PriorSql)
// {
// #if defined(USE_EXCEPTION_REFS)
// 		throw CPriorSqlException<_PriorSql>(_szSourceInfo,_nSqlErrorCode);
// #else
// 		throw trace_alloc(
// 			new CPriorSqlException<_PriorSql>(_szSourceInfo,_nSqlErrorCode);
// 			);
// #endif
// }
// 
// template<typename _PriorSql> inline 
// 	void check_priorsql_error_throwF(LPCTSTR _szSourceInfo,long _nSqlErrorCode,_PriorSql)
// {
// 	if(sql_success(_nSqlErrorCode)) return;
// #if defined(USE_EXCEPTION_REFS)
// 		throw CPriorSqlException<_PriorSql>(_szSourceInfo,_nSqlErrorCode);
// #else
// 		throw trace_alloc(
// 			new CPriorSqlException<_PriorSql>(_szSourceInfo,_nSqlErrorCode);
// 			);
// #endif
// }

#define throw_priorsql_exception(_sql)	\
	throw_priorsql_exceptionF(CREATE_SOURCEINFO(),_sql)

#define check_priorsql_error_throw(_sql)	\
	check_priorsql_error_throwF(CREATE_SOURCEINFO(),_sql)


/**\brief Utilities labrary inner class used to create buffers for stream 
		  or output params. 
*/
struct CSqlTempBuffer
{
	/**\brief Constructor.
	*/
	CSqlTempBuffer():m_pbuf(NULL),m_size(0),m_pos(0)
	{
	}

	/**\brief Destructor.
	*/
	~CSqlTempBuffer()
	{
		free();
	}

	/**\brief to update all datas connected to the buffer
	*/
	void restart()
	{
		m_pos = 0;
	}

	/**\brief update one param.
	Retry data from buffer.
	\tparam _Type -- type of variable to get from sql buffer.
	\param _data -- object to copy data to.
	*/
	template<typename _Type>
		void update(_Type& _data)
	{
		VERIFY_EXIT(NOT_NULL(m_pbuf));
		VERIFY_EXIT(m_pos+sizeof(_Type)<=m_size && m_pos>=0);
		_data = *reinterpret_cast<_Type*>(m_pbuf + m_pos);
		m_pos += sizeof(_Type);
	}

	/**\brief update string param (there is string max legth for string params)
	\param[out] _data -- string to update
	\param[in] _size -- max length of string buffer.
	*/
	void update(CString_& _data,long _size)
	{
		VERIFY_EXIT(NOT_NULL(m_pbuf));
		VERIFY_EXIT(m_pos+sizeof(TCHAR)*(_size+1)<=m_size && m_pos>=0);
		long len = 0;
		for(len=0;len<_size && (unsigned char)*(m_pbuf + m_pos + len)!=0;len++){};
		_data = (LPCTSTR)CString_(reinterpret_cast<LPCTSTR>(m_pbuf + m_pos),len);
		m_pos += sizeof(TCHAR)*(_size+1);
	}

	/**\clear \ref CSqlTempBuffer bjects (remove all buffers)
	*/
	void clear()
	{
		free();
		m_size = 0;
	}

	/**\brief used to calculate needed size for buffer.
	This fucntion adds size of variable of type \ref _Type
	\tparam _Type
	\param used to select type realization
	*/
	template<typename _Type>
		void prepare_add(_Type&)
	{
		m_size += sizeof(_Type);
	}

	/**\brief used to calculate needed size for buffer.
	This function adds to buffer length max size of string.
	*/
	void prepare_add(CString_&,long _size)
	{
		m_size += sizeof(TCHAR)*(_size+1);
	}

	/**\brief creates buffer of allocated before size
	*/
	void create_buffer()
	{
		free();
		VERIFY_EXIT(m_size>=0);
		if(EQL(m_size,0)) return;
		m_pbuf = trace_alloc(new BYTE[m_size]);
		ZeroMemory(m_pbuf,m_size);
		restart();
	}

	/**\brief template function to add some data to buffer
	\tparam _Type -- type of data to add
	\param _data -- data that value is used to initialize allocated data item.
	\return pointer to added data, that can be used for Bind() or SetParam() operations
	*/
	template<typename _Type>
		_Type* add(_Type& _data)
	{
		VERIFY_EXIT1(NOT_NULL(m_pbuf),NULL);
		VERIFY_EXIT1(m_pos+sizeof(_Type)<=m_size && m_pos>=0,NULL);
		_Type* newptr = new (static_cast<LPVOID>(m_pbuf+m_pos)) _Type(_data);
		m_pos += sizeof(_Type);
		return newptr; 
	}

	/**\brief fauntion that return buffer for string.
	\param _str -- string that data is used to initialize buffer 
	\param _size -- maximum length of string 
	\return string buffer (LPTSTR) to use in Bind() or in SetParam() fucntions.
	*/
	LPTSTR add(CString_& _str,long _size)
	{
		VERIFY_EXIT1(NOT_NULL(m_pbuf),NULL);
		VERIFY_EXIT1(m_pos+sizeof(TCHAR)*(_size+1)<=m_size && m_pos>=0,NULL);
		LPTSTR newptr = reinterpret_cast<LPTSTR>(m_pbuf+m_pos);;
		lstrcpyn(newptr,_str,std::_cpp_min((int)_size,_str.GetLength()));
		m_pos += sizeof(TCHAR)*(_size+1);
		return newptr;
	}

	/**\brief return value from buffer
	\tparam _Type -- type of data to get
	\return value from buffer of specified type
	*/
	template<typename _Type>
		_Type* get(_Type&)
	{
		VERIFY_EXIT1(NOT_NULL(m_pbuf),NULL);
		VERIFY_EXIT1(m_pos+sizeof(_Type)<=m_size && m_pos>=0,NULL);
		_Type* newptr = reinterpret_cast<_Type*>(m_pbuf+m_pos);
		m_pos += sizeof(_Type);
		return newptr;
	}

	/**\brief function to get string data from buffer
	\param _str -- not used
	\param _size -- specifies string maximum length
	\return string value from buffer
	*/
	LPTSTR get(CString_& _str,long _size)
	{
		VERIFY_EXIT1(NOT_NULL(m_pbuf),NULL);
		VERIFY_EXIT1(m_pos+sizeof(TCHAR)*(_size+1)<=m_size && m_pos>=0,NULL);
		LPTSTR newptr = reinterpret_cast<LPTSTR>(m_pbuf+m_pos);
		m_pos += sizeof(TCHAR)*(_size+1);
		return newptr;
	}

	/**\brief remove from buffer item of specified type
	\tparam _Type -- buffer item type
	*/
	template<typename _Type>
		void remove(_Type&)
	{
		VERIFY_EXIT(NOT_NULL(m_pbuf));
		VERIFY_EXIT(m_pos+sizeof(_Type)<=m_size && m_pos>=0);
		_DESTRUCTOR(_Type,reinterpret_cast<_Type*>(m_pbuf+m_pos));
		ZeroMemory(m_pbuf+m_pos,sizeof(_Type));
		m_pos += sizeof(_Type);
	}

	/**\brief remove string from buffer
	\param _size -- size of string buffer.
	*/
	void remove(CString_&,long _size)
	{
		VERIFY_EXIT(NOT_NULL(m_pbuf));
		VERIFY_EXIT(m_pos+sizeof(TCHAR)*(_size+1)<=m_size && m_pos>=0);
		ZeroMemory(m_pbuf+m_pos,sizeof(TCHAR)*_size);
		m_pos += sizeof(TCHAR)*(_size+1);
	}
protected:
	/**\brief frees data of the buffer
	*/
	void free()
	{
		if(NOT_NULL(m_pbuf)) delete trace_free(m_pbuf);
		m_pbuf = NULL;
	}
protected:
	long m_size;		///< size of buffer
	long m_pos;			///< current position in buffer
	LPBYTE m_pbuf;		///< buffer 
private:
	CSqlTempBuffer(const CSqlTempBuffer& _);
	CSqlTempBuffer& operator = (const CSqlTempBuffer& _);
};//struct CSqlTempBuffer

/**\brief class to escape verification of params before operations
*/
struct CSqlNoVerifyItem
{
	/**\brief function return true if you available to set this param
	\tparam _ItemType - type of item to verify
	\return true if you can set this param (for this class this function return true always)
	*/
	template<typename _ItemType>
		bool can_setparam(const _ItemType&) const
	{
		return true;
	}
};//struct CSqlNoVerifyItem

/**\brief this functor allow to set params if they are not empty.
*/
struct CSqlVerifyItemIfParamEmpty
{
	/**\brief return true if param not equal to it default value.
	          is not equal to value of item from default constructor 
			  (_ItemType()).
	\tparam _ItemType -- item type to verify
	\param _ -- param value to verify
	\return true if this param don`t have a default value.
	*/
	template<typename _ItemType>
		bool can_setparam(const _ItemType& _) const
	{
		return NEQL(_,_ItemType());
	}

	/**\brief function specialization for CString_ data
	\param _s -- string param to verify
	\return true if this string is not empty
	*/
	bool can_setparam(const CString_& _s) const
	{
		CString_ s = _s;
		s.TrimLeft();
		s.TrimRight();
		return !s.IsEmpty();
	}

	/**\brief function specialization for Date class data
	\param _ -- date param to verify
	\return true if this date is not empty
	*/
	bool can_setparam(const Date& _) const
	{
		return NEQL(_,Date(0l))!=0;
	}
};//struct CSqlVerifyItemIfParamEmpty

/**\brief interface for prior sql layout item
\tparam _Struct -- data struct of sql layout item
\tparam _PriorSql -- class to access to Data Base (ST)
*/
template<typename _Struct,typename _PriorSql>
interface ISqlLayoutItem
{
	/**\brief declare virtual destructor. for correct data delete in derived classes.
	*/
	virtual ~ISqlLayoutItem(){}
	//virtual bool can_setparam(_Struct& _struct) const = 0;
	/**\brief set this parameter to data base access class
	\param _sNamePrefix -- name prefix
	\param _struct -- data struct to set item data for
	\param _sql -- data base access class to set param to
	\return true if data was successfully set.
	*/
	virtual bool set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql) const = 0;
	/**\brief function to set param with \ref CSqlTempBuffer (for need of output params (CS_RETURN))
	\param _sNamePrefix - prefix of the name 
	\param _struct -- data struct to set item data for
	\param _sql -- DB access class(ST)
	\param _tmpbuf -- temp buffer
	\return true if data was successfully set.
	*/
	virtual bool set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf) const = 0;
	/**\brief function to bind data
	\param _struct -- data struct to bind data for 
	\param _sql - DB access class 
	\param _tmpbuf -- temp buffer
	\param _bfirst -- if the buffer need to be created. At second time buffer already exists.
	\return true if parametr was successfully set.
	*/
	virtual bool bind(_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf,bool _bfirst = false) const = 0;
	/**\brief prepare buffer for this param item
	\param _struct -- struct to prepare item data for
	\param _tmpbuf -- temp buffer
	\return true if data buffer was successfully prepared
	*/
	virtual bool prepare_buffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const = 0;
	/**\brief update item data from buffer. Need after every Fetch() function call.
	\param _struct -- sql data to update item data to
	\param _tmpbuf -- temp buffer
	\return true if data was successfully updated
	*/
	virtual bool update_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const = 0;
	/**\brief remove data item from buffer
	\param _struct -- data to remove item 
	\param _tmpbuf -- temp buffer
	\return true if data was successfully removed
	*/
	virtual bool remove_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const = 0;
};//template<> interface ISqlLayoutItem

/**\brief param direction (param types)
*/
enum SqlParamDirectionEn{
	SPD_Null= 0	
	,SPD_Input				///< param is of an input type
	,SPD_InputOutput		///< param is of an input and an output type
	,SPD_Output				///< param is of an output type
};

/**\brief function to convert from \ref SqlParamDirectionEn parameter type to 
          PrioSql direction type.
\param _dir -- \ref SqlParamDirectionEn parameter type
\return PriorSql direction value
*/
inline
int get_sybase_direction(SqlParamDirectionEn _dir)
{
	switch(_dir)
	{
	case SPD_Input:
		return CS_INPUTVALUE;
	case SPD_InputOutput:
	case SPD_Output:
		return CS_RETURN;
	default: 
		ASSERT_(FALSE);
		return 0;
	}
	ASSERT_(FALSE);
	return 0;
}

//namespace typedetail
//{

//common implementation
/**\brief inner class to specify PriorSql functions
\tparam _Type -- data type for function
*/
template<typename _Type>
struct sqlfuncs
{
	/**\brief SetParam specialization
	\tparam _PriorSql -- Data Base access class
	\param _sql -- data base access object
	\param _sName -- parameter name
	\param _pdata -- parameter value
	\param _dir -- parameter direction type
	*/
	template<typename _PriorSql>
	static bool SqlSetParam(_PriorSql& _sql,const CString_& _sName,const _Type* _pdata,SqlParamDirectionEn _dir) 
	{
		_sql.SetParam(_sName,const_cast<_Type*>(_pdata),get_sybase_direction(_dir));
		return true;
	}

	/**\brief bind specialization
	\tparam _PriorSql -- data base access class
	\param _sql -- data base access object
	\param _pdata -- data to Bind
	*/
	template<typename _PriorSql>
	static bool SqlBind(_PriorSql& _sql,_Type* _pdata)
	{
		_sql.Bind(_pdata);
		return true;
	}

};

/**\brief inner class, string specification for sql functions 
*/
template<>
struct sqlfuncs<LPTSTR>
{
	/**\brief set param specification for strings
	\tparam _PriorSql -- data base access class
	\param _sql -- data access object
	\param _sName -- parameter name to set
	\param _data -- string data to set
	\param _dir -- paramenetr data direction
	\return true if data was successfully set
	*/
	template<typename _PriorSql>
	static bool SqlSetParam(_PriorSql& _sql,const CString_& _sName,LPTSTR _data,SqlParamDirectionEn _dir) 
	{
		_sql.SetParam(_sName,_data,get_sybase_direction(_dir));
		return true;
	}

	/**\brief bind param specification for string
	\tparam _PriorSql -- data base access class
	\param _sql -- data access object
	\param _buf -- string buffer to bind
	\param _maxlen -- maximum length of string buffer to bind
	\return true if string buffer was successfully bound
	*/
	template<typename _PriorSql>
	static bool SqlBind(_PriorSql& _sql,LPTSTR _buf,long _maxlen)
	{
		_sql.Bind(_buf,_maxlen);
		return true;
	}
};

//};//namespace typedetail

/**\brief implementation of \ref ISqlLayoutItem interface.
This class is a simple and generalized implementation of \ref ISqlLayoutItem interface
for most data types. 
\tparam _Struct -- item member field data structure
\tparam _PriorSql -- data base access type
\tparam _ItemType -- member field type
\tparam _ParamVerifier -- verifier type
*/
template<typename _Struct,typename _PriorSql,typename _ItemType,typename _ParamVerifier>
struct CSqlLayoutItem : public ISqlLayoutItem<_Struct,_PriorSql>
{
	/**\brief Constructor.
	\param _sName -- sql layout item name
	\param _pmember -- structure member field pointer
	\param _paramverifier -- param verifier object
	*/
	CSqlLayoutItem(const CString_& _sName
		,_ItemType _Struct::* _pmember
		,const _ParamVerifier& _paramverifier
		)
		:m_sName(_sName),m_pmember(_pmember)
		,m_paramverifier(_paramverifier)
	{
	}

	/**\brief function that implements \ref ISqlLayoutItem::set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql) const
	\param _sNamePrefix -- name prefix 
	\param _struct -- data struct to set param from
	\param _sql -- data base access object
	\return true if parameter was successfully set
	\sa ISqlLayoutItem::set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql) const
	*/
	virtual bool set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql) const
	{
		VERIFY_EXIT1(NOT_NULL(m_pmember),false);
		if(!m_paramverifier.can_setparam(_struct.*m_pmember)) return true;
		//_sql.SetParam(_sNamePrefix+m_sName,&(_struct.*m_pmember));
		return sqlfuncs<_ItemType>::SqlSetParam(_sql,_sNamePrefix+m_sName,&(_struct.*m_pmember),SPD_Input);
	}

	/**\brief function that implements \ref ISqlLayoutItem::set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf) const
	\param _sNamePrefix -- name prefix
	\param _struct -- data struct to set param from
	\param _sql -- data base access object
	\param _tmpbuf -- temp buffer 
	\return true if parameter was successfully set
	\sa ISqlLayoutItem::set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf) const
	*/
	virtual bool set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf) const
	{
		VERIFY_EXIT1(NOT_NULL(m_pmember),false);
		if(!m_paramverifier.can_setparam(_struct.*m_pmember)) return true;
		_ItemType* pitem = _tmpbuf.add(_struct.*m_pmember);
		//_sql.SetParam(_sNamePrefix+m_sName,pitem,CS_RETURN);
		return sqlfuncs<_ItemType>::SqlSetParam(_sql,_sNamePrefix+m_sName,pitem,SPD_InputOutput);
	}

	/**\brief function that implement \ref ISqlLayoutItem::bind(_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf,bool _bfirst = false) const
	\param _struct -- data struct to bind data for
	\param _sql -- data base access object
	\param _tmpbuf -- temp buffer
	\param _bfirst -- creation flag
	\return true if item was successfully bound
	\sa ISqlLayoutItem::bind(_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf,bool _bfirst) const
	*/
	virtual bool bind(_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf,bool _bfirst = false) const
	{
		VERIFY_EXIT1(NOT_NULL(m_pmember),false);
		_ItemType* pitem = NULL;
		if(_bfirst)
			pitem = _tmpbuf.add(_struct.*m_pmember);
		else
			pitem = _tmpbuf.get(_struct.*m_pmember);
		//_sql.Bind(pitem);

		sqlfuncs<_ItemType>::SqlBind(_sql,pitem);
		return true;
	}

	/**\brief function that implement \ref ISqlLayoutItem::prepare_buffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const 
	\param _struct -- data struct to prepare buffer for
	\param _tmpbuf -- temp buffer to prepare buffer in
	\return return true if parameter was successfully prepared in buffer
	\sa ISqlLayoutItem::prepare_buffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const 
	*/
	virtual bool prepare_buffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const 
	{
		VERIFY_EXIT1(NOT_NULL(m_pmember),false);
		_tmpbuf.prepare_add(_struct.*m_pmember);
		return true;
	}

	/**\brief function that implement \ref ISqlLayoutItem::update_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	\param _struct -- data to update buffer for
	\param _tmpbuf -- temp buffer to get data from and to put data info _struct
	\return true is data was successfully updated
	\sa ISqlLayoutItem::update_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	*/
	virtual bool update_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	{
		VERIFY_EXIT1(NOT_NULL(m_pmember),false);
		_tmpbuf.update(_struct.*m_pmember);
		return true;
	}

	/**\brief function that implement \ref ISqlLayoutItem::remove_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	\param _struct -- data struct to remove data for
	\param _tmpbuf -- temp buffer
	\return true if data was successfully removed
	\sa ISqlLayoutItem::remove_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	*/
	virtual bool remove_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	{
		VERIFY_EXIT1(NOT_NULL(m_pmember),false);
		_tmpbuf.remove(_struct.*m_pmember);
		return true;
	}

protected:
	_ItemType _Struct::* m_pmember;		///< field member pointer
	const CString_ m_sName;					///< field name
	const _ParamVerifier m_paramverifier;	///< param verifier object
};//template<> struct CSqlLayoutItem

/**\brief implementation of \ref ISqlLayoutItem
          for string type as field member.
\tparam _Struct -- data structure
\tparam _PriorSql -- data base access class (ST)
\tparam _ParamVerifier -- verifier class
*/
template<typename _Struct,typename _PriorSql,typename _ParamVerifier>
struct CSqlLayoutStringItem : public ISqlLayoutItem<_Struct,_PriorSql>
{

	/**\brief Constructor. It create \ref ISqlLayoutItem for string membre 
		      with specified maximum string buffer length.
	\param _sName -- sql layout field name
	\param _pmember -- field member pointer
	\param _maxlen -- maximum length of string buffer
	\param _paramverifier -- parameter verifier
	*/
	CSqlLayoutStringItem(const CString_& _sName
		,CString_ _Struct::* _pmember
		,long _maxlen
		,const _ParamVerifier& _paramverifier
		)
		:m_sName(_sName),m_pmember(_pmember)
		,m_maxlen(_maxlen)
		,m_paramverifier(_paramverifier)
	{
	}

	/**\brief function that implements string version of \ref ISqlLayoutItem::set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql) const 
	\param _sNamePrefix -- name prefix 
	\param _struct -- data struct to set field parameter of it. 
	\param _sql -- data access object
	\return true if parameter was successfully set
	\sa ISqlLayoutItem::set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql) const 
	*/
	virtual bool set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql) const 
	{
		VERIFY_EXIT1(NOT_NULL(m_pmember),false);
		if(!m_paramverifier.can_setparam(_struct.*m_pmember)) return true;
		//_sql.SetParam(_sNamePrefix+m_sName,(LPTSTR)(LPCTSTR)(_struct.*m_pmember));
		sqlfuncs<LPTSTR>::SqlSetParam(_sql,_sNamePrefix+m_sName,(LPTSTR)(LPCTSTR)(_struct.*m_pmember),SPD_Input);
		return true;
	}

	/**\brief function that implements string version of \ref ISqlLayoutItem::set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf) const
	\param _sNamePrefix -- name prefix 
	\param _struct -- data to set for it field
	\param _sql -- data access object
	\param _tmpbuf -- temp buffer
	\return true if parmeter was successfully set
	\sa ISqlLayoutItem::set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf) const
	*/
	virtual bool set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf) const
	{
		VERIFY_EXIT1(NOT_NULL(m_pmember),false);
		if(!m_paramverifier.can_setparam(_struct.*m_pmember)) return true;
		LPTSTR pitem = _tmpbuf.add(_struct.*m_pmember,m_maxlen);
		//_sql.SetParam(_sNamePrefix+m_sName,pitem,CS_RETURN);
		sqlfuncs<LPTSTR>::SqlSetParam(_sql,_sNamePrefix+m_sName,pitem,SPD_InputOutput);
		return true;
	}

	/**\brief function that implements string version of \ref ISqlLayoutItem::bind(_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf,bool _bfirst) const 
	\param _struct -- data struct to bind field of it
	\param _sql -- data base access object
	\param _tmpbuf -- temp buffer 
	\param _bfirst -- creation flag 
	\return true if data was successfully bound
	\sa ISqlLayoutItem::bind(_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf,bool _bfirst) const 
	*/
	virtual bool bind(_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf,bool _bfirst = false) const 
	{
		VERIFY_EXIT1(NOT_NULL(m_pmember),false);
		LPTSTR pitem = NULL;
		if(_bfirst)
			pitem = _tmpbuf.add(_struct.*m_pmember,m_maxlen);
		else
			pitem = _tmpbuf.get(_struct.*m_pmember,m_maxlen);
		//_sql.Bind(pitem,m_maxlen);
		sqlfuncs<LPTSTR>::SqlBind(_sql,pitem,m_maxlen);
		return true;
	}

	/**\brief function that implement string version of \ref ISqlLayoutItem::prepare_buffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	\param _struct -- data struct to work with
	\param _tmpbuf -- temp buffer
	\return true if function successfully prepare buffer for this sql layout item
	\sa ISqlLayoutItem::prepare_buffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	*/
	virtual bool prepare_buffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	{
		VERIFY_EXIT1(NOT_NULL(m_pmember),false);
		_tmpbuf.prepare_add(_struct.*m_pmember,m_maxlen);
		return true;
	}

	/**\brief function that implements string version of \ref ISqlLayoutItem::update_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	\param _struct -- data struct to work with
	\param _tmpbuf -- temp buffer
	\return true if fucntion successfully update data from buffer into field of _struct
	\sa ISqlLayoutItem::update_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	*/
	virtual bool update_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	{
		VERIFY_EXIT1(NOT_NULL(m_pmember),false);
		_tmpbuf.update(_struct.*m_pmember,m_maxlen);
		return true;
	}
	/**\brief function that implements string version of \ref ISqlLayoutItem::remove_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	\param _struct -- data struct to work with
	\param _tmpbuf -- temp buffer
	\return true if fucntion successfully remove data of current field from buffer
	\sa ISqlLayoutItem::remove_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	*/
	virtual bool remove_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	{
		VERIFY_EXIT1(NOT_NULL(m_pmember),false);
		_tmpbuf.remove(_struct.*m_pmember,m_maxlen);
		return true;
	}

protected:
	CString_ _Struct::* m_pmember;		///< field member pointer
	long m_maxlen;						///< maximim length of this string buffer
	const CString_ m_sName;					///< name of this field
	const _ParamVerifier m_paramverifier;	///< field verifier
};//template<> struct CSqlLayoutStringItem

//template<typename _Struct,typename _PriorSql>
//interface ISqlLayout
//{
//};//template<> ISqlLayout

/**\brief sql layout item for base class (to represent in sql layout base class members)
\tparam _Struct -- data struct
\tparam _PriorSql -- data base access class (type)
\tparam _BaseType -- base class to represent in a layout
*/
template<typename _Struct,typename _PriorSql,typename _BaseType>
struct SqlBaseItem : public ISqlLayoutItem<_Struct,_PriorSql>
{
	/**\brief Constructor.
	\param _basesqllayout -- layout of base class
	*/
	SqlBaseItem(const ISqlLayoutItem<_BaseType,_PriorSql>& _basesqllayout)
		:m_basesqllayout(_basesqllayout)
	{
	}

	//virtual bool can_setparam(_Struct& _struct) const = 0;

	/**\brief set paramters of the base class.
	          function that implements \ref ISqlLayoutItem::set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql) const
	\param _sNamePrefix -- prefix for name
	\param _struct -- data struct to work with (used as derived type)
	\param _sql -- data base access object
	\return true if parameters of base type was successsfully set
	\sa ISqlLayoutItem::set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql) const
	*/
	virtual bool set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql) const
	{
		return m_basesqllayout.set_param(
			_sNamePrefix
			,static_cast<_BaseType&>(_struct)
			,_sql
			);
	}

	/**\brief set parameters of the base class (usung temp buffer).
	          function that impelments \ref ISqlLayoutItem::set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf) const
	\param _sNamePrefix -- prefix of name
	\param _struct -- data struct to work with 
	\param _sql -- data base access object
	\param _tmpbuf -- temp buffer 
	\return true if function successfully set all base params
	\sa ISqlLayoutItem::set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf) const
	*/
	virtual bool set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf) const
	{
		return m_basesqllayout.set_param(
			_sNamePrefix
			,static_cast<_BaseType&>(_struct)
			,_sql
			,_tmpbuf
			);
	}

	/**\brief bind parameters of base class.
	          function that implements \ref ISqlLayoutItem::bind(_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf,bool _bfirst) const
	\param _struct -- data struct to work with 
	\param _sql -- data base access objects 
	\param _tmpbuf -- temp buffer
	\param _bfirst -- buffer allocation flag
	\return true if all base params was successfully bound
	\sa ISqlLayoutItem::bind(_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf,bool _bfirst) const
	*/
	virtual bool bind(_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf,bool _bfirst = false) const
	{
		return m_basesqllayout.bind(
			static_cast<_BaseType&>(_struct)
			,_sql
			,_tmpbuf
			,_bfirst
			);
	}

	/**\brief prepare temp sql buffer for base class params.
	          function that implements \ref ISqlLayoutItem::prepare_buffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	\param _struct -- data struct to work with it fields
	\param _tmpbuf -- temp buffer
	\return true if function successfully prepared buffers for all base type.
	\sa ISqlLayoutItem::prepare_buffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	*/
	virtual bool prepare_buffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	{
		return m_basesqllayout.prepare_buffer(
			static_cast<_BaseType&>(_struct)
			,_tmpbuf
			);
	}

	/**\brief update data for base type.
	          function that implements \ref ISqlLayoutItem::update_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	\param _struct -- dat struct to work with
	\param _tmpbuf -- temprory buffer
	\return true if for all base fields data was successfully updated
	\sa ISqlLayoutItem::update_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	*/
	virtual bool update_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	{
		return m_basesqllayout.update_frombuffer(
			static_cast<_BaseType&>(_struct)
			,_tmpbuf
			);
	}

	/**\brief remove buffers from temp buffer for all members of base class.
	          function that implements \ref ISqlLayoutItem::remove_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	\param _struct -- dat struct to work with
	\param _tmpbuf -- temprory buffer
	return true if all base params was successfully removed
	\sa ISqlLayoutItem::remove_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	*/
	virtual bool remove_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	{
		return m_basesqllayout.remove_frombuffer(
			static_cast<_BaseType&>(_struct)
			,_tmpbuf
			);
	}
protected:
	const ISqlLayoutItem<_BaseType,_PriorSql>& m_basesqllayout;	///< sql layout for base class
};//template<> struct SqlBaseItem

/**\brief struct field implementation of \ref ISqlLayoutItem 
\tparam _Struct -- struct data type
\tparam _PriorSql -- data base access type 
\tparam _ItemFieldStruct -- field item type
\tparam _ParamVerifier -- verifier type
*/
template<
	typename _Struct
	,typename _PriorSql
	,typename _ItemFieldStruct
	,typename _ParamVerifier
	>
struct CSqlStructFieldLayoutItem 
:
	public ISqlLayoutItem<_Struct,_PriorSql>
{
protected:
	CString_ m_prefix;			///< name prefix
	const ISqlLayoutItem<_ItemFieldStruct,_PriorSql>& m_sqllayout;	///<  field layout of struct data 
	_ItemFieldStruct _Struct::* m_pmember;		///< field member pointer
	const _ParamVerifier m_paramverifier;		///< verifier object
public:

	/**\brief Constructor.
	\param _sPrefix -- name prefix
	\param _pmember -- member field
	\param _sqllayout -- sql layout for this field
	\param _paramverifier -- verifier object
	*/
	CSqlStructFieldLayoutItem(
		const CString_& _sPrefix
		,_ItemFieldStruct _Struct::* _pmember
		,const ISqlLayoutItem<_ItemFieldStruct,_PriorSql>& _sqllayout
		,const _ParamVerifier& _paramverifier
		)
		:m_prefix(_sPrefix)
		,m_pmember(_pmember)
		,m_sqllayout(_sqllayout)
		,m_paramverifier(_paramverifier)
	{
		VERIFY_EXIT(NOT_NULL(_pmember));
	}

	/**\brief function to set parameter for this item.
	          This function implements \ref ISqlLayoutItem::set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql) const
	\param _sNamePrefix -- name prefix
	\param _struct -- data type for this field item
	\param _sql -- data base access object
	\return true if parameter was successfully set.
	\sa ISqlLayoutItem::set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql) const
	*/
	virtual bool set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql) const
	{
		VERIFY_EXIT1(NOT_NULL(m_pmember),false);
		if(!m_paramverifier.can_setparam(_struct.*m_pmember)) return true;
		return m_sqllayout.set_param(
			_sNamePrefix+m_prefix
			,_struct.*m_pmember
			,_sql
			);
	}

	/**\brief function to set parameter for this item (using temp buffer).
			  This function implements \ref ISqlLayoutItem::set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf) const
	\param _sNamePrefix -- prefix of name
	\param _struct -- data type for this field item
	\param _sql -- data base access object
	\param _tmpbuf -- temp buffer.
	\return true if parameter was successfully set.
	\sa ISqlLayoutItem::set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf) const
	*/
	virtual bool set_param(const CString_& _sNamePrefix,_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf) const
	{
		VERIFY_EXIT1(NOT_NULL(m_pmember),false);
		if(!m_paramverifier.can_setparam(_struct.*m_pmember)) return true;
		return m_sqllayout.set_param(
			_sNamePrefix+m_prefix
			,_struct.*m_pmember
			,_sql
			,_tmpbuf
			);
	}

	/**\brief function to bind parameter for this item.
	          This function implements \ref ISqlLayoutItem::bind(_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf,bool _bfirst) const
	\param _struct -- data type for this field item
	\param _sql -- data base access object
	\param _tmpbuf -- temp buffer.
	\param _bfirst -- temp buffer creating flag
	\return true if parameter was successfully bound
	\sa ISqlLayoutItem::bind(_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf,bool _bfirst) const
	*/
	virtual bool bind(_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf,bool _bfirst = false) const
	{
		VERIFY_EXIT1(NOT_NULL(m_pmember),false);
		return m_sqllayout.bind(
			_struct.*m_pmember
			,_sql
			,_tmpbuf
			,_bfirst
			);
	}

	/**\brief function to prepare buffer for this item.
	          This function implements \ref ISqlLayoutItem::prepare_buffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	\param _struct -- data type for this field item
	\param _tmpbuf -- temp buffer.
	\return true if parameter buffer was successfully set
	\sa ISqlLayoutItem::prepare_buffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	*/
	virtual bool prepare_buffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	{
		VERIFY_EXIT1(NOT_NULL(m_pmember),false);
		return m_sqllayout.prepare_buffer(
			_struct.*m_pmember
			,_tmpbuf
			);
	}

	/**\brief function to update data from buffer.
	          This function implements \ref ISqlLayoutItem::update_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	\param _struct -- data type for this field item
	\param _tmpbuf -- temp buffer.
	\return true if parameter was successfully updated from buffer to parameter field of _struct
	\sa ISqlLayoutItem::update_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	*/
	virtual bool update_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	{
		VERIFY_EXIT1(NOT_NULL(m_pmember),false);
		return m_sqllayout.update_frombuffer(
			_struct.*m_pmember
			,_tmpbuf
			);
	}

	/**\brief function to remove parameter from sql buffer.
		      This function implements \ref ISqlLayoutItem::remove_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	\param _struct -- data type for this field item
	\param _tmpbuf -- temp buffer.
	\return true if parameter was successfully removed from temp sql buffer.
	\sa ISqlLayoutItem::remove_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	*/
	virtual bool remove_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	{
		VERIFY_EXIT1(NOT_NULL(m_pmember),false);
		return m_sqllayout.remove_frombuffer(
			_struct.*m_pmember
			,_tmpbuf
			);
	}
};//template<> struct CSqlStructFieldLayoutItem

/**\brief base class of application`s sql layouts.
\tparam _Struct -- data type for which sql layout will be created
\tparam _PriorSql -- data base access type (ST)
*/
template<typename _Struct,typename _PriorSql>
struct CSqlStructLayoutBase : public ISqlLayoutItem<_Struct,_PriorSql>
{
private:
	typedef std::vector<ISqlLayoutItem<_Struct,_PriorSql>* > ItemsLst;	///< type def vector of layout items 
public:

	/**\brief Constructor.
	*/
	CSqlStructLayoutBase()
	{
	}

	/**\brief Destructor.
	*/
	~CSqlStructLayoutBase()
	{
		free();
	}
	
	/**\brief function to specify base class layout
	\tparam _BaseType -- base type. _Struct is derived from _BaseType
	\param _basesqllayout -- layout for the base class
	*/
	template<typename _BaseType>
		void add_base(const ISqlLayoutItem<_BaseType,_PriorSql>& _basesqllayout)
	{
		m_items.push_back(
			trace_alloc((new SqlBaseItem<_Struct,_PriorSql,_BaseType>(_basesqllayout)))
			);
	}

	/**\brief function to add string parameter
	This function use \ref CSqlNoVerifyItem that equivalent to do not any verifications.
	\param _name -- name of this parameter
	\param _pmember -- field pointer
	\param _maxlen -- maximum length for the string buffer
	*/
	void add(const FieldName<_Struct>& _name
		,CString_ _Struct::* _pmember
		,long _maxlen
		)
	{
		CString_ sName;
		_name.get_name(reinterpret_cast<void* _Struct::*>(_pmember),sName);
		m_items.push_back(
			trace_alloc(
					(
						new CSqlLayoutStringItem<_Struct,_PriorSql,CSqlNoVerifyItem>
						(
							sName
							,_pmember
							,_maxlen
							,CSqlNoVerifyItem()
						)
					)
				)
			);
	}

	/**\brief function to add string parameter with field verifier
	\tparam _ParamVerifier -- verifier type
	\param _name -- parameter name
	\param _pmember -- parameter field (member) pointer 
	\param _maxlen -- maximum length of string buffer
	\param _paramverifier -- verifier object
	*/
	template<typename _ParamVerifier>
		void add_withverifier(
			const FieldName<_Struct>& _name
			,CString_ _Struct::* _pmember
			,long _maxlen
			,const _ParamVerifier& _paramverifier
			)
	{
		CString_ sName;
		_name.get_name(reinterpret_cast<void* _Struct::*>(_pmember),sName);
		m_items.push_back(
			trace_alloc(
					(
						new CSqlLayoutStringItem<_Struct,_PriorSql,_ParamVerifier>(
							sName,_pmember,_maxlen,_paramverifier
						)
					)
				)
			);
	}

	/**\brief function to add generelized parameter.
	This function use \ref CSqlNoVerifyItem that equivalent to do not any verifications.
	\tparam _ItemType -- type of the field
	\param _name -- parameter name
	\param _pmember -- parameter field (member) pointer 
	*/
	template<typename _ItemType>
		void add(
			const FieldName<_Struct>& _name
			,_ItemType _Struct::* _pmember
			)
	{
		CString_ sName;
		_name.get_name(reinterpret_cast<void* _Struct::*>(_pmember),sName);
		m_items.push_back(
			trace_alloc(
					(	
						new CSqlLayoutItem<_Struct,_PriorSql,_ItemType,CSqlNoVerifyItem>(
							sName
							,_pmember
							,CSqlNoVerifyItem()
						)
					)
				)
			);
	}

	/**\brief function to add generelized parameter and set verifier for it.
	This function use \ref CSqlNoVerifyItem that equivalent to do not any verifications.
	\tparam _ItemType -- type of the field
	\tparam _ParamVerifier -- verifier type
	\param _name -- parameter name
	\param _pmember -- parameter field (member) pointer 
	\param _paramverifier -- parameter verifier
	*/
	template<typename _ItemType,typename _ParamVerifier>
		void add_withverifier(
			const FieldName<_Struct>& _name
			,_ItemType _Struct::* _pmember
			,const _ParamVerifier& _paramverifier
			)
	{
		CString_ sName;
		_name.get_name(reinterpret_cast<void* _Struct::*>(_pmember),sName);
		m_items.push_back(
			trace_alloc(
					(
						new CSqlLayoutItem<_Struct,_PriorSql,_ItemType,_ParamVerifier>
						(
							sName,_pmember,_paramverifier
						)
					)
				)
			);
	}

	/**\brief function to add complex data as a member of sql layout (you specify sql layout for this member).
	\tparam _StructFieldType -- complex type
	\param _name -- field name
	\param _pmember -- filed member pointer
	\param _sqllayout -- sql layout for this field
	*/
	template<typename _StructFieldType>
		void add_complex(
			const FieldName<_Struct>& _name
			,_StructFieldType _Struct::* _pmember
			,const ISqlLayoutItem<_StructFieldType,_PriorSql>& _sqllayout
			)
	{
		CString_ sName;
		_name.get_name(reinterpret_cast<void* _Struct::*>(_pmember),sName);
		m_items.push_back(
			trace_alloc((new CSqlStructFieldLayoutItem<_Struct,_PriorSql,_StructFieldType,CSqlNoVerifyItem>(
					sName
					,_pmember
					,_sqllayout
					,CSqlNoVerifyItem()
					))
				)
			);
	}

	/**\brief function to add complex data as a member of sql layout (you specify sql layout for this member)
	          and set field verifier for this parameter.
	\tparam _StructFieldType -- complex type
	\tparam _ParamVerifier -- verifier type
	\param _name -- field name
	\param _pmember -- filed member pointer
	\param _sqllayout -- sql layout for this field
	\param _paramverifier -- parameter verifier
	*/
	template<typename _StructFieldType,typename _ParamVerifier>
		void add_complex_withverifier(
			const FieldName<_Struct>& _name
			,_StructFieldType _Struct::* _pmember
			,const ISqlLayoutItem<_StructFieldType,_PriorSql>& _sqllayout
			,const _ParamVerifier& _paramverifier
			)
	{
		CString_ sName;
		_name.get_name(reinterpret_cast<void* _Struct::*>(_pmember),sName);
		m_items.push_back(
			trace_alloc((new CSqlStructFieldLayoutItem<_Struct,_PriorSql,_StructFieldType,_ParamVerifier>(
					sName
					,_pmember
					,_sqllayout
					,_paramverifier
					))
				)
			);
	}

	/**\brief function to set params for this layout. 
	          This fucntion implements \ref ISqlLayoutItem::set_param(const CString_& _pefix,_Struct& _struct,_PriorSql& _sql) const
	\param _pefix -- prefix of name
	\param _struct -- data struct to use
	\param _sql -- data base access object (ST)
	\return true if all parametrs of this layout was successfully sets
	\sa ISqlLayoutItem::set_param(const CString_& _pefix,_Struct& _struct,_PriorSql& _sql) const
	*/
	bool set_param(const CString_& _pefix,_Struct& _struct,_PriorSql& _sql) const
	{
		ItemsLst::const_iterator it,ite;
		it = m_items.begin();
		ite = m_items.end();
		for(;it!=ite;++it)
		{
			if(!(*it)->set_param(_T("@"),_struct,_sql)) return false;
		}
		return true;
	}

	/**\brief function to set params for this layout (using sql temp buffer)
			  This function implements \ref ISqlLayoutItem::set_param(const CString_& _pefix,_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf) const
	\param _pefix -- prefix of the paramters names of this layout
	\param _struct -- data struct to use
	\param _sql -- data base access object (ST)
	\param _tmpbuf -- temp buffer 
	\return true if all layout params was successfully set
	\sa ISqlLayoutItem::set_param(const CString_& _pefix,_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf) const
	*/
	bool set_param(const CString_& _pefix,_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf) const
	{
		ItemsLst::const_iterator it,ite;
		it = m_items.begin();
		ite = m_items.end();
		for(;it!=ite;++it)
		{
			if(!(*it)->set_param(_T("@"),_struct,_sql,_tmpbuf)) return false;
		}
		return true;
	}

	/**\brief fucntion to bind params for this layout.
			  This function implements \ref ISqlLayoutItem::bind(_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf,bool _bfirst) const
	\param _struct -- data struct to use
	\param _sql -- data base access object (ST)
	\param _tmpbuf -- temp buffer 
	\param _bfirst -- temp buffer creation flag
	\return true if all parameters was successfully bound
	\sa ISqlLayoutItem::bind(_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf,bool _bfirst) const
	*/
	bool bind(_Struct& _struct,_PriorSql& _sql,CSqlTempBuffer& _tmpbuf,bool _bfirst = false) const
	{
		ItemsLst::const_iterator it,ite;
		it = m_items.begin();
		ite = m_items.end();
		for(;it!=ite;++it)
		{
			if(!(*it)->bind(_struct,_sql,_tmpbuf,_bfirst)) return false;
		}
		return true;
	}

	/**\brief function to perpare buffer for all parameters of this layout. 
	          This function implements \ref ISqlLayoutItem::prepare_buffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	\param _struct -- data struct to use
	\param _tmpbuf -- temp buffer 
	\return return if for all parameters of this layout buffer was successfully prepared
	\sa ISqlLayoutItem::prepare_buffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	*/
	bool prepare_buffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	{
		ItemsLst::const_iterator it,ite;
		it = m_items.begin();
		ite = m_items.end();
		for(;it!=ite;++it)
		{
			if(!(*it)->prepare_buffer(_struct,_tmpbuf)) 
			{
				//return false; // XXX: possible memory leaks better to remove all already allocated
				ite = it;
				ite++;
				ItemsLst::const_iterator it = m_items.begin();
				for(;it<ite;++it)
				{
					VERIFY_DO((*it)->remove_frombuffer(_struct,_tmpbuf),continue);
				}
				return false;
			}
		}
		return true;
	}

	/**\brief function to upadate data from temp sql buffer to struct. 
	          This function implements \ref ISqlLayoutItem::update_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	\param _struct -- data struct to use
	\param _tmpbuf -- temp buffer 
	\return true if all data from sql temp buffer was successfully updated to struct
	\sa ISqlLayoutItem::update_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	*/
	virtual bool update_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	{
		ItemsLst::const_iterator it,ite;
		it = m_items.begin();
		ite = m_items.end();
		for(;it!=ite;++it)
		{
			if(!(*it)->update_frombuffer(_struct,_tmpbuf)) 
				return false; 
		}
		return true;
	}

	/**\brief function to remove all parameters from temp sql buffer for this layout.
			  This function implements \ref ISqlLayoutItem::remove_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	\param _struct -- data struct to use
	\param _tmpbuf -- temp buffer 
	\return true if all layout paramters was successfully removed
	\sa ISqlLayoutItem::remove_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	*/
	virtual bool remove_frombuffer(_Struct& _struct,CSqlTempBuffer& _tmpbuf) const
	{
		ItemsLst::const_iterator it,ite;
		it = m_items.begin();
		ite = m_items.end();
		for(;it!=ite;++it)
		{
			if(!(*it)->remove_frombuffer(_struct,_tmpbuf)) return false;
		}
		return true;
	}

	
protected:
	/**\brief function to free class data
	*/
	void free()
	{
		ItemsLst::iterator it,ite;
		it = m_items.begin();
		ite = m_items.end();
		for(;it!=ite;++it) delete trace_free(*it);
		m_items.clear();
	}
protected:
	ItemsLst m_items;		///< list of items of this layout object
private:
	CSqlStructLayoutBase(const CSqlStructLayoutBase<_Struct,_PriorSql>& _);
	CSqlStructLayoutBase<_Struct,_PriorSql>& operator=(const CSqlStructLayoutBase<_Struct,_PriorSql>& _);
};//template<> struct CSqlStructLayoutBase

/**\brief inner class (don`t use it) for metaprograming (to get correct sql layout)
          either from innner class or from other class layout position.
*/
struct SqlLayoutInnerRealize{};

/**\brief inner class (don`t use it) for metaprograming to specify location
          of layout class. Default lacation is in a structure class.
*/
template<typename _Struct>
struct SqlLayoutTraits
{
	typedef SqlLayoutInnerRealize SqlLayoutRealize;
};

/**\brief macro to specify sql layout for layout class that is placed not 
          in the data class of this layout.
*/
#define SQL_LAYOUT_EX(_Struct,_SqlLayout)	\
template<>	\
struct SqlLayoutTraits<_Struct>	\
{	\
	typedef _SqlLayout SqlLayoutRealize;	\
};

/**\inner namespace (don`t use this members in you application)
*/
namespace ns_private
{
	/**\breif class to get sql layout from layout class is not placed in the
	          data class.
	\tparam _SqlLayout -- sql layout to get layout for.
	*/
	template<typename _SqlLayout>
		struct get_sqllayout
	{
		/**\brief operator to get layout object
		\tparam _Struct -- data struct to get layout for
		\tparam _PriorSql -- data base access type (ST)
		\return sql layout constant reference
		*/
		template<typename _Struct,typename _PriorSql>
			const CSqlStructLayoutBase<_Struct,_PriorSql>& operator () (_Struct,_PriorSql)
		{
			return singleton<_SqlLayout>::get();
		}
	};

	/**\brief partial specification of \ref ns_private::get_sqllayout
	          to get layout object that is placed in the data struct class. 
	*/
	template<>
		struct get_sqllayout<SqlLayoutInnerRealize>
	{
		/**\brief fucntion to get sql layout that is placed in the struct class.
		\tparam _Struct -- data struct to get layout for
		\tparam _PriorSql -- data base access type (ST)
		\return sql layout constant reference
		*/
		template<typename _Struct,typename _PriorSql>
			const CSqlStructLayoutBase<_Struct,_PriorSql>& operator () (_Struct,_PriorSql)
		{
			typename _Struct::CSqlLayout;
			static _Struct::CSqlLayout<_PriorSql> _;
			return _;
		}
	};
};//namespace ns_private

/**\brief function that used to retry sql layout object for data class
\tparam _Struct -- data struct to get layout for
\tparam _PriorSql -- data base access type (ST)
\return \ref CSqlStructLayoutBase object constant reference
*/
template<typename _Struct,typename _PriorSql> inline 
const CSqlStructLayoutBase<_Struct,_PriorSql>& GetSqlStructLayout()
{
	typedef typename SqlLayoutTraits<_Struct>::SqlLayoutRealize SqlLayoutRealization;
	return ns_private::get_sqllayout<SqlLayoutRealization>()(_Struct(),_PriorSql());
}


template<typename _Params,typename _PriorSql> inline
	bool sql_set_params(_PriorSql& _sql,_Params& _params)
{
	const CSqlStructLayoutBase<_Params,_PriorSql>& layout = GetSqlStructLayout<_Params,_PriorSql>();
	return layout.set_param(_T(""),_params,_sql);
}

/*
	кор€ва€ MSVC 5.0 нужно писать на каждый набор параметров свою собсвенную функцию (и 
	дублировать в теле код). т.е. разрабатывать эту библиотеку на MSVC 5.0 лучше просто 
	нет смысла. ¬се же частична€ спецификаци€ мощна€ вещь. ј без нее некоторые вещи просто 
	не возможны. 
*/

/**\brief function to create sql call with input params
You should specify Sql Layout for input parameters data type.
\tparam _Params -- input params fo rsql call 
\tparam _PriorSql -- data base access class
\tparam _SyncObj -- sync type to syncronize set of input params (to lock access to 
                 to input params).

\param _sProcName -- name of procedure to call. if this paramter is empty than 
                     don`t set procedure name.
\param _params -- input params to set for sql call.
\param _sql -- data base access object (ST)
\param _bcreateconnection -- flag to create connection. If you already create 
                             connection and pass it to this function than 
							 set this argument to false.
\param _syncobj -- syncronize object
\return sql return code. you can verify this code with \ref sql_success() like function.
*/
template<typename _Params,typename _PriorSql,typename _SyncObj = NullType>
inline int CallSqlProcI(
	const CString_& _sProcName
	,_Params& _params
	,_PriorSql& _sql
	,bool _bcreateconnection = true
	,const _SyncObj& _syncobj = NullType()
	)
{
	CSqlStructLayoutBase<_Params,_PriorSql>& layout = GetSqlStructLayout<_Params,_PriorSql>();

	if(_bcreateconnection) _sql.CreateConnect();
	if(!_sProcName.IsEmpty()) _sql.SetProcCmd((LPTSTR)(LPCTSTR)_sProcName);

	{//lock params
		CAutoLock<_SyncObj> __lock(_syncobj);
		VERIFY_EXIT1(layout.set_param(_T(""),_params,_sql),-1);
	}
	_sql.ExecProc();
	return _sql.GetReturnValue(); 
}

/**\brief function to make sql call with input and output params.
You should specify Sql Layout for input and output parameters data types.
\tparam _Params -- input params for sql call 
\tparam _ReturnParams -- output params for sql call
\tparam _PriorSql -- data base access class
\tparam _SyncObj -- sync type to syncronize set of input params (to lock access to 
                 to input params).
\param _sProcName -- name of procedure to call. if this paramter is empty than 
                     don`t set procedure name.
\param _params -- input params to set for sql call.
\param _retparams -- output params 
\param _sql -- data base access object (ST)
\param _bcreateconnection -- flag to create connection. If you already create 
                             connection and pass it to this function than 
							 set this argument to false.
\param _hStopEvent -- stop event. Event to stop sql call and immediately return from this fucntion.
\param _syncobj -- syncronize object
\return sql return code. you can verify this code with \ref sql_success() like function.
*/
template<
	typename _Params
	,typename _ReturnParams
	,typename _PriorSql
	,typename _SyncObj = NullType
	>
inline int CallSqlProcIO(
	const CString_ _sProcName						// procedure name
	,_Params& _params								// params just to send to proc
	,_ReturnParams& _retparams						// params to [send and to] receive from procedure
	,_PriorSql& _sql
	,bool _bcreateconnection = true
	,HANDLE _hStopEvent = NULL
	,const _SyncObj& _syncobj = NullType()
	)
{
	const CSqlStructLayoutBase<_Params,_PriorSql>& layout = GetSqlStructLayout<_Params,_PriorSql>();
	const CSqlStructLayoutBase<_ReturnParams,_PriorSql>& retlayout = GetSqlStructLayout<_ReturnParams,_PriorSql>();

	if(_bcreateconnection) _sql.CreateConnect();
	if(!_sProcName.IsEmpty()) _sql.SetProcCmd((LPTSTR)(LPCTSTR)_sProcName);
	{//lock in params
		CAutoLockT<_SyncObj> __lock(_syncobj);
		VERIFY_EXIT1(layout.set_param(_T(""),_params,_sql),-1);
	}
	CSqlTempBuffer tmpbuf;
	{//lock in out params
		CAutoLockT<_SyncObj> __lock(_syncobj);
		VERIFY_EXIT1(retlayout.prepare_buffer(_retparams,tmpbuf),-1);
		tmpbuf.create_buffer();
		if(!retlayout.set_param(_T(""),_retparams,_sql,tmpbuf)) 
		{
			ASSERT_(FALSE);
			tmpbuf.restart();
			VERIFY(retlayout.remove_frombuffer(_retparams,tmpbuf)); //for calling delete for classes
			return -1;
		}
	}
	if(_sql.Send())
	{
		while(_sql.Exec())
		{
			if(is_stoped(_hStopEvent))
			{
				_sql.CancelAllCmd();
				return 0;
			}
			if(CS_ROW_RESULT==_sql.GetResult())
			{
				ASSERT_(FALSE); //incorrect function usage use CallSqlProc that returns stream
				_sql.Flush();
				while(_sql.Fetch())
				{
					if(is_stoped(_hStopEvent))
					{
						_sql.CancelAllCmd();
						return 0;
					}
				}
				continue;
			}
			if(CS_PARAM_RESULT==_sql.GetResult())
			{
				_sql.Flush();

				CAutoLockT<_SyncObj> __lock(_syncobj);

				tmpbuf.restart();
				VERIFY_DO(retlayout.bind(_retparams,_sql,tmpbuf),continue);
				while(_sql.Fetch())
				{
					tmpbuf.restart();
					VERIFY(retlayout.update_frombuffer(_retparams,tmpbuf));
					if(is_stoped(_hStopEvent))
					{
						_sql.CancelAllCmd();
						return 0;
					}
				}
				continue;
			}
		}
	}
	tmpbuf.restart();
	{
		CAutoLockT<_SyncObj> __lock(_syncobj);
		VERIFY(retlayout.remove_frombuffer(_retparams,tmpbuf)); //for calling delete for classes
	}
	return _sql.GetReturnValue(); 
}

/**\brief function to make sql call with input and output params.
You should specify Sql Layout for input and output parameters data types.
\tparam _Params -- input params for sql call 
\tparam _ReturnParams -- output params for sql call
\tparam _PriorSql -- data base access class
\tparam _SyncObj -- sync type to syncronize set of input params (to lock access to 
                 to input params).
\param _sProcName -- name of procedure to call. if this paramter is empty than 
                     don`t set procedure name.
\param _params -- input params to set for sql call.
\param _retparams -- output params 
\param _sql -- data base access object (ST)
\param _bcreateconnection -- flag to create connection. If you already create 
                             connection and pass it to this function than 
							 set this argument to false.
\param _hStopEvent -- stop event. Event to stop sql call and immediately return from this fucntion.
\param _syncobj -- syncronize object
\return sql return code. you can verify this code with \ref sql_success() like function.
*/
template<
	typename _ReturnParams
	,typename _PriorSql
	,typename _SyncObj = NullType
	>
inline int CallSqlProcO(
	const CString_ _sProcName						// procedure name
	,_ReturnParams& _retparams						// params to [send and to] receive from procedure
	,_PriorSql& _sql
	,bool _bcreateconnection = true
	,HANDLE _hStopEvent = NULL
	,const _SyncObj& _syncobj = NullType()
	)
{
	const CSqlStructLayoutBase<_ReturnParams,_PriorSql>& retlayout = GetSqlStructLayout<_ReturnParams,_PriorSql>();

	if(_bcreateconnection) _sql.CreateConnect();
	if(!_sProcName.IsEmpty()) _sql.SetProcCmd((LPTSTR)(LPCTSTR)_sProcName);

	CSqlTempBuffer tmpbuf;
	{//lock in out params
		CAutoLockT<_SyncObj> __lock(_syncobj);
		VERIFY_EXIT1(retlayout.prepare_buffer(_retparams,tmpbuf),-1);
		tmpbuf.create_buffer();
		if(!retlayout.set_param(_T(""),_retparams,_sql,tmpbuf)) 
		{
			ASSERT_(FALSE);
			tmpbuf.restart();
			VERIFY(retlayout.remove_frombuffer(_retparams,tmpbuf)); //for calling delete for classes
			return -1;
		}
	}
	if(_sql.Send())
	{
		while(_sql.Exec())
		{
			if(is_stoped(_hStopEvent))
			{
				_sql.CancelAllCmd();
				return 0;
			}
			if(CS_ROW_RESULT==_sql.GetResult())
			{
				ASSERT_(FALSE); //incorrect function usage use CallSqlProc that returns stream
				_sql.Flush();
				while(_sql.Fetch())
				{
					if(is_stoped(_hStopEvent))
					{
						_sql.CancelAllCmd();
						return 0;
					}
				}
				continue;
			}
			if(CS_PARAM_RESULT==_sql.GetResult())
			{
				_sql.Flush();

				CAutoLockT<_SyncObj> __lock(_syncobj);

				tmpbuf.restart();
				VERIFY_DO(retlayout.bind(_retparams,_sql,tmpbuf),continue);
				while(_sql.Fetch())
				{
					tmpbuf.restart();
					VERIFY(retlayout.update_frombuffer(_retparams,tmpbuf));
					if(is_stoped(_hStopEvent))
					{
						_sql.CancelAllCmd();
						return 0;
					}
				}
				continue;
			}
		}
	}
	tmpbuf.restart();
	{
		CAutoLockT<_SyncObj> __lock(_syncobj);
		VERIFY(retlayout.remove_frombuffer(_retparams,tmpbuf)); //for calling delete for classes
	}
	return _sql.GetReturnValue(); 
}


/**\brief function to make sql call with input, output params and stream to get from sql call.
You should specify Sql Layout for input, output parameters and stream data types.
\tparam _Params -- input params for sql call 
\tparam _ReturnParams -- output params for sql call
\tparam _StreamContType -- container type of sql stream.
\tparam _PriorSql -- data base access class
\tparam _SyncObj -- sync type to syncronize set of input params (to lock access to 
                 to input params).
\param _sProcName -- name of procedure to call. if this paramter is empty than 
                     don`t set procedure name.
\param _params -- input params to set for sql call.
\param _retparams -- output params 
\param _outstream -- output stream holder (list of stream types)
\param _sql -- data base access object (ST)
\param _bcreateconnection -- flag to create connection. If you already create 
                             connection and pass it to this function than 
							 set this argument to false.
\param _hStopEvent -- stop event. Event to stop sql call and immediately return from this fucntion.
\param _syncobj -- syncronize object
\return sql return code. you can verify this code with \ref sql_success() like function.
*/
template<
	typename _Params
	,typename _ReturnParams
	,typename _StreamContType
	,typename _PriorSql
	,typename _SyncObj = NullType
	>
inline int CallSqlProcIOS(
	const CString_ _sProcName
	,_Params& _params,_ReturnParams& _retparams
	,_StreamContType& _outstream
	,_PriorSql& _sql
	,bool _bcreateconnection = true
	,HANDLE _hStopEvent = NULL
	,const _SyncObj& _syncobj = NullType()
	)
{
	typedef typename _StreamContType::value_type StreamType;

	const CSqlStructLayoutBase<_Params,_PriorSql>& layout = GetSqlStructLayout<_Params,_PriorSql>();
	const CSqlStructLayoutBase<_ReturnParams,_PriorSql>& retlayout = GetSqlStructLayout<_ReturnParams,_PriorSql>();
	const CSqlStructLayoutBase<StreamType,_PriorSql>& streamlayout = GetSqlStructLayout<StreamType,_PriorSql>();

	if(_bcreateconnection) _sql.CreateConnect();
	if(!_sProcName.IsEmpty()) _sql.SetProcCmd((LPTSTR)(LPCTSTR)_sProcName);
	{
		CAutoLockT<_SyncObj> __lock(_syncobj);
		VERIFY_EXIT1(layout.set_param(_T(""),_params,_sql),-1);
	}
	CSqlTempBuffer tmpbuf;

	{
		CAutoLockT<_SyncObj> __lock(_syncobj);

		VERIFY_EXIT1(retlayout.prepare_buffer(_retparams,tmpbuf),-1);
		tmpbuf.create_buffer();
		if(!retlayout.set_param(_T(""),_retparams,_sql,tmpbuf))
		{
			ASSERT_(FALSE);
			tmpbuf.restart();
			VERIFY(retlayout.remove_frombuffer(_retparams,tmpbuf)); //for calling delete for classes
			return -1;
		}
	}

	if(_sql.Send())
	{
		while(_sql.Exec())
		{
			if(is_stoped(_hStopEvent))
			{
				_sql.CancelAllCmd();
				return 0;
			}
			if(CS_ROW_RESULT==_sql.GetResult())
			{
				_sql.Flush();

				StreamType databuf;
				CSqlTempBuffer tmpbufstream;
				VERIFY_DO(streamlayout.prepare_buffer(databuf,tmpbufstream),continue);
				tmpbufstream.create_buffer();
				VERIFY(streamlayout.bind(databuf,_sql,tmpbufstream,true));
				while(_sql.Fetch())
				{
					tmpbufstream.restart();
					VERIFY(streamlayout.update_frombuffer(databuf,tmpbufstream));
					{
						CAutoLockT<_SyncObj> __lock(_syncobj);
						_outstream.push_back(databuf);
					}
					if(is_stoped(_hStopEvent))
					{
						_sql.CancelAllCmd();
						return 0;
					}
				}
				tmpbufstream.restart();
				streamlayout.remove_frombuffer(databuf,tmpbufstream);
				continue;
			}
			if(CS_PARAM_RESULT==_sql.GetResult())
			{
				_sql.Flush();

				CAutoLockT<_SyncObj> __lock(_syncobj);

				tmpbuf.restart();
				retlayout.bind(_retparams,_sql,tmpbuf);
				while(_sql.Fetch())
				{
					tmpbuf.restart();
					retlayout.update_frombuffer(_retparams,tmpbuf);
					if(is_stoped(_hStopEvent))
					{
						_sql.CancelAllCmd();
						return 0;
					}
				}
				continue;
			}
		}
	}

	{
		CAutoLockT<_SyncObj> __lock(_syncobj);

		tmpbuf.restart();
		retlayout.remove_frombuffer(_retparams,tmpbuf); //for calling delete for classes
	}
	return _sql.GetReturnValue(); 
}

/**\brief function to make sql call with input params and stream to get from sql call.
You should specify Sql Layout for input, output parameters and stream data types.
\tparam _Params -- input params for sql call 
\tparam _StreamContType -- container type of sql stream.
\tparam _PriorSql -- data base access class
\tparam _SyncObj -- sync type to syncronize set of input params (to lock access to 
                 to input params).
\param _sProcName -- name of procedure to call. if this paramter is empty than 
                     don`t set procedure name.
\param _params -- input params to set for sql call.
\param _outstream -- output stream holder (list of stream types)
\param _sql -- data base access object (ST)
\param _bcreateconnection -- flag to create connection. If you already create 
                             connection and pass it to this function than 
							 set this argument to false.
\param _hStopEvent -- stop event. Event to stop sql call and immediately return from this fucntion.
\param _syncobj -- syncronize object
\return sql return code. you can verify this code with \ref sql_success() like function.
*/
template<
	typename _Params
	,typename _StreamContType
	,typename _PriorSql
	,typename _SyncObj = NullType
	>
inline int CallSqlProcIS(
	const CString_ _sProcName
	,_Params& _params
	,_StreamContType& _outstream
	,_PriorSql& _sql
	,bool _bcreateconnection = true
	,HANDLE _hStopEvent = NULL
	,const _SyncObj& _syncobj = NullType()
	)
{
	typedef typename _StreamContType::value_type StreamType;

	const CSqlStructLayoutBase<_Params,_PriorSql>& layout = GetSqlStructLayout<_Params,_PriorSql>();
	const CSqlStructLayoutBase<StreamType,_PriorSql>& streamlayout = GetSqlStructLayout<StreamType,_PriorSql>();

	if(_bcreateconnection) _sql.CreateConnect();
	if(!_sProcName.IsEmpty()) _sql.SetProcCmd((LPTSTR)(LPCTSTR)_sProcName);
	{
		CAutoLockT<_SyncObj> __lock(_syncobj);
		layout.set_param(_T(""),_params,_sql);
	}
	if(_sql.Send())
	{
		while(_sql.Exec())
		{
			if(is_stoped(_hStopEvent))
			{
				_sql.CancelAllCmd();
				return 0;
			}
			if(CS_ROW_RESULT==_sql.GetResult())
			{
				_sql.Flush();

				StreamType databuf;
				CSqlTempBuffer tmpbufstream;
				streamlayout.prepare_buffer(databuf,tmpbufstream);
				tmpbufstream.create_buffer();
				streamlayout.bind(databuf,_sql,tmpbufstream,true);
				while(_sql.Fetch())
				{
					tmpbufstream.restart();
					streamlayout.update_frombuffer(databuf,tmpbufstream);
					{
						CAutoLockT<_SyncObj> __lock(_syncobj);
						_outstream.push_back(databuf);
					}
					if(is_stoped(_hStopEvent))
					{
						_sql.CancelAllCmd();
						return 0;
					}
				}
				tmpbufstream.restart();
				streamlayout.remove_frombuffer(databuf,tmpbufstream);
				continue;
			}
			if(CS_PARAM_RESULT==_sql.GetResult())
			{
				ASSERT_(FALSE);//illegal use of function. call CallSqlProc with return params
				_sql.Flush();
				while(_sql.Fetch())
				{
					if(is_stoped(_hStopEvent))
					{
						_sql.CancelAllCmd();
						return 0;
					}
				}
				continue;
			}
		}
	}
	return _sql.GetReturnValue(); 
}

/**\brief function to make sql call with stream to get from sql call.
You should specify Sql Layout for input, output parameters and stream data types.
\tparam _StreamContType -- container type of sql stream.
\tparam _PriorSql -- data base access class
\tparam _SyncObj -- sync type to syncronize set of input params (to lock access to 
                 to input params).
\param _sProcName -- name of procedure to call. if this paramter is empty than 
                     don`t set procedure name.
\param _outstream -- output stream holder (list of stream types)
\param _sql -- data base access object (ST)
\param _bcreateconnection -- flag to create connection. If you already create 
                             connection and pass it to this function than 
							 set this argument to false.
\param _hStopEvent -- stop event. Event to stop sql call and immediately return from this fucntion.
\param _syncobj -- syncronize object
\return sql return code. you can verify this code with \ref sql_success() like function.
*/
template<
	typename _StreamContType
	,typename _PriorSql
	,typename _SyncObj = NullType
	>
inline int CallSqlProcS(
	const CString_ _sProcName
	,_StreamContType& _outstream
	,_PriorSql& _sql
	,bool _bcreateconnection = true
	,HANDLE _hStopEvent = NULL
	,const _SyncObj& _syncobj = NullType()
	)
{
	typedef typename _StreamContType::value_type StreamType;

	const CSqlStructLayoutBase<StreamType,_PriorSql>& streamlayout = GetSqlStructLayout<StreamType,_PriorSql>();

	if(_bcreateconnection) _sql.CreateConnect();
	if(!_sProcName.IsEmpty()) _sql.SetProcCmd((LPTSTR)(LPCTSTR)_sProcName);
	if(_sql.Send())
	{
		while(_sql.Exec())
		{
			if(is_stoped(_hStopEvent))
			{
				_sql.CancelAllCmd();
				return 0;
			}
			if(CS_ROW_RESULT==_sql.GetResult())
			{
				_sql.Flush();

				StreamType databuf;
				CSqlTempBuffer tmpbufstream;
				streamlayout.prepare_buffer(databuf,tmpbufstream);
				tmpbufstream.create_buffer();
				streamlayout.bind(databuf,_sql,tmpbufstream,true);
				while(_sql.Fetch())
				{
					tmpbufstream.restart();
					streamlayout.update_frombuffer(databuf,tmpbufstream);
					{
						CAutoLockT<_SyncObj> __lock(_syncobj);
						_outstream.push_back(databuf);
					}
					if(is_stoped(_hStopEvent))
					{
						_sql.CancelAllCmd();
						return 0;
					}
				}
				tmpbufstream.restart();
				streamlayout.remove_frombuffer(databuf,tmpbufstream);
				continue;
			}
			if(CS_PARAM_RESULT==_sql.GetResult())
			{
				ASSERT_(FALSE);//illegal use of function. call CallSqlProc with return params
				_sql.Flush();
				while(_sql.Fetch())
				{
					if(is_stoped(_hStopEvent))
					{
						_sql.CancelAllCmd();
						return 0;
					}
				}
				continue;
			}
		}
	}
	return _sql.GetReturnValue(); 
}

/**\brief function to make sql call with stream to get from sql call.
You should specify Sql Layout for input, output parameters and stream data types.
\tparam _StreamContType -- container type of sql stream.
\tparam _PriorSql -- data base access class
\tparam _SyncObj -- sync type to syncronize set of input params (to lock access to 
                 to input params).
\param _sProcName -- name of procedure to call. if this paramter is empty than 
                     don`t set procedure name.
\param _outstream1 -- first output stream holder (list of stream types)
\param _outstream1 -- second output stream holder (list of stream types)
\param _sql -- data base access object (ST)
\param _bcreateconnection -- flag to create connection. If you already create 
                             connection and pass it to this function than 
							 set this argument to false.
\param _hStopEvent -- stop event. Event to stop sql call and immediately return from this fucntion.
\param _syncobj -- syncronize object
\return sql return code. you can verify this code with \ref sql_success() like function.
*/
template<
	typename _StreamContType1
	,typename _StreamContType2
	,typename _PriorSql
	,typename _SyncObj = NullType
	>
inline int CallSqlProcS2(
	const CString_ _sProcName
	,_StreamContType1& _outstream1
	,_StreamContType2& _outstream2
	,_PriorSql& _sql
	,bool _bcreateconnection = true
	,HANDLE _hStopEvent = NULL
	,const _SyncObj& _syncobj = NullType()
	)
{
	typedef typename _StreamContType1::value_type StreamType1;
	typedef typename _StreamContType2::value_type StreamType2;

	const CSqlStructLayoutBase<StreamType1,_PriorSql>& streamlayout1 = GetSqlStructLayout<StreamType1,_PriorSql>();
	const CSqlStructLayoutBase<StreamType2,_PriorSql>& streamlayout2 = GetSqlStructLayout<StreamType2,_PriorSql>();

	if(_bcreateconnection) _sql.CreateConnect();
	if(!_sProcName.IsEmpty()) _sql.SetProcCmd((LPTSTR)(LPCTSTR)_sProcName);

	long nStreamNumber = 0;

	if(_sql.Send())
	{
		while(_sql.Exec())
		{
			if(is_stoped(_hStopEvent))
			{
				_sql.CancelAllCmd();
				return 0;
			}
			if(CS_ROW_RESULT==_sql.GetResult())
			{
				_sql.Flush();

				nStreamNumber++;

				if(nStreamNumber==1)
				{
					StreamType1 databuf;
					CSqlTempBuffer tmpbufstream;
					streamlayout1.prepare_buffer(databuf,tmpbufstream);
					tmpbufstream.create_buffer();
					streamlayout1.bind(databuf,_sql,tmpbufstream,true);
					while(_sql.Fetch())
					{
						tmpbufstream.restart();
						streamlayout1.update_frombuffer(databuf,tmpbufstream);
						{
							CAutoLockT<_SyncObj> __lock(_syncobj);
							_outstream1.push_back(databuf);
						}
						if(is_stoped(_hStopEvent))
						{
							_sql.CancelAllCmd();
							return 0;
						}
					}
					tmpbufstream.restart();
					streamlayout1.remove_frombuffer(databuf,tmpbufstream);
				}
				else if(nStreamNumber==2)
				{
					StreamType2 databuf;
					CSqlTempBuffer tmpbufstream;
					streamlayout2.prepare_buffer(databuf,tmpbufstream);
					tmpbufstream.create_buffer();
					streamlayout2.bind(databuf,_sql,tmpbufstream,true);
					while(_sql.Fetch())
					{
						tmpbufstream.restart();
						streamlayout2.update_frombuffer(databuf,tmpbufstream);
						{
							CAutoLockT<_SyncObj> __lock(_syncobj);
							_outstream2.push_back(databuf);
						}
						if(is_stoped(_hStopEvent))
						{
							_sql.CancelAllCmd();
							return 0;
						}
					}
					tmpbufstream.restart();
					streamlayout2.remove_frombuffer(databuf,tmpbufstream);
				}
				continue;
			}
			if(CS_PARAM_RESULT==_sql.GetResult())
			{
				ASSERT_(FALSE);//illegal use of function. call CallSqlProc with return params
				_sql.Flush();
				while(_sql.Fetch())
				{
					if(is_stoped(_hStopEvent))
					{
						_sql.CancelAllCmd();
						return 0;
					}
				}
				continue;
			}
		}
	}
	return _sql.GetReturnValue(); 
}

/**\brief function to make sql call with output params and stream to get from sql call.
You should specify Sql Layout for input, output parameters and stream data types.
\tparam _ReturnParams -- output params for sql call
\tparam _StreamContType -- container type of sql stream.
\tparam _PriorSql -- data base access class
\tparam _SyncObj -- sync type to syncronize set of input params (to lock access to 
                 to input params).
\param _sProcName -- name of procedure to call. if this paramter is empty than 
                     don`t set procedure name.
\param _retparams -- output params 
\param _outstream -- output stream holder (list of stream types)
\param _sql -- data base access object (ST)
\param _bcreateconnection -- flag to create connection. If you already create 
                             connection and pass it to this function than 
							 set this argument to false.
\param _hStopEvent -- stop event. Event to stop sql call and immediately return from this fucntion.
\param _syncobj -- syncronize object
\return sql return code. you can verify this code with \ref sql_success() like function.
*/
template<
	typename _ReturnParams
	,typename _StreamContType
	,typename _PriorSql
	,typename _SyncObj = NullType
	>
inline int CallSqlProcOS(
	const CString_ _sProcName
	,_ReturnParams& _retparams
	,_StreamContType& _outstream
	,_PriorSql& _sql
	,bool _bcreateconnection = true
	,HANDLE _hStopEvent = NULL
	,const _SyncObj& _syncobj = NullType()
	)
{
	typedef typename _StreamContType::value_type StreamType; 

	const CSqlStructLayoutBase<_ReturnParams,_PriorSql>& retlayout = GetSqlStructLayout<_ReturnParams,_PriorSql>();
	const CSqlStructLayoutBase<StreamType,_PriorSql>& streamlayout = GetSqlStructLayout<StreamType,_PriorSql>();

	if(_bcreateconnection) _sql.CreateConnect();
	if(!_sProcName.IsEmpty()) _sql.SetProcCmd((LPTSTR)(LPCTSTR)_sProcName);
	CSqlTempBuffer tmpbuf;
	{
		CAutoLockT<_SyncObj> __lock(_syncobj);
		retlayout.prepare_buffer(_retparams,tmpbuf);
		tmpbuf.create_buffer();
		retlayout.set_param(_T(""),_retparams,_sql,tmpbuf);
	}
	if(_sql.Send())
	{
		while(_sql.Exec())
		{
			if(is_stoped(_hStopEvent))
			{
				_sql.CancelAllCmd();
				return 0;
			}
			if(CS_ROW_RESULT==_sql.GetResult())
			{
				_sql.Flush();

				StreamType databuf;
				CSqlTempBuffer tmpbufstream;
				streamlayout.prepare_buffer(databuf,tmpbufstream);
				tmpbufstream.create_buffer();
				streamlayout.bind(databuf,_sql,tmpbufstream,true);
				while(_sql.Fetch())
				{
					tmpbufstream.restart();
					streamlayout.update_frombuffer(databuf,tmpbufstream);
					{
						CAutoLockT<_SyncObj> __lock(_syncobj);
						_outstream.push_back(databuf);
					}
					if(is_stoped(_hStopEvent))
					{
						_sql.CancelAllCmd();
						return 0;
					}
				}
				tmpbufstream.restart();
				streamlayout.remove_frombuffer(databuf,tmpbufstream);
				continue;
			}
			if(CS_PARAM_RESULT==_sql.GetResult())
			{
				_sql.Flush();
				CAutoLockT<_SyncObj> __lock(_syncobj);

				tmpbuf.restart();
				retlayout.bind(_retparams,_sql,tmpbuf);
				while(_sql.Fetch())
				{
					tmpbuf.restart();
					retlayout.update_frombuffer(_retparams,tmpbuf);
					if(is_stoped(_hStopEvent))
					{
						_sql.CancelAllCmd();
						return 0;
					}
				}
				continue;
			}
		}
	}
	{
		CAutoLockT<_SyncObj> __lock(_syncobj);
		tmpbuf.restart();
		retlayout.remove_frombuffer(_retparams,tmpbuf); //for calling delete for classes
	}
	return _sql.GetReturnValue(); 
}

//#pragma warning(pop)


//@}

/**\page Page_QuickStart_PrioSqlTemplates Quick start: "Using of sql template to work with DB"

This page show how to use PriorSql templates to access to DB.

First we need to create sql layout.

So to create sql layout class you need: 
-# declare inner template class CSqlLayout
-# derived from CSqlStructLayoutBase
-# and in constructor specify sql parameters of this class
-# also you can specify names in a class derived from \ref AttributesBase
-# and use this names in sql layout specification.

Lets see example of sql layout creation.

\code

class CMainFilter  
{
public:
	double m_fClientID;
	double m_fUserID;
	Account m_acc;
	Date m_inportdatefrom;
	Date m_processdatefrom;
	Date m_inportdatetill;
	Date m_processdatetill;
	CString_ m_sBankFileName;
	CString_ m_sImportFileName;
	long m_nQueryType;
	long m_bIsImportError;
	long m_bIsProcessError;
	Date m_ImportDate;
	Date m_QueryTimeIn;
	Date m_QueryTimeOut;
	Date m_QueryTimeAnswer;
	long m_nQueryErrorCode;
	CString_ m_sDescription;

	//[4] -- also you can specify names in a class derived from \ref AttributesBase
	struct UniqueNames : public AttributesBase<CMainFilter,CString_>
	{
		UniqueNames()
		{
			add(&CMainFilter::m_fClientID,_T("ClientID"));
			add(&CMainFilter::m_fUserID,_T("UserID"));
			add(&CMainFilter::m_acc,_T("Acc"));
			add(&CMainFilter::m_inportdatefrom,_T("ImportDateFrom"));
			add(&CMainFilter::m_processdatefrom,_T("DateTimeINFrom"));
			add(&CMainFilter::m_inportdatetill,_T("ImportDateTo"));
			add(&CMainFilter::m_processdatetill,_T("DateTimeINTo"));
			add(&CMainFilter::m_sBankFileName,_T("BankFileName"));
			add(&CMainFilter::m_sImportFileName,_T("ImportFileName"));
			add(&CMainFilter::m_nQueryType,_T("QueryType"));
			add(&CMainFilter::m_bIsImportError,_T("WithImportError"));
			add(&CMainFilter::m_bIsProcessError,_T("WithProcessError"));
			add(&CMainFilter::m_QueryTimeIn,_T("QueryTimeIn"));
			add(&CMainFilter::m_QueryTimeOut,_T("QueryTimeOut"));
			add(&CMainFilter::m_QueryTimeAnswer,_T("QueryTimeAnswer"));
			add(&CMainFilter::m_nQueryErrorCode,_T("QueryErrorCode"));
			add(&CMainFilter::m_sDescription,_T("DefaultDescription"));
			add(&CMainFilter::m_ImportDate,_T("ImportDate"));
		}
	};//struct UniqueNames

	struct DefaultLayout : public Layout<CMainFilter>
	{
		DefaultLayout()
		{
			FieldNameFromAttr<CMainFilter,CMainFilter::UniqueNames> name_attrs;
			add_simple(name_attrs,&CMainFilter::m_fClientID);
			add_simple(name_attrs,&CMainFilter::m_fUserID);
			add_simple(name_attrs,&CMainFilter::m_acc);
			add_simple(name_attrs,&CMainFilter::m_inportdatefrom);
			add_simple(name_attrs,&CMainFilter::m_processdatefrom);
			add_simple(name_attrs,&CMainFilter::m_sBankFileName);
			add_simple(name_attrs,&CMainFilter::m_sImportFileName);
			add_simple(name_attrs,&CMainFilter::m_nQueryType);
			add_simple(name_attrs,&CMainFilter::m_bIsImportError);
			add_simple(name_attrs,&CMainFilter::m_bIsProcessError);
			add_simple(name_attrs,&CMainFilter::m_QueryTimeIn);
			add_simple(name_attrs,&CMainFilter::m_QueryTimeOut);
			add_simple(name_attrs,&CMainFilter::m_QueryTimeAnswer);
			add_simple(name_attrs,&CMainFilter::m_nQueryErrorCode);
			add_simple(name_attrs,&CMainFilter::m_sDescription);
		}
	};//struct DefaultLayout

	//[1] -- declare inner template class CSqlLayout
	template<typename _PriorSql>
		struct CSqlLayout : public CSqlStructLayoutBase<CMainFilter,_PriorSql> 
							//[2] -- derived from CSqlStructLayoutBase
	{
		CSqlLayout()
		{
			//[5] -- and use this names in sql layout specification
			FieldNameFromAttr<CMainFilter,CMainFilter::UniqueNames> name_attrs;

			//[5] -- and use this names in sql layout specification
			add_withverifier(name_attrs,&CMainFilter::m_fClientID,CSqlVerifyItemIfParamEmpty());
			add_withverifier(name_attrs,&CMainFilter::m_fUserID,CSqlVerifyItemIfParamEmpty());
			add_complex_withverifier(name_attrs,&CMainFilter::m_acc,GetSqlStructLayout<Account,_PriorSql>(),CSqlVerifyItemIfParamEmpty());
			add_withverifier(name_attrs,&CMainFilter::m_inportdatefrom,CSqlVerifyItemIfParamEmpty());
			add_withverifier(name_attrs,&CMainFilter::m_processdatefrom,CSqlVerifyItemIfParamEmpty());
			add_withverifier(name_attrs,&CMainFilter::m_inportdatetill,CSqlVerifyItemIfParamEmpty());
			add_withverifier(name_attrs,&CMainFilter::m_processdatetill,CSqlVerifyItemIfParamEmpty());
			add_withverifier(name_attrs,&CMainFilter::m_sBankFileName,250,CSqlVerifyItemIfParamEmpty());
			add_withverifier(name_attrs,&CMainFilter::m_sImportFileName,250,CSqlVerifyItemIfParamEmpty());
			add_withverifier(name_attrs,&CMainFilter::m_nQueryType,CSqlVerifyItemIfParamEmpty());
			add_withverifier(name_attrs,&CMainFilter::m_bIsImportError,CSqlVerifyItemIfParamEmpty());
			add_withverifier(name_attrs,&CMainFilter::m_bIsProcessError,CSqlVerifyItemIfParamEmpty());
		}
	};//template<> struct CSqlLayout

};//class CMainFilter

struct CFileFilter : public CMainFilter
{
	long m_nLineNumber;

	CFileFilter()
		:m_nLineNumber(-1)
	{
	}

	struct DefaultLayout : public Layout<CFileFilter>
	{
		DefaultLayout()
		{
			add_base(get_structlayout<CMainFilter>());
			add_simple(_T("LineNumber"),&CFileFilter::m_nLineNumber);
		}
	};
	//[1] -- declare inner template class CSqlLayout
	template<typename _PriorSql>
		struct CSqlLayout : public CSqlStructLayoutBase<CFileFilter,_PriorSql>	
							//[2] -- derived from CSqlStructLayoutBase
	{
		CSqlLayout()
		{
			//[3] -- and in constructor specify sql parameters of this class
			add_base(GetSqlStructLayout<CMainFilter,_PriorSql>());
			add_withverifier(_T("LineNumber"),&CFileFilter::m_nLineNumber,CSqlVerifyLineParam());
		}
	};

	void get_description(const CMainFilter& _main,CString_& _str) const;
};//struct CFileFilter

\endcode

As you can see you also can use \ref FieldNameFromAttr class to specify names and use then its 
in sql layout or default layout specification.


But in some cases we can`t edit some class (may be it is library class that we can`t
edit in any way) but we need to create sql layout class. What can we do in this case.
That alright you need just:
-# declare sql layout class 
-# link you sql layout class with data class (using macro \ref SQL_LAYOUT_EX())

Lets see code snippet for such case.

\code 

//[1] declare sql layout class.
template<typename _PriorSql>
struct CAccoutSqlLayout : public CSqlStructLayoutBase<Account,_PriorSql>
{
	CAccoutSqlLayout()
	{
		add(_T("Balance"),&Account::Balance);
		add(_T("Client"),&Account::Client);
		add(_T("Key"),&Account::Key);
	}
};//template<> struct CAccoutSqlLayout

//[2] link you sql layout class with data class
SQL_LAYOUT_EX(Account,CAccoutSqlLayout<PriorSqlSwitch>);

\endcode
*/



#endif//#if !defined(__PRIORSQLTEMPLATES_H__EF2970ED_08F3_4378_8F10_84B76D15D784)

