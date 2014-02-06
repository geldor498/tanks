#if !defined(__ERRORS_H__9A6C1070_9BA0_4840_99FC_F0A21402612D__INCLUDED)
#define __ERRORS_H__9A6C1070_9BA0_4840_99FC_F0A21402612D__INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <utils\config.h>
#include <utils\atlmfc.h>
#include <utils\errors.h>
#include <utils\serialize.h>

/**\brief error that can be thrown by pipe classes
*/
enum PipeErrorsEn{
	PipeError_NoError = 0					///< no error
	,PipeError_Success = 0					///< no error
	,PipeError_BadArguments					///< bad arguments was passed to function 
	,PipeError_InvalidPipeMetaData			///< error reading pipe data -- bad meta type of data block
	,PipeError_NotInitialized				///< pipe class is not initialized or failed to initialize before
	,PipeError_ReadFail						///< fails to read
	,PipeError_WriteFail					///< fails to write
	,PipeError_ConnectFail					///< fails to connect
	,PipeError_OpenFail						///< fails to open
	,PipeError_SetPipeStateFail				///< fails to set pipe state
	,PipeError_CreateFail					///< fails to create pipe
	,PipeError_DataTooLarge					///< fails to read block. see sec_criticalbuffersize constant for measure 
	,PipeError_Win32Error					///< window api error
	,PipeError_ProgrammerError				///< inner error (call programmer)
	,PipeError_StillConnecting				///< pipe classes can`t process operation because still connecting to pipe
};

struct PipeErrorsEn_Descr : public EnumDescriptionBase<PipeErrorsEn>
{
	PipeErrorsEn_Descr()
	{
		enumerator(PipeError_NoError,_T("no error"));
		enumerator(PipeError_BadArguments,_T("bad arguments was passes to function"));
		enumerator(PipeError_InvalidPipeMetaData,_T("pipe data error reading. bad metatype of data block"));
		enumerator(PipeError_NotInitialized,_T("pipe class is not initialized or failed to initialize before"));
		enumerator(PipeError_ReadFail,_T("failed to read"));
		enumerator(PipeError_WriteFail,_T("failed to write"));
		enumerator(PipeError_ConnectFail,_T("failed to connect"));
		enumerator(PipeError_OpenFail,_T("failed to open"));
		enumerator(PipeError_SetPipeStateFail,_T("failed to set pipe state"));
		enumerator(PipeError_CreateFail,_T("failed to create pipe"));
		enumerator(PipeError_DataTooLarge,_T("failed to read data block. data block too large (see sec_criticalbuffersize constant for measure)"));
		enumerator(PipeError_ProgrammerError,_T("programmer error. contact developers"));
		enumerator(PipeError_StillConnecting,_T("pipe class can`t process operation because still connecting to pipe servre side"));
	}
};

DECLARE_ENUMDESCR(PipeErrorsEn,PipeErrorsEn_Descr);


struct CPipeException : public utils::CUtilitiesExceptionBase
{
	CPipeException(PipeErrorsEn _pipeerrorcode,LPCTSTR _szDescription,LPCTSTR _szSourceInfo)
		:CUtilitiesExceptionBase(_szSourceInfo)
	{
		utils::CErrorTag __et;
		__et
			<< cnamedvalue(_T("PipeErrorCode"),(long)_pipeerrorcode)
			<< cnamedvalue(_T("PipeErrorCodeSTR"),enum2str(_pipeerrorcode))
			<< cnamedvalue(_T("PipeErrorDescription"),_szDescription)

			<< cnamedvalue(g_errtagWin32ErrorCode,::GetLastError())
			<< cnamedvalue(g_errtagWin32ErrorCodeStr,utils::GetErrorString(::GetLastError()))
			;

		initialize_exception();
	}
};

inline
void throw_pipe_errorF(PipeErrorsEn _pipeerrorcode,LPCTSTR _szDescription,LPCTSTR _szSourceInfo) 
{
#if defined(USE_EXCEPTION_REFS)
		throw CPipeException(
			_pipeerrorcode,_szDescription
			,_szSourceInfo
			);
#else
		throw trace_alloc(
			new CPipeException(
				_pipeerrorcode,_szDescription
				,_szSourceInfo
				)
			);
#endif
}

#define throw_pipe_error(_PipeErrorCode,_sDescription)	\
	throw_pipe_errorF((_PipeErrorCode),(_sDescription),CREATE_SOURCEINFO())



#endif //#if !defined(__ERRORS_H__9A6C1070_9BA0_4840_99FC_F0A21402612D__INCLUDED)
