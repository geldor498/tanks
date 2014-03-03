#pragma once

#include "../utility/graphics.h"
#include "messages.h"
#include "phisics.h"
#include <fstream>
#include <math.h>
using std::ofstream;

#define M_PI       3.14159265358979323846

template<class _TOwner,typename _TObject,class _TCommandsTL>
struct ProcessCommand
{
	typedef typename _TCommandsTL::Item Item;
	typedef typename _TCommandsTL::Tail Tail;

	static void process_command(_TOwner& _processor,CMessage* _pmsg,_TObject* _pobj)
	{
		if(_pmsg->get_message_guid()==Item::get_guid())
		{
			_processor.process_command(static_cast<Item&>(*_pmsg),*_pobj);
			return;
		}
		ProcessCommand<_TOwner,_TObject,Tail>::process_command(_processor,_pmsg,_pobj);
	}
};

template<class _TOwner,typename _TObject>
struct ProcessCommand<_TOwner,_TObject,NullType>
{
	static void process_command(_TOwner& _processor,CMessage* _pmsg,_TObject* _pobj)
	{
	}
};



typedef TYPELIST_8(CShotCmd,CMoveCmd,CTakeArtefactCmd,CUseArtefactCmd,CPutArtefactCmd,CGetRadarInfoCmd,CGetTankInfoCmd,CGetExploitsInfoCmd) TankServerMessages;

// Model classes of MVC architecture
class CTank
	:public CServerPipeCommImpl<CTank,TankServerMessages>
	,public CMessagesQueue
	,public CPhisicsTank
{
protected:
	CString m_sTeamName;
	CString m_sTankName;
	CClientPipeComm m_client;
	CFlagColor m_flag[3];
	double m_LTrackPos;
	double m_RTrackPos;
	long m_nTankID;
	HANDLE m_hGameFlag;

protected:
	CCriticalSection m_critsect;
public:
	CTank(
		const CString& _sClientComputerName
		,const CString& _sClientPipeName
		,const CString& _sServerPipeName	
		,const CString& _sTeamName
		,const CString& _sTankName
		,const CFlagColor (&_flag)[3]
		,long _nTankID
		,HANDLE _hGameFlag
		)	
		:CServerPipeCommImpl(_sServerPipeName)
		,m_LTrackPos(0)
		,m_RTrackPos(0)
		,m_sTeamName(_sTeamName)
		,m_sTankName(_sTankName)
		,m_hGameFlag(_hGameFlag)
	{
		m_nTankID = _nTankID;
		start();
		Sleep(100);
		m_client.open(_sClientComputerName,_sClientPipeName);
		size_t i=0;
		for(i=0;i<_countof(m_flag);i++)
			m_flag[i] = _flag[i];
	}

	long get_TankID() const {return m_nTankID;}

	template<typename _Message>
	bool process(_Message* _pmsg)
	{
		//if(is_signaled(m_hGameFlag,0))
			return CMessagesQueue::process(_pmsg);
		//else
		//	return true;
	}

	CString get_name() const {return m_sTankName;}

	template<typename _CommandTarget>
	bool process_command(_CommandTarget& _cmdtarget)
	{
		CMessage* pmsg = next_message();
		if(IS_NULL(pmsg)) return false;
		ProcessCommand<_CommandTarget,CTank,TankServerMessages>::process_command(
			_cmdtarget,pmsg,this
			);
		delete pmsg;
		return true;
	}

	void on_error(const utils::IUtilitiesException& _exc,bool& _bCancel)
	{
		CString_ msg = FormatException(_exc);
	}

	void run(double _dt)
	{
		CAutoLock __al(m_critsect);
		this->update(_dt);
		m_LTrackPos = m_chassis.m_power_left_track_required*_dt;
		m_RTrackPos = m_chassis.m_power_right_track_required*_dt;
	}

	void set_tracksv(double _fVLeftTrack,double _fVRightTrack)
	{
		CAutoLock __al(m_critsect);
		m_chassis.m_power_left_track_required = _fVLeftTrack;
		m_chassis.m_power_right_track_required = _fVRightTrack;
	}

	void set_gun(double _fTurretAngle,double _fGunAngle, bool _bShot)
	{
		CAutoLock __al(m_critsect);
		m_turret.n_GunAngle = _fGunAngle;
		m_turret.n_TurretAngle = _fTurretAngle;
		m_turret.m_bShot = _bShot;
	}

	void set_pos(const CFPoint2D& _pos)
	{
		CAutoLock __al(m_critsect);
		m_mass_center.x = _pos.x;
		m_mass_center.y = _pos.y;
		m_mass_center.z = 0.;
		//this->mass_center(CFPoint3D(_pos.x, _pos.y,0));
	}

	CFPoint2D get_pos() const
	{
		CAutoLock __al(m_critsect);
		
		return CFPoint2D(m_mass_center.x, m_mass_center.y);
	}

	double get_direction() //const 
	{
		CAutoLock __al(m_critsect);		
		return this->get_dir();
	}

	void get_state(double& _ltarck,double& _rtrack,double& _turretangle,double& _gunangle) const
	{
		CAutoLock __al(m_critsect);
		_ltarck = CPHelper::norm_2d(m_chassis.m_velocity_left_track);
		_rtrack = CPHelper::norm_2d(m_chassis.m_velocity_right_track);
		_turretangle = m_turret.n_TurretAngle;
		_gunangle = m_turret.n_GunAngle;
	}

	void send_radarinfo(const CRadarInfo& _ri)
	{
		m_client.save(&_ri);
	}

	void send_tankinfo(const CTankInfo& _ti)
	{
		m_client.save(&_ti);
	}

	void send_artefactinfo(const CArtefactInfo& _ai)
	{
		m_client.save(&_ai);
	}

	void send_exploits_info(const CExploitsInfo& _eis)
	{
		m_client.save(&_eis);
	}

	const CFlagColor* get_flag() const {return m_flag;}

	void get_info(CTankInfoLI& _ti)
	{
		_ti.m_sTeamName = m_sTeamName;
		_ti.m_sTankName = m_sTankName;
		_ti.m_fArmor = get_armor();
		_ti.m_fFuel = get_fuel();
	}

	bool is_same_team(const CTank& _tank) const
	{
		return !m_sTeamName.CompareNoCase(_tank.m_sTeamName);
	}
};//class CTank

PIPESERVRERTRAITS(CTank,TankServerMessages,false,true);
