#if !defined(__AUTOPTR_H__87476E7F_C04B_4FCE_9400_8D94AF36E589_INCLUDED)
#define __AUTOPTR_H__87476E7F_C04B_4FCE_9400_8D94AF36E589_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "config.h"
#include "atlmfc.h"
#include "utils.h"


/**\defgroup UtilitiesAutoPtrClasses Utilities support for auto pointers.
Mostly this classes used inside of utilities library. You can use in you 
code more usual classes. 
*/

//@{

/**\brief simple auto ptr class. It use classes that support functions: 
          private_addref() and private_release() to control 
		  life time of member pointer. 
can`t used in container classes. because this class overload operator & ().
\tparam _Type -- contained type
*/
template<typename _Type>
struct ptr
{
	/**\brief Constructor that create empty auto pointer.
	*/
	ptr():m_ptr(NULL)
	{
	}

	/**\brief Constructor that create auto pointer class from some pointer 
	          to specified type.
	\param _p -- pointer
	*/
	ptr(_Type* _p)
	{
		if(NOT_NULL(m_ptr = _p)) 
			m_ptr->private_addref();
	}

	/**\brief copy constructor.
	\param _p -- auto pointer class to create auto ptr as copy of.
	*/
	ptr(const ptr<_Type>& _p)
	{
		if(NOT_NULL(m_ptr=(_Type*)(const _Type*)_p))
			m_ptr->private_addref();
	}

	/**\brief Destructor.
	*/
	~ptr()
	{
		if(NOT_NULL(m_ptr))
			m_ptr->private_release();
	}

	/**\brief function to release data from auto ptr.
	*/
	void release()
	{
		if(NOT_NULL(m_ptr))
			m_ptr->private_release();
		m_ptr = NULL;
	}

	/**\brief operator that return pointer to contaned type. 
	\return \ref _Type pointer 
	*/
	operator _Type* () {return m_ptr;}

	/**\brief operator that return constant pointer to contaned type. 
	\return constatnt \ref _Type pointer 
	*/
	operator const _Type* () const {return m_ptr;}

	/**\brief function that return pointer to contaned type. 
	\return \ref _Type pointer 
	*/
	_Type* get() {return m_ptr;}

	/**\brief function that return constant pointer to contaned type. 
	\return constatnt \ref _Type pointer 
	*/
	const _Type* get() const {return m_ptr;}

	/**\brief operator to return reference to comtained object
	\return reference to contained object
	*/
	_Type& operator* () 
	{
		ASSERT_(NOT_NULL(m_ptr));			
		return *m_ptr;
	}

	/**\brief operator to return pointer for contained type
	\return contained object pointer
	*/
	_Type* operator -> () 
	{
		ASSERT_(NOT_NULL(m_ptr));
		return m_ptr;
	}

	/**\brief operator to return constant pointer for contained type
	\return contained object constant pointer
	*/
	const _Type* operator -> () const 
	{
		ASSERT_(m_ptr);
		return m_ptr;
	}

	/**\brief assignment operator
	\param _p -- pointer to assign value of
	\return this reference
	*/
	ptr<_Type>& operator=(const ptr<_Type> _p)
	{
		if(EQL(m_ptr,_p.m_ptr)) return *this;
		if(NOT_NULL(m_ptr)) m_ptr->private_release();
		if(NOT_NULL(m_ptr = (_Type*)(const _Type*)_p))
			m_ptr->private_addref();
		return *this;
	}

	/**\brief assignment operator
	\param _p -- pointer to asign
	\return this reference
	*/
	ptr<_Type>& operator=(_Type* _p)
	{
		if(EQL(m_ptr,_p)) return *this;
		if(NOT_NULL(m_ptr)) m_ptr->private_release();
		if(NOT_NULL(m_ptr = _p))
			m_ptr->private_addref();
		return *this;
	}
protected:
	_Type* m_ptr;				///< contained object
};//template<> struct ptr

/**\brief class that implement reference count semantic for derived classes
*/
struct CRefcounted
{
	/**\brief Constructor.
	*/
	CRefcounted()	:m_nRefCnt(0) {}
	/**\brief Destructor.
	*/
	virtual ~CRefcounted()
	{
		ASSERT_(m_nRefCnt==0);
	}

