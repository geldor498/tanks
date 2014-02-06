#if !defined(__SECURITY_H__2CC8BC6A_BDE5_4238_A1C4_F821081F3278)
#define __SECURITY_H__2CC8BC6A_BDE5_4238_A1C4_F821081F3278

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "config.h"
#include "atlmfc.h"

#if _MSC_VER > 1100 
#error Don`t use this header not for MSVC 5.0
#endif

#include <new.h>
#include <LMCONS.H>
#include <accctrl.h>
#include <aclapi.h>
//#include <cguid.h>
#include <xutility>
#include <list>
#include <vector>
//#include <rpcdce.h>
//#include <sddl.h>
#include <algorithm>
#include <map>

#include "errors.h"

//#pragma warning(push)

#pragma warning(disable:4786)

/**\defgroup Utilities_Security Utilities to support security.
This code is a adapted for MSVC 5.0 ATL file. So if you work in MSVC of version
greater than 5.0 please use original security headers from ATL. Because they contains
only it own errors :) (but not mine too :-) ).

!!! This code is not fully tasted.!!!
*/

//@{
//#undef bool
#include <stl/bool.h>

#if !defined(sizea)
#define sizea(arg) sizeof(arg)/sizeof(arg[0])
#endif

#if !defined(IS_NULL)
#define IS_NULL(arg) (NULL==(arg))
#endif

#if !defined(NOT_NULL)
#define NOT_NULL(arg) (!IS_NULL(arg))
#endif

#if !defined(EQL)
#define EQL(val,arg) ((val)==(arg))
#endif

#if !defined(NEQL)
#define NEQL(val,arg) (!EQL(val,arg))
#endif

#if !defined(LT)
#define LT(_arg,_val) ((_arg)<(_val))
#endif

#if !defined(LTE)
#define LTE(_arg,_val) (LT(_arg,_val) || EQL(_arg,_val))
#endif

#if !defined(GT)
#define GT(_arg,_val) (!LTE(_arg,_val))
#endif

#if !defined(GTE)
#define GTE(_arg,_val) (!LT(_arg,_val))
#endif

#if !defined(THROW0)
	#define THROW0 throw()
#endif

#if !defined(THROWANY)
	#if _MSC_VER > 1300
		#define THROWANY throw(...)
	#else
		#define THROWANY
	#endif
#endif

#pragma warning(disable:4666)

#if !defined(TOBOOL_OPERATOR)
#define TOBOOL_OPERATOR

inline
bool to_bool(int _val)
{
	return NEQL(_val,FALSE);
}

//inline 
//bool to_bool(bool _val)
//{
//	return _val;
//}

#endif//#if !defined(TOBOOL_OPERATOR)

#pragma warning(default:4666)

#if !defined(VERIFY_EXIT)
#define VERIFY_EXIT(_arg)	\
{	\
	bool _ = to_bool(_arg);	\
	ASSERT_(_);	\
	if(!_) return;	\
}
#endif

#if !defined(VERIFY_EXIT1)
#define VERIFY_EXIT1(_arg,_ret)	\
{	\
	bool _ = to_bool(_arg);	\
	ASSERT_(_);	\
	if(!_) return (_ret);	\
}
#endif

#if !defined(VERIFY_DO)
#define VERIFY_DO(_arg,_operation)	\
{	\
	bool _ = to_bool(_arg);	\
	ASSERT_(_);	\
	if(!_) _operation;	\
}
#endif

#pragma warning(disable:4666)

// #if !defined(NEQL_OPERATOR)
// #define NEQL_OPERATOR
// template<typename _Type>
// bool operator!=(const _Type& _x,const _Type& _y)
// {
// 	return !(_x==_y);
// }
// #endif
// 
// #if !defined(GT_OPERATOR)
// #define GT_OPERATOR
// template<typename _Type>
// bool operator>(const _Type& _x,const _Type& _y)
// {
// 	return to_bool(_y<_x);
// }
// #endif

#pragma warning(default:4666)

namespace utils
{
	template<typename _Type>
	struct CTmpBuffer
	{
	protected:
		_Type* m_pbuf;
	public:
		CTmpBuffer(_Type* _pbuf = NULL):m_pbuf(NULL)
		{
			attach(_pbuf);
		}

		CTmpBuffer(long _size):m_pbuf(NULL)
		{
			allocate(_size);
		}

		~CTmpBuffer()
		{
			free();
		}

		void attach(_Type* _pbuf)
		{
			free();
			m_pbuf = _pbuf;
		}

		_Type* allocate(long _size)
		{
			free();
			if(_size<=0) return NULL;
			m_pbuf = trace_alloc(reinterpret_cast<_Type*>(new BYTE[_size]));
			ZeroMemory(m_pbuf,_size);
			return m_pbuf;
		}

		template<typename _ItemType>
		_Type* allocate_items(long _size,_ItemType)
		{
			free();
			if(_size<=0) return NULL;
			m_pbuf = trace_alloc(reinterpret_cast<_Type*>(new BYTE[_size*sizeof(_ItemType)]));
			ZeroMemory(m_pbuf,_size);
			return m_pbuf;
		}

		_Type* detach()
		{
			_Type* pbuf = m_pbuf;
			m_pbuf = NULL;
			return pbuf;
		}

		_Type* get() const {return m_pbuf;}
		operator _Type* () const {return get();}
		//operator void* () const {return static_cast<void*>(m_pbuf);}
		_Type* operator->() const {return get();}
		//_Type** operator& () {return &m_pbuf;}
		_Type** get_bufferptr() const {return &(const_cast<_Type*>(m_pbuf));}

		operator bool () const 
		{
			return NOT_NULL(m_pbuf);
		}

		bool operator!() const
		{
			return !operator bool();
		}

		bool operator==(int _null) const
		{
			VERIFY_EXIT1(_null==0,false);
			return !(bool)*this;
		}

		bool operator!=(int _null) const
		{
			return !operator==(_null);
		}

		CTmpBuffer<_Type>& operator = (_Type* _pbuf)
		{
			free();
			m_pbuf = _pbuf;
			return *this;
		}

		void free()
		{
			if(NULL==m_pbuf) return;
			delete trace_free((LPBYTE)m_pbuf);
			m_pbuf = NULL;
		}
	private:
		CTmpBuffer(const CTmpBuffer& _);
		CTmpBuffer<_Type>& operator=(const CTmpBuffer& _);
	};//struct CTmpBuffer

	template<typename _Type,long _DefBufferSize = 1024>
	struct CTmpBufferOnStack
	{
		CTmpBufferOnStack(long _size = 0)
			:m_ptr(NULL)
		{
			allocate(_size);
		}

		~CTmpBufferOnStack()
		{
			free();
		}

		void free()
		{
			if(IS_NULL(m_ptr)) return;
			if(NEQL((LPVOID)m_ptr,(LPVOID)m_buffer)) delete trace_free((LPBYTE)m_ptr);
			m_ptr = NULL;
		}

		void allocate(long _size)
		{
			free();
			if(EQL(_size,0)) return;
			VERIFY_EXIT(_size>0);
			if(_size<=_DefBufferSize) m_ptr = reinterpret_cast<_Type*>(m_buffer);
			else m_ptr = trace_alloc(reinterpret_cast<_Type*>(new BYTE[_size]));
		}

		operator _Type* () const {return m_ptr;}
		_Type* operator -> () const {return m_ptr;}
		operator bool () const {return NOT_NULL(m_ptr);}
		bool operator ! () const {return !operator bool();}
		bool operator==(int _null) const {VERIFY_EXIT1(_null==0,false); return operator !();}
		bool operator!=(int _null) const {return !operator==(_null);}
	protected:
		_Type* m_ptr;
		BYTE m_buffer[_DefBufferSize];
	private:
		CTmpBufferOnStack(const CTmpBufferOnStack& _);
		CTmpBufferOnStack<_Type>& operator=(const CTmpBufferOnStack& _);
	};//struct CTmpBufferOnStack

	struct CGuid : public GUID 
	{
		CGuid()
		{
			ZeroMemory(this,sizeof(GUID));
		}

		CGuid(const GUID& _)
		{
			operator=(_);
		}

		CGuid& operator=(const GUID& _)
		{
			memcpy(this,&_,sizeof(GUID));
			return *this;
		}
	};//struct CGuid

	struct CAclSizeInformation : public ACL_SIZE_INFORMATION
	{
		CAclSizeInformation()
		{
			ZeroMemory(this,sizeof(ACL_SIZE_INFORMATION));
		}
	};//struct CAclSizeOnformation

	struct CAclRevisionInforamtion : public ACL_REVISION_INFORMATION
	{
		CAclRevisionInforamtion()
		{
			ZeroMemory(this,sizeof(ACL_REVISION_INFORMATION));
		}
	};//struct CAclRevisionInforamtion

	struct CAceHeader : public ACE_HEADER
	{
		CAceHeader()
		{
			ZeroMemory(this,sizeof(ACE_HEADER));
		}
	};//struct CAclHeader

	inline
	bool checked_memcpy(LPVOID _outbuf,long _size1,LPCVOID _inbuf,long _size2)
	{
		if(_size2<0 || IsBadReadPtr(_inbuf,_size2)) return false;
		if(_size1<0 || IsBadWritePtr(_outbuf,_size1)) return false;
		long size = min(_size1,_size2);
		memcpy(_outbuf,_inbuf,size);
		return true;
	}

}//namespace utils

struct CLuid : public LUID
{
	CLuid(LONG _HighPart = 0,DWORD _LowPart = 0)
	{
		LowPart = _LowPart; 
		HighPart = _HighPart;
	}

	CLuid(const LUID& _)
	{
		LowPart = _.LowPart;
		HighPart = _.HighPart;
	}

	bool operator==(const CLuid& _) const
	{
		return EQL(LowPart,_.LowPart) && EQL(HighPart,_.HighPart);
	}

	bool operator<(const CLuid& _) const
	{
		return LT(HighPart,_.HighPart) 
			|| EQL(HighPart,_.HighPart) && LT(LowPart,_.LowPart)
			;
	}
};//struct CLuid


template<class _Ty>
class auto_ptrex
{
public:
	typedef _Ty element_type;
	explicit auto_ptrex(_Ty *_P = 0) 
		: _Owns(_P != 0), _Ptr(_P) 
	{}

	auto_ptrex(const auto_ptrex<_Ty>& _Y) 
		: _Owns(_Y._Owns), _Ptr(_Y.release()) 
	{}

	auto_ptrex<_Ty>& operator=(const auto_ptrex<_Ty>& _Y)
	{
		if (_Ptr != _Y.get())
		{
			if(_Owns) delete trace_free(_Ptr);
			_Owns = _Y._Owns;
			_Ptr = _Y.release(); 
		}
		else if(_Y._Owns) _Owns = true;
		return (*this); 
	}

	~auto_ptrex()
	{
		if(_Owns) delete trace_free(_Ptr); 
	}

	_Ty& operator*() const
	{
		return (*get()); 
	}

	_Ty* operator->() const 
	{
		return (get()); 
	}

	_Ty* get() const 
	{
		return (_Ptr); 
	}

	_Ty* release() const 
	{
		const_cast<auto_ptrex<_Ty>*>(this)->_Owns = false;
		return (_Ptr);
	}

	bool operator==(const auto_ptrex<_Ty>& _) const
	{
		return NULL==_Ptr && NULL==_._Ptr
			|| NULL!=_Ptr && NULL!=_._Ptr && *_Ptr==*_._Ptr
			;
	}

	bool operator!=(const auto_ptrex<_Ty>& _) const
	{
		return !(*this==_);
	}

	bool operator<(const auto_ptrex<_Ty>& _) const
	{
		if(*this==_) return false;
		return NULL==_Ptr && NULL!=_._Ptr
			|| NULL!=_Ptr && NULL!=_._Ptr && *_Ptr<*_._Ptr
			;
	}

	bool operator>(const auto_ptrex<_Ty>& _) const 
	{
		return _<*this;
	}
private:
	bool _Owns;
	_Ty *_Ptr;
};//template<> class auto_ptrex

//#define SID_REVISION                     (1)    // Current revision level
//#define SID_MAX_SUB_AUTHORITIES          (15)
//#define SID_RECOMMENDED_SUB_AUTHORITIES  (1)    // Will change to around 6

                                                // in a future release.
//#ifndef MIDL_PASS
//#define SECURITY_MAX_SID_SIZE  \
//      (sizeof(SID) - sizeof(DWORD) + (SID_MAX_SUB_AUTHORITIES * sizeof(DWORD)))
//#endif // MIDL_PASS

#if !defined(SYSTEM_AUDIT_OBJECT_ACE_TYPE)
	#define SYSTEM_AUDIT_OBJECT_ACE_TYPE (0x7)
#endif
#if !defined(ACCESS_ALLOWED_OBJECT_ACE_TYPE)
	#define ACCESS_ALLOWED_OBJECT_ACE_TYPE (0x5)
#endif
#if !defined(ACE_OBJECT_TYPE_PRESENT)
	#define ACE_OBJECT_TYPE_PRESENT (0x1)
#endif
#if !defined(SE_DACL_AUTO_INHERITED)
	#define SE_DACL_AUTO_INHERITED (0x0400)
#endif
#if !defined(SE_DACL_PROTECTED)
	#define SE_DACL_PROTECTED (0x1000)
#endif
#if !defined(SE_SACL_AUTO_INHERITED)
	#define SE_SACL_AUTO_INHERITED (0x0800)
#endif
#if !defined(SE_SACL_PROTECTED)
	#define SE_SACL_PROTECTED (0x2000)
#endif
#if !defined(PROTECTED_DACL_SECURITY_INFORMATION)
	#define PROTECTED_DACL_SECURITY_INFORMATION (0x80000000L)
#endif
#if !defined(UNPROTECTED_DACL_SECURITY_INFORMATION)
	#define UNPROTECTED_DACL_SECURITY_INFORMATION (0x20000000L)
#endif
#if !defined(PROTECTED_SACL_SECURITY_INFORMATION)
	#define PROTECTED_SACL_SECURITY_INFORMATION (0x40000000L)
#endif
#if !defined(UNPROTECTED_SACL_SECURITY_INFORMATION)
	#define UNPROTECTED_SACL_SECURITY_INFORMATION (0x10000000L)
#endif

namespace security
{

	enum SecuredObjectTypeEn
	{
		SecObjType_UnknownObjectType = 0
		,SecObjType_FileObject
		,SecObjType_Service
		,SecObjType_Printer
		,SecObjType_RegistryKey
		,SecObjType_LMShare
		,SecObjType_KernelObject
		,SecObjType_WindowObject
		,SecObjType_DSObject
		,SecObjType_DSObjectAll
		,SecObjType_ProviderDefinedObject
		,SecObjType_WMIGuidObject
		,SecObjType_RegistryWOW64_32Key
	};//enum SecuredObjectTypeEn


	enum AccessRightsEn
	{
		AccessRights_FileFullControl = FILE_ALL_ACCESS
		,AccessRights_FileRead = FILE_GENERIC_READ
		,AccessRights_FileWrite = FILE_GENERIC_WRITE
		,AccessRights_FileExecute = FILE_GENERIC_EXECUTE
		,AccessRights_FileReadExecute = (FILE_GENERIC_READ | FILE_GENERIC_EXECUTE)
		,AccessRights_FileModify = (FILE_GENERIC_READ | FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE | DELETE)
		,AccessRights_FileReadUnsync = (STANDARD_RIGHTS_READ | FILE_READ_DATA | FILE_READ_ATTRIBUTES | FILE_READ_EA)
		,AccessRights_FileWriteUnsync = (FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA | FILE_APPEND_DATA)
		,AccessRights_FileReadExecuteUnsync = (FILE_GENERIC_READ | FILE_GENERIC_EXECUTE) & (~SYNCHRONIZE)
		,AccessRights_FileListDirectory = FILE_LIST_DIRECTORY
		,AccessRights_FileAddFile = FILE_ADD_FILE
		,AccessRights_FileAddSubdirectory = FILE_ADD_SUBDIRECTORY

		,AccessRights_KeyAllAccess = KEY_ALL_ACCESS
		,AccessRights_KeyRead = KEY_READ
		,AccessRights_KeyWrite = KEY_WRITE
		,AccessRights_KeyExecute = KEY_EXECUTE
		/*
		,AccessRights_DsReadProp = ADS_RIGHT_DS_READ_PROP,
		,AccessRights_DsWriteProp = ADS_RIGHT_DS_WRITE_PROP,
		,AccessRights_DsCreateChild = ADS_RIGHT_DS_CREATE_CHILD,
		,AccessRights_DsDeleteChild = ADS_RIGHT_DS_DELETE_CHILD,
		,AccessRights_DsList = ADS_RIGHT_DS_LIST,
		,AccessRights_DsSelf = ADS_RIGHT_DS_SELF,
		,AccessRights_DsListObject = ADS_RIGHT_DS_LIST_OBJECT,
		,AccessRights_DsDeleteTree = ADS_RIGHT_DS_DELETE_TREE,
		,AccessRights_DsControlAccess = ADS_RIGHT_DS_CONTROL_ACCESS,
		*/
		,AccessRights_GenericRead = GENERIC_READ
		,AccessRights_GenericWrite = GENERIC_WRITE
		,AccessRights_GenericExecute = GENERIC_EXECUTE
		,AccessRights_GenericAll = GENERIC_ALL

		,AccessRights_Delete = DELETE
		,AccessRights_ReadControl = READ_CONTROL
		,AccessRights_WriteDac = WRITE_DAC
		,AccessRights_WriteOwner = WRITE_OWNER
		,AccessRights_Synchronize = SYNCHRONIZE

		,AccessRights_StandardRightsAll = STANDARD_RIGHTS_ALL
		,AccessRights_SpecificRightsAll = SPECIFIC_RIGHTS_ALL
	};//enum AccessRightsEn

	enum AceTypeEn
	{
		AceType_AccessAllowed = ACCESS_ALLOWED_ACE_TYPE
		,AceType_AccessDenied = ACCESS_DENIED_ACE_TYPE
		,AceType_SystemAudit = SYSTEM_AUDIT_ACE_TYPE
		,AceType_SystemAlarm = SYSTEM_ALARM_ACE_TYPE
		,AceType_AccessAllowedCompound = 0x4
		,AceType_AccessAllowedObject = ACCESS_ALLOWED_OBJECT_ACE_TYPE
		,AceType_AccessDeniedObject = 0x6				//ACCESS_DENIED_OBJECT_ACE_TYPE
		,AceType_SystemAuditObject = SYSTEM_AUDIT_OBJECT_ACE_TYPE
		,AceType_SystemAlarmObject = 0x8				//SYSTEM_ALARM_OBJECT_ACE_TYPE
		,AceType_AccessAllowedCallback = 0x9
		,AceType_AccessDeniedCallback = 0xA
		,AceType_AccessAllowedCallbackObject = 0xB
		,AceType_AccessDeniedCallbackObject = 0xC
		,AceType_SystemAuditCallback = 0xD
		,AceType_SystemAlarmCallback = 0xE
		,AceType_SystemAuditCallbackObject = 0xF
		,AceType_SystemAlarmCallbackObject = 0x10
		,AceType_Undefined = 0xFF
	};//enum AceTypeEn

	#if !defined(INHERITED_ACE)
	#define INHERITED_ACE (0x10)
	#endif

	enum AceInheritanceFlagsEn
	{
		AceInheritance_ObjectInherit = OBJECT_INHERIT_ACE
		,AceInheritance_ContainerInherit = CONTAINER_INHERIT_ACE
		,AceInheritance_NoPropagateInherit = NO_PROPAGATE_INHERIT_ACE
		,AceInheritance_InheritOnly = INHERIT_ONLY_ACE
		,AceInheritance_Inherited = INHERITED_ACE
		,AceInheritance_SuccessfulAccess = SUCCESSFUL_ACCESS_ACE_FLAG
		,AceInheritance_FailedAccess = FAILED_ACCESS_ACE_FLAG
	};//enum AceInheritanceFlagsEn



	inline
	SID* CopySid(const SID* _psid)
	{
		DWORD len = ::GetLengthSid(static_cast<PSID>(const_cast<SID*>(_psid)));
		utils::CTmpBuffer<SID> pdestsid(len);
		if(::CopySid(len,pdestsid,static_cast<PSID>(const_cast<SID*>(_psid)))) return pdestsid.detach();
		return NULL;
	}

	inline
	SID* GetSidFromName(LPCTSTR _pszAccountName,LPCTSTR _pszSystem,CString_* _psDomain = NULL)
	{
		DWORD dwSidSize = ::GetSidLengthRequired(SID_MAX_SUB_AUTHORITIES);
		DWORD dwDomainSize = 128;
		DWORD cbSid = dwSidSize, cbDomain = dwDomainSize;
		utils::CTmpBuffer<SID> psid(dwSidSize);
		utils::CTmpBuffer<TCHAR> szDomain(dwDomainSize);
		SID_NAME_USE snu = SidTypeUnknown;

		BOOL bSuccess = ::LookupAccountName(_pszSystem,_pszAccountName,psid,&cbSid,szDomain,&cbDomain,&snu);
		if(!bSuccess)
		{
			if(::GetLastError()==ERROR_INSUFFICIENT_BUFFER)
			{
				if(cbSid>dwSidSize)
				{
					psid.allocate(cbSid);
					//dwSidSize = cbSid;
				}
				if(cbDomain>dwDomainSize)
				{
					szDomain.allocate(cbDomain);
					//dwDomainSize = cbDomain;
				}
				bSuccess = ::LookupAccountName(_pszSystem,_pszAccountName,psid,&cbSid,szDomain,&cbDomain,&snu);
			}
		}
		if(bSuccess) 
		{
			if(NOT_NULL(_psDomain)) *_psDomain = szDomain;
			return psid.detach();
		}

		return NULL;
	}

	inline
	ACL* CopyAcl(const ACL* _pacl)
	{
		if(!_pacl) return NULL;
		utils::CTmpBuffer<ACL> pdestacl(_pacl->AclSize);
		memcpy((void*)pdestacl,_pacl,_pacl->AclSize);
		return pdestacl.detach();
	}
};//namespace security

class CSid
{
public:
	CSid() THROW0
		:m_sidnameuse(SidTypeInvalid)
		,m_psid(NULL)
	{
	}

	explicit CSid(LPCTSTR _pszAccountName, LPCTSTR _pszSystem = NULL) THROWANY
		:m_sidnameuse(SidTypeInvalid)
		,m_psid(NULL)
	{
		if(!load_account(_pszAccountName,_pszSystem))
			throw_lastWin32Error();
	}

	explicit CSid(const SID* _pSid, LPCTSTR _pszSystem = NULL) THROWANY
		:m_sidnameuse(SidTypeInvalid)
		,m_psid(NULL)
	{
		if(!load_account(_pSid,_pszSystem))
			throw_lastWin32Error();
	}

