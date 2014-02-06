#if !defined(__THREAD_H__1F6FA635_EE75_40a1_B9B2_3938834F5AC5)
#define __THREAD_H__1F6FA635_EE75_40a1_B9B2_3938834F5AC5

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "config.h"
#include "atlmfc.h"

#include <vector>

#include "tracealloc.h"
#include "autoptr.h"
#include "utils.h"
#include "errors.h"


/**\defgroup UtilitiesThreadSupport Utilities library thread support

This classes helps you to build multiple thread applications.
*/

//@{

//#define USE_CRT_THREADING (defined(_MT) || defined(_DLL)) && !(defined(_MT) && defined(_DLL))

/**\brief macro to verify CRT (C run time) is used */
#define USE_CRT_THREADING defined(_MT) || defined(_DLL)

#if USE_CRT_THREADING
   // Due to the nature of the multithreaded C runtime lib we
   // need to use _beginthreadex() and _endthreadex() instead
   // of CreateThread() and ExitThread(). See Q104641.
   // http://www.rsdn.ru/forum/message/9995.flat.aspx for case when used Multithreaded DLL
   #include <process.h>
#endif // _MT

/**\brief thread function class wrapper
\tparam _bManaged -- param that specify if a class manage itself for thread data
*/
template<bool _bManaged>
struct CThreadAPIWrapT
{
protected:
	HANDLE m_hThread;			///< handle to thread
	DWORD m_dwThreadID;			///< thread id

public:
	/**\brief Cosntructor
	*/
	CThreadAPIWrapT()
		:m_hThread(NULL),m_dwThreadID(0)
	{
	}

	/**\brief destructor
	not a virtual because threap api wrap class will not be used as a interface.
	*/
	~CThreadAPIWrapT()
	{
		if(_bManaged) release();
	}

	/**\breif function to release data of class
	it close thread handle.
	return TRUE if thread handle was successfully closed
	*/
	BOOL release()
	{
		if(NOT_NULL(m_hThread))
		{
			if(!::CloseHandle(m_hThread)) return FALSE; // see GetLastError()
		}
		m_hThread = NULL;
		m_dwThreadID = 0;
		return TRUE;
	}

	/**\brief function tio attach to thread api wrapper class thread handle and id.
	\param _hThread -- thread handle to attach
	\param _dwThreadID -- thread id to attach
	*/
	void attach(HANDLE _hThread,DWORD _dwThreadID = 0)
	{
		VERIFY_EXIT(IS_NULL(m_hThread));
		m_hThread = _hThread;
		m_dwThreadID = _dwThreadID;
	}

	/**\brief function to detach thread handle.
	\return used by thread api wrapper class thread handle 
	*/
	HANDLE detach()
	{
		HANDLE hThread = m_hThread;
		m_hThread = NULL;
		m_dwThreadID = 0;
		return hThread;
	}

	/**\brief function to set priority of thread
	\param _iPriority -- priority value to set
	\return TRUE if data was successfully set, else, if returned FALSE see GetLastError() for error code.
	\sa CThreadAPIWrapT::get_priority()
	*/
	BOOL set_priority(int _iPriority)
	{
		VERIFY_EXIT1(NOT_NULL(m_hThread),FALSE);
		return ::SetThreadPriority(m_hThread,_iPriority);	// see GetLastError()
	}

	/**\brief function that returns thread priority
	\return thread priority or THREAD_PRIORITY_ERROR_RETURN if thread handle is null
	\sa CThreadAPIWrapT::set_priority()
	*/
	int get_priority() const
	{
		VERIFY_EXIT1(NOT_NULL(m_hThread),THREAD_PRIORITY_ERROR_RETURN);
		return GetThreadPriority(m_hThread);	// see GetLastError()
	}

	/**\brief function to suspend thread.
	If the function succeeds, execution of the specified thread is suspended and 
	the thread's suspend count is incremented.\n

	Suspending a thread causes the thread to stop executing user-mode (application) code.\n 

	Each thread has a suspend count (with a maximum value of MAXIMUM_SUSPEND_COUNT).\n

	If the suspend count is greater than zero, the thread is suspended; otherwise, 
	the thread is not suspended and is eligible for execution. Calling \ref CThreadAPIWrapT::suspend() 
	causes the target thread's suspend count to be incremented. Attempting to 
	increment past the maximum suspend count causes an error without incrementing 
	the count.\n

	The \ref CThreadAPIWrapT::resume() function decrements the suspend count of a suspended thread. 

	\return If the function succeeds, the return value is the thread's previous suspend count; 
	        otherwise, it is -1. To get extended error information, use the GetLastError() function.
			if thread handle (CThreadAPIWrapT::m_hThread) is null  then function return -1.
	\sa CThreadAPIWrapT::resume()
	*/
	DWORD suspend()
	{
		VERIFY_EXIT1(NOT_NULL(m_hThread),-1);
		return ::SuspendThread(m_hThread);	// see GetLastError()
	}

