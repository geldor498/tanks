#pragma once

#include <utils/serialize.h>
#include <utils/synchronize.h>
#include <utils/security.h>
#include "../utility/graphics.h"
#include "GameConsts.h"

struct CMessage
{
	virtual ~CMessage()
	{
	}

	virtual GUID get_message_guid() = 0;
};

#define GET_MESSAGEGUID(_Class)			\
virtual GUID get_message_guid()			\
{										\
	return _Class::get_guid();			\
}

struct CMessagesQueue
{
	typedef std::list<CMessage*> MessagesLst;

	CMessagesQueue()
	{
		ZeroObj(m_waitguid);
	}

	~CMessagesQueue()
	{
		free_messages();
	}

	void free_messages()
	{
		CAutoLock __al(m_critsect);
		MessagesLst::iterator
			it = m_messages.begin()
			,ite = m_messages.end()
			;
		for(;it!=ite;++it){delete *it;}
		m_messages.clear();
	}

	template<typename _Message>
	bool wait_message(_Message& _msg,bool _exact=true,HANDLE _hstop=NULL)
	{
		GUID guid = _msg.get_message_guid();
		long cnt = 1;
		for(;;cnt=0)
		{
			{
				//try to find message in list
				CAutoLock __al(m_critsect);
				MessagesLst::iterator 
					it = m_messages.begin()
					,ite = m_messages.end()
					;
				for(;it!=ite;++it)
				{
					if((*it)->get_message_guid()==guid)
					{
						_msg = static_cast<_Message&>(**it);
						delete *it;
						m_messages.erase(it);
						return true;
					}
				}
				m_waitguid = guid;
			}
			if(!cnt && _exact) return false;
			DWORD dwWaitRes = 0;
			if(NOT_NULL(_hstop))
			{
				HANDLE h[] = {m_newmessage,_hstop};
				dwWaitRes = WaitForMultipleObjects(_countof(h),h,FALSE,INFINITE);
			}
			else
			{
				dwWaitRes = WaitForSingleObject(m_newmessage,INFINITE);
			}
			if(dwWaitRes!=WAIT_OBJECT_0) return false;
		}
		return false;
	}

	template<typename _Msg>
	bool process(_Msg* _pmsg)
	{
		{
			CAutoLock __al(m_critsect);
			m_messages.push_back(new _Msg(*_pmsg));
		}
		m_newmessage.SetEvent();
		return true;
	}

protected:
	CMessage* next_message() 
	{
		CAutoLock __al(m_critsect);
		if(m_messages.empty()) return NULL;
		CMessage* pmsg = m_messages.front();
		m_messages.erase(m_messages.begin());
		return pmsg;
	}

	CCriticalSection m_critsect;
	MessagesLst m_messages;
	GUID m_waitguid;
	CEvent m_newmessage;
};

// {EE8306F7-2BE6-44b8-B65F-4DF9B7A967AC}
static const GUID g_CShotCmdGUID = 
{ 0xee8306f7, 0x2be6, 0x44b8, { 0xb6, 0x5f, 0x4d, 0xf9, 0xb7, 0xa9, 0x67, 0xac } };

struct CShotCmd 
	:public CMessage
	,public CPipeSerializedDataBaseImpl<CShotCmd>
{
	DECLARE_DATAGUID(g_CShotCmdGUID);
	GET_MESSAGEGUID(CShotCmd);

	long m_id;					// returned by exploit info message 
	double m_fTurretAngle;		// turret rotate angle
	double m_fGunAngle;			// gun angle
	bool m_bShot;				// true if this is a shot

	CShotCmd()
		:m_fTurretAngle(0)
		,m_fGunAngle(0)
		,m_bShot(false)
		,m_id(0)
	{
	}

	struct DefaultLayout : public Layout<CShotCmd>
	{
		DefaultLayout()
		{
			add_simple(_T("id"),&CShotCmd::m_id);
			add_simple(_T("TurretAngle"),&CShotCmd::m_fTurretAngle);
			add_simple(_T("GunAngle"),&CShotCmd::m_fGunAngle);
			add_simple(_T("IsShoot"),&CShotCmd::m_bShot);
		}
	};
};//struct CShotCmd

