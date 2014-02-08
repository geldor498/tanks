#include "StdAfx.h"
#include "gameworld.h"
#include "phisics.h"
#include "GameConsts.h"
#include "../utility/graphics.h"
#include "messages.h"

inline
CFPoint3D todraw(const CFPoint3D& _pt)
{
	CFPoint3D pt(_pt.x,_pt.z,_pt.y);
	return pt;
}

CGameView::CGameView()	
:m_pworld(NULL)
,m_quad(NULL)
{
}

CGameView::~CGameView()
{
	gluDeleteQuadric(m_quad);
}

void CGameView::onRedrawTimer()
{
	//m_fwaterindex += 1.0/3.0;
	//if(m_fwaterindex>=_countof(m_waters)) m_fwaterindex-=_countof(m_waters);
	//m_waterindex = (size_t)floor(m_fwaterindex);
}

void CGameView::link(CGameWorld* _pworld,CGameMap* _pmap)
{
	m_pworld = _pworld;
	m_pmap = _pmap;
	m_mountains.init(*_pmap);
	m_trees.init(*_pmap);
	//m_waterindex = 0;
	//size_t i=0;
	//for(i=0;i<_countof(m_waters);i++)
	//{
	//	m_waters[i].init(*_pmap,_scale);
	//}
}

void CGameView::draw_tanks(double _angle)
{
	if(IS_NULL(m_pworld)) return;
	CAutoLock __al(m_pworld->m_critsect);

	CGameWorld::Tanks::iterator
		tit = m_pworld->m_tanks.begin()
		,tits = m_pworld->m_tanks.end()
		;
	for(;tit!=tits;tit++)
	{
		CTank& tank = **tit;
		CFPoint2D pos = tank.get_pos();
		double fDirection = tank.get_direction();
		double ltrack = 0;
		double rtrack = 0;
		double ta = 0;
		double ga = 0;
		tank.get_state(ltrack,rtrack,ta,ga);
		m_tankmodel.m_ltrack = -ltrack;
		m_tankmodel.m_rtrack = -rtrack;
		m_tankmodel.m_gunangle = ga;
		m_tankmodel.m_turrentangle = ta;
		m_tankmodel.norm();
		glPushMatrix();
		glTranslated(pos.x,0,pos.y);
		//TRACE(_T("tank %f,%f\n"),pos.x,pos.y);
		glRotated(fDirection,0,1,0);
		//draw tank
		glColor3f(0.75f,0.75f,0.75f);
		m_tankmodel.draw();
		glPopMatrix();

		//draw flag
		double scale = singleton<CGameConsts>::get().scale();
		glPushMatrix();
		glTranslated(pos.x,singleton<CGameConsts>::get().rockheight()*scale*1.25,pos.y);
		glRotated(_angle,0,1,0);
		glColor3d(0.9f,0.9f,0.9f);
		float tankamb[] = {0.9f,0.9f,0.9f};
		glMaterialfv(GL_FRONT,GL_AMBIENT,tankamb);
		float tankdiff[] = {0.9f,0.9f,0.9f};
		glMaterialfv(GL_FRONT,GL_DIFFUSE,tankdiff);
		glBegin(GL_LINES);
		line(CFPoint3D(0,0,0),CFPoint3D(-5*scale,3*scale,0));
		line(CFPoint3D(-5*scale,3*scale,0),CFPoint3D(5*scale,3*scale,0));
		line(CFPoint3D(5*scale,3*scale,0),CFPoint3D(0,0,0));
		glEnd();
		float flagclr[3];
		glBegin(GL_QUADS);
		flagclr[0] = (GLfloat)GetRValue(tank.get_flag()[0])/256.0f;
		flagclr[1] = (GLfloat)GetGValue(tank.get_flag()[0])/256.0f;
		flagclr[2] = (GLfloat)GetBValue(tank.get_flag()[0])/256.0f;
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,flagclr);
		glColor3fv(flagclr);
		glVertex(CFPoint3D(-5*scale,3*scale,0));
		glVertex(CFPoint3D(5*scale,3*scale,0));
		glVertex(CFPoint3D(5*scale,5*scale,0));
		glVertex(CFPoint3D(-5*scale,5*scale,0));
		flagclr[0] = (GLfloat)GetRValue(tank.get_flag()[1])/256.0f;
		flagclr[1] = (GLfloat)GetGValue(tank.get_flag()[1])/256.0f;
		flagclr[2] = (GLfloat)GetBValue(tank.get_flag()[1])/256.0f;
		glColor3fv(flagclr);
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,flagclr);
		glVertex(CFPoint3D(-5*scale,5*scale,0));
		glVertex(CFPoint3D(5*scale,5*scale,0));
		glVertex(CFPoint3D(5*scale,7*scale,0));
		glVertex(CFPoint3D(-5*scale,7*scale,0));
		flagclr[0] = (GLfloat)GetRValue(tank.get_flag()[2])/256.0f;
		flagclr[1] = (GLfloat)GetGValue(tank.get_flag()[2])/256.0f;
		flagclr[2] = (GLfloat)GetBValue(tank.get_flag()[2])/256.0f;
		glColor3fv(flagclr);
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,flagclr);
		glVertex(CFPoint3D(-5*scale,7*scale,0));
		glVertex(CFPoint3D(5*scale,7*scale,0));
		glVertex(CFPoint3D(5*scale,9*scale,0));
		glVertex(CFPoint3D(-5*scale,9*scale,0));
		glEnd();
		glPopMatrix();
	}
}

