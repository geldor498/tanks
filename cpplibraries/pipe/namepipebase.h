#if !defined(__NAMEDPIPEBASE_H__1F6FA635_EE75_40a1_B9B2_3938834F5AC5)
#define __NAMEDPIPEBASE_H__1F6FA635_EE75_40a1_B9B2_3938834F5AC5

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <list>
#include <malloc.h>

#include <utils/config.h>
#include <utils/atlmfc.h>
#include <utils/utils.h>
#include <utils/tracealloc.h>
#include <utils/osversion.h>
#include <utils\WinAPIExt.h>

#include "errors.h"
#include "pipe_utils.h"
#include "databases.h"

struct PipeNeedToReconnect {};
struct PipeOperationCanceled {};
struct PipeDisconnected
{
	PipeDisconnected(DWORD _dwErr):m_dwerr(_dwErr){};
	DWORD m_dwerr;
};

// don`t produce user error message
inline
void throw_needToReconect()
{
#if defined(USE_EXCEPTION_REFS)
	throw PipeNeedToReconnect();
#else
	throw trace_alloc(new PipeNeedToReconnect());
#endif
}

// don`t produce user error message
inline 
void throw_cancel()
{
#if defined(USE_EXCEPTION_REFS)
	throw PipeOperationCanceled();
#else
	throw trace_alloc(new PipeOperationCanceled());
#endif
}

// don`t produce user error message
inline 
void throw_disconnect(DWORD _dwErr)
{
#if defined(USE_EXCEPTION_REFS)
	throw PipeDisconnected(_dwErr);
#else
	throw trace_alloc(new PipeDisconnected(_dwErr));
#endif
}

/**\brief pipe buffer chunk class
*/
struct CPipeBufferChunk
{
	CPipeBufferChunk()
		:m_pbuf(NULL),m_readed(0),m_length(0),m_written(0)
	{}

	~CPipeBufferChunk()
	{
		free();
	}

	/**\breif function to allocate chunk 
	\param[in] _sz -- size of chunk to allocate
	\return true if chunk was allocated successfully
	*/
	bool alloc(DWORD _sz)
	{
		VERIFY_EXIT1(IS_NULL(m_pbuf) && EQL(m_readed,0) && EQL(m_length,0),false);
		VERIFY_EXIT1(NEQL(_sz,0),false);

		m_pbuf = trace_alloc(new LPBYTE[_sz]);
		m_readed = 0;
		m_written = 0;
		m_length = _sz;
		ZeroMemory(m_pbuf,m_length);
		return true;
	}

	/**\brief function to return block (placed in that chunk) size
	*/
	DWORD get_readlength() const 
	{
		return m_length - m_readed;
	}

	DWORD get_writelength() const
	{
		return m_length - m_written;
	}

	/**\brief move pointer of chunk by param \ref _sz 
	\param[in] _sz -- size to move chunk position by
	\return true if function successfully move chunk position
	*/
	bool readed(DWORD _sz)
	{
		VERIFY_EXIT1(NOT_NULL(m_pbuf),false);
		m_readed += _sz;
		VERIFY_EXIT1(m_readed<=m_length,(m_readed=m_length,false));
		return true;
	}

	/**\brief function to move chunk buffer position while writting data
	\param[in] _sz -- value to move chunk position while writting data
	\return true if function successfully move chunk position
	*/
	bool written(DWORD _sz)
	{
		VERIFY_EXIT1(NOT_NULL(m_pbuf),false);
		ASSERT_(m_written+_sz<=m_length);
		m_written += _sz;
		VERIFY_EXIT1(m_written<=m_length,(m_written=m_length,false));
		return true;
	}

	/**\brief function return true if chunk is full (position at the chunk buffer end)
		      verify is last chunk was filled up. You should allocate new one
	*/
	bool is_chunkfull() const
	{
		return NOT_NULL(m_pbuf)
			&& m_length>0
			&& m_written>=m_length
			;
	}

	operator bool () const { return !(IS_NULL(m_pbuf) || m_readed>=m_length);}
	bool operator !() const {return !operator bool();}

	operator LPVOID () {return m_pbuf;}
	operator const LPVOID () const {return m_pbuf;}


//	bool is_bad2write() const 
//	{
//		return get_writelength() < 2*sizeof(DWORD)
//			|| EQL(get_firstDWORD(),0)
//			;
//	}
//
//	bool is_bad2read() const 
//	{
//		return get_readlength() < 2*sizeof(DWORD)
//			|| EQL(get_firstDWORD(),0)
//			;
//	}

	bool operator==(const CPipeBufferChunk& _) const
	{
		return EQL(m_pbuf,_.m_pbuf)
			&& m_readed==_.m_readed
			;
	}

	bool operator!=(const CPipeBufferChunk& _) const
	{
		return !operator==(_);
	}

	bool operator<(const CPipeBufferChunk& _) const
	{
		return m_pbuf<_.m_pbuf;
	}

	bool operator>(const CPipeBufferChunk& _) const
	{
		return m_pbuf>_.m_pbuf;
	}

	DWORD first() const {return m_readed;}

	LPVOID get_writestart() const 
	{
		if(!*this) return NULL;
		return (LPBYTE)m_pbuf + m_written;
	}

	LPVOID get_readstart() const 
	{
		if(!*this) return NULL;
		return (LPBYTE)m_pbuf + m_readed;
	}

	/**\brief used with \ref CPipeBufferChunk::readed()
	*/
	bool is_allreaded() const 
	{
		return NOT_NULL(m_pbuf) && m_readed>=m_length;
	}

protected:

//	DWORD get_firstDWORD() const
//	{
//		VERIFY_EXIT1(*this,0);
//		return *((const DWORD*)((LPBYTE)m_pbuf + m_readed));
//	}

	void free()
	{
		if(NOT_NULL(m_pbuf)) 
		{
			delete[] trace_free(m_pbuf);
		}
		m_pbuf = NULL;
		m_readed = 0;
		m_length = 0;
	}

	LPVOID m_pbuf;			///< chunk buffer pointer
	DWORD m_readed;			///< readed position
	DWORD m_length;			///< chunk length
	DWORD m_written;		///< written size
};//struct CPipeBufferChunk

template<typename _Type>
_Type get_first(const CPipeBufferChunk& _chunk,DWORD _shift = 0)
{
	if(!_chunk) return _Type();
	return *((const _Type*)((LPBYTE)((const LPVOID)_chunk)+_chunk.first()+_shift));
}

