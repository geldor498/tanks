#if !defined(__CONVERT_H__AD9E4D50_793A_47c2_AA5C_1304ACB7AE7D_INCLUDED)
#define __CONVERT_H__AD9E4D50_793A_47c2_AA5C_1304ACB7AE7D_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "config.h"
#include "atlmfc.h"


#include "utils.h"
#include "strconv.h"

/**\defgroup UtilitiesConvert Utilities library support of convert data from one type to another

This library part support function \ref common::convert() to convert from one data type 
to another one. If you add some type you should also define common::convert() function 
for your type. If you would like to use it as primitive one in layout classes.
*/
//@{

namespace common
{
	template<typename _Type> inline
		CString_ hex2str(_Type _val)
	{
		return hex2str(_val,sizeof(_Type));
	}

	inline CString_ hex2str(LPVOID _val,long _fldsize)
	{
		return format(format(_T("0x%%0%dX"),_fldsize),_val);
	}

	inline CString_ hex2str(DWORD _val,long _fldsize)
	{
		return format(format(_T("0x%%0%dX"),_fldsize),_val);
	}
};//namespace common

template<typename _Type>
struct HexType
{
	HexType(const _Type& _val)
		:m_val(_val)
	{
	}

	_Type m_val;
};

template<typename _Type> inline
HexType<_Type> hextype(const _Type& _val)
{
	return HexType<_Type>(_val);
}


/**\brief private namespace of Utilities Libarary.
*/
namespace ns_private
{

/**\brief clas that represent string data divided by '\0' and ended with "\0\0". It is used at \ref CRegistryKey
*/
struct CMultiStrings
{
	/**\brief constructor
	\param _sz -- data that present multi string type
	*/
	CMultiStrings(LPCTSTR _sz,long _dwSize = -1):m_sz(_sz),m_nSize(_dwSize){}

	LPCTSTR m_sz;	///< multi string type pointer
	long m_nSize;	///< string size

	bool inside(long _pos) const
	{
		if(m_nSize>=0)
			return _pos>=0 && _pos<m_nSize;
		else 
			return _pos>=0;
	}

	bool empty() const
	{
		return IS_NULL(m_sz) || EQL(m_nSize,0);
	}

	std::pair<LPCTSTR,long> get_next(long _pos) const
	{
		if(_pos<0) _pos = 0;
		long lastpos = _pos,pos = _pos;
		if(m_nSize>=0 && pos>=m_nSize) return std::pair<LPCTSTR,long>((LPCTSTR)NULL,0);
		if(m_sz[pos]==_T('\0')) pos++;
		for(;m_sz[pos]!=_T('\0') && inside(pos); pos++) {}
		return std::pair<LPCTSTR,long>(m_sz+lastpos,pos-lastpos);
	}

	long get_length() const
	{
		if(m_nSize>=0) return m_nSize;
		long pos = 0;
		std::pair<LPCTSTR,long> p1((LPCTSTR)NULL,0);
		do
		{
			p1 = get_next(pos);
			pos += p1.second;
		}while(NOT_NULL(p1.first));
		return pos+1; 
	}
};

/**\brief The Class to represent blob data.	It is used at \ref CRegistryKey.
*/
struct CBlobWrap
{
	/**\brief Constructor.
	\param _ptr -- blob data pointer
	\param _sz -- blob data size
	*/
	CBlobWrap(LPVOID _ptr,DWORD _sz)
		:m_ptr(_ptr),m_sz(_sz)
	{}

	LPVOID m_ptr;		///< blob data pointer variable
	DWORD m_sz;			///< blob data size variable
};//struct CBlobWrap

};//namespace ns_private

/*
	declare strategies for data buffer allocations
*/

/**\brief class that declare startegy to allocate blob always in heep.
*/
struct CDynBlobAllocator
{
	/**\brief allocate function
	\param _sz -- size of data to allocate
	\return allocated buffer pointer
	*/
	LPVOID alloc(size_t _sz)
	{
		if(_sz<=0) return NULL;
		return trace_alloc(new BYTE[_sz]);
	}
	