	/**\brief function that resume thread.
	The ResumeThread function checks the suspend count of the subject thread. 
	If the suspend count is zero, the thread is not currently suspended. Otherwise, 
	the subject thread's suspend count is decremented. If the resulting value is zero, 
	then the execution of the subject thread is resumed.\n 

	If the return value is zero, the specified thread was not suspended. If the 
	return value is 1, the specified thread was suspended but was restarted. 
	If the return value is greater than 1, the specified thread is still suspended.\n

	Note that while reporting debug events, all threads within the reporting process are 
	frozen. Debuggers are expected to use the SuspendThread() and ResumeThread() functions to 
	limit the set of threads that can execute within a process. By suspending all 
	threads in a process except for the one reporting a debug event, it is possible 
	to "single step" a single thread. The other threads are not released by a 
	continue operation if they are suspended.\n

	\return If the function succeeds, the return value is the thread's previous suspend count. 
		    If the function fails, the return value is -1. To get extended error information, 
			call GetLastError(). If thread handle (CThreadAPIWrapT::m_hThread) is null 
			then function return -1.
	\sa CThreadAPIWrapT::suspend()
	*/
	DWORD resume()
	{
		VERIFY_EXIT1(NOT_NULL(m_hThread),-1);
		return ::ResumeThread(m_hThread);	// see GetLastError()
	}

	/**\brief function that resume thread any where. 
	This function resume thread regardless of suspend count of the thread. it meen that 
	after calling resumefully() you will always start thread, if it was suspended before
	regardless of count of call to CThreadAPIWrapT::suspend().
	\return TRUE if thread was resumed and FALSE else where.
	*/
	BOOL resumefully()
	{
		VERIFY_EXIT1(NOT_NULL(m_hThread),FALSE);
		bool bcontinue = false;
		do 
		{
			DWORD dw = ::ResumeThread(m_hThread);
			if(EQL((DWORD)-1,dw)) return FALSE;
			bcontinue = dw>1;
		}while(bcontinue);
		return TRUE;
	}

	/**\brief verify if this thread is running now (this meen thread still do not exit). 
	\return TRUE if thread not exit still. (don`t meen that thread is suspended)
	*/
	BOOL is_running() const
	{
		if(IS_NULL(m_hThread)) return FALSE;
		DWORD dwcode = 0;
		::GetExitCodeThread(m_hThread,&dwcode);
		return EQL(dwcode,STILL_ACTIVE);
	}

	/**\brief function to wait for thread.
	\param _dwTimeout -- timeout in milisecunds (default value is INFINITE)
	\return TRUE if thread handle is signaled (thread ended or suspended).
	*/
	BOOL wait_forThread(DWORD _dwTimeout = INFINITE) const 
	{
		VERIFY_EXIT1(NOT_NULL(m_hThread),FALSE);
		return EQL(::WaitForSingleObject(m_hThread,_dwTimeout),WAIT_OBJECT_0);
	}

	/**\brief fucntion to wait for therad handle signaled or message was send to this thread.
	Use this function for thread that have windows.
	\param _dwTimeout -- timeout in milisecunds (default value INFINITE)
	\param _dwWakeMask -- wake up masks see WinAPI function MsgWaitForMultipleObjects()
	                      for details.
	\return TRUE if thread handle was signaled
	*/
	BOOL msgwait_forThread(
		DWORD _dwTimeout = INFINITE
		,DWORD _dwWakeMask = QS_ALLEVENTS|QS_ALLINPUT|QS_ALLPOSTMESSAGE
		) const
	{
		VERIFY_EXIT1(NOT_NULL(m_hThread),FALSE);
		const HANDLE handles[] = {m_hThread};

		return EQL(::MsgWaitForMultipleObjects(sizea(handles),(LPHANDLE)handles
				,FALSE
				,_dwTimeout,_dwWakeMask
				)
			,WAIT_OBJECT_0
			);
	}

	/**\brief terminate thread fucntion.
	See Q254956 why calling this could be a bad idea!
	\param _dwExitCode -- exit code to terminate thread will be set
	\return TRUE if thread was terminated
	*/
	BOOL terminate(DWORD _dwExitCode = 0)
	{
		VERIFY_EXIT1(NOT_NULL(m_hThread),FALSE);
		return ::TerminateThread(m_hThread,_dwExitCode);
	}

	/**\brief fucntion to return thread identity
	\return thread identity
	*/
	DWORD get_threadid() const {return m_dwThreadID;}

	BOOL get_exitcode(DWORD* _dwexitcode)
	{
		VERIFY_EXIT1(NOT_NULL(m_hThread),FALSE);
		VERIFY_EXIT1(!::IsBadWritePtr(_dwexitcode,sizeof(LPDWORD)),FALSE);
		return ::GetExitCodeThread(m_hThread,_dwexitcode);
	}

#if(WINVER >= 0x0500)