typedef std::list<CPipeBufferChunk> PipeBufferChunkLst;

struct CPipeReadedBuffer
{
protected:
	PipeBufferChunkLst m_chunklst;
public:
	enum PipeBufferErrorsEn {
		PBE_Success = 0
		,PBE_InvalidArg
		,PBE_InvalidMemoryPointer
		,PBE_WrongBufferSize
		,PBE_DataNotReady
		,PBE_Empty
	};

	/**\brief addnew chunk to buffer
	*/
	LPVOID alloc_chunk(DWORD _sz)
	{
		m_chunklst.push_back(CPipeBufferChunk());
		if(!m_chunklst.back().alloc(_sz)) return NULL;
		return m_chunklst.back();
	}

	/**\brief modify last chunk written position 
	*/
	bool written_lastchunk(DWORD _sz)
	{
		VERIFY_EXIT1(!is_empty(),false);
		return m_chunklst.back().written(_sz);
	}

	/**\brief return true if last chunk was filled up
	*/
	bool is_lastchunkfull() const
	{
		VERIFY_EXIT1(!is_empty(),false);
		return m_chunklst.back().is_chunkfull();
	}

	/**\brief return data size
	*/
	DWORD get_firstBlockSendedSize() const
	{
		if(is_empty()) return 0;

		if(m_chunklst.front().get_readlength()<sizeof(DWORD)*2)
		{
			ASSERT_(FALSE);	// invalid data block or invalid chunk size
			return get_badalligned_dword(sizeof(DWORD));
		}
		else
			return get_first<DWORD>(m_chunklst.front(),sizeof(DWORD));
	}

	/**\brief return data meta type
	*/
	DWORD get_firstBlockMetaData() const
	{
		if(is_empty()) return PDM_Null;

		if(m_chunklst.front().get_readlength()<sizeof(DWORD))
		{
			ASSERT_(FALSE);	// invalid data block or invalid chunk size
			return get_badalligned_dword();
		}
		else
			return get_first<DWORD>(m_chunklst.front());
	}

	/**\brief return block real size (readed buf size)
	*/
	DWORD get_firstBlockRealSize() const
	{
		if(is_empty()) return 0;
		DWORD sendedsz = get_firstBlockSendedSize();

		DWORD sz = (DWORD)(neg(sizeof(DWORD)*2));
		PipeBufferChunkLst::const_iterator it,ite;
		it = m_chunklst.begin();
		ite = m_chunklst.end();
		for(;it!=ite;++it)
		{
			const CPipeBufferChunk& pipechunk = *it;
			sz += pipechunk.get_readlength();
			if(sz>=sendedsz) return sendedsz;
		}
		return sz;
	}

	/**\brief 
	*/
	LPVOID get_lastWriteStart()
	{
		if(is_empty()) return NULL;
		return m_chunklst.back().get_writestart();
	}

	DWORD get_lastWriteLength() const 
	{
		if(is_empty()) return 0;
		return m_chunklst.back().get_writelength();
	}

	bool is_empty() const
	{
		return m_chunklst.empty();
	}

	PipeBufferErrorsEn read(LPVOID& _pbuf,DWORD& _sz,DWORD& _dwMetaData,bool _balloc = false)
	{
		//verify arguments
		if(_balloc) 
		{
			if(NOT_NULL(_pbuf)) return PBE_InvalidArg;
		}
		else
		{
			if(IS_NULL(_pbuf)) return PBE_InvalidArg;
			if(::IsBadWritePtr(_pbuf,_sz)) return PBE_InvalidMemoryPointer;
		}

		_dwMetaData = get_firstBlockMetaData();
		DWORD dwSendedSize = get_firstBlockSendedSize();
		DWORD dwRealSize = get_firstBlockRealSize();
		if(
			EQL(_dwMetaData,PDM_Null) 
			|| NEQL(dwRealSize,dwSendedSize)
			)
			return PBE_DataNotReady;	//can read only full data block not a chunks

		if(_balloc)
		{
			// beware memory leaks 
			_sz = dwSendedSize;
			_pbuf = trace_alloc(new BYTE[_sz]);
			ZeroMemory(_pbuf,_sz);
		}
		else if(NEQL(_sz,dwSendedSize)) 
		{
			if(NOT_NULL(_pbuf) && _balloc) 
			{
				delete trace_free(_pbuf);
				_pbuf = NULL;
			}
			return PBE_WrongBufferSize; // can`t read a block with the different size
		}

		DWORD dwSize = dwSendedSize;
		DWORD dwAlignedSize = align(dwSize,unsigned __int64());
		DWORD offs = sizeof(DWORD)*2;

		//coping first block
		PipeBufferChunkLst::iterator it,ite;
		it = m_chunklst.begin();
		ite = m_chunklst.end();

		// process offset (offs)
		for(;it!=ite && offs>0;)
		{
			CPipeBufferChunk& pipechunk = *it;
			long length = min(pipechunk.get_readlength(),offs);
			pipechunk.readed(length);
			offs -= length;
			if(pipechunk.is_allreaded()) ++it;
		}

		// process data
		DWORD pos = 0;
		for(;it!=ite && pos<dwSize;)
		{
			CPipeBufferChunk& pipechunk = *it;
			long length = min(pipechunk.get_readlength(),dwSize-pos);
			if(pos+length>dwSize)
			{
				ASSERT_(FALSE);
				pos += length;
				continue;
			}
			memcpy((LPVOID)((LPBYTE)_pbuf+pos),(LPBYTE)pipechunk.get_readstart(),length);
			pipechunk.readed(length); //mark data as readed
			pos += length;
			if(pipechunk.is_allreaded()) ++it;
		}

		//process align
		if(dwAlignedSize>dwSize)
		{
			for(;it!=ite && pos<dwAlignedSize;)
			{
				CPipeBufferChunk& pipechunk = *it;
				long length = min(pipechunk.get_readlength(),dwAlignedSize-pos);
				if(pos+length>dwAlignedSize)
				{
					ASSERT_(FALSE);
					pos += length;
					continue;
				}
				pipechunk.readed(length); //mark data as readed
				pos += length;
				if(pipechunk.is_allreaded()) ++it;
			}
		}

		//free readed block
		it = m_chunklst.begin();
		for(;it!=m_chunklst.end() && it->is_allreaded();)
			it = m_chunklst.erase(it);
		return PBE_Success;
	}

	PipeBufferErrorsEn free_firstBlock_Always()
	{
		return free_firstBlock0();
	}