	/**\brief function to free blob 
	\param _ptr -- pointer to free
	*/
	void free(LPVOID _ptr,size_t)
	{
		if(NOT_NULL(_ptr)) delete trace_free((LPBYTE)_ptr);
	}
};//struct CDynBlobAllocator

/**\brief clas that declare stategy to allocate vlob or in stack or in heep, depends it size 
\tparam _maxsize -- maximum size for stack allocation
*/
template<DWORD _maxsize = 1024>
struct CStackBlobAllocator
{
	/**\brief function to allocate blob
	\param _sz -- size to allocate 
	\return return allocated buffer pointer
	*/
	LPVOID alloc(size_t _sz)
	{
		if(_sz<=0) return NULL;
		else if(verify_size(_sz)) return &m_stackbuf;
		else return trace_alloc(new BYTE[_sz]);
	}
	
	/**\brief function to free allocated blob data
	\param _ptr -- pointer to blob to free
	\param _sz -- blob size
	*/
	void free(LPVOID _ptr,size_t _sz)
	{
		if(verify_size(_sz)) return;
		else if(NOT_NULL(_ptr)) delete trace_free(_ptr);
	}

protected:
	/**\brief function return true if used local buffer
	\return true if should use local buffer
	*/
	bool verify_size(size_t _sz)
	{
		return _sz<sizeof(m_stackbuf);
	}

	BYTE m_stackbuf[_maxsize];		///< local buffer
};//template<> struct CStackBlobAllocator

/*\brief Class that support blob data
\tparam _BlobAllocator - blob allocation strategy (default \ref CDynBlobAllocator)
*/
template<typename _BlobAllocator = CDynBlobAllocator>
struct CBlobT
{
	/**\brief Constructor
	*/
	CBlobT():m_ptr(NULL),m_sz(0){}

	/**\brief copy constructor
	\tparam _LocalAllocator -- memory allocator strategy
	\param _blob -- blob data to copy from
	*/
	template<typename _LocalAllocator>
		CBlobT(const CBlobT<_LocalAllocator>& _blob)
			:m_ptr(NULL),m_sz(0)
	{
		operator=(_blob);
	}

	CBlobT(const CBlobT<_BlobAllocator>& _blob)
		:m_ptr(NULL),m_sz(0)
	{
		operator=(_blob);
	}
	
	/**\brief Destructor
	*/
	~CBlobT()
	{
		free();
	}

	/**\brief assignment operator
	\tparam _LocalAllocator -- memory allocator strategy
	\param _blob -- blob data to copy from
	\return this reference
	*/
	template<typename _LocalAllocator>
		CBlobT<_BlobAllocator>& operator = (const CBlobT<_LocalAllocator>& _blob)
	{
		set((LPVOID)_blob,_blob.size());
		return *this;
	}

	CBlobT<_BlobAllocator>& operator = (const CBlobT<_BlobAllocator>& _blob)
	{
		set(_blob.m_ptr,_blob.m_sz);
		return *this;
	}

	/**\brief associate blob data. function that create buffer and then copy to it specified data
	\param _ptr -- buffer data pointer
	\param _sz -- buffer size
	*/
	void set(const LPVOID _ptr,size_t _sz)
	{
		free();
		if(IS_NULL(_ptr)) return;
		VERIFY_EXIT(_sz>0 && NOT_NULL(_ptr) && !IsBadReadPtr(_ptr,_sz));
		m_sz = _sz;
		m_ptr = m_allocator.alloc(m_sz);// new BYTE[m_sz];
		memcpy(m_ptr,_ptr,m_sz);
	}

	/**\brief function to allocate blob 
	\param _sz -- blob size to allocate
	\param _bzeromem -- flag to clear allocated buffer
	*/
	void alloc(size_t _sz,bool _bzeromem = true)
	{
		free();
		if(_sz<=0) return;
		m_sz = _sz;
		m_ptr = m_allocator.alloc(m_sz);
		if(_bzeromem) memset(m_ptr,0,m_sz);
	}

	/**\brief function that free blob 
	*/
	void free()
	{
		if(NOT_NULL(m_ptr)) m_allocator.free(m_ptr,m_sz); //delete m_ptr;
		m_ptr = NULL;
		m_sz = 0;
	}

