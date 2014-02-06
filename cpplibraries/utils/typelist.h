#if !defined(__TYPELIST_H__839A51F0_7912_4B47_91FB_2A799668A84D)
#define __TYPELIST_H__839A51F0_7912_4B47_91FB_2A799668A84D

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "config.h"

#include "nulltype.h"

/**\defgroup UtilitiesTypesList Types lists in utilities
This header brings you litle type list support. It defined 
macroses to easy build a types list types.
*/

//@{

/**\brief class to define types list
\tparam _Item -- item type (that starts this list of types)
\tparam _Tail -- tail type (that follows the start item, it may be another TypeList type of NullType)
*/
template<typename _Item,typename _Tail>
struct TypeList
{
	typedef _Item Item;
	typedef _Tail Tail;
};

/**\brief macro that define tyoe list from one type
\param T1 -- first type
*/
#define TYPELIST_1(T1) TypeList<T1,NullType>

/**\brief macro that define tyoe list from two types
\param T1 -- first type
\param T2 -- second type
*/
#define TYPELIST_2(T1,T2) TypeList<T1,TYPELIST_1(T2) >

/**\brief macro that define tyoe list from three types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
*/
#define TYPELIST_3(T1,T2,T3) TypeList<T1, TYPELIST_2(T2,T3) >

/**\brief macro that define tyoe list from four types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
*/
#define TYPELIST_4(T1,T2,T3,T4) TypeList<T1, TYPELIST_3(T2,T3,T4) >

/**\brief macro that define tyoe list from five types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
*/
#define TYPELIST_5(T1,T2,T3,T4,T5) TypeList<T1, TYPELIST_4(T2,T3,T4,T5) >

/**\brief macro that define tyoe list from six types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
*/
#define TYPELIST_6(T1,T2,T3,T4,T5,T6) TypeList<T1, TYPELIST_5(T2,T3,T4,T5,T6) >

/**\brief macro that define tyoe list from seven types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
*/
#define TYPELIST_7(T1,T2,T3,T4,T5,T6,T7) TypeList<T1, TYPELIST_6(T2,T3,T4,T5,T6,T7) >

/**\brief macro that define tyoe list from eight types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
*/
#define TYPELIST_8(T1,T2,T3,T4,T5,T6,T7,T8) TypeList<T1, TYPELIST_7(T2,T3,T4,T5,T6,T7,T8) >

/**\brief macro that define tyoe list from nine types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
*/
#define TYPELIST_9(T1,T2,T3,T4,T5,T6,T7,T8,T9) TypeList<T1, TYPELIST_8(T2,T3,T4,T5,T6,T7,T8,T9) >

/**\brief macro that define tyoe list from ten types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
*/
#define TYPELIST_10(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10) TypeList<T1, TYPELIST_9(T2,T3,T4,T5,T6,T7,T8,T9,T10) >

/**\brief macro that define tyoe list from eleven types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
\param T11 -- eleventh type
*/
#define TYPELIST_11(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11) TypeList<T1, TYPELIST_10(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11) >

/**\brief macro that define tyoe list from twelve types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
\param T11 -- eleventh type
\param T12 -- twelfth type
*/
#define TYPELIST_12(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12) TypeList<T1, TYPELIST_11(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12) >

/**\brief macro that define tyoe list from thirteen types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
\param T11 -- eleventh type
\param T12 -- twelfth type
\param T13 -- thirteenth type
*/
#define TYPELIST_13(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13) TypeList<T1, TYPELIST_12(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13) >

/**\brief macro that define tyoe list from fourteen types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
\param T11 -- eleventh type
\param T12 -- twelfth type
\param T13 -- thirteenth type
\param T14 -- fourteenth type
*/
#define TYPELIST_14(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14) TypeList<T1, TYPELIST_13(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14) >

/**\brief macro that define tyoe list from fifteen types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
\param T11 -- eleventh type
\param T12 -- twelfth type
\param T13 -- thirteenth type
\param T14 -- fourteenth type
\param T15 -- fifteenth type
*/
#define TYPELIST_15(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15) TypeList<T1, TYPELIST_14(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15) >

/**\brief macro that define tyoe list from sixteen types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
\param T11 -- eleventh type
\param T12 -- twelfth type
\param T13 -- thirteenth type
\param T14 -- fourteenth type
\param T15 -- fifteenth type
\param T16 -- sixteenth type
*/
#define TYPELIST_16(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16) TypeList<T1, TYPELIST_15(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16) >