void CGameView::draw_ground()
{
	if(IS_NULL(m_pmap)) return;

	double scale = singleton<CGameConsts>::get().scale();
	glEnable(GL_LIGHTING);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	glEnable(GL_NORMALIZE);
	//glEnable(GL_COLOR_MATERIAL);
	//glColorMaterial(GL_FRONT,GL_DIFFUSE);

	GLfloat light0_diffuse[] = {0.75f, 0.75f, 0.75f};
	GLfloat light0_direction[] = {0.0f, 1.0f, 0.0f, 0.0f};
	glEnable(GL_LIGHT2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT2, GL_POSITION, light0_direction);

	size_t y=0,x=0;
	size_t w = m_pmap->width();
	size_t h = m_pmap->height();

	glColor3d(0.1,0.1,0.1);
	float grnmat[] = {0.3f,0.3f,0.3f};
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,grnmat);
	glBegin(GL_QUADS);

	for(y=0;y<h;y++)
	for(x=0;x<w;x++)
	{
		if(m_pmap->get_object(x,y)!=Obj_Ground) continue;
		double x1 = ((double)x-(double)w*0.5)*scale;
		double y1 = ((double)y-(double)h*0.5)*scale;
		double x2 = (((double)x+1)-(double)w*0.5)*scale;
		double y2 = (((double)y+1)-(double)h*0.5)*scale;
		glVertex(CFPoint3D(x1,0,y1));
		glVertex(CFPoint3D(x2,0,y1));
		glVertex(CFPoint3D(x2,0,y2));
		glVertex(CFPoint3D(x1,0,y2));
	}
	glEnd();
	double i=0;

	float treeamb[] = {0.0f,0.2f,0.0f};
	glMaterialfv(GL_FRONT,GL_AMBIENT,treeamb);
	float treediff[] = {0.0f,0.4f,0.0f};
	glMaterialfv(GL_FRONT,GL_DIFFUSE,treediff);

	//glColor3d(0,0.55,0);
	//m_trees.draw();
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_DOUBLE, 0, m_trees.get_vertex());
	glNormalPointer(GL_DOUBLE, 0, m_trees.get_normals());
	glDrawArrays(GL_TRIANGLES, 0, (GLint)m_trees.size());

	//glColor3d(1,0,0);
	//m_mountains.draw();

	float rockamb[] = {0.1f,0.1f,0.1f};
	glMaterialfv(GL_FRONT,GL_AMBIENT,rockamb);
	float rockdiff[] = {0.2f,0.2f,0.2f};
	glMaterialfv(GL_FRONT,GL_DIFFUSE,rockdiff);
	glVertexPointer(3, GL_DOUBLE, 0, m_mountains.get_vertex());
	glNormalPointer(GL_DOUBLE, 0, m_mountains.get_normals());
	glDrawArrays(GL_TRIANGLES, 0, (GLint)m_mountains.size());

	//glColor3d(0,0,0.75);
	//m_waters[m_waterindex].draw();
	float wattermat[] = {0.0f,0.0f,0.75f};
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,wattermat);
	float watershines[] = {64.0f};
	glMaterialfv(GL_FRONT,GL_SHININESS,watershines);

	//glVertexPointer(3,GL_DOUBLE,0,m_waters[m_waterindex].get_vertex());
	//glNormalPointer(GL_DOUBLE, 0, m_waters[m_waterindex].get_normals());
	//glDrawArrays(GL_TRIANGLES, 0, (GLint)m_waters[m_waterindex].size());
	glBegin(GL_QUADS);

	for(y=0;y<h;y++)
	for(x=0;x<w;x++)
	{
		if(m_pmap->get_object(x,y)!=Obj_Water) continue;
		double x1 = ((double)x-(double)w*0.5)*scale;
		double y1 = ((double)y-(double)h*0.5)*scale;
		double x2 = (((double)x+1)-(double)w*0.5)*scale;
		double y2 = (((double)y+1)-(double)h*0.5)*scale;
		glVertex(CFPoint3D(x1,0,y1));
		glVertex(CFPoint3D(x2,0,y1));
		glVertex(CFPoint3D(x2,0,y2));
		glVertex(CFPoint3D(x1,0,y2));
	}
	glEnd();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