	CSid(const SID_IDENTIFIER_AUTHORITY& _IdentifierAuthority, BYTE _nSubAuthorityCount, ...) THROWANY
		:m_sidnameuse(SidTypeInvalid)
		,m_psid(NULL)
	{
		VERIFY_EXIT(_nSubAuthorityCount!=0);

		utils::CTmpBuffer<SID> psid(::GetSidLengthRequired(_nSubAuthorityCount));
		if(!::InitializeSid(psid
				,const_cast<SID_IDENTIFIER_AUTHORITY *>(&_IdentifierAuthority)
				,_nSubAuthorityCount
				)
			)
			throw_lastWin32Error();

		va_list args;
		va_start(args, _nSubAuthorityCount);
		for(UINT i = 0; i < _nSubAuthorityCount; i++)
			*::GetSidSubAuthority(psid, i) = va_arg(args, DWORD);
		va_end(args);

		m_psid = psid.detach();

		m_sidnameuse = SidTypeUnknown;
	}

	CSid(HANDLE _hToken) THROWANY
		:m_sidnameuse(SidTypeInvalid)
		,m_psid(NULL)
	{
		load_account(_hToken);
	}

	virtual ~CSid() THROW0
	{
		free();
	}

	CSid(const CSid& _) THROWANY
		:m_sidnameuse(_.m_sidnameuse)
		,m_strAccountName(_.m_strAccountName)
		,m_strDomain(_.m_strDomain)
		,m_strSid(_.m_strSid)
		,m_strSystem(_.m_strSystem)
		,m_psid(NULL)
	{
		if(NOT_NULL(_.m_psid))
		{
			m_psid = security::CopySid(_.m_psid);
			if(IS_NULL(m_psid)) throw_lastWin32Error();
		}
	}

	CSid& operator=(const CSid& _) THROWANY
	{
		if(NEQL(this,&_))
		{
			m_sidnameuse = _.m_sidnameuse;
			m_strAccountName = _.m_strAccountName;
			m_strDomain = _.m_strDomain;
			m_strSid = _.m_strSid;
			m_strSystem = _.m_strSystem;

			if(NOT_NULL(_.m_psid))
			{
				copy(_.m_psid);
			}
		}
		return *this;
	}

	CSid(const SID& _sid) THROWANY
		:m_sidnameuse(SidTypeInvalid)
	{
		copy(&_sid);
	}

	CSid& operator=(const SID& _sid) THROWANY
	{
		if(NEQL(m_psid,&_sid))
		{
			copy(&_sid);
			m_sidnameuse = SidTypeUnknown;
		}
		return *this;
	}

	bool load_account(LPCTSTR _pszAccountName, LPCTSTR _pszSystem = NULL) THROWANY
	{
		clear();

		VERIFY_EXIT1(NOT_NULL(_pszAccountName),false);
		CString_ sDomain;
		m_psid = security::GetSidFromName(_pszAccountName,_pszSystem,&sDomain);
		if(IS_NULL(m_psid)) throw_lastWin32Error();
		m_strDomain = sDomain;
		m_strAccountName = _pszAccountName;
		m_strSystem = _pszSystem;
		return true;
	}

	bool load_account(const SID* _pSid, LPCTSTR _pszSystem = NULL) THROWANY
	{
		clear();
		if(NOT_NULL(_pSid))
		{
			try
			{
				m_strSystem = _pszSystem;
				copy(_pSid);
				return true;
			}
			catch(utils::CWin32Exception EXC_ACCESS_TYPE)
			{
				clear();
				throw;
			}
		}

		return false;
	}

	bool load_account(HANDLE _hToken)
	{
		clear();

		DWORD dwInfoBufferSize = 0;
		::GetTokenInformation(_hToken, TokenUser, NULL, 0, &dwInfoBufferSize);
		utils::CTmpBuffer<TOKEN_USER> pTokenUser(dwInfoBufferSize);

		if (::GetTokenInformation(_hToken, TokenUser, pTokenUser, dwInfoBufferSize, &dwInfoBufferSize))
		{
			copy((SID*)(pTokenUser->User.Sid));
			return true;
		}
		throw_lastWin32Error();
		return false;
	}

	LPCTSTR get_accountName() const THROWANY
	{
		if(m_strAccountName.IsEmpty())
			get_accountNameAndDomain();
		return m_strAccountName;
	}

	LPCTSTR get_domain() const THROWANY
	{
		if(m_strDomain.IsEmpty())
			get_accountNameAndDomain();
		return m_strDomain;
	}

	LPCTSTR get_sid() const THROWANY
	{
		try
		{
			if(m_strSid.IsEmpty())
			{
#if(_WIN32_WINNT >= 0x0500)

				LPTSTR pszSid;
				if(::ConvertSidToStringSid(static_cast<PSID>(const_cast<SID*>(m_psid)), &pszSid))
				{
					m_strSid = pszSid;
					::LocalFree(pszSid);
				}
			
#else

				SID_IDENTIFIER_AUTHORITY *psia = ::GetSidIdentifierAuthority(static_cast<PSID>(const_cast<SID*>(m_psid)));
				UINT i;

				if(psia->Value[0] || psia->Value[1])
				{
					m_strSid = format(_T("S-%d-0x%02hx%02hx%02hx%02hx%02hx%02hx"), SID_REVISION,
						(USHORT)psia->Value[0],
						(USHORT)psia->Value[1],
						(USHORT)psia->Value[2],
						(USHORT)psia->Value[3],
						(USHORT)psia->Value[4],
						(USHORT)psia->Value[5]);
				}
				else
				{
					ULONG nAuthority = 0;
					for(i = 2; i < 6; i++)
					{
						nAuthority <<= 8;
						nAuthority |= psia->Value[i];
					}
					m_strSid = format(_T("S-%d-%lu"), SID_REVISION, nAuthority);
				}

				UINT nSubAuthorityCount = *::GetSidSubAuthorityCount(static_cast<PSID>(const_cast<SID*>(m_psid)));
				CString_ strTemp;
				for(i = 0; i < nSubAuthorityCount; i++)
				{
					strTemp = format(_T("-%lu"), *::GetSidSubAuthority(static_cast<PSID>(const_cast<SID*>(m_psid)),i));
					m_strSid += strTemp;
				}
#endif
				return m_strSid;
			}
		}
		catch(...)
		{
			m_strSid.Empty();
			throw;
		}
		return m_strSid;
	}

	const SID* GetPSID() const THROWANY {return m_psid;}
	operator const SID* () const THROWANY {return GetPSID();}

	SID_NAME_USE get_SidNameUse() const THROW0 {return m_sidnameuse;}

	UINT get_length() const THROW0
	{
		VERIFY_EXIT1(NOT_NULL(m_psid),0);
		return ::GetLengthSid(static_cast<PSID>(const_cast<SID*>(m_psid)));
	}

	// SID functions
	bool equal_prefix(const CSid& _rhs) const THROW0
	{
		if(IS_NULL(m_psid) || IS_NULL(_rhs.m_psid) || !IsValid() || !_rhs.IsValid())
			return false;
		return to_bool(
			::EqualPrefixSid(
				static_cast<PSID>(const_cast<SID*>(m_psid))
				,static_cast<PSID>(const_cast<SID*>(_rhs.m_psid))
				)
			);
	}

	bool equal_prefix(const SID* _psid) const THROW0
	{
		if(IS_NULL(m_psid) || !IsValid() || !::IsValidSid(static_cast<PSID>(const_cast<SID*>(_psid))))
			return false;
		return to_bool(
			::EqualPrefixSid(
				static_cast<PSID>(const_cast<SID*>(m_psid))
				,static_cast<PSID>(const_cast<SID*>(_psid))
				)
			);
	}

	const SID_IDENTIFIER_AUTHORITY *GetPSID_IDENTIFIER_AUTHORITY() const THROW0
	{
		VERIFY_EXIT1(IsValid(),NULL);
		return ::GetSidIdentifierAuthority(static_cast<PSID>(const_cast<SID*>(m_psid)));
	}

	DWORD get_subAuthority(DWORD _nSubAuthority) const THROW0
	{
		VERIFY_EXIT1(IsValid(),NULL);
		return *::GetSidSubAuthority(static_cast<PSID>(const_cast<SID*>(m_psid)), _nSubAuthority);
	}

	UCHAR get_subAuthorityCount() const THROW0
	{
		VERIFY_EXIT1(IsValid(),NULL);
		return *::GetSidSubAuthorityCount(static_cast<PSID>(const_cast<SID*>(m_psid)));
	}

	bool IsValid() const THROW0
	{
		return NOT_NULL(m_psid) && to_bool(::IsValidSid(m_psid));
	}

	bool equal(const CSid& _sid) const THROW0
	{
		if(IS_NULL(m_psid) || !IsValid() || !::IsValidSid(static_cast<PSID>(const_cast<SID*>(_sid.m_psid))))
			return false;
		return to_bool(
			::EqualSid(
				static_cast<PSID>(const_cast<SID*>(m_psid))
				,static_cast<PSID>(const_cast<SID*>(_sid.m_psid))
				)
			);
	}

	bool lt(const CSid& _sid) const THROW0
	{
		const SID_IDENTIFIER_AUTHORITY* la = GetPSID_IDENTIFIER_AUTHORITY();
		const SID_IDENTIFIER_AUTHORITY* ra = _sid.GetPSID_IDENTIFIER_AUTHORITY();

		int i;
		for(i=0; i<6; i++)
		{
			if (la->Value[i] < ra->Value[i])
			{
				return true;
			}
			else if (la->Value[i] > ra->Value[i])
			{
				return false;
			}
		}

		for(i=0; i<_sid.get_subAuthorityCount(); i++)
		{
			if (get_subAuthorityCount() == i)
			{
				// this is a prefix of _sid
				return true;
			}

			if (get_subAuthority(i) < _sid.get_subAuthority(i))
			{
				return true;
			}
			else if (get_subAuthority(i) > _sid.get_subAuthority(i))
			{
				return false;
			}
		}

		return false;
	}

	static CSid* get_currentUser() THROWANY
	{
		HANDLE hAccessToken;
		if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_READ, &hAccessToken))
			return trace_alloc(new CSid(hAccessToken));
		throw_lastWin32Error();
		return NULL;
	}

	bool operator==(const CSid& _sid) const THROW0
	{
		return equal(_sid);
	}

	bool operator!=(const CSid& _sid) const THROW0
	{
		return NEQL(*this,_sid);
	}

	bool operator<(const CSid& _sid) const THROW0
	{
		return lt(_sid);
	}

	bool operator>(const CSid& _sid) const THROW0
	{
		return _sid.lt(*this);
	}

//	bool operator<=(const CSid& _sid) const /*throw()*/
//	{
//		return !(_sid.lt(*this));
//	}
//
//	bool operator>=(const CSid& _sid) const /*throw()*/
//	{
//		return !(lt(_sid));
//	}

private:
	void copy(const SID* _p) THROWANY
	{
		VERIFY_EXIT(EQL(m_sidnameuse,SidTypeInvalid));
		VERIFY_EXIT(m_strAccountName.IsEmpty());
		VERIFY_EXIT(m_strDomain.IsEmpty());
		VERIFY_EXIT(m_strSid.IsEmpty());

		m_psid = security::CopySid(_p);
		if(IS_NULL(m_psid)) throw_lastWin32Error();
	}

	void clear() THROW0
	{
		free();
		m_sidnameuse = SidTypeInvalid;
		m_strAccountName.Empty();
		m_strDomain.Empty();
		m_strSid.Empty();
		m_strSystem.Empty();
	}

	void free() THROW0
	{
		if(NOT_NULL(m_psid)) delete trace_free(m_psid);
		m_psid = NULL;
	}

	void get_accountNameAndDomain() const THROWANY
	{
		static const DWORD dwMax = 32;
		DWORD cbName = dwMax, cbDomain = dwMax;
		TCHAR szName[dwMax], szDomain[dwMax];

		if(::LookupAccountSid(
				m_strSystem
				,static_cast<PSID>(const_cast<SID*>(m_psid))
				,szName,&cbName
				,szDomain, &cbDomain
				,&m_sidnameuse
				)
			)
		{
			m_strAccountName = szName;
			m_strDomain = szDomain;
		}
		else
		{
			switch(::GetLastError())
			{
			case ERROR_INSUFFICIENT_BUFFER:
			{
				LPTSTR pszName = m_strAccountName.GetBuffer(cbName);
				LPTSTR pszDomain = m_strDomain.GetBuffer(cbDomain);

				if (!::LookupAccountSid(
							m_strSystem
							,static_cast<PSID>(const_cast<SID*>(m_psid))
							,pszName, &cbName
							,pszDomain,&cbDomain
							,&m_sidnameuse
							)
						)
					throw_lastWin32Error();

				m_strAccountName.ReleaseBuffer();
				m_strDomain.ReleaseBuffer();
				break;
			}

			case ERROR_NONE_MAPPED:
				m_strAccountName.Empty();
				m_strDomain.Empty();
				m_sidnameuse = SidTypeUnknown;
				break;

			default:
				ASSERT_(FALSE);
			}
		}
	}

	//SID* _GetPSID() const /*throw()*/;

	//BYTE m_buffer[SECURITY_MAX_SID_SIZE];
	SID* m_psid;
	//bool m_bValid; // true if the CSid has been given a value

	mutable SID_NAME_USE m_sidnameuse;
	mutable CString_ m_strAccountName;
	mutable CString_ m_strDomain;
	mutable CString_ m_strSid;

	CString_ m_strSystem;
};//class CSid

typedef std::list<CSid> SidLst;

#if !defined(SECURITY_PRINCIPAL_SELF_RID)
	#define SECURITY_PRINCIPAL_SELF_RID (0x0000000AL)
#endif
#if !defined(SECURITY_AUTHENTICATED_USER_RID)
	#define SECURITY_AUTHENTICATED_USER_RID (0x0000000BL)
#endif
#if !defined(SECURITY_RESTRICTED_CODE_RID)
	#define SECURITY_RESTRICTED_CODE_RID (0x0000000CL)
#endif
#if !defined(SECURITY_TERMINAL_SERVER_RID)
	#define SECURITY_TERMINAL_SERVER_RID (0x0000000DL)
#endif
#if !defined(SECURITY_NETWORK_SERVICE_RID)
	#define SECURITY_NETWORK_SERVICE_RID (0x00000014L)
#endif
#if !defined(DOMAIN_ALIAS_RID_RAS_SERVERS)
	#define DOMAIN_ALIAS_RID_RAS_SERVERS (0x00000229L)
#endif
#if !defined(DOMAIN_ALIAS_RID_PREW2KCOMPACCESS)
	#define DOMAIN_ALIAS_RID_PREW2KCOMPACCESS (0x0000022AL)
#endif

namespace security
{
	namespace Sids
	{

static const SID_IDENTIFIER_AUTHORITY 
	SecurityNullSidAuthority = SECURITY_NULL_SID_AUTHORITY
	,SecurityWorldSidAuthority = SECURITY_WORLD_SID_AUTHORITY
	,SecurityLocalSidAuthority = SECURITY_LOCAL_SID_AUTHORITY
	,SecurityCreatorSidAuthority = SECURITY_CREATOR_SID_AUTHORITY
	,SecurityNonUniqueAuthority = SECURITY_NON_UNIQUE_AUTHORITY
	,SecurityNTAuthority = SECURITY_NT_AUTHORITY
	;

// Universal
inline CSid Null() THROWANY
{
	return CSid(SecurityNullSidAuthority, 1, SECURITY_NULL_RID);
}
inline CSid World() THROWANY
{
	return CSid(SecurityWorldSidAuthority, 1, SECURITY_WORLD_RID);
}
inline CSid Local() THROWANY
{
	return CSid(SecurityLocalSidAuthority, 1, SECURITY_LOCAL_RID);
}
inline CSid CreatorOwner() THROWANY
{
	return CSid(SecurityCreatorSidAuthority, 1, SECURITY_CREATOR_OWNER_RID);
}
inline CSid CreatorGroup() THROWANY
{
	return CSid(SecurityCreatorSidAuthority, 1, SECURITY_CREATOR_GROUP_RID);
}
inline CSid CreatorOwnerServer() THROWANY
{
	return CSid(SecurityCreatorSidAuthority, 1, SECURITY_CREATOR_OWNER_SERVER_RID);
}
inline CSid CreatorGroupServer() THROWANY
{
	return CSid(SecurityCreatorSidAuthority, 1, SECURITY_CREATOR_GROUP_SERVER_RID);
}

// NT Authority
inline CSid Dialup() THROWANY
{
	return CSid(SecurityNTAuthority, 1, SECURITY_DIALUP_RID);
}
inline CSid Network() THROWANY
{
	return CSid(SecurityNTAuthority, 1, SECURITY_NETWORK_RID);
}
inline CSid Batch() THROWANY
{
	return CSid(SecurityNTAuthority, 1, SECURITY_BATCH_RID);
}
inline CSid Interactive() THROWANY
{
	return CSid(SecurityNTAuthority, 1, SECURITY_INTERACTIVE_RID);
}
inline CSid Service() THROWANY
{
	return CSid(SecurityNTAuthority, 1, SECURITY_SERVICE_RID);
}
inline CSid AnonymousLogon() THROWANY
{
	return CSid(SecurityNTAuthority, 1, SECURITY_ANONYMOUS_LOGON_RID);
}
inline CSid Proxy() THROWANY
{
	return CSid(SecurityNTAuthority, 1, SECURITY_PROXY_RID);
}
inline CSid ServerLogon() THROWANY
{
	return CSid(SecurityNTAuthority, 1, SECURITY_SERVER_LOGON_RID);
}
inline CSid Self() THROWANY
{
	return CSid(SecurityNTAuthority, 1, SECURITY_PRINCIPAL_SELF_RID);
}
inline CSid AuthenticatedUser() THROWANY
{
	return CSid(SecurityNTAuthority, 1, SECURITY_AUTHENTICATED_USER_RID);
}
inline CSid RestrictedCode() THROWANY
{
	return CSid(SecurityNTAuthority, 1, SECURITY_RESTRICTED_CODE_RID);
}
inline CSid TerminalServer() THROWANY
{
	return CSid(SecurityNTAuthority, 1, SECURITY_TERMINAL_SERVER_RID);
}
inline CSid System() THROWANY
{
	return CSid(SecurityNTAuthority, 1, SECURITY_LOCAL_SYSTEM_RID);
}
inline CSid NetworkService() THROWANY
{
	return CSid(SecurityNTAuthority, 1, SECURITY_NETWORK_SERVICE_RID);
}

// NT Authority\BUILTIN
inline CSid Admins() THROWANY
{
	return CSid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS);
}
inline CSid Users() THROWANY
{
	return CSid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_USERS);
}
inline CSid Guests() THROWANY
{
	return CSid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_GUESTS);
}
inline CSid PowerUsers() THROWANY
{
	return CSid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_POWER_USERS);
}
inline CSid AccountOps() THROWANY
{
	return CSid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ACCOUNT_OPS);
}
inline CSid SystemOps() THROWANY
{
	return CSid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_SYSTEM_OPS);
}
inline CSid PrintOps() THROWANY
{
	return CSid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_PRINT_OPS);
}
inline CSid BackupOps() THROWANY
{
	return CSid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_BACKUP_OPS);
}
inline CSid Replicator() THROWANY
{
	return CSid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_REPLICATOR);
}

inline CSid RasServers() THROWANY
{
	return CSid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_RAS_SERVERS);
}
inline CSid PreW2KAccess() THROWANY
{
	return CSid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_PREW2KCOMPACCESS);
}

inline CSid* get_currentUser()
{
	return CSid::get_currentUser();
}


	};//namespace Sids
};//namespace security

class CAce
{
public:
	CAce(const CSid& _rSid, ACCESS_MASK _accessmask, BYTE _aceflags) THROWANY
		:m_dwAccessMask(_accessmask)
		,m_sid(_rSid)
		,m_aceflags(_aceflags)
	{
	}

	virtual ~CAce() THROW0
	{
		free();
	}

	CAce(const CAce& _rhs) THROWANY
		:m_sid(_rhs.m_sid)
		,m_dwAccessMask(_rhs.m_dwAccessMask)
		,m_aceflags(_rhs.m_aceflags)
	{
	}

	CAce& operator=(const CAce& _rhs) THROWANY
	{
		if(NEQL(&_rhs,this))
		{
			free();
			m_sid = _rhs.m_sid;
			m_dwAccessMask = _rhs.m_dwAccessMask;
			m_aceflags = _rhs.m_aceflags;
		}
		return *this;
	}

	virtual void* GetACE() const THROWANY = 0;
	virtual long GetLength() const THROW0 = 0;
	virtual BYTE AceType() const THROW0 = 0;

	virtual bool IsObjectAce() const THROW0
	{
		return false;
	}

	virtual GUID ObjectType() const THROW0
	{
		return GUID_NULL;
	}

	virtual GUID InheritedObjectType() const THROW0
	{
		return GUID_NULL;
	}

	ACCESS_MASK AccessMask() const THROW0
	{
		return m_dwAccessMask; 
	}

	BYTE AceFlags() const THROW0
	{
		return m_aceflags;
	}

	const CSid& Sid() const THROW0
	{
		return m_sid;
	}

	void AddAccess(ACCESS_MASK _accessmask) THROW0
	{
		m_dwAccessMask |= _accessmask;
		free();
	}

protected:
	void free() THROW0
	{
		m_pAce.free();
	}

protected:
	CSid m_sid;
	ACCESS_MASK m_dwAccessMask;
	BYTE m_aceflags;
	mutable utils::CTmpBuffer<void> m_pAce;
};//class CAce


typedef std::vector<ACCESS_MASK> AccessMaskVec;
typedef std::vector<BYTE> AceTypeVec;
typedef std::vector<BYTE> AceFlagVec;

class CAcl
{
public:
	CAcl() THROW0
		:m_dwAclRevision(ACL_REVISION)
		,m_bNull(false)
	{
	}

	virtual ~CAcl() THROW0
	{
		free();
	}

	CAcl(const CAcl& _acl) THROWANY
		:m_dwAclRevision(_acl.m_dwAclRevision)
		,m_bNull(_acl.m_bNull)
	{
	}

	CAcl& operator=(const CAcl& _acl) THROWANY
	{
		if(this!=&_acl)
		{
			free();
			m_dwAclRevision = _acl.m_dwAclRevision;
			m_bNull = _acl.m_bNull;
		}
		return *this;
	}

	void GetAclEntries(
		SidLst& _sids
		,AccessMaskVec* _pAccessMasks = NULL
		,AceTypeVec* _pAceTypes = NULL
		,AceFlagVec* _pAceFlags = NULL
		) const THROWANY
	{
		_sids.clear();
		if(_pAccessMasks) _pAccessMasks->clear();
		if(_pAceTypes) _pAceTypes->clear();
		if(_pAceFlags) _pAceFlags->clear();

		const CAce* pace = NULL;
		const long nCount = GetAceCount();
		long i = 0;
		for(i=0;i<nCount;i++)
		{
			pace = GetAce(i);

			_sids.push_back(pace->Sid());
			if(_pAccessMasks) _pAccessMasks->push_back(pace->AccessMask());
			if(_pAceTypes) _pAceTypes->push_back(pace->AceType());
			if(_pAceFlags) _pAceFlags->push_back(pace->AceFlags());
		}
	}

