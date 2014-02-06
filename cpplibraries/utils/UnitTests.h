#if !defined(__UNITTESTS_H__5E394060_D09C_4A02_BCC2_9496DDB971CA__INCLUDED)
#define __UNITTESTS_H__5E394060_D09C_4A02_BCC2_9496DDB971CA__INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "config.h"
#include "atlmfc.h"

#include <iostream>
#include <objbase.h>

#include "utils.h"

/**\defgroup SimpleUnitTests Utilities library support for simple unit tests
This classes and macroses help you to build simple unit tests. You need this classes 
and macroses only in MSVC 5.0, in letter version it is better to use CppUnits library
(http://sourceforge.net/apps/mediawiki/cppunit/index.php?title=Main_Page). 
*/

//#define TRACE_UNITTESTS_MESSAGES

//@{

/**\brief simple units test interface
*/
interface IUnitTest
{
	/**\brief test function 
	\param _cond -- condition to verify
	\param _szDescription -- description of error (if _cond is false)
	\param _szModule -- tests module
	\param _szTestName -- test name
	\param _szFile -- position in units test file 
	\param _nLine -- line number of error in units test file
	*/
	virtual void test(
		bool _cond
		,LPCTSTR _szDescription,LPCTSTR _szModule,LPCTSTR _szTestName
		,LPCTSTR _szFile,long _nLine
		) = 0;
};

/**\brief simple class to support unit tests conditions test
*/
struct OK
{
	/**\brief constructor that create instance of unit tester
	\param _pUnitTests -- interface use to collect test information
	\param _szModule -- module name
	\param _szTestName -- test name
	*/
	OK(IUnitTest* _pUnitTests,LPCTSTR _szModule,LPCTSTR _szTestName)
		:m_pUnitTests(_pUnitTests)
		,m_sModule(_szModule)
		,m_sTestName(_szTestName)
	{
	}

	/**\brief operator to verify some condition
	\param _cond -- condition to verify
	\param _szDescription -- description of error
	\param _szFile -- file name where condition is located
	\param _nLine -- file line number where condition is located
	*/
	void operator () (bool _cond,LPCTSTR _szDescription,LPCTSTR _szFile,long _nLine)
	{
		VERIFY_EXIT(NOT_NULL(m_pUnitTests));
		m_pUnitTests->test(_cond,_szDescription,m_sModule,m_sTestName,_szFile,_nLine);
	}

	/**\brief operator to verify some condition
	\param _cond -- condition to verify
	\param _szDescription -- description of error part 1
	\param _szDescription2 -- error description part 2
	\param _szFile -- file name where condition is located
	\param _nLine -- file line number where condition is located
	*/
	void operator () (bool _cond,LPCTSTR _szDescription1,LPCTSTR _szDescription2,LPCTSTR _szFile,long _nLine)
	{
		VERIFY_EXIT(NOT_NULL(m_pUnitTests));
		CString s = _szDescription1;
		s += _szDescription2;
		m_pUnitTests->test(_cond,s,m_sModule,m_sTestName,_szFile,_nLine);
	}
protected:
	CString_ m_sModule;			///< module name 
	CString_ m_sTestName;		///< test name
	IUnitTest* m_pUnitTests;	///< unit tests interface
};

/**\brief macro that specify test suite 
\param _punittests -- inits test interface 
\param _szModule -- module name 
\param _szTestName -- test name
*/
#define TEST_SUITE(_punittests,_szModule,_szTestName) OK _ok(_punittests,(_szModule),(_szTestName));

#define ok_exception(_condition,_exception,_description)	\
do{	\
	bool bres = false;	\
	CString sExceptionDescr;	\
	try	\
	{	\
		(_condition);	\
		bres = false;	\
		sExceptionDescr = _T("failed to throw exception ");	\
	}	\
	catch(_exception EXC_ACCESS_TYPE)	\
	{	\
		bres = true;	\
	}	\
	catch(...)	\
	{	\
		bres = false;	\
		sExceptionDescr = _T("failed to throw expected exception ");	\
	}	\
	sExceptionDescr += (_description);\
	_ok(bres,(LPCTSTR)sExceptionDescr,(LPCTSTR)_T(__FILE__),__LINE__);	\
}while(false);

/*#define ok_structured_exception(_condition,_exceptions_arr,_description)	\
do{	\
	bool bres = false;	\
	LPCTSTR szDescription = _T("");	\
	__try	\
	{	\
		(_condition);	\
		bres = false;	\
		szDescription = _T("failed to throw exception ");	\
	}	\
	__except(EXCEPTION_EXECUTE_HANDLER)	\
	{	\
		if(TEST_EXCEPTIONS(_exceptions_arr))	\
		{	\
			bres = true;	\
		}	\
		else	\
		{	\
			bres = false;	\
			szDescription = _T("failed to throw expected exception ");	\
			}	\
	}	\
	_ok(bres,(LPCTSTR)szDescription,(LPCTSTR)(_description),(LPCTSTR)_T(__FILE__),__LINE__);	\
}while(false);*/


#define ok_noexception(_condition,_exception,_description)	\
do{	\
	bool bres = true;	\
	CString sExceptionDescr;	\
	try	\
	{	\
		(_condition);	\
	}	\
	catch(_exception EXC_ACCESS_TYPE)	\
	{	\
		bres = false;	\
	}	\
	catch(...)	\
	{	\
		bres = false;	\
		sExceptionDescr = _T("failed to throw expected exception ");	\
	}	\
	sExceptionDescr += (_description);\
	_ok(bres,(LPCTSTR)sExceptionDescr,(LPCTSTR)_T(__FILE__),__LINE__);	\
}while(false);