	/**\brief function to clear content of this blob
	*/
	void clear()
	{
		free();
	}
	
	bool is_valid() const {return NOT_NULL(m_ptr);}

	bool operator==(int _null) const
	{
		VERIFY_EXIT1(_null==0,false);
		return is_valid();
	}

	bool operator != (int _null) const
	{
		return !(operator==(_null));
	}
	
	/**\buffer operator to return buffer pointer
	\return buffer pointer
	*/
	operator LPVOID () {return m_ptr;};		

	/**\buffer operator to return buffer pointer
	\return buffer pointer
	*/
	operator const LPVOID () const {return m_ptr;};		

	/**\brief operator to return buffer pointer
	\return bufefr pointer
	*/
	operator const BYTE* () const {return (const BYTE*)m_ptr;}

	/**\brief function to set blob buffer with datas from buffer
	\param _index -- byte index of blob to start set data from
	\param _data -- data buffer to fill the blob from
	\param _sz -- data size to set
	*/
	void set(long _index,const LPVOID _data,size_t _sz)
	{
		VERIFY_EXIT(NOT_NULL(m_ptr) && _index<m_sz && _index>=0 && NOT_NULL(_data));
		long sz = max(_sz,m_sz-_index);
		VERIFY_EXIT(!IsBadReadPtr(_data,_sz) && !IsBadWritePtr(&((LPBYTE)m_ptr)[_index],_sz));
		memcpy(&((LPBYTE)m_ptr)[_index],_data,sz);
	}

	/**\brief function that return blob size
	*/
	size_t size() const {return m_sz;}
protected:
	LPVOID m_ptr;						///< blob data buffer
	size_t m_sz;							///< blob size
	_BlobAllocator m_allocator;			///< blob allocator
};//struct CBlobT

typedef CBlobT<CDynBlobAllocator> CBlob; ///< typedef for default blob class
typedef CBlobT<CStackBlobAllocator<> > CBlobOnStack; ///< typedef for stack blob

/**\brief namespace of common parts of Utilities Library 
*/
namespace common
{
	/**\brief function that convert big andian DWORD to PC DWORD (little endian)
	\param bigEndian -- data to convert
	\return converted data
	*/
	inline
	DWORD ConvertBigEndian(DWORD bigEndian)
	{
		DWORD temp = 0;

		temp |= bigEndian >> 24;
		temp |= ((bigEndian & 0x00FF0000) >> 8);
		temp |= ((bigEndian & 0x0000FF00) << 8);
		temp |= ((bigEndian & 0x000000FF) << 24);

		return temp;
	}

	/**\brief function to convert from DWORD to CString_
	\param[in] _dw -- convert from
	\param[out] _str -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(DWORD _dw,CString_& _str)
	{
		_str = format(_T("%u"),_dw);
		return true;
	}

	/**\brief function to convert from DWORD to DWORD
	\param[in] _dw -- convert from
	\param[out] _val -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(DWORD _dw,DWORD& _val)
	{
		_val = _dw;
		return true;
	}

	/**\brief function to convert from DWORD to long
	\param[in] _dw -- convert from
	\param[out] _val -- convert to
	\return true if data was successfully converted
	*/
	inline 	bool convert(DWORD _dw,long& _val)
	{
		_val = _dw;
		return true;
	}

	/**\brief function to convert from DWORD to int
	\param[in] _dw -- convert from
	\param[out] _val -- convert to
	\return true if data was successfully converted
	*/
	inline 	bool convert(DWORD _dw,int& _val)
	{
		_val = _dw;
		return true;
	}

	/**\brief function to convert from DWORD to double
	\param[in] _dw -- convert from
	\param[out] _val -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(DWORD _dw,double& _dv)
	{
		_dv = _dw;
		return true;
	}

	/**\brief function to convert from DWORD to \ref StringLst
	\param[in] _dw -- convert from
	\param[out] _val -- convert to
	\return true if data was successfully converted
	*/
	inline 	bool convert(DWORD _dw,StringLst& _val)
	{
		_val.clear();
		CString_ str;
		if(!convert(_dw,str)) return false;
		_val.push_back(str);
		return true;
	}