	void GetAclEntry(
		long _nIndex
		,CSid* _pSid
		,ACCESS_MASK* _pMask = NULL
		,BYTE* _pType = NULL
		,BYTE* _pFlags = NULL
		,GUID* _pObjectType = NULL
		,GUID* _pInheritedObjectType = NULL
		) const THROWANY
	{
		const CAce* pAce = GetAce(_nIndex);

		if(_pSid)
			*_pSid = pAce->Sid();
		if(_pMask)
			*_pMask = pAce->AccessMask();
		if(_pType)
			*_pType = pAce->AceType();
		if(_pFlags)
			*_pFlags = pAce->AceFlags();
		if(_pObjectType)
			*_pObjectType = pAce->ObjectType();
		if(_pInheritedObjectType)
			*_pInheritedObjectType = pAce->InheritedObjectType();
	}

	bool RemoveAces(const CSid& _rSid) THROWANY
	{
		VERIFY_EXIT1(_rSid.IsValid(),false);

		if(IsNull()) return false;

		bool bret = false;
		const CAce* pace = NULL;
		long i = 0;
		for(i=0;i<GetAceCount();)
		{
			pace = GetAce(i);
			if(EQL(_rSid,pace->Sid()))
			{
				RemoveAce(i);
				bret = true;
			}
			else i++;
		}
		if(bret) Dirty();
		return bret;
	}

	virtual long GetAceCount() const THROW0 = 0;
	virtual void RemoveAllAces() THROW0 = 0;
	virtual void RemoveAce(long _nIndex)  = 0;

	const ACL* GetPACL() const THROWANY
	{
		if(NOT_NULL(m_pAcl) && !m_bNull) return m_pAcl;

		UINT nAclLength = sizeof(ACL);
		const CAce* pAce = NULL;
		long i = 0;
		long nCount = GetAceCount();

		for(i=0;i<nCount;i++)
		{
			pAce = GetAce(i);
			VERIFY_DO(NOT_NULL(pAce),continue);
			nAclLength += pAce->GetLength();
		}

		m_pAcl.allocate(nAclLength);

		if(!::InitializeAcl(m_pAcl,(DWORD)nAclLength,m_dwAclRevision))
		{
			DWORD dwErr = ::GetLastError();
			m_pAcl.free();
			throw_win32Error(dwErr);
		}
		else
		{
			PrepareAcesForACL();

			VERIFY_DO(EQL(GetAceCount(),nCount),nCount=GetAceCount());

			for(i=0;i<nCount;i++)
			{
				pAce = GetAce(i);
				VERIFY_DO(NOT_NULL(pAce),continue);
				
				if(!::AddAce(m_pAcl,m_dwAclRevision,MAXDWORD,pAce->GetACE(),(DWORD) pAce->GetLength()))
				{
					DWORD dwErr = ::GetLastError();
					m_pAcl.free();
					throw_win32Error(dwErr);
				}
			}
		}
		return m_pAcl;
	}

	operator const ACL* () const THROWANY {return GetPACL();}

	long GetLength() const THROWANY
	{
		PACL pacl = const_cast<PACL>(GetPACL());
		ACL_SIZE_INFORMATION AclSize;

		VERIFY_EXIT1(NOT_NULL(pacl),0);

		if(!::GetAclInformation(pacl,&AclSize,sizeof(AclSize), AclSizeInformation))
			throw_lastWin32Error();

		return AclSize.AclBytesInUse;
	}

	void SetNull() THROW0
	{
		RemoveAllAces();
		m_bNull = true;
	}

	void SetEmpty() THROW0
	{
		RemoveAllAces();
		m_bNull = false;
	}

	bool IsNull() const THROW0
	{
		return m_bNull;
	}

	bool IsEmpty() const THROW0
	{
		return !m_bNull && EQL(0,GetAceCount());
	}

private:
	mutable utils::CTmpBuffer<ACL> m_pAcl;
	bool m_bNull;

protected:
	void free() THROW0
	{
		m_pAcl.free();
	}

	void Dirty() THROW0
	{
		free();
	}

	virtual const CAce *GetAce(long _nIndex) const = 0;

	virtual void PrepareAcesForACL() const THROW0
	{
	}

	DWORD m_dwAclRevision;
};//class CAcl

class CDacl;

class CAccessAce : public CAce
{
public:
	CAccessAce(const CSid& _rSid, ACCESS_MASK _accessmask, BYTE _aceflags, bool _bAllowAccess) THROWANY
		:CAce(_rSid,_accessmask,_aceflags)
		,m_bAllow(_bAllowAccess)
	{
	}

	~CAccessAce() THROW0
	{
	}

	void* GetACE() const THROWANY
	{
		VERIFY_EXIT1(EQL(sizeof(ACCESS_ALLOWED_ACE),sizeof(ACCESS_DENIED_ACE)),NULL);
		VERIFY_EXIT1(EQL(offsetof(ACCESS_ALLOWED_ACE, Header),offsetof(ACCESS_DENIED_ACE, Header)),NULL);
		VERIFY_EXIT1(EQL(offsetof(ACCESS_ALLOWED_ACE, Mask),offsetof(ACCESS_DENIED_ACE, Mask)),NULL);
		VERIFY_EXIT1(EQL(offsetof(ACCESS_ALLOWED_ACE, SidStart),offsetof(ACCESS_DENIED_ACE, SidStart)),NULL);

		if(!m_pAce)
		{
			long nLength = GetLength();
			utils::CTmpBuffer<ACCESS_ALLOWED_ACE> pAce;
			pAce.allocate(nLength);
			
			pAce->Header.AceSize = static_cast<WORD>(nLength);
			pAce->Header.AceFlags = m_aceflags;
			pAce->Header.AceType = AceType();

			pAce->Mask = m_dwAccessMask;
			VERIFY_EXIT1(nLength-offsetof(ACCESS_ALLOWED_ACE, SidStart) >= m_sid.get_length(),NULL);
			if(!utils::checked_memcpy(&pAce->SidStart, nLength-offsetof(ACCESS_ALLOWED_ACE, SidStart), m_sid.GetPSID(), m_sid.get_length())) return NULL;

			m_pAce.attach(static_cast<void*>(pAce.detach()));
		}
		return m_pAce;
	}

	long GetLength() const THROW0
	{
		return offsetof(ACCESS_ALLOWED_ACE, SidStart) + m_sid.get_length();
	}

	BYTE AceType() const THROW0
	{
		return (BYTE)(m_bAllow ? ACCESS_ALLOWED_ACE_TYPE : ACCESS_DENIED_ACE_TYPE);
	}

	bool Allow() const THROW0
	{
		return m_bAllow;
	}

	bool Inherited() const THROW0
	{
		return 0 != (m_aceflags & INHERITED_ACE);
	}

	static int Order(const CAccessAce& _left_ace, const CAccessAce& _right_ace) THROW0
	{
		// The order is:
		// denied direct aces
		// denied direct object aces
		// allowed direct aces
		// allowed direct object aces
		// denied inherit aces
		// denied inherit object aces
		// allowed inherit aces
		// allowed inherit object aces

		// inherited aces are always "greater" than non-inherited aces
		if(_left_ace.Inherited() && !_right_ace.Inherited())
			return -1;
		if(!_left_ace.Inherited() && _right_ace.Inherited())
			return 1;

		// if the aces are *both* either inherited or non-inherited, continue...

		// allowed aces are always "greater" than denied aces (subject to above)
		if(_left_ace.Allow() && !_right_ace.Allow())
			return -1;
		if(!_left_ace.Allow() && _right_ace.Allow())
			return 1;

		// if the aces are *both* either allowed or denied, continue...

		// object aces are always "greater" than non-object aces (subject to above)
		if(_left_ace.IsObjectAce() && !_right_ace.IsObjectAce())
			return -1;
		if(!_left_ace.IsObjectAce() && _right_ace.IsObjectAce())
			return 1;

		// aces are "equal" (e.g., both are access denied inherited object aces)
		return 0;
	}

	bool operator==(const CAccessAce& _) const THROW0
	{
		return !Order(*this,_);
	}

	bool operator<(const CAccessAce& _) const THROW0
	{
		return Order(*this,_)<0;
	}

	bool operator!=(const CAccessAce& _) const THROW0
	{
		return to_bool(Order(*this,_));
	}

	bool operator>(const CAccessAce& _) const THROW0
	{
		return Order(*this,_)>0;
	}

protected:
	bool m_bAllow;
};//class CAccessAce


#if(_WIN32_WINNT >= 0x0500)
class CAccessObjectAce : public CAccessAce
{
public:
	CAccessObjectAce(
		const CSid& _rSid
		,ACCESS_MASK _accessmask
		,BYTE _aceflags
		,bool _bAllowAccess
		,const GUID* _pObjectType
		,const GUID* _pInheritedObjectType
		) THROWANY
		:CAccessAce(_rSid, _accessmask, _aceflags, _bAllowAccess)
		,m_pObjectType(NULL)
		,m_pInheritedObjectType(NULL)
	{
		if(NOT_NULL(_pObjectType))
			m_pObjectType = trace_alloc(new utils::CGuid(*_pObjectType));
		if(NOT_NULL(_pInheritedObjectType))
			m_pInheritedObjectType = trace_alloc(new utils::CGuid(*_pInheritedObjectType));
	}

	~CAccessObjectAce() THROW0
	{
		if(NOT_NULL(m_pObjectType)) delete trace_free(m_pObjectType);
		if(NOT_NULL(m_pInheritedObjectType)) delete trace_free(m_pInheritedObjectType);
	}

	CAccessObjectAce(const CAccessObjectAce& _accessObjectAce) THROWANY
		:CAccessAce(_accessObjectAce)
		,m_pObjectType(NULL)
		,m_pInheritedObjectType(NULL)
	{
		*this = _accessObjectAce;
	}

	CAccessObjectAce& operator=(const CAccessObjectAce& _accessObjectAce) THROWANY
	{
		if(this!=&_accessObjectAce)
		{
			CAccessAce::operator=(_accessObjectAce);

			if(NOT_NULL(_accessObjectAce.m_pObjectType))
			{
				if(IS_NULL(m_pObjectType)) m_pObjectType = trace_alloc(new utils::CGuid());
				*m_pObjectType = *_accessObjectAce.m_pObjectType;
			}
			else
			{
				if(NOT_NULL(m_pObjectType)) delete trace_free(m_pObjectType);
				m_pObjectType = NULL;
			}

			if(NOT_NULL(_accessObjectAce.m_pInheritedObjectType))
			{
				if(IS_NULL(m_pInheritedObjectType)) m_pInheritedObjectType = trace_alloc(new utils::CGuid());
				*m_pInheritedObjectType = *_accessObjectAce.m_pInheritedObjectType;
			}
			else
			{
				if(NOT_NULL(m_pInheritedObjectType)) delete trace_free(m_pInheritedObjectType);
				m_pInheritedObjectType = NULL;
			}
		}
		return *this;
	}

	void* GetACE() const THROWANY
	{
		VERIFY_EXIT1(EQL(sizeof(ACCESS_ALLOWED_OBJECT_ACE),sizeof(ACCESS_DENIED_OBJECT_ACE)),NULL);
		VERIFY_EXIT1(EQL(offsetof(ACCESS_ALLOWED_OBJECT_ACE, Header),offsetof(ACCESS_DENIED_OBJECT_ACE, Header)),NULL);
		VERIFY_EXIT1(EQL(offsetof(ACCESS_ALLOWED_OBJECT_ACE, Mask),offsetof(ACCESS_DENIED_OBJECT_ACE, Mask)),NULL);
		VERIFY_EXIT1(EQL(offsetof(ACCESS_ALLOWED_OBJECT_ACE, Flags),offsetof(ACCESS_DENIED_OBJECT_ACE, Flags)),NULL);
		VERIFY_EXIT1(EQL(offsetof(ACCESS_ALLOWED_OBJECT_ACE, ObjectType),offsetof(ACCESS_DENIED_OBJECT_ACE, ObjectType)),NULL);
		VERIFY_EXIT1(EQL(offsetof(ACCESS_ALLOWED_OBJECT_ACE, InheritedObjectType),offsetof(ACCESS_DENIED_OBJECT_ACE, InheritedObjectType)),NULL);
		VERIFY_EXIT1(EQL(offsetof(ACCESS_ALLOWED_OBJECT_ACE, SidStart),offsetof(ACCESS_DENIED_OBJECT_ACE, SidStart)),NULL);

		if(!m_pAce)
		{
			long nLength = GetLength();
			utils::CTmpBuffer<ACCESS_ALLOWED_OBJECT_ACE> pAce(nLength);

			pAce->Header.AceSize = static_cast<WORD>(nLength);
			pAce->Header.AceFlags = m_aceflags;
			pAce->Header.AceType = AceType();

			pAce->Mask = m_dwAccessMask;
			pAce->Flags = 0;

			BYTE* pb = (reinterpret_cast<BYTE*>(pAce.get())) + offsetof(ACCESS_ALLOWED_OBJECT_ACE, SidStart);
			if(!m_pObjectType)
				pb -= sizeof(GUID);
			else
			{
				pAce->ObjectType = *m_pObjectType;
				pAce->Flags |= ACE_OBJECT_TYPE_PRESENT;
			}

			if(!m_pInheritedObjectType)
				pb -= sizeof(GUID);
			else
			{				if(m_pObjectType)
					pAce->InheritedObjectType = *m_pInheritedObjectType;
				else
					pAce->ObjectType = *m_pInheritedObjectType;
				pAce->Flags |= ACE_INHERITED_OBJECT_TYPE_PRESENT;
			}

			VERIFY_EXIT1((UINT)(pb - reinterpret_cast<BYTE *>(pAce.get())) >= m_sid.get_length(),NULL);
			utils::checked_memcpy(pb, (pb - reinterpret_cast<BYTE *>(pAce.get())), m_sid.GetPSID(), m_sid.get_length());

			m_pAce.attach(static_cast<void*>(pAce.detach()));
		}

		return m_pAce;
	}

	long GetLength() const THROW0
	{
		long nLength = offsetof(ACCESS_ALLOWED_OBJECT_ACE, SidStart);

		if(!m_pObjectType)
			nLength -= sizeof(GUID);
		if(!m_pInheritedObjectType)
			nLength -= sizeof(GUID);

		nLength += m_sid.get_length();

		return nLength;
	}

	BYTE AceType() const THROW0
	{
		return (BYTE)(m_bAllow ? ACCESS_ALLOWED_OBJECT_ACE_TYPE : ACCESS_DENIED_OBJECT_ACE_TYPE);
	}

	bool IsObjectAce() const THROW0
	{
		return true;
	}

	virtual GUID ObjectType() const THROW0
	{
		return NOT_NULL(m_pObjectType) ? *m_pObjectType : GUID_NULL;
	}

	virtual GUID InheritedObjectType() const THROW0
	{
		return NOT_NULL(m_pInheritedObjectType) ? *m_pInheritedObjectType : GUID_NULL;
	}

protected:
	utils::CGuid* m_pObjectType;
	utils::CGuid* m_pInheritedObjectType;
};//class CAccessObjectAce
#endif

struct op_SortAccessAce
{
	bool operator()(const auto_ptrex<CAccessAce>& _accessace1,const auto_ptrex<CAccessAce>& _accessace2) const
	{
		return CAccessAce::Order(*_accessace1,*_accessace2)>0;
	}
};//struct op_SortAccessAce

class CDacl : public CAcl
{
	typedef std::list<auto_ptrex<CAccessAce> > AcePtrsVec;
public:
	CDacl() THROW0
	{
	}

	~CDacl() THROW0
	{
		CDacl::RemoveAllAces();
	}

	CDacl(const CDacl& _rhs) THROWANY
	{
		Copy(_rhs);
	}

	CDacl& operator=(const CDacl& _dacl) THROWANY
	{
		if(NEQL(this,&_dacl))
		{
			RemoveAllAces();
			Copy(_dacl);
		}
		return *this;
	}

	CDacl(const ACL& _acl) THROWANY
	{
		Copy(_acl);
	}

	CDacl& operator=(const ACL& _acl) THROWANY
	{
		RemoveAllAces();
		Copy(_acl);
		return *this;
	}

	bool AddAllowedAce(const CSid& _rSid, ACCESS_MASK _accessmask, BYTE _aceflags = 0) THROWANY
	{
		VERIFY_EXIT1(_rSid.IsValid(),false);
		if(IsNull()) SetEmpty();
		
		auto_ptrex<CAccessAce> ptr(trace_alloc(new CAccessAce(_rSid,_accessmask,_aceflags,true)));
		m_acl.push_back(ptr);

		Dirty();

		return true;
	}

	bool AddDeniedAce(const CSid& _rSid, ACCESS_MASK _accessmask, BYTE _aceflags = 0) THROWANY
	{
		VERIFY_EXIT1(_rSid.IsValid(),false);
		if(IsNull()) SetEmpty();
		
		auto_ptrex<CAccessAce> ptr(trace_alloc(new CAccessAce(_rSid,_accessmask,_aceflags,false)));
		m_acl.push_back(ptr);

		Dirty();

		return true;
	}

#if(_WIN32_WINNT >= 0x0500)
	bool AddAllowedAce(
		const CSid& _rSid
		,ACCESS_MASK _accessmask
		,BYTE _aceflags
		,const GUID* _pObjectType
		,const GUID* _pInheritedObjectType
		) THROWANY
	{
		if(IS_NULL(_pObjectType) && IS_NULL(_pInheritedObjectType))
			return AddAllowedAce(_rSid,_accessmask,_aceflags);

		VERIFY_EXIT1(_rSid.IsValid(),false);

		if(IsNull()) SetEmpty();

		auto_ptrex<CAccessAce> ptr(trace_alloc(new CAccessObjectAce(_rSid,_accessmask,_aceflags,true
				,_pObjectType,_pInheritedObjectType
				))
			);
		m_acl.push_back(ptr);

		m_dwAclRevision = ACL_REVISION_DS;
		Dirty();
		return true;
	}

	bool AddDeniedAce(
		const CSid& _rSid
		,ACCESS_MASK _accessmask
		,BYTE _aceflags
		,const GUID* _pObjectType
		,const GUID* _pInheritedObjectType
		) THROWANY
	{
		if(IS_NULL(_pObjectType) && IS_NULL(_pInheritedObjectType))
			return AddAllowedAce(_rSid,_accessmask,_aceflags);

		VERIFY_EXIT1(_rSid.IsValid(),false);

		if(IsNull()) SetEmpty();

		auto_ptrex<CAccessAce> ptr = auto_ptrex<CAccessAce>(
				trace_alloc(new CAccessObjectAce(_rSid,_accessmask,_aceflags,false
				,_pObjectType,_pInheritedObjectType
				))
			);
		m_acl.push_back(ptr);

		m_dwAclRevision = ACL_REVISION_DS;
		Dirty();
		return true;
	}
#endif
	void RemoveAllAces() THROW0
	{
		m_acl.clear();
		Dirty();
	}

	void RemoveAce(long _nIndex) 
	{
		VERIFY_EXIT(_nIndex<0 || _nIndex>=m_acl.size());
		AcePtrsVec::iterator it = m_acl.begin();
		std::advance(it,_nIndex);
		m_acl.erase(it);
	}

	long GetAceCount() const THROW0
	{
		return m_acl.size();
	}

private:
	void Copy(const CDacl& _dacl) THROWANY
	{
		CSid sid;
		ACCESS_MASK accessmask = 0;
		BYTE type = 0;
		BYTE flags = 0;
		utils::CGuid guidType;
		utils::CGuid guidInheritedType;
		auto_ptrex<CAccessAce> pAce;

		Dirty();

		if(_dacl.IsNull()) SetNull();
		else SetEmpty();

		m_dwAclRevision = _dacl.m_dwAclRevision;

		for(long i = 0;i<_dacl.GetAceCount(); i++)
		{
			_dacl.GetAclEntry(i,&sid,&accessmask,&type,&flags,&guidType,&guidInheritedType);
			switch(type)
			{
			case ACCESS_ALLOWED_ACE_TYPE:
			case ACCESS_DENIED_ACE_TYPE:
				pAce = auto_ptrex<CAccessAce>(trace_alloc(new CAccessAce(sid,accessmask,flags,EQL(ACCESS_ALLOWED_ACE_TYPE,type))));
				m_acl.push_back(pAce);
				break;

#if(_WIN32_WINNT >= 0x0500)
			case ACCESS_ALLOWED_OBJECT_ACE_TYPE:
			case ACCESS_DENIED_OBJECT_ACE_TYPE:
			{
				GUID *pguidType = NULL;
				GUID *pguidInheritedType = NULL;
				if(guidType != GUID_NULL)
				{
					pguidType = &guidType;
				}

				if(guidInheritedType != GUID_NULL)
				{
					pguidInheritedType = &guidInheritedType;
				}

				pAce = auto_ptrex<CAccessAce>(trace_alloc(new CAccessObjectAce(
							sid
							,accessmask
							,flags
							,ACCESS_ALLOWED_OBJECT_ACE_TYPE == type
							,pguidType
							,pguidInheritedType
							))
						);
				m_acl.push_back(pAce);
				break;
			}
#endif
			default:
				//wrong ACE type
				ASSERT_(FALSE);
			}
		}
	}

	void Copy(const ACL& _acl) THROWANY
	{
		ACL* pAcl = const_cast<ACL*>(&_acl);
		if(IS_NULL(pAcl))
		{
			SetNull();
			return;
		}
		utils::CAclSizeInformation aclsizeinfo;
		utils::CAclRevisionInforamtion aclrevisioninfo;
		utils::CAceHeader* pHeader = NULL;
		CSid sid;
		ACCESS_MASK accessmask = 0;
		auto_ptrex<CAccessAce> pAce;

		Dirty();

		if(!::GetAclInformation(pAcl, &aclsizeinfo, sizeof(aclsizeinfo), AclSizeInformation))
			throw_lastWin32Error();

		if(!::GetAclInformation(pAcl, &aclrevisioninfo, sizeof(aclrevisioninfo), AclRevisionInformation))
			throw_lastWin32Error();

		m_dwAclRevision = aclrevisioninfo.AclRevision;

		for(long i = 0 ; i<aclsizeinfo.AceCount ; i++)
		{
			if(!::GetAce(pAcl, i, reinterpret_cast<void **>(&pHeader)))
				throw_lastWin32Error();

			accessmask = *(
				reinterpret_cast<ACCESS_MASK*>(
					reinterpret_cast<BYTE*>(pHeader) + sizeof(ACE_HEADER)
					)
				);

			switch(pHeader->AceType)
			{
			case ACCESS_ALLOWED_ACE_TYPE:
			case ACCESS_DENIED_ACE_TYPE:
				sid = reinterpret_cast<SID*>
						(
							reinterpret_cast<BYTE *>(pHeader) + sizeof(ACE_HEADER) + sizeof(ACCESS_MASK)
						);

				pAce = auto_ptrex<CAccessAce>(trace_alloc(new CAccessAce(sid, accessmask
						,pHeader->AceFlags
						,ACCESS_ALLOWED_ACE_TYPE == pHeader->AceType
						))
					);

				m_acl.push_back(pAce);
				break;

#if(_WIN32_WINNT >= 0x0500)
			case ACCESS_ALLOWED_OBJECT_ACE_TYPE:
			case ACCESS_DENIED_OBJECT_ACE_TYPE:
			{
				GUID *pObjectType = NULL, *pInheritedObjectType = NULL;
				BYTE *pb = reinterpret_cast<BYTE*>(pHeader) 
					+ offsetof(ACCESS_ALLOWED_OBJECT_ACE, SidStart)
					;

				DWORD dwFlags = reinterpret_cast<ACCESS_ALLOWED_OBJECT_ACE*>(pHeader)->Flags;

				if(dwFlags & ACE_OBJECT_TYPE_PRESENT)
				{
					pObjectType = reinterpret_cast<GUID*>
						(
							reinterpret_cast<BYTE *>(pHeader) +
							offsetof(ACCESS_ALLOWED_OBJECT_ACE, ObjectType)
						);
				}
				else
					pb -= sizeof(GUID);

				if(dwFlags & ACE_INHERITED_OBJECT_TYPE_PRESENT)
				{
					pInheritedObjectType = reinterpret_cast<GUID *>
						(
							reinterpret_cast<BYTE *>(pHeader) +
							(
								pObjectType?offsetof(ACCESS_ALLOWED_OBJECT_ACE, InheritedObjectType)
								:offsetof(ACCESS_ALLOWED_OBJECT_ACE, ObjectType)
							)
						);
				}
				else
					pb -= sizeof(GUID);

				sid = *reinterpret_cast<SID*>(pb);

				pAce = auto_ptrex<CAccessAce>(trace_alloc(new CAccessObjectAce(sid
							,accessmask
							,pHeader->AceFlags
							,ACCESS_ALLOWED_OBJECT_ACE_TYPE == pHeader->AceType
							,pObjectType,pInheritedObjectType
							))
						);
				m_acl.push_back(pAce);
				break;
			}
#endif

			default:
				// Wrong ACE type
				ASSERT_(false);
			}
			
		}
	}

