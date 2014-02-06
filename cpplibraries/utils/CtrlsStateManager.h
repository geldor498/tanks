#if !defined(__CTRLSSTATEMANAGER_H__20E29348_7C5D_43B6_8887_F1AF51DE65C7__INCLUDE)
#define __CTRLSSTATEMANAGER_H__20E29348_7C5D_43B6_8887_F1AF51DE65C7__INCLUDE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "utils.h"

/**\brief class to present control rule
*/
struct ctrl_rule
{
	/**\brief constructor
	*/
	ctrl_rule()
		:m_bShowExist(false)
		,m_bHideExist(false)
		,m_bEnableExist(false)
		,m_bDisableExist(false)

		,m_bShow(false)
		,m_bHide(false)
		,m_bEnable(false)
		,m_bDisable(false)
	{
	}

	/**\brief function to replace value of this
	\param[in] _ -- new value
	*/
	void set(const ctrl_rule& _)
	{
		m_bShowExist = _.m_bShowExist;
		m_bHideExist = _.m_bHideExist;
		m_bEnableExist = _.m_bEnableExist;
		m_bDisableExist = _.m_bDisableExist;

		m_bShow = _.m_bShow;
		m_bHide = _.m_bHide;
		m_bEnable = _.m_bEnable;
		m_bDisable = _.m_bDisable;
	}

	/**\brief function to combine existing value with new one
	\param[in] _ -- value to combine this value with
	*/
	void combine(const ctrl_rule& _)
	{
		combine(m_bShowExist,m_bShow,_.m_bShowExist,_.m_bShow);
		combine(m_bHideExist,m_bHide,_.m_bHideExist,_.m_bHide);
		combine(m_bEnableExist,m_bEnable,_.m_bEnableExist,_.m_bEnable);
		combine(m_bDisableExist,m_bDisable,_.m_bDisableExist,_.m_bDisable);
	}

	/**\brief function to check conflict of this item with input one
	\param[in] _ -- value to check
	\return true if there is some conflict
	*/
	bool check_conflict(const ctrl_rule& _) const
	{
		return check_conflict(m_bShowExist,m_bShow,_.m_bShowExist,_.m_bShow)
			|| check_conflict(m_bHideExist,m_bHide,_.m_bHideExist,_.m_bHide)
			|| check_conflict(m_bEnableExist,m_bEnable,_.m_bEnableExist,_.m_bEnable)
			|| check_conflict(m_bDisableExist,m_bDisable,_.m_bDisableExist,_.m_bDisable)
			;
	}

	/**\brief function that return true if this class is valid one
	\return true if this class is valid one
	*/
	bool is_valid() const
	{
		if(m_bShowExist && m_bHideExist)
		{
			if(m_bShow==m_bHide) return false;
		}
		if(m_bEnableExist && m_bDisableExist)
		{
			if(m_bEnable==m_bDisable) return false;
		}
		return true;
	}

	/**\brief process some dlg item
	\param[in] _ItemHwnd -- dialog item window handle
	*/
	void process(HWND _ItemHwnd) const
	{
		VERIFY_EXIT(is_valid());
		VERIFY_EXIT(NOT_NULL(_ItemHwnd) && ::IsWindow(_ItemHwnd));

		if(m_bShowExist || m_bHideExist)
		{
			bool bShow = 
				(m_bShowExist?m_bShow:false)
				|| (m_bHideExist?!m_bHide:false)
				;
			::ShowWindow(_ItemHwnd,bShow?SW_SHOW:SW_HIDE);
		}
		if(m_bEnableExist || m_bDisableExist)
		{
			bool bEnable = 
				(m_bEnableExist?m_bEnable:false)
				|| (m_bDisableExist?!m_bDisable:false)
				;
			::EnableWindow(_ItemHwnd,bEnable);
		}
	}
	
	/**\brief function to get string representation of this class
	*/
	void to_string(CString_& _s) const
	{
		_s.Empty();
		if(m_bShowExist) _s += Format(_T("show = %s"),bool2str(m_bShow));
		if(m_bHideExist) _s += Format(_T("hide = %s"),bool2str(m_bHide));
		if(m_bEnableExist) _s += Format(_T("enable = %s"),bool2str(m_bEnable));
		if(m_bDisableExist) _s += Format(_T("disable = %s"),bool2str(m_bDisable));
	}
protected:
	/**\brief function to check conflick for some variable of whole class
	\param _bValExist -- if current value exist
	\param _bVal -- current value if this value exist
	\param _bOuterValExist -- outer value exist
	\param _bOuterVal -- outer value if it is exist
	\return true if there is some conflict
	*/
	bool check_conflict(bool _bValExist,bool _bVal,bool _bOuterValExist,bool _bOuterVal) const
	{
		if(!_bOuterValExist) return false;
		if(_bValExist) return NEQL(_bVal,_bOuterVal);
		return false;
	}

