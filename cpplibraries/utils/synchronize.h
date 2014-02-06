#if !defined(__SYNCHRONIZE_H__DEEBDCE8_9F80_4647_B4D9_648BAFE0BB82_INCLUDED)
#define __SYNCHRONIZE_H__DEEBDCE8_9F80_4647_B4D9_648BAFE0BB82_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "config.h"
#include "atlmfc.h"
#include "utils.h"


#include "nulltype.h"

/**\defgroup UtilitiesSynchronize Utilities library synchronize support
This classes and function to create synchronized code blocks or to support 
of objects access synchronization.
*/

//@{

/**\brief template class to support common mechanism of code blocks synchronize.
\tparam _Type -- synchronize object type
*/
template<typename _Type>
struct CAutoLockT
{
};

/**\brief auto lock class for cretical section.
          This class is a partitional specialization of \ref CAutoLockT for 
		  CCriticalSection_ class.
*/
template<>
struct CAutoLockT<CCriticalSection_>
{
	/**\brief constructor. It mark synchronized block start.
	\param _critsec -- critical section object used for synchronization
	*/
	CAutoLockT(CCriticalSection_& _critsec)
	{
		::EnterCriticalSection(GetCriticalSectionStruct(_critsec));
		m_phCritSect = GetCriticalSectionStruct(_critsec);
	}
	/**\brief constructor. It mark synchronized block start.
	\param _critsec -- critical section object used for synchronization
	*/
	CAutoLockT(const CCriticalSection_& _critsec)
	{
		::EnterCriticalSection(GetCriticalSectionStruct(_critsec));
		m_phCritSect = GetCriticalSectionStruct(_critsec);
	}
	/**\brief constructor. It mark synchronized block start.
	\param hCritSect -- critical section object used for synchronization
	*/
	CAutoLockT(CRITICAL_SECTION& hCritSect)
	{
		::EnterCriticalSection(&hCritSect);
		m_phCritSect = &hCritSect;
	}
	/**\brief constructor. It mark synchronized block start.
	\param hCritSect -- critical section object used for synchronization
	*/
	CAutoLockT(const CRITICAL_SECTION& hCritSect)
	{
		::EnterCriticalSection((CRITICAL_SECTION*)&hCritSect);
		m_phCritSect = (CRITICAL_SECTION*)&hCritSect;
	}
	/**\brief Destructor used to mark end of synchronized code block
	*/
	~CAutoLockT()
	{
		::LeaveCriticalSection(m_phCritSect);
	}
protected:
	CRITICAL_SECTION* m_phCritSect;		///< critical section object pointer
};

/**\brief null specialization. used to create not synchronized code blocks
*/
template<>
struct CAutoLockT<NullType>
{
	/**\brief Constructor.
	*/
	CAutoLockT(NullType&){}

	/**\brief Destructor.
	*/
	CAutoLockT(const NullType&){}
};

#if defined(USE_MFC)
/**\brief Mutex specialization
This class create synch code block syncronized with CMutext class
*/
template<>
struct CAutoLockT<CMutex>
{
	/**\brief Constructor. Mark start of synch code block 
	\param _mutex -- mutext object used to synch code block
	*/
	CAutoLockT(CMutex& _mutex):m_sl(&_mutex,TRUE){}

	/**\brief Constructor. Mark start of synch code block 
	\param _mutex -- mutext object used to synch code block
	*/
	CAutoLockT(const CMutex& _mutex):m_sl((CMutex*)&_mutex,TRUE){}

	/**\brief Destructor. Mark end of the synch code block
	*/
	~CAutoLockT()
	{m_sl.Unlock();}
protected:
	CSingleLock m_sl;	///< data that is used to synch block
};

/**\brief specialization of lock object to use CSemaphore to synchronize code block
*/
template<>
struct CAutoLockT<CSemaphore>
{
	/**\brief Constructor. Mark begin of synchronized code block with CSemaphore object.
	\param _sem -- object used for synchronization
	*/
	CAutoLockT(CSemaphore& _sem):m_sl(&_sem,TRUE){}