	PipeBufferErrorsEn free_firstBlock()
	{
		if(is_empty()) return PBE_Empty;
		PipeDataMetaEn MetaType = (PipeDataMetaEn)get_firstBlockMetaData();
		DWORD dwSendedSize = get_firstBlockSendedSize();
		DWORD dwRealSize = get_firstBlockRealSize();
		if(EQL(MetaType,PDM_Null) || NEQL(dwRealSize,dwSendedSize)) 
			return PBE_DataNotReady;	//can read only full data block not a chunks
		return free_firstBlock0();
	}

protected:
	DWORD get_badalligned_dword(long _offset = 0) const
	{
		DWORD buf = 0;
		PipeBufferChunkLst::const_iterator it = m_chunklst.begin();
		long len1 = it->get_readlength();
		if(len1<_offset)
		{
			++it;
			VERIFY_EXIT1(it!=m_chunklst.end(),0);
			memcpy(&buf,(LPBYTE)it->get_readstart()+_offset-len1,sizeof(DWORD));
		}
		else if(len1<_offset+(long)sizeof(DWORD))
		{
			long len2 = len1-_offset;
			memcpy(&buf,(LPBYTE)it->get_readstart()+_offset,len2);
			++it;
			memcpy(&buf,it->get_readstart(),sizeof(DWORD)-len2);
		}
		else
		{
			buf = get_first<DWORD>(m_chunklst.front(),_offset);
		}
		return buf;
	}

protected:
	PipeBufferErrorsEn free_firstBlock0()
	{
		if(is_empty()) return PBE_Empty;
		DWORD dwSendedSize = get_firstBlockSendedSize();
		PipeDataMetaEn MetaData = (PipeDataMetaEn)get_firstBlockMetaData();
		if(EQL(MetaData,PDM_Null)) return PBE_Empty;
		DWORD dwRealSize = get_firstBlockRealSize();
		
		DWORD dwSize = dwSendedSize + sizeof(DWORD)*2;

		PipeBufferChunkLst::iterator it,ite;
		it = m_chunklst.begin();
		ite = m_chunklst.end();
		DWORD pos = 0;
		for(;it!=ite && pos<dwSize;++it)
		{
			CPipeBufferChunk& pipechunk = *it;
			long length = min(pipechunk.get_readlength(),dwSize-pos);
			if(pos+length>dwSize)
			{
				ASSERT_(FALSE);
				pos += length;
				continue;
			}
			pipechunk.readed(length); //mark data as readed
			pos += length;
		}

		//free readed block
		it = m_chunklst.begin();
		ite = m_chunklst.end();
		for(;it!=ite && it->is_allreaded();)
			it = m_chunklst.erase(it);
		return PBE_Success;
	}
};//struct CPipeReadedBuffer

inline
long get_widestrlength(const CHAR* _str,long _len)
{
	return ::MultiByteToWideChar(CP_ACP,0,_str,_len,NULL,NULL);
}

inline
long get_widestrlength(WCHAR* _str,long _len)
{
	return _len;
}


#pragma pack(push,1)
struct DateInfo
{
	long year;
	long month;
	long day;
	long hour;
	long minute;
	long second;
	long millisec;

	DateInfo(long _year = 0,long _month = 0,long _day = 0
		,long _hour = 0,long _minute = 0,long _second = 0,long _millisec = 0
		)
		:year(_year),month(_month),day(_day)
		,hour(_hour),minute(_minute),second(_second)
		,millisec(_millisec)
	{
	}
};//struct DateInfo
#pragma pack(pop)