	/**\brief function that return thread times
	See Win API function GetThreadTimes() for more details
	\param[out] _lpCreationTime -- Pointer to a FILETIME structure that receives the creation time of the thread. 
	\param[out] _lpExitTime -- Pointer to a FILETIME structure that receives the exit time of the thread. If the thread has not exited, the content of this structure is undefined. 
	\param[out] _lpKernelTime -- Pointer to a FILETIME structure that receives the amount of time that the thread has executed in kernel mode
	\param[out] _lpUserTime -- Pointer to a FILETIME structure that receives the amount of time that the thread has executed in user mode. 
	\return If the function succeeds, the return value is nonzero.If the function fails, the return value is zero.
	*/
	BOOL get_threadTimes(
		LPFILETIME _lpCreationTime
		,LPFILETIME _lpExitTime
		,LPFILETIME _lpKernelTime
		,LPFILETIME _lpUserTime
		) const
	{
		VERIFY_EXIT1(NOT_NULL(m_hThread),FALSE);
		VERIFY_EXIT1(
			NOT_NULL(_lpCreationTime) && !::IsBadWritePtr(_lpCreationTime,sizeof(LPFILETIME))
				&& NOT_NULL(_lpExitTime) && !::IsBadWritePtr(_lpExitTime,sizeof(LPFILETIME))
				&& NOT_NULL(_lpKernelTime) && !::IsBadWritePtr(_lpKernelTime,sizeof(LPFILETIME))
				&& NOT_NULL(_lpUserTime) && !::IsBadWritePtr(_lpUserTime,sizeof(LPFILETIME))
			,FALSE
			);
		return ::GetThreadTimes(m_hThread, _lpCreationTime, _lpExitTime, _lpKernelTime, _lpUserTime);
	}

#endif // #if(WINVER >= 0x0500)

#if(WINVER >= 0x0501)

	/**\brief function sets a processor affinity mask for the specified thread.
	A thread affinity mask is a bit vector in which each bit represents the 
	processors that a thread is allowed to run on.\n
	A thread affinity mask must be a proper subset of the process affinity mask 
	for the containing process of a thread. A thread is only allowed to run on 
	the processors its process is allowed to run on.
	\param[in] _dwThreadMask -- Affinity mask for the thread.
	\return If the function succeeds, the return value is the thread's previous affinity mask.\n
			If the function fails, the return value is zero. To get extended error information, call GetLastError.
	*/
	BOOL set_threadAffinityMask(DWORD _dwThreadMask)
	{
		VERIFY_EXIT1(NOT_NULL(m_hThread),FALSE);
		return NEQL(::SetThreadAffinityMask(m_hThread, _dwThreadMask),0);
	}
	
	/**\brief function sets a preferred processor for a thread. The system schedules 
	          threads on their preferred processors whenever possible.
	You can use the GetSystemInfo function to determine the number of processors 
	on the computer. You can also use the GetProcessAffinityMask function to 
	check the processors on which the thread is allowed to run. Note that 
	GetProcessAffinityMask returns a bit mask whereas SetThreadIdealProcessor 
	uses an integer value to represent the processor.\n
	To compile an application that uses this function, define _WIN32_WINNT as 0x0400 or later.
	\param[in] _dwIdealProcessor -- Number of the preferred processor for the thread.
	           This value is zero-based. A value of MAXIMUM_PROCESSORS tells the system 
			   that the thread has no preferred processor.
	\return If the function succeeds, the return value is the previous preferred 
	        processor or MAXIMUM_PROCESSORS if the thread does not have a 
			preferred processor.\n
            If the function fails, the return value is (DWORD) – 1. To get 
			extended error information, call GetLastError.
	*/
	BOOL set_threadIdealProcessor(DWORD _dwIdealProcessor)
	{
		VERIFY_EXIT1(NOT_NULL(m_hThread),FALSE);
		return NEQL(::SetThreadIdealProcessor(m_hThread, _dwIdealProcessor),(DWORD)-1);
	}

	DWORD get_threadIdealProcessor() const
	{
		VERIFY_EXIT1(NOT_NULL(m_hThread),FALSE);
		return ::SetThreadIdealProcessor(m_hThread, MAXIMUM_PROCESSORS);
	}

#endif // #if(WINVER >= 0x0501)

	/**\brief operator to return thread handle.
	operator that can look at this class as a thread handle. 
	\return thread handle
	*/
	operator HANDLE() const { return m_hThread; }	

protected:

	/**\brief exit thread function which supports CRT case to exit
	ATTENTION: Call this function instaed of ExitThread directly call
	\param[in] _dwExitCode -- exit code for thread
	*/
	static void exit_thread(DWORD _dwExitCode = 0)
	{
#if USE_CRT_THREADING
		_endthreadex(_dwExitCode);
#else
		::ExitThread(_dwExitCode);
#endif //#if USE_CRT_THREADING
	}

	/**\brief function that creates thread.
	\param _pThreadProc -- thread procedure should type of THREAD_START_ROUTINE
	\param _pParam -- [aram to path to thread
	\param _iPriority -- thread start priority
	\param _psa -- security attributes
	*/
	BOOL create(
		LPTHREAD_START_ROUTINE _pThreadProc
		,LPVOID _pParam = NULL
		,int _iPriority = THREAD_PRIORITY_NORMAL
		,LPSECURITY_ATTRIBUTES _psa = NULL
		)
	{
		if(is_running()) return	FALSE;
		VERIFY_EXIT1(NOT_NULL(_pThreadProc),FALSE);
#if USE_CRT_THREADING
		m_hThread = (HANDLE) _beginthreadex(
			_psa
			,0
			,(UINT (WINAPI*)(void*))_pThreadProc
			,_pParam
			,CREATE_SUSPENDED
			,(UINT*)&m_dwThreadID
			);
#else
		m_hThread = ::CreateThread(
			_psa
			,0							// use default stack size
			,_pThreadProc
			,_pParam
			,CREATE_SUSPENDED
			,&m_dwThreadID
			);
#endif // #if USE_CRT_THREADING
		if(IS_NULL(m_hThread)) return FALSE; // see GetLastError() for error information of course
		if(NEQL(_iPriority,THREAD_PRIORITY_NORMAL))
		{
			if(!::SetThreadPriority(m_hThread,_iPriority)) 
			{
				ASSERT_(FALSE);// can`t set priority of the thread
			}
		}
		return ::ResumeThread(m_hThread) != (DWORD)-1;
	}
};//template<> struct CThreadAPIWrapT

