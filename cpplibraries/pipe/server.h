#if !defined(__SERVER_H__1F6FA635_EE75_40A1_B9B2_3938834F5AC5)
#define __SERVER_H__1F6FA635_EE75_40A1_B9B2_3938834F5AC5

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <utils/config.h>
#include <utils/atlmfc.h>
#include <utils/thread.h>

#include "errors.h"
#include "pipe_utils.h"
#include "databases.h"
#include "namepipebase.h"

template<typename _TypeList>
struct FindDataTypeAndProcess
{
	template<typename _Server>
	static bool process(const IPipeCommunicator* _pcomm,const GUID& _guid,_Server* _pserver)
	{
		typedef typename _TypeList::Item CurrentItem;
		typedef typename _TypeList::Tail Tail;
		if(is_equal(_guid,CurrentItem::get_guid()))
		{
			CString_ sGUID;
			common::convert(CurrentItem::get_guid(),sGUID);
			TRACE_(_T("founded object for guid %s\n"),(LPCTSTR)sGUID);
			return process_item(_pcomm,_pserver,CurrentItem());
		}
		return FindDataTypeAndProcess<Tail>::process(_pcomm,_guid,_pserver);
	}

	template<typename _Server,typename _ItemType>
	static bool process_item(const IPipeCommunicator* _pcomm,_Server* _pserver,_ItemType)
	{
		_ItemType* pitem = trace_alloc(new _ItemType());
		bool bret = false;
		try
		{
			if(!pitem->load(_pcomm))
			{
				delete trace_free(pitem);
				return false;
			}
			bret = _pserver->process(pitem); 
			delete trace_free(pitem);
			pitem = NULL;
		}
		catch(...)
		{
			if(NOT_NULL(pitem))
			{
				delete trace_free(pitem);
				pitem = NULL;
			}
			throw;
		}
		return bret;
	}
};//template<> struct FindDataTypeAndProcess

template<>
struct FindDataTypeAndProcess<NullType>
{
	template<typename _Server>
	static bool process(const IPipeCommunicator* _pcomm,const GUID& _guid,_Server* _pserver)
	{
		CString_ sGUID;
		common::convert(_guid,sGUID);
		TRACE_(_T("Data object not founded (GUID = %s)\n"),(LPCTSTR)sGUID);
		return false;
	}
};//template<NullType> struct FindDataTypeAndProcess

template<typename _Server,typename _TypeList>
struct CPipeServerTraits
{
	enum {
		info_on_connect = false
		,info_on_disconnect = false
		,info_on_error = false
	};
};

#define PIPESERVRERTRAITS(_Server,_TypeList,_InfoOnConnectDisconnect,_InfoOnError)	\
template<>	\
struct CPipeServerTraits<_Server,_TypeList>	\
{	\
	enum {	\
		info_on_connect = (_InfoOnConnectDisconnect)	\
		,info_on_disconnect = (_InfoOnConnectDisconnect)	\
		,info_on_error = (_InfoOnError)	\
	};	\
};