template<typename _Type>
struct CPipeCommunicatorImpl 
:
	public IPipeCommunicator
{
	virtual bool write(const CString_& _str)
	{
		string_converter<TCHAR,WCHAR> converted(_str);
		DWORD dwMetaData = PDM_Data;
		return static_cast<_Type*>(this)->write_pipe((LPVOID)(LPCWSTR)converted,converted.get_length()*sizeof(WCHAR),dwMetaData);
	}

	virtual bool read(CString_& _str) const
	{
		LPVOID pdata = NULL;
		DWORD sz = 0;
		DWORD dwMetaData = PDM_Null;
		bool bret = const_cast<_Type*>(static_cast<const _Type*>(this))->read_pipe(pdata,sz,dwMetaData,true);
		if(bret && EQL(dwMetaData,PDM_Data))
		{
			string_converter<WCHAR,TCHAR> strT((WCHAR*)pdata,sz/sizeof(WCHAR));
			_str = CString_((LPCTSTR)strT,strT.get_length());

			if(pdata) delete trace_free(pdata);
			pdata = NULL;
		}
		else 
		{
			if(pdata) delete trace_free(pdata);
			pdata = NULL;

#if defined(USE_EXCEPTION_ERRORHANDLING)
			throw_pipe_error(PipeError_ReadFail,_T("Invalid pipe data sequence or read error"));
#endif
			bret = false;
		}

		return bret;
	}

	virtual bool write(const CBlob& _blob)
	{
		DWORD dwMetaData = PDM_Data;
		bool bret = static_cast<_Type*>(this)->write_pipe((LPVOID)_blob,(DWORD)_blob.size(),dwMetaData);
		if(!bret)
		{
#if defined(USE_EXCEPTION_ERRORHANDLING)
			throw_pipe_error(PipeError_WriteFail,_T("Pipe write error"));
#endif
		}
		return bret;
	}

	virtual bool read(CBlob& _blob) const
	{
		LPVOID pdata = NULL;
		DWORD sz = 0;
		DWORD dwMetaData = PDM_Null;
		bool bret = const_cast<_Type*>(static_cast<const _Type*>(this))->read_pipe(pdata,sz,dwMetaData,true);
		if(bret && EQL(dwMetaData,PDM_Data))
		{
			_blob.set(pdata,sz);
			if(pdata) delete trace_free(pdata);
			pdata = NULL;
		}
		else 
		{
			if(pdata) delete trace_free(pdata);
			pdata = NULL;

#if defined(USE_EXCEPTION_ERRORHANDLING)
			throw_pipe_error(PipeError_ReadFail,_T("Invalid pipe data sequence or read error"));
#endif

			bret = false;
		}

		return bret;
	}

	virtual bool write(long _lv)
	{
		DWORD dwMetaData = PDM_Data;
		bool bret = static_cast<_Type*>(this)->write_pipe(&_lv,sizeof(_lv),dwMetaData);
		if(!bret)
		{
#if defined(USE_EXCEPTION_ERRORHANDLING)
			throw_pipe_error(PipeError_WriteFail,_T("Pipe write error"));
#endif
		}
		return bret;
	}

	virtual bool read(long& _lv) const
	{
		LPVOID ptr = &_lv;
		DWORD sz = sizeof(_lv);
		DWORD dwMetaData = PDM_Null;
		if(!const_cast<_Type*>(static_cast<const _Type*>(this))->read_pipe(ptr,sz,dwMetaData)
			|| NEQL(dwMetaData,PDM_Data)
			|| sz!=sizeof(_lv)
			)
		{
#if defined(USE_EXCEPTION_ERRORHANDLING)
			throw_pipe_error(PipeError_ReadFail,_T("Invalid pipe data sequence or read error"));
#endif
			return false;
		}
		return true;
	}

	virtual bool write(const double& _dv)
	{
		DWORD dwMetaData = PDM_Data;
		bool bret = static_cast<_Type*>(this)->write_pipe((LPVOID)&_dv,sizeof(_dv),dwMetaData);
		if(!bret)
		{
#if defined(USE_EXCEPTION_ERRORHANDLING)
			throw_pipe_error(PipeError_WriteFail,_T("Pipe write error"));
#endif
		}
		return bret;
	}

	virtual bool read(double& _dv) const
	{
		LPVOID ptr = &_dv;
		DWORD sz = sizeof(_dv);
		DWORD dwMetaData = PDM_Null;
		if(!const_cast<_Type*>(static_cast<const _Type*>(this))->read_pipe(ptr,sz,dwMetaData)
			|| NEQL(dwMetaData,PDM_Data)
			|| sz!=sizeof(_dv)
			) 
		{
#if defined(USE_EXCEPTION_ERRORHANDLING)
			throw_pipe_error(PipeError_ReadFail,_T("Invalid pipe data sequence or read error"));
#endif
			return false;
		}
		return true;
	}

	virtual bool write(const CTime_& _tv)
	{
		_Type* ptype = static_cast<_Type*>(this);
		DateInfo di(_tv.GetYear(),_tv.GetMonth(),_tv.GetDay(),_tv.GetHour(),_tv.GetMinute(),_tv.GetSecond());
		DWORD dwMetaData = PDM_Data;
		if(!ptype->write_pipe((LPVOID)&di,sizeof(di),dwMetaData)) 
		{
#if defined(USE_EXCEPTION_ERRORHANDLING)
			throw_pipe_error(PipeError_WriteFail,_T("Pipe write error"));
#endif
			return false;
		}
		return true;
	}

	virtual bool read(CTime_& _tv) const
	{
		_Type* ptype = const_cast<_Type*>(static_cast<const _Type*>(this));
		DateInfo di;
		LPVOID ptr = &di;
		DWORD sz = sizeof(di);
		DWORD dwMetaData = PDM_Null;
		if(!ptype->read_pipe(ptr,sz,dwMetaData) || NEQL(dwMetaData,PDM_Data)) 
		{
#if defined(USE_EXCEPTION_ERRORHANDLING)
			throw_pipe_error(PipeError_ReadFail,_T("Invalid pipe data sequence or read error"));
#endif
			return false;
		}

		_tv = CTime_(di.year,di.month,di.day,di.hour,di.minute,di.second);
		return true;
	}

protected:
	virtual bool write_guid(const GUID& _guid)
	{
		CString_ str;
		guid_tostring(_guid,str);
		string_converter<TCHAR,WCHAR> converted(str);
		DWORD dwMetaData = PDM_GUIDofData;
		bool bret = static_cast<_Type*>(this)->write_pipe(
			(LPVOID)(LPCWSTR)converted
			,converted.get_length()*sizeof(WCHAR)
			,dwMetaData
			);
		if(!bret)
		{
#if defined(USE_EXCEPTION_ERRORHANDLING)
			throw_pipe_error(PipeError_WriteFail,_T("Pipe write error"));
#endif
		}
		return bret;
	}

	virtual bool read_guid(GUID& _guid) const
	{
		LPVOID pdata = NULL;
		DWORD sz = 0;
		DWORD dwMetaData = PDM_Null;
		bool bret = const_cast<_Type*>(static_cast<const _Type*>(this))->read_pipe(pdata,sz,dwMetaData,true);
		if(bret && EQL(dwMetaData,PDM_GUIDofData))
		{
			string_toguid((const TCHAR*)string_converter<WCHAR,TCHAR>((WCHAR*)pdata,sz/sizeof(WCHAR)),_guid);
			if(pdata) delete trace_free(pdata);
			pdata = NULL;
		}
		else 
		{
			if(pdata) delete trace_free(pdata);
			pdata = NULL;
#if defined(USE_EXCEPTION_ERRORHANDLING)
			throw_pipe_error(PipeError_ReadFail,_T("Invalid pipe data sequence or read error"));
#endif
			bret = false;
		}

		return bret;
	}
protected:
	~CPipeCommunicatorImpl()
	{
	}
};//template<> struct CPipeCommunicatorImpl


struct CNamedPipeWrap
{
protected:
	HANDLE m_hpipe;

	//data for reconnection
	CString_ m_sServerName;
	CString_ m_sPipeName;
	DWORD m_dwDesiredAccess;
	bool m_bWriteThroughMode;
	DWORD m_dwPipeMode;
	LPSECURITY_ATTRIBUTES m_lpsa;

	HANDLE m_hStopEvent;
	CPipeReadedBuffer m_readbuffer;
	CEvent_ m_readevnt;
	OVERLAPPED m_overlappedread;
	bool m_breadnewchunk;
	CEvent_ m_connectevnt;
	OVERLAPPED m_overlappedconnect;
	bool m_bnamedpipe;
	bool m_bconnected;
public:
	CNamedPipeWrap()
		:m_hpipe(INVALID_HANDLE_VALUE)
		,m_dwDesiredAccess(0)
		,m_bWriteThroughMode(false)
		,m_dwPipeMode(0)
		,m_lpsa(NULL)
		,m_hStopEvent(NULL)
		,m_readevnt(TRUE,FALSE)
		,m_breadnewchunk(true)
		,m_connectevnt(FALSE,TRUE)
		,m_bnamedpipe(false)
		,m_bconnected(false)
	{
		ZeroObj(m_overlappedread);
		m_overlappedread.hEvent = m_readevnt;
		ZeroObj(m_overlappedconnect);
		m_overlappedconnect.hEvent = m_connectevnt;
	}

