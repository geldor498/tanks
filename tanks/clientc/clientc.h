#pragma once

#include "resource.h"
#include "../server/messages.h"

struct CTank : public CClientTank
{
	CTank(const CString& _sTeamName,const CString& _sTankName,const CString& _sClientName)
		:CClientTank(_sTeamName,_sTankName,_sClientName)
		,m_fTurretAngle(0)
		,m_fGunAngle(0)
		,m_fLeftTrackSpeed(0)
		,m_fRightTrackSpeed(0)
		,m_bShot(true)
	{
	}

	void lefttrack(double _dv)
	{
		m_fLeftTrackSpeed += _dv;
	}

	void righttrack(double _dv)
	{
		m_fRightTrackSpeed += _dv;
	}

	void rotateturret(double _angle)
	{
		m_fTurretAngle += _angle;
	}

	void upgun(double _angle)
	{
		m_fGunAngle += _angle;
	}

	void move()
	{
		CClientTank::move(m_fLeftTrackSpeed,m_fRightTrackSpeed);
	}

	void shot()
	{
		CClientTank::shot(m_fTurretAngle,m_fGunAngle, m_bShot,1);
	}

protected:
	double m_fTurretAngle;
	double m_fGunAngle;
	double m_fLeftTrackSpeed;
	double m_fRightTrackSpeed;
	bool m_bShot;
};//struct CTank

CString to_str(ObjectEn _obj)
{
	switch(_obj)
	{
	case Obj_Null:		return _T(".");
	case Obj_Ground:	return _T("_");
	case Obj_Water:		return _T("~");
	case Obj_Tree:		return _T("T");
	case Obj_Rock:		return _T("O");
	default:
		return _T(":");
	}
}

void print_ri(const CRadarInfo& _ri)
{
	long sz = _ri.m_sz;
	long x = 0,y = 0;
	for(y=0;y<sz;y++)
	{
		for(x=0;x<sz;x++)
		{
			double h = 0;
			ObjectEn obj = _ri.get_object(x,y,h);
			if(EQL(x,sz/2) && EQL(y,sz/2)) std::cout << "+";
			else if(x<78) std::wcout << (LPCTSTR)to_str(obj);
		}
		std::cout << "\n";
	}
}