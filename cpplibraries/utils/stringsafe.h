#if !defined(__ATTRIBUTES_H__11079D2C_CBDF_4334_A7A2_99BD2199998C_INCLUDED)
#define __ATTRIBUTES_H__11079D2C_CBDF_4334_A7A2_99BD2199998C_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "utils.h"


//#define THROW_EXCEPTION_FOR_STRINGFUNCTIONS 

// define this macro to throw exception for access violation with buffers
// don`t define this macro to return FALSE if there is access violation


/*
	���������� ��� ��������� ������, ������� ������ ��������������� �������

	1. ����������� ���������
		1.1. ������� ������
			1.1.1. ��������� �� NULL
			1.1.2. ��������� �� ����� (����������� ��� ������������)
				   + ������ ������ ����� ������
			1.1.3. ������� �������� ������ �� ������ (�������� �� _T('\0'))

	2. ������ ��� ������� ����������
		2.1. ������� ������
			2.1.1. ����� NULL -- �� ����� ������ �����������, ������� � ������� (return FALSE)
			2.1.2. ����� ����� �� ������ <=0. ������� � �������
			2.1.3. ����� ����� �� ������ ������ ����� � �������. �������� ������������� ������ 
			       ����������� ������ �� ����� �����(XXX). ����������� ���� ����������
				   ��������� � ������������ �������� ������ �� ����������� access violation 
				   ���� ���������� ���� THROW_EXCEPTION_FOR_STRINGFUNCTIONS �� ��� ���������� 
				   �������� �� ������� ���. ���� ���� �� ����������, �� ����� 
				   access violation � return FALSE
		2.2. �������� ������
			2.2.1. NULL -- ���� ������� ����� �����, �� ������� � TRUE (� ������ �� ��������)
			2.2.2. ���� ����� ����� � ������ ������ ==0 �� ������� � TRUE
			2.2.3. ���� ����� ����� � ������ ������ ������, �� ���������� �� ��� �� ����� 
			       ����������� ������������ ��� �� ������������ access violation 

	3. ����������� ��������� ������ (������ ������ ���������� _T('\0'))
		3.1. �� �������� ����������� �� ���������� '\0'
		3.2. ���� ����� ����������� '\0' �� ���������� � �����, �� ������������ ��������� 
			������ ������ ������������� '\0'
		3.3. ���� ������ �������� ������ == 0 � �������������� �� �� ����� �������� '\0'
		    �� ���������� FALSE

	4. ����������� ������ 
		4.1. ���� ������ ������� ������� �� �� ������ �������� ������, �� 
			�������� ������ ��� �� ���������� ����� ������� ������ 
			��� �� ���������� ������� '\0' � �������� 

	5. ������� ���������� FALSE (��������� ������), ���� 
		5.1. ������� ������ �� ������ (NULL ��� ������<0)
		5.2. ���������� ���������� �������� ������ � (������ ����������� '\0') 
			-- ������ ������ == 0

	6. ������� ������ ������������ �������� ����� ����� �� �������� �������, �.�. 
		�� ������������ �������� ������ (�.�. � ������� ���� ������������ �� 
		��������������������� ������)
		6.1. ����� ����������� ������ ���������� �������� 
		6.2. ���� ����������� �� �����, �� ������ ���������� ��������
		6.3. ������ ��� ������ ��������� �������� �� �����.
*/


inline BOOL strcpyn_sec(LPTSTR _Dest,long _DestSize,LPCTSTR _Src,long _SrcSize = -1)
{

	// return the empty string if arguments are bad
	if(IS_NULL(_Dest) || _DestSize<=0)
		return FALSE; // don`t need to supply contract of zero ended string

	__try
	{
		// if source size is not specified then assign it to destination size
		if(_SrcSize<0) _SrcSize = _DestSize; 
		
		if(IS_NULL(_Src) || _SrcSize==0) 
		{
			*_Dest = _T('\0');	// provide contract of zero ended string
			return TRUE;	// we need to supply contact of zero ended string
		}

		// destination buffer have free space after a copied string
		bool bFreeSpaceAfterCopyEndPos = false;
		// min(_DestSize:_SrcSize)
		long Size = (bFreeSpaceAfterCopyEndPos=(_SrcSize<_DestSize))?_SrcSize:_DestSize;

		// copy string 
		for(;Size>0 && NEQL(*_Src,_T('\0'));Size--) *_Dest++ = *_Src++;

		// if destination buffer don`t have a free space
		if(!bFreeSpaceAfterCopyEndPos && !Size) _Dest--;

		// write ending zero byte to buffer to supply contract
		*_Dest = _T('\0');
	}

#if defined(THROW_EXCEPTION_FOR_STRINGFUNCTIONS) 
	__except(EXCEPTION_CONTINUE_SEARCH)
	{
	}
	
#else
	__except((GetExceptionCode()==STATUS_ACCESS_VIOLATION)?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH)
	{
		// return the empty string if arguments are bad
		return FALSE;
	}
#endif

	return TRUE;
}

