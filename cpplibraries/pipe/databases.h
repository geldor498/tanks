#if !defined(__DATABASES_H__1F6FA635_EE75_40a1_B9B2_3938834F5AC5)
#define __DATABASES_H__1F6FA635_EE75_40a1_B9B2_3938834F5AC5

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <utils\config.h>
#include <utils\utils.h>
#include <utils\atlmfc.h>
#include <utils\serialize.h>
#include <utils\convert.h>

#include "errors.h"
#include "pipe_utils.h"

/**\brief pipe data types (meta data)
*/
enum PipeDataMetaEn {
	PDM_Null=0				///< unknown type
	,PDM_Data				///< data type
	,PDM_GUIDofData			///< guid type. this is the mark of a new data start
	,PDM_Size
};

/**\brief interface that implements simple data operation with the pipe
implementation of every function of interface IPipeCommunicator should be reenterable
*/
interface IPipeCommunicator
{
	/**\brief function to write CString_ to pipe
	\param[in] _str -- string to write
	\return true is operation was done successfully
	*/
	virtual bool write(const CString_& _str) = 0;

	/**\brief function to read CString_ from pipe
	\param[out] _str -- readed string
	\return true is operation was done successfully
	*/
	virtual bool read(CString_& _str) const = 0;

	/**\brief function to write long value to pipe
	\param[in] _lv -- long valu to write
	\return true is operation was done successfully
	*/
	virtual bool write(long _lv) = 0;

	/**\brief function to read long value from pipe
	\param[out] _lv -- readed long value
	\return true is operation was done successfully
	*/
	virtual bool read(long& _lv) const = 0;

	/**\brief function to write double value to pipe
	\param[in] _dv -- double value to write
	\return true is operation was done successfully
	*/
	virtual bool write(const double& _dv) = 0;

	/**\brief function to read double value from pipe
	\param[out] _dv -- readed value
	\return true is operation was done successfully
	*/
	virtual bool read(double& _dv) const = 0;

	/**\brief function to write CTime_ value to pipe
	\param[in] _tv -- time value to write
	\return true is operation was done successfully
	*/
	virtual bool write(const CTime_& _tv) = 0;

	/**brief function to read CTime_ value from pipe
	\param[out] _tv -- readed CTime_ value
	\return true is operation was done successfully
	*/
	virtual bool read(CTime_& _tv) const = 0;

	/**\brief function to write into the pipe BLOB data
	\param[in] _blob -- blob data to write
	\return true is operation was done successfully
	*/
	virtual bool write(const CBlob& _blob) = 0;

	/**\brief function to read BLOB data from pipe
	\param[out] _blob -- readed BLOB data
	\return true is operation was done successfully
	*/
	virtual bool read(CBlob& _blob) const = 0;
	
	///-------------------------------------------------------------------------
	/// meta data read/write functions

	/**\brief function to write GUID data to pipe. 
	          GUIDs are identity of written data objects.
	\param[in] _guid -- guid to write
	\return true is operation was done successfully
	*/
	virtual bool write_guid(const GUID& _guid) = 0;

	/**\brief function to read GUID data from pipe.
	\param[out] _guid -- guid to read
	\return true is operation was done successfully
	*/
	virtual bool read_guid(GUID& _guid) const = 0;
};//struct IPipeCommunicator

/**\brief interface need to implement by all data objects to read/write from/to 
          pipe channel.
*/
interface IPipeDataBase
{
	/**\brief function to save current data object into pipe
	\param _ppipe -- pipe communication interface
	\return true is operation was done successfully
	*/
	virtual bool save(IPipeCommunicator* _ppipe) const = 0;

	/**\brief function to save GUID of current data object 
	\param _ppipe -- pipe communication interface
	\return true is operation was done successfully
	*/
	virtual bool save_guid(IPipeCommunicator* _ppipe) const = 0;

	/**\brief function that loads data object from pipe
	\param _ppipe -- pipe communication interface
	\return true is operation was done successfully
	*/
	virtual bool load(const IPipeCommunicator* _ppipe) = 0;
};//struct IPipeDataBase

//namespace pipeutils
//{
//	inline bool load_guid(const IPipeCommunicator* _ppipe,GUID& _guid)
//	{
//		CString_ tmp;
//		DWORD dwMetaData = PDM_Null;
//		if(!_ppipe->read(tmp,dwMetaData) && NEQL(dwMetaData,PDM_GUIDofData)) return false;
//		TRACE_(_T("GUID = %s\n"),(LPCTSTR)tmp);
//		if(!string_toguid(tmp,_guid)) return false;
//		return true;
//	}
//};//namespace pipeutils

/*
	maximum size of this structure save/load size should be less then 1 Mb
	see: sec_criticalbuffersize constant in namepipebase.h file
*/
template<typename _PipeDataClass>
struct CPipeDataBaseImpl : public IPipeDataBase
{
	virtual bool save_guid(IPipeCommunicator* _ppipe) const
	{
		return _ppipe->write_guid(_PipeDataClass::get_guid());
	}
};//struct CPipeDataBaseImpl

