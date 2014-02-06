#if !defined(__WINAPIEXT_H__0EC9D168_86C2_4791_954A_7A656482B90A_INCLUDED)
#define __WINAPIEXT_H__0EC9D168_86C2_4791_954A_7A656482B90A_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "config.h"
#include "atlmfc.h"

#include <Richedit.h>

#include "utils.h"
#include "strconv.h"

/**\defgroup UtilitiesWinAPIExtentions Utilities WinAPI extentions
This group define some WinAPI extention to easy use of 
standart WinAPI.
*/
//@{

/**\brief struct that extend WinAPI PARAFORMAT structure 
*/

enum ParaNumberingEn{
	ParaNumbering_Null = 0
	,ParaNumbering_Bullet = PFN_BULLET
	,ParaNumbering_ArabicNumbers = 2
	,ParaNumbering_LowCaseLetters = 3
	,ParaNumbering_UpperCaseLetters = 4
	,ParaNumbering_LowCaseRomanNumerals = 5
	,ParaNumbering_UpperCaseRomanNumerals = 6
	,ParaNumbering_UnicodeCharacters = 7
};	

enum ParaShadingStyleEn{
	ParaShadingStyle_None = 0
	,ParaShadingStyle_DarkHorizontal = 1
	,ParaShadingStyle_DarkVertical = 2
	,ParaShadingStyle_DarkDownDiagonal = 3
	,ParaShadingStyle_DarkUpDiagonal = 4
	,ParaShadingStyle_DarkGrid = 5
	,ParaShadingStyle_DarkTrellis = 6
	,ParaShadingStyle_LightHorizontal = 7
	,ParaShadingStyle_LightVertical = 8
	,ParaShadingStyle_LightDownDiagonal = 9
	,ParaShadingStyle_LightUpDiagonal = 10
	,ParaShadingStyle_LightGrid = 11
	,ParaShadingStyle_LightTrellis = 12
};

enum ParaShardingColorEn {
	ParaShardingColor_Black = 0
	,ParaShardingColor_Blue = 1
	,ParaShardingColor_Cyan = 2
	,ParaShardingColor_Green = 3
	,ParaShardingColor_Magenta = 4
	,ParaShardingColor_Red = 5
	,ParaShardingColor_Yellow = 6
	,ParaShardingColor_White = 7
	,ParaShardingColor_DarkBlue = 8
	,ParaShardingColor_DarkCyan = 9
	,ParaShardingColor_DarkGreen = 10
	,ParaShardingColor_DarkMagenta = 11
	,ParaShardingColor_DarkRed = 12
	,ParaShardingColor_DarkYellow = 13
	,ParaShardingColor_DarkGray = 14
	,ParaShardingColor_LightGray = 15
};

template<typename _Type>
struct ParaFormatOpsImpl
{
	void set_numbering(WORD _wNumbering)
	{
		static_cast<_Type&>(*this).dwMask |= PFM_NUMBERING;
		static_cast<_Type&>(*this).wNumbering = _wNumbering;
	}

	void set_numbering(ParaNumberingEn _numbering)
	{
		static_cast<_Type&>(*this).dwMask |= PFM_NUMBERING;
		static_cast<_Type&>(*this).wNumbering = _numbering;
	}

	void set_bulleting()
	{
		static_cast<_Type&>(*this).dwMask |= PFM_NUMBERING;
		static_cast<_Type&>(*this).wNumbering = PFN_BULLET;
	}

	void set_effects(WORD _wEffects)
	{
		static_cast<_Type&>(*this).wEffects = _wEffects;
	}

	void set_left2right_readorder()
	{
		static_cast<_Type&>(*this).wEffects = 0;
	}

	void set_right2left_readorder()
	{
		static_cast<_Type&>(*this).wEffects = PFE_RTLPARA;
	}

	void set_startindent(LONG _dxStartIndent)
	{
		static_cast<_Type&>(*this).dwMask |= PFM_STARTINDENT;
		static_cast<_Type&>(*this).dxStartIndent = _dxStartIndent;
	}