// {AE79B3D6-0B1E-4867-80E2-AE92C32823A0}
static const GUID g_CMoveCmdGUID = 
{ 0xae79b3d6, 0xb1e, 0x4867, { 0x80, 0xe2, 0xae, 0x92, 0xc3, 0x28, 0x23, 0xa0 } };


struct CMoveCmd 
	:public CMessage
	,public CPipeSerializedDataBaseImpl<CMoveCmd>
{
	DECLARE_DATAGUID(g_CMoveCmdGUID);
	GET_MESSAGEGUID(CMoveCmd);

	double m_fVLeftTrack;	// left track speed
	double m_fVRightTrack;	// right track speed

	CMoveCmd()
		:m_fVLeftTrack(0)
		,m_fVRightTrack(0)
	{
	}

	struct DefaultLayout : public Layout<CMoveCmd>
	{
		DefaultLayout()
		{
			add_simple(_T("VLeftTrack"),&CMoveCmd::m_fVLeftTrack);
			add_simple(_T("VRightTrack"),&CMoveCmd::m_fVRightTrack);
		}
	};
};//struct CMoveCmd

// {ABB3C25E-3962-475e-A7B5-A4A104674E44}
static const GUID g_CTakeArtefactCmdGUID = 
{ 0xabb3c25e, 0x3962, 0x475e, { 0xa7, 0xb5, 0xa4, 0xa1, 0x4, 0x67, 0x4e, 0x44 } };


struct CTakeArtefactCmd 
	:public CMessage
	,public CPipeSerializedDataBaseImpl<CTakeArtefactCmd>
{
	DECLARE_DATAGUID(g_CTakeArtefactCmdGUID);
	GET_MESSAGEGUID(CTakeArtefactCmd);

	CTakeArtefactCmd()
	{
	}

	struct DefaultLayout : public Layout<CTakeArtefactCmd>
	{
		DefaultLayout()
		{
		}
	};
};//struct CTakeArtefactCmd

enum ArtefactTypeEn
{
	Artefact_Null = 0
	//,Artefact_Shell
	,Artefact_Fuel
	,Artefact_Armor
};

struct CArtefactTypeEnDescription : public EnumDescriptionBase<ArtefactTypeEn>
{
	CArtefactTypeEnDescription()
	{
		enumerator(Artefact_Null,_T("NullArtefact"));
		//enumerator(Artefact_Shell,_T("ShellArtefact"));
		enumerator(Artefact_Fuel,_T("FuelArtefact"));
		enumerator(Artefact_Armor,_T("ArmorArtefact"));
	}
};//struct CArtefactTypeEnDescription

DECLARE_ENUMDESCR(ArtefactTypeEn,CArtefactTypeEnDescription);

// {7EA14133-8682-4be1-913C-19E8AC87E4C6}
static const GUID g_CArtefactInfoGUID = 
{ 0x7ea14133, 0x8682, 0x4be1, { 0x91, 0x3c, 0x19, 0xe8, 0xac, 0x87, 0xe4, 0xc6 } };

struct CArtefactInfo 
	:public CMessage
	,public CPipeSerializedDataBaseImpl<CArtefactInfo>
{
	DECLARE_DATAGUID(g_CArtefactInfoGUID);
	GET_MESSAGEGUID(CArtefactInfo);

	ArtefactTypeEn m_type;	// artefact type
	double m_fMass;			// mass of artefact
	long m_ID;				// artefact id. used in operations with artefacts

	CArtefactInfo()
		:m_type(Artefact_Null)
		,m_fMass(0)
		,m_ID(0)
	{
	}

	struct DefaultLayout : public Layout<CArtefactInfo>
	{
		DefaultLayout()
		{
			add_struct(_T("type"),&CArtefactInfo::m_type,get_enumlayout<ArtefactTypeEn>());
			add_simple(_T("Mass"),&CArtefactInfo::m_fMass);
			add_simple(_T("ID"),&CArtefactInfo::m_ID);
		}
	};
};//struct CArtefactInfo