	/**\brief fucntion to add reference to object
	\return count value. see win api function InterlockedIncrement() for more info
	*/
	LONG AddRef()	{return InterlockedIncrement(&m_nRefCnt);}

	/**\brief function to release refernce to object.
	And delete object if it was lst release.
	\return count value. see win api function InterlockedDecrement() for more info
	*/
	LONG Release() 
	{
		LONG ret = 0;
		if(NEQL(0,(ret=InterlockedDecrement(&m_nRefCnt)))) return ret;
		delete trace_free(this);
		return 0;
	}
private:
	LONG m_nRefCnt;			///< reference count variable
};//struct CRefcounted

/*
	shared ptr class for types that was derived from CRefcounted
*/

/**\brief aouto ptr class that user with is classed derived from \ref CRefcounted
\tparam RefcountedType -- reference counter class
*/
template <typename RefcountedType>
class CPtrShared
{
public:
	typedef CPtrShared<RefcountedType> ThisType;	///< This type

	/**\brief Copy constructor.
	\param _ -- pointer to assign to this class
	*/
	CPtrShared(const CPtrShared& _)	:m_p(NULL){operator=(_);};

	/**\brief Constructor. 
	\param _p -- pointre to assign
	*/
	CPtrShared(RefcountedType* _p = NULL)	:m_p(NULL){operator=(_p);};

	/**\brief Destructor.
	*/
	~CPtrShared()
	{
		operator=((RefcountedType*)NULL);
	};

	/**\brief assignment operator 
	\param _ -- auto ptr to assign pointer from
	\return this reference
	*/
	ThisType& operator=(ThisType& _)
	{
		if(*this==_.m_p) return *this;
		if(NOT_NULL(m_p)) m_p->Release();
		m_p = _,m_p;
		if(NOT_NULL(m_p)) m_p->AddRef();
		return *this;
	}

	/**\brief assignment operator
	\param _p -- pointer to assign
	\return this reference
	*/
	ThisType& operator=(RefcountedType* _p)
	{
		if(*this==_p) return *this;
		if(NOT_NULL(m_p)) m_p->Release();
		m_p = _p;
		if(NOT_NULL(m_p)) m_p->AddRef();
		return *this;
	}

	/**\brief compare operator
	\param _ -- pointer to compare
	\return true if pointers are equal
	*/
	bool operator == (const ThisType& _) const {return m_p==_.m_p;}

	/**\brief compare operator
	\param _p -- pointer to compare
	\return true if pointers are equal
	*/
	bool operator == (RefcountedType* _p) const {return m_p==_p;}

	bool operator == (int _null) const {VERIFY_EXIT1(_null==NULL,false);return IS_NULL(m_p);}
	bool operator != (int _null) const {VERIFY_EXIT1(_null==NULL,false);return NOT_NULL(m_p);}

	/**\brief compare operator
	\param _ -- pointer to compare
	\return true if pointers are not equal
	*/
	bool operator != (const ThisType& _) const	{return !operator==(_);}

	/**\brief compare operator
	\param _p -- pointer to compare
	\return true if pointers are not equal
	*/
	bool operator != (RefcountedType* _p) const {return !operator==(_p);}

	/**\brief valid operator
	\return true if inner pointer is not NULL
	*/
	operator bool() const {return NOT_NULL(m_p);};

	/**\brief valid operator
	\return true if inner pointer is NULL
	*/
	bool operator !() const {return !operator bool();};

	/**\brief operator to return inner pointer
	\return contained pointer
	*/
	operator RefcountedType* ()   const { return m_p; }

	/**\brief operator to return inner pointer
	\return contained pointer
	*/
	RefcountedType* operator ->() const { return m_p; }

	RefcountedType* get() const {return m_p;}

	/**\brief function to copy pointer to other pointer
	\param _pp -- pointer to copy to
	*/
	void CopyTo(RefcountedType** _pp) {*_pp = m_p; m_p->AddRef();}

protected:
	RefcountedType* m_p;		///< contained pointer.
};//template<> class CPtrShared

template<typename _RefcountedType> inline
	bool operator == (int _null,const CPtrShared<_RefcountedType>& _autoptr)
{
	return _autoptr==_null;
}