	void set_rightindent(LONG _dxRightIndent)
	{
		static_cast<_Type&>(*this).dwMask |= PFM_RIGHTINDENT;
		static_cast<_Type&>(*this).dxRightIndent = _dxRightIndent;
	}

	void set_offset(LONG _dxOffset)
	{
		static_cast<_Type&>(*this).dwMask |= PFM_OFFSET;
		static_cast<_Type&>(*this).dxOffset = _dxOffset;
	}

	void set_alignment(WORD _wAlignment)
	{
		static_cast<_Type&>(*this).dwMask |= PFM_ALIGNMENT;
		static_cast<_Type&>(*this).wAlignment = _wAlignment;
	}

	void set_left_alignment()
	{
		static_cast<_Type&>(*this).dwMask |= PFM_ALIGNMENT;
		static_cast<_Type&>(*this).wAlignment = PFA_LEFT;
	}

	void set_center_alignment()
	{
		static_cast<_Type&>(*this).dwMask |= PFM_ALIGNMENT;
		static_cast<_Type&>(*this).wAlignment = PFA_CENTER;
	}

	void set_right_alignment()
	{
		static_cast<_Type&>(*this).dwMask |= PFM_ALIGNMENT;
		static_cast<_Type&>(*this).wAlignment = PFA_RIGHT;
	}

	void set_stop_position(SHORT _index,LONG _pos)
	{
		VERIFY_EXIT(_index>=0 && _index+1<MAX_TAB_STOPS);
		static_cast<_Type&>(*this).rgxTabs[_index] = _pos;
		static_cast<_Type&>(*this).cTabCount = max(static_cast<_Type&>(*this).cTabCount,_index+1);
		static_cast<_Type&>(*this).cTabCount = min(static_cast<_Type&>(*this).cTabCount,MAX_TAB_STOPS);
	}
}; // template<> struct ParaFormatOpsImpl

struct CParaFormat 
:
	public PARAFORMAT
	,ParaFormatOpsImpl<CParaFormat>
{
	CParaFormat()
	{
		ZeroObj(*this);
		cbSize = sizeof(PARAFORMAT);
	}

	/**\brief convert PARAFORMAT to string
	\param[out] _str -- result string represantation of PARAFORMAT structure 
	*/
	void to_str(CString_& _str) const
	{
		CString_ s1;
		mask2str(s1);
		_str = CString_(_T("dwMask = [")) + s1 + CString_(_T("] "));
		if(dwMask&PFM_ALIGNMENT)
		{
			align2str(s1);
			_str += CString_(_T("wAlignment: ")) + s1 + CString_(_T(" "));
		}
		if(dwMask&PFM_NUMBERING)
		{
			if(wNumbering==PFN_BULLET) _str += _T("wNumbering :PFN_BULLET ");
		}
		if(dwMask&PFM_OFFSET)
		{
			s1 = format(_T("%d"),dxOffset);
			_str += CString_(_T("dxOffset: ")) + s1 + CString_(_T(" "));
		}
		if(dwMask&PFM_OFFSETINDENT)
		{
			s1 = format(_T("%d"),dxStartIndent);
			_str += CString_(_T("dxStartIndent: ")) + s1 + CString_(_T(" "));
		}
		if(dwMask&PFM_RIGHTINDENT)
		{
			s1 = format(_T("%d"),dxRightIndent);
			_str += CString_(_T("dxRightIndent: ")) + s1 + CString_(_T(" "));
		}
		if(dwMask&PFM_STARTINDENT)
		{
			s1 = format(_T("%d"),dxStartIndent);
			_str += CString_(_T("dxStartIndent: ")) + s1 + CString_(_T(" "));
		}
	}

	/**\brief function that convert align values to strings 
	\param[out] _s -- string reperesentation of PARAFORMAT structure aligns
	*/
	void align2str(CString_& _s) const
	{
		typedef std::pair<DWORD,LPCTSTR> Align2Str;
#define ALIGN2STR(_arg) Align2Str(_arg,_T(#_arg))

		static const Align2Str aligns001[] = 
		{
			ALIGN2STR(PFA_CENTER)
			,ALIGN2STR(PFA_LEFT)
			,ALIGN2STR(PFA_RIGHT)
		};

		long i=0;
		_s.Empty();
		for(i=0;i<sizea(aligns001);i++)
		{
			if(wAlignment==aligns001[i].first)
			{
				_s = aligns001[i].second;
				return;
			}
		}
		_s = format(_T("Unknown (%d)"),wAlignment);
#undef ALIGN2STR
	}

	/**\brief function to convert mask values to string
	\param[out] _s -- string representation of PARAFORMAT structure mask values
	*/
	void mask2str(CString_& _s) const
	{
		typedef std::pair<DWORD,LPCTSTR> Mask2Str;
#define MASK2STR(_arg) Mask2Str(_arg,_T(#_arg))
		
		static const Mask2Str masks001[] = 
		{
			MASK2STR(PFM_ALIGNMENT)
			,MASK2STR(PFM_NUMBERING)
			,MASK2STR(PFM_OFFSET)
			,MASK2STR(PFM_OFFSETINDENT)
			,MASK2STR(PFM_RIGHTINDENT)
			,MASK2STR(PFM_RTLPARA)
			,MASK2STR(PFM_STARTINDENT)
			,MASK2STR(PFM_TABSTOPS)
		};
		long i=0;
		_s.Empty();
		for(i=0;i<sizea(masks001);i++)
		{
			if((dwMask&masks001[i].first)!=0)
			{
				_s += masks001[i].second;
				_s += _T(" ");
			}
		}

#undef MASK2STR
	}
	
};//struct CParaFormat