/*
	1. ����������� ���������
		1.1. NULL 
		1.2. ��������� �� ������ � ������ < 0
		1.3. �������� �� ������ � ������ == 0
		1.4. ��������� �� ������ � ������� > 0
		1.5. ��������� �� ������ � ����������� ������ (�������� ������ size<0)

	2. ������ ��� ������� ����������
		2.1. ������� ������ 
			2.1.1. NULL -- ������ -- return FALSE
			2.1.2. ��������� � ������ < 0 -- ������ -- return FALSE
			2.1.3. ��������� � ������ == 0 -- ������ -- return FALSE
			2.1.4. ��������� � ������ > 0, �� ������ ������ ��������� ������ 
				��� �� ����� ��������� ��� �� access violation
		2.2. �������� ������
			2.2.1. NULL ��� ������ == 0 -- ������ �� ����� ���������� -- return TRUE -- ��������� �������� 
			2.2.2. ��������� �� ������ � ������ > ������ ������ -- �������� �� ������ ������
			2.2.3. ��������� �� ������ � ������ < ������ ������ -- �������� �� ������� �������� ������
			2.2.4. ��������� �� ������ � ������ �� ������� (<0) -- �������� ���� �� ���������� '\0'

	3. ���������� ���������
		3.1. ���� �� ������ �� ����������, �� ���������� TRUE �� ����� ���������� �������� -- ������ ������ ��������������� � '\0'
		3.2. ���� �� ���������� ������ � ����� ��������� ����������, �� ������������� Dest[DestSize-1] = '\0'
		3.3. ���� ��������� ������, �� �������� �� ����� ��������� -- ���� ���������� FALSE �� �������� �� ����� ���������
		3.4. ���� �� �� ����� �������� ��������, �� ����� ������� FALSE

	4. ����������� 
		4.1. ������ �������� ������ == 0 -- ������������ ��������, ���� '\0' �� ���������� �� ����� ������, �� Dest[0] = '\0'
		4.2. ������ �������� ������ ������ ������� ������ -- ��������� ����� � ��������� '\0'
		4.3. ������ �������� ������ == ������� ������ -- �������� � ������� ������ Dest[DestSize-1] = '\0'
		4.4. ������ �������� ������ > ������� ������ -- �������� � ������� ������ Dest[DestSize-1] = '\0'

*/
inline BOOL strcatn_sec(LPTSTR _Dest,long _DestSize,LPCTSTR _Src,long _SrcSize = -1)
{
	// return the empty string if arguments are bad
	if(IS_NULL(_Dest) || _DestSize<=0)
		return FALSE;

	__try
	{
		// save target buffer pos and size
		LPTSTR szStart = _Dest;
		long nSize = _DestSize;

		// skip to the target _T('\0')
		for(;_DestSize>0 && NEQL(_T('\0'),*_Dest);_DestSize--,_Dest++){}

		// check free space to concatenate strings
		if(0==_DestSize)
		{
			// there is no space to concatenate strings, but it is not an error. so return TRUE
			if(EQL(_T('\0'),*_Dest)) return TRUE;
			// target string is ill-formed, so correct it and continue
			_DestSize = nSize;
			_Dest = szStart;
			*_Dest = _T('\0');
		}
		ASSERT(*_Dest==_T('\0'));
		
		// exit if there is nothing to copy
		if(IS_NULL(_Src) || _SrcSize==0)
		{
			return TRUE;
		}

		// if source size is not specified then assign it to destination size
		if(_SrcSize<0) _SrcSize = _DestSize; 

		bool bHaveFreeSpaceAfterCopyEndPos = false;
		
		// min(_DestSize:_SrcSize)
		nSize = (bHaveFreeSpaceAfterCopyEndPos = (_SrcSize<_DestSize))?_SrcSize:_DestSize;
		
		// copy string 
		for(;nSize>0 && NEQL(*_Src,_T('\0'));nSize--) *_Dest++ = *_Src++;
		
		// if destination buffer don`t have a free space
		if(!bHaveFreeSpaceAfterCopyEndPos && !nSize) _Dest--;
		
		// write ending zero byte to buffer to supply contract
		*_Dest = _T('\0');
	}
	
#if defined(THROW_EXCEPTION_FOR_STRINGFUNCTIONS) 
	__except(EXCEPTION_CONTINUE_SEARCH)
	{
	}
	
#else
	__except((GetExceptionCode()==STATUS_ACCESS_VIOLATION)?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH)
	{
		// return the empty string if arguments are bad
		return FALSE;
	}
#endif
	
	return TRUE;
}