	virtual ~CNamedPipeWrap()
	{
#if defined(USE_EXCEPTION_ERRORHANDLING)
		// destructor should never throw exception if they can be call during "stack unwinding" 
		close_nothrow();			
		free_nothrow();
#else
		close();
		free();
#endif
	}

	/*
		Error codes to handle after this call:

		ERROR_FILE_NOT_FOUND	--	This error is recieved by client when it is unabled to find a 
									given named pipe server on particular machine. The server machine
									is itself accessible. 
									(Mean the error in the <_sPipeName> param)

		ERROR_ACCESS_DENIED		--	This commonly occurs in trying to access pipe server for which 
									NT secure denies access. The bright side to this error message is
									that access to the pipe server`s machine itself is permitted 
									(otherwise, an ERROR_LOGON_FAILURE would have been received),
									although this specific named pipe on the machine isn`t accessible 
									(Mean the error in the <_psa> param or in the settings of the server-client)

		ERROR_BAD_NETPATH		--	If the clien recieves this error, it has been unable to locate
									the machine of the desired named pipe server
									(Mean the error in the <_sServerName> param)

		ERROR_BAD_PATHNAME		--	think same as ERROR_BAD_NETPATH for local pipes
									(Mean the error in the <_sServerName> param)

		ERROR_LOGON_FAILURE		--	This indicates a logon problem with either an account or password.
									A client has no access to the server machine at all.
									(Mean the error in the <_psa> param)

		ERROR_BAD_PIPE			--	may be can get in call to SetNamedPipeHandleState
									because of the pipe was created with an other pipe mode
									(Mean the error in the <_dwPipeMode> param)
		
		ERROR_SEM_TIMEOUT		--	WaitNamedPipe() return error 
		
		ERROR_BAD_NET_NAME		--  because the resource name is invalid, or because the named resource cannot 
									be located.

		ERROR_CANCELLED			--  The attempt to make the connection was cancelled by the user through 
									a dialog box from one of the network resource providers, or by a called 
									resource.

		ERROR_NO_NET_OR_BAD_PATH --	The operation cannot be performed because a network component is not 
									started or because a specified name cannot be used.

		ERROR_NO_NETWORK		--	The network is unavailable.
		

		(see: http://www.ddj.com/showArticle.jhtml?documentID=win0302d&pgno=2)
	*/
	bool open(const CString_& _sServerName			// "." -- mean a current machine
		,const CString_& _sPipeName
		,DWORD _dwDesiredAccess						//GENERIC_WRITE
		,DWORD _nWaitTime = NMPWAIT_USE_DEFAULT_WAIT
		,DWORD _dwPipeMode = PIPE_READMODE_MESSAGE	// if specified PIPE_READMODE_MESSAGE mode then the system always perform 
													// write operations on pipe as if the <_bWriteThroughMode> will be true
		,bool _bWriteThroughMode = true				// mean don`t return write operation while server recieves data
													// otherwise data will be written in local buffer and write opeartion
													// return after the write data to the local buffer. then the system itself
													// will transfer the data to server
		,LPSECURITY_ATTRIBUTES _psa = NULL			// security attributes
		)
	{
		VERIFY_EXIT1(EQL(m_hpipe,INVALID_HANDLE_VALUE),false);

		//save for future reconnections
		m_sServerName = _sServerName;
		m_sPipeName = _sPipeName;
		m_dwDesiredAccess = _dwDesiredAccess;
		m_bWriteThroughMode = _bWriteThroughMode;
		m_dwPipeMode = _dwPipeMode;
		m_lpsa = _psa;

		if(_sServerName.Compare(_T(".")))
		{
			CString_ netpath;
			netpath = _T("\\\\");
			netpath  += _sServerName;

			if(!can_connect(netpath)) 
			{
				utils::CErrorTag __et(cnamedvalue(_T("RemoteComputerName"),netpath));
#if defined(USE_EXCEPTION_ERRORHANDLING)
				throw_pipe_error(PipeError_ConnectFail,_T("Can`t connect to remote computer"));
#endif
				return false;
			}
		}

		CString_ pipename;
		pipename += _T("\\\\");
		pipename += _sServerName;
		pipename += _T("\\pipe\\");
		pipename += _sPipeName;

		DWORD dwFlagsAndAttributes = _bWriteThroughMode?FILE_FLAG_WRITE_THROUGH:0;

		while(true)
		{
			utils::CErrorTag __etpn(cnamedvalue(_T("PipeName"),pipename));

			m_hpipe = ::CreateFile(
				pipename
				,_dwDesiredAccess
				,0					// no sharing
				,_psa				
				,OPEN_EXISTING
				,dwFlagsAndAttributes
				,NULL				// no template file
				);

			if(NEQL(INVALID_HANDLE_VALUE,m_hpipe)) break;	// ok. pipe created 
			DWORD err = GetLastError();
			if(NEQL(GetLastError(),ERROR_PIPE_BUSY)) 
			{
#if defined(USE_EXCEPTION_ERRORHANDLING)

				throw_pipe_error(PipeError_OpenFail,_T("Can`t open pipe"));
#endif
				return false;	// there is a error
			}
			if(!WaitNamedPipe(pipename,_nWaitTime))
			{
				if(OSVersion::is().win9x() && EQL(::GetLastError(),ERROR_PIPE_NOT_CONNECTED))
				{
					::SetLastError(ERROR_BAD_PATHNAME);	// see: http://support.microsoft.com/kb/180222
				}
#if defined(USE_EXCEPTION_ERRORHANDLING)
				throw_pipe_error(PipeError_OpenFail,_T("Can`t open pipe"));
#endif
				return false;
			}
		}
		DWORD dwPipeMode = _dwPipeMode;
		dwPipeMode = dwPipeMode & (PIPE_READMODE_BYTE | PIPE_READMODE_MESSAGE);
		dwPipeMode = (dwPipeMode & ~(PIPE_NOWAIT)) | PIPE_WAIT;
		bool bret = NEQL(::SetNamedPipeHandleState(m_hpipe,&dwPipeMode,NULL,NULL),0);
		if(!bret)
		{
#if defined(USE_EXCEPTION_ERRORHANDLING)
			throw_pipe_error(PipeError_SetPipeStateFail,_T("Can`t set pipe handle state"));
#endif
		}
		return bret;
		//TODO: may be add the buffer size setting
	}