#if !defined(PFA_FULL_INTERWORD)
#	define PFA_FULL_INTERWORD		4
#endif

/**\brief structure that extend winAPI PARAFORMAT2 structure
*/
struct CParaFormat2 
:
	public PARAFORMAT2
	,ParaFormatOpsImpl<CParaFormat2>
{
	typedef PARAFORMAT2 base;		///< base type typedef 

	/**\brief Constructor
	*/
	CParaFormat2()	
	{
		ZeroObj(*(PARAFORMAT2*)this);
		cbSize = sizeof(PARAFORMAT2);
	}


	void set_justify_alignment()
	{
		dwMask |= PFM_ALIGNMENT;
		wAlignment = PFA_JUSTIFY;
	}

	void set_full_interword_alignment()
	{
		dwMask |= PFM_ALIGNMENT;
		wAlignment = PFA_FULL_INTERWORD;
	}

	void space_before(LONG _twips)
	{
		dwMask |= PFM_SPACEBEFORE;
		dySpaceBefore = _twips;
	}

	void space_after(LONG _twips)
	{
		dwMask |= PFM_SPACEAFTER;
		dySpaceAfter = _twips;
	}

	void set_linespacing(LONG _spacing)
	{
		dwMask |= PFM_LINESPACING;
		dyLineSpacing = _spacing;
	}

	void set_style(SHORT _style)
	{
		dwMask |= PFM_STYLE;
		sStyle = _style;
	}

	void set_spacing_10()
	{
		dwMask |= PFM_LINESPACING;
		bLineSpacingRule = 0;
	}
	
	void set_spacing_15()
	{
		dwMask |= PFM_LINESPACING;
		bLineSpacingRule = 1;
	}

	void set_spacing_20()
	{
		dwMask |= PFM_LINESPACING;
		bLineSpacingRule = 2;
	}

	void set_twips_spacing(LONG _twips)
	{
		dwMask |= PFM_LINESPACING;
		bLineSpacingRule = 3;
		dyLineSpacing = _twips;
	}

	void set_twips_exact_spacing(LONG _twips)
	{
		dwMask |= PFM_LINESPACING;
		bLineSpacingRule = 4;
		dyLineSpacing = _twips;
	}

	void set_20th_spacing(LONG _spacing)
	{
		dwMask |= PFM_LINESPACING;
		bLineSpacingRule = 5;
		dyLineSpacing = _spacing;
	}

	void set_shading_weight(WORD _shadingweight)
	{
		dwMask |= PFM_SHADING;
		wShadingWeight = _shadingweight;
	}

	void set_shading_style(
		ParaShadingStyleEn _style
		,ParaShardingColorEn _foregrounfcolor
		,ParaShardingColorEn _backgroundcolor
		)
	{
		dwMask |= PFM_SHADING;
		wShadingStyle = 
			(_style & 0xf)
			| ((_foregrounfcolor & 0xf) << 4)
			| ((_backgroundcolor & 0xf) << 8)
			;
	}

	void set_numbering_start(WORD _wNumberingStart)
	{
		dwMask |= PFM_NUMBERINGSTART|PFM_NUMBERING;
		wNumbering = ParaNumbering_UnicodeCharacters;
		wNumberingStart = _wNumberingStart;
	}

	void set_numbering_style(WORD _wNumberingStyle)
	{
		dwMask |= PFM_NUMBERINGSTYLE;
		wNumberingStyle = _wNumberingStyle ;
	}

	void set_numbering_tab(WORD _twips)
	{
		dwMask |= PFM_NUMBERINGTAB;
		wNumberingTab = _twips;
	}

};//struct CParaFormat2

