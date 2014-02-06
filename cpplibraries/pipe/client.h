#if !defined(__CLIENT_H__1F6FA635_EE75_40A1_B9B2_3938834F5AC5)
#define __CLIENT_H__1F6FA635_EE75_40A1_B9B2_3938834F5AC5

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <utils/config.h>
#include <utils/atlmfc.h>
#include <utils/utils.h>
#include <utils/thread.h>
#include <utils/errors.h>

#include "errors.h"
#include "pipe_utils.h"
#include "databases.h"
#include "namepipebase.h"


/**\brief client side
*/
struct CClientPipeComm
:
	public CNamedPipeWrap												///< pipe api wrap
	,protected CPipeCommunicatorImpl<CClientPipeComm>					///< implementation of pipe communicator
	,protected CWorkedThreadImpl<CClientPipeComm,CThreadStopAtEvent>	///< worked thread base
{
protected:
	bool m_bneedtoreconnect;						///< flag if need to reconnect 
	DWORD m_asyncreconnecttime;						///< flag of async reconnection

	utils::CUtilityException m_last_reconnect_error;
public:

	/**\brief constructor
	*/
	CClientPipeComm()
		:m_bneedtoreconnect(false)
		,m_asyncreconnecttime(0)
	{
	}

	/**\brief destructor
	*/
	~CClientPipeComm()
	{
		predestruct();
		//base class destructor CNamedPipeWrap call the close() method to close pipe
	}

	/**\brief function to save some data into the pipe at client side
	\param _data -- data object to save
	*/
	bool save(const IPipeDataBase* _data)
	{
		if(is_running()) 
		{
			::SetLastError(ERROR_IO_INCOMPLETE);

#if defined(USE_EXCEPTION_ERRORHANDLING)
			throw_pipe_error(PipeError_StillConnecting,_T("save pipe error"));
#endif

			return false;	// initialization is running;
		}
		if(!is_valid()) 
		{
#if defined(USE_EXCEPTION_ERRORHANDLING)
			throw_pipe_error(PipeError_NotInitialized,_T("save pipe data"));
#endif
			return false;	// not initialized
		}
		//TRACE_(_T("------------ START object save ---------------------\n"));
		bool bret = false;
		try
		{
			if(!_data->save_guid(this)) 
			{
				test_recconnection();
				return false;
			}
			bret = _data->save(this);
			test_recconnection();
		}
		catch(PipeNeedToReconnect EXC_ACCESS_TYPE)
		{
			m_bneedtoreconnect = true;
			throw;
		}
		//TRACE_(_T("------------ END object save ---------------------\n"));
		return bret;
	}

	/**\brief function to open pipe at client side
	\param _sServerName -- server name to open pipe at. (use "." to specify current machine)
	\param _sPipeName -- pipe name to open
	\param _dwDesiredAccess -- pipe access flag
	\param _nWaitTime -- timeout (milisec) used to wait for pipe server creation 
	\param _bWriteThroughMode -- if this flag is true, than pipe API will be wait 
	       while data arrives server side.
	\param _dwPipeMode -- pipe mode to open
	\param _psa -- security attributes is used to open pipe
	*/
	bool open(const CString_& _sServerName
		,const CString_& _sPipeName
		,DWORD _dwDesiredAccess = GENERIC_WRITE
		,DWORD _nWaitTime = NMPWAIT_USE_DEFAULT_WAIT
		,bool _bWriteThroughMode = false
		,DWORD _dwPipeMode = PIPE_READMODE_BYTE
		,LPSECURITY_ATTRIBUTES _psa = NULL
		)
	{
		bool bret = CNamedPipeWrap::open(
			_sServerName
			,_sPipeName
			,_dwDesiredAccess
			,_nWaitTime
			,_dwPipeMode
			,_bWriteThroughMode
			,_psa
			);
		if(bret) m_bneedtoreconnect = false;
		return bret;
	}

	/**\brief function to close pipe
	*/
	void close()
	{
		CNamedPipeWrap::close();
	}

	/**\brief function to flush pipe data. Function returns only after all pipe 
	          data buffers will be flushed.
	*/
	void flush()
	{
		CNamedPipeWrap::flush();
	}

	bool is_valid() const 
	{
		return CNamedPipeWrap::is_valid();
	}

	/**\brief operator that return true if pipe is valid for use
	\return true if pipe is valid for use
	*/
	operator bool() const
	{
		return CNamedPipeWrap::operator bool();
	}

	/**\brief operator that return true if pipe ISN`T valid for use
	\return true if pipe ISN`T valid for use.
	*/
	bool operator ! () const 
	{
		return CNamedPipeWrap::operator ! ();
	}

	/**\brief function that reconnect client
	\param _nWaitTime -- timeout to try to reconnect (millisec)
	\return true if pipe was successfully reconnected and now it is valid for use.
	*/
	bool reconnect_client(DWORD _nWaitTime = NMPWAIT_USE_DEFAULT_WAIT)
	{
		if(reconnecting()) return false;	// don`t call this function at the call time of the async_reconnect_client
		try
		{
			return CNamedPipeWrap::reconnect_client(_nWaitTime);
		}
		catch(utils::IUtilitiesException EXC_ACCESS_TYPE _exc)
		{
			ExceptionPtr<utils::IUtilitiesException> pexc(_exc,false);	// don`t delete exception 
			m_last_reconnect_error.init(pexc);
			m_bneedtoreconnect = true;
			throw;
		}
	}
	
	/**\brief function that start async reconnection
	\param _nWaitTime -- timeout to try to reconnect (mill€isec)
	*/
	void async_reconnect_client(DWORD _nWaitTime = NMPWAIT_USE_DEFAULT_WAIT)
	{
		if(reconnecting()) return;	// ignore all next calls
		m_asyncreconnecttime = _nWaitTime;
		start();
	}

	/**\brief function that return true if client is reconnecting now
	\return true if client is reconnecting now
	*/
	bool reconnecting() const 
	{
		return to_bool(is_running());
	}

	/**\brief function that return true if need to reconnect
	\return true if need to reconnect
	*/
	bool nead_toReconnect() const
	{
		return m_bneedtoreconnect || !*static_cast<const CNamedPipeWrap*>(this);
	}

	/**\brief async reconnect function. Don`t call this function
	*/
	DWORD thread_main()
	{
		try
		{ 
			return CNamedPipeWrap::reconnect_client(m_asyncreconnecttime);
		} 
		catch(utils::IUtilitiesException EXC_ACCESS_TYPE _exc)
		{
			ExceptionPtr<utils::IUtilitiesException> pexc(_exc);	// delete in this block exception
			return 0;
		}
		catch(PipeOperationCanceled EXC_ACCESS_TYPE _exc)
		{
			ExceptionPtr<PipeOperationCanceled> pexc(_exc);
			return 0;
		}
		catch(...)
		{
			return 0;
		} // just for not exiting the process
	}

protected:

	/**\brief function that return true if need to reconnect (according to 
	          WinAPI last error code).
	\return return true if need to reconnect
	*/
	void test_recconnection()
	{
		DWORD dwerr = ::GetLastError();
		static const DWORD errors001[] = {ERROR_PIPE_NOT_CONNECTED,ERROR_VC_DISCONNECTED};
		m_bneedtoreconnect = IN_ARR(dwerr,errors001);
	}

};//struct CClientPipeComm

#endif //#if !defined(__CLIENT_H__1F6FA635_EE75_40A1_B9B2_3938834F5AC5)