template<typename _RefcountedType> inline
	bool operator != (int _null,const CPtrShared<_RefcountedType>& _autoptr)
{
	return _autoptr!=_null;
}

template<typename _RefcountedType> inline
	bool operator == (_RefcountedType* _p,const CPtrShared<_RefcountedType>& _autoptr)
{
	return _autoptr==_p;
}

template<typename _RefcountedType> inline
	bool operator != (_RefcountedType* _p,const CPtrShared<_RefcountedType>& _autoptr)
{
	return _autoptr!=_p;
}

/**\brief one another method to rule of object reference count
Class that allocate some data with it counter.
*/
template<typename _Type>
struct CSharedObjectData
{
	CSharedObjectData():counter(0){}

	LONG counter;
	_Type data;
};

/**\brief auto ptr that use \ref CSharedObjectData to hold data and counters 
          for data.
\tparam _Type -- auto pointer type
*/
template<typename _Type>
class CSharedObject
{
public:

	/**\brief Constructor.
	It create new data with operator new() and add reference to object
	*/
	CSharedObject():m_pdata(NULL)
	{
		m_pdata = trace_alloc(new CSharedObjectData<_Type>());
		addref();
	}

	/**\brief constructor.
	*/
	CSharedObject(CSharedObjectData<_Type>* _p)
		:m_pdata(NULL)
	{
		operator = (_p);
	}

	/**\brief Destructor. it release owned object.
	*/
	~CSharedObject()
	{
		release();
	}

	/**\brief Copy constructor.
	\param _ -- pointer to copy value from
	*/
	CSharedObject(const CSharedObject& _):m_pdata(NULL)
	{
		operator=(_);
	}

	/**\brief Copy operator.
	\param _ -- pointer to copy value from.
	\return this reference
	*/
	CSharedObject& operator=(const CSharedObject& _)
	{
		if(NEQL(m_pdata,_.m_pdata))
		{
			release();
			m_pdata = _.m_pdata;
			addref();
		}
		return *this;
	}

	/**\brief assignment operator 
	\param _p -- pointer to assign 
	\return this reference
	*/
	CSharedObject& operator=(CSharedObjectData<_Type>* _p)
	{
		assign(_p);
		return *this;
	}
	
	/**\brief function that return contained constant pointer
	\return contained data constant pointer.
	*/
	const _Type* get() const {return get_inner();}

	/**\brief function that return contained pointer
	\return contained data pointer.
	*/
	_Type* get() {return const_cast<_Type*>(get_inner());}

	/**\brief function that return contained data constant reference
	\return contained object constant reference
	*/
	const _Type& get_ref() const {return get_refinner();}

	/**\brief function that return contained data reference
	\return contained object reference
	*/
	_Type& get_ref() {return const_cast<_Type&>(get_refinner());}

	/**\brief operator to return contained pointer
	\return contained pointer
	*/
	operator _Type*() {return get();}

	/**\brief operator to return contained constant pointer
	\return contained constant pointer
	*/
	operator const _Type*() const {return get();}

	/**\brief opertor to get pointer
	\return pointer to cotained type
	*/
	_Type* operator->() {return get();}

	/**\brief operator to return pointer of contained type
	\return contained type pointer
	*/
	const _Type* operator->() const {return get();}

	/**\brief operator to return constant reference of contained type
	\return contained type constant reference
	*/
	const _Type& operator * () const {return get_ref();}

	/**\brief operator to return reference of contained type
	\return contained type reference
	*/
	_Type& operator* () {return get_ref();}

	/**\brief operator to return constant reference of contained type
	\return contained type constant reference
	*/
	operator const _Type& () const {return get_ref();}

	/**\brief operator to return reference of contained type
	\return contained type reference
	*/
	operator _Type& () {return get_ref();}

protected:
	/**\brief function to assign pointer value to this auto ptr class
	\param _p -- pointer to assign
	*/
	void assign(CSharedObjectData<_Type>* _p)
	{
		if(EQL(_p,m_pdata)) return;
		addref(_p);
		release(m_pdata);
		m_pdata = _p;
	}

