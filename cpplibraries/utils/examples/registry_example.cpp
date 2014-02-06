#include "..\registry.h"

#if !defined(USE_EXCEPTION_REFS)
  #error Expected USE_EXCEPTION_REFS macro definition 
  // for correct exception catching as refernces not a pointers 
#endif

static const LPCTSTR g_szKey = _T("Software\\SystemTechnologies\\ST_SB\\ConnectionList\\Connect_ALPHA_SB_user");

void test_registry()
{
	try
	{
		CRegistryKey regkey(HKEY_CURRENT_USER,g_szKey,KEY_READ|KEY_WRITE);	//[1] create registry key object
														//[3] create from parent HKEY and subkey name

		DWORD dwVal = 0;
		regkey.read_value(_T("IsActive"),dwVal);		//[6] use function to operate with registry key value
		cout << _T("IsActive = ") << dwVal << endl;

		CString dbname;
		regkey.read_value(_T("DBName"),dbname);			//[6] use function to operate with registry key value
		cout << _T("DBName = ") << (LPCTSTR)dbname << endl;

		regkey.load_value(_T("LoginName"));				//[6] use function to operate with registry key value
		CBlob blob;
		regkey.get_last(blob);							//[6] use function to operate with registry key value

		DWORD dwval1 = 0x6745f674;
		CString sServer;

		regkey 
			>> namedvalue(_T("IsActive"),dwval1)		//[4] use operator >> to read values
			>> namedvalue(_T("Server"),sServer)
			;

		cout << _T("IsActive = ") << dwval1 << endl
			<< _T("Server = ") << (LPCTSTR)sServer << endl
			;

		regkey											//[5] use operator << to write values
			<< namedvalue(_T("NewStrValue"),_T("value_value"))
			<< namedvalue(_T("NewIntValue"),10)
			;

		CString str = _T("IsActive0");
		regkey.write_value(_T("IsActive0"),str);		//[6] use functions to write value
		dwVal = !dwVal;
		regkey.write_value(_T("IsActive1"),dwVal);		//[6] use functions to write value

		regkey.write_value(_T("SomeLPCTSTR"),_T("LPCTSTR value"));
														//[6] use functions to write value
	}
	catch(utils::CWin32Exception& _exc)
	{
		CString str;
		_exc.Format(str);
		std::cout << _T("Error :") << (LPCTSTR)str << std::endl;	// show error information
	}
}
 
