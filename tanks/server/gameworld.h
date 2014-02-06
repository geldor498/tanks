#pragma once

#include "tank.h"
#include "MapGenerator.h"
#include "models.h"
#include <fstream>
#include <map>
#include <list>
#include <gl\GLU.h>
#include <gl\GL.h>

using std::ofstream;
using std::list;
using std::map;
using std::endl;
class CControl;
class CGameView;

struct CExploit
{
	CExploit(size_t _ticks = 0,const CFPoint3D& _pos = CFPoint3D(0,0,0))
	{
		m_maxticks = m_ticks = _ticks;
		m_pos = _pos;
		m_r = 1.0;
	}

	size_t m_maxticks;
	size_t m_ticks;
	double m_r;
	CFPoint3D m_pos;
	std::list<CFPoint3D> m_pts;

	void next_tick(double _dr)
	{
		if(m_ticks) m_ticks--;
		m_r+=_dr;
	}
};

class CGameWorld
{
public:
	typedef std::vector<CTank*> Tanks;
	typedef std::list<CShell*> Shells;
	typedef std::map<long, CArtefact*> Artefacts;
	typedef std::list<CExploit> Exploits;
	struct interacted {
		bool operator() (const CShell* value) { return value->m_interaction; }
	};
protected:
	Tanks m_tanks;
	Shells m_Shells;
	Artefacts m_artefacts;
	Exploits m_exploits;
	CCriticalSection m_critsect;
	CGameMap& m_map;

public:
	~CGameWorld();
	CGameWorld(CGameMap& _map);
	void add_tank(
		const CString& _sClientComputerName
		,const CString& _sClientPipeName
		,const CString& _sServerPipeName	
		,const CString& _sTeamName
		,const CString& _sTankName
		,const CFlagColor (&_flag)[3]
		);
	void add_Shell(const CPhisicsTank& _tank);
	void generate_position(CFPoint2D& _pos);
	void generate_artefacts();
	void add_exploit(size_t _ticks,const CFPoint3D& _pos,CShell& _shell);
	void get_tanksinfo(TanksInfoLst& _ti);
protected:
	void free();

	friend class CControl;
	friend class CGameView;
};//class CGameWorld

// Control classes of MVC architecture
class CControl
{
protected:
	CGameWorld& m_world;
	CGameMap& m_map;
public:

	CControl(
		CGameWorld& _world
		,CGameMap& _map
		);
	void process_commands(double _dt);
	void process_tank(CTank& _tank,double _dt);
	void process_command(const CMoveCmd& _move,CTank& _tank);
	void process_command(const  CShotCmd& _shot,CTank& _tank);
	void process_command(const CTakeArtefactCmd& _cmd,CTank& _tank);
	void process_command(const CUseArtefactCmd& _cmd,CTank& _tank);
	void process_command(const CPutArtefactCmd& _cmd,CTank& _tank);
	void process_command(const CGetRadarInfoCmd& _cmd,CTank& _tank);
	void process_command(const CGetTankInfoCmd& _cmd,CTank& _tank);
};//class CControl

class CGameView
{
protected:
	CTankModel m_tankmodel;
protected:
	CGameWorld* m_pworld;
	CGameMap* m_pmap;
	CMountains m_mountains;
	CTrees m_trees;
	GLUquadricObj* m_quad;
	//CWaters m_waters[32];//32
	//size_t m_waterindex;
	//double m_fwaterindex;

public:
	CGameView();
	~CGameView();
	void onRedrawTimer();
	void link(CGameWorld* _pworld,CGameMap* _pmap);
	void draw_tanks(double _angle);
	void draw_ground();
	void draw_artefacts();
	void draw_exploits();
	void draw_shells();
	void initialize();
};