/**\brief pipe serialize data types enumeration
*/
enum PipeSerializeDataTypeEn {
	PSDT_Null = 0					///< null 
	,PSDT_DataEnd					///< signal end of data
	,PSDT_NodeBegin					///< begin of node. next string -- name of node
	,PSDT_NodeEnd					///< end of current node
	,PSDT_ValueStr					///< string value. next string -- value of node
	,PSDT_ValueBlob					///< blob value. next blob -- value of node
};

inline 
CString_ to_str(PipeSerializeDataTypeEn _pipeserializedatatype)
{
	typedef std::pair<PipeSerializeDataTypeEn,LPCTSTR> Descr;
#define DESCR(_enumvalue) Descr(_enumvalue,_T(#_enumvalue))
	static const Descr descrs[] = {
		DESCR(PSDT_Null)
		,DESCR(PSDT_DataEnd)
		,DESCR(PSDT_NodeBegin)
		,DESCR(PSDT_NodeEnd)
		,DESCR(PSDT_ValueStr)
		,DESCR(PSDT_ValueBlob)
	};
#undef DESCR

	long i=0;
	for(i=0;i<sizea(descrs);i++)
	{
		if(descrs[i].first==_pipeserializedatatype)
		{
			return descrs[i].second;
		}
	}
	static const LPCTSTR szUnknown = _T("[unknown]");
	return szUnknown;
}


/**\brief Pipe serialize item
This items is passed over pipe channel. and then list of them used to initialize 
data structures.
*/
struct CPipeSerializeItem
{
protected:
	PipeSerializeDataTypeEn m_type;				///< pipe serialize item type 
	CString_ m_strValue;						///< serialize item string value
	CBlob m_blobValue;							///< serialize item blob value
	long m_start_pos;							///< start position 
	long m_nodesize;							///< node length. this value helps to find closing node item for node begin
public:

	/**\brief default constructor
	*/
	CPipeSerializeItem()
		:m_type(PSDT_Null)
		,m_nodesize(1)
		,m_start_pos(-1)
	{
	}

	template<typename _Stream>
		void trace(_Stream& _stream,long& _shift) const
	{
		_stream << (LPCTSTR)shift(_shift);
		if(m_type==PSDT_NodeBegin)
			_stream << _T("<--- : ") << (LPCTSTR)to_str(m_type);
		else if(m_type==PSDT_NodeEnd) 
			_stream << _T(" : ") << (LPCTSTR)to_str(m_type) << _T("--->");
		else 
			_stream << _T(" : ") << (LPCTSTR)to_str(m_type);
		_stream  << std::endl;

		switch(m_type)
		{
		// no more data
		case PSDT_DataEnd:
			break;

		case PSDT_NodeEnd:
			_shift--;
			break;				

		//string data
		case PSDT_NodeBegin: 
			_stream << (LPCTSTR)shift(_shift) << _T("name : ") << (LPCTSTR)m_strValue << std::endl;
			_stream << (LPCTSTR)shift(_shift) << _T("start pos : ") << m_start_pos << std::endl;
			_stream << (LPCTSTR)shift(_shift) << _T("size :") << m_nodesize << std::endl;
			_shift++;
			break;

		case PSDT_ValueStr: 
			_stream << (LPCTSTR)shift(_shift) << _T("string value : ") << (LPCTSTR)m_strValue << std::endl;
			break;

		//blob data 
		case PSDT_ValueBlob:
			//if(!_ppipe->read(m_blobValue)) return false;
			_stream << (LPCTSTR)shift(_shift) << _T("blob value (...)") << std::endl;
			break;

		default:
			VERIFY_EXIT(FALSE);// unknown type
		}
	}

	void trace_str(CString_& _str,long& _shift) const
	{
		CString_ sShift = shift(_shift);
		_str += sShift;
		if(m_type==PSDT_NodeBegin)
		{
			_str +=	 _T("<--- : ");
			_str += to_str(m_type);
		}
		else if(m_type==PSDT_NodeEnd) 
		{
			_str += _T(" : ");
			_str += to_str(m_type);
			_str += _T("--->");
		}
		else 
		{
			_str += _T(" : ");
			_str += to_str(m_type);
		}
		_str += _T("\n");

		switch(m_type)
		{
			// no more data
		case PSDT_DataEnd:
			break;

		case PSDT_NodeEnd:
			_shift--;
			break;				

			//string data
		case PSDT_NodeBegin: 
		{
			_str += sShift;
			_str += _T("name : ");
			_str += m_strValue;
			_str += _T("\n");

			_str += sShift;
			_str += _T("start pos : ");
			_str += Format(_T("%d"),m_start_pos);
			_str += _T("\n");

			_str += sShift;
			_str += _T("size :");
			_str += Format(_T("%d"),m_nodesize);
			_str += _T("\n");
			_shift++;
			break;
		}

		case PSDT_ValueStr: 
		{
			_str += sShift;
			_str += _T("string value : ");
			_str += m_strValue;
			_str += _T("\n");
			break;
		}

		//blob data 
		case PSDT_ValueBlob:
		{
			_str += sShift;
			_str += _T("blob value (...)");
			_str += _T("\n");
			break;
		}

		default:
			VERIFY_EXIT(FALSE);// unknown type
		}
	}