template<typename _Server,typename _TypeList>
struct CServerPipeReadThread 
:
	public CWorkedThreadImpl<CServerPipeReadThread<_Server,_TypeList>, CThreadStopAtExternalEvent>
	,public CNamedPipeWrap 
	,protected CPipeCommunicatorImpl<CServerPipeReadThread<_Server,_TypeList> >
{
private:
	typedef CPipeCommunicatorImpl<CServerPipeReadThread<_Server,_TypeList> > Communicator;
	typedef CPipeServerTraits<_Server,_TypeList> PipeServerTraits;
protected:
	_Server* m_pserver;
public:
	CServerPipeReadThread(_Server* _pserver)
		:m_pserver(_pserver)
	{
		VERIFY_EXIT(NOT_NULL(_pserver));
		//TRACE_(_T("CServerPipeReadThread 0x%08x constructor\n"),this);
		m_pserver->private_addref();
	}

	~CServerPipeReadThread()
	{
		//TRACE_(_T("CServerPipeReadThread 0x%08x destructor\n"),this);
		m_pserver->private_release();
	}

	bool process()
	{
		GUID guid;
		ZeroObj(guid);
		if(!this->read_guid(guid)) return false;
		utils::CErrorTag __etguid(namedvalue(_T("Data GUID"),guid));
		CString_ sguid;
		common::convert(guid,sguid);
		TRACE_(_T("load GUID = %s\n"),(LPCTSTR)sguid);
		if(!FindDataTypeAndProcess<_TypeList>::process(this,guid,m_pserver)) return false;
		return true;
	}

	bool create(const CString_& _sPipeName,LPSECURITY_ATTRIBUTES _psa = NULL)
	{
		set_externalStopEvent(*m_pserver);
		return CNamedPipeWrap::create(
			_sPipeName
			,*m_pstopevent
			,PIPE_ACCESS_INBOUND					// data goes from client to server
			,PIPE_TYPE_BYTE
				|PIPE_READMODE_BYTE 
			,false
			,PIPE_UNLIMITED_INSTANCES
			,3000
			,_psa
			);
	}

	DWORD thread_main()
	{
		VERIFY_EXIT1(NOT_NULL(m_pserver),0);
		on_connected(Bool2Type<PipeServerTraits::info_on_connect>::Type());

		bool bCancel = false;

		while(!is_aborted())
		{
			try
			{
				if(!process()) 
				{
					//verify for a good errors
					break;
				}
			}
			catch(PipeDisconnected EXC_ACCESS_TYPE _exc)
			{
				ExceptionPtr<PipeDisconnected> exc(_exc);
				on_disconnected(Bool2Type<PipeServerTraits::info_on_disconnect>::Type());
				break;
			}
			catch(utils::IUtilitiesException EXC_ACCESS_TYPE _exc)
			{
				ExceptionPtr<utils::IUtilitiesException> exc(_exc);
				on_error(*exc,bCancel,Bool2Type<PipeServerTraits::info_on_error>::Type());
				break;
			}
			catch(PipeOperationCanceled EXC_ACCESS_TYPE _exc)
			{
				ExceptionPtr<PipeOperationCanceled> exc(_exc);
				break;
			}
			catch(...)
			{
				CPipeException err(PipeError_ProgrammerError,_T("illegal exception type"),CREATE_SOURCEINFO());
				on_error(err,bCancel,Bool2Type<PipeServerTraits::info_on_error>::Type());
				break;
			}
		}
		close();
		//TRACE_(_T("exit read data (CServerPipeReadThread class)\n"));
		return 0;
	}
protected:
	// do nothing
	void on_connected(FalseType){}	
	void on_disconnected(FalseType){}
	void on_error(const utils::IUtilitiesException& _exc,bool& _bCancel,FalseType){}


	void on_connected(TrueType)
	{
		VERIFY_EXIT(NOT_NULL(m_pserver));
		m_pserver->on_connected();
	}

	void on_disconnected(TrueType)
	{
		VERIFY_EXIT(NOT_NULL(m_pserver));
		m_pserver->on_disconnected();
	}

	void on_error(const utils::IUtilitiesException& _exc,bool& _bCancel,TrueType)
	{
		VERIFY_EXIT(NOT_NULL(m_pserver));
		m_pserver->on_error(_exc,_bCancel);
	}
};//template<> struct CServerPipeReadThread