	const CAce* GetAce(long _nIndex) const 
	{
		if(_nIndex<0 || _nIndex>=m_acl.size()) return NULL;
		AcePtrsVec::const_iterator it = m_acl.begin();
		std::advance(it,_nIndex);
		const auto_ptrex<CAccessAce>& tmp = *it;
		return &**it;
	}

	void PrepareAcesForACL() const THROW0
	{
		//std::sort(m_acl.begin(),m_acl.end(),op_SortAccessAce());
		m_acl.sort();
	}

	mutable AcePtrsVec m_acl;
};//class CDacl


class CAuditAce : public CAce
{
public:
	CAuditAce(
		const CSid& _rSid
		,ACCESS_MASK _accessmask
		,BYTE _aceflags
		,bool _bAuditSuccess
		,bool _bAuditFailure
		) THROWANY
		:CAce(_rSid, _accessmask, _aceflags)
		,m_bSuccess(_bAuditSuccess)
		,m_bFailure(_bAuditFailure)
	{
		if(m_bSuccess)
			m_aceflags |= SUCCESSFUL_ACCESS_ACE_FLAG;
		if(m_bFailure)
			m_aceflags |= FAILED_ACCESS_ACE_FLAG;
	}

	~CAuditAce() THROW0
	{
	}

	void* GetACE() const THROWANY
	{
		if(!m_pAce)
		{
			long nLength = GetLength();
			utils::CTmpBuffer<SYSTEM_AUDIT_ACE> pAce(nLength);

			pAce->Header.AceSize = static_cast<WORD>(nLength);
			pAce->Header.AceFlags = m_aceflags;
			pAce->Header.AceType = AceType();

			pAce->Mask = m_dwAccessMask;
			VERIFY_EXIT1(nLength-offsetof(SYSTEM_AUDIT_ACE, SidStart) >= m_sid.get_length(),NULL);
			utils::checked_memcpy(&pAce->SidStart, nLength-offsetof(SYSTEM_AUDIT_ACE, SidStart), m_sid.GetPSID(), m_sid.get_length());

			m_pAce = pAce;
		}
		return m_pAce;
	}

	long GetLength() const THROW0
	{
		return offsetof(SYSTEM_AUDIT_ACE, SidStart) + m_sid.get_length();
	}

	BYTE AceType() const THROW0
	{
		return SYSTEM_AUDIT_ACE_TYPE;
	}

	bool operator==(const CAuditAce& _auditace)
	{
		return EQL(static_cast<void*>(m_pAce),static_cast<void*>(_auditace.m_pAce));
	}

	bool operator<(const CAuditAce& _auditace)
	{
		return true; //ignore
	}

protected:
	bool m_bSuccess;
	bool m_bFailure;
};//class CAuditAce


#if(_WIN32_WINNT >= 0x0500)
class CAuditObjectAce : public CAuditAce
{
public:
	CAuditObjectAce(
		const CSid& _rSid
		,ACCESS_MASK _accessmask
		,BYTE _aceflags
		,bool _bAuditSuccess
		,bool _bAuditFailure
		,const GUID* _pObjectType
		,const GUID* _pInheritedObjectType
		) THROWANY
		:CAuditAce(_rSid, _accessmask, _aceflags, _bAuditSuccess, _bAuditFailure)
		,m_pObjectType(NULL)
		,m_pInheritedObjectType(NULL)
	{
		if(NOT_NULL(_pObjectType))
		{
			m_pObjectType = trace_alloc(new utils::CGuid(*_pObjectType));
		}
		if(NOT_NULL(_pInheritedObjectType))
		{
			m_pInheritedObjectType = trace_alloc(new utils::CGuid(*_pInheritedObjectType));
		}
	}

	~CAuditObjectAce() THROW0
	{
		if(NOT_NULL(m_pObjectType)) delete trace_free(m_pObjectType);
		if(NOT_NULL(m_pInheritedObjectType)) delete trace_free(m_pInheritedObjectType);
	}

	CAuditObjectAce(const CAuditObjectAce& _auditobjectace) THROWANY
		:CAuditAce(_auditobjectace)
		,m_pObjectType(NULL)
		,m_pInheritedObjectType(NULL)
	{
		*this = _auditobjectace;
	}

	CAuditObjectAce& operator=(const CAuditObjectAce& _auditobjectace) THROWANY
	{
		if(this != &_auditobjectace)
		{
			CAuditAce::operator=(_auditobjectace);

			if(NOT_NULL(_auditobjectace.m_pObjectType))
			{
				if(IS_NULL(m_pObjectType)) m_pObjectType = trace_alloc(new utils::CGuid());
				*m_pObjectType = *_auditobjectace.m_pObjectType;
			}
			else
			{
				if(NOT_NULL(m_pObjectType)) delete trace_free(m_pObjectType);
				m_pObjectType = NULL;
			}

			if(NOT_NULL(_auditobjectace.m_pInheritedObjectType))
			{
				if(IS_NULL(m_pInheritedObjectType))	m_pInheritedObjectType = trace_alloc(new utils::CGuid());
				*m_pInheritedObjectType = *_auditobjectace.m_pInheritedObjectType;
			}
			else
			{
				if(NOT_NULL(m_pInheritedObjectType)) delete trace_free(m_pInheritedObjectType);
				m_pInheritedObjectType = NULL;
			}
		}
		return *this;
	}

	void* GetACE() const THROWANY
	{
		if(m_pAce) return m_pAce;

		long nLength = GetLength();
		utils::CTmpBuffer<SYSTEM_AUDIT_OBJECT_ACE> pAce(nLength);

		pAce->Header.AceType = SYSTEM_AUDIT_OBJECT_ACE_TYPE;
		pAce->Header.AceSize = static_cast<WORD>(nLength);
		pAce->Header.AceFlags = m_aceflags;

		pAce->Mask = m_dwAccessMask;
		pAce->Flags = 0;

		if(m_bSuccess)
			pAce->Header.AceFlags |= SUCCESSFUL_ACCESS_ACE_FLAG;
		else
			pAce->Header.AceFlags &= ~SUCCESSFUL_ACCESS_ACE_FLAG;

		if(m_bFailure)
			pAce->Header.AceFlags |= FAILED_ACCESS_ACE_FLAG;
		else
			pAce->Header.AceFlags &= ~FAILED_ACCESS_ACE_FLAG;

		BYTE *pb = ((BYTE *) pAce.get()) + offsetof(SYSTEM_AUDIT_OBJECT_ACE, SidStart);

		if(!m_pObjectType) pb -= sizeof(GUID);
		else
		{
			pAce->ObjectType = *m_pObjectType;
			pAce->Flags |= ACE_OBJECT_TYPE_PRESENT;
		}

		if(!m_pInheritedObjectType)
			pb -= sizeof(GUID);
		else
		{
			if(m_pObjectType) pAce->InheritedObjectType = *m_pInheritedObjectType;
			else pAce->ObjectType = *m_pInheritedObjectType;

			pAce->Flags |= ACE_INHERITED_OBJECT_TYPE_PRESENT;
		}
		VERIFY_EXIT1(UINT(pb - reinterpret_cast<BYTE*>(pAce.get())) >= m_sid.get_length(),NULL);
		utils::checked_memcpy(pb, pb - reinterpret_cast<BYTE*>(pAce.get()), m_sid.GetPSID(), m_sid.get_length());
		m_pAce = pAce;

		return m_pAce;
	}

	long GetLength() const THROW0
	{
		long nLength = offsetof(SYSTEM_AUDIT_OBJECT_ACE, SidStart);

		if(!m_pObjectType) nLength -= sizeof(GUID);
		if(!m_pInheritedObjectType) nLength -= sizeof(GUID);

		nLength += m_sid.get_length();

		return nLength;
	}

	BYTE AceType() const THROW0
	{
		return SYSTEM_AUDIT_OBJECT_ACE_TYPE;
	}

	bool IsObjectAce() const THROW0
	{
		return true;
	}

	virtual GUID ObjectType() const THROW0
	{
		return m_pObjectType ? *m_pObjectType : GUID_NULL;
	}

	virtual GUID InheritedObjectType() const THROW0
	{
		return m_pInheritedObjectType ? *m_pInheritedObjectType : GUID_NULL;
	}

protected:
	utils::CGuid* m_pObjectType;
	utils::CGuid* m_pInheritedObjectType;
};//class CAuditObjectAce
#endif

/*****************************************
// CSacl
*****************************************/

class CSacl : public CAcl
{
	typedef std::vector<auto_ptrex<CAuditAce> > AuditAcePtrsVec;
public:

	CSacl() THROW0
	{
	}

	~CSacl() THROW0
	{
		CSacl::RemoveAllAces();
	}

	CSacl(const CSacl& _sacl) THROWANY
	{
		Copy(_sacl);
	}

	CSacl& operator=(const CSacl& _sacl) THROWANY
	{
		if(NEQL(this,&_sacl))
		{
			RemoveAllAces();
			Copy(_sacl);
		}

		return *this;
	}

	CSacl(const ACL& _acl) THROWANY
	{
		Copy(_acl);
	}

	CSacl& operator=(const ACL& _acl) THROWANY
	{
		RemoveAllAces();
		Copy(_acl);
		return *this;
	}

	bool AddAuditAce(
		const CSid &_rSid
		,ACCESS_MASK _accessmask
		,bool _bSuccess
		,bool _bFailure
		,BYTE _aceflags = 0
		) THROWANY
	{
		VERIFY_EXIT1(_rSid.IsValid(),false);

		if(IsNull()) SetEmpty();

		auto_ptrex<CAuditAce> pAce(trace_alloc(new CAuditAce(_rSid, _accessmask, _aceflags, _bSuccess, _bFailure)));
		m_acl.push_back(pAce);

		Dirty();
		return true;
	}

#if(_WIN32_WINNT >= 0x0500)
	bool AddAuditAce(
		const CSid& _rSid
		,ACCESS_MASK _accessmask
		,bool _bSuccess
		,bool _bFailure
		,BYTE _aceflags
		,const GUID* _pObjectType
		,const GUID* _pInheritedObjectType
		) THROWANY
	{
		if(IS_NULL(_pObjectType) && IS_NULL(_pInheritedObjectType))
			return AddAuditAce(_rSid, _accessmask, _bSuccess, _bFailure, _aceflags);

		VERIFY_EXIT1(_rSid.IsValid(),false);

		if(IsNull()) SetEmpty();

		auto_ptrex<CAuditAce> pAce(
			trace_alloc(new CAuditObjectAce(_rSid, _accessmask, _aceflags, _bSuccess
				,_bFailure, _pObjectType, _pInheritedObjectType
				))
			);
		m_acl.push_back(pAce);
		m_dwAclRevision = ACL_REVISION_DS;
		Dirty();
		return true;
	}
#endif

	void RemoveAllAces() THROW0
	{
		m_acl.clear();
		Dirty();
	}

	void RemoveAce(long _nIndex) 
	{
		if(_nIndex<0 || _nIndex>=m_acl.size()) return;
		AuditAcePtrsVec::iterator it = m_acl.begin();
		std::advance(it,_nIndex);
		m_acl.erase(it);
	}

	long GetAceCount() const THROW0
	{
		return m_acl.size();
	}

private:
	void Copy(const CSacl& _sacl) THROWANY
	{
		CSid sid;
		ACCESS_MASK accessmask;
		BYTE type;
		BYTE flags;
		utils::CGuid guidType;
		utils::CGuid guidInheritedType;
		bool bSuccess;
		bool bFailure;
		auto_ptrex<CAuditAce> pAce;

		Dirty();

		if(_sacl.IsNull()) SetNull();
		else SetEmpty();

		m_dwAclRevision = _sacl.m_dwAclRevision;

		long i = 0;
		for(i=0; i<_sacl.GetAceCount(); i++)
		{
			_sacl.GetAclEntry(i, &sid, &accessmask, &type, &flags, &guidType, &guidInheritedType);

			bSuccess = to_bool(flags & SUCCESSFUL_ACCESS_ACE_FLAG);
			bFailure = to_bool(flags & FAILED_ACCESS_ACE_FLAG);

			switch (type)
			{
			case SYSTEM_AUDIT_ACE_TYPE:
				pAce = auto_ptrex<CAuditAce>(trace_alloc(new CAuditAce(sid, accessmask, flags, bSuccess, bFailure)));
				m_acl.push_back(pAce);
				break;

#if(_WIN32_WINNT >= 0x0500)
			case SYSTEM_AUDIT_OBJECT_ACE_TYPE:
			{
				GUID* pguidType = NULL;
				GUID* pguidInheritedType = NULL;
				if(guidType != GUID_NULL)
				{
					pguidType = &guidType;
				}

				if(guidInheritedType != GUID_NULL)
				{
					pguidInheritedType = &guidInheritedType;
				}

				pAce = auto_ptrex<CAuditAce>(trace_alloc(new CAuditObjectAce(
							sid
							,accessmask
							,flags
							,bSuccess
							,bFailure
							,pguidType
							,pguidInheritedType
							))
						);
				m_acl.push_back(pAce);
				break;
			}
#endif
			default:
				// Wrong ACE type
				ASSERT_(FALSE);
			}
		}

	}

	void Copy(const ACL& _acl) THROWANY
	{
		ACL* pAcl = const_cast<ACL*>(&_acl);
		utils::CAclSizeInformation aclsizeinfo;
		utils::CAclRevisionInforamtion aclrevisioninfo;
		utils::CAceHeader* pHeader = NULL;
		CSid sid;
		ACCESS_MASK accessmask = 0;
		bool bSuccess = false, bFailure = true;
		auto_ptrex<CAuditAce> pAce;

		Dirty();

		if(!::GetAclInformation(pAcl, &aclsizeinfo, sizeof(aclsizeinfo), AclSizeInformation))
			throw_lastWin32Error();

		if(!::GetAclInformation(pAcl, &aclrevisioninfo, sizeof(aclrevisioninfo), AclRevisionInformation))
			throw_lastWin32Error();

		m_dwAclRevision = aclrevisioninfo.AclRevision;

		DWORD i =0;
		for(i = 0; i < aclsizeinfo.AceCount; i++)
		{
			if(!::GetAce(pAcl, i, reinterpret_cast<void **>(&pHeader)))
				throw_lastWin32Error();

			accessmask = *reinterpret_cast<ACCESS_MASK*>(
				reinterpret_cast<BYTE *>(pHeader) + sizeof(ACE_HEADER)
				);

			bSuccess = to_bool(pHeader->AceFlags & SUCCESSFUL_ACCESS_ACE_FLAG);
			bFailure = to_bool(pHeader->AceFlags & FAILED_ACCESS_ACE_FLAG);

			switch(pHeader->AceType)
			{
			case SYSTEM_AUDIT_ACE_TYPE:
				sid = reinterpret_cast<SID*>(
						reinterpret_cast<BYTE *>(pHeader) +	sizeof(ACE_HEADER) + sizeof(ACCESS_MASK)
						);

				pAce = auto_ptrex<CAuditAce>(trace_alloc(new CAuditAce(sid, accessmask, pHeader->AceFlags, bSuccess, bFailure)));
				m_acl.push_back(pAce);
				break;

#if(_WIN32_WINNT >= 0x0500)
			case SYSTEM_AUDIT_OBJECT_ACE_TYPE:
			{
				GUID *pObjectType = NULL, *pInheritedObjectType = NULL;

				BYTE *pb = reinterpret_cast<BYTE*>(pHeader) 
					+ offsetof(SYSTEM_AUDIT_OBJECT_ACE, SidStart)
					;

				DWORD dwFlags = reinterpret_cast<SYSTEM_AUDIT_OBJECT_ACE*>(pHeader)->Flags;

				if(dwFlags & ACE_OBJECT_TYPE_PRESENT)
				{
					pObjectType = reinterpret_cast<GUID *>(
						reinterpret_cast<BYTE *>(pHeader) 
						+ offsetof(SYSTEM_AUDIT_OBJECT_ACE, ObjectType)
						);
				}
				else
					pb -= sizeof(GUID);

				if(dwFlags & ACE_INHERITED_OBJECT_TYPE_PRESENT)
				{
					pInheritedObjectType = reinterpret_cast<GUID*>(
						reinterpret_cast<BYTE *>(pHeader) 
						+ (pObjectType ?
							offsetof(SYSTEM_AUDIT_OBJECT_ACE, InheritedObjectType) 
							:offsetof(SYSTEM_AUDIT_OBJECT_ACE, ObjectType)
							)
						);
				}
				else
					pb -= sizeof(GUID);

				sid = *reinterpret_cast<SID*>(pb);

				pAce = auto_ptrex<CAuditAce>(trace_alloc(new CAuditObjectAce(sid, accessmask, pHeader->AceFlags,
					bSuccess, bFailure, pObjectType, pInheritedObjectType)));
				m_acl.push_back(pAce);
				break;
			}
#endif
			default:
				// Wrong ACE type
				ASSERT_(false);
			}
		}

	}

	const CAce *GetAce(long _nIndex) const 
	{
		if(_nIndex<0 || _nIndex>=m_acl.size()) return NULL;
		AuditAcePtrsVec::const_iterator it = m_acl.begin();
		std::advance(it,_nIndex);
		return &**it;
	}

	AuditAcePtrsVec m_acl;
};//class CSacl

//******************************************
// CSecurityDesc

class CSecurityDesc
{
public:
	CSecurityDesc() THROW0
	{
	}

	virtual ~CSecurityDesc() THROW0
	{
		Clear();
	}

	CSecurityDesc(const CSecurityDesc& _securydescr) THROWANY
	{
		if(_securydescr.m_pSecurityDescriptor)
			Init(*_securydescr.m_pSecurityDescriptor);
	}

	CSecurityDesc& operator=(const CSecurityDesc& _securydescr) THROWANY
	{
		if(this!=&_securydescr)
		{
			Clear();
			if(_securydescr.m_pSecurityDescriptor)
				Init(*_securydescr.m_pSecurityDescriptor);
		}
		return *this;
	}

	CSecurityDesc(const SECURITY_DESCRIPTOR& _securydescr) THROWANY
	{
		Init(_securydescr);
	}

	CSecurityDesc& operator=(const SECURITY_DESCRIPTOR &_securydescr) THROWANY
	{
		if(m_pSecurityDescriptor != &_securydescr)
		{
			Clear();
			Init(_securydescr);
		}
		return *this;
	}

#if(_WIN32_WINNT >= 0x0500)
	bool FromString(LPCTSTR _pstr) THROWANY
	{
		SECURITY_DESCRIPTOR *pSD = NULL;
		if(!::ConvertStringSecurityDescriptorToSecurityDescriptor(
				_pstr
				,SDDL_REVISION_1
				,(PSECURITY_DESCRIPTOR*)&pSD
				,NULL
				)
			)
			throw_lastWin32Error();

		*this = *pSD;
		::LocalFree(pSD);

		return true;
	}

	bool ToString(
		CString_& _str
		,SECURITY_INFORMATION _si = OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION 
			| DACL_SECURITY_INFORMATION | SACL_SECURITY_INFORMATION
		) const THROWANY
	{
		if(IS_NULL(m_pSecurityDescriptor)) return false;

		LPTSTR pszStringSecurityDescriptor = NULL;
		if(!::ConvertSecurityDescriptorToStringSecurityDescriptor(
				m_pSecurityDescriptor
				,SDDL_REVISION_1
				,_si
				,&pszStringSecurityDescriptor
				,NULL
				)
			)
			throw_lastWin32Error();

		try
		{
			_str = pszStringSecurityDescriptor;
		}
		catch(...)
		{
			::LocalFree(pszStringSecurityDescriptor);
			throw;
		}

		::LocalFree(pszStringSecurityDescriptor);
		return true;
	}
#endif

	void SetOwner(const CSid& _sid, bool _bDefaulted = false) THROWANY
	{
		if(!_sid.IsValid()) throw_win32Error(ERROR_INVALID_PARAMETER);

		if(NOT_NULL(m_pSecurityDescriptor)) MakeAbsolute();

		PSID pOldOwner = NULL;
		if(m_pSecurityDescriptor)
		{
			BOOL bDefaulted = FALSE;
			if(!::GetSecurityDescriptorOwner(m_pSecurityDescriptor, &pOldOwner, &bDefaulted))
				throw_lastWin32Error();
		}
		else
		{
			AllocateAndInitializeSecurityDescriptor();
			pOldOwner = NULL;
		}

		long nSidLength = _sid.get_length();
		utils::CTmpBuffer<SID> pNewOwner(nSidLength);

		if(!::CopySid((DWORD) nSidLength, pNewOwner, static_cast<PSID>(const_cast<SID*>(_sid.GetPSID()))) 
			|| !::SetSecurityDescriptorOwner(m_pSecurityDescriptor, pNewOwner.detach(), _bDefaulted)
			)
		{
			throw_lastWin32Error();
		}

		utils::CTmpBuffer<SID> old((SID*)pOldOwner);
	}

	void SetGroup(const CSid& _sid, bool _bDefaulted = false) THROWANY
	{
		if(!_sid.IsValid()) throw_win32Error(ERROR_INVALID_PARAMETER);

		if(NOT_NULL(m_pSecurityDescriptor)) MakeAbsolute();

		PSID pOldGroup = NULL;
		if(NOT_NULL(m_pSecurityDescriptor))
		{
			BOOL bDefaulted = FALSE;
			if(!::GetSecurityDescriptorGroup(m_pSecurityDescriptor, &pOldGroup, &bDefaulted))
				throw_lastWin32Error();
		}
		else
		{
			AllocateAndInitializeSecurityDescriptor();
			pOldGroup = NULL;
		}

		long nSidLength = _sid.get_length();
		utils::CTmpBuffer<SID> pNewGroup(nSidLength);

		if(!::CopySid((DWORD) nSidLength, pNewGroup, static_cast<PSID>(const_cast<SID*>(_sid.GetPSID()))) 
			|| !::SetSecurityDescriptorGroup(m_pSecurityDescriptor, pNewGroup.detach(), _bDefaulted)
			)
		{
			throw_lastWin32Error();
		}

		utils::CTmpBuffer<SID> old((SID*)pOldGroup);
	}