	/**\brief function that return constant contained pointer
	\return constant contained pointer
	*/
	const _Type* get_inner() const	
	{
		VERIFY_EXIT1(NOT_NULL(m_pdata),NULL);
		return &(m_pdata->data);
	}

	/**\brief function to return constant reference of contained type
	\return contained type constant reference
	*/
	const _Type& get_refinner() const 
	{
		const _Type* ptr = get_inner();
		if(IS_NULL(ptr))
		{
			ASSERT_(FALSE);
			//use of the incorrect initialized or destroyed object
			static _Type fish;
			return fish;
		}
		return *ptr;
	}

	/**\brief function to release reference to pointer
	\param _p -- pointer
	*/
	static void release(CSharedObjectData<_Type>*& _p)
	{
		if(IS_NULL(_p)) return;
		if(EQL(0,InterlockedDecrement(&(_p->counter))))
		{
			delete trace_free(_p);
			_p = NULL;
		}
	}

	/**\brief function to add reference to pointer
	\param _p -- pointer
	*/
	static void addref(CSharedObjectData<_Type>*& _p)
	{
		if(IS_NULL(_p)) return;
		InterlockedIncrement(&(_p->counter));
	}
	
	/**\brief function to add refernce for contained object
	*/
	void addref()
	{
		if(IS_NULL(m_pdata)) return;
		InterlockedIncrement(&(m_pdata->counter));
	}

	/**\brief function to release references for contained object
	*/
	void release()
	{
		if(IS_NULL(m_pdata)) return;
		if(EQL(0,InterlockedDecrement(&(m_pdata->counter))))
		{
			delete trace_free(m_pdata);
			m_pdata = NULL;
		}
	}
protected:
	CSharedObjectData<_Type>* m_pdata;		///< contained object pointer

private:
	//use of this operators/functions may lead to corruption of counter value reliability
	CSharedObject(_Type*);					
	CSharedObject& operator=(_Type*);
	CSharedObject(const _Type*);
	CSharedObject& operator=(const _Type*);
};//template<> class CSharedObject

/*
	 simple auto ptr class that supports containers 
*/

/**\brief same as \ref _ptr but you can put this auto ptr to containers 
          because this class don`t overload operator & ().
\tparam _Type -- contained pointer type
*/
template<typename _Type>
struct CAutoPtr 
{
	/**\brief Constructor
	\param _ptrdta -- pointer to initialize
	*/
	CAutoPtr(_Type* _ptrdta):m_ptr(_ptrdta)
	{
	}

	/**\brief Destructor.
	*/
	~CAutoPtr()
	{
		free();
	}

	/**\brief frees inner pointer
	*/
	void free()
	{
		if(NOT_NULL(m_ptr)) delete trace_free(m_ptr);
		m_ptr = NULL;
	}

	/**\brief function to return contained pointer
	\return contained pointer
	*/
	_Type* get() {return m_ptr;}

	/**\brief function to return constant contained pointer
	\return constant contained pointer
	*/
	const _Type* get() const {return m_ptr;}

	/**\brief operator to return contained pointer
	\return contained pointer
	*/
	operator _Type* () {return get();}

	/**\brief operator to return contained pointer
	\return contained pointer
	*/
	_Type* operator -> () {return get();}

	/**\brief operator to return contained pointer
	\return contained pointer
	*/
	const _Type* operator -> () const {return get();}

	/**\brief operator to return contained pointer
	\return contained pointer
	*/
	operator const _Type* () const {return get();}
	
	/**\brief verify valid state
	\return true if inner pointer is not NULL
	*/

	operator bool () const {return NOT_NULL(m_ptr);}

	/**\brief verify valid state
	\return true if inner pointer is NULL
	*/
	bool operator ! () const {return !operator bool();}

	/**\brief compare operator
	\param _null -- param to compare with (_null should be 0)
	\return true if inner pointer is null
	*/
	bool operator == (int _null) const 
	{
		VERIFY_EXIT1(_null==NULL,false);
		return operator bool();
	}

	/**\brief compare operator 
	\param _ptrdta -- pointer to compare with
	\return true if inner pointer and param are equal
	*/
	bool operator == (const _Type* _ptrdta) const {return m_ptr==_ptrdta;}

