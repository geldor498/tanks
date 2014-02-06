#include <utils\errors.h>		//[1]

template<typename _Stream>
void print(_Stream& _cout,const CString_& _s)
{
	static CCriticalSection critsect;

	CAutoLock __lock(critsect);

	long i=0;
	for(i=0;i<_s.GetLength();i+=256)
	{
		_cout << (LPCTSTR)_s.Mid(i,256);
	}
}

struct CTestInThread : public CWorkedThreadImpl<CTestInThread>
{
	DWORD thread_main()
	{
		utils::CErrorContextCreater __ErrorContextForCurrentThread;		//[2]

		utils::CErrorTag __et1(cnamedvalue(_T("ThreadID"),GetCurrentThreadId())); //[3]
		try
		{
			Sleep(1000);
			utils::CErrorTag __et2(cnamedvalue(_T("Time"),(LPCTSTR)CTime::GetCurrentTime().Format(_T("%c"))));//[3]

			utils::throw_win32Error(ERROR_SUCCESS);	//[5]
		}
		catch(utils::CWin32Exception& _exc)
		{
			CString_ s;
			_exc.Format(s);
			print(std::cerr,s);
		}

		return 0;
	}
};

void test_errorshandling(LPCTSTR _szParam,long _nParam)
{

	utils::CErrorTag __input(
		namedvalue(_T("String param"),_szParam)
		,namedvalue(_T("long param"),_nParam)
		);											//[3]

	try
	{
		utils::CErrorTag __one(namedvalue(_T("ParamOne"),1));	//[3]

		utils::throw_win32Error(ERROR_SUCCESS);		//[5]
	}
	catch (utils::CWin32Exception& _exc)
	{
		CString s;
		_exc.Format(s);
		print(std::cerr,s);
	}

	try
	{
		utils::CErrorTag __0(namedvalue(_T("zero"),0));			//[3]
		{
			utils::CErrorTag __two(namedvalue(_T("TwoParam"),2));	//[3]
			__two << namedvalue(_T("Three"),3);						//[4]
		}
		utils::CErrorTag __1(namedvalue(_T("1"),1));				//[3]

		utils::throw_win32Error(ERROR_SUCCESS);			//[5]
	}
	catch (utils::CWin32Exception& _exc)
	{
		CString s;
		_exc.Format(s);
		print(std::cerr,s);
	}

	CTestInThread thread;
	thread.start();
	thread.wait();
}