	/**\brief function to combine some value of this class with another 
	\param[in,out] _bValExist -- if value exist
	\param[in,out] _bVal -- if value exist
	\param[in] _bOuterValExist -- flag if outer value exist
	\param[in] _bOuterVal -- outer value (if exist)
	*/
	void combine(bool& _bValExist,bool& _bVal,bool _bOuterValExist,bool _bOuterVal)
	{
		if(!_bOuterValExist) return; // don`t modify any thing
		if(_bValExist)
		{
			// verify if we can combine values
			VERIFY_EXIT(_bVal==_bOuterVal); // should not create conflicting rules
			return;
		}
		// assign value 
		_bValExist = _bOuterValExist;
		_bVal = _bOuterVal;
	}

protected:	
	bool m_bShowExist;			///\< flag if show value exist
	bool m_bHideExist;			///\< flag if hide value exist
	bool m_bEnableExist;		///\< flag if enable value exist
	bool m_bDisableExist;		///\< flag if disable value exist

	bool m_bShow;				///\< show flag value
	bool m_bHide;				///\< hide flag value
	bool m_bEnable;				///\< enable flag value
	bool m_bDisable;			///\< disable flag value
};

/**\brief class to add show rule
*/
struct show_rule : public ctrl_rule
{
	explicit show_rule(bool _bCondition)
	{
		TRACE(_T("create show_rule(%s)\n"),bool2str(_bCondition));
		m_bShowExist = true;
		m_bShow = _bCondition;
	}
};

/**\brief class to add hide rule
*/
struct hide_rule : public ctrl_rule
{
	explicit hide_rule(bool _bCondition)
	{
		TRACE(_T("create hide_rule(%s)\n"),bool2str(_bCondition));
		m_bHideExist = true;
		m_bHide = _bCondition;
	}
};

/**\brief class to add enable rule
*/
struct enable_rule : public ctrl_rule
{
	explicit enable_rule(bool _bCondition)
	{
		TRACE(_T("create enable_rule(%s)\n"),bool2str(_bCondition));
		m_bEnableExist = true;
		m_bEnable = _bCondition;
	}
};

/**\brief class to add disable rule
*/
struct disable_rule : public ctrl_rule
{
	explicit disable_rule(bool _bCondition)
	{
		TRACE(_T("create disable_rule(%s)\n"),bool2str(_bCondition));
		m_bDisableExist = true;
		m_bDisable = _bCondition;
	}
};

typedef std::map<UINT,ctrl_rule> DlgItemsMap;		///\< typedef of map of rules for some item

/**\brief ctrls state manager 
*/
struct CCtrlsStateManager
{
	/**\brief constructor
	\param[in] _bIgnoreUnknow -- flag to ignore (don`t use assert) for dialog items
	                             that was not found at dialog window
	*/
	CCtrlsStateManager(bool _bIgnoreUnknow = false)
		:m_bAnyApplied(true)
		,m_bIgnoreUnknow(_bIgnoreUnknow)
	{
	}

	/**\brief operator to add item control id
	*/
	CCtrlsStateManager& operator << (int _ctrlid)
	{
		TRACE(_T("add dialog item %d\n"),_ctrlid);
		VERIFY_EXIT1(m_currule.is_valid(),*this);
		DlgItemsMap::iterator fit = m_map.find(_ctrlid);

		m_bAnyApplied = true;

		if(fit==m_map.end())
			fit = m_map.insert(DlgItemsMap::value_type(_ctrlid,m_currule)).first;
		else
		{
			fit->second.combine(m_currule);
			VERIFY_EXIT1(fit->second.is_valid(),*this);
		}

		return *this;
	}

	/**\brief operator to add rule for next items
	*/
	CCtrlsStateManager& operator << (const ctrl_rule& _rule)
	{
		CString_ s;
		_rule.to_string(s);
		TRACE(_T("add of rule {%s}\n"),(LPCTSTR)s);
		VERIFY_EXIT1(_rule.is_valid(),*this);
		if(m_bAnyApplied)
			m_currule.set(_rule);
		else 
			m_currule.combine(_rule);
		VERIFY_EXIT1(m_currule.is_valid(),*this);
		return *this;
	}

	/**\brief function to process 
	*/
	void process(HWND _hwnd) const
	{
		DlgItemsMap::const_iterator
			it = m_map.begin()
			,ite = m_map.end()
			;

		for(;it!=ite;++it)
		{
			UINT ctrlid = it->first;
			HWND hWndCtrl = GetDlgItem(_hwnd,ctrlid);
			bool bCtrlExist = NOT_NULL(hWndCtrl) && ::IsWindow(hWndCtrl);
			if(!bCtrlExist)
			{
				if(m_bIgnoreUnknow) continue;
				VERIFY_DO(FALSE,continue);
			}
			it->second.process(hWndCtrl);
		}
	}

protected:
	DlgItemsMap m_map;			///\< items rules
	ctrl_rule m_currule;		///\< current rule
	bool m_bAnyApplied;			///\< flag if to create new rule
	bool m_bIgnoreUnknow;		///\< flag to ignore unknown items at dialog window while process
};//struct CCtrlsStateManager




#endif//#if !defined(__CTRLSSTATEMANAGER_H__20E29348_7C5D_43B6_8887_F1AF51DE65C7__INCLUDE)