	/**\brief function to initialize this pipe serialize item
	\param[in] _type -- item type
	\param[in] _pstr -- string pointer (if need)
	\param[in] _pblob -- blob pointer (if need)
	*/
	void initialize(PipeSerializeDataTypeEn _type,const CString_* _pstr = NULL,const CBlob* _pblob = NULL)
	{
		VERIFY_EXIT(EQL(m_type,PSDT_Null) && NEQL(_type,PSDT_Null));

		m_type = _type;

		if(NOT_NULL(_pstr))
		{
			m_strValue = *_pstr;
		}
		else if(NOT_NULL(_pblob))
		{
			m_blobValue = *_pblob;
		}
	}

	/**\brief function that return true if this item is type of Data End
	\return true if this item is type of Data End
	*/
	bool eod() const {return EQL(m_type,PSDT_DataEnd) || EQL(m_type,PSDT_Null);}

	/**\brief function return true if this item is a type of Node Begin
	\return true if this item is a type of Node Begin
	*/
	bool is_start_of_node() const {return EQL(m_type,PSDT_NodeBegin);}

	/**\brief function that return true if this item is a type of Node End
	\return true if this item is a type of Node End
	*/
	bool is_end_of_node() const {return EQL(m_type,PSDT_NodeEnd);}

	/**\brief function that return true if this node is a string value node
	\return true if this node is a string value node
	*/
	bool is_str_value() const {return EQL(m_type,PSDT_ValueStr);}

	/**\brief function that return true if this node is a blob value node
	\return true if this node is a blob value node
	*/
	bool is_blob_value() const {return EQL(m_type,PSDT_ValueBlob);}

	/**\brief function that return true if this node is a value node (no matter is it blob or string)
	\return true if this node is a blob value node
	*/
	bool is_value() const {return is_str_value() || is_blob_value();}

	/**\brief function that return start position of node
	*/
	long get_start() const 
	{
		VERIFY_EXIT1(is_start_of_node(),-1);
		return m_start_pos;
	} 

	/**\brief function that return node length (you can find end of this node by this value)
	This value is valid only for item of type \ref PSDT_NodeBegin
	\return node length
	*/
	long get_length() const 
	{
		VERIFY_EXIT1(is_start_of_node(),-1);
		return m_nodesize;
	}

	/**\brief fucntion that return node name
	This fucntion valid only for item of type \ref PSDT_NodeBegin
	\return node name 
	*/
	const CString_& get_name() const 
	{
		VERIFY_EXIT1(is_start_of_node(),Fish<CString_>());
		return m_strValue;
	}

	/**\brief function that return blob value of item
	This function is valid only for item of type \ref PSDT_ValueBlob
	\return blob value of this item
	*/
	const CBlob& get_blob() const 
	{
		VERIFY_EXIT1(is_blob_value(),Fish<CBlob>());
		return m_blobValue;
	}

	/**\brief function that return string value of item
	This function is valid only for item of type \ref PSDT_ValueStr
	\return string value of this item
	*/
	const CString_& get_value() const 
	{
		VERIFY_EXIT1(is_str_value(),Fish<CString_>());
		return m_strValue;
	}
	
	/**\brief function to set node length 
	This function valid only for item of type \ref PSDT_NodeBegin
	\param _len -- length to set
	*/
	void set_position(long _pos,long _len)
	{
		VERIFY_EXIT(is_start_of_node());
		m_start_pos = _pos;
		m_nodesize = _len;
	}

	/**\brief function that load item from pipe 
	\param _ppipe -- pipe interface to load from
	\return true if data was successfully loaded
	*/
	bool load(const IPipeCommunicator* _ppipe)
	{
		VERIFY_EXIT1(NOT_NULL(_ppipe),false);

		//TRACE_(_T("loading CPipeSerializeItem() object"));
		// load item type
		long type = PSDT_Null;
		if(!_ppipe->read(type)) return false;
		m_type = (PipeSerializeDataTypeEn)type;
		//TRACE_(_T("\ttype = %s\n"),(LPCTSTR)to_str(m_type));

		// load item data, if any
		switch(m_type)
		{
		case PSDT_NodeEnd:
			//TRACE_(_T("\tname = %s\n"),(LPCTSTR)m_strValue);
			break;

		// no more data
		case PSDT_DataEnd:
			break;				

		//string data
		case PSDT_NodeBegin: 
		case PSDT_ValueStr: 
		{
			if(!_ppipe->read(m_strValue)) return false;
			//TRACE_(_T("\tvalue = %s\n"),(LPCTSTR)m_strValue);
			break;
		}

		//blob data 
		case PSDT_ValueBlob:
		{
			if(!_ppipe->read(m_blobValue)) return false;
			//TRACE_(_T("\tblob (...)\n"));
			break;
		}

		default:
			VERIFY_EXIT1(FALSE,false);// unknown type
		}
		return true;
	}