typedef CThreadAPIWrapT<false> CThreadAPIWrapHandle;	///< typedef for thread api wrapper class
typedef CThreadAPIWrapT<true> CThreadAPIWrap;			///< typedef for thread api wrapper class

/**\brief strategy class
Strategy class to stop thread running using boolean variable flag
*/
struct CThreadStopAtBool
{
protected:
	volatile bool m_bStopped;		///< variable to stop
public:
	CThreadStopAtBool():m_bStopped(false){}		///< constructor
protected:
	/**\brief function that return event handle.
	\return event handle to signal thread running stop event
	*/
	HANDLE _get_eventhandle() const {return NULL;}			
	/**\brief function that clears stop flag\event
	\return TRUE if there is no error.
	*/
	BOOL _clear_abort() {m_bStopped = false; return TRUE;}
	/**\brief function that abort thread running
	\return TRUE if there is no error
	*/
	BOOL _abort() {m_bStopped = true; return TRUE;}
	/**\brief function retirn true if thread was stoped
	\return true is there is no error
	*/
	BOOL _is_aborted() const {return m_bStopped; return TRUE;}
};//struct CThreadStopAtBool

/**\brief helpfull class to handle shared between several classes event handle.
This function used for purpose of thread classses.
*/
struct CSharedEvent
{
protected:
	LONG m_counter;			///< refernces count variable 
	HANDLE m_hEvent;		///< event handle
public:

	/**\brief Constructior
	\param[in] _psa -- secure attributes to create event with
	\param[in] _bManualReset -- flag to specify reset of this event "by hands"
	\param[in] _bInitialState -- event initial state
	\param[in] _szName -- event name
	*/
	CSharedEvent(LPSECURITY_ATTRIBUTES _psa,BOOL _bManualReset,BOOL _bInitialState,LPCTSTR _szName)
		:m_hEvent(NULL),m_counter(0)
	{
		//TRACE_(_T("CSharedEvent constructed 0x%08x\n"),this);
		create(_psa,_bManualReset,_bInitialState,_szName);
	}

	/**\brief function to create event.
	If event already exit that function fails.
	\param[in] _psa -- secure attributes to create event with
	\param[in] _bManualReset -- flag to specify reset of this event "by hands"
	\param[in] _bInitialState -- event initial state
	\param[in] _szName -- event name
	*/
	void create(LPSECURITY_ATTRIBUTES _psa,BOOL _bManualReset,BOOL _bInitialState,LPCTSTR _szName)
	{
		VERIFY_EXIT(IS_NULL(m_hEvent));
		m_hEvent = ::CreateEvent(_psa,_bManualReset,_bInitialState,_szName);
	}

	/**\brief function to rule reference counter. to add reference 
	*/
	void private_addref()
	{
		ASSERT_(m_hEvent!=NULL);
		InterlockedIncrement(&m_counter);
	}

	/**\brief function to rule refernce counter. to release reference
	if reference counter became 0 than this object is deleted
	*/
	void private_release()
	{
		if(InterlockedDecrement(&m_counter)<=0)
		{
			delete trace_free(this);
		}
	}

	/**\brief operator to return event handle.
	*/
	operator HANDLE () const {return m_hEvent;}

protected:
	/**\brief function that close event handle
	*/
	void free()
	{
		if(IS_NULL(m_hEvent)) return;
		::CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}

	/**\brief protected destructor to not allow call delete operator in clent code. object
	          can be deleted only by call private_release().
	*/
	~CSharedEvent()
	{
		//TRACE_(_T("CSharedEvent destructed 0x%08x\n"),this);
		free();
	}
};//struct CSharedEvent

/**\brief startegy class to stop thread with help of event
*/
struct CThreadStopAtEvent
{
protected:
	//HANDLE m_hStopEvent;
	ptr<CSharedEvent> m_pstopevent;		///< stop event handle
public:

	/**\brief constructor
	*/
	CThreadStopAtEvent() 
	{
		m_pstopevent = trace_alloc(new CSharedEvent(NULL,TRUE,FALSE,NULL));
	}

	/**\brief destructor
	*/
	~CThreadStopAtEvent() 
	{
		m_pstopevent.release();
	}
	/**\brief fucntion that return shared event
	*/
	CSharedEvent* get_sharedevent() {return m_pstopevent;}
protected:

	/**\brief function that return event handle
	\return event handle
	*/
	HANDLE _get_eventhandle() const {return *m_pstopevent;}

	/**\brief function to clear event handle
	\return true it event has successfuly cleared
	*/
	BOOL _clear_abort() {return ::ResetEvent(*m_pstopevent);}

	/**\brief function to set event
	\return return TRUE if event was successfully set
	*/
	BOOL _abort() {return ::SetEvent(*m_pstopevent);}