// {46B81284-ED87-4ada-8D10-25E44299B074}
static const GUID g_CUseArtefactCmdGUID = 
{ 0x46b81284, 0xed87, 0x4ada, { 0x8d, 0x10, 0x25, 0xe4, 0x42, 0x99, 0xb0, 0x74 } };

struct CUseArtefactCmd
	:public CMessage
	,public CPipeSerializedDataBaseImpl<CUseArtefactCmd>
{
	DECLARE_DATAGUID(g_CUseArtefactCmdGUID);
	GET_MESSAGEGUID(CUseArtefactCmd);

	long m_ArtefactID;	// artefact id to use it 

	CUseArtefactCmd()
		:m_ArtefactID(0)
	{
	}

	struct DefaultLayout : public Layout<CUseArtefactCmd>
	{
		DefaultLayout()
		{
			add_simple(_T("ArtefactID"),&CUseArtefactCmd::m_ArtefactID);
		}
	};
};//struct CUseArtefactCmd 

// {0AD719CD-76B0-47ab-BFF5-B1B3D4E5D1CD}
static const GUID g_CPutArtefactCmdGUID = 
{ 0xad719cd, 0x76b0, 0x47ab, { 0xbf, 0xf5, 0xb1, 0xb3, 0xd4, 0xe5, 0xd1, 0xcd } };

struct CPutArtefactCmd 
	:public CMessage 
	,public CPipeSerializedDataBaseImpl<CPutArtefactCmd>
{
	DECLARE_DATAGUID(g_CPutArtefactCmdGUID);
	GET_MESSAGEGUID(CPutArtefactCmd);

	long m_ArtefactID;	// artefact id to put it down

	CPutArtefactCmd()
		:m_ArtefactID(0)
	{
	}

	struct DefaultLayout : public Layout<CPutArtefactCmd>
	{
		DefaultLayout()
		{
			add_simple(_T("ArtefactID"),&CPutArtefactCmd::m_ArtefactID);
		}
	};
};//struct CPutArtefactCmd 

// {412BE62A-884D-4f3c-B643-778C9A7666D8}
static const GUID g_CGetRadarInfoGUIDCmd = 
{ 0x412be62a, 0x884d, 0x4f3c, { 0xb6, 0x43, 0x77, 0x8c, 0x9a, 0x76, 0x66, 0xd8 } };

struct CGetRadarInfoCmd 
	:public CMessage 
	,public CPipeSerializedDataBaseImpl<CGetRadarInfoCmd>
{
	DECLARE_DATAGUID(g_CGetRadarInfoGUIDCmd);
	GET_MESSAGEGUID(CGetRadarInfoCmd);

	struct DefaultLayout : public Layout<CGetRadarInfoCmd>
	{
		DefaultLayout()
		{
		}
	};
};//struct CGetRadarInfoCmd

enum ObjectEn
{
	Obj_Null = 0		// returned for placed out of game plane
	,Obj_Ground			// ground
	,Obj_Water			// water
	,Obj_Tree			// trees
	,Obj_Rock			// rocks (and havy metal)))
};

static const TCHAR g_groundsym = 1;
static const TCHAR g_watersym = 2;
static const TCHAR g_treesym = 3;
static const TCHAR g_rocksym = 4;

struct CTankPosition
{
	CFPoint2D m_pos;		// tank position
	long m_id;				// identity of tank on radar 

	CTankPosition(const CFPoint2D& _pos = CFPoint2D(),long _id = -1)
		:m_pos(_pos),m_id(_id)
	{
	}

	struct DefaultLayout : public Layout<CTankPosition>
	{
		DefaultLayout()
		{
			add_struct(_T("pt"),&CTankPosition::m_pos,get_structlayout<CFPoint2D>());
			add_simple(_T("id"),&CTankPosition::m_id);
		}
	};
};

// {26CE5BD0-7097-414b-9499-1DA922D4DEAB}
static const GUID g_CRadarInfoGUID = 
{ 0x26ce5bd0, 0x7097, 0x414b, { 0x94, 0x99, 0x1d, 0xa9, 0x22, 0xd4, 0xde, 0xab } };