/**\brief macro that define tyoe list from seventeen types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
\param T11 -- eleventh type
\param T12 -- twelfth type
\param T13 -- thirteenth type
\param T14 -- fourteenth type
\param T15 -- fifteenth type
\param T16 -- sixteenth type
\param T17 -- seventeenth type
*/
#define TYPELIST_17(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17) TypeList<T1, TYPELIST_16(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17) >

/**\brief macro that define tyoe list from eighteen types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
\param T11 -- eleventh type
\param T12 -- twelfth type
\param T13 -- thirteenth type
\param T14 -- fourteenth type
\param T15 -- fifteenth type
\param T16 -- sixteenth type
\param T17 -- seventeenth type
\param T18 -- eighteenth type
*/
#define TYPELIST_18(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18) TypeList<T1, TYPELIST_17(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18) >

/**\brief macro that define tyoe list from nineteen types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
\param T11 -- eleventh type
\param T12 -- twelfth type
\param T13 -- thirteenth type
\param T14 -- fourteenth type
\param T15 -- fifteenth type
\param T16 -- sixteenth type
\param T17 -- seventeenth type
\param T18 -- eighteenth type
\param T19 -- nineteenth type
*/
#define TYPELIST_19(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19) TypeList<T1, TYPELIST_18(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19) >

/**\brief macro that define tyoe list from twenty types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
\param T11 -- eleventh type
\param T12 -- twelfth type
\param T13 -- thirteenth type
\param T14 -- fourteenth type
\param T15 -- fifteenth type
\param T16 -- sixteenth type
\param T17 -- seventeenth type
\param T18 -- eighteenth type
\param T19 -- nineteenth type
\param T20 -- twentieth type
*/
#define TYPELIST_20(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20) TypeList<T1, TYPELIST_19(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20) >

/**\brief macro that define tyoe list from twenty one types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
\param T11 -- eleventh type
\param T12 -- twelfth type
\param T13 -- thirteenth type
\param T14 -- fourteenth type
\param T15 -- fifteenth type
\param T16 -- sixteenth type
\param T17 -- seventeenth type
\param T18 -- eighteenth type
\param T19 -- nineteenth type
\param T20 -- twentieth type
\param T21 -- 21st type
*/
#define TYPELIST_21(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21) TypeList<T1, TYPELIST_20(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21) >

/**\brief macro that define tyoe list from twenty two types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
\param T11 -- eleventh type
\param T12 -- twelfth type
\param T13 -- thirteenth type
\param T14 -- fourteenth type
\param T15 -- fifteenth type
\param T16 -- sixteenth type
\param T17 -- seventeenth type
\param T18 -- eighteenth type
\param T19 -- nineteenth type
\param T20 -- twentieth type
\param T21 -- 21st type
\param T22 -- 22nd type
*/
#define TYPELIST_22(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22) TypeList<T1, TYPELIST_21(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22) >

/**\brief macro that define tyoe list from twenty three types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
\param T11 -- eleventh type
\param T12 -- twelfth type
\param T13 -- thirteenth type
\param T14 -- fourteenth type
\param T15 -- fifteenth type
\param T16 -- sixteenth type
\param T17 -- seventeenth type
\param T18 -- eighteenth type
\param T19 -- nineteenth type
\param T20 -- twentieth type
\param T21 -- 21st type
\param T22 -- 22nd type
\param T23 -- 23rd type
*/
#define TYPELIST_23(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23) TypeList<T1, TYPELIST_22(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23) >

/**\brief macro that define tyoe list from twenty four types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
\param T11 -- eleventh type
\param T12 -- twelfth type
\param T13 -- thirteenth type
\param T14 -- fourteenth type
\param T15 -- fifteenth type
\param T16 -- sixteenth type
\param T17 -- seventeenth type
\param T18 -- eighteenth type
\param T19 -- nineteenth type
\param T20 -- twentieth type
\param T21 -- 21st type
\param T22 -- 22nd type
\param T23 -- 23rd type
\param T24 -- twenty fourth type
*/
#define TYPELIST_24(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23,T24) TypeList<T1, TYPELIST_23(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23,T24) >

/**\brief macro that define type list from twenty five types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
\param T11 -- eleventh type
\param T12 -- twelfth type
\param T13 -- thirteenth type
\param T14 -- fourteenth type
\param T15 -- fifteenth type
\param T16 -- sixteenth type
\param T17 -- seventeenth type
\param T18 -- eighteenth type
\param T19 -- nineteenth type
\param T20 -- twentieth type
\param T21 -- 21st type
\param T22 -- 22nd type
\param T23 -- 23rd type
\param T24 -- twenty fourth type
\param T25 -- twenty fifth type
*/
#define TYPELIST_25(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23,T24,T25) TypeList<T1, TYPELIST_24(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23,T24,T25) >