	/**\brief function that verify event state and return TRUE is event was set (somebody tries to stop thread)
	\return TRUE if event was state (somebody tries to stop thread)
	*/
	BOOL _is_aborted() const 
	{
		return EQL(::WaitForSingleObject(*m_pstopevent,0),WAIT_OBJECT_0);
	}
};//struct CThreadStopAtEvent

/**\brief strategy class to stop thread|verify stop flag
*/
struct CThreadStopAtExternalEvent
{
protected:
	//HANDLE m_hStopEvent;
	ptr<CSharedEvent> m_pstopevent;		///< outer event handle
public:
	/**\brief Constructor
	*/
	CThreadStopAtExternalEvent(){}
	/**\brief Destructor
	*/
	~CThreadStopAtExternalEvent()
	{
		m_pstopevent.release();
	}

	/**\brief function to initialize this class with external stop policy that use event.
	*/
	void set_externalStopEvent(CThreadStopAtEvent& _policy)
	{
		m_pstopevent = _policy.get_sharedevent();
	}
protected:	
	/**\brief function that return event handle
	\return event handle
	*/
	HANDLE _get_eventhandle() const {return *m_pstopevent;}

	/**\brief function to clear event handle
	\return true it event has successfuly cleared
	*/
	BOOL _clear_abort() {return ::ResetEvent(*m_pstopevent);}

	/**\brief function to set event
	\return return TRUE if event was successfully set
	*/
	BOOL _abort() {return ::SetEvent(*m_pstopevent);}

	/**\brief function that verify event state and return TRUE is event was set (somebody tries to stop thread)
	\return TRUE if event was state (somebody tries to stop thread)
	*/
	BOOL _is_aborted() const 
	{
		return EQL(::WaitForSingleObject(*m_pstopevent,0),WAIT_OBJECT_0);
	}
};//struct CThreadStopAtExternalEvent

/**\brief interface to support outer information about if we need to stop thread
*/
interface IThreadState
{
	/**\brief fucntion to verify thread state (if somebody wants to stop thread)
	\return TRUE if anyone wants to stop thread running.
	*/
	virtual BOOL is_aborted() const = 0;
	virtual HANDLE get_stophandle() const = 0;
};//interface IThreadState

/**\brief implementation of work thread
\tparam _Type -- class type where DWORD thread_main() function is defined (the function that 
                 implements thread code). 
\tparam _StopPolicy -- policy that used to stop thread running
*/
template<
	typename _Type
	,typename _StopPolicy = CThreadStopAtEvent
	>