	void SetDacl(const CDacl& _Dacl, bool _bDefaulted = false) THROWANY
	{
		if(NOT_NULL(m_pSecurityDescriptor)) MakeAbsolute();

		PACL pOldDacl = NULL;
		if(NOT_NULL(m_pSecurityDescriptor))
		{
			BOOL bDefaulted = FALSE, bPresent = FALSE;
			if(!::GetSecurityDescriptorDacl(m_pSecurityDescriptor, &bPresent, &pOldDacl, &bDefaulted))
				throw_lastWin32Error();
		}
		else
			AllocateAndInitializeSecurityDescriptor();

		utils::CTmpBuffer<ACL> pNewDacl;

		if(_Dacl.IsNull() || _Dacl.IsEmpty()) pNewDacl = NULL;
		else
		{
			long nAclLength = _Dacl.GetLength();
			ASSERT_(nAclLength>0);
			if(nAclLength>0)
			{
				pNewDacl.allocate(nAclLength);
	
				utils::checked_memcpy(
					static_cast<void*>(static_cast<PACL>(pNewDacl)), nAclLength
					,_Dacl.GetPACL(),nAclLength
					);
			}
		}

#ifdef _DEBUG
		if(_Dacl.IsNull())
		{
			// setting a NULL DACL is almost always the wrong thing to do
			TRACE_(_T("Error: Setting Dacl to Null offers no security\n"));
			ASSERT_(FALSE);
		}
#endif

		if(!::SetSecurityDescriptorDacl(m_pSecurityDescriptor, _Dacl.IsNull() || (bool)pNewDacl, pNewDacl.detach(), _bDefaulted))
		{
			throw_lastWin32Error();
		}

		utils::CTmpBuffer<ACL> old(pOldDacl);
	}

	void SetDacl(bool _bPresent, bool _bDefaulted = false) THROWANY
	{
		if(NOT_NULL(m_pSecurityDescriptor)) MakeAbsolute();

		PACL pOldDacl = NULL;
		if(NOT_NULL(m_pSecurityDescriptor))
		{
			BOOL bDefaulted = FALSE, bPresent = FALSE;
			if(!::GetSecurityDescriptorDacl(m_pSecurityDescriptor, &bPresent, &pOldDacl, &bDefaulted))
				throw_lastWin32Error();
		}
		else
			AllocateAndInitializeSecurityDescriptor();

#ifdef _DEBUG
		if(_bPresent)
		{
			// setting a NULL DACL is almost always the wrong thing to do
			TRACE_(_T("Error: Setting Dacl to Null offers no security\n"));
			ASSERT_(FALSE);
		}
#endif

		if(!::SetSecurityDescriptorDacl(m_pSecurityDescriptor, _bPresent, NULL, _bDefaulted))
			throw_lastWin32Error();

		utils::CTmpBuffer<ACL> old(pOldDacl);
	}

	void SetSacl(const CSacl& _sacl, bool _bDefaulted = false) THROWANY
	{
		if(NOT_NULL(m_pSecurityDescriptor)) MakeAbsolute();

		PACL pOldSacl = NULL;
		if(NOT_NULL(m_pSecurityDescriptor))
		{
			BOOL bDefaulted = FALSE, bPresent = FALSE;
			if(!::GetSecurityDescriptorSacl(m_pSecurityDescriptor, &bPresent, &pOldSacl, &bDefaulted))
				throw_lastWin32Error();
		}
		else
			AllocateAndInitializeSecurityDescriptor();

		utils::CTmpBuffer<ACL> pNewSacl;
		if(_sacl.IsNull() || _sacl.IsEmpty())
			pNewSacl = NULL;
		else
		{
			long nAclLength = _sacl.GetLength();
			ASSERT_(nAclLength>0);
			if(nAclLength>0)
			{
				pNewSacl.allocate(nAclLength);

				utils::checked_memcpy(static_cast<void*>(static_cast<ACL*>(pNewSacl)), nAclLength, _sacl.GetPACL(), nAclLength);
			}
		}

		if(!::SetSecurityDescriptorSacl(m_pSecurityDescriptor, _sacl.IsNull() || (bool)pNewSacl, pNewSacl.detach(), _bDefaulted))
		{
			throw_lastWin32Error();
		}

		utils::CTmpBuffer<ACL> old(pOldSacl);
	}

	bool GetOwner(CSid& _sid, bool* _pbDefaulted = NULL) const THROWANY
	{
		SID* pOwner = NULL;
		BOOL bDefaulted = FALSE;

		if(!m_pSecurityDescriptor 
			|| !::GetSecurityDescriptorOwner(m_pSecurityDescriptor, (PSID*) &pOwner, &bDefaulted)
			)
		{
			return false;
		}

		_sid = pOwner;

		if(_pbDefaulted)
			*_pbDefaulted = to_bool(bDefaulted);

		return true;
	}

	bool GetGroup(CSid& _sid, bool* _pbDefaulted = NULL) const THROWANY
	{
		SID* pGroup = NULL;
		BOOL bDefaulted = FALSE;

		if(!m_pSecurityDescriptor 
			|| !::GetSecurityDescriptorGroup(m_pSecurityDescriptor, (PSID*) &pGroup, &bDefaulted)
			)
		{
			return false;
		}

		_sid = pGroup;

		if(_pbDefaulted)
			*_pbDefaulted = to_bool(bDefaulted);

		return true;
	}

	bool GetDacl(CDacl* _pDacl, bool* _pbPresent = NULL, bool* _pbDefaulted = NULL) const THROWANY
	{
		ACL* pAcl = NULL;
		BOOL bPresent = FALSE, bDefaulted = FALSE;

		if(!m_pSecurityDescriptor 
			|| !::GetSecurityDescriptorDacl(m_pSecurityDescriptor, &bPresent, &pAcl, &bDefaulted)
			)
		{
			return false;
		}

		if(NOT_NULL(_pDacl))
		{
			if(bPresent)
			{
				if(NOT_NULL(pAcl))
					*_pDacl = *pAcl;
				else
					_pDacl->SetNull();
			}
			else
				_pDacl->SetEmpty();
		}

		if(_pbPresent)
			*_pbPresent = to_bool(bPresent);

		if(_pbDefaulted)
			*_pbDefaulted = to_bool(bDefaulted);

		return true;
	}

	bool GetSacl(CSacl* _pSacl, bool* _pbPresent = NULL, bool* _pbDefaulted = NULL) const THROWANY
	{
		ACL* pAcl = NULL;
		BOOL bPresent = FALSE, bDefaulted = FALSE;

		if(!m_pSecurityDescriptor 
			|| !::GetSecurityDescriptorSacl(m_pSecurityDescriptor, &bPresent, &pAcl, &bDefaulted)
			)
		{
			return false;
		}

		if(NOT_NULL(_pSacl))
		{
			if(bPresent)
			{
				if(NOT_NULL(pAcl))
					*_pSacl = *pAcl;
				else
					_pSacl->SetNull();
			}
			else
				_pSacl->SetEmpty();
		}

		if(_pbPresent)
			*_pbPresent = to_bool(bPresent);

		if(_pbDefaulted)
			*_pbDefaulted = to_bool(bDefaulted);

		return true;
	}

	bool IsDaclDefaulted() const THROW0
	{
		SECURITY_DESCRIPTOR_CONTROL sdc = 0;
		if(!GetControl(&sdc)) return false;
		return (sdc & SE_DACL_PRESENT) && (sdc & SE_DACL_DEFAULTED);
	}

	bool IsDaclPresent() const THROW0
	{
		SECURITY_DESCRIPTOR_CONTROL sdc = 0;
		if(!GetControl(&sdc)) return false;
		return to_bool(sdc & SE_DACL_PRESENT);
	}

	bool IsGroupDefaulted() const THROW0
	{
		SECURITY_DESCRIPTOR_CONTROL sdc = 0;
		if(!GetControl(&sdc)) return false;
		return to_bool(sdc & SE_GROUP_DEFAULTED);
	}

	bool IsOwnerDefaulted() const THROW0
	{
		SECURITY_DESCRIPTOR_CONTROL sdc = 0;
		if(!GetControl(&sdc)) return false;
		return to_bool(sdc & SE_OWNER_DEFAULTED);
	}

	bool IsSaclDefaulted() const THROW0
	{
		SECURITY_DESCRIPTOR_CONTROL sdc = 0;
		if(!GetControl(&sdc)) return false;
		return (sdc & SE_SACL_PRESENT) && (sdc & SE_SACL_DEFAULTED);
	}

	bool IsSaclPresent() const THROW0
	{
		SECURITY_DESCRIPTOR_CONTROL sdc = 0;
		if(!GetControl(&sdc)) return false;
		return to_bool(sdc & SE_SACL_PRESENT);
	}

	bool IsSelfRelative() const THROW0
	{
		SECURITY_DESCRIPTOR_CONTROL sdc = 0;
		if(!GetControl(&sdc)) return false;
		return to_bool(sdc & SE_SELF_RELATIVE);
	}

	// Only meaningful on Win2k or better
	bool IsDaclAutoInherited() const THROW0
	{
		SECURITY_DESCRIPTOR_CONTROL sdc = 0;
		if(!GetControl(&sdc)) return false;
		return to_bool(sdc & SE_DACL_AUTO_INHERITED);
	}

	bool IsDaclProtected() const THROW0
	{
		SECURITY_DESCRIPTOR_CONTROL sdc = 0;
		if(!GetControl(&sdc)) return false;
		return to_bool(sdc & SE_DACL_PROTECTED);
	}

	bool IsSaclAutoInherited() const THROW0
	{
		SECURITY_DESCRIPTOR_CONTROL sdc = 0;
		if(!GetControl(&sdc)) return false;
		return to_bool(sdc & SE_SACL_AUTO_INHERITED);
	}

	bool IsSaclProtected() const THROW0
	{
		SECURITY_DESCRIPTOR_CONTROL sdc = 0;
		if(!GetControl(&sdc)) return false;
		return to_bool(sdc & SE_SACL_PROTECTED);
	}

	const SECURITY_DESCRIPTOR* GetPSECURITY_DESCRIPTOR() const THROW0
	{
		return m_pSecurityDescriptor;
	}

	operator const SECURITY_DESCRIPTOR* () const THROW0
	{
		return GetPSECURITY_DESCRIPTOR();
	}

	bool GetSECURITY_DESCRIPTOR(SECURITY_DESCRIPTOR* _pSD, LPDWORD _lpdwBufferLength) THROWANY
	{
		VERIFY_EXIT1(_lpdwBufferLength>0,false);
		VERIFY_EXIT1(NOT_NULL(m_pSecurityDescriptor),false);

		MakeAbsolute();

		if(!::MakeSelfRelativeSD(m_pSecurityDescriptor, _pSD, _lpdwBufferLength))
		{	
			if(IS_NULL(_pSD) && EQL(GetLastError(),ERROR_INSUFFICIENT_BUFFER)) return false;
			throw_lastWin32Error();
		}
		return true;
	}

	long GetLength() THROW0
	{
		return ::GetSecurityDescriptorLength(m_pSecurityDescriptor);
	}

	bool GetControl(SECURITY_DESCRIPTOR_CONTROL* _psdc) const THROW0
	{
		VERIFY_EXIT1(NOT_NULL(_psdc),false);

		DWORD dwRev = 0;
		*_psdc = NULL;
		if(!m_pSecurityDescriptor 
			|| !::GetSecurityDescriptorControl(m_pSecurityDescriptor, _psdc, &dwRev)
			)
		{
			return false;
		}
		return true;
	}

#if(_WIN32_WINNT >= 0x0500)
	bool SetControl(
		SECURITY_DESCRIPTOR_CONTROL _ControlBitsOfInterest
		,SECURITY_DESCRIPTOR_CONTROL _ControlBitsToSet
		) THROW0
	{
		VERIFY_EXIT1(NOT_NULL(m_pSecurityDescriptor),false);

		return to_bool(
			::SetSecurityDescriptorControl(
				m_pSecurityDescriptor
				,_ControlBitsOfInterest
				,_ControlBitsToSet
				)
			);
	}
#endif

	void MakeSelfRelative() THROWANY
	{
		SECURITY_DESCRIPTOR_CONTROL sdc = 0;
		if(IS_NULL(m_pSecurityDescriptor)) return;

		if(!GetControl(&sdc)) throw_win32Error(ERROR_INVALID_HANDLE);

		if(sdc & SE_SELF_RELATIVE) return;

		DWORD dwLen = 0;

		::MakeSelfRelativeSD(m_pSecurityDescriptor, NULL, &dwLen);
		if(::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			throw_lastWin32Error();

		utils::CTmpBuffer<SECURITY_DESCRIPTOR> pSD(dwLen);

		if(!::MakeSelfRelativeSD(m_pSecurityDescriptor, pSD, &dwLen))
		{
			throw_lastWin32Error();
		}

		Clear();
		m_pSecurityDescriptor = pSD.detach();
	}

	void MakeAbsolute() THROWANY
	{
		SECURITY_DESCRIPTOR_CONTROL sdc = 0;
		if(IS_NULL(m_pSecurityDescriptor)) return;

		if(!GetControl(&sdc)) throw_win32Error(ERROR_INVALID_HANDLE);

		if(!(sdc & SE_SELF_RELATIVE)) return;

		DWORD dwSD = 0, dwOwner = 0, dwGroup = 0, dwDacl = 0, dwSacl = 0;

		::MakeAbsoluteSD(m_pSecurityDescriptor
			,NULL, &dwSD
			,NULL, &dwDacl
			,NULL, &dwSacl
			,NULL, &dwOwner
			,NULL, &dwGroup
			);
		if(::GetLastError()!=ERROR_INSUFFICIENT_BUFFER) throw_lastWin32Error();

		utils::CTmpBuffer<SECURITY_DESCRIPTOR> pSD(dwSD);
		utils::CTmpBuffer<SID> pOwner(dwOwner);
		utils::CTmpBuffer<SID> pGroup(dwGroup);
		utils::CTmpBuffer<ACL> pDacl(dwDacl);
		utils::CTmpBuffer<ACL> pSacl(dwSacl);

		if(!::MakeAbsoluteSD(m_pSecurityDescriptor
				,pSD, &dwSD
				,pDacl, &dwDacl
				,pSacl, &dwSacl
				,pOwner, &dwOwner
				,pGroup, &dwGroup
				)
			)
		{
			throw_lastWin32Error();
		}

		Clear();
		m_pSecurityDescriptor = pSD.detach();
		pOwner.detach();
		pGroup.detach();
		pDacl.detach();
		pSacl.detach();
	}

protected:
	virtual void Clear() THROW0
	{
		if(NOT_NULL(m_pSecurityDescriptor))
		{
			SECURITY_DESCRIPTOR_CONTROL sdc = 0;
			if(GetControl(&sdc) && !(sdc & SE_SELF_RELATIVE))
			{
				utils::CTmpBuffer<SID> pOwner,pGroup;
				utils::CTmpBuffer<ACL> pDacl,pSacl;
				BOOL bDefaulted = FALSE, bPresent = FALSE;

				::GetSecurityDescriptorOwner(m_pSecurityDescriptor, (PSID*)pOwner.get_bufferptr(), &bDefaulted);
				::GetSecurityDescriptorGroup(m_pSecurityDescriptor, (PSID*)pGroup.get_bufferptr(), &bDefaulted);
				::GetSecurityDescriptorDacl(m_pSecurityDescriptor, &bPresent, pDacl.get_bufferptr(), &bDefaulted);
				if(!bPresent) pDacl.detach();
				::GetSecurityDescriptorSacl(m_pSecurityDescriptor, &bPresent, pSacl.get_bufferptr(), &bDefaulted);
				if(!bPresent) pSacl.detach();
			}
			m_pSecurityDescriptor.free();
		}
	}

	void AllocateAndInitializeSecurityDescriptor() THROWANY
	{
		VERIFY_DO(!m_pSecurityDescriptor,m_pSecurityDescriptor.free());

		m_pSecurityDescriptor.allocate(sizeof(SECURITY_DESCRIPTOR));
		if(!::InitializeSecurityDescriptor(m_pSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION))
		{
			DWORD dwError = ::GetLastError();
			m_pSecurityDescriptor.free();
			throw_win32Error(dwError);
		}
	}

	void Init(const SECURITY_DESCRIPTOR& _pSD) THROWANY
	{
		SECURITY_DESCRIPTOR *pSD = const_cast<SECURITY_DESCRIPTOR *>(&_pSD);
		DWORD dwRev, dwLen = ::GetSecurityDescriptorLength(pSD);

		m_pSecurityDescriptor.allocate(dwLen);

		SECURITY_DESCRIPTOR_CONTROL sdc = 0;
		if(!::GetSecurityDescriptorControl(pSD, &sdc, &dwRev))
		{
			DWORD dwError = ::GetLastError();
			m_pSecurityDescriptor.free();
			throw_win32Error(dwError);
		}

		if(sdc & SE_SELF_RELATIVE)
		{
			utils::checked_memcpy(static_cast<void*>(static_cast<SECURITY_DESCRIPTOR*>(m_pSecurityDescriptor)), dwLen, pSD, dwLen);
		}
		else
		{
			if(!::MakeSelfRelativeSD(pSD, m_pSecurityDescriptor, &dwLen))
			{
				DWORD dwError = ::GetLastError();
				m_pSecurityDescriptor.free();
				throw_win32Error(dwError);
			}
		}
	}

	utils::CTmpBuffer<SECURITY_DESCRIPTOR> m_pSecurityDescriptor;
};//class CSecurityDesc

// **************************************************************
// CSecurityAttributes

class CSecurityAttributes : public SECURITY_ATTRIBUTES
{
public:
	CSecurityAttributes() THROW0
	{
		nLength = 0;
		lpSecurityDescriptor = NULL;
		bInheritHandle = FALSE;
	}

	explicit CSecurityAttributes(const CSecurityDesc& _rSecurityDescriptor, bool _bInheritHandle = false) THROWANY
		:m_SecurityDescriptor(_rSecurityDescriptor)
	{
		Set(m_SecurityDescriptor, _bInheritHandle);
	}

	void Set(const CSecurityDesc& _rSecurityDescriptor, bool _bInheritHandle = false) THROWANY
	{
		m_SecurityDescriptor = _rSecurityDescriptor;
		nLength = sizeof(SECURITY_ATTRIBUTES);
		lpSecurityDescriptor = const_cast<SECURITY_DESCRIPTOR*>(m_SecurityDescriptor.GetPSECURITY_DESCRIPTOR());
		this->bInheritHandle = _bInheritHandle;
	}

protected:
	CSecurityDesc m_SecurityDescriptor;
};//class CSecurityAttributes


typedef std::vector<CLuid> LUIDVec;

//******************************************************
// CTokenPrivileges

typedef std::vector<CString_> TokenPrivilegesNamesLst;
typedef std::vector<DWORD> TokenPrivilegesAttributesLst;

class CTokenPrivileges
{
	typedef std::map<CLuid, DWORD> Luid2AttrMap;
public:
	CTokenPrivileges() THROW0
		:m_bDirty(true)
	{
	}

	virtual ~CTokenPrivileges() THROW0
	{
	}

	CTokenPrivileges(const CTokenPrivileges& _tokenprivileges) THROWANY
	{
		m_mapTokenPrivileges = _tokenprivileges.m_mapTokenPrivileges;
	}

	CTokenPrivileges& operator=(const CTokenPrivileges& _tokenprivileges) THROWANY
	{
		if(this!=&_tokenprivileges)
		{
			m_mapTokenPrivileges.clear();
			m_mapTokenPrivileges = _tokenprivileges.m_mapTokenPrivileges;
			m_bDirty = true;
		}
		return *this;
	}

	CTokenPrivileges(const TOKEN_PRIVILEGES& _rPrivileges) THROWANY
	{
		AddPrivileges(_rPrivileges);
	}

	CTokenPrivileges& operator=(const TOKEN_PRIVILEGES& _rPrivileges) THROWANY
	{
		m_mapTokenPrivileges.clear();
		AddPrivileges(_rPrivileges);
		return *this;
	}

	void Add(const TOKEN_PRIVILEGES& _rPrivileges) THROWANY
	{
		AddPrivileges(_rPrivileges);
	}

	bool Add(LPCTSTR _pszPrivilege, bool _bEnable) THROWANY
	{
		LUID_AND_ATTRIBUTES la;
		ZeroMemory(&la,sizeof(la));

		if(!::LookupPrivilegeValue(NULL, _pszPrivilege, &la.Luid))
			return false;

		la.Attributes = _bEnable ? SE_PRIVILEGE_ENABLED : 0;

		m_mapTokenPrivileges.insert(Luid2AttrMap::value_type(la.Luid, la.Attributes));

		m_bDirty = true;
		return true;
	}

	bool LookupPrivilege(LPCTSTR _pszPrivilege, DWORD* _pdwAttributes = NULL) const THROWANY
	{
		DWORD dwAttributes = 0;
		CLuid luid;

		if(!::LookupPrivilegeValue(NULL, _pszPrivilege, &luid)) return false;

		Luid2AttrMap::const_iterator fit = m_mapTokenPrivileges.find(luid);
		if(m_mapTokenPrivileges.end()==fit) return false;
		if(NOT_NULL(_pdwAttributes)) *_pdwAttributes = fit->second;
		return true;
	}

	void GetNamesAndAttributes(TokenPrivilegesNamesLst* _pNames
		,TokenPrivilegesAttributesLst* _pAttributes = NULL
		) const THROWANY
	{
		VERIFY_EXIT(NOT_NULL(_pNames));

		utils::CTmpBuffer<TCHAR> psz;
		DWORD cbTmp = 0,cbName = 0;

		_pNames->clear();
		if(_pAttributes) _pAttributes->clear();
		
		Luid2AttrMap::const_iterator it,ite;
		it = m_mapTokenPrivileges.begin();
		ite = m_mapTokenPrivileges.end();
		for(;it!=ite;++it)
		{
			const Luid2AttrMap::value_type& val = *it;
			if(!::LookupPrivilegeName(NULL,const_cast<LUID*>(static_cast<const LUID*>(&val.first)),psz,&cbTmp))
			{
				if(::GetLastError()==ERROR_INSUFFICIENT_BUFFER)
				{
					psz.allocate_items(cbTmp+1,TCHAR());
					cbName = cbTmp;
					if(!::LookupPrivilegeName(NULL,const_cast<LUID*>(static_cast<const LUID*>(&val.first)),psz,&cbTmp))
						break;
				}
				else 
					break;
			}
			_pNames->push_back((LPCTSTR)(TCHAR*)psz);
			if(_pAttributes)
				_pAttributes->push_back(val.second);
		}

		if(it!=ite) 
		{
			_pNames->clear();
			if(NOT_NULL(_pAttributes)) _pAttributes->clear();
		}
	}