	/**\brief function to convert from DWORD to \ref CBlobT
	\tparam _Type -- blob buffer allocation strategy
	\param[in] _dw -- convert from
	\param[out] _blob -- convert to
	\return true if data was successfully converted
	*/
	template<typename _Type>
	inline bool convert(DWORD _dw,CBlobT<_Type>& _blob)
	{
		_blob.set(&_dw,sizeof(_dw));
		return true;
	}

	/**\brief function to convert from int to \ref CBlobT
	\tparam _Type -- blob buffer allocation strategy
	\param[in] _iw -- convert from
	\param[out] _blob -- convert to
	\return true if data was successfully converted
	*/
	template<typename _Type>
	inline bool convert(int _iw,CBlobT<_Type>& _blob)
	{
		_blob.set(&_iw,sizeof(_iw));
		return true;
	}

	/**\brief function to convert from long to \ref CBlobT
	\tparam _Type -- blob buffer allocation strategy
	\param[in] _lw -- convert from
	\param[out] _blob -- convert to
	\return true if data was successfully converted
	*/
	template<typename _Type>
	inline bool convert(long _lw,CBlobT<_Type>& _blob)
	{
		_blob.set(&_lw,sizeof(_lw));
		return true;
	}

	inline bool convert(const CString_& _from,CString_& _to)
	{
		_to = _from;
		return true;
	}

	/**\brief function to convert from LPCWSTR to CString_
	\param[in] _sz -- convert from
	\param[out] _s -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(LPCWSTR _sz,CString_& _s)
	{
		string_converter<WCHAR,TCHAR,1024> str(_sz);
		_s = str;
		return true;
	}

	/**\brief function to convert from LPCSTR to CString_
	\param[in] _sz -- convert from
	\param[out] _s -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(LPCSTR _sz,CString_& _s)
	{
		string_converter<CHAR,TCHAR,1024> str(_sz);
		_s = str;
		return true;
	}

	/**\brief function to convert from LPCTSTR to long
	\param[in] _sz -- convert from
	\param[out] _dw -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(LPCTSTR _sz,unsigned long& _dw)
	{
		_dw = _tcstoul(_sz,NULL,10);
		return true;
	}
	
	/**\brief function to convert from long to long
	\param[in] _lvsrc -- convert from
	\param[out] _lv -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(long _lvsrc,long& _lv)
	{
		_lv = _lvsrc;
		return true;
	}

//	inline bool convert(unsigned long _lvsrc,unsigned long& _lv)
//	{
//		_lv = _lvsrc;
//		return true;
//	}

	/**\brief function to convert from float to long
	\param[in] _fv -- convert from
	\param[out] _lv -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(float _fv,long& _lv)
	{
		_lv = (long)_fv;
		return true;
	}


	/**\brief function to convert from LPCTSTR to long
	\param[in] _sz -- convert from
	\param[out] _lv -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(LPCTSTR _sz,long& _lv)
	{
		_lv = _tcstol(_sz,NULL,10);
		return true;
	}

	/**\brief function to convert from LPCTSTR to int
	\param[in] _sz -- convert from
	\param[out] _iv -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(LPCTSTR _sz,int& _iv)
	{
		_iv = _tcstol(_sz,NULL,10);
		return true;
	}

	/**\brief function to convert from LPCTSTR to short
	\param[in] _sz -- convert from
	\param[out] _iv -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(LPCTSTR _sz,short& _iv)
	{
		_iv = (short)_tcstol(_sz,NULL,10);
		return true;
	}

	/**\brief function to convert from LPCTSTR to unsigned short
	\param[in] _sz -- convert from
	\param[out] _iv -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(LPCTSTR _sz,unsigned short& _iv)
	{
		_iv = (unsigned short)_tcstol(_sz,NULL,10);
		return true;
	}

	/**\brief function to convert from LPCTSTR to double
	\param[in] _sz -- convert from
	\param[out] _dv -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(LPCTSTR _sz,double& _dv)
	{
		_dv = _tcstod(_sz,NULL);
		return true;
	}

	/**\brief function to convert from CString_ to double
	\param[in] _s -- convert from
	\param[out] _dv -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const CString_& _s,double& _dv)
	{
		_dv = _tcstod((LPCTSTR)_s,NULL);
		return true;
	}

	/**\brief function to convert from LPCTSTR to float
	\param[in] _sz -- convert from
	\param[out] _dv -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(LPCTSTR _sz,float& _dv)
	{
		_dv = (float)_tcstod(_sz,NULL);
		return true;
	}

	/**\brief function to convert from CString_ to float
	\param[in] _s -- convert from
	\param[out] _dv -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const CString_& _s,float& _dv)
	{
		_dv = (float)_tcstod((LPCTSTR)_s,NULL);
		return true;
	}

	/**\brief function to convert from LPCTSTR to StringLst
	\param[in] _sz -- convert from
	\param[out] _val -- convert to
	\return true if data was successfully converted
	*/
	inline 	bool convert(LPCTSTR _sz,StringLst& _val)
	{
		_val.clear();
		_val.push_back(_sz);
		return true;
	}