	/**\brief function that save data to pipe 
	\param _ppipe -- pipe to save to
	\return true if data was successfully saved.
	*/
	bool save(IPipeCommunicator* _ppipe)
	{
		VERIFY_EXIT1(NOT_NULL(_ppipe),false);

		// save type
		long type = (long)m_type;
		//TRACE_(_T("save CPipeSerializeItem() object\n"));
		if(!_ppipe->write(type)) return false;
		//TRACE_(_T("\ttype = %s\n"),(LPCTSTR)to_str((PipeSerializeDataTypeEn)type));

		// save item data if any
		switch(m_type)
		{
		case PSDT_NodeEnd:
			//TRACE_(_T("name = %s\n"),(LPCTSTR)m_strValue);
			break;

		case PSDT_DataEnd:
			break;

		case PSDT_NodeBegin:
		case PSDT_ValueStr: 
		{
			//TRACE_(_T("\tvalue = %s\n"),(LPCTSTR)m_strValue);
			if(!_ppipe->write(m_strValue)) return false;
			break;
		}

		case PSDT_ValueBlob:
		{
			//TRACE_(_T("\tblob value\n"));
			if(!_ppipe->write(m_blobValue)) return false;
			break;
		}

		default:
			VERIFY_EXIT1(FALSE,false);// unknown type
		}
		return true;
	}

};//struct CPipeSerializeItem

typedef std::list<CPipeSerializeItem> PipeSerializeItemLst;		///< typedef of pipe items list

/**\brief struct to hold node data while loading of node
*/
struct CPipeNodeItem 
{
	/**\brief constructor
	*/
	CPipeNodeItem(const CString_& _sName = _T(""),long _currpos = 0)
		:m_sName(_sName)
		,m_start_pos(_currpos)
	{
	}

	CString_ m_sName;		///< node name
	long m_start_pos;		///< node start position
};

typedef std::list<CPipeNodeItem> PipeNodeItemsLsts;		///< typedef of pipe nodes descriptions stack

/**\brief class that helps to load save pipe items list
This class save data directly to pipe, but when it load data 
it first loads pipe node items in memory first and then use
this data to initialize data object. 
*/
struct CPipeSerializer
{
	/**\brief serializer mode
	*/
	enum ModeEn{
		Mode_SaveToPipe			///< mode to save data to pipe
		,Mode_LoadFromPipe		///< mode to load data from pipe
	};

	/**\brief Constructor.
	*/
	CPipeSerializer(ModeEn _Mode,const IPipeCommunicator* _ppipe)
		:m_Mode(_Mode)
		,m_ppipe(const_cast<IPipeCommunicator*>(_ppipe))
	{
		VERIFY_EXIT(NOT_NULL(_ppipe));
	}

	/**\brief function that return true if serializer work in save mode
	\return true if serializer work in save mode
	*/
	bool is_save_mode() const {return EQL(m_Mode,Mode_SaveToPipe);}

	/**\brief function that load list of all pipe items 
	\return true if list was successfully loaded
	*/
	bool load()
	{
		VERIFY_EXIT1(NOT_NULL(m_ppipe),false);
		VERIFY_EXIT1(EQL(m_Mode,Mode_LoadFromPipe),false);

		long shift = 1;
		long pos = 0;
		for(pos=0;;pos++)
		{
			CPipeSerializeItem item;
			if(!item.load(m_ppipe)) return false;

			//CString_ sInfo;
			//item.trace_str(sInfo,shift);
			//trace_string(sInfo);
			
			m_items.push_back(item); // add item to list

			if(item.eod())	// if end of data item
			{
				if(!m_loadstack.empty()) return false;
				break;
			}

			if(item.is_value()) continue; // if it is a valu node than just read more data

			if(item.is_start_of_node())	// if this is a start of node
			{
				// push to stack node info
				m_loadstack.push_back(CPipeNodeItem(item.get_name(),pos));
				continue;
			}

			if(item.is_end_of_node())	// if this is a end of node
			{
				if(m_loadstack.empty()) return false; // node nesting error 

				// set length of top node
				CPipeSerializeItem& item = const_cast<CPipeSerializeItem&>(arr(m_items)[m_loadstack.back().m_start_pos]);
				long startpos = m_loadstack.back().m_start_pos;
				item.set_position(startpos, pos - startpos);

				//TRACE_(_T("closing node %s\n"),(LPCTSTR)item.get_name());

				m_loadstack.erase(--m_loadstack.end());	// remove top item from nesting stack

				continue;
			}
		}
		VERIFY_EXIT1(m_loadstack.empty(),false);

		//CString_ str;
		//trace_str(str);
		//trace_string(str);
		return true;
	}

	CPipeSerializeItem* get_top()
	{
		VERIFY_EXIT1(!is_save_mode() && !m_items.empty(),NULL);
		return &m_items.front();
	}

	/**\brief function that return named root item
	\param[in] _sName -- node name to search
	\param[out] _pitem -- founded item pointer
	\return true if data was successfully found
	*/
	bool get_root(const CString_& _sName,CPipeSerializeItem*& _pitem)
	{
		return search(_sName,0,-1,_pitem);
	}

	/**\brief function that return named root item
	\param[in] _pitem -- item to search child of
	\param[in] _sName -- node name to search
	\param[out] _pitem -- finded item pointer
	\return true if data was successfully found
	*/
	bool get_child(const CPipeSerializeItem* _pitem,const CString_& _sName,CPipeSerializeItem*& _pchild)
	{
		VERIFY_EXIT1(
			NOT_NULL(_pitem)
				&& _pitem->is_start_of_node()
			,false
			);
		long start = _pitem->get_start();
		long len = _pitem->get_length();
		return search(_sName,start,start+len,_pchild);
	}

