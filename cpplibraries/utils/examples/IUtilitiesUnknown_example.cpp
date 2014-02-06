#include "..\IUtilitiesUnknown.h"


// declare interface IA
interface IA
{
	virtual void a() = 0;
};

// declare interface IB
interface IB
{
	virtual void b() = 0;
};

// declare interface IC
interface IC
{
	virtual void c() = 0;
};

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