struct CWorkedThreadImpl
:
	public CThreadAPIWrap		///< CThreadAPIWrap base class create wrapper for usefull win api functions
	,public _StopPolicy			///< stop policy class
	,public IThreadState		///< this template support extended information about if we need to stop thread running
{
protected:
	typedef CWorkedThreadImpl<_Type,_StopPolicy> ThisType;	///< typedef for this 

	bool m_bAutoDelete;			///< flag to delete this object after was ended thread_main() function
	bool m_bAutoCleanup;		///< flag to stop thread when this object is destructed 
	
	DWORD m_dwCoInit;
	//CEvent_ m_stoped;
public:

	/**\brief constructor
	It set m_bAutoDelete = false and m_bAutoCleanup = true.
	*/
	CWorkedThreadImpl()
		:m_bAutoDelete(false),m_bAutoCleanup(true)
		,m_dwCoInit(0)
		//,m_stoped(TRUE,TRUE,NULL,NULL)
	{
	}

	/**\brief virtual destructor to correct delete derived thread classes when for example  \ref CWorkedThreadImpl::m_bAutoDelete == true.
	Destructor call \ref CWorkedThreadImpl::stop() function if  \ref CWorkedThreadImpl::m_bAutoCleanup == true
	*/
	virtual ~CWorkedThreadImpl()
	{
		if(m_bAutoCleanup) stop();
	}

	/**\brief COM CoInitialize dword flag
	You should set this flag before calling of start() function
	\param _dwCoInit -- CoInitialize dword flag
	*/
	void set_CoInitDWORD(DWORD _dwCoInit)
	{
		m_dwCoInit = _dwCoInit;
	}

	/**\brief function that you derived class should call before any code in you destructor
	ATTENSION: call this function before any code in your virtual desctructor will be called\n
		
	You nead to call this function in your destructor as the working thread function (thread_main())
	can use data (mostly class data memberes) after they are will be destroyed in 
	child destructor. First calls the child destruct and only after this the parent 
	destructor thats why the thread will be stoped after it data membere will be destroyed
	*/
	void predestruct()
	{
		if(m_bAutoCleanup) stop();
	}

	/**\brief function to start thread
	This function clear stop polycy flag/event before to create thread. So 
	you will be able to stop thread after this function. If derived tread class 
	support thread stop.
	\return TRUE if thread was successfully created.
	*/
	BOOL start()
	{
		if(!_StopPolicy::_clear_abort()) return FALSE;
		if(!create(ThreadProc,(LPVOID)this)) return FALSE;
		//m_stoped.ResetEvent();
		return TRUE;
	}

	/**\brief function to wait when thread handle became signalled.
	\param _ms - timeout to wait (in milisecunds, default value INFINITE)
	\return true if thread handle was signalled in specified timeout.
	*/
 	BOOL wait(DWORD _ms = INFINITE)
 	{
 		if(!is_running()) return TRUE;
 		VERIFY_EXIT1(NOT_NULL(m_hThread),FALSE);
 		HANDLE handles[] = {m_hThread};
 		if(NEQL(::WaitForMultipleObjects(sizea(handles),handles,TRUE,_ms),WAIT_OBJECT_0)) 
 			return FALSE;
 		return TRUE;
 	}

	/**\brief function that stop thread by setting of stop event and waiting 
	          when thread are exit.
	\param _dwTimeout -- timeout to wait for therad exit.
	\return TRUE if thread was successfully exit.
	*/
	BOOL stop(DWORD _dwTimeout = INFINITE)
	{
		if(!abort()) return FALSE;
		//if(!wait()) return FALSE;
		if(!wait_forThread(_dwTimeout)) return FALSE;
		return release();
	}
	
	/**\brief set abort (stop) signal to indicate that somebody what to terminate(exit) this thread. 
	this function are called from \ref CWorkedThreadImpl::stop()
	\return TRUE if stop(abort) event|flag was successfully set.
	*/
	BOOL abort()
	{
		if(IS_NULL(m_hThread))return FALSE;
		if(!_StopPolicy::_abort()) return FALSE;
		return TRUE;
	}

	/**\brief return TRUE if stop|abort event|flag is set.
	\return true if stop|abort event|flag is set (somebody want to terminate thread running).
	*/
	BOOL is_aborted() const
	{
		VERIFY_EXIT1(NOT_NULL(m_hThread),FALSE);
		return _StopPolicy::_is_aborted();
	}

	/**\brief function that set auto clean flag
	\param _bAutoClean -- value to set fro auto clean flag.
	\sa CWorkedThreadImpl::m_bAutoCleanup
	\sa CWorkedThreadImpl::~CWorkedThreadImpl()
	*/
	void set_autoClean(bool _bAutoClean = true)
	{
		m_bAutoCleanup = _bAutoClean;
	}

	/**\brief function that set auto delete flag 
	\param _bAutoDelete -- value to set for m_bAutoDelete
	\sa CWorkedThreadImpl::m_bAutoCleanup
	\sa CWorkedThreadImpl::ThreadProc()
	*/
	void set_deleteOnExit(bool _bAutoDelete = true)
	{
		m_bAutoDelete = _bAutoDelete;
		m_bAutoCleanup = !_bAutoDelete;
	}

protected:
	/**\brief thread procedure. 
	\param _pData -- thread data
	\return exit code
	\sa CWorkedThreadImpl::start()
	*/
	static DWORD WINAPI ThreadProc(LPVOID _pData)
	{
		utils::CErrorContextCreater __ErrorContextForCurrentThread;

		ThisType* pThis = reinterpret_cast<ThisType*>(_pData);
		_Type* pObject = static_cast<_Type*>(pThis);

#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
		bool bCoInit = pThis->m_dwCoInit>=0;
		if(bCoInit) ::CoInitializeEx(NULL,pThis->m_dwCoInit);
#endif // DCOM

		DWORD dwexitres = 0;
#if USE_CRT_THREADING
		DWORD dwexitres = pObject->thread_main();
#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
		if(bCoInit) ::CoUninitialize();
#endif // DCOM
		if( pThis->m_bAutoDelete ) delete trace_free(pObject);
		//else static_cast<ThisType*>(pObject)->m_stoped.SetEvent();
#else
		dwexitres = pObject->thread_main();
#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
		if(bCoInit) ::CoUninitialize();
#endif // DCOM
		if( pThis->m_bAutoDelete ) delete trace_free(pObject);
		//else static_cast<ThisType*>(pObject)->m_stoped.SetEvent();
#endif //#if USE_CRT_THREADING
		//exit_thread(dwexitres);
		return dwexitres;
	}

public:	
	/**\brief functionthat return event handle to stop|abort thread running
	*/
	HANDLE get_stophandle() const
	{
		return _StopPolicy::_get_eventhandle();
	}

//	DWORD thread_main()
//	{
//		ASSERT_(FALSE); // must override this
//		return 0;
//	}
};//template<> struct CWorkedThreadImpl


/**\brief declaration of null param
This class use to specify thread queue null param
*/
struct CQueueNullParams 
{
	bool operator==(const CQueueNullParams&) const;
	bool operator!=(const CQueueNullParams&) const;
	bool operator<(const CQueueNullParams&) const;
	bool operator>(const CQueueNullParams&) const;
};

/**\brief simple queue implementation
\tparam _Type -- struct type of queue
\tparam _ParamType -- params type
\tapram _StopPolicy -- stop policy used in queue class
*/
template<
	typename _Type
	,typename _ParamType = CQueueNullParams
	,typename _StopPolicy = CThreadStopAtEvent
	>