	/*
		function create named pipe 
		(don`t works on Windows Me/95/98)

		Error codes to handle after this call:

		ERROR_INVALID_PARAMETER			--	if <_dwMaxInstance> param is geater then PIPE_UNLIMITED_INSTANCES

		ERROR_ACCESS_DENIED				--	if you create another instance of pipe with different pipe type 
											or pipe access or instance count or time-out value
											params: <_dwAccessMode>, <_bWriteThroghMode>, 
												<_dwMaxInstance>, <_nDefaultTimeOut>
	*/
	bool create(
		const CString_& _sPipeName
		,HANDLE _hStopEvent
		,DWORD _dwAccessMode			// PIPE_ACCESS_INBOUND
		,DWORD _dwPipeMode = PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE
		,bool _bWriteThroghMode = true
		,DWORD _dwMaxInstance = PIPE_UNLIMITED_INSTANCES
		,DWORD _nDefaultTimeOut = 3000	// Default time-out value, in milliseconds, if the WaitNamedPipe 
										// function specifies NMPWAIT_USE_DEFAULT_WAIT. 
										// Each instance of a named pipe must specify the same value.
		,LPSECURITY_ATTRIBUTES _psa = NULL
		)
	{
#if defined(USE_EXCEPTION_ERRORHANDLING)
		if(NEQL(m_hpipe,INVALID_HANDLE_VALUE) 
			|| IS_NULL(_hStopEvent)
			) 
		{
			throw_pipe_error(PipeError_BadArguments,_T("Can`t create pipe"));
		}
#endif
		VERIFY_EXIT1(EQL(m_hpipe,INVALID_HANDLE_VALUE),false); // should not initialized
		VERIFY_EXIT1(NOT_NULL(_hStopEvent),false);

		m_hStopEvent = _hStopEvent;

		CString_ pipename(_T("\\\\.\\pipe\\"));
		pipename += _sPipeName;

		DWORD dwOpenMode = 
			FILE_FLAG_OVERLAPPED
				| (_dwAccessMode & (PIPE_ACCESS_DUPLEX|PIPE_ACCESS_INBOUND|PIPE_ACCESS_OUTBOUND))
				| ((_bWriteThroghMode)?FILE_FLAG_WRITE_THROUGH:0)
			;

		static const DWORD pipemodemask = PIPE_READMODE_MESSAGE|PIPE_TYPE_MESSAGE;

//#if defined(USE_EXCEPTION_ERRORHANDLING)
//		if(!
//				(
//				::is_set<DWORD>(_dwPipeMode,pipemodemask,PIPE_READMODE_MESSAGE|PIPE_TYPE_MESSAGE)
//				|| ::is_set<DWORD>(_dwPipeMode,pipemodemask,PIPE_READMODE_BYTE|PIPE_TYPE_BYTE)
//				)
//			)
//			throw_pipe_error(PipeError_BadArguments,_T("Can`t create pipe"));
//#endif
//		VERIFY_EXIT1(
//			::is_set<DWORD>(_dwPipeMode,pipemodemask,PIPE_READMODE_MESSAGE|PIPE_TYPE_MESSAGE)
//				|| ::is_set<DWORD>(_dwPipeMode,pipemodemask,PIPE_READMODE_BYTE|PIPE_TYPE_BYTE)
//			,false
//			);

		//ASSERT_(!::is(_dwPipeMode,PIPE_NOWAIT,PIPE_NOWAIT)); // DEPRECATED

		utils::CErrorTag __et;
		__et
			<< cnamedvalue(_T("PipeName"),pipename)
			;

		m_hpipe = ::CreateNamedPipe(
			(LPCTSTR)pipename
			,dwOpenMode
			,_dwPipeMode
			,_dwMaxInstance
			,1024					
			,1024
			,_nDefaultTimeOut
			,_psa
			);

		if(NEQL(m_hpipe,INVALID_HANDLE_VALUE))
		{
			m_bnamedpipe = true;
			return true;
		}

#if defined(USE_EXCEPTION_ERRORHANDLING)
		throw_pipe_error(PipeError_CreateFail,_T("Can`t create pipe"));
#endif
		return false;
	}

	void close()
	{
		if(EQL(INVALID_HANDLE_VALUE,m_hpipe)) return;

		flush();
		if(m_bnamedpipe) disconnect();
		::CloseHandle(m_hpipe);

		m_hpipe = INVALID_HANDLE_VALUE;
	}

	void close_nothrow()
	{
		if(EQL(INVALID_HANDLE_VALUE,m_hpipe)) return;
		flush();
		if(m_bnamedpipe) disconnect_nothrow();
		::CloseHandle(m_hpipe);
		m_hpipe = INVALID_HANDLE_VALUE;
	}


	void flush()
	{
		if(EQL(INVALID_HANDLE_VALUE,m_hpipe)) return;
		::FlushFileBuffers(m_hpipe);
	}

	bool is_valid() const 
	{
		return NEQL(m_hpipe,INVALID_HANDLE_VALUE);
	}

	operator bool() const
	{
		return is_valid();
	}

	bool operator ! () const 
	{
		return !is_valid();
	}

	bool reconnect_client(DWORD _nWaitTime = NMPWAIT_USE_DEFAULT_WAIT)
	{
		//VERIFY_EXIT1((bool)*this,false);	// not connected yet, so why your trying to REconnect
		close();
		return open(m_sServerName,m_sPipeName
			,m_dwDesiredAccess
			,_nWaitTime
			,m_dwPipeMode
			,m_bWriteThroughMode
			,m_lpsa
			);
	}

	

