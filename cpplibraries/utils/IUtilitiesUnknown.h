#if !defined(__IUTILITIESUNKNOWN_H__AA08BA5D_2A6E_415E_8E97_28500598DE55_INCLUDED)
#define __IUTILITIESUNKNOWN_H__AA08BA5D_2A6E_415E_8E97_28500598DE55_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "config.h"
#include "atlmfc.h"

#include "nulltype.h"
#include "utils.h"

/**\defgroup UtilitiesUnknown Utilities to support interface getters. 
Classes and fucntions of this group helps to create independent modules.
*/
//@{


/**\brief interface to search/get an specified interface.
Mostly you should not use this interface directly. 
Use \ref InterfacePtr to create interface you need from this.
*/
interface IUtilitiesUnknown
{
	/**\brief function to retry interface 
	\param[in] _szInterfaceName -- interface name to get
	\param[out] _pinterface -- returned interface 
	*/
	virtual bool get_interface(LPCTSTR _szInterfaceName,LPVOID& _pinterface) = 0;
};

/**\brief interface traits. inner class to suppport interface lookup for list of interfaces. 
This traits return name of some interface.
If you got the error LINK2001: "unresolved external symbol " public: static char const * __cdecl IUtilitiesInterfaceTraits<struct _SomeType>::get_name(void)"
declare interface name with macro DECLARE_INTERFACE_NAME(_SomeInterfaceType)
\tparam _InterfaceType 
*/
template<typename _InterfaceType>
struct IUtilitiesInterfaceTraits
{
	/**\brief static function to return Interface name
	\return name of interface
	*/
	static LPCTSTR get_name();		
};