struct CRadarInfo 
	:public CMessage
	,public CPipeSerializedDataBaseImpl<CRadarInfo>	
{
	DECLARE_DATAGUID(g_CRadarInfoGUID);
	GET_MESSAGEGUID(CRadarInfo);

	CString m_map;			// for map data
	long m_sz;				// for map data
	CFPoint2D m_pos;		// tank position 
	CFPoint2D m_mappos;		// tank position on map
	std::list<CTankPosition> m_tanks;	// tanks on a radar
	std::list<CFPoint2D> m_artefacts;	// artefacts on a radar

	CRadarInfo()
		:m_sz(0)
	{
	}

	void resize(size_t _sz)
	{
		m_sz = (int)_sz;
		LPTSTR sz = m_map.GetBuffer(m_sz*m_sz+1);
		int i=0;
		for(i=0;i<m_sz*m_sz;i++)
		{
			sz[i] = g_groundsym;
		}
		m_map.ReleaseBuffer(m_sz*m_sz+1);
	}

	ObjectEn get_object(size_t _x,size_t _y,double& _height) const
	{
		_height = 0;
		if(m_sz==0 || (int)(_x)>=m_sz || (int)(_y)>=m_sz) return Obj_Null;
		int pos = (int)(_y)*m_sz + (int)(_x);
		if(m_map.GetLength()<pos) return Obj_Null;
		TCHAR ch = m_map.GetAt(pos);
		switch(ch)
		{
		case 0:
		case g_groundsym: 
			return Obj_Ground;
		case g_watersym: 
			return Obj_Water;
		case g_treesym: 
			return Obj_Tree;
		}
		double rockheight = singleton<CGameConsts>::get().rockheight();
		const long maxv = (1<<(sizeof(TCHAR)*8)) - 1 - g_rocksym;
		_height = (double)(ch-g_rocksym)/maxv*rockheight;
		return Obj_Rock;
	}

	void set_object(size_t _x,size_t _y,ObjectEn _obj,double _height)
	{
		if(m_sz==0 || (int)(_x)>=m_sz || (int)(_y)>=m_sz) return;
		int pos = (int)(_y)*m_sz + (int)(_x);
		if(m_map.GetLength()<pos) return;
		switch(_obj)
		{
		case Obj_Null:
		case Obj_Ground:
			m_map.SetAt(pos,g_groundsym);
			return;
		case Obj_Water:
			m_map.SetAt(pos,g_watersym);
			return;
		case Obj_Tree:
			m_map.SetAt(pos,g_treesym);
			return;
		case Obj_Rock:
			break;
		}
		double rockheight = singleton<CGameConsts>::get().rockheight();
		const long maxv = (1<<(sizeof(TCHAR)*8)) - 1 - g_rocksym;
		TCHAR ch = (TCHAR)floor(_height/rockheight*maxv);
		m_map.SetAt(pos,g_rocksym+ch);
	}

	struct DefaultLayout : public Layout<CRadarInfo>
	{
		DefaultLayout()
		{
			add_simple(_T("map"),&CRadarInfo::m_map);
			add_simple(_T("size"),&CRadarInfo::m_sz);
			add_list(_T("tank"),&CRadarInfo::m_tanks,get_structlayout<CTankPosition>());
			add_list(_T("artefact"),&CRadarInfo::m_artefacts,get_structlayout<CFPoint2D>());
			add_struct(_T("position"),&CRadarInfo::m_pos,get_structlayout<CFPoint2D>());
			add_struct(_T("map_position"),&CRadarInfo::m_mappos,get_structlayout<CFPoint2D>());
		}
	};
};//struct CRadarInfo

// {3664F64E-F886-4ebb-A9AF-A564E577D1F7}
static const GUID g_CGetTankInfoCmdGUID = 
{ 0x3664f64e, 0xf886, 0x4ebb, { 0xa9, 0xaf, 0xa5, 0x64, 0xe5, 0x77, 0xd1, 0xf7 } };