	/*
		Error codes to handle after this call: 

		ERROR_PIPE_NOT_CONNECTED		--	client recieve this error when the local(!) pipe 
											server disconnects. appears after call to 
											DisconnectNamedPipe()
		ERROR_VC_DISCONNECTED			--	The underlying NetBIOS session beetween two machines
											is dissconnected. In the event of any kind of network
											outage, the client and server need to dissconnect 
											and reconnect. 
	*/
	bool write_pipe(const LPVOID _pdata,DWORD _size,DWORD _dwMetaData)
	{
		if(::IsBadReadPtr(_pdata,_size) 
			|| _dwMetaData<=PDM_Null || _dwMetaData>=PDM_Size
			) 
		{
#if defined(USE_EXCEPTION_ERRORHANDLING)
			throw_pipe_error(PipeError_BadArguments,_T("pipe write error"));
#endif
			return false;
		}

#if defined(USE_EXCEPTION_ERRORHANDLING)
		if(!is_valid())
		{
			throw_pipe_error(PipeError_NotInitialized,_T("pipe write error"));
		}
#endif

		VERIFY_EXIT1(*this,false);	// not initialized

		LPVOID stackbuf = NULL;
		std::auto_ptr<byte> heapbuf;
		DWORD size = align((_size + sizeof(DWORD)*2),unsigned __int64());
		if(size<1024*16)
		{
			stackbuf = alloca(size);
			ZeroMemory(stackbuf,size);
			*((LPDWORD)stackbuf) = _dwMetaData;
			*((LPDWORD)stackbuf + 1) = _size;
			memcpy((LPDWORD)stackbuf + 2,_pdata,_size);
		}
		else
		{
			std::auto_ptr<byte> buf(trace_alloc(new BYTE[size]));
			heapbuf = buf;
			ZeroMemory(heapbuf.get(),size);
			*((LPDWORD)heapbuf.get()) = _dwMetaData;
			*((LPDWORD)heapbuf.get() + 1) = _size;
			memcpy((LPDWORD)heapbuf.get() + 2,_pdata,_size);
		}


		DWORD dwwritten = 0;
		if(!::WriteFile(
				m_hpipe								// pipe handle
				,stackbuf?stackbuf:(LPVOID)heapbuf.get(),size		// data to write
				,&dwwritten							// written data 
				,NULL								// synchronously
				)
			)
		{
			if(EQL(::GetLastError(),ERROR_PIPE_NOT_CONNECTED))
			{
				//try to recoonnect immediately
				if(!reconnect_client(NMPWAIT_NOWAIT))
				{
#if defined(USE_EXCEPTION_ERRORHANDLING)
					throw_needToReconect();	// do not produce user error message 
#endif
					::SetLastError(ERROR_PIPE_NOT_CONNECTED);
					return false;
				}
				if(!::WriteFile(
						m_hpipe									// pipe handle
						,stackbuf?stackbuf:(LPVOID)heapbuf.get(),size		// data to write
						,&dwwritten								// written data 
						,NULL									// synchronously
						)
					) 
				{
#if defined(USE_EXCEPTION_ERRORHANDLING)
					if(EQL(::GetLastError(),ERROR_PIPE_NOT_CONNECTED)) 
					{
						throw_needToReconect();	// don`t produce user error message
					}
					else 
					{
						throw_pipe_error(PipeError_WriteFail,_T("pipe write error"));
					}
#endif
					return false;
				}
			}
		}
		bool bret = EQL(dwwritten,size);
#if defined(USE_EXCEPTION_ERRORHANDLING)
		if(!bret)
		{
			utils::CErrorTag __etsize(cnamedvalue(_T("DataSize"),size));
			utils::CErrorTag __etwrittensize(cnamedvalue(_T("WrittenDataSize"),dwwritten));
			throw_pipe_error(PipeError_WriteFail,_T("pipe write error"));
		}
#endif
		return bret;
	}

	bool read_pipe(LPVOID& _pdata,DWORD& _size,DWORD& _dwMetaData,bool _ballocate = false)
	{
#if defined(USE_EXCEPTION_ERRORHANDLING)
		if(!is_valid()) 
		{
			throw_pipe_error(PipeError_NotInitialized,_T("read pipe error"));
		}
#endif 
		VERIFY_EXIT1((bool)*this,false);

		static const DWORD bufchunksize = 512;
		static const long sec_criticalbuffersize = 1024*1024; // 1 Mb

		if(try_readdata(_pdata,_size,_dwMetaData,_ballocate)) return true;

		LPVOID pbuf = NULL;
		DWORD size = 0;

		while(true)
		{
			DWORD dwError = ERROR_SUCCESS;
			if(m_breadnewchunk)
			{
				if(m_readbuffer.get_firstBlockSendedSize()>=sec_criticalbuffersize) 
				{
					ASSERT_(FALSE);	// SECURE: it is secure on fall case.
									// if you get here otherwise, see use of you data structures sizes
									// may be some of them is too large

					m_readbuffer.free_firstBlock_Always();

#if defined(USE_EXCEPTION_ERRORHANDLING)
					throw_pipe_error(PipeError_DataTooLarge,_T("read pipe error"));
#endif 
					return false;
				}

				pbuf = m_readbuffer.alloc_chunk(bufchunksize);
				//TRACE_(_T("new buffer allocated buf = %p size = %d\n"),pbuf,bufchunksize);

				if(!::ReadFile(m_hpipe,pbuf,bufchunksize,NULL,&m_overlappedread))
				{
					// ignore errors because we test result after
					dwError = GetLastError();
				}
			}
			else
			{
				VERIFY_EXIT1(!m_readbuffer.is_empty() && m_readbuffer.get_lastWriteLength()>0,false);
				//ask for more data
				pbuf = m_readbuffer.get_lastWriteStart();
				size = m_readbuffer.get_lastWriteLength();
				//TRACE_(_T("using existing buffer buf = %p, size = %d\n"),pbuf,size);

				if(!::ReadFile(m_hpipe,pbuf,size,NULL,&m_overlappedread))
				{
					// ignore errors because we test result after
					dwError = GetLastError();
				}
			}

			// io pending process with WaitForMultipleObjects() call
			if(ERROR_IO_PENDING==dwError) 
				dwError=ERROR_SUCCESS;

			HANDLE handles[] = {m_hStopEvent,(HANDLE)m_readevnt};

			DWORD res = WAIT_FAILED;
				
			if(dwError==ERROR_SUCCESS)
			{
				res = ::WaitForMultipleObjects(sizea(handles),handles,FALSE,INFINITE);
				//TRACE_(_T("read_pipe() wait return %s\n")
				//	,EQL(WAIT_OBJECT_0,res)?_T("WAIT_OBJECT_0 (stop event signaled)")
				//	:EQL(WAIT_OBJECT_0+1,res)?_T("WAIT_OBJECT_1 (data was readed)")
				//	:EQL(WAIT_TIMEOUT,res)?_T("WAIT_TIMEOUT")
				//	:EQL(WAIT_ABANDONED,res)?_T("WAIT_ABANDONED")
				//	:EQL(WAIT_ABANDONED+1,res)?_T("WAIT_ABANDONED+1")
				//	:(LPCTSTR)pipeutils::Format(_T("%d"),res)
				//	);
			}

			if(EQL(res,WAIT_OBJECT_0))
			{
				::CancelIo(m_hpipe);
#if defined(USE_EXCEPTION_ERRORHANDLING)
				throw_cancel();					// do not produce user error message
#endif
				return false;	//just return with error
			}
			else if(EQL(res,WAIT_OBJECT_0+1) || dwError!=ERROR_SUCCESS)
			{
				ResetEvent(m_readevnt);
				DWORD dwReaded = 0;
				if(dwError==ERROR_SUCCESS && !::GetOverlappedResult(m_hpipe,&m_overlappedread,&dwReaded,FALSE) || dwReaded==0)
				{
					dwError = GetLastError();
					VERIFY_DO(dwReaded<=size,dwError=ERROR_BUFFER_OVERFLOW);
				}
				///}
				if(dwError!=ERROR_SUCCESS)					
				{
					switch(dwError)
					{
					case ERROR_BROKEN_PIPE:
#if defined(USE_EXCEPTION_ERRORHANDLING)
						throw_disconnect(dwError);		// do not produce user error message
#endif
						return false;

					case ERROR_PIPE_NOT_CONNECTED:
					case ERROR_BAD_PIPE:
					case ERROR_VC_DISCONNECTED:
					case ERROR_BUFFER_OVERFLOW:
						{
#if defined(USE_EXCEPTION_ERRORHANDLING)
						throw_pipe_error(PipeError_Win32Error,_T("read pipe error"));
#endif
						return false;
						}

					case ERROR_MORE_DATA: //just for PIPE_TYPE_MESSAGE mode and messages
						m_breadnewchunk = true;
						continue;

					case ERROR_IO_INCOMPLETE: // think it will never be needed 
						continue;

					default:;
					}
					return false;
				}
				//TRACE_(_T("readed data size = %d\n"),dwReaded);
				m_readbuffer.written_lastchunk(dwReaded);			// modify written position
				m_breadnewchunk = m_readbuffer.is_lastchunkfull();	// if last chunk is filled up set allocation flag
				if(try_readdata(_pdata,_size,_dwMetaData,_ballocate)) return true;
				continue;
			}
#if defined(USE_EXCEPTION_ERRORHANDLING)
			utils::CErrorTag __etWaitReturn(cnamedvalue(_T("WaitForObjectReturnCode"),res));
			utils::CErrorTag __etWaitReturnStr(cnamedvalue(_T("WaitForObjectReturnCodeStr"),to_str(CWaitResult(res,_countof(handles)))));

			throw_pipe_error(PipeError_ReadFail,_T("read pipe error"));
#endif
			return false;
		}
		ASSERT_(FALSE); //unreachable point

#if defined(USE_EXCEPTION_ERRORHANDLING)
		throw_pipe_error(PipeError_ProgrammerError,_T("read pipe error"));
#endif
		return false; //unreachable point
	}