void CGameView::draw_artefacts()
{
	if(IS_NULL(m_pworld)) return;
	double scale = singleton<CGameConsts>::get().scale();
	CAutoLock __al(m_pworld->m_critsect);
	CGameWorld::Artefacts::const_iterator 
		it = m_pworld->m_artefacts.begin()
		,ite = m_pworld->m_artefacts.end()
		;
	double d = scale*0.75;
	for(;it!=ite;++it)
	{
		const CArtefact* partefact = it->second;
		
		glPushMatrix();
		glTranslated(partefact->m_mass_center.x,partefact->m_mass_center.z,partefact->m_mass_center.y);
		float artefamb[] = {0.9f,0.9f,0.9f};
		glMaterialfv(GL_FRONT,GL_AMBIENT,artefamb);
		float artefdiff[] = {0.9f,0.9f,0.9f};
		glMaterialfv(GL_FRONT,GL_DIFFUSE,artefdiff);
		glBegin(GL_LINES);
		glTranslated(scale*0.25,0.2,scale*0.25);
		line(CFPoint3D(0,0,0),CFPoint3D(d,0,0));
		line(CFPoint3D(0,0,0),CFPoint3D(0,0,d));
		line(CFPoint3D(d,0,0),CFPoint3D(d,0,d));
		line(CFPoint3D(0,0,d),CFPoint3D(d,0,d));
		line(CFPoint3D(0,d,0),CFPoint3D(d,d,0));
		line(CFPoint3D(0,d,0),CFPoint3D(0,d,d));
		line(CFPoint3D(d,d,0),CFPoint3D(d,d,d));
		line(CFPoint3D(0,d,d),CFPoint3D(d,d,d));
		line(CFPoint3D(0,0,0),CFPoint3D(0,d,0));
		line(CFPoint3D(d,0,0),CFPoint3D(d,d,0));
		line(CFPoint3D(0,0,d),CFPoint3D(0,d,d));
		line(CFPoint3D(d,0,d),CFPoint3D(d,d,d));
		glEnd();
		glPopMatrix();
	}
}

void CGameView::draw_shells()
{
	if(IS_NULL(m_pworld)) return;
	CAutoLock __al(m_pworld->m_critsect);
	CGameWorld::Shells::const_iterator
		it = m_pworld->m_Shells.begin()
		,ite = m_pworld->m_Shells.end()
		;
	glColor3d(1,1,1);
	float explamb[] = {1.0f,1.0f,1.0f};
	glMaterialfv(GL_FRONT,GL_AMBIENT,explamb);
	float expldiff[] = {1.0f,1.0f,1.0f};
	glMaterialfv(GL_FRONT,GL_DIFFUSE,expldiff);
	glBegin(GL_LINES);
	for(;it!=ite;++it)
	{
		CFPoint3D pt1,pt2;
		(*it)->get_nears(1./10.,pt1,pt2);
		line(todraw(pt1),todraw(pt2));
	}
	glEnd();
}