	bool get_string_value(const CPipeSerializeItem* _pitem,CString_& _str)
	{
		VERIFY_EXIT1(
			NOT_NULL(_pitem)
				&& _pitem->is_start_of_node()
			,false
			);
		long start = _pitem->get_start();
		long len = _pitem->get_length();
		return search_string_value(start,start+len,_str);
	}

	bool get_blob_value(const CPipeSerializeItem* _pitem,CBlob& _blob)
	{
		VERIFY_EXIT1(
			NOT_NULL(_pitem)
				&& _pitem->is_start_of_node()
			,false
			);
		long start = _pitem->get_start();
		long len = _pitem->get_length();
		return search_blob_value(start,start+len,_blob);
	}

	/**\brief function that force to save node start to pipe
	\param _sName -- node name
	\return true if data was successfully saved
	*/
	bool save_node(const CString_& _sName)
	{
		VERIFY_EXIT1(is_save_mode(),false);

		CPipeSerializeItem item;
		item.initialize(PSDT_NodeBegin,&_sName);
		return item.save(m_ppipe);
	}

	/**\brief function that force to save node end to pipe
	\return true if data was successfully saved
	*/
	bool save_node_end(const CString_& _sName)
	{
		VERIFY_EXIT1(is_save_mode(),false);

		CPipeSerializeItem item;
		item.initialize(PSDT_NodeEnd,&_sName);
		return item.save(m_ppipe);
	}

	/**\brief function that force to save data end to pipe
	\return true if data was successfully saved
	*/
	bool save_data_end()
	{
		VERIFY_EXIT1(is_save_mode(),false);

		CPipeSerializeItem item;
		item.initialize(PSDT_DataEnd);
		return item.save(m_ppipe);
	}

	/**\brief function that force to save node value to pipe
	\param _sValue -- string value to save
	\return true if data was successfully saved
	*/
	bool save_string_value(const CString_& _sValue)
	{
		VERIFY_EXIT1(is_save_mode(),false);

		CPipeSerializeItem item;
		item.initialize(PSDT_ValueStr,&_sValue);
		return item.save(m_ppipe);
	}

	/**\brief function that force to save node value to pipe
	\param _blob -- blob value to save
	\return true if data was successfully saved
	*/
	bool save_blob_value(const CBlob& _blob)
	{
		VERIFY_EXIT1(is_save_mode(),false);

		CPipeSerializeItem item;
		item.initialize(PSDT_ValueBlob,NULL,&_blob);
		return item.save(m_ppipe);
	}

	/**\breif function to count children items of _pitem named _sName 
	\param _pitem -- item to count children of 
	\param _sName -- nodes names to count 
	\return number of items that was found
	*/
	long count(const CPipeSerializeItem* _pitem,const CString_& _sName)
	{
		VERIFY_EXIT1(!is_save_mode() 
			&& NOT_NULL(_pitem)
			&& _pitem->is_start_of_node()
			,0
			);
	
		long start = _pitem->get_start();
		long len = _pitem->get_length();
		return count(start,start+len,_sName);
	}

	CPipeSerializeItem* getat(const CPipeSerializeItem* _pitem,const CString_& _sName,long _index)
	{
		VERIFY_EXIT1(!is_save_mode() 
			&& NOT_NULL(_pitem)
			&& _pitem->is_start_of_node()
			,0
			);
	
		long start = _pitem->get_start();
		long len = _pitem->get_length();
		return getat(start,start+len,_sName,_index);
	}

protected:

	bool search_string_value(long _start,long _end,CString_& _strValue)
	{
		_strValue.Empty();
		VERIFY_EXIT1(_start>=0 && _start<(long)m_items.size(),false);

		PipeSerializeItemLst::iterator it = m_items.begin();
		std::advance(it,_start);
		PipeSerializeItemLst::iterator ite = m_items.begin();
		if(_end<0 || _end>=(long)m_items.size()) 
		{
			_end = (long)m_items.size();
			ite = m_items.end();
		}
		else 
			std::advance(ite,_end);

		long pos = 0;
		bool bret = false;
		for(
			pos=_start+1,it++
			;pos<_end && it!=ite
			;++it,pos++
			)
		{
			CPipeSerializeItem& item = *it;
			if(item.eod()) break;
			if(item.is_str_value())
			{
				_strValue += item.get_value();
				bret |= true;
			}
			if(!item.is_start_of_node()) continue;

			//pass to end of node
			long len = item.get_length();
			pos += len;
			std::advance(it,len);
		}
		return bret;
	}

	bool search_blob_value(long _start,long _end,CBlob& _blobValue)
	{
		_blobValue.free();
		VERIFY_EXIT1(_start>=0 && _start<(long)m_items.size(),false);

		PipeSerializeItemLst::iterator it = m_items.begin();
		std::advance(it,_start);
		PipeSerializeItemLst::iterator ite = m_items.begin();
		if(_end<0 || _end>=(long)m_items.size()) 
		{
			_end = (long)m_items.size();
			ite = m_items.end();
		}
		else 
			std::advance(ite,_end);

		long pos = 0;
		for(
			pos=_start+1,it++
			;pos<_end && it!=ite
			;++it,pos++
			)
		{
			CPipeSerializeItem& item = *it;
			if(item.eod()) break;
			if(item.is_blob_value())
			{
				_blobValue = item.get_blob();
				return true;
			}
			if(!item.is_start_of_node()) continue;

			//pass to end of node
			long len = item.get_length();
			pos += len;
			std::advance(it,len);
		}
		return false;
	}