/**\brief macro that define type list from twenty six types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
\param T11 -- eleventh type
\param T12 -- twelfth type
\param T13 -- thirteenth type
\param T14 -- fourteenth type
\param T15 -- fifteenth type
\param T16 -- sixteenth type
\param T17 -- seventeenth type
\param T18 -- eighteenth type
\param T19 -- nineteenth type
\param T20 -- twentieth type
\param T21 -- 21st type
\param T22 -- 22nd type
\param T23 -- 23rd type
\param T24 -- twenty fourth type
\param T25 -- twenty fifth type
\param T26 -- twenty sixth type
*/
#define TYPELIST_26(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23,T24,T25,T26) TypeList<T1, TYPELIST_25(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23,T24,T25,T26) >

/**\brief macro that define type list from twenty seven types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
\param T11 -- eleventh type
\param T12 -- twelfth type
\param T13 -- thirteenth type
\param T14 -- fourteenth type
\param T15 -- fifteenth type
\param T16 -- sixteenth type
\param T17 -- seventeenth type
\param T18 -- eighteenth type
\param T19 -- nineteenth type
\param T20 -- twentieth type
\param T21 -- 21st type
\param T22 -- 22nd type
\param T23 -- 23rd type
\param T24 -- twenty fourth type
\param T25 -- twenty fifth type
\param T26 -- twenty sixth type
\param T27 -- twenty seventh type
*/
#define TYPELIST_27(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23,T24,T25,T26,T27) TypeList<T1, TYPELIST_26(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23,T24,T25,T26,T27) >

/**\brief macro that define type list from twenty eight types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
\param T11 -- eleventh type
\param T12 -- twelfth type
\param T13 -- thirteenth type
\param T14 -- fourteenth type
\param T15 -- fifteenth type
\param T16 -- sixteenth type
\param T17 -- seventeenth type
\param T18 -- eighteenth type
\param T19 -- nineteenth type
\param T20 -- twentieth type
\param T21 -- 21st type
\param T22 -- 22nd type
\param T23 -- 23rd type
\param T24 -- twenty fourth type
\param T25 -- twenty fifth type
\param T26 -- twenty sixth type
\param T27 -- twenty seventh type
\param T28 -- twenty eighth type
*/
#define TYPELIST_28(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23,T24,T25,T26,T27,T28) TypeList<T1, TYPELIST_27(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23,T24,T25,T26,T27,T28) >

/**\brief macro that define type list from twenty nine types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
\param T11 -- eleventh type
\param T12 -- twelfth type
\param T13 -- thirteenth type
\param T14 -- fourteenth type
\param T15 -- fifteenth type
\param T16 -- sixteenth type
\param T17 -- seventeenth type
\param T18 -- eighteenth type
\param T19 -- nineteenth type
\param T20 -- twentieth type
\param T21 -- 21st type
\param T22 -- 22nd type
\param T23 -- 23rd type
\param T24 -- twenty fourth type
\param T25 -- twenty fifth type
\param T26 -- twenty sixth type
\param T27 -- twenty seventh type
\param T28 -- twenty eighth type
\param T29 -- twenty ninth type
*/
#define TYPELIST_29(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23,T24,T25,T26,T27,T28,T29) TypeList<T1, TYPELIST_28(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23,T24,T25,T26,T27,T28,T29) >

/**\brief macro that define type list from thirty types
\param T1 -- first type
\param T2 -- second type
\param T3 -- third type
\param T4 -- forth type
\param T5 -- fifth type
\param T6 -- sixth type
\param T7 -- seventh type
\param T8 -- eighth type
\param T9 -- ninth type
\param T10 -- tenth type
\param T11 -- eleventh type
\param T12 -- twelfth type
\param T13 -- thirteenth type
\param T14 -- fourteenth type
\param T15 -- fifteenth type
\param T16 -- sixteenth type
\param T17 -- seventeenth type
\param T18 -- eighteenth type
\param T19 -- nineteenth type
\param T20 -- twentieth type
\param T21 -- 21st type
\param T22 -- 22nd type
\param T23 -- 23rd type
\param T24 -- twenty fourth type
\param T25 -- twenty fifth type
\param T26 -- twenty sixth type
\param T27 -- twenty seventh type
\param T28 -- twenty eighth type
\param T29 -- twenty ninth type
\param T30 -- thirtieth type
*/
#define TYPELIST_30(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23,T24,T25,T26,T27,T28,T29,T30) TypeList<T1, TYPELIST_29(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23,T24,T25,T26,T27,T28,T29,T30) >

//@}


#endif //#if !defined(__TYPELIST_H__839A51F0_7912_4B47_91FB_2A799668A84D)