/**\brief macro to test some condition
\param _condition -- condition 
\param _description -- error description
*/
#define ok(_condition,_description) _ok((_condition),(_description),(LPCTSTR)_T(__FILE__),__LINE__)


/**\brief simple implementation of \ref IUnitTest
This class output to std::cerr.
*/
struct CUnitTestsOverStdOut : public IUnitTest
{
	/**\brief constructor
	\param[in] _bVerbose -- if verbose mode is true than will be showen all tests 
	                        and if they are started with _T("OK -- ")
							then test pass ok.
	*/
	CUnitTestsOverStdOut(bool _bVerbose = false)
		:m_AllTestsCount(0)
		,m_FailedTestsCount(0)
		,m_bVerbose(_bVerbose)
	{
	}

	/**\brief function that implements \ref IUnitTest::test()
	\param _cond -- condition to verify
	\param _szDescription -- description of error (if _cond is false)
	\param _szModule -- tests module
	\param _szTestName -- test name
	\param _szFile -- position in units test file 
	\param _nLine -- line number of error in units test file
	*/
	virtual void test(
		bool _cond
		,LPCTSTR _szDescription,LPCTSTR _szModule,LPCTSTR _szTestName
		,LPCTSTR _szFile,long _nLine
		)
	{
		m_AllTestsCount++;
		if(_cond) 
		{
			if(!m_bVerbose) return;
			std::cerr << _T("OK -- ");
#if defined(TRACE_UNITTESTS_MESSAGES)
			TRACE_(_T("OK -- "));
#endif
		}
		std::cerr << _szFile << _T("(") << _nLine << _T("):") 
			<< _szDescription 
			<< std::endl
			<< _T("    in module : ") << unnull(_szModule)
			<< std::endl
			<< _T("    test name : ") << unnull(_szTestName)
			<< std::endl
			;
#if defined(TRACE_UNITTESTS_MESSAGES)
		TRACE_(_T("%s(%d):%s\n\tin module : %s\n\ttest name : %s\n")
			,_szFile,_nLine
			,_szDescription
			,_szModule,_szTestName
			);
#endif
		if(!_cond) m_FailedTestsCount++;
	}

	/**\brief function that output to std::cerr tests results
	*/
	void result()
	{
		std::cerr << _T("All tests count :") << m_AllTestsCount << std::endl
			<< _T("Failed tests count :") << m_FailedTestsCount << std::endl
			;

#if defined(TRACE_UNITTESTS_MESSAGES)
		TRACE_(_T("All tests count : %d\nFailed tests count : %d\n")
			,m_AllTestsCount,m_FailedTestsCount
			);
#endif

		if(m_FailedTestsCount==0)
			std::cerr << _T("All tests was successfully passed") << std::endl;
		else
			std::cerr << _T("There was error while tests passing") << std::endl;

#if defined(TRACE_UNITTESTS_MESSAGES)
		if(m_FailedTestsCount==0)
			TRACE_(_T("All tests was successfully passed\n"));
		else
			TRACE_(_T("There was error while tests passing\n"));
#endif

	}
protected:
	long m_AllTestsCount;			///< tests counter
	long m_FailedTestsCount;		///< count of failed tests
	bool m_bVerbose;				///< use verbose mode or not
};

//@}

/**\page Page_QuickStart_SimpleUnitTest Quick start: "Simple unit tests in the utities library"

Lets see steps you need to implement to realize unit tests:
	-# create unit tests implementation
	-# create unit test function (unit tests suite)
	-# call unit tests suiet 
	-# create test suite
	-# and create tests
	.

Lets see code snippet that illustrate this steps
\code

struct CMyException{}

struct CA
{
	long m_a;
	long m_b;

	CA()
		:m_a(10),m_b(0)
	{
	}

	CA(long _val)
		:m_a(_val)
		,m_b(_val*2)
	{
	}

	void test(long _a)
	{
		if(_a<m_a || _a>m_b) throw CMyException;
	}
};

void test_constructor(IUnitTest* _ptest)	//[2] -- create test function
{
	TEST_SUITE(_ptest,_T("class CA tests"),_T("constructors test"));	//[4] -- create test suite

	CA ca;
	ok(ca.m_a==10 && ca.m_b==0,_T("wrong constructor initialization"));	//[5] -- create tests

	CA ca1(2);
	ok(ca1.m_a==2 && ca1.m_b==4,_T("wrong constructor initialization")); //[5] -- create tests

	ca.m_a = 1;
	ca.m_b = 3;
	ok_exception(ca.test(0),CMyException&,_T("failed to test"));
}

void run_tests()
{
	CUnitTestsOverStdOut tests; // [1] -- create unit tests implementation
	test_constructor(&tests);	// [3] -- call unit tests suite 
	tests.result();				// show tests result
}
\endcode

As you see we create class CA constructors tests. 


Also example of build of Unit Test module you can find at MSVC\projects\Core\utils\UnitTests\UnitTests.dsp.
*/



#endif // #if !defined(__UNITTESTS_H__5E394060_D09C_4A02_BCC2_9496DDB971CA__INCLUDED)