/**\brief macro that declare interface name.
\param _InterfaceType -- interface type to declare interface name for.
*/
#define DECLARE_INTERFACE_NAME(_InterfaceType) \
template<>	\
struct IUtilitiesInterfaceTraits<_InterfaceType>	\
{	\
	static LPCTSTR get_name() {return _T( #_InterfaceType );}	\
};

/**\brief macro to retry interface name for some interface type
\param _InterfaceType -- interface type to get name for
*/
#define GET_INTERFACE_NAME(_InterfaceType) (IUtilitiesInterfaceTraits<_InterfaceType>::get_name())
// this is the wrong way 

/**\brief inner class to implement interface lookup for interface typelist
You should not use this class directly. Use \ref InterfacePtr to create interface you need from this.
*/
template<typename _TypeList>
struct IUtilitiesGetInterface
{
	typedef typename _TypeList::Item CurrentItem;
	typedef typename _TypeList::Tail Tail;

	/**\brief function that do static_cast and return interface that base of _Struct type if 
	          this class enter in _TypeList and _Struct derived from interface with name _szInterfaceName.
	\tparam _Struct -- type to get interface with static_cast from.
	\param[in] _szInterfaceName -- name of interface to get
	\param[out] _pinterface -- returned interface if interface was founded.
	\param[in] _psource -- type to get interface with static_cast from.
	\return true if interface was founded successfully.
	*/
	template<typename _Struct>
		static bool get(LPCTSTR _szInterfaceName,LPVOID& _pinterface,_Struct* _psource)
	{
		VERIFY_EXIT1(NOT_NULL(_szInterfaceName),false); // you should use DECLARE_INTERFACE_NAME() macro for interface
		if(!lstrcmp(GET_INTERFACE_NAME(CurrentItem),_szInterfaceName))
		{
			_pinterface = reinterpret_cast<LPVOID>(static_cast<CurrentItem*>(_psource));
			return true;
		}
		return IUtilitiesGetInterface<Tail>::get(_szInterfaceName,_pinterface,_psource);
	}
};//template<> struct IUtilitiesGetInterface

/**\brief default implementation of \ref IUtilitiesGetInterface.
This class implement case when interface was not founded.
*/
template<>
struct IUtilitiesGetInterface<NullType>
{
	template<typename _Struct>
		static bool get(LPCTSTR _szInterfaceName,LPVOID& _pinterface,_Struct* _psource)
	{
		_pinterface = NULL;
		return false;
	}
};//template<> struct IUtilitiesGetInterface<NullType>

/**\brief simple function to get specified (by it thype) interface from \ref IUtilitiesUnknown
\tparam _InterfaceType -- type of interface to get from utilities unknown
\param _punknown -- utilities unknown interface to get interface from
\return poiter to interface or NULL if interface of specified type was not founded.
*/
template<typename _InterfaceType>
_InterfaceType* get_interface(IUtilitiesUnknown* _punknown)
{
	LPVOID ptr = NULL;
	if(!_punknown->get_interface(GET_INTERFACE_NAME(_InterfaceType),ptr)) return NULL;
	return reinterpret_cast<_InterfaceType*>(ptr);
}

/**\brief simple function to get specified (by it thype) interface from \ref IUtilitiesUnknown
\tparam _InterfaceType -- type of interface to get from utilities unknown
\param _punknown -- utilities unknown interface to get interface from
\return constant poiter to interface or NULL if interface of specified type was not founded.
*/
template<typename _InterfaceType>
const _InterfaceType* get_interface(const IUtilitiesUnknown* _punknown)
{
	LPVOID ptr = NULL;
	if(!_punknown->get_interface(GET_INTERFACE_NAME(_InterfaceType),ptr)) return NULL;
	return reinterpret_cast<const _InterfaceType*>(ptr);
}

/**\brief \ref IUtilitiesUnknown implementation for you types. 
You should simple derive you class from \ref IUtilitiesUnknownImpl to implement 
IUtilitiesUnknown semantic. You should specify type list of all interfaces that
your supports in you class.
\tparam _Struct -- you struct type
\tparam _TypeList -- type list of all interfaces that you support in you class.
\sa TypeList
\sa TYPELIST_1(),TYPELIST_2(),TYPELIST_3(),TYPELIST_4()
\sa TYPELIST_5(),TYPELIST_6(),TYPELIST_7(),TYPELIST_8()
\sa TYPELIST_9(),TYPELIST_10() ...
*/
template<typename _Struct,typename _TypeList>
struct IUtilitiesUnknownImpl : public IUtilitiesUnknown
{
	/**\brief interface \ref IUtilitiesUnknown function \ref IUtilitiesUnknown::get_interface()
	          implementation. This fucntion return pointer to interface for specified interface
			  name.
	\param[in] _szInterfaceName -- interface name to search
	\param[out] _pinterface -- returned interface or NULL
	\return true if interface named _szInterfaceName was successfully founded.
	*/
	virtual bool get_interface(LPCTSTR _szInterfaceName,LPVOID& _pinterface)
	{
		return IUtilitiesGetInterface<_TypeList>::get(
			_szInterfaceName
			,_pinterface
			,static_cast<_Struct*>(this)
			);
	}

	/**\brief helpull function to get interface 
	\tparam _InterfaceType -- interface type to get
	\return pointer to _InterfaceType or NULL
	*/
	template<typename _InterfaceType>
		_InterfaceType* get(_InterfaceType)
	{
		return ::get_interface<_InterfaceType>(this);
	}

	/**\brief helpull function to get interface 
	\tparam _InterfaceType -- interface type to get
	\return constant pointer to _InterfaceType or NULL
	*/
	template<typename _InterfaceType>
		const _InterfaceType* get(_InterfaceType) const
	{
		return ::get_interface<_InterfaceType>(this);
	}
};

/**\brief class to support interfaces from \ref IUtilitiesUnknown
\tparam _InterfaceType -- interface type to get from \ref IUtilitiesUnknown
*/
template<typename _InterfaceType>
struct InterfacePtr
{
	/**\brief constructor. build this interface pointer wrapper for type _InterfaceType
	          and pointer to \ref IUtilitiesUnknown.
	\param _punknown -- utilities unknown interface
	*/
	InterfacePtr(IUtilitiesUnknown* _punknown)
		:m_pinterface(NULL)
	{
		m_pinterface = ::get_interface<_InterfaceType>(_punknown);
	}

	InterfacePtr(IUtilitiesUnknown& _unknown)
		:m_pinterface(NULL)
	{
		m_pinterface = ::get_interface<_InterfaceType>(&_unknown);
	}

	/**\brief return pointer to interface
	\return pointer to interface
	*/
	_InterfaceType* get()
	{
		VERIFY_EXIT1(NOT_NULL(m_pinterface),NULL);
		return m_pinterface;
	}

	/**\brief return pointer to interface
	\return constant pointer to interface
	*/
	const _InterfaceType* get() const
	{
		VERIFY_EXIT1(NOT_NULL(m_pinterface),NULL);
		return m_pinterface;
	}

	/**\brief return pointer to interface
	\return pointer to interface
	*/
	operator _InterfaceType*() { return get();}

	/**\brief return pointer to interface
	\return constant pointer to interface
	*/
	operator const _InterfaceType*() const { return get();}

	/**\brief return pointer to interface
	\return pointer to interface
	*/
	_InterfaceType* operator -> () {return get();}

	/**\brief return pointer to interface
	\return constant pointer to interface
	*/
	const _InterfaceType* operator -> () const {return get();}

	/**\brief return true if interface was successfully got from \ref IUtilitiesUnknown
	          If this class is valid.
	\return true if interface was successfully got from \ref IUtilitiesUnknown
	*/
	bool is_valid() const {return NOT_NULL(m_pinterface);}

	/**\brief return true if interface was successfully got from \ref IUtilitiesUnknown
	          If this class is valid.
	\return true if interface was successfully got from \ref IUtilitiesUnknown
	*/
	operator bool () const {return is_valid;}

	/**\brief return true if interface was not successfully got from \ref IUtilitiesUnknown
	          If this class is not valid.
	\return true if interface was not successfully got from \ref IUtilitiesUnknown
	*/
	bool operator ! () const {return !is_valid();}

	/**\brief operator to compare this class with NULL pointer
	\return true if this clas is invalid \sa InterfacePtr::is_valid()
	*/
	bool operator == (long _null) const
	{
		VERIFY_EXIT1(_null==0,false);
		return operator ! ();
	}

	/**\brief operator to compare this class with NULL pointer
	\return true if this clas is not invalid \sa InterfacePtr::is_valid()
	*/
	bool operator != (long _null) const
	{
		VERIFY_EXIT1(_null==0,false);
		return operator bool ();
	}

protected:
	operator int ();
	operator long();
	operator ULONG ();
	operator UINT();

	_InterfaceType* m_pinterface;		///< pointer to interface for this class.
};// template<> struct InterfacePtr

//@}

/**\example IUtilitiesUnknown_example.cpp
*/

/**\page Page_QuickStart_IUtilitiesUnknown Quick start: "Use of utilities unknown interface"

first let see how to create you custom class as implementation for some interfaces and
for \ref IUtilitiesUnknown to get impelmented interfaces. See next code snippet.
\code

// declare interface IA
interface IA
{
	virtual void a() = 0;
};

DECLARE_INTERFACE_NAME(IA);

// declare interface IB
interface IB
{
	virtual void b() = 0;
};

DECLARE_INTERFACE_NAME(IB);

// declare interface IC
interface IC
{
	virtual void c() = 0;
};

DECLARE_INTERFACE_NAME(IC);

// typedefs list of supported interfaces for you class implementation
typedef TYPELIST_2(IA,IB) ABTypesLst;

// you class that implement IUtilitiesUnknown , IA, and IB
struct CMyImplementOfAB
:
	public IA		// derive from IA
	,public IB		// derive from IB
	,public IUtilitiesUnknownImpl<CMyImplementOfAB,ABTypesLst>	
		// create IUtilitiesUnknown implementation for list of types ABTypesLst
		// and for class CMyImplementOfAB that implement this interfaces
{
	// implement void IA::a() 
	virtual void a()
	{
		std::cout << _T("call to a()") << std::endl;
	}

	//implement void IB::b() 
	virtual void b()
	{
		std::cout << _T("call to b()") << std::endl;
	}
};
\endcode

As your can see you need minimal to support \ref IUtilitiesUnknown. 
Now let see snippet of the usage of this class.

\code

void test_interfaces(IUtilitiesUnknown* _punknown)
{
	InterfacePtr<IA> pa(_punknown);
	if(pa.is_valid()) pa->a();
	else std::cout << _T("failed to get IA") << std::endl;

	InterfacePtr<IB> pb(_punknown);
	if(pb.is_valid()) pb->b();
	else std::cout << _T("failed to get IB") << std::endl;

	InterfacePtr<IC> pc(_punknown);
	if(pc.is_valid()) pc->c();
	else std::cout << _T("failed to get IC") << std::endl;
}

void test()
{
	CMyImplementOfAB dta;	
	test_interfaces(&dta);
}

//after test() your will get next output:
//call to a()
//call to b()
//failed to get IC

\endcode

As you can see from this example when you will need to implement 
some changes you still no need to change interface of function 
void test_interfaces(IUtilitiesUnknown* _punknown). That why this 
classes can help minimize changes of code when you will need 
some extentions for application.
*/

#endif//#if !defined(__IUTILITIESUNKNOWN_H__AA08BA5D_2A6E_415E_8E97_28500598DE55_INCLUDED)