void CGameView::initialize()
{
	if(IS_NULL(m_quad))
	{
		m_quad = gluNewQuadric();
		gluQuadricDrawStyle(m_quad, GLU_FILL);
		gluQuadricNormals(m_quad,GLU_FLAT);
		gluQuadricOrientation(m_quad,GLU_OUTSIDE);
	}
}

void CGameView::draw_exploits()
{
	if(IS_NULL(m_pworld)) return;
	double scale = singleton<CGameConsts>::get().scale();
	CAutoLock __al(m_pworld->m_critsect);
	CGameWorld::Exploits::iterator
		it = m_pworld->m_exploits.begin()
		,ite = m_pworld->m_exploits.end()
		;
	glColor3d(1,0,0);
	float explamb[] = {1.0f,0.0f,0.0f};
	glMaterialfv(GL_FRONT,GL_AMBIENT,explamb);
	float expldiff[] = {1.0f,0.0f,0.0f};
	glMaterialfv(GL_FRONT,GL_DIFFUSE,expldiff);
	for(;it!=m_pworld->m_exploits.end();)
	{
		CExploit& exploit = *it;
		glPushMatrix();
		glTranslated(exploit.m_pos.x,exploit.m_pos.z,exploit.m_pos.y);
		gluSphere(m_quad,exploit.m_r,10,10);
		glPopMatrix();
		
//		std::list<CFPoint3D>::const_iterator
//			pit = exploit.m_pts.begin()
//			,pite = exploit.m_pts.end()
//			;
//		glBegin(GL_LINES);
//		CFPoint3D pt;
//		if(pit!=pite) pt = *pit;
//		for(;pit!=pite;pt=*pit,++pit)
//		{
//			line(todraw(pt),todraw(*pit));
//		}
//		glEnd();

		exploit.next_tick(scale*1.2/(exploit.m_maxticks - exploit.m_ticks+1));
		if(exploit.m_ticks<=0)
			it = m_pworld->m_exploits.erase(it);
		else
			++it;
	}
}

CControl::CControl(
		 CGameWorld& _world
		 ,CGameMap& _map
		 )
		 :m_world(_world)
		 ,m_map(_map)
{
}

void CControl::process_commands(double _dt)
{
	CAutoLock __al(m_world.m_critsect);

	if(_dt>0){
		//пеерсчитываем танки
		CGameWorld::Tanks::iterator
			tit = m_world.m_tanks.begin()
			,tits = m_world.m_tanks.end()
			;
		for(;tit!=tits;tit++)
		{
			CTank& tank = **tit;
			process_tank(tank,_dt);
		}

		//пересчитываем снаряды
		CGameWorld::Shells::iterator
			sit = m_world.m_Shells.begin()
			,sits = m_world.m_Shells.end();

		for(;sit!=sits;sit++)
		{				
			(*sit)->update(_dt);
		}

		//поиск столкновений
		tit = m_world.m_tanks.begin();
		for(;tit!=tits;tit++)
		{				
			CCollissionFinder::collision((**tit), m_map);

			sit = m_world.m_Shells.begin();			
			for(;sit!=sits;sit++)
				CCollissionFinder::collision((**tit), (**sit));					
		}

		//находим столкновения снарядов с ландшафтом
		sit = m_world.m_Shells.begin();
		for(;sit!=sits;sit++)	
			CCollissionFinder::collision((**sit),m_map);

		//находим непрямыс столлкновения снарядов с танками 
		//(т.е. определяем пострадавших от ударных волн и осколков)
		tit = m_world.m_tanks.begin();
		for(; tit!=tits; tit++)
		{
			sit = m_world.m_Shells.begin();
			for(;sit!=sits;sit++)
				CCollissionFinder::collision((**tit), (**sit));
		}

		// опредялем взрывы
		sit = m_world.m_Shells.begin();
		for(;sit!=sits;++sit)
		{
			if((**sit).m_interaction || (**sit).fallback())
			{
				m_world.add_exploit(
					(**sit).m_interaction?10:4
					,(**sit).m_mass_center
					,**sit,(*sit)->get_id(),(*sit)->get_fly_time()
					,(*sit)->get_tank_id()
					,(*sit)->m_fHit
					);
				(**sit).m_interaction = true;
			}
		}

		//чистим мир от провзаимодействовавших снарядов
		m_world.m_Shells.remove_if(CGameWorld::interacted());
	}
}