	/**\brief function to convert from LPCTSTR to \ref CBlobT
	\tparam _Type -- blob buffer allocation strategy
	\param[in] _sz -- convert from
	\param[out] _val -- convert to
	\return true if data was successfully converted
	*/
	template<typename _Type>
	inline bool convert(LPCTSTR _sz,CBlobT<_Type>& _blob)
	{
		_blob.set((LPVOID)_sz,(lstrlen(_sz)+1)*sizeof(TCHAR));
		return true;
	}

	/**\brief function to convert from \ref ns_private::CMultiStrings to \ref StringLst
	\param[in] _multisz -- convert from
	\param[out] _lst -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const ns_private::CMultiStrings& _multisz,StringLst& _lst)
	{
		_lst.clear();
		if(_multisz.empty()) return true;
		long pos = 0;
		std::pair<LPCTSTR,long> p1 = _multisz.get_next(pos);
		pos += p1.second;
		for(;NOT_NULL(p1.first);)
		{
			_lst.push_back(p1.first);
			p1 = _multisz.get_next(pos);
			pos += p1.second;
		}
		return true;
	}

	/**\brief function to convert from \ref ns_private::CMultiStrings to long
	\param[in] _multisz -- convert from
	\param[out] _lv -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const ns_private::CMultiStrings& _multisz,long& _lv)
	{
		if(_multisz.empty()) return true;
		long pos = 0;
		std::pair<LPCTSTR,long> p1 = _multisz.get_next(pos);
		if(NOT_NULL(p1.first))
			_lv = _tcstol(p1.first,NULL,10);
		else 
			_lv = 0;
		return NOT_NULL(p1.first);
	}

	/**\brief function to convert from \ref ns_private::CMultiStrings to unigned long
	\param[in] _multisz -- convert from
	\param[out] _lv -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const ns_private::CMultiStrings& _multisz,unsigned long& _lv)
	{
		if(_multisz.empty()) return true;
		long pos = 0;
		std::pair<LPCTSTR,long> p1 = _multisz.get_next(pos);
		if(NOT_NULL(p1.first))
			_lv = _tcstoul(p1.first,NULL,10);
		else 
			_lv = 0;
		return NOT_NULL(p1.first);
	}

	/**\brief function to convert from \ref ns_private::CMultiStrings to double
	\param[in] _multisz -- convert from
	\param[out] _fv -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const ns_private::CMultiStrings& _multisz,double& _fv)
	{
		if(_multisz.empty()) return true;
		long pos = 0;
		std::pair<LPCTSTR,long> p1 = _multisz.get_next(pos);
		if(NOT_NULL(p1.first))
			_fv = _tcstod(p1.first,NULL);
		else 
			_fv = 0;
		return NOT_NULL(p1.first);
	}

	/**\brief function to convert from \ref ns_private::CMultiStrings to CString_
	\param[in] _multisz -- convert from
	\param[out] _sv -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const ns_private::CMultiStrings& _multisz,CString_& _sv)
	{
		if(_multisz.empty()) return true;
		long pos = 0;
		std::pair<LPCTSTR,long> p1 = _multisz.get_next(pos);
		if(NOT_NULL(p1.first))
			_sv = CString_(p1.first,p1.second);
		else 
			_sv.Empty();
		return NOT_NULL(p1.first);
	}

	/**\brief function to convert from \ref ns_private::CMultiStrings to \ref CBlobT
	\tparam _Type -- blob buffer allocation strategy
	\param[in] _multisz -- convert from
	\param[out] _blob -- convert to
	\return true if data was successfully converted
	*/
 	template<typename _Type>
 	inline bool convert(const ns_private::CMultiStrings& _multisz,CBlobT<_Type>& _blob)
 	{
		_blob.set(_multisz.m_sz,_multisz.get_length());
		return true;
	}