enum CharFormatEffectsEn{
	CharFormatEffects_AutoColor = CFE_AUTOCOLOR
	,CharFormatEffects_Bold = CFE_BOLD
	,CharFormatEffects_Disabled = CFE_DISABLED
	,CharFormatEffects_Italic = CFE_ITALIC
	,CharFormatEffects_Strikeout = CFE_STRIKEOUT
	,CharFormatEffects_Underline = CFE_UNDERLINE
	,CharFormatEffects_Protected = CFE_PROTECTED
};

struct CCharFormat2 : public CHARFORMAT2
{
	typedef CHARFORMAT2 base;

	CCharFormat2()
	{
		ZeroObj(*(CHARFORMAT2*)this);
		cbSize = sizeof(CHARFORMAT2);
	}

	CCharFormat2(DWORD _mask)
	{
		ZeroObj(*(CHARFORMAT2*)this);
		cbSize = sizeof(CHARFORMAT2);
		dwMask = _mask;
	}
	
	void set_text_color(COLORREF _txtclr)
	{
		dwMask |= CFM_COLOR;
		crTextColor = _txtclr;
	}

	void clear_text_color()
	{
		dwMask &= ~(CFM_COLOR);
		crTextColor = 0;
	}

	void set_bold(bool _bold = true)
	{
		dwMask |= CFM_BOLD;
		if(_bold) dwEffects |= CFE_BOLD;
		else dwEffects &= ~(CFE_BOLD);
	}
};//struct CCharFormat2



/**\brief clas that extend WinAPI COLORREF type
*/
class CColor 
{
public:
	/**\brief constructor
	*/
	CColor(COLORREF _clr = RGB(0,0,0))
	{
		operator=(_clr);
	}

	/**\brief operator to return COLORREF type
	\return COLORREF value
	*/
	operator COLORREF () const {return m_data;}

	/**\brief assignment operator 
	\param _clr -- value to assign
	\return thsi reference
	*/
	CColor& operator=(COLORREF _clr)
	{
		m_data = _clr;
		return *this;
	}

	/**\brief function that return blue part of COLORREF
	\return blue part of COLORREF
	*/
	byte get_Blue() const {return (byte)(m_data&0xff);}

	/**\brief function that return red part of COLORREF
	\return red part of COLORREF
	*/
	byte get_Red() const {return byte((m_data>>16)&0xff);}

	/**\brief function that return green part of COLORREF
	\return green part of COLORREF
	*/
	byte get_Green() const {return (byte)((m_data>>8)&0xff);}

	/**\brief function to set blue part of COLORREF
	\param[in] _b -- blue value byte
	\return this reference
	*/
	CColor& set_Blue(byte _b) 
	{
		m_data = (m_data&0xffffff00)|_b;
		return *this;
	}

