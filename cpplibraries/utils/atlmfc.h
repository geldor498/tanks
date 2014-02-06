#if !defined(__ATLMFC_H__A01142A0_80FD_4A55_AF9F_7901D306F3CC__INCLUDED)
#define __ATLMFC_H__A01142A0_80FD_4A55_AF9F_7901D306F3CC__INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "config.h"

/*
	this header file to config for usage with MFC or ATL library
	it define typedefs and macros for correct with out dependings 
	of base library uses.
*/


#if !defined(USE_MFC) && !defined(USE_ATL)
#pragma message("Please specify USE_MFC or USE_ATL macro")
#endif

#if defined(USE_ATL)
#if !defined(USE_MFC)

	#pragma message("Used with ATL library")

	#include <atlbase.h>
	#include <atlapp.h>
	#include <atlmisc.h>
	#include <atlsync.h>
	#include <comutil.h>
	#include <comdef.h>
	#include <atltime.h>
	//#include <atltypes.h>

	#define ASSERT_ ATLASSERT
	#define TRACE_ ATLTRACE 
	typedef WTL::CString CString_;
	using ATL::CString;
	#define CString CString_
	
	typedef ATL::CCriticalSection CCriticalSection_;
	typedef ATL::CEvent CEvent_;
	typedef ATL::CTime CTime_;
	typedef WTL::CRect CRect_;
	typedef WTL::CPoint CPoint_;
	//typedef WTL::CRect CRect;
	//typedef WTL::CPoint CPoint;

	inline
	CRITICAL_SECTION* GetCriticalSectionStruct(const ATL::CCriticalSection& _) 
	{
		return const_cast<CRITICAL_SECTION*>(
				static_cast<const CRITICAL_SECTION*>(&_)
				)
			;
	}

	inline
	void ResetEvent(CEvent_& _event)
	{
		_event.Reset();
	}

	inline
	void SetEvent(CEvent_& _event)
	{
		_event.Set();
	}

	#pragma warning(disable:4290)

#endif//#if !defined(USE_MFC)
#endif//#if defined(USE_ATL)

#if defined(USE_MFC)
#if !defined(USE_ATL)

	#pragma message("Used with MFC library")

	#include <afx.h>
	#include <afxmt.h>

	typedef CString CString_;
	#define ASSERT_ ASSERT  
	#define TRACE_ TRACE 

	typedef CEvent CEvent_;
	typedef CTime CTime_;
	typedef CRect CRect_;
	typedef CPoint CPoint_;

#if(_WIN32_WINNT >= 0x0400)

	typedef CCriticalSection CCriticalSection_;

	inline
	CRITICAL_SECTION* GetCriticalSectionStruct(const CCriticalSection_& _) 
	{
		return const_cast<CRITICAL_SECTION*>(&_.m_sect);
	}

	#pragma message("Use of native CCriticalSection for _WIN32_WINNT >= 0x0400 (just WinNT, WinXP,..)")

#else

	struct CCriticalSectionEx : public CCriticalSection
	{
		bool TryEnter() const
		{
			return 0==InterlockedCompareExchange((PVOID*)&m_CurrentThread,(PVOID)(LONG)::GetCurrentThread(),0);
		}

		void Leave() const
		{
			InterlockedExchange(&m_CurrentThread,0);
		}
	protected:
		mutable LONG m_CurrentThread;
	};

	typedef CCriticalSectionEx CCriticalSection_;

	inline
	CRITICAL_SECTION* GetCriticalSectionStruct(const CCriticalSection_& _) 
	{
		return const_cast<CRITICAL_SECTION*>(&_.m_sect);
	}

	#pragma message("Use of CCriticalSectionEx for _WIN32_WINNT < 0x0400 (Win9x support, EXPERIMENTAL!)")

#endif //#if(_WIN32_WINNT >= 0x0400)

	inline
	void ResetEvent(CEvent_& _event)
	{
		_event.ResetEvent();
	}

	inline
	void SetEvent(CEvent_& _event)
	{
		_event.SetEvent();
	}

#endif//#if !defined(USE_ATL)
#endif//#if defined(USE_MFC)



#endif //#if !defined(__ATLMFC_H__A01142A0_80FD_4A55_AF9F_7901D306F3CC__INCLUDED)