	/**\brief Constructor. Mark synchronized code block with CSemaphore object.
	\param _sem -- object used for synchronization
	*/
	CAutoLockT(const CSemaphore& _sem):m_sl((CSemaphore*)&_sem,TRUE){}

	/**\brief Destructor. Mark end of synchronized code block.
	*/
	~CAutoLockT()
	{m_sl.Unlock();}
protected:
	CSingleLock m_sl;	///< data that is used to synch code block
};

typedef CAutoLockT<CMutex> CAutoLockMutex;				///< typedef of historical name
typedef CAutoLockT<CSemaphore> CAutoLockSemaphore;		///< typedef of historical name
#endif //#if defined(USE_MFC)

typedef CAutoLockT<CCriticalSection_> CAutoLock;			///< typedef of historical name

/**\brief classt that used to declare syncj code block for some data type
\tparam _Item -- item type
\tparam _SyncObj -- synchronize type
*/
template<typename _Item,typename _SyncObj = CCriticalSection_>
struct CUseLocked : public CAutoLockT<_SyncObj>
{
	/**\brief Costructor that starts synch code block for parameter data
	\param _item -- data object to create synch block for access to it fields.
	*/
	CUseLocked(_Item& _item)
		:CAutoLockT<_SyncObj>(_item)
		,m_item(_item)
	{
	}

	/**\brief function that returns data type refernce
	\return contained data reference
	*/
	_Item& get_ref() {return m_item;}

	/**\brief function that returns data type constant refernce
	\return contained data constant reference
	*/
	const _Item& get_ref() const {return m_item;}

	/**\brief function that return contained type pointer
	\return contained type pointer
	*/
	_Item* get_ptr() {return &m_item;}

	/**\brief function that return contained type constant pointer
	\return contained type constant pointer
	*/
	const _Item* get_ptr() const {return &m_item;}

	/**\brief operator to return contained item type reference
	\return contained object reference
	*/
	operator _Item& () {return get_ref();} 

	/**\brief operator to return contained item type constant reference
	\return contained object constant reference
	*/
	operator const _Item& () const {return get_ref();} 

	/**\brief operator to return contained type pointer
	\return contained type pointer
	*/
	operator _Item* () {return get_ptr();} 

	/**\brief operator to return contained type const pointer
	\return contained type const pointer
	*/
	operator const _Item* () const {return get_ptr();} 

	/**\brief operator to return contained type pointer
	\return contained type pointer
	*/
	_Item* operator->() {return get_ptr();}

	/**\brief operator to return contained type constant pointer
	\return contained type constant pointer
	*/
	const _Item* operator->() const {return get_ptr();}

	/**\brief operator to return contained item type reference
	\return contained object reference
	*/
	_Item& operator * () {return get_ref();}

	/**\brief operator to return contained item type constant reference
	\return contained object constant reference
	*/
	const _Item& operator * () const {return get_ref();}

protected:
	_Item& m_item;				///< item reference
};//template<> struct CUseLocked

struct CantEnterCriticalSection{};	///< type to specify situation when you can enter into critical section

template<typename _Type>
struct CAutoTryLockT
{
	CAutoTryLockT(_Type&);
	CAutoTryLockT(const _Type&);
	~CAutoTryLockT();
};


#if(_WIN32_WINNT >= 0x0400)

/**\brief class to try to enter into critical section
*/
template<>
struct CAutoTryLockT<CCriticalSection_>
{

	/**\brief constructor and start point of synch block
	This constructor tries to enter critical section and if it is impossible then 
	throw exception of type \ref CantEnterCriticalSection
	\param hCritSect -- critical section
	\throw CantEnterCriticalSection
	*/
	CAutoTryLockT(CRITICAL_SECTION& hCritSect) throw(CantEnterCriticalSection)
		:m_phCritSect(NULL)
	{
		if(!::TryEnterCriticalSection(&hCritSect)) throw CantEnterCriticalSection();
		m_phCritSect = &hCritSect;
	}