inline BOOL strcmp_sec(LPCTSTR _s1,LPCTSTR _s2,long& _result)
{
	int ret = 0;
	__try
	{
		if(IS_NULL(_s1) || IS_NULL(_s2))
		{
			ret = _s1 - _s2;
		}
		else
		{
			for(;
				EQL(0,(ret = ((unsigned long)*_s1)-((unsigned long)*_s2))) 
					&& NEQL(_T('\0'),*_s2)
				;_s1++,_s2++
				){}

			_result = 	
				ret<0?-1
				:ret>0?1
				:0;
		}
	}
#if defined(THROW_EXCEPTION_FOR_STRINGFUNCTIONS) 
	__except(EXCEPTION_CONTINUE_SEARCH)
	{
	}
	
#else
	__except((GetExceptionCode()==STATUS_ACCESS_VIOLATION)?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH)
	{
		// return the empty string if arguments are bad
		return FALSE;
	}
#endif
	return TRUE;
}

inline BOOL strlen_sec(LPCTSTR _s,long& _result)
{

	__try
	{
		if(IS_NULL(_s)) 
		{
			_result = 0;
		}
		else
		{
			LPCTSTR s1 = _s;
			for(;*s1++;){}
			_result = s1 - _s;
		}
	}
#if defined(THROW_EXCEPTION_FOR_STRINGFUNCTIONS) 
	__except(EXCEPTION_CONTINUE_SEARCH)
	{
	}
	
#else
	__except((GetExceptionCode()==STATUS_ACCESS_VIOLATION)?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH)
	{
		// return the empty string if arguments are bad
		return FALSE;
	}
#endif
	return TRUE;
}

// any bad case
template<int _FunctionAvailability>
struct secure
{
	static BOOL strcpy___function_not_available__arg_not_array();
	static BOOL strcat___function_not_available__arg_not_array();
};

// Dest is an array
template<>
struct secure<2>
{
	static BOOL strcpy___function_not_available__arg_not_array(LPTSTR _Dest,long _DestSize,LPCTSTR _Src,long _SrcSize)
	{
		return strcpyn_sec(_Dest,_DestSize,_Src,-1);
	}
	
	static BOOL strcat___function_not_available__arg_not_array(LPTSTR _Dest,long _DestSize,LPCTSTR _Src,long _SrcSize)
	{
		return strcatn_sec(_Dest,_DestSize,_Src,-1);
	}
};

// _Dest and _Src are arrays
template<>
struct secure<3>
{
	static BOOL strcpy___function_not_available__arg_not_array(LPTSTR _Dest,long _DestSize,LPCTSTR _Src,long _SrcSize)
	{
		return strcpyn_sec(_Dest,_DestSize,_Src,_SrcSize);
	}
	
	static BOOL strcat___function_not_available__arg_not_array(LPTSTR _Dest,long _DestSize,LPCTSTR _Src,long _SrcSize)
	{
		return strcatn_sec(_Dest,_DestSize,_Src,_SrcSize);
	}
};

#define strcpy_t(_Dest,_Src)	\
	secure<((sizeof(_Dest)!=sizeof(&(_Dest)[0]))?2:0) + ((sizeof(_Src)!=sizeof(&(_Src)[0]))?1:0)>::	\
		strcpy___function_not_available__arg_not_array(_Dest,_countof(_Dest),_Src,_countof(_Src))


#define strcat_t(_Dest,_Src)	\
	secure<((sizeof(_Dest)!=sizeof(&(_Dest)[0]))?2:0) + ((sizeof(_Src)!=sizeof(&(_Src)[0]))?1:0)>::	\
		strcat___function_not_available__arg_not_array(_Dest,_countof(_Dest),_Src,_countof(_Src))




#endif // #if !defined(__ATTRIBUTES_H__11079D2C_CBDF_4334_A7A2_99BD2199998C_INCLUDED)

