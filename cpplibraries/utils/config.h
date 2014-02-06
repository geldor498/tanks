#if !defined(__CONFIG_H__7A05B9FD_D41E_41A3_8C76_0762848FDD17_INCLUDED)
#define __CONFIG_H__7A05B9FD_D41E_41A3_8C76_0762848FDD17_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(CUSTOM_UTILITIES_CONFIG) && !defined(USE_MFC) /*&& !defined(USE_ST_CLASSES)*/ && !defined(USE_EXCEPTION_ERRORHANDLING) && !defined(USE_EXCEPTION_REFS)
	#if defined(USE_ATL)
		#define USE_EXCEPTION_ERRORHANDLING
		#define USE_EXCEPTION_REFS
		#define USE_TRANSLATION
	#else
		#define USE_MFC
		/*#define USE_ST_CLASSES*/
		#define USE_EXCEPTION_ERRORHANDLING
		#define USE_EXCEPTION_REFS
		#define USE_TRANSLATION
	#endif//#if defined(USE_ATL)
#endif

//// declare USE_ATL macro if you use <utils/...> for ATL project
#pragma message("--------- Configuration information ------------------")
//
#if defined(USE_ATL)
	#pragma message("you declare to use ATL in your project") 
#endif 

//#define USE_MFC
// declare USE_MFC macro if you use <utils/...> for MFC project

#if defined(USE_MFC)
	#pragma message("you declare to use MFC in your project")
#endif


//#define USE_EXCEPTION_ERRORHANDLING
// declare USE_EXCEPTION_ERRORHANDLING to use exception for error 
// handling system mechanism

#if defined(USE_EXCEPTION_ERRORHANDLING)
	#if !defined(_CPPUNWIND)
		#error code should be compiled with /GX (Enable Exception Handling).
	#endif
	#pragma message("you declare to use exceptions in errors handling mechanism")
#endif

//#define USE_EXCEPTION_REFS
// declare this macro if you would like to catch reference of 
// exceptions classes 
// and not define this macro to catch pointer to exception classes

#if defined(USE_EXCEPTION_REFS) 
	#pragma message("you declare to use catch blocks with references to exceptions (like: catch(CException& _ex))")
#else
	#pragma message("you declare to use catch blocks with pointers to exceptions (like: catch(CException* _pex))")
#endif


#if defined(USE_EXCEPTION_REFS) 
	#define EXC_ACCESS_TYPE	&
#else
	#define EXC_ACCESS_TYPE	*
#endif

#if defined(USE_TRANSLATION)
	#pragma message("you declare to use translation for base classes data")
	#define TR tr
#else 
	#pragma message("you declare to do not use translation for base classes data")
	#define TR 
#endif

#pragma message("------------------------------------------------------")

#if _MSC_VER >= 1200
	#define TYPENAME typename
#else 
	#define TYPENAME 
#endif


#endif//#if !defined(__CONFIG_H__7A05B9FD_D41E_41A3_8C76_0762848FDD17_INCLUDED)