	/**\brief function that count all nodes in region [_start,_end] that is named as _sName
	\param[in] _start -- region start
	\param[in] _end -- region end
	\param[in] _sName -- nodes to count
	\return number of items that was found in the specified region
	*/
	long count(long _start,long _end,const CString_& _sName)
	{
		long cnt = 0;
		VERIFY_EXIT1(_start>=0 && _start<(long)m_items.size(),0);

		PipeSerializeItemLst::iterator it = m_items.begin();
		std::advance(it,_start);
		PipeSerializeItemLst::iterator ite = m_items.begin();
		if(_end<0 || _end>=(long)m_items.size()) 
		{
			_end = (long)m_items.size();
			ite = m_items.end();
		}
		else 
			std::advance(ite,_end);

		long pos = 0;
		for(
			pos=_start+1,it++
			;pos<_end && it!=ite
			;++it,pos++
			)
		{
			CPipeSerializeItem& item = *it;
			if(item.eod()) break;
			if(!item.is_start_of_node()) continue;

			if(_sName.IsEmpty() || !_sName.Compare(item.get_name())) cnt++;

			//pass to end of node
			long len = item.get_length();
			pos += len;
			std::advance(it,len);
		}

		return cnt;
	}

	CPipeSerializeItem* getat(long _start,long _end,const CString_& _sName,long _index)
	{
		VERIFY_EXIT1(_start>=0 || _start<(long)m_items.size() && _index>=0,NULL);

		PipeSerializeItemLst::iterator it = m_items.begin();
		std::advance(it,_start);
		PipeSerializeItemLst::iterator ite = m_items.begin();
		if(_end<0 || _end>=(long)m_items.size()) 
		{
			_end = (long)m_items.size();
			ite = m_items.end();
		}
		else 
			std::advance(ite,_end);

		long pos = 0;
		for(
			pos=_start+1,it++
			;pos<_end && it!=ite
			;++it,pos++
			)
		{
			CPipeSerializeItem& item = *it;
			if(item.eod()) break;
			if(!item.is_start_of_node()) continue;

			if(_sName.IsEmpty() || !_sName.Compare(item.get_name())) 
			{
				if(!_index--) return &item;
			}

			//pass to end of node
			long len = item.get_length();
			pos += len;
			std::advance(it,len);
		}

		return NULL;
	}

	/**\brief function to search 
	\param[in] _sName -- node name to search for
	\param[in] _start -- start index to search from
	\param[in] _end -- end index to search to
	\param[out] _pitem -- found item
	\return true if child item was successfully found
	*/
	bool search(const CString_& _sName,long _start,long _end,CPipeSerializeItem*& _pitem)
	{
		VERIFY_EXIT1(NOT_NULL(&_pitem),false);

		_pitem = NULL;

		VERIFY_EXIT1(_start>=0 && _start<(long)m_items.size(),false);

		PipeSerializeItemLst::iterator it = m_items.begin();
		std::advance(it,_start);
		PipeSerializeItemLst::iterator ite = m_items.begin();
		if(_end<0 || _end>=(long)m_items.size()) 
		{
			_end = (long)m_items.size();
			ite = m_items.end();
		}
		else 
			std::advance(ite,_end);

		long pos = 0;
		for(
			pos=_start+1,it++
			;pos<_end && it!=ite
			;++it,pos++
			)
		{
			CPipeSerializeItem& item = *it;
			if(item.eod()) break;
			if(!item.is_start_of_node()) continue;

			if(item.get_name()==_sName) 
			{
				// node is found, than return
				_pitem = &item;
				return true;
			}
			//pass to end of node
			long len = item.get_length();
			pos += len;
			std::advance(it,len);
		}

		return false;
	}

public:
	template<typename _Stream>
	void trace(_Stream& _stream) const 
	{
		_stream << _T("Loaded data ------------------------------------") << std::endl;
		_stream << _T("Items count : ") << m_items.size() << std::endl;
		PipeSerializeItemLst::const_iterator 
			it = m_items.begin()
			,ite = m_items.end()
			;
		long shift = 1;
		for(;it!=ite;++it)
		{
			it->trace_str(_stream,shift);
			_stream << std::endl;
		}
		_stream << _T("------------------------------------------------") << std::endl;
	}

	void trace_str(CString_& _str) const
	{
		_str.Empty();
		PipeSerializeItemLst::const_iterator 
			it = m_items.begin()
			,ite = m_items.end()
			;
		_str += Format(_T("item count = %d\n"),m_items.size());
		long shift = 1;
		for(;it!=ite;++it)
		{
			it->trace_str(_str,shift);
			_str += _T("\n");
		}
	}

protected:
	PipeSerializeItemLst m_items;				///< loaded items
	PipeNodeItemsLsts m_loadstack;				///< stack used while loading of data

	IPipeCommunicator* m_ppipe;					///< pipe communicator that used to save/load data
	ModeEn m_Mode;								///< serialize mode
};//struct CPipeSerializer