	void GetDisplayNames(TokenPrivilegesNamesLst* _pDisplayNames) const THROWANY
	{
		VERIFY_EXIT(NOT_NULL(_pDisplayNames));

		_pDisplayNames->clear();

		utils::CTmpBuffer<TCHAR> psz;
		DWORD cbTmp = 0,cbDisplayName = 0,dwLang = 0;

		TokenPrivilegesNamesLst names;

		GetNamesAndAttributes(&names);
		
		TokenPrivilegesNamesLst::const_iterator it,ite;
		it = names.begin();
		ite = names.end();

		for(;it!=ite;++it)
		{
			if(!::LookupPrivilegeDisplayName(NULL,*it,psz,&cbTmp,&dwLang))
			{
				if(EQL(::GetLastError(),ERROR_INSUFFICIENT_BUFFER))
				{
					psz.allocate_items(cbTmp+1,TCHAR());
					cbDisplayName = cbTmp;
					if(!::LookupPrivilegeDisplayName(NULL,*it,psz,&cbTmp,&dwLang))
						break;
				}
				else
					break;
			}
			_pDisplayNames->push_back((LPCTSTR)(TCHAR*)psz);
		}

		if(it!=ite) _pDisplayNames->clear();
	}

	void GetLuidsAndAttributes(LUIDVec* _pLuids
		,TokenPrivilegesAttributesLst* _pAttributes = NULL
		) const THROWANY
	{
		VERIFY_EXIT(NOT_NULL(_pLuids));

		_pLuids->clear();
		if(NOT_NULL(_pAttributes)) _pAttributes->clear();

		Luid2AttrMap::const_iterator it,ite;
		it = m_mapTokenPrivileges.begin();
		ite = m_mapTokenPrivileges.end();
		for(;it!=ite;++it)
		{
			const Luid2AttrMap::value_type& val = *it;
			_pLuids->push_back(val.first);
			if(NOT_NULL(_pAttributes)) _pAttributes->push_back(val.second);
		}
	}

	bool Delete(LPCTSTR _pszPrivilege) THROW0
	{
		CLuid luid;
		if(!::LookupPrivilegeValue(NULL, _pszPrivilege, &luid))
			return false;

		Luid2AttrMap::iterator fit = m_mapTokenPrivileges.find(luid);
		if(m_mapTokenPrivileges.end()==fit)	return false;

		m_mapTokenPrivileges.erase(fit);
		m_bDirty = true;
		return true;
	}

	void DeleteAll() THROW0
	{
		m_mapTokenPrivileges.clear();
		m_bDirty = true;
	}

	long GetCount() const THROW0
	{
		return m_mapTokenPrivileges.size();
	}

	long GetLength() const THROW0
	{
		return offsetof(TOKEN_PRIVILEGES,Privileges) + sizeof(LUID_AND_ATTRIBUTES)*GetCount();
	}

	const TOKEN_PRIVILEGES *GetPTOKEN_PRIVILEGES() const THROWANY
	{
		if(m_bDirty)
		{
			m_pTokenPrivileges.free();

			if(!m_mapTokenPrivileges.empty())
			{
				m_pTokenPrivileges.allocate(GetLength());

				m_pTokenPrivileges->PrivilegeCount = (DWORD) GetCount();

				Luid2AttrMap::const_iterator it,ite;
				it = m_mapTokenPrivileges.begin();
				ite = m_mapTokenPrivileges.end();

				long i = 0;
				for(i=0;it!=ite;++it,i++)
				{
					const Luid2AttrMap::value_type& val = *it;
					m_pTokenPrivileges->Privileges[i].Luid = val.first;
					m_pTokenPrivileges->Privileges[i].Attributes = val.second;
				}
			}
			m_bDirty = false;
		}
		return m_pTokenPrivileges;
	}

	operator const TOKEN_PRIVILEGES *() const THROWANY
	{
		return GetPTOKEN_PRIVILEGES();
	}

private:
	Luid2AttrMap m_mapTokenPrivileges;
	mutable utils::CTmpBuffer<TOKEN_PRIVILEGES> m_pTokenPrivileges;
	mutable bool m_bDirty;

	void AddPrivileges(const TOKEN_PRIVILEGES& _rPrivileges) THROWANY
	{
		m_bDirty = true;
		long i = 0;
		for(i=0;i<_rPrivileges.PrivilegeCount;i++)
		{
			Luid2AttrMap::iterator fit = m_mapTokenPrivileges.find(_rPrivileges.Privileges[i].Luid);
			if(m_mapTokenPrivileges.end()==fit)
				m_mapTokenPrivileges.insert(Luid2AttrMap::value_type(
						_rPrivileges.Privileges[i].Luid
						,_rPrivileges.Privileges[i].Attributes
						)
					);
			else fit->second = _rPrivileges.Privileges[i].Attributes;
		}	
	}
};//class CTokenPrivileges

//******************************************************
// CTokenGroups

class CTokenGroups
{
	typedef std::map<CSid,DWORD> Sid2AttrMap;
public:
	CTokenGroups() THROW0
		:m_bDirty(true)
	{
	}

	virtual ~CTokenGroups() THROW0
	{
	}

	CTokenGroups(const CTokenGroups& _tokengroup) THROWANY
		:m_bDirty(true)
	{
		Sid2AttrMap::const_iterator it,ite;
		it = _tokengroup.m_mapTokenGroups.begin();
		ite = _tokengroup.m_mapTokenGroups.end();
		for(;it!=ite;++it)
		{
			m_mapTokenGroups.insert(Sid2AttrMap::value_type(*it));
		}
	}

	CTokenGroups& operator=(const CTokenGroups& _tokengroup) THROWANY
	{
		if(this!=&_tokengroup)
		{
			m_mapTokenGroups.clear();
			Sid2AttrMap::const_iterator it,ite;
			it = _tokengroup.m_mapTokenGroups.begin();
			ite = _tokengroup.m_mapTokenGroups.end();
			for(;it!=ite;++it)
			{
				m_mapTokenGroups.insert(Sid2AttrMap::value_type(*it));
			}
			m_bDirty = true;
		}
		return *this;
	}

	CTokenGroups(const TOKEN_GROUPS& _tokengroup) THROWANY
	{
		AddTokenGroups(_tokengroup);
	}

	CTokenGroups &operator=(const TOKEN_GROUPS& _tokengroup) THROWANY
	{
		m_mapTokenGroups.clear();
		AddTokenGroups(_tokengroup);
		return *this;
	}

	void Add(const TOKEN_GROUPS& _rTokenGroups) THROWANY
	{
		AddTokenGroups(_rTokenGroups);
	}

	void Add(const CSid& _rSid, DWORD _dwAttributes) THROWANY
	{
		Sid2AttrMap::iterator fit = m_mapTokenGroups.find(_rSid);
		if(m_mapTokenGroups.end()==fit)
			m_mapTokenGroups.insert(Sid2AttrMap::value_type(_rSid,_dwAttributes));
		else 
			fit->second = _dwAttributes;
		m_bDirty = true;
	}

	bool LookupSid(const CSid& _rSid, DWORD* _pdwAttributes = NULL) const THROW0
	{
		Sid2AttrMap::const_iterator fit = m_mapTokenGroups.find(_rSid);
		if(m_mapTokenGroups.end()==fit) return false;
		if(NOT_NULL(_pdwAttributes)) *_pdwAttributes = fit->second;
		return true;
	}

	void GetSidsAndAttributes(
		SidLst* _pSids
		,std::vector<DWORD>* _pAttributes = NULL
		) const THROWANY
	{
		VERIFY_EXIT(NOT_NULL(_pSids));

		_pSids->clear();
		if(NOT_NULL(_pAttributes)) _pAttributes->clear();

		Sid2AttrMap::const_iterator it,ite;
		it = m_mapTokenGroups.begin();
		ite = m_mapTokenGroups.end();
		for(;it!=ite;++it)
		{
			const Sid2AttrMap::value_type& val = *it;
			_pSids->push_back(val.first);
			if(NOT_NULL(_pAttributes)) _pAttributes->push_back(val.second);
		}
	}

	bool Delete(const CSid& _rSid) THROW0
	{
		Sid2AttrMap::iterator fit = m_mapTokenGroups.find(_rSid);
		if(m_mapTokenGroups.end()==fit) return false;
		m_mapTokenGroups.erase(fit);
		m_bDirty = true;
		return true;
	}

	void DeleteAll() THROW0
	{
		m_mapTokenGroups.clear();
		m_bDirty = true;
	}

	long GetCount() const THROW0
	{
		return m_mapTokenGroups.size();
	}

	long GetLength() const THROW0
	{
		return offsetof(TOKEN_GROUPS, Groups) 
			+ sizeof(SID_AND_ATTRIBUTES) * m_mapTokenGroups.size()
			;
	}

	const TOKEN_GROUPS* GetPTOKEN_GROUPS() const THROWANY
	{
		if(m_bDirty)
		{
			m_pTokenGroups.free();

			if(!m_mapTokenGroups.empty())
			{
				m_pTokenGroups.allocate(GetLength());

				m_pTokenGroups->GroupCount = m_mapTokenGroups.size();

				Sid2AttrMap::const_iterator it,ite;
				it = m_mapTokenGroups.begin();
				ite = m_mapTokenGroups.end();
				long i =0;
				for(i=0;it!=ite;++it,i++)
				{
					const Sid2AttrMap::value_type& val = *it;
					m_pTokenGroups->Groups[i].Sid = const_cast<SID*>(val.first.GetPSID());
					m_pTokenGroups->Groups[i].Attributes = val.second;
				}
			}
			m_bDirty = false;
		}
		return m_pTokenGroups;
	}

	operator const TOKEN_GROUPS* () const THROWANY
	{
		return GetPTOKEN_GROUPS();
	}

private:
	Sid2AttrMap m_mapTokenGroups;
	mutable utils::CTmpBuffer<TOKEN_GROUPS> m_pTokenGroups;
	mutable bool m_bDirty;

	void AddTokenGroups(const TOKEN_GROUPS& _rTokenGroups) THROWANY
	{
		m_bDirty = true;
		long i = 0;
		for(i=0; i<_rTokenGroups.GroupCount; i++)
		{
			CSid sid(static_cast<SID*>(_rTokenGroups.Groups[i].Sid));
			Sid2AttrMap::iterator fit = m_mapTokenGroups.find(sid);
			if(m_mapTokenGroups.end()==fit)
				m_mapTokenGroups.insert(Sid2AttrMap::value_type(sid,_rTokenGroups.Groups[i].Attributes));
			else
				fit->second = _rTokenGroups.Groups[i].Attributes;
		}
	}
};//class CTokenGroups

// *************************************
// CAccessToken

class CAccessToken
{
public:
	CAccessToken() THROW0
		:m_hToken(NULL)
		,m_hProfile(NULL)
		,m_pRevert(NULL)
	{
	}

	virtual ~CAccessToken() THROW0
	{
		Clear();
	}

	void Attach(HANDLE _hToken) THROW0
	{
		VERIFY_EXIT(IS_NULL(m_hToken));
		m_hToken = _hToken;
	}

	HANDLE Detach() THROW0
	{
		HANDLE hToken = m_hToken;
		m_hToken = NULL;
		Clear();
		return hToken;
	}

	HANDLE GetHandle() const THROW0
	{
		return m_hToken;
	}

	HKEY HKeyCurrentUser() const THROW0
	{
		return reinterpret_cast<HKEY>(m_hProfile);
	}

	// Privileges
	bool EnablePrivilege(
		LPCTSTR _pszPrivilege
		,CTokenPrivileges* _pPreviousState = NULL
		,bool* _pbErrNotAllAssigned=NULL
		) THROWANY
	{
		CTokenPrivileges NewState;
		NewState.Add(_pszPrivilege, true);
		return EnableDisablePrivileges(NewState, _pPreviousState,_pbErrNotAllAssigned);
	}

	bool EnablePrivileges(
		const std::list<CString_>& _rPrivileges
		,CTokenPrivileges* _pPreviousState = NULL
		,bool* _pbErrNotAllAssigned=NULL
		) THROWANY
	{
		return EnableDisablePrivileges(_rPrivileges, true, _pPreviousState,_pbErrNotAllAssigned);
	}

	bool DisablePrivilege(
		LPCTSTR _pszPrivilege
		,CTokenPrivileges* _pPreviousState = NULL
		,bool* _pbErrNotAllAssigned=NULL
		) THROWANY
	{
		CTokenPrivileges NewState;
		NewState.Add(_pszPrivilege, false);
		return EnableDisablePrivileges(NewState, _pPreviousState,_pbErrNotAllAssigned);
	}

	bool DisablePrivileges(
		const std::list<CString_>& _rPrivileges
		,CTokenPrivileges* _pPreviousState = NULL
		,bool* _pbErrNotAllAssigned=NULL
		) THROWANY
	{
		return EnableDisablePrivileges(_rPrivileges, false, _pPreviousState,_pbErrNotAllAssigned);
	}

	bool EnableDisablePrivileges(
		const CTokenPrivileges& _rNewState
		,CTokenPrivileges* _pPreviousState = NULL
		,bool* _pbErrNotAllAssigned=NULL
		) THROWANY
	{
		if(!_rNewState.GetCount()) return true;

		TOKEN_PRIVILEGES *pNewState = const_cast<TOKEN_PRIVILEGES *>(_rNewState.GetPTOKEN_PRIVILEGES());

		DWORD dwLength = offsetof(TOKEN_PRIVILEGES, Privileges)
			+ _rNewState.GetCount() * sizeof(LUID_AND_ATTRIBUTES)
			;

		utils::CTmpBufferOnStack<TOKEN_PRIVILEGES> pPrevState(dwLength);
		if(::AdjustTokenPrivileges(m_hToken, FALSE, pNewState, dwLength, pPrevState, &dwLength))
		{
			if (_pbErrNotAllAssigned)
			{
				if(::GetLastError() == ERROR_NOT_ALL_ASSIGNED)
				{
					*_pbErrNotAllAssigned=true;
				} else
				{
					*_pbErrNotAllAssigned=false;
				}
			}
		}else
		{
			return false;
		}

		if(_pPreviousState)
		{
			_pPreviousState->Add(*pPrevState);
		}

		return true;
	}

	bool PrivilegeCheck(PPRIVILEGE_SET _RequiredPrivileges, bool* _pbResult) const THROWANY
	{
		BOOL bResult = FALSE;
		if(!::PrivilegeCheck(m_hToken, _RequiredPrivileges, &bResult)) return false;
		*_pbResult = to_bool(bResult);
		return true;
	}

	bool GetLogonSid(CSid* _pSid) const THROWANY
	{
		VERIFY_EXIT1(NOT_NULL(_pSid),false);

		DWORD dwLen = 0;
		::GetTokenInformation(m_hToken, TokenGroups, NULL, 0, &dwLen);
		if(::GetLastError()!=ERROR_INSUFFICIENT_BUFFER) return false;

		utils::CTmpBufferOnStack<TOKEN_GROUPS> pGroups(dwLen);
		if(::GetTokenInformation(m_hToken, TokenGroups, pGroups, dwLen, &dwLen))
		{
			for(UINT i = 0; i < pGroups->GroupCount; i++)
			{
				if(pGroups->Groups[i].Attributes & SE_GROUP_LOGON_ID)
				{
					*_pSid = static_cast<SID*>(pGroups->Groups[i].Sid);
					return true;
				}
			}
		}
		return false;
	};

	bool GetTokenId(LUID* _pluid) const THROWANY
	{
		VERIFY_EXIT1(NOT_NULL(_pluid),false);

		TOKEN_STATISTICS Statistics;
		ZeroMemory(&Statistics,sizeof(Statistics));

		if(!GetStatistics(&Statistics)) return false;

		*_pluid = Statistics.TokenId;
		return true;
	}

	bool GetLogonSessionId(LUID* _pluid) const THROWANY
	{
		VERIFY_EXIT1(NOT_NULL(_pluid),false);

		TOKEN_STATISTICS Statistics;
		ZeroMemory(&Statistics,sizeof(Statistics));

		if(!GetStatistics(&Statistics))	return false;

		*_pluid = Statistics.AuthenticationId;
		return true;
	}

	bool CheckTokenMembership(const CSid& _rSid, bool* _pbIsMember) const THROWANY
	{
		// "this" must be an impersonation token and NOT a primary token
		BOOL bIsMember = FALSE;

		VERIFY_EXIT1(NOT_NULL(_pbIsMember),false);

#if(_WIN32_WINNT >= 0x0500)
		if(::CheckTokenMembership(m_hToken, const_cast<SID*>(_rSid.GetPSID()), &bIsMember))
#else
		GENERIC_MAPPING gm = {0, 0, 0, 1};
		PRIVILEGE_SET ps;
		DWORD cb = sizeof(PRIVILEGE_SET);
		DWORD ga;
		CSecurityDesc sd;
		CDacl dacl;

		if (!dacl.AddAllowedAce(_rSid, 1))
			return false;
		sd.SetOwner(_rSid);
		sd.SetGroup(_rSid);
		sd.SetDacl(dacl);

		if(::AccessCheck(
				const_cast<SECURITY_DESCRIPTOR*>(sd.GetPSECURITY_DESCRIPTOR())
				,m_hToken, 1, &gm, &ps, &cb, &ga, &bIsMember
				)
			)
#endif
		{
			*_pbIsMember = to_bool(bIsMember);
			return true;
		}
		return false;
	}

#if(_WIN32_WINNT >= 0x0500)
	bool IsTokenRestricted() const THROW0
	{
		return to_bool(::IsTokenRestricted(m_hToken));
	}
#endif

	// Token Information
protected:
	void InfoTypeToRetType(CSid* _pRet, const TOKEN_USER& _rWork) const THROWANY
	{
		VERIFY_EXIT(NOT_NULL(_pRet));
		*_pRet = *static_cast<SID*>(_rWork.User.Sid);
	}

	void InfoTypeToRetType(CTokenGroups* _pRet, const TOKEN_GROUPS& _rWork) const THROWANY
	{
		VERIFY_EXIT(NOT_NULL(_pRet));
		*_pRet = _rWork;
	}

	void InfoTypeToRetType(CTokenPrivileges* _pRet, const TOKEN_PRIVILEGES& _rWork) const THROWANY
	{
		VERIFY_EXIT(NOT_NULL(_pRet));
		*_pRet = _rWork;
	}

	void InfoTypeToRetType(CSid* _pRet, const TOKEN_OWNER& _rWork) const THROWANY
	{
		VERIFY_EXIT(NOT_NULL(_pRet));
		*_pRet = *static_cast<SID *>(_rWork.Owner);
	}

	void InfoTypeToRetType(CSid* _pRet, const TOKEN_PRIMARY_GROUP& _rWork) const THROWANY
	{
		VERIFY_EXIT(NOT_NULL(_pRet));
		*_pRet = *static_cast<SID *>(_rWork.PrimaryGroup);
	}

	void InfoTypeToRetType(CDacl* _pRet, const TOKEN_DEFAULT_DACL& _rWork) const THROWANY
	{
		VERIFY_EXIT(NOT_NULL(_pRet));
		*_pRet = *_rWork.DefaultDacl;
	}

	template<typename RET_T, typename INFO_T>
	bool GetInfoConvert(RET_T* _pRet, TOKEN_INFORMATION_CLASS _TokenClass, INFO_T) const THROWANY
	{
		VERIFY_EXIT1(NOT_NULL(_pRet),false);

		DWORD dwLen = 0;
		::GetTokenInformation(m_hToken, _TokenClass, NULL, 0, &dwLen);
		if(::GetLastError()!=ERROR_INSUFFICIENT_BUFFER) return false;

		utils::CTmpBufferOnStack<INFO_T> pWork(dwLen);
		if(!::GetTokenInformation(m_hToken, _TokenClass, pWork, dwLen, &dwLen)) return false;

		InfoTypeToRetType(_pRet, *pWork);
		return true;
	}

	template<typename RET_T>
	bool GetInfo(RET_T* _pRet, TOKEN_INFORMATION_CLASS _TokenClass) const THROWANY
	{
		VERIFY_EXIT1(NOT_NULL(_pRet),false);

		DWORD dwLen = 0;
		if(!::GetTokenInformation(m_hToken, _TokenClass, _pRet, sizeof(RET_T), &dwLen)) return false;
		return true;
	}

public:
	bool GetDefaultDacl(CDacl* _pDacl) const THROWANY
	{
		return GetInfoConvert(_pDacl, TokenDefaultDacl,TOKEN_DEFAULT_DACL());
	}

	bool GetGroups(CTokenGroups* _pGroups) const THROWANY
	{	
		return GetInfoConvert(_pGroups, TokenGroups,TOKEN_GROUPS());
	}

	bool GetImpersonationLevel(SECURITY_IMPERSONATION_LEVEL* _pImpersonationLevel) const THROWANY
	{
		return GetInfo(_pImpersonationLevel, TokenImpersonationLevel);
	}

	bool GetOwner(CSid* _pSid) const THROWANY
	{
		return GetInfoConvert(_pSid, TokenOwner,TOKEN_OWNER());
	}

	bool GetPrimaryGroup(CSid* _pSid) const THROWANY
	{
		return GetInfoConvert(_pSid, TokenPrimaryGroup,TOKEN_PRIMARY_GROUP());
	}

	bool GetPrivileges(CTokenPrivileges* _pPrivileges) const THROWANY
	{
		return GetInfoConvert(_pPrivileges, TokenPrivileges,TOKEN_PRIVILEGES());
	}

	bool GetTerminalServicesSessionId(DWORD* _pdwSessionId) const THROWANY
	{
		return GetInfo(_pdwSessionId, /*TokenSessionId*/(TOKEN_INFORMATION_CLASS)(1+11));
	}

	bool GetSource(TOKEN_SOURCE* _pSource) const THROWANY
	{
		return GetInfo(_pSource, TokenSource);
	}

	bool GetStatistics(TOKEN_STATISTICS* _pStatistics) const THROWANY
	{
		return GetInfo(_pStatistics, TokenStatistics);
	}

	bool GetType(TOKEN_TYPE* _pType) const THROWANY
	{
		return GetInfo(_pType, TokenType);
	}

	bool GetUser(CSid* _pSid) const THROWANY
	{
		return GetInfoConvert(_pSid, TokenUser,TOKEN_USER());
	}

	bool SetOwner(const CSid& rSid) THROWANY
	{
		TOKEN_OWNER to;
		ZeroMemory(&to,sizeof(to));
		to.Owner = const_cast<SID*>(rSid.GetPSID());
		return to_bool(::SetTokenInformation(m_hToken, TokenOwner, &to, sizeof(to)));
	}

	bool SetPrimaryGroup(const CSid &rSid) THROWANY
	{
		TOKEN_PRIMARY_GROUP tpg;
		ZeroMemory(&tpg,sizeof(tpg));
		tpg.PrimaryGroup = const_cast<SID*>(rSid.GetPSID());
		return to_bool(::SetTokenInformation(m_hToken, TokenPrimaryGroup, &tpg, sizeof(tpg)));
	}

	bool SetDefaultDacl(const CDacl &rDacl) THROWANY
	{
		TOKEN_DEFAULT_DACL tdd;
		ZeroMemory(&tdd,sizeof(tdd));
		tdd.DefaultDacl = const_cast<ACL*>(rDacl.GetPACL());
		return 0 != ::SetTokenInformation(m_hToken, TokenDefaultDacl, &tdd, sizeof(tdd));
	}