struct CGetTankInfoCmd 
	:public CMessage
	,public CPipeSerializedDataBaseImpl<CGetTankInfoCmd>
{
	DECLARE_DATAGUID(g_CGetTankInfoCmdGUID);
	GET_MESSAGEGUID(CGetTankInfoCmd);

	struct DefaultLayout : public Layout<CGetTankInfoCmd>
	{
		DefaultLayout()
		{
		}
	};
};

// {FEEB411F-F2C1-45e8-A266-E1B74836612F}
static const GUID g_CTankInfoGUID = 
{ 0xfeeb411f, 0xf2c1, 0x45e8, { 0xa2, 0x66, 0xe1, 0xb7, 0x48, 0x36, 0x61, 0x2f } };

struct CTankInfo 
	:public CMessage
	,public CPipeSerializedDataBaseImpl<CTankInfo>
{
	DECLARE_DATAGUID(g_CTankInfoGUID);
	GET_MESSAGEGUID(CTankInfo)

	CFPoint2D m_pos;
	CFPoint2D m_fOrt;
	double m_fDirection;
	double m_fVLeftTrack;
	double m_fVRightTrack;
	double m_fMass;
	double m_fArmor;
	double m_fFuel;
	std::list<CArtefactInfo> m_artefacts;
	

	CTankInfo()
		:m_fMass(0)
		,m_fVLeftTrack(0)
		,m_fVRightTrack(0)
		,m_fArmor(0)
		,m_fFuel(0)
		,m_fDirection(0)
	{
	}

	struct DefaultLayout : public Layout<CTankInfo>
	{
		DefaultLayout()
		{
			add_struct(_T("position"),&CTankInfo::m_pos,get_structlayout<CFPoint2D>());
			add_struct(_T("ort"),&CTankInfo::m_fOrt,get_structlayout<CFPoint2D>());
			add_simple(_T("direction"),&CTankInfo::m_fDirection);
			add_simple(_T("mass"),&CTankInfo::m_fMass);
			add_simple(_T("VLeftTrack"),&CTankInfo::m_fVLeftTrack);
			add_simple(_T("VRightTrack"),&CTankInfo::m_fVRightTrack);
			add_simple(_T("Armor"),&CTankInfo::m_fArmor);
			add_simple(_T("Fuel"),&CTankInfo::m_fFuel);
			add_list(_T("Artefact"),&CTankInfo::m_artefacts,get_structlayout<CArtefactInfo>());
		}
	};
};//struct CTankInfo

// {3D96119D-8D2B-4c9d-8DC7-6C906BECC895}
static const GUID g_CGetExploitsInfoCmdGUID = 
{ 0x3d96119d, 0x8d2b, 0x4c9d, { 0x8d, 0xc7, 0x6c, 0x90, 0x6b, 0xec, 0xc8, 0x95 } };

struct CGetExploitsInfoCmd
	:public CMessage
	,public CPipeSerializedDataBaseImpl<CGetExploitsInfoCmd>	
{
	DECLARE_DATAGUID(g_CGetExploitsInfoCmdGUID);
	GET_MESSAGEGUID(CGetExploitsInfoCmd);

	CGetExploitsInfoCmd()
	{
	}

	struct DefaultLayout : public Layout<CGetExploitsInfoCmd>
	{
		DefaultLayout()
		{
		}
	};
};

struct CExploitInfo
{
	long m_id;			// shell id. that was send by CShootCmd
	CFPoint3D m_pt;		// end point of shell. exploit coordinates
	double m_fFlyTime;	// fly time in seconds
	double m_fHitPoints;	// armor loss of tank when there was a hit

	CExploitInfo()
		:m_id(0)
		,m_fFlyTime(0)
		,m_fHitPoints(0)
	{
	}

	struct DefaultLayout : public Layout<CExploitInfo>
	{
		DefaultLayout()
		{
			add_simple(_T("id"),&CExploitInfo::m_id);
			add_struct(_T("pt"),&CExploitInfo::m_pt,get_structlayout<CFPoint3D>());
			add_simple(_T("FlyTime"),&CExploitInfo::m_fFlyTime);
			add_simple(_T("HitPoints"),&CExploitInfo::m_fHitPoints);
		}
	};
};