class CPipeNode : public IBlobNode
{
protected:
	CPipeSerializer& m_pipeserializer;						///< pipe serializer is used to write/read data 
	CPipeSerializeItem* m_pitem;							///< current item
	CString_ m_sName;										///< node name

public:
	/**\brief constructor
	\param _pipeserializer -- pipe serializer that used to save/load node
	\param _pitem -- 
	*/
	CPipeNode(
		CPipeSerializer& _pipeserializer
		,CPipeSerializeItem* _pitem
		,const CString_& _sName
		)
		:m_pipeserializer(_pipeserializer)
		,m_pitem(_pitem)
		,m_sName(_sName)
	{
		VERIFY_EXIT(!m_sName.IsEmpty() 
			&& NOT_NULL(&_pipeserializer) 
			);

		if(m_pipeserializer.is_save_mode())
			m_pipeserializer.save_node(m_sName);
		else
		{
			VERIFY_EXIT(NOT_NULL(_pitem) && _pitem->is_start_of_node());
		}
	}

	/**\brief virtual destructor to support free member data of derived classes.
	*/
	virtual ~CPipeNode() 
	{
		if(m_pipeserializer.is_save_mode())
			m_pipeserializer.save_node_end(m_sName);
	};

	/**\brief interface function that return name of node
	\return node name
	*/
	virtual CString_ get_name() const
	{
		return m_sName;
	}

	/**\brief interface function that returns value of the node (string)
	\return value of this node
	*/
	virtual CString_ get_value() const
	{
		VERIFY_EXIT1(!m_pipeserializer.is_save_mode(),Fish<CString_>());
		VERIFY_EXIT1(NOT_NULL(m_pitem),Fish<CString_>());
		CString_ str;
		if(!m_pipeserializer.get_string_value(m_pitem,str)) return Fish<CString_>();
		return str;
	}

	/**\brief function to set name of this node
	\param _sName -- name to set for this node
	*/
	virtual void set_name(const CString_& _sName)
	{
		ASSERT_(FALSE);	// this function should not used, because name was saved in constructor
		// there is no graceful implementation 
	}

	/**\brief function that sets value for this node
	\param _sValue -- new node value
	*/
	virtual void set_value(const CString_& _sValue)
	{
		m_pipeserializer.save_string_value(_sValue);
	}

	virtual void get_value(CBlob& _blob) const
	{
		VERIFY_EXIT(!m_pipeserializer.is_save_mode());
		VERIFY_EXIT(NOT_NULL(m_pitem));

		m_pipeserializer.get_blob_value(m_pitem,_blob); 
	}

	virtual void set_value(const CBlob& _blob)
	{
		m_pipeserializer.save_blob_value(_blob);
	}

	/**\brief return children list
	\param _sName -- children list items name
	\return children list for this node
	*/
	virtual INodeList* get_childlist(const CString_& _sName) const;

	/**\brief return children list
	\return children list of this node
	*/
	virtual INodeList* get_allchildlist() const;

	/**\brief return child node with specified name
	\param _sName -- name of child name
	\return \ref INode pointer or NULL if there is no child node with name specified.
	*/
	virtual INode* get_child(const CString_& _sName) const
	{
		if(m_pipeserializer.is_save_mode())
		{
			return NULL;
		}
		VERIFY_EXIT1(NOT_NULL(m_pitem),NULL);
		CPipeSerializeItem* pitem = NULL;
		if(!m_pipeserializer.get_child(m_pitem,_sName,pitem)) return NULL;
		return trace_alloc(new CPipeNode(m_pipeserializer,pitem,_sName));
	}

	/**\brief create new child with specified name
	\param _sName -- child node name to create
	\return \ref INode pointer of already created node
	*/
	virtual INode* add_child(const CString_& _sName)
	{
		VERIFY_EXIT1(m_pipeserializer.is_save_mode(),NULL);
		return trace_alloc(new CPipeNode(m_pipeserializer,NULL,_sName));
	}
};//class CPipeNode

class CPipeNodeList : public INodeList
{
protected:
	/**\brief constructor
	*/
	CPipeNodeList(
		CPipeSerializer& _pipeserializer
		,CPipeSerializeItem* _pParent
		,const CString_& _sName = _T("")
		)
		:m_pipeserializer(_pipeserializer)
		,m_pParent(_pParent)
		,m_sName(_sName)
		,m_count(0)
	{
	}

public:
	/**\brief virtual destructor to support correct data free of derived classes
	*/
	virtual ~CPipeNodeList() 
	{
	}

	/**\brief function that clears this children list
	*/
	virtual void clear()
	{
		VERIFY_EXIT(m_pipeserializer.is_save_mode());
		m_count = 0;
	}

	/**\brief function that return children count of this list
	*/
	virtual size_t size() const
	{
		return m_pipeserializer.count(m_pParent,m_sName);
	}

	/**\brief operator that return child node by it index
	\param _index -- index
	\return \ref INode pointer of child node or NULL if ther is no child node with specified index
	*/
	virtual INode* operator [] (long _index) const
	{
		if(!m_pipeserializer.is_save_mode())
		{
			CPipeSerializeItem* pitem = m_pipeserializer.getat(m_pParent,m_sName,_index);
			return trace_alloc(new CPipeNode(m_pipeserializer,pitem,m_sName));
		}
		else if(_index>=0 && _index<m_count)
		{
			return trace_alloc(new CPipeNode(m_pipeserializer,NULL,m_sName)); 
		}
		return NULL;
	}