	/**\brief constructor and start point of synch block
	This constructor tries to enter critical section and if it is impossible then 
	throw exception of type \ref CantEnterCriticalSection
	\param _critsec -- critical section
	\throw CantEnterCriticalSection
	*/
	CAutoTryLockT(CCriticalSection_& _critsec) throw(CantEnterCriticalSection)
		:m_phCritSect(NULL)
	{
		if(!::TryEnterCriticalSection(GetCriticalSectionStruct(_critsec))) throw CantEnterCriticalSection();
		m_phCritSect = GetCriticalSectionStruct(_critsec);
	}

	/**\brief constructor and start point of synch block
	This constructor tries to enter critical section and if it is impossible then 
	throw exception of type \ref CantEnterCriticalSection
	\param _hCritSect -- critical section
	\throw CantEnterCriticalSection
	*/
	CAutoTryLockT(const CRITICAL_SECTION& _hCritSect) throw(CantEnterCriticalSection)
		:m_phCritSect(NULL)
	{
		if(!::TryEnterCriticalSection(const_cast<CRITICAL_SECTION*>(&_hCritSect))) 
			throw CantEnterCriticalSection();
		m_phCritSect = (CRITICAL_SECTION*)&_hCritSect;
	}

	/**\brief constructor and start point of synch block
	This constructor tries to enter critical section and if it is impossible then 
	throw exception of type \ref CantEnterCriticalSection
	\param _critsec -- critical section
	\throw CantEnterCriticalSection
	*/
	CAutoTryLockT(const CCriticalSection_& _critsec) throw(CantEnterCriticalSection)
		:m_phCritSect(NULL)
	{
		if(!::TryEnterCriticalSection(GetCriticalSectionStruct(_critsec))) 
			throw CantEnterCriticalSection();
		m_phCritSect = GetCriticalSectionStruct(_critsec);
	}

	/**\brief destructor and end point of synch block
	*/
	~CAutoTryLockT()
	{
		if(NOT_NULL(m_phCritSect))
			::LeaveCriticalSection(m_phCritSect);
	}	
protected:
	CRITICAL_SECTION* m_phCritSect;		///< pointer to critical section used to create synch code block
};

template<>
struct CAutoTryLockT<NullType>
{
	CAutoTryLockT(NullType&) {}
	CAutoTryLockT(const NullType&) {}
	CAutoTryLockT(NullType*) {}
	CAutoTryLockT(const NullType*) {}
	~CAutoTryLockT() {}
};


#else //#if(_WIN32_WINNT >= 0x0400)

template<>
struct CAutoTryLockT<CCriticalSection_>
{
	/**\brief constructor and start point of synch block
	This constructor tries to enter critical section and if it is impossible then 
	throw exception of type \ref CantEnterCriticalSection
	\param _critsec -- critical section
	\throw CantEnterCriticalSection
	*/
	CAutoTryLockT(CCriticalSection_& _critsec) throw(CantEnterCriticalSection)
		:m_phCritSect(NULL)
		,m_pCritSectObj(NULL)
	{
		if(!_critsec.TryEnter())  throw CantEnterCriticalSection();
		m_pCritSectObj = &_critsec;
		m_phCritSect = GetCriticalSectionStruct(_critsec);
		EnterCriticalSection(m_phCritSect);
	}

	/**\brief constructor and start point of synch block
	This constructor tries to enter critical section and if it is impossible then 
	throw exception of type \ref CantEnterCriticalSection
	\param _critsec -- critical section
	\throw CantEnterCriticalSection
	*/
	CAutoTryLockT(const CCriticalSection_& _critsec) throw(CantEnterCriticalSection)
		:m_phCritSect(NULL)
		,m_pCritSectObj(NULL)
	{
		if(!_critsec.TryEnter())  throw CantEnterCriticalSection();
		m_pCritSectObj = &_critsec;
		m_phCritSect = GetCriticalSectionStruct(_critsec);
		EnterCriticalSection(m_phCritSect);
	}