// {405E90DF-401E-491a-960F-A65A39DB21AF}
static const GUID g_CExploitsInfoGUID = 
{ 0x405e90df, 0x401e, 0x491a, { 0x96, 0xf, 0xa6, 0x5a, 0x39, 0xdb, 0x21, 0xaf } };

struct CExploitsInfo
	:public CMessage
	,public CPipeSerializedDataBaseImpl<CExploitsInfo>
{
	DECLARE_DATAGUID(g_CExploitsInfoGUID);
	GET_MESSAGEGUID(CExploitsInfo);

	std::list<CExploitInfo> m_exploits;	// exploits of you shells

	struct DefaultLayout : public Layout<CExploitsInfo>
	{
		DefaultLayout()
		{
			add_list(_T("exploits"),&CExploitsInfo::m_exploits,get_structlayout<CExploitInfo>());
		}
	};
};

struct CFlagColor : public CColor
{
	CFlagColor& operator =(const COLORREF& _clr)
	{
		m_data = _clr;
		return *this;
	}

	struct DefaultLayout : public Layout<CFlagColor>
	{
		DefaultLayout()
		{
			add_simple(_T("Color"),&CFlagColor::m_data);
		}
	};
};

// {8A91B602-D920-45f9-BBD5-F61E8BA2C65A}
static const GUID g_CHelloWorldGUID = 
{ 0x8a91b602, 0xd920, 0x45f9, { 0xbb, 0xd5, 0xf6, 0x1e, 0x8b, 0xa2, 0xc6, 0x5a } };

struct CHelloWorld 
	:public CMessage
	,public CPipeSerializedDataBaseImpl<CHelloWorld>
{
	DECLARE_DATAGUID(g_CHelloWorldGUID);
	GET_MESSAGEGUID(CHelloWorld);

	CString m_sTankName;	// tank name
	CString m_sTeamName;	// tank team name
	CString m_sTankClientComputer;	// computer name. used to connect server to client
	CString m_sPipeName;	// used to connect server to client
	CFlagColor m_flag[3];	// flag colors

	struct DefaultLayout : public Layout<CHelloWorld>
	{
		DefaultLayout()
		{
			add_simple(_T("tankname"),&CHelloWorld::m_sTankName);
			add_simple(_T("teamname"),&CHelloWorld::m_sTeamName);
			add_simple(_T("clientcomputer"),&CHelloWorld::m_sTankClientComputer);
			add_simple(_T("pipeName"),&CHelloWorld::m_sPipeName);
			add_array(_T("Flag"),&CHelloWorld::m_flag,get_structlayout<CFlagColor>());
		}
	};
};

// {6F655479-8F1E-4984-B922-EEEE7E680ED1}
static const GUID g_CHandShackGUID = 
{ 0x6f655479, 0x8f1e, 0x4984, { 0xb9, 0x22, 0xee, 0xee, 0x7e, 0x68, 0xe, 0xd1 } };

struct CHandShack 
	:public CMessage
	,public CPipeSerializedDataBaseImpl<CHandShack>
{
	DECLARE_DATAGUID(g_CHandShackGUID);
	GET_MESSAGEGUID(CHandShack);

	CString m_sTankPipeID;	// server pipe name. used to connect client to server

	struct DefaultLayout : public Layout<CHandShack>
	{
		DefaultLayout()
		{
			add_simple(_T("TankID"),&CHandShack::m_sTankPipeID);
		}
	};
};

typedef TYPELIST_4(CArtefactInfo,CRadarInfo,CTankInfo,CHandShack) TankClientMessages;