	bool CreateImpersonationToken(
		CAccessToken* _pImp
		,SECURITY_IMPERSONATION_LEVEL _sil = SecurityImpersonation
		) const THROWANY
	{
		VERIFY_EXIT1(NOT_NULL(_pImp),false);

		HANDLE hToken = NULL;
		if(!::DuplicateToken(m_hToken, _sil, &hToken)) return false;

		_pImp->Clear();
		_pImp->m_hToken = hToken;
		return true;
	}

	bool CreatePrimaryToken(
		CAccessToken* _pPri
		,DWORD _dwDesiredAccess = MAXIMUM_ALLOWED
		,const CSecurityAttributes* _pTokenAttributes = NULL
		) const THROWANY
	{
		VERIFY_EXIT1(NOT_NULL(_pPri),false);

		HANDLE hToken = NULL;
		if(!::DuplicateTokenEx(
				m_hToken
				,_dwDesiredAccess
				,const_cast<CSecurityAttributes *>(_pTokenAttributes),
				SecurityAnonymous, TokenPrimary, &hToken
				)
			)
		{
			return false;
		}

		_pPri->Clear();
		_pPri->m_hToken = hToken;
		return true;
	}

#if(_WIN32_WINNT >= 0x0500)
	bool CreateRestrictedToken(
		CAccessToken *_pRestrictedToken
		,const CTokenGroups& _SidsToDisable
		,const CTokenGroups& _SidsToRestrict 
		,const CTokenPrivileges& _PrivilegesToDelete = CTokenPrivileges()
		) const THROWANY
	{
		VERIFY_EXIT1(NOT_NULL(_pRestrictedToken),false);

		HANDLE hToken = NULL;
		SID_AND_ATTRIBUTES* pSidsToDisable = NULL;
		SID_AND_ATTRIBUTES* pSidsToRestrict = NULL;
		LUID_AND_ATTRIBUTES* pPrivilegesToDelete = NULL;

		long nDisableSidCount = _SidsToDisable.GetCount();
		if(nDisableSidCount>0)
		{
			const TOKEN_GROUPS* pTOKEN_GROUPS = _SidsToDisable.GetPTOKEN_GROUPS();

			ASSERT_(NOT_NULL(pTOKEN_GROUPS));

			if(NOT_NULL(pTOKEN_GROUPS))
			{
				pSidsToDisable = const_cast<SID_AND_ATTRIBUTES*>(pTOKEN_GROUPS->Groups);
			}
			else
			{
				return false;
			}
		}
		else
		{
			pSidsToDisable = NULL;
		}

		long nRestrictedSidCount = _SidsToRestrict.GetCount();
		if(nRestrictedSidCount>0)
		{
			const TOKEN_GROUPS* pTOKEN_GROUPS = _SidsToRestrict.GetPTOKEN_GROUPS();

			ASSERT_(NOT_NULL(pTOKEN_GROUPS));

			if(NOT_NULL(pTOKEN_GROUPS))
			{
				pSidsToRestrict = const_cast<SID_AND_ATTRIBUTES*>(pTOKEN_GROUPS->Groups);
			}
			else
			{
				return false;
			}

		}
		else
		{
			pSidsToRestrict = NULL;
		}

		long nDeletePrivilegesCount = _PrivilegesToDelete.GetCount();
		if(nDeletePrivilegesCount>0)
		{
			const TOKEN_PRIVILEGES* pTOKEN_PRIVILEGES = _PrivilegesToDelete.GetPTOKEN_PRIVILEGES();

			ASSERT_(NOT_NULL(pTOKEN_PRIVILEGES));

			if(NOT_NULL(pTOKEN_PRIVILEGES))
			{
				pPrivilegesToDelete = const_cast<LUID_AND_ATTRIBUTES*>(pTOKEN_PRIVILEGES->Privileges);
			}
			else
			{
				return false;
			}

		}
		else
		{
			pPrivilegesToDelete = NULL;
		}

		if(!::CreateRestrictedToken(
				m_hToken, 0
				,(DWORD) nDisableSidCount, pSidsToDisable
				,(DWORD) nDeletePrivilegesCount, pPrivilegesToDelete
				,(DWORD) nRestrictedSidCount, pSidsToRestrict
				,&hToken
				)
			)
		{
			return false;
		}

		_pRestrictedToken->Clear();
		_pRestrictedToken->m_hToken = hToken;
		return true;
	}
#endif

	// Token API type functions
	bool GetProcessToken(DWORD _dwDesiredAccess, HANDLE _hProcess = NULL) THROW0
	{
		if(!_hProcess) _hProcess = ::GetCurrentProcess();

		HANDLE hToken = NULL;
		if(!::OpenProcessToken(_hProcess, _dwDesiredAccess, &hToken)) return false;

		Clear();
		m_hToken = hToken;
		return true;
	}

	bool GetThreadToken(DWORD _dwDesiredAccess, HANDLE _hThread = NULL, bool _bOpenAsSelf = true) THROW0
	{
		if(!_hThread) _hThread = ::GetCurrentThread();

		HANDLE hToken = NULL;
		if(!::OpenThreadToken(_hThread, _dwDesiredAccess, _bOpenAsSelf, &hToken)) return false;

		Clear();
		m_hToken = hToken;

		return true;
	}

	bool GetEffectiveToken(DWORD _dwDesiredAccess) THROW0
	{
		if(!GetThreadToken(_dwDesiredAccess)) return GetProcessToken(_dwDesiredAccess);
		return true;
	}

	bool OpenThreadToken(
		DWORD _dwDesiredAccess
		,bool _bImpersonate = false
		,bool _bOpenAsSelf = true
		,SECURITY_IMPERSONATION_LEVEL _sil = SecurityImpersonation
		) THROW0
	{
		CheckImpersonation();

		if(!::ImpersonateSelf(_sil))	return false;

		HANDLE hToken = NULL;
		if(!::OpenThreadToken(::GetCurrentThread(), _dwDesiredAccess, _bOpenAsSelf, &hToken))
			return false;

		Clear();
		m_hToken = hToken;

		if(!_bImpersonate) 
			::RevertToSelf();
		else
		{
			try{m_pRevert = trace_alloc(new CRevertToSelf);} catch(...){m_pRevert=NULL;}
			if(NOT_NULL(m_pRevert))
			{
				::RevertToSelf();
				Clear();
				return false;
			}
		}
		return true;
	}

#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) 

	bool OpenCOMClientToken(
		DWORD _dwDesiredAccess
		,bool _bImpersonate = false
		,bool _bOpenAsSelf = true
		) THROWANY
	{
		CheckImpersonation();

		if(FAILED(::CoImpersonateClient()))
			return false;

		HANDLE hToken = NULL;
		if(!::OpenThreadToken(::GetCurrentThread(), _dwDesiredAccess, _bOpenAsSelf, &hToken))
			return false;

		Clear();
		m_hToken = hToken;

		if(!_bImpersonate)
			::CoRevertToSelf();
		else
		{
			try{m_pRevert = trace_alloc(new CCoRevertToSelf);}catch(...){m_pRevert=NULL;}
			if(NOT_NULL(m_pRevert))
			{
				::CoRevertToSelf();
				Clear();
				return false;
			}
		}
		return true;
	}

#endif //(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) 

	bool OpenNamedPipeClientToken(
		HANDLE _hPipe
		,DWORD _dwDesiredAccess
		,bool _bImpersonate = false
		,bool _bOpenAsSelf = true
		) THROWANY
	{
		CheckImpersonation();

		if(!::ImpersonateNamedPipeClient(_hPipe))
			return false;

		HANDLE hToken = NULL;
		if(!::OpenThreadToken(::GetCurrentThread(), _dwDesiredAccess, _bOpenAsSelf, &hToken))
			return false;

		Clear();
		m_hToken = hToken;

		if(!_bImpersonate)
			::RevertToSelf();
		else
		{
			try{m_pRevert = trace_alloc(new CRevertToSelf);} catch(...){m_pRevert = NULL;}
			if(NOT_NULL(m_pRevert))
			{
				::RevertToSelf();
				Clear();
				return false;
			}
		}
		return true;
	}

	bool OpenRPCClientToken(
		RPC_BINDING_HANDLE _BindingHandle
		,DWORD _dwDesiredAccess
		,bool _bImpersonate = false
		,bool _bOpenAsSelf = true
		) THROWANY
	{
		CheckImpersonation();

		if(RPC_S_OK!=::RpcImpersonateClient(_BindingHandle)) return false;

		HANDLE hToken = NULL;
		if(!::OpenThreadToken(::GetCurrentThread(), _dwDesiredAccess, _bOpenAsSelf, &hToken))
			return false;

		Clear();
		m_hToken = hToken;

		if(!_bImpersonate)
		{
			RPC_STATUS rpcStatus = ::RpcRevertToSelfEx(_BindingHandle);
			if(rpcStatus!=RPC_S_OK)
			{
				Clear();
				return false;
			}
		}
		else
		{
			try{m_pRevert = trace_alloc(new CRpcRevertToSelfEx(_BindingHandle));}catch(...){m_pRevert = NULL;}
			if(NOT_NULL(m_pRevert))
			{
				RPC_STATUS rpcStatus = ::RpcRevertToSelfEx(_BindingHandle);
				if( rpcStatus != RPC_S_OK )
				{
					// If we continue, arbitrary user code will run in the wrong context. Too dangerous to risk
					::TerminateProcess(::GetCurrentProcess(), 0);
				}
			}
		}
		return true;
	}

	bool ImpersonateLoggedOnUser() const THROWANY
	{
		CheckImpersonation();

		VERIFY_EXIT1(NOT_NULL(m_hToken),false);

		if(!::ImpersonateLoggedOnUser(m_hToken)) return false;
		VERIFY_DO(IS_NULL(m_pRevert),delete trace_free(m_pRevert));
		try{m_pRevert = trace_alloc(new CRevertToSelf);}catch(...){m_pRevert=NULL;}
		if(NOT_NULL(m_pRevert))
		{
			::RevertToSelf();
			return false;
		}
		return true;
	}

	bool Impersonate(HANDLE _hThread = NULL) const THROWANY
	{
		CheckImpersonation();

		VERIFY_EXIT1(NOT_NULL(m_hToken),false);
		return to_bool(::SetThreadToken(_hThread ? &_hThread : NULL, m_hToken));
	};

	bool Revert(HANDLE _hThread = NULL) const THROWANY
	{
		if(NOT_NULL(_hThread) || IS_NULL(m_pRevert))
			return to_bool(::SetThreadToken(_hThread ? &_hThread : NULL, NULL));

		bool bRet = m_pRevert->Revert();
		delete trace_free(m_pRevert);
		m_pRevert = NULL;
		return bRet;
	}

//#define USE_PROFILE_API
#if defined(USE_PROFILE_API)
	bool LoadUserProfile() THROWANY
	{
		VERIFY_EXIT1(NOT_NULL(m_hToken) && IS_NULL(m_hProfile),false);

		CSid UserSid;
		PROFILEINFO Profile;

		if(!GetUser(&UserSid)) return false;

		ZeroMemory(&Profile, sizeof(PROFILEINFO));
		Profile.dwSize = sizeof(PROFILEINFO);
		Profile.lpUserName = const_cast<LPTSTR>(UserSid.AccountName());
		if(!::LoadUserProfile(m_hToken, &Profile))
			return false;

		m_hProfile = Profile.hProfile;

		return true;
	}

	HANDLE GetProfile() const THROW0
	{
		return m_hProfile;
	}
#endif//#if defined(USE_PROFILE_API)

	// Must hold Tcb privilege
	bool LogonUser(
		LPCTSTR _pszUserName
		,LPCTSTR _pszDomain
		,LPCTSTR _pszPassword
		,DWORD _dwLogonType = LOGON32_LOGON_INTERACTIVE
		,DWORD _dwLogonProvider = LOGON32_PROVIDER_DEFAULT
		) THROW0
	{
		Clear();

		return to_bool(::LogonUser(
				const_cast<LPTSTR>(_pszUserName)
				,const_cast<LPTSTR>(_pszDomain)
				,const_cast<LPTSTR>(_pszPassword)
				,_dwLogonType, _dwLogonProvider, &m_hToken
				)
			);
	}

#if defined(USE_PROFILE_API)
	// Must hold AssignPrimaryToken (unless restricted token) and
	// IncreaseQuota privileges
	bool CreateProcessAsUser(
		LPCTSTR _pApplicationName
		,LPTSTR _pCommandLine
		,LPPROCESS_INFORMATION _pProcessInformation
		,LPSTARTUPINFO _pStartupInfo
		,DWORD _dwCreationFlags = NORMAL_PRIORITY_CLASS
		,bool _bLoadProfile = false
		,const CSecurityAttributes* _pProcessAttributes = NULL
		,const CSecurityAttributes* _pThreadAttributes = NULL
		,bool _bInherit = false
		,LPCTSTR _pCurrentDirectory = NULL
		) THROW0
	{
		VERIFY_EXIT1(NOT_NULL(_pProcessInformation),false);
		VERIFY_EXIT1(NOT_NULL(_pStartupInfo),false);

		LPVOID pEnvironmentBlock = NULL;
		PROFILEINFO Profile;
		CSid UserSid;
		DWORD dwLastError = 0;

		HANDLE hToken = m_hToken;

		ZeroMemory(&Profile,sizeof(Profile));

		// Straighten out impersonation problems...
		TOKEN_TYPE TokenType;
		if(!GetType(&TokenType)) return false;

		if(TokenType != TokenPrimary)
		{
			hToken = NULL;
			if (!::DuplicateTokenEx(m_hToken
					,TOKEN_QUERY|TOKEN_DUPLICATE|TOKEN_ASSIGN_PRIMARY, NULL
					,SecurityImpersonation, TokenPrimary
					,&hToken
					)
				)
			{
				return false;
			}
		}

		// Profile
		if(_bLoadProfile && IS_NULL(m_hProfile))
		{
			if(!GetUser(&UserSid))
			{
				if(TokenType != TokenPrimary)
					::CloseHandle(hToken);
				return false;
			}
			ZeroMemory(&Profile,sizeof(Profile));
			Profile.dwSize = sizeof(PROFILEINFO);
			Profile.lpUserName = const_cast<LPTSTR>(UserSid.AccountName());
			if(::LoadUserProfile(hToken, &Profile))
				m_hProfile = Profile.hProfile;
		}

		// Environment block
		if(!::CreateEnvironmentBlock(&pEnvironmentBlock, hToken, _bInherit))
			return false;

		BOOL bRetVal = ::CreateProcessAsUser(
			hToken
			,_pApplicationName
			,_pCommandLine
			,const_cast<CSecurityAttributes *>(_pProcessAttributes)
			,const_cast<CSecurityAttributes *>(_pThreadAttributes)
			,_bInherit
			,_dwCreationFlags
			,pEnvironmentBlock
			,_pCurrentDirectory
			,_pStartupInfo
			,_pProcessInformation
			);

		 dwLastError = ::GetLastError();

		if(TokenType != TokenPrimary) ::CloseHandle(hToken);

		::DestroyEnvironmentBlock(pEnvironmentBlock);

		::SetLastError(dwLastError);
		return bRetVal != 0;
	}
#endif//#if defined(USE_PROFILE_API)

protected:
	bool EnableDisablePrivileges(
		const std::list<CString_>& _rPrivileges,
		bool _bEnable,
		CTokenPrivileges* _pPreviousState,bool* _pbErrNotAllAssigned=NULL
		) THROWANY
	{
		CTokenPrivileges NewState;
		std::list<CString_>::const_iterator it,ite;
		it = _rPrivileges.begin();
		ite = _rPrivileges.end();
		for(;it!=ite;++it)
		{
			NewState.Add(*it, _bEnable);
		}
		return EnableDisablePrivileges(NewState, _pPreviousState,_pbErrNotAllAssigned);
	}

	bool CheckImpersonation() const THROW0
	{
		// You should not be impersonating at this point.  Use GetThreadToken
		// instead of the OpenXXXToken functions or call Revert before
		// calling Impersonate.
		HANDLE hToken=INVALID_HANDLE_VALUE;

		if(!::OpenThreadToken(::GetCurrentThread(), 0, false, &hToken) 
			&& ::GetLastError() != ERROR_NO_TOKEN
			)
		{
			TRACE_(_T("Caution: replacing thread impersonation token.\n"));
			return true;
		}
		if(hToken!=INVALID_HANDLE_VALUE)
		{
			::CloseHandle(hToken);
		}
		return false;
	}

	virtual void Clear() THROW0
	{
#if defined(USE_PROFILE_API)
		if(m_hProfile)
		{
			if(m_hToken) ::UnloadUserProfile(m_hToken, m_hProfile);
			m_hProfile = NULL;
		}
#endif//#if defined(USE_PROFILE_API) 

		if(m_hToken)
		{
			::CloseHandle(m_hToken);
			m_hToken = NULL;
		}
		if(m_pRevert) 
		{
			delete trace_free(m_pRevert);
			m_pRevert = NULL;
		}
	}

	HANDLE m_hToken;
	HANDLE m_hProfile;

private:
	CAccessToken(const CAccessToken &rhs) THROWANY;
	CAccessToken &operator=(const CAccessToken &rhs) THROWANY;

	class CRevert
	{
	public:
		virtual bool Revert() THROW0 = 0;
	};

	class CRevertToSelf : public CRevert
	{
	public:
		bool Revert() THROW0
		{
			return 0 != ::RevertToSelf();
		}
	};

#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) 
	class CCoRevertToSelf : public CRevert
	{
	public:
		bool Revert() THROW0
		{
			return SUCCEEDED(::CoRevertToSelf());
		}
	};
#endif //(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) 

	class CRpcRevertToSelfEx : public CRevert
	{
	public:
		CRpcRevertToSelfEx(RPC_BINDING_HANDLE BindingHandle) THROW0
			:m_hBinding(BindingHandle)
		{
		}
		bool Revert() THROW0
		{
			return RPC_S_OK == ::RpcRevertToSelfEx(m_hBinding);
		}

	private:
		RPC_BINDING_HANDLE m_hBinding;
	};
	mutable CRevert *m_pRevert;
};//class CAccessToken

//*******************************************
// CAutoRevertImpersonation

class CAutoRevertImpersonation
{
public:
	CAutoRevertImpersonation(const CAccessToken* _pAT) THROW0
	{
		m_pAT = _pAT;
	}

	~CAutoRevertImpersonation() THROW0
	{
		if (m_pAT != NULL)
		{
			m_pAT->Revert();
		}
	}

	void Attach(const CAccessToken* _pAT) THROW0
	{
		VERIFY_EXIT(IS_NULL(m_pAT));
		m_pAT = _pAT;
	}

	const CAccessToken* Detach() THROW0
	{
		const CAccessToken* pAT = m_pAT;
		m_pAT = NULL;
		return pAT;
	}

	const CAccessToken* GetAccessToken() THROW0
	{
		return m_pAT;
	}

private:
	const CAccessToken* m_pAT;

	CAutoRevertImpersonation(const CAutoRevertImpersonation &rhs) THROWANY;
	CAutoRevertImpersonation &operator=(const CAutoRevertImpersonation &rhs) THROWANY;
};//class CAutoRevertImpersonation

//*******************************************
// CPrivateObjectSecurityDesc

class CPrivateObjectSecurityDesc : public CSecurityDesc
{
public:
	CPrivateObjectSecurityDesc() THROW0
		:m_bPrivate(false)
		,CSecurityDesc()
	{
	}

	~CPrivateObjectSecurityDesc() THROW0
	{
		Clear();
	}

	bool Create(
		const CSecurityDesc* _pParent
		,const CSecurityDesc* _pCreator
		,bool _bIsDirectoryObject
		,const CAccessToken& _Token
		,PGENERIC_MAPPING _GenericMapping
		) THROW0
	{
		Clear();

		const SECURITY_DESCRIPTOR* pSDParent = _pParent ? _pParent->GetPSECURITY_DESCRIPTOR() : NULL;
		const SECURITY_DESCRIPTOR* pSDCreator = _pCreator ? _pCreator->GetPSECURITY_DESCRIPTOR() : NULL;

		if(!::CreatePrivateObjectSecurity(
				const_cast<SECURITY_DESCRIPTOR *>(pSDParent)
				,const_cast<SECURITY_DESCRIPTOR *>(pSDCreator)
				,reinterpret_cast<PSECURITY_DESCRIPTOR *>(&m_pSecurityDescriptor)
				,_bIsDirectoryObject,_Token.GetHandle(),_GenericMapping
				)
			)
		{
			return false;
		}

		m_bPrivate = true;
		return true;
	}

#if(_WIN32_WINNT >= 0x0500)
	bool Create(
		const CSecurityDesc* _pParent
		,const CSecurityDesc* _pCreator
		,GUID* _ObjectType
		,bool _bIsContainerObject
		,ULONG _AutoInheritFlags
		,const CAccessToken& _Token
		,PGENERIC_MAPPING _GenericMapping
		) THROW0
	{
		Clear();

		const SECURITY_DESCRIPTOR* pSDParent = _pParent ? _pParent->GetPSECURITY_DESCRIPTOR() : NULL;
		const SECURITY_DESCRIPTOR* pSDCreator = _pCreator ? _pCreator->GetPSECURITY_DESCRIPTOR() : NULL;

		if(!::CreatePrivateObjectSecurityEx(
				const_cast<SECURITY_DESCRIPTOR *>(pSDParent)
				,const_cast<SECURITY_DESCRIPTOR *>(pSDCreator)
				,reinterpret_cast<PSECURITY_DESCRIPTOR *>(&m_pSecurityDescriptor)
				,_ObjectType, _bIsContainerObject, _AutoInheritFlags, _Token.GetHandle(), _GenericMapping
				)
			)
		{
			return false;
		}

		m_bPrivate = true;
		return true;
	}

#endif

