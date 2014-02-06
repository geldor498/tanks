#if !defined(__SECURE_H__8BA6EC05_9523_485C_8CFB_15DB761F25ED__INCLUDED)
#define __SECURE_H__8BA6EC05_9523_485C_8CFB_15DB761F25ED__INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "config.h"

/**\brief test memory allocation for interger overflow
you should use this function if you have any code like this
\code
	new _Type[_sz]
\endcode
usage:\n
-# you should pass \ref _Type and \ref _sz to the function from operator new
-# you should call function before memory allocation and of course 
before any usage of an allocated memory (as a possible place of buffer overflow
vulnerability).
\tparam _Type -- type of memory array to allocate
\tparam _Exception -- exception type to throw if there is interger overflow
\param _sz -- array size
\attention	this is simple implementation (just for MSVC 5.0) please use SafeInt 
			class or it analog for newer compilers (see http://safeint.codeplex.com/ 
			or http://msdn.microsoft.com/en-us/library/dd570023.aspx for details 
			on SafeInt class).
*/
template<typename _Type,typename _Exception>
	void secure_test_array_allocation(size_t _sz,bool _bZeroSzOk = true)
{
	if(_sz==0 && _bZeroSzOk) return;

	unsigned __int64 res = (unsigned __int64)sizeof(_Type)*(unsigned __int64)_sz;
	if(res>0 && res<0xffffffff) return;

#if defined(USE_EXCEPTION_REFS)
	throw _Exception();
#else
	throw new _Exception();
#endif
}

#endif //#if !defined(__SECURE_H__8BA6EC05_9523_485C_8CFB_15DB761F25ED__INCLUDED)