struct CClientTank 
	:public CServerPipeCommImpl<CClientTank,TankClientMessages>
	,public CMessagesQueue
{
protected:
	CSecurityAttributes m_sa;
public:
	CClientTank(IN const CString& _sTeamName,IN const CString& _sTankName,IN const CString& _sClientName)
		:CServerPipeCommImpl(_sClientName)
		,m_sTeamName(_sTeamName)
		,m_sClientPipeName(_sClientName)
		,m_sTankName(_sTankName)
	{
		CSid everyone = security::Sids::World();
		CSid owner = security::Sids::Self();

		CDacl dacl;
		dacl.AddDeniedAce(everyone,security::AccessRights_WriteDac);
		dacl.AddAllowedAce(everyone,security::AccessRights_GenericWrite|security::AccessRights_GenericRead);
		dacl.AddAllowedAce(owner,security::AccessRights_GenericAll);

		CSecurityDesc secrdesc;
		secrdesc.SetDacl(dacl);

		m_sa.Set(secrdesc);
		m_psa = &m_sa;

		start();
		Sleep(20);
	}

	~CClientTank()
	{
		predestruct();
	}

	bool connect(IN const CString& _sServerCompName,IN const CString& _sServerPipeName,IN const CFlagColor (&_flag)[3])
	{
		CClientPipeComm serv;
		if(!serv.open(_sServerCompName,_sServerPipeName)) return false;
		CString sThisCompName;
		GetComputerName(sThisCompName);
		CHelloWorld hw;
		hw.m_sTankClientComputer = sThisCompName;
		hw.m_sPipeName = m_sClientPipeName;
		hw.m_sTankName = m_sTankName;
		hw.m_sTeamName = m_sTeamName;
		size_t i=0;
		for(i=0;i<_countof(hw.m_flag);i++)
			hw.m_flag[i] = _flag[i];
		serv.save(&hw);
		CHandShack hs;
		if(!wait_message(hs,true,get_stophandle())) return false;
		return m_server.open(_sServerCompName,hs.m_sTankPipeID);
	}

	void shot(IN double _fTurretAngle,IN double _fGunAngle,IN bool _bShot,IN long _nID)
	{
		CShotCmd shot;
		shot.m_fTurretAngle = _fTurretAngle;
		shot.m_fGunAngle = _fGunAngle;
		shot.m_bShot = _bShot;
		shot.m_id = _nID;
		m_server.save(&shot);
	}

	void move(IN double _fLeftTrackSpeed,IN double _fRightTrackSpeed)
	{
		CMoveCmd move;
		move.m_fVLeftTrack = _fLeftTrackSpeed;
		move.m_fVRightTrack = _fRightTrackSpeed;
		m_server.save(&move);
	}

	void take_artefact(OUT CArtefactInfo& _artefact)
	{
		CTakeArtefactCmd cmd;
		m_server.save(&cmd);
		wait_message(_artefact,true,get_stophandle());
	}

	void use_artefact(IN long _nArtefactID)
	{
		CUseArtefactCmd cmd;
		cmd.m_ArtefactID = _nArtefactID;
		m_server.save(&cmd);
	}

	void put_artefact(IN long _nArtefactID)
	{
		CPutArtefactCmd cmd;
		cmd.m_ArtefactID = _nArtefactID;
		m_server.save(&cmd);
	}

	void get_radarinfo(OUT CRadarInfo& _info)
	{
		CGetRadarInfoCmd cmd;
		m_server.save(&cmd);
		wait_message(_info,true,get_stophandle());
	}

	void get_tankinfo(OUT CTankInfo& _tankinfo)
	{
		CGetTankInfoCmd cmd;
		m_server.save(&cmd);
		wait_message(_tankinfo,true,get_stophandle());
	}

	void get_exploitsinfo(OUT CExploitsInfo& _exploitsinfo)
	{
		CGetExploitsInfoCmd cmd;
		m_server.save(&cmd);
		wait_message(_exploitsinfo,true,get_stophandle());
	}

	template<typename _Message>
	bool process(_Message* _pmsg)
	{
		return CMessagesQueue::process(_pmsg);
	}

protected:
	CClientPipeComm m_server;
	CString m_sClientPipeName;
	CString m_sTeamName;
	CString m_sTankName;
};//struct CTank 

PIPESERVRERTRAITS(CClientTank,TankClientMessages,false,true);

static const LPCTSTR g_szTankBattleConnectServer = _T("TankBattleConnectServer");