	/**\brief destructor and end point of synch block
	*/
	~CAutoTryLockT()
	{
		if(NOT_NULL(m_phCritSect))
		{
			if(NOT_NULL(m_pCritSectObj)) m_pCritSectObj->Leave();
			LeaveCriticalSection(m_phCritSect);
		}
	}	
protected:
	CRITICAL_SECTION* m_phCritSect;		///< pointer to critical section used to create synch code block
	const CCriticalSection_* m_pCritSectObj;
};

#endif /* _WIN32_WINNT >= 0x0400 */

typedef CAutoTryLockT<CCriticalSection_> CAutoTryLock;

template<typename _Object>
CCriticalSection_& get_syncobject(const _Object& _obj,const CCriticalSection_&)
{
	return const_cast<CCriticalSection_&>(static_cast<const CCriticalSection_&>(_obj));
}

//template<typename _Object>
//CCriticalSection_& get_syncobject(const _Object* _pobj,const CCriticalSection_&)
//{
//	return get_syncobject(*_pobj);
//}

template<typename _Object>
NullType& get_syncobject(const _Object& _obj,const NullType&)
{
	return Fish<NullType>::get();
}


//@}

/**\page Page_QuickStart_Synchronization Quick start: "Using syncronization classes in you application"

Clases \ref CAutoLockT, \ref CAutoLock, \ref CAutoLockMutex, \ref CAutoLockSemaphore
and \ref CUseLocked are used to create synchronized code blocks. Such code blocks that 
set special rules to enter/leave them. 

Lets see code snippet for \ref CAutoLock.

\code
struct CData
{
	CString_ m_s;
};

static CData g_data;
static CCriticalSection_ g_func1_critsect;

void func1(long _a)
{
	// local data we don`t need synchronization for it
	_a = _a*20;											// place [0]
	_a -= 0x20;											// ...
	_a &= 0xffff;										// place [0]

	// we need synchronization for next code
	CAutoLock __lock(g_func1_critsect);					// place [1]
	g_data = format(_T("%d"),_a);							// place [2]
}														// place [3]
\endcode

Suppose that function func1() can be called from different thread at same 
time. Place [0] in this function works fine in this situation, buf place 2
can fail. We need synchronization for place 2. \ref CAutoLock object 
create local variable and in constructor calls function that starts 
critical section and call fucntion to end critical section in destructor
(it mean at end of code block in which this variable was created).
Now places 1, 2 and 3 will be called only from on thread at one time.

It starts critical section block from place where locakl variable was created
to place where this locakl variable will be destructed. So it is very easy
to declare synch code block. Just remember of life time of local variables. 

Similar are used classes \ref CAutoLockMutex and \ref CAutoLockSemaphore
except that they are using another synch types. 

Some times data classes are created with some synch object to rule access to 
it members. In this case you can use \ref CUseLocked class. Lets see nxt code 
snippet.

\code
struct CA												// user class 
{
	CString_ m_a;										// class variable
	operator CCriticalSection_& () {return m_critsect;}	// operator that return critical section object reference
protected:
	CSharedObject<CCriticalSection_> m_critsect;			// critical section object. one for every CA object
};

void func2(CA& _ca,long _a)
{
	_a = _a*20;	
	_a -= 0x20;	
	_a &= 0xffff;

	CUseLocked<CA> lockedptr(_ca);						// synch all call to locked variable field 
	lockedptr->m_a = format(_T("a = %d"),_a);
}
\endcode

As you see in this code snippet you create auto ptr variable 
and at same time we create critical section for a life time of 
that auto ptr variable. So we can`t modify locked variable without 
locking.
*/



#endif //#if !defined(__SYNCHRONIZE_H__DEEBDCE8_9F80_4647_B4D9_648BAFE0BB82_INCLUDED)