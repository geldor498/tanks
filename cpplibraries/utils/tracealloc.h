#if !defined(__TRACEALLOC_H__17E19909_FB2F_4D9D_8CA9_27118097646B_INCLUDED)
#define __TRACEALLOC_H__17E19909_FB2F_4D9D_8CA9_27118097646B_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "config.h"
#include "atlmfc.h"

/**\defgroup UtilitiesMemoryAllocationTrace Utilities library support for memory allocation of library classes

Because most libraries includes before #define new DEBUG_NEW lines in CPP files so 
programmes need some tools to control memory allocation while debuging applications.
Function \ref trace_allocation() is used in every memory allocation call of cpp operator new
to trace all memory allocations and that so support search of memory leaks.
*/

//@{

/**\brief function to trace memory allocation for utilities library
\param _ptr -- allocated pointer 
\param _szFileName -- source file name 
\param _nLine -- source file line
\param _szCompileTime -- compiler time
\return pointer that pass as _ptr param
*/
template<typename _Type> inline
_Type* trace_allocation(
	_Type* _ptr
	,LPCTSTR _szFileName,long _nLine,LPCTSTR _szCompileTime
	)
{
#if defined(_DEBUG) && defined(TRACE_MEM_ALLOCATION)
	TRACE_(_T("%s(%d): memory allocate block 0x%p (%s)\n")
		,_szFileName,_nLine
		,(LPVOID)_ptr
		,_szCompileTime
		);
#endif
	return _ptr;
}

/**\brief function to trace memory freeing
\param _ptr -- allocated pointer 
\param _szFileName -- source file name 
\param _nLine -- source file line
\param _szCompileTime -- compiler time
\return pointer that pass as _ptr param
*/
template<typename _Type> 
inline
_Type* trace_memory_free(_Type* _ptr,LPCTSTR _szFileName,long _nLine,LPCTSTR _szCompileTime)
{
#if defined(_DEBUG) && defined(TRACE_MEM_ALLOCATION)
	TRACE_(_T("%s(%d): memory free block 0x%08x (%s)\n")
		,_szFileName,_nLine
		,(LPVOID)_ptr
		,_szCompileTime
		);
#endif
	return _ptr;
}


/**\macro used in every memory allocation call
\param _ptr -- pointer or expression of memory allocatio
*/
#define trace_alloc(_ptr) trace_allocation((_ptr),(LPCTSTR)_T(__FILE__),__LINE__,(LPCTSTR)_T(__TIMESTAMP__))

/**\brief macro to trace memory free places
\param _ptr -- pointer or expression of memory to free
*/
#define trace_free(_ptr) trace_memory_free((_ptr),(LPCTSTR)_T(__FILE__),__LINE__,(LPCTSTR)_T(__TIMESTAMP__))

//@}

/**\page Page_QuickStart_ControlUtilitiesMemoryAllocation Quick start: "Utilities memory allocation"

To view in IDE debug window define macro TRACE_MEM_ALLOCATION. 
Then you will be able to view all memjry allocations in utilities library.
And when you will get messages about memory leaks you will can see 
if this memory leaks happens in utilities library or not.
*/

#endif // #if !defined(__TRACEALLOC_H__17E19909_FB2F_4D9D_8CA9_27118097646B_INCLUDED)