	/**\brief function to convert from unsigned __int64 to DWORD
	\param[in] _sv -- convert from
	\param[out] _dw -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(unsigned __int64 _sv,DWORD& _dw)
	{
		_dw = (DWORD)_sv;
		return to_bool(_sv & ~ULONG_MAX);
	}

	/**\brief function to convert from unsigned __int64 to long
	\param[in] _sv -- convert from
	\param[out] _lv -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(unsigned __int64 _sv,long& _lv)
	{
		_lv = (long)_sv;
		return to_bool(_sv & ~LONG_MAX);
	}

	/**\brief function to convert from unsigned __int64 to int
	\param[in] _sv -- convert from
	\param[out] _lv -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(unsigned __int64 _sv,int& _lv)
	{
		_lv = (int)_sv;
		return to_bool(_sv & ~INT_MAX);
	}

	/**\brief function to convert from unsigned __int64 to CString_
	\param[in] _sv -- convert from
	\param[out] _v -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(unsigned __int64 _sv,CString_& _v)
	{
		_v = format(_T("I64u"),_sv);
		return true;
	}

	inline bool convert(const CString_& _sv,size_t& _v)
	{
		//_v = format(_T("I64u"),(__int64)_sv);
		_v = (size_t)_tcstoui64(_sv,NULL,10);
		return true;
	}

	/**\brief function to convert from unsigned __int64 to StringLst
	\param[in] _sv -- convert from
	\param[out] _lst -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(unsigned __int64 _sv,StringLst& _lst)
	{
		_lst.clear();
		CString_ str;
		if(!convert(_sv,str)) return false;
		_lst.push_back(str);
		return true;
	}

	/**\brief function to convert from unsigned __int64 to \ref CBlobT
	\tparam _Type -- blob buffer allocation strategy
	\param[in] _sv -- convert from
	\param[out] _blob -- convert to
	\return true if data was successfully converted
	*/
	template<typename _Type>
	inline bool convert(unsigned __int64 _sv,CBlobT<_Type>& _blob)
	{
		_blob.set(&_sv,sizeof(_sv));
		return true;
	}

	/**\brief function to convert from \ref ns_private::CBlobWrap to \ref CBlobT
	\tparam _Type -- blob buffer allocation strategy
	\param[in] _bw -- convert from
	\param[out] _blob -- convert to
	\return true if data was successfully converted
	*/
	template<typename _Type>
	inline bool convert(const ns_private::CBlobWrap& _bw,CBlobT<_Type>& _blob)
	{
		_blob.set(_bw.m_ptr,_bw.m_sz);
		return true;
	}