struct CTaskQueueSimpleImpl 
:
	public CWorkedThreadImpl<CTaskQueueSimpleImpl<_Type,_ParamType,_StopPolicy>,_StopPolicy>	///< derived from CWorkedThreadImpl template class
{
private:
	typedef CWorkedThreadImpl<CTaskQueueSimpleImpl<_Type,_ParamType,_StopPolicy>,_StopPolicy> base;	///< typedef of this type
public:
	typedef _ParamType ParamType;											///< typedef for params
	typedef void (taskFunc)(_Type*,const ParamType&,IThreadState*,HANDLE);	///< typedef for function of queue
	typedef taskFunc* taskFuncPtr;											///< typedef of queue function pointer
	typedef std::list<std::pair<taskFuncPtr,ParamType> > TasksLst;			///< typedef of list of queue function pointers and it params

	/**\brief constructor 
	\param _phost -- host of queued functions
	*/
	CTaskQueueSimpleImpl(_Type* _phost)
		:m_phost(_phost)
		,m_stoped(TRUE,TRUE,NULL,NULL)
	{
		VERIFY_EXIT(NOT_NULL(_phost));
	}

	/**\brief destructor
	*/
	~CTaskQueueSimpleImpl()
	{
		predestruct();
	}

	/**\brief main thread function
	\return thread exit code
	\todo add exception handling mechanism
	*/
	DWORD thread_main()
	{
		VERIFY_EXIT1(NOT_NULL(m_phost),0);
		m_stoped.ResetEvent();
		taskFuncPtr pfunc = NULL;
		ParamType params;
		for(pfunc=next_func(params);NOT_NULL(pfunc);pfunc=next_func(params))
		{
			try
			{
				call_func(pfunc,params);
			}
			catch(...)
			{
				ASSERT_(FALSE);
				break;
			}
			if(is_aborted()) break;
		}
		m_stoped.SetEvent();
		return 0;
	}

	/**\brief function that adds function to queue 
	\param _pfunc -- host function pointer
	\param _params -- function params
	\param _bstart -- flag to start thread if it is not running now.
	*/
	void add(taskFuncPtr _pfunc,const ParamType& _params = ParamType(),bool _bstart = true)
	{
		add_func(_pfunc,_params);
		if(_bstart 
			&& (!is_running() || !is_signaled(m_stoped))
			) start();
	}

	/**\brief function to retmove all function from queue
	\param _pfunc -- function ponter to remove
	*/
	void remove(taskFuncPtr _pfunc)
	{
		remove_allfuncs(_pfunc);
	}

	/**\brief clears all function in queue
	This function is not stop currently running function, it only 
	remove all next function list. To stop current use \ref CWorkedThreadImpl::stop()
	for example.
	*/
	void clear()
	{
		CAutoLock __al(m_critsect);
		m_tasks.clear();
	}

private:
	/**\brief function that return next function in queue and params for it
	\param[out] _param -- params of next function
	\return function pointer to run next or NULL if there is no any.
	*/
	taskFuncPtr next_func(ParamType& _param)
	{
		CAutoLock __al(m_critsect);
		if(m_tasks.empty()) return NULL;
		taskFuncPtr pfunc = m_tasks.front().first;
		_param = m_tasks.front().second;
		m_tasks.erase(m_tasks.begin());
		return pfunc;
	}

	/**\brief function to add to list of queue some function and it params.
	\param[in] _pfunc -- function pointer
	\param[in] _params -- function parameters
	*/
	void add_func(taskFuncPtr _pfunc,const ParamType& _params)
	{
		CAutoLock __al(m_critsect);
		m_tasks.push_back(TasksLst::value_type(_pfunc,_params));
	}

	/**\brief function that removes some function from list of queued functions
	\param _pfunc -- function to remove from queue
	*/
	void remove_allfuncs(taskFuncPtr _pfunc)
	{
		CAutoLock __al(m_critsect);
		TasksLst::iterator it,ite;
		it = m_tasks.begin();
		ite = m_tasks.end();
		for(;it!=ite;)
		{
			if(EQL(it->first,_pfunc))
				it = m_tasks.erase(it);
			else
				++it;
		}
	}

	/**\brief function to call some queued function
	\param[in] _pfunc -- function to call
	\param[in] _param -- params that used for function call
	*/
	void call_func(taskFuncPtr _pfunc,const ParamType& _param)
	{
		_pfunc(m_phost,_param,static_cast<IThreadState*>(this),_get_eventhandle());
	}

protected:
	_Type* m_phost;					///< queued functions host class
	TasksLst m_tasks;				///< queue ist
	CSharedObject<CCriticalSection_> m_critsect;	///< critical section to synchronize work with queue list
	CEvent_ m_stoped;				///< stop event
};//template<> struct CTaskQueueSimpleImpl 

//@}