void CControl::process_tank(CTank& _tank,double _dt)
{
	for(;_tank.process_command(*this);) {}
	_tank.m_friction_coefficient = m_map.get_passability((size_t)CPHelper::tr_x(_tank.m_mass_center.x),(size_t)CPHelper::tr_y(_tank.m_mass_center.y));
	_tank.run(_dt);
}

void CControl::process_command(const CMoveCmd& _move,CTank& _tank)
{
	_tank.set_tracksv(_move.m_fVLeftTrack,_move.m_fVRightTrack);
}

void CControl::process_command(const  CShotCmd& _shot,CTank& _tank)
{
	_tank.set_gun(_shot.m_fTurretAngle,_shot.m_fGunAngle, _shot.m_bShot);
	if(_shot.m_bShot)
		m_world.add_Shell(_tank,_shot.m_id,_tank.get_TankID());	
}

void CControl::process_command(const CTakeArtefactCmd& _cmd,CTank& _tank)
{
	CArtefactInfo ai;
	ai.m_fMass = ai.m_ID = 0;
	ai.m_type = Artefact_Null;

	//_tank.m_m
	//нужно определить есть ли артефакт в зоне ограничивающей сферы танка
	CGameWorld::Artefacts::iterator it = m_world.m_artefacts.begin()
		,ite =  m_world.m_artefacts.end();

	for(;it!=ite;++it)
	{
		if(CPHelper::norm2_2d(_tank.m_mass_center - it->second->m_mass_center)<pow(_tank.m_sphereRadious, 2) )
		{
			if(!it->second->m_occupied)
			{
				_tank.take_artefact(it->second);
				it->second->m_occupied = true;
				ai.m_fMass = it->second->m_mass;
				ai.m_type = it->second->m_type;
				ai.m_ID = it->second->m_ID;
				_tank.send_artefactinfo(ai);
				return;
			}
		}
	}
	// send artefact info if there is no artefact
	_tank.send_artefactinfo(ai);
}

void CControl::process_command(const CUseArtefactCmd& _cmd,CTank& _tank)
{
	if(_tank.find_artefact(_cmd.m_ArtefactID))
	{
		VERIFY_EXIT(m_world.m_artefacts.find(_cmd.m_ArtefactID)!=m_world.m_artefacts.end());
		_tank.use_artefact(m_world.m_artefacts[_cmd.m_ArtefactID]);
		delete m_world.m_artefacts[_cmd.m_ArtefactID];
		m_world.m_artefacts.erase(_cmd.m_ArtefactID);
	}
}

void CControl::process_command(const CPutArtefactCmd& _cmd,CTank& _tank)
{
	if(_tank.find_artefact(_cmd.m_ArtefactID))
	{
		CGameWorld::Artefacts::iterator it = m_world.m_artefacts.find(_cmd.m_ArtefactID);
		if(it != m_world.m_artefacts.end())
		{
			_tank.put_artefact(it->second);
			it->second->m_occupied = false;
		}
	}
}