	/**\brief function to convert from \ref ns_private::CBlobWrap to long
	\remarks Not implemented.
	\tparam _Type -- blob buffer allocation strategy
	\param[in] _bw -- convert from
	\param[out] _blob -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const ns_private::CBlobWrap& _bw,long& _blob)
	{
		return false; //not implemented
	}

	/**\brief function to convert from \ref ns_private::CBlobWrap to int
	\remarks Not implemented.
	\tparam _Type -- blob buffer allocation strategy
	\param[in] _bw -- convert from
	\param[out] _blob -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const ns_private::CBlobWrap& _bw,int& _blob)
	{
		return false; //not implemented
	}

	/**\brief function to convert from \ref ns_private::CBlobWrap to DWORD
	\remarks Not implemented.
	\tparam _Type -- blob buffer allocation strategy
	\param[in] _bw -- convert from
	\param[out] _blob -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const ns_private::CBlobWrap& _bw,DWORD& _blob)
	{
		return false; //not implemented
	}

	/**\brief function to convert from \ref ns_private::CBlobWrap to DWORD
	\remarks Not implemented.
	\tparam _Type -- blob buffer allocation strategy
	\param[in] _bw -- convert from
	\param[out] _blob -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const ns_private::CBlobWrap& _bw,CString_& _blob)
	{
		return false; //not implemented
	}

	/**\brief function to convert from \ref ns_private::CBlobWrap to \ref StringLst
	\remarks Not implemented.
	\tparam _Type -- blob buffer allocation strategy
	\param[in] _bw -- convert from
	\param[out] _blob -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const ns_private::CBlobWrap& _bw,StringLst& _blob)
	{
		return false; //not implemented
	}

	/**\brief function to convert from \ref ns_private::CBlobWrap to \ref CBlobT
	\tparam _Type -- blob buffer allocation strategy
	\param[in] _lst -- convert from
	\param[out] _blob -- convert to
	\return true if data was successfully converted
	*/
	template<typename _Type>
	inline bool convert(const StringLst& _lst,CBlobT<_Type>& _blob)
	{
		long len = 0;
		StringLst::const_iterator it,ite;
		it = _lst.begin();
		ite = _lst.end();
		for(len=0;it!=ite;++it)	{len += it->GetLength()+1;}
		len += 1;

		_blob.alloc(len*sizeof(TCHAR));

		it = _lst.begin();
		DWORD pos = 0;
		for(;it!=ite;pos+=(it->GetLength()+1)*sizeof(TCHAR))
		{
			_blob.set(pos,(LPCVOID)(LPCTSTR)*it,(it->GetLength()+1)*sizeof(TCHAR));
		}
	}

	/**\brief function to convert from long to _str
	\param[in] _dw -- convert from
	\param[out] _str -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(int _dw,CString_& _str)
	{
		_str = format(_T("%d"),_dw);
		return true;
	}

	/**\brief function to convert from unsigned char to _str
	\param[in] _dw -- convert from
	\param[out] _str -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(unsigned char _dw,CString_& _str)
	{
		_str = format(_T("%d"),_dw);
		return true;
	}

	/**\brief function to convert from CString_ to unsigned char
	\param[in] _str -- convert from
	\param[out] _dw -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const CString_& _str,unsigned char& _dw)
	{
		_dw = (unsigned char)_tcstol(_str,NULL,10);
		return true;
	}

	/**\brief function to convert from CString_ to char
	\param[in] _str -- convert from
	\param[out] _dw -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const CString_& _str,char& _dw)
	{
		_dw = (unsigned char)_tcstol(_str,NULL,10);
		return true;
	}

	/**\brief function to convert from CString_ to char
	\param[in] _str -- convert from
	\param[out] _dw -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(long _dw,CString_& _str)
	{
		_str = format(_T("%d"),_dw);
		return true;
	}

	inline bool convert(unsigned int _dw,CString_& _str)
	{
		_str = format(_T("%u"),_dw);
		return true;
	}

	/**\brief function to convert from unsigned int to CString_
	\param[in] _dw -- convert from
	\param[out] _str -- convert to
	\return true if data was successfully converted
	*/
//	inline bool convert(unsigned int _dw,CString_& _str)
//	{
//		_str = format(_T("%u"),_dw);
//		return true;
//	}

	/**\brief function to convert from double to CString_
	\param[in] _fv -- convert from
	\param[out] _str -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(double _fv,CString_& _str)
	{
		_str = format(_T("%g"),_fv);
		return true;
	}

	/**\brief function to convert from float to CString_
	\param[in] _fv -- convert from
	\param[out] _str -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(float _fv,CString_& _str)
	{
		_str = format(_T("%g"),(double)_fv);
		return true;
	}

//	/**\brief function to convert from CString_ to CString_
//	\param[in] _from -- convert from
//	\param[out] _str -- convert to
//	\return true if data was successfully converted
//	*/
//	inline bool convert(const CString_& _from,CString_& _str)
//	{
//		_str = _from;
//		return true;
//	}

#if !defined(bool)	
	// XXX fuck some bool definer.

