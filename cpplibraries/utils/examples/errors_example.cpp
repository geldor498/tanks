#include <utils\errors.h>

#if !defined(USE_EXCEPTION_REFS)
  #error Expected USE_EXCEPTION_REFS macro definition 
  // for correct exception catching as refernces not a pointers 
#endif

bool example(const CString& _filename,const CString& _data2save)
{
	bool bret = true;
	HANDLE hfile = INVALID_HANDLE_VALUE;
	try
	{
		if(_filename.IsEmpty()) 
			utils::throw_win32Error(ERROR_BAD_ARGUMENTS); // error if file name not specified

		hfile = ::CreateFile(
			_filename
			,GENERIC_READ
			,FILE_SHARE_READ
			,NULL
			,CREATE_NEW
			,FILE_ATTRIBUTE_NORMAL
			,NULL
			);
		if(hfile==INVALID_HANDLE_VALUE) 
			utils::throw_lastWin32Error();	//throw current win API error

		DWORD dwWritten = 0;
		if(!WriteFile(hfile,(LPCTSTR)_data2save,_data2save.GetLength(),&dwWritten,NULL)
			|| dwWritten!=_data2save.GetLength()
			)
		{
			utils::throw_lastWin32Error(); // throw current win api error
		}

		if(!::CloseHandle(hfile)) 
		{
			hfile = INVALID_HANDLE_VALUE;
			utils::throw_lastWin32Error();
		}
		hfile = INVALID_HANDLE_VALUE;
	}
	catch(utils::CWin32Exception& _exc)		// need USE_EXCEPTION_REFS macro defined for this catch style
	{
		CString sError;
		_exc.Format(sError);
		std::cout << _T("Error \"") << (LPCTSTR)sError << std::endl;
		bret = false;
	}
	if(hfile!=INVALID_HANDLE_VALUE) 
	{
		::CloseHandle(hfile);
		hfile = INVALID_HANDLE_VALUE;
	}
	return bret;
}
