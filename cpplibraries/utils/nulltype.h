#if !defined(__NULLTYPE_H__F217E437_4635_40AF_B248_CBB1034F88CB_INCLUDED)
#define __NULLTYPE_H__F217E437_4635_40AF_B248_CBB1034F88CB_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "config.h"

/**\brief type that differ from all other types.
This type is used to sets end of type list, to pass Null type in metaprogramming
for some type proccessors.
*/
struct NullType{};

struct TrueType{};
struct FalseType{};

template<bool _cond>
struct Bool2Type
{
	typedef TrueType Type;
};

template<>
struct Bool2Type<false>
{
	typedef FalseType Type;
};

#endif//#if !defined(__NULLTYPE_H__F217E437_4635_40AF_B248_CBB1034F88CB_INCLUDED)