	/**\brief function to convert from bool to CString_
	\param[in] _bval -- convert from
	\param[out] _str -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(bool _bval,CString_& _str)
	{
		_str = bool2str(_bval);
		return true;
	}
#endif

	/**\brief function to convert from CString_ to bool
	\param[in] _sval -- convert from
	\param[out] _bres -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const CString_& _sval,bool& _bres)
	{
		CString_ sval = _sval;
		sval.TrimLeft();sval.TrimRight();
		_bres = !sval.CompareNoCase(_T("TRUE")) 
			|| _tcstol(sval,NULL,10)!=0
			;
		return true;
	}

	/**\brief function to convert from some pointer to CString_
	\tparam _Type -- pointer type
	\param[in] _ptrdata -- convert from
	\param[out] _str -- convert to
	\return true if data was successfully converted
	*/
	template<typename _Type>
	inline bool convert(const _Type* _ptrdata,CString_& _str)
	{
		if(IS_NULL(_ptrdata))
		{
			_str = _T("[NULL]");
			return true;
		}
		else return convert(*_ptrdata,_str);
	}

	/**\brief function to convert from GUID to CString_
	\param[in] _guid -- convert from
	\param[out] _str -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const GUID& _guid,CString_& _str)
	{
		guid_tostring(_guid,_str);
		return true;
	}

	/**\brief function to convert from CString_ to GUID
	\param[in] _str -- convert from
	\param[out] _guid -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const CString_& _str,GUID& _guid)
	{
		return string_toguid(_str,_guid);
	}

	template<typename _Type> inline 
		bool convert(const HexType<_Type>& _val,CString_& _str)
	{
		_str = hex2str(_val.m_val);
		return true;
	}

#if defined(USE_ATL) || defined(USE_MFC)
	//////////////////////////////////////////////////////////////////////////
	//
	// ATL or MFC specific
	//
#include <comutil.h>

	/**\brief function to convert from CRect to CString_
	\param[in] _rc -- convert from
	\param[out] _str -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const CRect_& _rc,CString_& _str)
	{
		_str = format(_T("(left=%d,top=%d - right=%d,bottom=%d)")
			,_rc.left,_rc.top,_rc.right,_rc.bottom
			);
		return true;
	}

	/**\brief function to convert from CPoint to CString_
	\param[in] _pt -- convert from
	\param[out] _str -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const CPoint_& _pt,CString_& _str)
	{
		_str = format(_T("CPoint(x=%d,y=%d)"),_pt.x,_pt.y);
		return true;
	}

	/**\brief function to convert from _bstr_t to CString_
	\param[in] _bstr -- convert from
	\param[out] _s -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const _bstr_t& _bstr,CString_& _s)
	{
		string_converter<WCHAR,TCHAR,1024> str(_bstr);
		_s = str;
		return true;
	}

	inline bool convert(const CTime_& _time,CString_& _s)
	{
		_s = _time.Format(_T("%c"));
		return true;
	}

#endif//#if defined(USE_ATL) || defined(USE_MFC)
	
#if defined(USE_MFC)
	//////////////////////////////////////////////////////////////////////////
	//
	// MFC specific
	//
	/**\brief function to convert from CWnd to CString_
	\param[in] _wnd -- convert from
	\param[out] _str -- convert to
	\return true if data was successfully converted
	*/
	inline bool convert(const CWnd& _wnd,CString_& _str)
	{
		CString_ s,s1;
		if(::IsWindow(_wnd)) _wnd.GetWindowText(s1);
		s = format(_T("CWnd (HWND = 0x%08x, text=\"\")"),(HWND)_wnd,(LPCTSTR)s1);
		_str = s;
		return true;
	}

#endif//#if defined(USE_MFC)


};//namespace common

//@}

/**\page Page_QuickStart_UtilitiesConvertDataTypes Quick start: "Utilities support of data conversion".


To convert some type to another just call \ref common::convert() 
with parameter as convert type from and a second parameter as a type convert to.

Lets see code snippet 
\code
void test()
{
	long i = 1;
	CString_ s;
	common::convert(i,s);
	ASSERT_(s==_T("1"));
}
\endcode
*/

#endif