	/**\brief assign operator
	\param _ -- auto pointer to assign 
	\return this reference
	*/
	CAutoPtr<_Type>& operator = (const CAutoPtr<_Type>& _)
	{
		Attach(const_cast<CAutoPtr<_Type>&>(_).Detach());
		return *this;
	}

	/**\brief set pointer values
	\param _dta -- data 
	\param _sz -- size
	*/
	void set(const _Type& _dta,size_t _sz)
	{
		VERIFY_EXIT(NOT_NULL(m_ptr));
		size_t i = 0;
		for(i=0;i<_sz;i++) m_ptr[i] = _dta;
	}

	void Attach(_Type* _ptrdta)
	{
		if(_ptrdta==m_ptr) return;
		free();
		m_ptr = _ptrdta;
	}

	_Type* Detach()
	{
		_Type* ptr = m_ptr;
		m_ptr = NULL;
		return ptr;
	}
protected:
	_Type* m_ptr;		///< inner object pointer
private:
	operator int ();
	operator long ();
};//template<> struct CAutoPtr

template<typename _Type>
struct PtrAsObject
{
	PtrAsObject(const _Type* _ptrdta = NULL)
		:m_ptr(_ptrdta)
	{
	}

	PtrAsObject(const _Type& _data)
		:m_ptr(&_data)
	{
	}

	bool is_valid() const {return NOT_NULL(m_ptr);}

	bool operator == (const PtrAsObject<_Type>& _) const
	{
		if(!is_valid() || !_.is_valid()) return false;

		return *m_ptr == *(_.m_ptr);
	}

	bool operator < (const PtrAsObject<_Type>& _) const
	{
		if(!is_valid() || !_.is_valid()) return false;

		return *m_ptr < *(_.m_ptr);
	}

	const _Type* get() const {return m_ptr;}
	const _Type* get() {return m_ptr;}

protected:
	const _Type* m_ptr;
};//template<> struct PtrAsObject

//@}

/**\page Page_QuickStart_AutoPtrs Quick start: "Using auto pointers".

This auto pointer classes used in utilities library. You can use any other 
classes. 

Most specific classes is \ref CPtrShared and \ref CSharedObject. Let see usage of 
this classes. 

To use \ref CPtrShared class you need:
	-# Derive you class from \ref CRefcounted
	-# Declare auto ptr variable \ref CPtrShared (and may be at same time initialize it)
	-# Use auto ptr variable as a pointer to you class. 
	-# release you variable or just wait for release when variable exit a scope.


Let see code snippet usage of \ref CPtrShared.
\code

struct CA : public CRefcounted	//[1] -- derive you class from CRefcounted
{
	CString_ m_data;
}

void some_func()
{
	CPtrShared<CA> pCA = new CA;	//[2] declare auto ptr variable
	pCA->m_data = _T("string");		//[3] use this variable as pointer to you class

	CPtrShared<CA> pCA1(pCA);
	pCA = NULL;						// [4] release variable

	std::cout << (LPCTSTR)pCA1->m_data;	//[3] ...
};

\endcode

For some cases you can`t derive class from \ref CRefcounted than you can use
class \ref CSharedObject. This class create contained object with according 
reference counter and share memory block that include both: object and reference 
count variable. So now you can`t create original _Type pointers (you should use 
or \ref CSharedObjectData to create pointer or use inner object creation 
mechanism). Mostly and for this case it is easy to use this class. 

To use \ref CSharedObject you should
	-# create variable of CSharedObject parametrized with you type.
	-# and use this variable as a pointer to you type

Let see snipplet of \ref CSharedObject usage.
\code
struct CB
{
	CString_ m_str;
	long m_n;
	//...
};

void some_func()
{
	CSharedObject<CB> pb;		//[1] 
								// here pointer was created with use of default constructor
	
	pb->m_n = 10;				//[2]
	pb->m_str = _T("string");

	CSharedObject<CB> pb1(pb);						//[1]
	std::cout << _T("m_n = ") << pb1->m_n			//[2]
		<< _T("m_str = ") << (LPCTSTR)pb1->m_str	//[2]
		;
}
\endcode
*/


#endif //#if !defined(__AUTOPTR_H__87476E7F_C04B_4FCE_9400_8D94AF36E589_INCLUDED)