void CControl::process_command(const CGetRadarInfoCmd& _cmd,CTank& _tank)	
{
	CFPoint2D pos = _tank.get_pos();
	long posx = (long)floor(CPHelper::tr_x(pos.x));
	long posy = (long)floor(CPHelper::tr_y(pos.y));
	size_t radarR = (long)singleton<CGameConsts>::get().radarR();
	CRadarInfo ri;
	ri.m_pos = pos;
	ri.m_mappos = CFPoint2D(posx,posy);
	ri.resize(radarR*2);
	long x=0,y=0;
	for(y=0;(size_t)y<radarR*2;y++)
	for(x=0;(size_t)x<radarR*2;x++)
	{
		ri.set_object(
			(size_t)x,(size_t)y
			,m_map.get_object(posx+(x-(long)radarR),posy+(y-(long)radarR))
			,m_map.get_height(posx+(x-(long)radarR),posy+(y-(long)radarR))
			);
	}
	{
		double scale = singleton<CGameConsts>::get().scale();
		size_t width = singleton<CGameConsts>::get().width();
		size_t height = singleton<CGameConsts>::get().height();
		CFRect rc(
			CFPoint2D(pos.x-radarR*scale,pos.y-radarR*scale)
			,CFPoint2D(pos.x+radarR*scale,pos.y+radarR*scale)
			);

		CAutoLock __al(m_world.m_critsect);

		CGameWorld::Tanks::const_iterator
			tit = m_world.m_tanks.begin()
			,tite = m_world.m_tanks.end()
			;
		for(;tit!=tite;++tit)
		{
			const CTank& tank = **tit;
			CFPoint2D pos = tank.get_pos();
			if(rc.inside(pos))
			{
				ri.m_tanks.push_back(CTankPosition(pos,tank.get_TankID()));
			}
		}
		CGameWorld::Artefacts::const_iterator
			ait = m_world.m_artefacts.begin()
			,aite = m_world.m_artefacts.end()
			;
		for(;ait!=aite;++ait)
		{
			const CArtefact* part = ait->second;
			CFPoint2D pos(part->m_mass_center.x,part->m_mass_center.y);
			if(rc.inside(pos))
			{
				ri.m_artefacts.push_back(pos);
			}
		}
	}
	_tank.send_radarinfo(ri);
}

void CControl::process_command(const CGetTankInfoCmd& _cmd,CTank& _tank)
{
	CTankInfo ti;
	double lt=0,rt=0,ta=0,ga=0;
	_tank.get_state(lt,rt,ta,ga);
	ti.m_fVLeftTrack = lt;
	ti.m_fVRightTrack = rt;
	ti.m_pos = _tank.get_pos();
	ti.m_fDirection = _tank.get_direction();
	ti.m_fArmor = _tank.get_armor();
	ti.m_fFuel = _tank.get_fuel();
	ti.m_fMass = _tank.get_mass();
	CPhisicsTank::OwnArtefacts artefactsids = _tank.get_artefacts();
	m_world.get_artefact_info(artefactsids,ti.m_artefacts);
	_tank.send_tankinfo(ti);
}

void CControl::process_command(const CGetExploitsInfoCmd& _cmd,CTank& _tank)
{
	CExploitsInfo eis;
	m_world.get_exploits_info(_tank.get_TankID(),eis,_tank);
	_tank.send_exploits_info(eis);
}

CGameWorld::~CGameWorld()
{
	free();
}

CGameWorld::CGameWorld(CGameMap& _map)
	:m_map(_map)
	,m_nTankIDLast(0)
{
}

void CGameWorld::add_tank(
			  const CString& _sClientComputerName
			  ,const CString& _sClientPipeName
			  ,const CString& _sServerPipeName	
			  ,const CString& _sTeamName
			  ,const CString& _sTankName
			  ,const CFlagColor (&_flag)[3]
			  )
{
	srand((unsigned)time(NULL));
	CAutoLock __al(m_critsect);
	m_tanks.push_back(new CTank(_sClientComputerName,_sClientPipeName,_sServerPipeName,_sTeamName,_sTankName,_flag,m_nTankIDLast++));
	CFPoint2D pos;
	generate_position(pos);
	m_tanks.back()->set_pos(pos);
}

void CGameWorld::generate_position(CFPoint2D& _pos)
{
	double scale = singleton<CGameConsts>::get().scale();
	size_t width = singleton<CGameConsts>::get().width();
	size_t height = singleton<CGameConsts>::get().height();
	bool avalable = false;
	size_t x = 0,y=0;
	for(;!avalable;)
	{
		x = ::rand(width-7)+3;
		y = ::rand(height-7)+3;
		avalable = m_map.available(x,y);
	}
	_pos.x = (x-width*0.5)*scale;
	_pos.y = (y-height*0.5)*scale;
}

void CGameWorld::generate_artefacts()
{
	//добавление артефктов
	long start_id = 1;
	long art_cnt = 40;
	for(long i = start_id; i<art_cnt; i++)
	{
		m_artefacts.insert(std::pair<long, CArtefact*>(i, new CFuelArtefact(i)) );
		CFPoint2D pos;
		generate_position(pos);
		Point3DT<double> point(pos.x,pos.y,0);
		m_artefacts[i]->m_mass_center =  point;
	}
}