	/**\brief function to set red part of COLORREF
	\param[in] _r -- red value byte
	\return this reference
	*/
	CColor& set_Red(byte _r)
	{
		m_data = (m_data&0xff00ffff)|_r<<16;
		return *this;
	}

	/**\brief function to set green part of COLORREF
	\param[in] _g -- green value byte
	\return this reference
	*/
	CColor& set_Green(byte _g)
	{
		m_data = (m_data&0xffff00ff)|_g<<8;
		return *this;
	}

	/**\brief set value from system color 
	See WinAPI function ::GetSysColor() for more detail.
	\param _index -- system color index 
	\return this reference
	*/
	CColor& set_SysColor(int _index)
	{
		m_data = ::GetSysColor(_index);
		return *this;
	}

	/**\brief function that multiply color by some coefficient
	\param _cf -- coiffecient to multiply
	\return this reference
	*/
	CColor& operator*=(double _cf)
	{
		set_Red(byte(get_Red()*_cf));
		set_Green(byte(get_Green()*_cf));
		set_Blue(byte(get_Blue()*_cf));
		return *this;
	}

	/**\brief function that add to current color value argument color
	\param _ -- argument to add 
	\return this reference
	*/
	CColor& operator+=(const CColor& _)
	{
		int r = _.get_Red() + get_Red();
		int g = _.get_Green() + get_Green();
		int b = _.get_Blue() + get_Blue();
		set_Red(min(0xff,max(0,r)));
		set_Green(min(0xff,max(0,g)));
		set_Blue(min(0xff,max(0,b)));
		return *this;
	}

	/**\brief function that sub from current color value argument color
	\param _ -- argument to sub 
	\return this reference
	*/
	CColor& operator-=(const CColor& _)
	{
		int r = get_Red() - _.get_Red();
		int g = get_Green() - _.get_Green();
		int b = get_Blue() - _.get_Blue();
		set_Red(min(0xff,max(0,r)));
		set_Green(min(0xff,max(0,g)));
		set_Blue(min(0xff,max(0,b)));
		return *this;
	}
protected:
	COLORREF m_data;			///< color value
};

/**\brief function to get color between two argument colors
\param _c1 -- fisrt color
\param _c2 -- second color
\param _cf -- coofficient 
\return mixture color
*/
inline
CColor between(const CColor& _c1,const CColor& _c2,double _cf)
{
	CColor c1 = _c1,c2 = _c2;
	c1*=_cf;
	c2*=(1.0-_cf);
	c1+=c2;
	return c1;
}

/**\brief simple COM initializer
*/
struct CCOMInit
{
	/**\brief simple COm initialize
	*/
	CCOMInit()
	{
		CoInitialize(NULL);
	}

#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
	/**\brief Extended COM initialize
	\param _dwCoInit -- COINIT value. See CoInitializeEx() function in MSDN
	*/
	CCOMInit(DWORD _dwCoInit)
	{
		::CoInitializeEx(NULL,_dwCoInit);
	}
#endif // DCOM

	/**\brief COM uninitializer
	*/
	~CCOMInit()
	{
		CoUninitialize();
	}
};//struct CCOMInit

#if !defined(OFN_ENABLESIZING)

	/**\brief macro that is not defined in MSVC 5.0 so we need specify it 
              for better work with CFileDialog. 
	*/
	#define OFN_ENABLESIZING             0x00800000

#endif


/**\brief wait result data type
*/
struct CWaitResult
{
	/**\brief constructor
	\param _wr -- wait result code
	*/
	explicit CWaitResult(DWORD _wr,long _EvntCnt = 1)
		:m_dwres(_wr),m_nEventCnt(_EvntCnt)
	{}
	DWORD m_dwres;		///< wait result value
	long m_nEventCnt;
};//struct CWaitResult