	bool connect(bool& _bstoped)
	{
		_bstoped = false;

#if defined(USE_EXCEPTION_ERRORHANDLING)
		if(!is_valid()) 
		{
			throw_pipe_error(PipeError_NotInitialized,_T("read pipe error"));
		}
#endif 

		VERIFY_EXIT1(is_valid(),false);

		if(!::ConnectNamedPipe(m_hpipe,&m_overlappedconnect))
		{
			DWORD dwerr = GetLastError();
			if(EQL(dwerr,ERROR_PIPE_CONNECTED)) 
			{
				m_bconnected = true;
				return true;
			}
		}

		HANDLE handles[2] = {m_hStopEvent,(HANDLE)m_connectevnt};

		DWORD res = ::WaitForMultipleObjects(sizea(handles),handles,FALSE,INFINITE);

		if(EQL(res,WAIT_OBJECT_0))
		{
			_bstoped = true;
#if defined(USE_EXCEPTION_ERRORHANDLING)
			throw_cancel();				// don`t produce user error message
#endif 
			return false;
		}
		else if(EQL(res,WAIT_OBJECT_0+1))
		{
			m_bconnected = true;
			return true;
		}
#if defined(USE_EXCEPTION_ERRORHANDLING)
		throw_pipe_error(PipeError_ConnectFail,_T("connect fail"));
#endif 
		return false;
	}

	bool disconnect()
	{
		VERIFY_EXIT1(NEQL(m_hpipe,INVALID_HANDLE_VALUE),false);
		if(!m_bconnected) return false;
		m_bconnected = false;
		if(::DisconnectNamedPipe(m_hpipe)) return true;
		DWORD dwerr = GetLastError();

#if defined(USE_EXCEPTION_ERRORHANDLING)
		throw_pipe_error(PipeError_Win32Error,_T("pipe disconnect fail"));
#endif 

		return false;
	}

	bool disconnect_nothrow()
	{
		VERIFY_EXIT1(NEQL(m_hpipe,INVALID_HANDLE_VALUE),false);
		if(!m_bconnected) return false;
		m_bconnected = false;
		if(::DisconnectNamedPipe(m_hpipe)) return true;
		DWORD dwerr = GetLastError();
		return false;
	}

	const CString_& get_servername() const {return m_sServerName;}
	const CString_& get_pipename() const {return m_sPipeName;}

protected:
	bool try_readdata(LPVOID& _pdata,DWORD& _size,DWORD& _dwMetaData,bool _ballocate)
	{
		if(!m_readbuffer.is_empty()
			&& NEQL(PDM_Null,m_readbuffer.get_firstBlockMetaData())
			&& EQL(m_readbuffer.get_firstBlockRealSize(),m_readbuffer.get_firstBlockSendedSize())
			)
		{// can read data exactly now
			CPipeReadedBuffer::PipeBufferErrorsEn res = m_readbuffer.read(_pdata,_size,_dwMetaData,_ballocate);
			if(EQL(res,CPipeReadedBuffer::PBE_Success)) return true; 
			if(EQL(res,CPipeReadedBuffer::PBE_WrongBufferSize))	m_readbuffer.free_firstBlock();
		}
		return false;
	}
protected:
	void free()
	{
		//ASSERT_(IS_NULL(m_lpsa));	// FIXIT: create copy of LPSECURITY_ATTRIBUTES and manage the copy 
									//		  not the pointer to prevent memory access errors or other 
									//        strange behavior 
	}

	void free_nothrow()
	{
	}
};//struct CNamedPipeWrap


#endif //#if !defined(__NAMEDPIPEBASE_H__1F6FA635_EE75_40a1_B9B2_3938834F5AC5)