void CGameWorld::add_exploit(
	size_t _ticks,const CFPoint3D& _pos,CShell& _shell
	,long _shellid,double _fly_time,long _nTankID
	,double _fHit
	)
{
	CAutoLock __al(m_critsect);
	//if(!m_exploits.empty()) m_exploits.pop_front();
	m_exploits.push_back(CExploit(_ticks,_pos,_shellid,_fly_time,_nTankID,_fHit));
//	std::vector<Point3DT<double> >::const_iterator 
//		it = _shell.m_path.begin()
//		,ite = _shell.m_path.end()
//		;
//	for(;it!=ite;++it)
//	{
//		m_exploits.back().m_pts.push_back(*it);
//	}
}

void CGameWorld::add_Shell(const CPhisicsTank& _tank,long _shotid,long _nTankID)
{
	CAutoLock __al(m_critsect);
	m_Shells.push_back(new CShell(_shotid,_nTankID));
	m_Shells.back()->init(_tank);
}

void CGameWorld::free()
{
	CAutoLock __al(m_critsect);
	Tanks::iterator
		it = m_tanks.begin()
		,ite = m_tanks.end()
		;
	for(;it!=ite;++it)
	{
		delete *it;
	}
	m_tanks.clear();
}

void CGameWorld::get_tanksinfo(TanksInfoLst& _ti)
{
	_ti.clear();
	CAutoLock __al(m_critsect);
	Tanks::iterator
		it = m_tanks.begin()
		,ite = m_tanks.end()
		;
	for(;it!=ite;++it)
	{
		CTankInfoLI ti;
		(*it)->get_info(ti);
		_ti.push_back(ti);
	}
}

void CGameWorld::get_artefact_info(const CPhisicsTank::OwnArtefacts& _artefactsids,std::list<CArtefactInfo>& _artefactsinfo)
{
	_artefactsinfo.clear();
	CPhisicsTank::OwnArtefacts::const_iterator
		it = _artefactsids.begin()
		,ite = _artefactsids.end()
		;
	for(;it!=ite;++it)
	{
		VERIFY_DO(m_artefacts.find(*it)!=m_artefacts.end(),continue);
		CArtefactInfo ai;
		ai.m_fMass = m_artefacts[*it]->m_mass;
		ai.m_type = m_artefacts[*it]->m_type;
		ai.m_ID = m_artefacts[*it]->m_ID;
		_artefactsinfo.push_back(ai);
	}
}

void CGameWorld::get_exploits_info(long _nTankID,CExploitsInfo& _eis,CTank& _tank)
{
	CAutoLock __al(m_critsect);

	_eis.m_exploits.clear();

	CFPoint2D pos = _tank.get_pos();
	size_t radarR = (long)singleton<CGameConsts>::get().radarR();
	double scale = singleton<CGameConsts>::get().scale();
	size_t width = singleton<CGameConsts>::get().width();
	size_t height = singleton<CGameConsts>::get().height();
	CFRect rc(
		CFPoint2D(pos.x-radarR*scale,pos.y-radarR*scale)
		,CFPoint2D(pos.x+radarR*scale,pos.y+radarR*scale)
		);

	Exploits::const_iterator
		it = m_exploits.begin()
		,ite = m_exploits.end()
		;
	for(;it!=ite;++it)
	{
		const CExploit& exp = *it;
		CFPoint2D pt = CFPoint2D(exp.m_pos.x,exp.m_pos.y);
		if(!(rc.inside(pt) || exp.m_nTankID==_nTankID)) continue;
		CExploitInfo ei;
		ei.m_fFlyTime = exp.m_fly_time;
		ei.m_pt = exp.m_pos;
		if(exp.m_nTankID==_nTankID)
		{
			ei.m_fHitPoints = exp.m_fHit;
			ei.m_id = exp.m_shellid;
		}
		else
		{
			ei.m_fHitPoints = 0;
			ei.m_id = -1;
		}
		_eis.m_exploits.push_back(ei);
	}
}