	/**\brief function to remove item from nodes list
	\param _index -- item index to remove
	\return true if item was successfully removed
	*/
	virtual bool remove(long _index)
	{
		ASSERT_(FALSE);			// no gracefull implementation. no need to implement for pipe datas
		return false;
	}

	/**\brief function to allocate children items
	\param _size -- count of children to allocate
	*/
	virtual void alloc(size_t _size)
	{
		m_count = (long)_size;
	}

	/**\brief function that add child with specified name and value
	\param _sName -- child name to create
	\param _sValue -- child value to allocate
	\return \ref INode pointer of new child or NULL if failed to create new child node
	*/
	virtual INode* add(const CString_& _sValue)
	{
		ASSERT_(FALSE);		// there is no graceful implementation 
		return NULL;
	}

	/**\brief function that add child with specified name and value
	\param _sName -- child name to create
	\param _sValue -- child value to allocate
	\return \ref INode pointer of new child or NULL if failed to create new child node
	*/
	virtual INode* add(const CString_& _sName,const CString_& _sValue)
	{
		ASSERT_(FALSE);		// there is no graceful implementation 
		return NULL;
	}

	friend class CPipeNode;

protected:
	CPipeSerializer& m_pipeserializer;			///< pipe serializer is used to write/read data 
	CPipeSerializeItem* m_pParent;				///< parent node
	long m_count;
	CString_ m_sName;
};

inline
INodeList* CPipeNode::get_childlist(const CString_& _sName) const
{
	return trace_alloc(
			new CPipeNodeList(m_pipeserializer,m_pitem,_sName)
		);
}

inline
INodeList* CPipeNode::get_allchildlist() const
{
	return trace_alloc(
			new CPipeNodeList(m_pipeserializer,m_pitem)
		);
}

class CPipeArchive : public IArchive
{
public:
	/**\brief constructor
	*/
	CPipeArchive(CPipeSerializer& _pserializer)
		:m_pserializer(_pserializer)
	{
		VERIFY_EXIT(NOT_NULL(&_pserializer));
		m_proot = trace_alloc(
				new CPipeNode(_pserializer
				,(_pserializer.is_save_mode()?NULL:_pserializer.get_top())
				,_T("pipedata")
				)
			);
	}

	/**\brief virtual destructor to support correct data free of derived classes.
	*/
	virtual ~CPipeArchive() 
	{
		if(NOT_NULL(m_proot))
		{
			delete trace_free(m_proot);				//used pointer to specify items delete order, and a order to call destructors
			m_proot = NULL;
		}
		if(NOT_NULL(&m_pserializer))
		{
			if(m_pserializer.is_save_mode())
				m_pserializer.save_data_end();
		}
	}

	/**\brief function that return root node of archive
	\return \ref INode pointer of the root item for thas archive
	*/
	virtual INode* get_root() const
	{
		return const_cast<INode*>(static_cast<const INode*>(m_proot));
	}
protected:
	CPipeNode* m_proot;							///< archive root node 
	CPipeSerializer& m_pserializer;				///< pipe serializer used to read/write data into a pipe
};//class CPipeArchive

/**\brief pipe data object implementation for classes/structures that 
          have layout description that used to save/load data 
		  of data objects.
\tparam _PipeDataClass -- pipe data object type
*/
template<typename _PipeDataClass>
struct CPipeSerializedDataBaseImpl : public IPipeDataBase
{
	/**\brief simple function to save GUID
	*/
	virtual bool save_guid(IPipeCommunicator* _ppipe) const
	{
		return _ppipe->write_guid(_PipeDataClass::get_guid());
	}

	/**\brief implementation of \ref IPipeDataBase::save() for classes that have 
	          a declared layout.
	\param _ppipe -- pipe communication channel to save data to
	\return true if function saved data successfully
	\sa IPipeDataBase
	*/
	virtual bool save(IPipeCommunicator* _ppipe) const
	{
		CPipeSerializer pipesaver(CPipeSerializer::Mode_SaveToPipe,_ppipe);
		CPipeArchive arch(pipesaver);
		::save(arch,_T("data"),static_cast<const _PipeDataClass&>(*this),get_structlayout<_PipeDataClass>());
		return true;
	}

	/**\brief implementation of \ref IPipeDataBase::load() for classes
	          that have a declared layout.
	\param _ppipe -- pipe communication channel to save data to
	\return true if function loaded data successfully
	\sa IPipeDataBase
	*/
	virtual bool load(const IPipeCommunicator* _ppipe)
	{
		// load data from pipe
		CPipeSerializer pipeloader(CPipeSerializer::Mode_LoadFromPipe,_ppipe);
		if(!pipeloader.load()) return false;

		CPipeArchive arch(pipeloader);
		// load data into structure members
		::load(arch,_T("data"),static_cast<_PipeDataClass&>(*this),get_structlayout<_PipeDataClass>());
		return true;
	}
};//template<> struct CPipeSerializedDataBaseImpl

#endif //#if !defined(__DATABASES_H__1F6FA635_EE75_40a1_B9B2_3938834F5AC5)