/**\page Page_QuickStart_UtilitiesThreads Quick start: "Thread support in utilities library".

Thread utilities support you the base (and as base class) for you working thread implimentations.
To create working thread you need:
	-# Create you class derived from \ref CWorkedThreadImpl
	-# declare function DWORD thread_main() in you class. this function was running in working thread
	-# verify if you need to abort DWORD thread_main() with \ref CWorkedThreadImpl::is_aborted()
	-# or if you use \ref CThreadStopAtEvent startegy to process worked thread function abort
	   then yor use event handle with WaitForSingleObject() or  WaitForMultipleObjects() return by 
	   \ref CWorkedThreadImpl::get_stophandle().

In a code that uses the worked thread you need:
	-# create you worked class and initialize it
	-# call \ref CWorkedThreadImpl::start() to start you working thread 
	-# may be call \ref CWorkedThreadImpl::stop() or \ref CWorkedThreadImpl::abort() to stop|abort 
	   working thread function.


Lets see code snippet to demostrait this steps.
\code

// my working thread declaration/implementation

struct CMyWorkedThread 
: 
	public CWorkedThreadImpl<CMyWorkedThread,CThreadStopAtEvent>	// [1] -- create you worked thread class
{
	CMyWorkedThread(const CString_& _data)		
		:m_data(_data)
	{
	}

	DWORD thread_main()												//[2] -- declare worked thread function
	{
		HANDLE hstopevnt = get_stophandle();
		long i=0;
		for(i=0;i<m_data.GetLength();i++)
		{
			if(m_data.GetAt(i)!=_T('_')) continue;
			m_data.SetAt(i,_T(' '));			// replace character
			if(is_signaled(hstopevnt)) return;	// return if stop event is signaled //[3] verify if we need to stop worked thread function
		}
		save_to_file(m_data,hstopevnt);			// pass stop event handle to function that save data to file
												// to abort saving if stop event is signaled
		return is_signaled(hstopevnt)?1;0;		// form exit code
	}

	static void save_to_file(const CString_& _str,HANDLE _hStopEvent);
protected:
	CString_ m_data;
};

bool is_eof(const CString_& _str);
void load(CString_& _str);

// client code

void app_main_thread()
{
	CString_ data;
	for(;is_eof(data);)
	{
		load(data);							// load data 
		CMyWorkedThread workthread(data);	// [1] create worked thread object		
		workthread.start();					// [2] start worked thread function CMyWorkedThread::thread_main()
		Sleep(3000);						// wait for 3 second
		workthread.stop();					// [3] stop worked thread if it still running
	}
}
\endcode
*/

/**\page Page_QuickStart_SimpleQueueImpl Quick start: "Simple function queue suport in you application"

To create simple queue implementation in you class you need:
	-# Create paramters class for you queue functions. 
	   This class will be uses one for all queued functions, so
	   you should support all data for all functions for
	   single queue implementation.
	-# Create class that will host all functions 
	-# Create object of type \ref CTaskQueueSimpleImpl (or derive you host class from \ref CTaskQueueSimpleImpl)
	-# define queued functions (and implement them)
	-# then just call function of class \ref CTaskQueueSimpleImpl to set queued functions. 
	.

Lets see code snippet of use of simple queue implementation in you application.
\code
//[1] -- create queued functions parameters class 
struct CQueueParams
{
	bool m_bPeriod;
	CTime m_begin;
	CTime m_end;

	// first function use just date 
	CQueueParams(const CTime& _time)
		:m_begin(_time)
		,m_bPeriod(false)	// first function use just time
	{}

	CQueueParams(const CTime& _from,const CTime& _till)
		:m_begin(_from)
		,m_end(_till)
		,m_bPeriod(true)
	{}
};

struct CMyClass : public CTaskQueueSimpleImpl<CMyClass,CQueueParams>	//[2,3] -- create class that host all queue functions
{																		// and derive you host class from \ref CTaskQueueSimpleImpl
	static void set_period(					//[4] - define queue function
		CMyClass* _phost					// host class (functions static and you should pass host object by this way)
		,const CQueueParams& _params		// parametrs for functions
		,IThreadState* _pthreadstate		// function to signal if you need to stop running of queue function
		,HANDLE _hstopevent					// stop handle if is used event handle to stop queue
		);

	static void set_date(					//[4] - define queue function
		CMyClass* _phost					// host class (functions static and you should pass host object by this way)
		,const CQueueParams& _params		// parametrs for functions
		,IThreadState* _pthreadstate		// function to signal if you need to stop running of queue function
		,HANDLE _hstopevent					// stop handle if is used event handle to stop queue
		);

	void add_to_queue_set_date_func()		//[5] -- call function of CTaskQueueSimpleImpl to rule queue
	{
		// create queued function parameter
		CTime t1;
		t1 = CTime::GetCurrentTime();
		CQueueParams params(t1);

		// add function set_date to queue
		add(set_date,params);				// add function to queue
	}

	void add_to_queue_set_period_func()		//[5] -- call function of CTaskQueueSimpleImpl to rule queue
	{
		// remove function set_date from queue
		remove(set_date);					// remove function from queue

		// create queued function parameters
		CTime t1;
		t1 = CTime::GetCurrentTime();
		CTime t2(t1);
		t2 += CTimeSpan(1,0,0,0);
		CQueueParams params(t1,t2);

		// add function set_period to queue
		add(set_period,params);				// add function to queue
	}
};

void CMyClass::set_period(	
	CMyClass* _phost	
	,const CQueueParams& _params		
	,IThreadState* _pthreadstate		
	,HANDLE _hstopevent					
	)
{
	// ...
	// some function implememntation 
	// that will protractedly run in another thread
}

void CMyClass::set_date(					
	CMyClass* _phost					
	,const CQueueParams& _params		
	,IThreadState* _pthreadstate		
	,HANDLE _hstopevent					
	)
{
	// ...
	// some function implememntation 
	// that will protractedly run in another thread
}
  
\endcode

This a example of use queue class. Just call functions:
	- CMyClass::add_to_queue_set_date_func()
	- CMyClass::add_to_queue_set_period_func() 
	.
to start run of protractedly running function in functions queue. 

*/




#endif //#if !defined(__THREAD_H__1F6FA635_EE75_40a1_B9B2_3938834F5AC5)