	bool Get(SECURITY_INFORMATION _si, CSecurityDesc* _pSDResult) const THROW0
	{
		VERIFY_EXIT1(NOT_NULL(_pSDResult),false);

		if(!m_bPrivate) return false;

		DWORD dwLength = 0;
		//SECURITY_DESCRIPTOR *pSDResult = NULL;

		if(!::GetPrivateObjectSecurity(m_pSecurityDescriptor, _si, _pSDResult, dwLength, &dwLength) &&
			::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		{
			return false;
		}

		utils::CTmpBufferOnStack<SECURITY_DESCRIPTOR> pSDResult(dwLength);
		if(!::GetPrivateObjectSecurity(m_pSecurityDescriptor, _si, _pSDResult, dwLength, &dwLength))
			return false;

		*_pSDResult = *pSDResult;

		return true;
	}

	bool Set(
		SECURITY_INFORMATION _si
		,const CSecurityDesc& _Modification
		,PGENERIC_MAPPING _GenericMapping
		,const CAccessToken& _Token
		) THROW0
	{
		if(!m_bPrivate)	return false;

		const SECURITY_DESCRIPTOR* pSDModification = _Modification.GetPSECURITY_DESCRIPTOR();

		return to_bool(::SetPrivateObjectSecurity(_si
				,const_cast<SECURITY_DESCRIPTOR *>(pSDModification)
				,reinterpret_cast<PSECURITY_DESCRIPTOR *>(&m_pSecurityDescriptor)
				,_GenericMapping, _Token.GetHandle()
				)
			);
	}

#if(_WIN32_WINNT >= 0x0500)
	bool Set(
		SECURITY_INFORMATION _si
		,const CSecurityDesc& _Modification
		,ULONG _AutoInheritFlags
		,PGENERIC_MAPPING _GenericMapping
		,const CAccessToken& _Token
		) THROW0
	{
		if(!m_bPrivate) return false;

		const SECURITY_DESCRIPTOR* pSDModification = _Modification.GetPSECURITY_DESCRIPTOR();

		return to_bool(::SetPrivateObjectSecurityEx(_si
				,const_cast<SECURITY_DESCRIPTOR *>(pSDModification)
				,reinterpret_cast<PSECURITY_DESCRIPTOR *>(&m_pSecurityDescriptor)
				,_AutoInheritFlags, _GenericMapping, _Token.GetHandle()
				)
			);
	}

	bool ConvertToAutoInherit(
		const CSecurityDesc* _pParent
		,GUID* _ObjectType
		,bool _bIsDirectoryObject
		,PGENERIC_MAPPING _GenericMapping
		) THROW0
	{
		if(!m_bPrivate)	return false;

		const SECURITY_DESCRIPTOR* pSDParent = _pParent ? _pParent->GetPSECURITY_DESCRIPTOR() : NULL;
		SECURITY_DESCRIPTOR* pSD;

		if(!::ConvertToAutoInheritPrivateObjectSecurity(
				const_cast<SECURITY_DESCRIPTOR *>(pSDParent)
				,m_pSecurityDescriptor
				,reinterpret_cast<PSECURITY_DESCRIPTOR *>(&pSD)
				,_ObjectType, _bIsDirectoryObject, _GenericMapping
				)
			)
		{
			return false;
		}

		Clear();
		m_bPrivate = true;
		m_pSecurityDescriptor = pSD;

		return true;
	}
#endif

protected:
	void Clear() THROW0
	{
		if(m_bPrivate)
		{
			VERIFY(::DestroyPrivateObjectSecurity(reinterpret_cast<PSECURITY_DESCRIPTOR *>(&m_pSecurityDescriptor)));
			m_bPrivate = false;
			m_pSecurityDescriptor = NULL;
		}
		else
			CSecurityDesc::Clear();
	}

private:
	bool m_bPrivate;

	CPrivateObjectSecurityDesc(const CPrivateObjectSecurityDesc &rhs) THROWANY;
	CPrivateObjectSecurityDesc &operator=(const CPrivateObjectSecurityDesc &rhs) THROWANY;
};//class CPrivateObjectSecurityDesc

namespace security
{
//*******************************************
// Global Functions

inline bool GetSecurityDescriptor(
	LPCTSTR _pszObjectName
	,SE_OBJECT_TYPE _ObjectType
	,CSecurityDesc* _pSecurityDescriptor
	,SECURITY_INFORMATION _requestedInfo =
		OWNER_SECURITY_INFORMATION 
		| GROUP_SECURITY_INFORMATION 
		| DACL_SECURITY_INFORMATION 
		| SACL_SECURITY_INFORMATION
	,bool _bRequestNeededPrivileges = true
	) THROWANY
{
	VERIFY_EXIT1(NOT_NULL(_pSecurityDescriptor),false);

	SECURITY_DESCRIPTOR* pSD = NULL;
	DWORD dwErr = ERROR_SUCCESS;

	CAccessToken at;
	CTokenPrivileges TokenPrivileges;

	if (!(_requestedInfo & SACL_SECURITY_INFORMATION))
		_bRequestNeededPrivileges = false;

	if(_bRequestNeededPrivileges)
	{
		if(!at.OpenThreadToken(TOKEN_IMPERSONATE | TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY
				,false, false, SecurityImpersonation
				) 
			|| !at.EnablePrivilege(SE_SECURITY_NAME, &TokenPrivileges) 
			|| !at.Impersonate()
			)
			return false;
	}

//#pragma warning(push)
//#pragma warning(disable: 6309 6387)
	/* psdk prefast noise 498088 */
	dwErr = ::GetNamedSecurityInfo(const_cast<LPTSTR>(_pszObjectName), _ObjectType, _requestedInfo, NULL, NULL, NULL, NULL, (PSECURITY_DESCRIPTOR *) &pSD);
//#pragma warning(pop)

	if(_bRequestNeededPrivileges)
	{
		if( !at.EnableDisablePrivileges(TokenPrivileges) )
			return false;
		if( !at.Revert() )
			return false;
	}

	if(dwErr != ERROR_SUCCESS)
	{
		::SetLastError(dwErr);
		return false;
	}
	::SetLastError(ERROR_SUCCESS);

	*_pSecurityDescriptor = *pSD;
	::LocalFree(pSD);
	return true;
}

inline bool GetSecurityDescriptor(
	HANDLE _hObject
	,SE_OBJECT_TYPE _ObjectType
	,CSecurityDesc* _pSecurityDescriptor
	,SECURITY_INFORMATION _requestedInfo =
		OWNER_SECURITY_INFORMATION |
		GROUP_SECURITY_INFORMATION |
		DACL_SECURITY_INFORMATION |
		SACL_SECURITY_INFORMATION
	,bool _bRequestNeededPrivileges = true
	) THROWANY
{
	VERIFY_EXIT1(NOT_NULL(_pSecurityDescriptor),false);

	SECURITY_DESCRIPTOR* pSD = NULL;
	DWORD dwErr = ERROR_SUCCESS;

	CAccessToken at;
	CTokenPrivileges TokenPrivileges;

	if(!(_requestedInfo & SACL_SECURITY_INFORMATION))
		_bRequestNeededPrivileges = false;

	if (_bRequestNeededPrivileges)
	{
		if(!at.OpenThreadToken(TOKEN_IMPERSONATE | TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY
				,false, false, SecurityImpersonation
				)
			|| !at.EnablePrivilege(SE_SECURITY_NAME, &TokenPrivileges) 
			|| !at.Impersonate()
			)
			return false;
	}

//#pragma warning(push)
//#pragma warning(disable: 6309 6387)
	/* psdk prefast noise 498088 */
	dwErr = ::GetSecurityInfo(_hObject, _ObjectType, _requestedInfo, NULL, NULL, NULL, NULL, reinterpret_cast<PSECURITY_DESCRIPTOR *>(&pSD));
//#pragma warning(pop)

	if(_bRequestNeededPrivileges)
	{
		if(!at.EnableDisablePrivileges(TokenPrivileges)) return false;
		if(!at.Revert()) return false;
	}

	if(dwErr!=ERROR_SUCCESS)
	{
		::SetLastError(dwErr);
		return false;
	}
	::SetLastError(ERROR_SUCCESS);

	*_pSecurityDescriptor = *pSD;
	::LocalFree(pSD);
	return true;
}

inline bool GetOwnerSid(
	HANDLE _hObject
	,SE_OBJECT_TYPE _ObjectType
	,CSid *_pSid
	) THROWANY
{
	VERIFY_EXIT1(NOT_NULL(_pSid),false);

	SID* pOwner = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;

//#pragma warning(push)
//#pragma warning(disable: 6309 6387)
	/* psdk prefast noise 498088 */
	DWORD dwErr = ::GetSecurityInfo(_hObject, _ObjectType, OWNER_SECURITY_INFORMATION, (PSID *) &pOwner, NULL, NULL, NULL, &pSD);
//#pragma warning(pop)

	if(dwErr != ERROR_SUCCESS)
	{
		::SetLastError(dwErr);
		return false;
	}
	::SetLastError(ERROR_SUCCESS);

	*_pSid = *pOwner;
	::LocalFree(pSD);
	return true;
}

inline bool SetOwnerSid(
	HANDLE _hObject
	,SE_OBJECT_TYPE _ObjectType
	,const CSid& _rSid
	) THROWANY
{
	VERIFY_EXIT1(NOT_NULL(_hObject) && NEQL(_hObject,INVALID_HANDLE_VALUE) && _rSid.IsValid(),false);

	DWORD dwErr = ::SetSecurityInfo(_hObject, _ObjectType, OWNER_SECURITY_INFORMATION
		,const_cast<SID*>(_rSid.GetPSID()), NULL, NULL, NULL
		);

	::SetLastError(dwErr);
	return ERROR_SUCCESS == dwErr;
}

inline bool GetOwnerSid(
	LPCTSTR _pszObjectName
	,SE_OBJECT_TYPE _ObjectType
	,CSid* _pSid
	) THROWANY
{
	VERIFY_EXIT1(NOT_NULL(_pszObjectName) && NOT_NULL(_pSid),false);

	SID* pOwner = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
//#pragma warning(push)
//#pragma warning(disable: 6309 6387)
	/* psdk prefast noise 498088 */
	DWORD dwErr = ::GetNamedSecurityInfo(const_cast<LPTSTR>(_pszObjectName), _ObjectType,	OWNER_SECURITY_INFORMATION, reinterpret_cast<PSID *>(&pOwner), NULL, NULL, NULL, &pSD);
//#pragma warning(pop)

	if(dwErr != ERROR_SUCCESS)
	{
		::SetLastError(dwErr);
		return false;
	}
	::SetLastError(ERROR_SUCCESS);

	*_pSid = *pOwner;
	::LocalFree(pSD);
	return true;
}

inline bool SetOwnerSid(
	LPCTSTR _pszObjectName
	,SE_OBJECT_TYPE _ObjectType
	,const CSid& _rSid
	) THROWANY
{
	VERIFY_EXIT1(NOT_NULL(_pszObjectName) && _rSid.IsValid(),false);

	DWORD dwErr = ::SetNamedSecurityInfo(const_cast<LPTSTR>(_pszObjectName), _ObjectType
		,OWNER_SECURITY_INFORMATION, const_cast<SID *>(_rSid.GetPSID()), NULL, NULL, NULL
		);

	::SetLastError(dwErr);
	return ERROR_SUCCESS == dwErr;
}

inline bool GetGroupSid(
	HANDLE hObject
	,SE_OBJECT_TYPE ObjectType
	,CSid* pSid
	) THROWANY
{
	VERIFY_EXIT1(NOT_NULL(hObject) && NOT_NULL(pSid),false);

	SID* pGroup = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
//#pragma warning(push)
//#pragma warning(disable: 6309 6387)
	/* psdk prefast noise 498088 */
	DWORD dwErr = ::GetSecurityInfo(hObject, ObjectType, GROUP_SECURITY_INFORMATION, NULL, reinterpret_cast<PSID *>(&pGroup), NULL, NULL, &pSD);
//#pragma warning(pop)

	if(dwErr != ERROR_SUCCESS)
	{
		::SetLastError(dwErr);
		return false;
	}
	::SetLastError(ERROR_SUCCESS);

	*pSid = *pGroup;
	::LocalFree(pSD);
	return true;
}

inline bool SetGroupSid(
	HANDLE _hObject
	,SE_OBJECT_TYPE _ObjectType
	,const CSid& _rSid
	) THROWANY
{
	VERIFY_EXIT1(NOT_NULL(_hObject) && _rSid.IsValid(),false);

	DWORD dwErr = ::SetSecurityInfo(_hObject, _ObjectType, GROUP_SECURITY_INFORMATION
		,NULL, const_cast<SID *>(_rSid.GetPSID()), NULL, NULL
		);

	::SetLastError(dwErr);
	return ERROR_SUCCESS == dwErr;
}

inline bool GetGroupSid(
	LPCTSTR _pszObjectName
	,SE_OBJECT_TYPE _ObjectType
	,CSid* _pSid
	) THROWANY
{
	VERIFY_EXIT1(NOT_NULL(_pszObjectName) && NOT_NULL(_pSid),false);

	SID* pGroup = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
//#pragma warning(push)
//#pragma warning(disable: 6309 6387)
	/* psdk prefast noise 498088 */
	DWORD dwErr = ::GetNamedSecurityInfo(
		const_cast<LPTSTR>(_pszObjectName)
		,_ObjectType, GROUP_SECURITY_INFORMATION, NULL
		,reinterpret_cast<PSID*>(&pGroup), NULL, NULL, &pSD
		);
//#pragma warning(pop)

	if(dwErr != ERROR_SUCCESS)
	{
		::SetLastError(dwErr);
		return false;
	}
	::SetLastError(ERROR_SUCCESS);

	*_pSid = *pGroup;
	::LocalFree(pSD);
	return true;
}

inline bool SetGroupSid(
	LPCTSTR _pszObjectName
	,SE_OBJECT_TYPE _ObjectType
	,const CSid& _rSid
	) THROWANY
{
	VERIFY_EXIT1(NOT_NULL(_pszObjectName) && _rSid.IsValid(),false);

	DWORD dwErr = ::SetNamedSecurityInfo(const_cast<LPTSTR>(_pszObjectName)
		,_ObjectType
		,GROUP_SECURITY_INFORMATION, NULL, const_cast<SID*>(_rSid.GetPSID())
		,NULL, NULL
		);

	::SetLastError(dwErr);
	return ERROR_SUCCESS == dwErr;
}

inline bool GetDacl(
	HANDLE hObject
	,SE_OBJECT_TYPE ObjectType
	,CDacl *pDacl
	) THROWANY
{
	VERIFY_EXIT1(NOT_NULL(hObject) && NOT_NULL(pDacl),false);

	ACL* pAcl = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;

//#pragma warning(push)
//#pragma warning(disable: 6309 6387)
	/* psdk prefast noise 498088 */
	DWORD dwErr = ::GetSecurityInfo(
		hObject, ObjectType
		,DACL_SECURITY_INFORMATION, NULL, NULL
		,&pAcl, NULL
		,&pSD
		);
//#pragma warning(pop)

	if(dwErr != ERROR_SUCCESS)
	{
		::SetLastError(dwErr);
		return false;
	}
	::SetLastError(ERROR_SUCCESS);

	if(pAcl) *pDacl = *pAcl;
	::LocalFree(pSD);

	return NULL != pAcl;
}

inline bool SetDacl(
	HANDLE hObject
	,SE_OBJECT_TYPE ObjectType
	,const CDacl &rDacl
	,DWORD dwInheritanceFlowControl = 0
	) THROWANY
{
	VERIFY_EXIT1(NOT_NULL(hObject),false);

	ASSERT_(dwInheritanceFlowControl == 0 
		|| dwInheritanceFlowControl == PROTECTED_DACL_SECURITY_INFORMATION 
		|| dwInheritanceFlowControl == UNPROTECTED_DACL_SECURITY_INFORMATION
		);

	DWORD dwErr = ::SetSecurityInfo(hObject, ObjectType
		,DACL_SECURITY_INFORMATION | dwInheritanceFlowControl
		,NULL, NULL, const_cast<ACL*>(rDacl.GetPACL())
		,NULL
		);

	::SetLastError(dwErr);
	return ERROR_SUCCESS == dwErr;
}

inline bool GetDacl(
	LPCTSTR _pszObjectName
	,SE_OBJECT_TYPE _ObjectType
	,CDacl* _pDacl
	) THROWANY
{
	VERIFY_EXIT1(NOT_NULL(_pszObjectName) && NOT_NULL(_pDacl),false);

	ACL* pAcl = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;

//#pragma warning(push)
//#pragma warning(disable: 6309 6387)
	/* psdk prefast noise 498088 */
	DWORD dwErr = ::GetNamedSecurityInfo(
		const_cast<LPTSTR>(_pszObjectName), _ObjectType
		,DACL_SECURITY_INFORMATION, NULL, NULL
		,&pAcl, NULL, &pSD
		);
//#pragma warning(pop)

	if(dwErr!=ERROR_SUCCESS)
	{
		::SetLastError(dwErr);
		return false;
	}
	::SetLastError(ERROR_SUCCESS);

	if(pAcl)
		*_pDacl = *pAcl;
	::LocalFree(pSD);

	return NULL != pAcl;
}

inline bool SetDacl(
	LPCTSTR _pszObjectName
	,SE_OBJECT_TYPE _ObjectType
	,const CDacl& _rDacl
	,DWORD _dwInheritanceFlowControl = 0
	) THROWANY
{
	VERIFY_EXIT1(NOT_NULL(_pszObjectName),false);

	ASSERT_(_dwInheritanceFlowControl == 0 
		|| _dwInheritanceFlowControl == PROTECTED_DACL_SECURITY_INFORMATION 
		|| _dwInheritanceFlowControl == UNPROTECTED_DACL_SECURITY_INFORMATION
		);

	DWORD dwErr = ::SetNamedSecurityInfo(const_cast<LPTSTR>(_pszObjectName), _ObjectType
		,DACL_SECURITY_INFORMATION | _dwInheritanceFlowControl
		,NULL, NULL, const_cast<ACL *>(_rDacl.GetPACL()), NULL
		);

	::SetLastError(dwErr);
	return ERROR_SUCCESS == dwErr;
}

inline bool GetSacl(
	HANDLE _hObject
	,SE_OBJECT_TYPE _ObjectType
	,CSacl* _pSacl
	,bool _bRequestNeededPrivileges = true
	) THROWANY
{
	VERIFY_EXIT1(NOT_NULL(_hObject) && NOT_NULL(_pSacl),false);

	ACL* pAcl = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
	CAccessToken at;
	CTokenPrivileges TokenPrivileges;

	if(_bRequestNeededPrivileges)
	{
		if(!at.OpenThreadToken(TOKEN_IMPERSONATE | TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY
				,false, false, SecurityImpersonation) 
			|| !at.EnablePrivilege(SE_SECURITY_NAME, &TokenPrivileges) 
			|| !at.Impersonate()
			)
			return false;
	}

//#pragma warning(push)
//#pragma warning(disable: 6309 6387)
	/* psdk prefast noise 498088 */
	DWORD dwErr = ::GetSecurityInfo(_hObject, _ObjectType
		,SACL_SECURITY_INFORMATION, NULL, NULL, NULL
		,&pAcl, &pSD
		);
//#pragma warning(pop)

	if(_bRequestNeededPrivileges)
	{
        if(!at.EnableDisablePrivileges(TokenPrivileges)) return false;
		if(!at.Revert()) return false;
	}

	if(dwErr!=ERROR_SUCCESS)
	{
		::SetLastError(dwErr);
		return false;
	}
	::SetLastError(ERROR_SUCCESS);

	if(pAcl) *_pSacl = *pAcl;
	::LocalFree(pSD);

	return NULL != pAcl;
}

inline bool SetSacl(
	HANDLE _hObject
	,SE_OBJECT_TYPE _ObjectType
	,const CSacl& _rSacl
	,DWORD _dwInheritanceFlowControl = 0
	,bool _bRequestNeededPrivileges = true
	) THROWANY
{
	VERIFY_EXIT1(NOT_NULL(_hObject),false);

	CAccessToken at;
	CTokenPrivileges TokenPrivileges;

	ASSERT_(_dwInheritanceFlowControl == 0 
		|| _dwInheritanceFlowControl == PROTECTED_SACL_SECURITY_INFORMATION 
		|| _dwInheritanceFlowControl == UNPROTECTED_SACL_SECURITY_INFORMATION
		);

	if(_bRequestNeededPrivileges)
	{
		if(!at.OpenThreadToken(TOKEN_IMPERSONATE | TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY
				,false, false, SecurityImpersonation) 
			|| !at.EnablePrivilege(SE_SECURITY_NAME, &TokenPrivileges) 
			|| !at.Impersonate()
			)
			return false;
	}

	DWORD dwErr = ::SetSecurityInfo(_hObject, _ObjectType
		,SACL_SECURITY_INFORMATION | _dwInheritanceFlowControl
		,NULL, NULL, NULL, const_cast<ACL *>(_rSacl.GetPACL())
		);

	if(_bRequestNeededPrivileges)
	{
		if(!at.EnableDisablePrivileges(TokenPrivileges)) return false;
		if(!at.Revert()) return false;
	}

	::SetLastError(dwErr);
	return ERROR_SUCCESS == dwErr;
}

inline bool GetSacl(
	LPCTSTR _pszObjectName
	,SE_OBJECT_TYPE _ObjectType
	,CSacl* _pSacl
	,bool _bRequestNeededPrivileges = true
	) THROWANY
{
	VERIFY_EXIT1(NOT_NULL(_pszObjectName) && NOT_NULL(_pSacl),false);

	ACL* pAcl = NULL;
	PSECURITY_DESCRIPTOR pSD;
	CAccessToken at;
	CTokenPrivileges TokenPrivileges;

	if(_bRequestNeededPrivileges)
	{
		if(!at.OpenThreadToken(TOKEN_IMPERSONATE | TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY
				,false, false, SecurityImpersonation) 
			|| !at.EnablePrivilege(SE_SECURITY_NAME, &TokenPrivileges) 
			|| !at.Impersonate()
			)
			return false;
	}

//#pragma warning(push)
//#pragma warning(disable: 6309 6387)
	/* psdk prefast noise 498088 */
	DWORD dwErr = ::GetNamedSecurityInfo(
		const_cast<LPTSTR>(_pszObjectName)
		,_ObjectType
		,SACL_SECURITY_INFORMATION
		,NULL,NULL,NULL
		,&pAcl, &pSD
		);
//#pragma warning(pop)

	if(_bRequestNeededPrivileges)
	{
		at.EnableDisablePrivileges(TokenPrivileges);
		at.Revert();
	}

	::SetLastError(dwErr);
	if(dwErr != ERROR_SUCCESS)
		return false;

	if(pAcl) *_pSacl = *pAcl;
	::LocalFree(pSD);

	return NULL!=pAcl;
}

inline bool SetSacl(
	LPCTSTR pszObjectName
	,SE_OBJECT_TYPE ObjectType
	,const CSacl& rSacl
	,DWORD dwInheritanceFlowControl = 0
	,bool bRequestNeededPrivileges = true
	) THROWANY
{
	VERIFY_EXIT1(NOT_NULL(pszObjectName),false);

	CAccessToken at;
	CTokenPrivileges TokenPrivileges;

	ASSERT_(dwInheritanceFlowControl == 0 
		|| dwInheritanceFlowControl == PROTECTED_SACL_SECURITY_INFORMATION 
		|| dwInheritanceFlowControl == UNPROTECTED_SACL_SECURITY_INFORMATION
		);

	if(bRequestNeededPrivileges)
	{
		if(!at.OpenThreadToken(TOKEN_IMPERSONATE | TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY
				,false, false, SecurityImpersonation) 
			|| !at.EnablePrivilege(SE_SECURITY_NAME, &TokenPrivileges) 
			|| !at.Impersonate()
			)
			return false;
	}

	DWORD dwErr = ::SetNamedSecurityInfo(const_cast<LPTSTR>(pszObjectName), ObjectType
		,SACL_SECURITY_INFORMATION | dwInheritanceFlowControl
		,NULL, NULL, NULL, const_cast<ACL *>(rSacl.GetPACL())
		);

	if(bRequestNeededPrivileges)
	{
		at.EnableDisablePrivileges(TokenPrivileges);
		at.Revert();
	}

	::SetLastError(dwErr);
	return ERROR_SUCCESS == dwErr;
}

};//namespace security
//@}

//#pragma warning(pop)


#endif //#if !defined(__SECURITY_H__2CC8BC6A_BDE5_4238_A1C4_F821081F3278)
