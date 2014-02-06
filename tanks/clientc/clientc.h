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
		CClientTank::shot(m_fTurretAngle,m_fGunAngle, m_bShot);
	}

protected:
	double m_fTurretAngle;
	double m_fGunAngle;
	double m_fLeftTrackSpeed;
	double m_fRightTrackSpeed;
	bool m_bShot;
};//struct CTank