/**\brief function that returns wait result code string representation
\param _wr -- wait result code
\return string representation of specified wait code
*/
inline
CString_ to_str(const CWaitResult& _wr)
{
	if(in(_wr.m_dwres,WAIT_OBJECT_0,WAIT_OBJECT_0 + _wr.m_nEventCnt))	
		return format(_T("WAIT_OBJECT_%d"),_wr.m_dwres-WAIT_OBJECT_0);

	if(in(_wr.m_dwres,WAIT_ABANDONED,WAIT_ABANDONED + _wr.m_nEventCnt))	
		return format(_T("WAIT_ABANDONED_%d"),_wr.m_dwres-WAIT_ABANDONED);

	if(EQL(_wr.m_dwres,WAIT_TIMEOUT))	return _T("WAIT_TIMEOUT");
	if(EQL(_wr.m_dwres,WAIT_FAILED))	return _T("WAIT_FAILED");
	return _T("Unknown wait result");
}

/**\brief class that implement base functions of top windows enumaration
\tparam _Type -- enumerator class that process windows
*/
template<typename _Type>
struct TopWindowsEnumeratorImpl 
{
	/**\brief enumerator function to start enumeration
	*/
	bool enum_windows()
	{
		return EnumWindows(EnumWindowsProc,(LPVOID)static_cast<_Type*>(this));
	}

protected:
	/**\brief callback function to window enumaration
	*/
	static BOOL CALLBACK EnumWindowsProc(HWND _hwnd,LPARAM _lParam)
	{
		_Type* pEnumerator = reinterpret_cast<_Type*>(_lParam);
		VERIFY_EXIT1(NOT_NULL(pEnumerator),FALSE);
		return pEnumerator->process_window(_hwnd);
	}

	/**\brief function that you should implement in you window enumartor;
	\param[in] _hwnd -- window handle
	\return To continue enumeration, the callback function must return TRUE; 
	        to stop enumeration, it must return FALSE.
	*/
	BOOL process_window(HWND _hwnd);
};

#if !defined(ERROR_INVALID_STATE)
	#define ERROR_INVALID_STATE (5023L)
#endif


/**\brief class to suuport load of some dynamic library and to get some library function
*/
class CLibrary
{
public:
	/**\brief Constructor
	*/
	CLibrary():m_hmodule(NULL){}

	/**\brief destructor
	*/
	~CLibrary()
	{
		free();
	}

	/**\brief function to return true if object is valid (mean module was succesfully loaded)
	\return true if DLL was successfully loaded
	*/
	bool is_valid() const 
	{
		return NOT_NULL(m_hmodule);
	}

	/**\brief validate operator 
	\return true if DLL was successfully loaded
	*/
	operator bool() const {return is_valid();}
	
	/**\brief validate operator 
	\return true if DLL was not successfully loaded
	*/
	bool operator ! () const {return !operator bool();}

	/**\brief function to free current loaded library
	*/
	void free()
	{
		if(m_hmodule) {::FreeLibrary(m_hmodule);m_hmodule = NULL;}
	}

	/**\brief function to load library
	\param _szModuleName -- DLL name or path
	*/
	void load(LPCTSTR _szModuleName)
	{
		free();
		m_hmodule = ::LoadLibrary(_szModuleName);
	}

	/**\brief function that return some exported function of loaded DLL
	\param _szName -- function name
	\return function pointer
	*/
	LPVOID get_func(LPCTSTR _szName)
	{
		return ::GetProcAddress(m_hmodule,string_converter<TCHAR,char>(_szName));
	}
protected:
	HMODULE m_hmodule;	///< DLL module handle
};

/**\brief class that suport work in you application with RichEdit Ctrl of 2.0 version
*/
struct RichEdit20Library
{
public:

	/**\brief function that return if you can use RichEdit 2.0 functions
	\return true if RichEdit 2.0 functions are supported
	*/
	static bool is_loaded()
	{
		static RichEdit20Library _;
		return _.m_richedit20;
	}

	/**\brief Destructor
	*/
	~RichEdit20Library()
	{
	}

protected:
	/**\brief protected constructor
	*/
	RichEdit20Library()	
	{
		m_richedit20.load(_T("Riched20.dll"));
	}

	CLibrary m_richedit20;		///< library load variable
};//struct RichEdit20Library

//@}

#endif//#if !defined(__WINAPIEXT_H__0EC9D168_86C2_4791_954A_7A656482B90A_INCLUDED)