template<typename _Server,typename _TypeList>
struct CServerPipeCommImpl 
:
	public CWorkedThreadImpl<CServerPipeCommImpl<_Server,_TypeList> >
{
protected:
	typedef CWorkedThreadImpl<CServerPipeCommImpl<_Server,_TypeList> > WorkedThread;

	CString_ m_sPipeName;
	CEvent_ m_waitforchilds;
	LONG m_childcounter;
	LPSECURITY_ATTRIBUTES m_psa;
	CEvent_ m_OnInitializedEvnt;
public:
	CServerPipeCommImpl(const CString_& _sPipeName)
		:m_sPipeName(_sPipeName)
#if defined(USE_ATL)
		,m_waitforchilds(NULL,TRUE,TRUE,NULL)
		,m_OnInitializedEvnt(NULL,TRUE,FALSE,NULL)
#endif
#if defined(USE_MFC)
		,m_waitforchilds(TRUE,TRUE,NULL,NULL)
		,m_OnInitializedEvnt(FALSE,TRUE,NULL,NULL)
#endif
		,m_childcounter(0)
		,m_psa(NULL)
	{
	}

	~CServerPipeCommImpl()
	{
		predestruct();
	}

	void predestruct()
	{
		WorkedThread::predestruct();
		DWORD res = ::WaitForSingleObject(m_waitforchilds,10000);
		if(EQL(res,WAIT_TIMEOUT)) 
		{
			TRACE_(_T("!!! ~CServerPipeCommImpl() wait timeout\n"));
		}
	}

	template<typename _MessageType>
		bool process(_MessageType* _pmessage)
	{
		VERIFY_EXIT1(NOT_NULL(_pmessage),false);
		return true;
	}

	template<typename _ExceptionType>
		bool on_error(_ExceptionType& _exc,bool& bCancel)
	{
		TRACE_(_T("error not processed\n"));
		return false;
	}

	void on_connected()
	{
		TRACE_(_T("on connected message not handled\n"));
	}

	void on_disconnected()
	{
		TRACE_(_T("on disconnected message not handled\n"));
	}

	DWORD thread_main()
	{
		while(!is_aborted())
		{
			CServerPipeReadThread<_Server,_TypeList>* preadthread = 
				trace_alloc( (new CServerPipeReadThread<_Server,_TypeList>(static_cast<_Server*>(this))) );

			bool bCancel = true;
			try
			{
				if(!preadthread->create(m_sPipeName,m_psa))
				{
					delete trace_free(preadthread);
					break;
				}
				SetEvent(m_OnInitializedEvnt);
				bool bstoped = false;
				if(!preadthread->connect(bstoped))
				{
					preadthread->close();
					delete trace_free(preadthread);
					if(bstoped) 
					{
						//TRACE_(_T("CServerPipeCommImpl stopped 0x%08x\n"),this);
						break;
					}
					else continue; //some error just continue
				}
				preadthread->set_deleteOnExit();
				preadthread->set_CoInitDWORD(m_dwCoInit);
				preadthread->start();
				preadthread = NULL;
				bCancel = false;
			}
			catch(utils::IUtilitiesException EXC_ACCESS_TYPE _exc)
			{
				ExceptionPtr<utils::IUtilitiesException> exc(_exc);
				static_cast<_Server*>(this)->on_error(*exc,bCancel);
				break;
			}
			catch(PipeOperationCanceled EXC_ACCESS_TYPE)
			{
				bCancel = true;
			}
			catch(...)
			{
				CPipeException err(PipeError_ProgrammerError,_T("illegal exception type"),CREATE_SOURCEINFO());
				static_cast<_Server*>(this)->on_error(err,bCancel);
			}

			if(NOT_NULL(preadthread))
			{
				delete trace_free(preadthread);
				preadthread = NULL;
			}
			if(bCancel) break;
		}
		return 0;
	}

	void private_addref()
	{
		ResetEvent(m_waitforchilds);
		InterlockedIncrement(&m_childcounter);
	}

	void private_release()
	{
		if(InterlockedDecrement(&m_childcounter)<=0)
		{
			SetEvent(m_waitforchilds);
		}
		else
		{
		}
	}

	HANDLE OnInitializedEvent() const {return m_OnInitializedEvnt;}

};//struct CServerPipeComm

#endif //#if !defined(__SERVER_H__1F6FA635_EE75_40A1_B9B2_3938834F5AC5)