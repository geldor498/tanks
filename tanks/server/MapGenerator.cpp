#include "StdAfx.h"
#include "MapGenerator.h"
#include "GameConsts.h"


CAvailable4Tanks::CAvailable4Tanks()
	:m_size(0)
	,m_prev(NULL)
	,m_pmap(NULL)
{
}

bool CAvailable4Tanks::find_first(size_t& _x,size_t& _y)
{
	VERIFY_EXIT1(NOT_NULL(m_pmap),false);
	size_t w = m_pmap->width(),h = m_pmap->height();
	for(_y=0;_y<h;_y++)
	for(_x=0;_x<w;_x++)
	{
		if(available(*m_pmap,_x,_y) && prev(_x,_y)<0) return true;
	}
	return false;
}

void CAvailable4Tanks::create(const CGameMap& _map,CAvailable4Tanks* _pprev)
{
	m_prev = _pprev;
	m_pmap = &_map;
	VERIFY_EXIT(NOT_NULL(m_pmap));
	m_size = 0;
	size_t w = _map.width(),h = _map.height();
	m_available.resize(w,h,(signed char)-1);
	size_t x=0,y=0;
	if(!find_first(x,y)) return;
	CAvailable4Tanks::line l = get_line(x,y);
	m_lines.push_back(l);
	fill_line(l);
	find_available();

	m_prev = NULL;
	m_pmap = NULL;
}

void CAvailable4Tanks::find_available()
{
	for(;!m_lines.empty();)
	{
		CAvailable4Tanks::line l = m_lines.front();
		m_lines.pop_front();
		find_lines(l);
	}
}

void CAvailable4Tanks::fill_line(const CAvailable4Tanks::line& _l)
{
	size_t x=0;
	for(x=_l.x1;x<=_l.x2;x++)
	{
		m_available.set(x,_l.y,1);
		m_size++;
		if(NOT_NULL(m_prev)) m_prev->m_available.set(x,_l.y,0);
	}
}

void CAvailable4Tanks::find_lines(const CAvailable4Tanks::line& _l)
{
	VERIFY_EXIT(NOT_NULL(m_pmap));
	size_t h = m_pmap->height();
	if(_l.y>0)
	{
		size_t x = _l.x1;
		for(;x<=_l.x2;x++)
		{
			if(m_available.get(x,_l.y-1)<0 && prev(x,_l.y-1)<0
				&& available(*m_pmap,x,_l.y-1)
				)
			{
				line l = get_line(x,_l.y-1);
				m_lines.push_back(l);
				fill_line(l);
				x = l.x2;
			}
		}
	}
	if(_l.y<h-1)
	{
		size_t x = _l.x1;
		for(;x<=_l.x2;x++)
		{
			if(m_available.get(x,_l.y+1)<0 && prev(x,_l.y+1)<0
				&& available(*m_pmap,x,_l.y+1)
				)
			{
				line l = get_line(x,_l.y+1);
				m_lines.push_back(l);
				fill_line(l);
				x = l.x2;
			}
		}
	}
}

CAvailable4Tanks::line CAvailable4Tanks::get_line(size_t _x,size_t _y) const
{
	VERIFY_EXIT1(NOT_NULL(m_pmap),line(_x,_y));
	size_t x1=0,x2=0;
	size_t w = m_pmap->width();
	line l(_x,_y);
	for(;
		l.x1>0  
		&& m_available.get(l.x1-1,_y)<0 && prev(l.x1-1,_y)<0 
		&& available(*m_pmap,l.x1-1,_y);l.x1--
		){}; 
	for(;
		l.x2<w-1 
		&& m_available.get(l.x2+1,_y)<0 && prev(l.x2+1,_y)<0 
		&& available(*m_pmap,l.x2+1,_y);l.x2++
		){};
	return l;
}

void CAvailable4Tanks::set_unavailable(const CGameMap& _map,const CAvailable4Tanks& _a1)
{
	size_t w = _map.width(),h = _map.height();
	size_t x=0,y=0;
	for(y=0;y<h;y++)
	for(x=0;x<w;x++)
	{
		if(_a1.m_available.get(x,y)>=0)
		{
			m_available.set(x,y,0);
		}
	}
}

void CAvailable4Tanks::init_unavailable(const CGameMap& _map)
{
	size_t w = _map.width(),h = _map.height();
	size_t x=0,y=0;
	for(y=0;y<h;y++)
	for(x=0;x<w;x++)
	{
		if(_map.get_object(x,y)==Obj_Rock)
		{
			m_available.set(x,y,0);
		}
	}
}

/*
void CAvailable4Tanks::mark(size_t _x,size_t _y)
{
	VERIFY_EXIT(NOT_NULL(m_pmap));
	size_t w = m_pmap->width(),h = m_pmap->height();
	if(available(*m_pmap,_x,_y))
	{
		if(NOT_NULL(m_prev)) m_prev->m_available.set(_x,_y,0);
		m_available.set(_x,_y,1);
		m_size++;
	}
	else
	{
		mark1(_x,_y);
	}

	// up left
	if(_x>0 && _y>0 && m_available.get(_x-1,_y-1)<0 && prev(_x-1,_y-1)<0)
	{
		mark(_x-1,_y-1);
	}

	// up
	if(_y>0 && m_available.get(_x,_y-1)<0 && prev(_x,_y-1)<0)
	{
		mark(_x,_y-1);
	}

	// up right
	if(_x<w-1 && _y>0 && m_available.get(_x+1,_y-1)<0 && prev(_x+1,_y-1)<0)
	{
		mark(_x+1,_y-1);
	}

	// left
	if(_x>0 && m_available.get(_x-1,_y)<0 && prev(_x-1,_y)<0)
	{
		mark(_x-1,_y);
	}

	// right
	if(_x<w-1 && m_available.get(_x+1,_y)<0 && prev(_x+1,_y)<0)
	{
		mark(_x+1,_y);
	}

	// down left
	if(_x>0 && _y<h-1 && m_available.get(_x-1,_y+1)<0 && prev(_x-1,_y+1)<0)
	{
		mark(_x-1,_y+1);
	}

	// down 
	if(_y<h-1 && m_available.get(_x,_y+1)<0 && prev(_x,_y+1)<0)
	{
		mark(_x,_y+1);
	}

	//down right
	if(_x<w-1 && _y<h-1 && m_available.get(_x+1,_y+1)<0 && prev(_x+1,_y+1)<0)
	{
		mark(_x+1,_y+1);
	}
}

void CAvailable4Tanks::mark1(size_t _x,size_t _y)
{
	VERIFY_EXIT(NOT_NULL(m_pmap));
	size_t w = m_pmap->width(),h = m_pmap->height();
	if(available(*m_pmap,_x,_y))
		return;
	else
	{
		if(NOT_NULL(m_prev)) m_prev->m_available.set(_x,_y,0);
		m_available.set(_x,_y,0);
	}

	// up left
	if(_x>0 && _y>0 && m_available.get(_x-1,_y-1)<0 && prev(_x-1,_y-1)<0)
	{
		mark1(_x-1,_y-1);
	}

	// up
	if(_y>0 && m_available.get(_x,_y-1)<0 && prev(_x,_y-1)<0)
	{
		mark1(_x,_y-1);
	}

	// up right
	if(_x<w-1 && _y>0 && m_available.get(_x+1,_y-1)<0 && prev(_x+1,_y-1)<0)
	{
		mark1(_x+1,_y-1);
	}

	// left
	if(_x>0 && m_available.get(_x-1,_y)<0 && prev(_x-1,_y)<0)
	{
		mark1(_x-1,_y);
	}

	// right
	if(_x<w-1 && m_available.get(_x+1,_y)<0 && prev(_x+1,_y)<0)
	{
		mark1(_x+1,_y);
	}

	// down left
	if(_x>0 && _y<h-1 && m_available.get(_x-1,_y+1)<0 && prev(_x-1,_y+1)<0)
	{
		mark1(_x-1,_y+1);
	}

	// down 
	if(_y<h-1 && m_available.get(_x,_y+1)<0 && prev(_x,_y+1)<0)
	{
		mark1(_x,_y+1);
	}

	//down right
	if(_x<w-1 && _y<h-1 && m_available.get(_x+1,_y+1)<0 && prev(_x+1,_y+1)<0)
	{
		mark1(_x+1,_y+1);
	}
}
*/

signed char CAvailable4Tanks::prev(size_t _x,size_t _y) const
{
	if(IS_NULL(m_prev)) return -1;
	return m_prev->m_available.get(_x,_y);
}

bool CAvailable4Tanks::available(const CGameMap& _map,size_t _x,size_t _y) const
{
	switch(_map.get_object(_x,_y))
	{
	case Obj_Rock:
		return false;
	case Obj_Water:
	case Obj_Tree:
		return _map.get_passability(_x,_y) > singleton<CGameConsts>::get().minpassability();
	default:
		return true;
	}
}

bool CAvailable4Tanks::available(size_t _x,size_t _y) const
{
	return m_available.get(_x,_y)>0;
}

void CGameMap::resize(size_t _width,size_t _height)
{
	m_passability.resize(_width,_height);
	std::fill(m_passability.begin(),m_passability.end(),1.0);
	m_objects.resize(_width,_height);
	std::fill(m_objects.begin(),m_objects.end(),Obj_Ground);
	m_heights.resize(_width,_height);
	std::fill(m_heights.begin(),m_heights.end(),0.0);
	m_treeh.resize(_width,_height);
	std::fill(m_treeh.begin(),m_treeh.end(),0.5);
}

void CGameMap::generate(double _sz,double _maxheight)
{
	if(m_objects.width()==0 || m_objects.height()==0) return;
	srand((unsigned)time(NULL));
	MapDataT<double> data;
	data.resize(m_objects.width(),m_objects.height());
	FillPerlinNoise2D noise;
	double fFreq1 = singleton<CGameConsts>::get().get_NoiseGeneratorFreq1();
	double fFreq2 = singleton<CGameConsts>::get().get_NoiseGeneratorFreq2();
	double fFreq3 = singleton<CGameConsts>::get().get_NoiseGeneratorFreq3();
	double fAmp1 = singleton<CGameConsts>::get().get_NoiseGeneratorAmp1();
	double fAmp2 = singleton<CGameConsts>::get().get_NoiseGeneratorAmp2();
	double fAmp3 = singleton<CGameConsts>::get().get_NoiseGeneratorAmp3();
	noise.add_frequency(fFreq1*1024/_sz,fAmp1);
	noise.add_frequency(fFreq2*1024/_sz,fAmp2);
	noise.add_frequency(fFreq3*1024/_sz,fAmp3);
	noise.restart();
	data.transform(noise);
	normdata(data);
	double fMapGenTreeCoef = singleton<CGameConsts>::get().get_MapGenTreeCoef();
	set_objects(data,Obj_Tree,fMapGenTreeCoef,_maxheight);
	noise.restart();
	data.transform(noise);
	normdata(data);
	double fMapGenWaterCoef = singleton<CGameConsts>::get().get_MapGenWaterCoef();
	set_objects(data,Obj_Water,fMapGenWaterCoef);
	noise.restart();
	data.transform(noise);
	normdata(data);
	double fMapGenRockCoef = singleton<CGameConsts>::get().get_MapGenRockCoef();
	set_objects(data,Obj_Rock,fMapGenRockCoef,_maxheight);
	create_tankavailablemap();
}

void CGameMap::normdata(MapDataT<double>& _data)
{
	FindMinMax<double> fmm;
	fmm = std::for_each(_data.begin(),_data.end(),fmm);
	Normalize<double> norm;
	norm = fmm;
	std::transform(_data.begin(),_data.end(),_data.begin(),norm);
}

void CGameMap::set_objects(
				 const MapDataT<double>& _data
				 ,ObjectEn _obj
				 ,double _measure
				 ,double _maxheight /*= 1.0*/
				 )
{
	MapDataT<double>::const_iterator 
		sit = _data.begin()
		,site = _data.end()
		;
	MapDataT<BYTE>::iterator 
		oit = m_objects.begin()
		,oite = m_objects.end()
		;
	MapDataT<double>::iterator
		pit = m_passability.begin()
		,pite = m_passability.end()
		;
	MapDataT<double>::iterator
		hit = m_heights.begin()
		,hite = m_heights.end()
		;
	MapDataT<double>::iterator
		tit = m_treeh.begin()
		,tite = m_treeh.end()
		;
	for(;sit!=site && oit!=oite && pit!=pite && hit!=hite
		;++sit,++pit,++oit,++hit,++tit)
	{
		if(*sit<_measure) continue;
		if(_obj!=Obj_Rock && *oit!=Obj_Ground) continue;
		*oit = _obj;
		if(_obj==Obj_Rock)
		{
			*pit = 0; // can`t drive any rock
			*hit = (*sit-_measure)/(1.0-_measure)*_maxheight;
		}
		else
		{
			*pit -= min((*sit-_measure)/(1.0-_measure),*pit);
		}
		if(_obj==Obj_Tree)
		{
			*tit = (0.5 + rand(0.0,0.3))*_maxheight/5;
		}
	}
}

void CGameMap::save()
{
	save_bmp(m_objects,_T("objects.bmp"));
	save_bmp(m_passability,_T("Passability.bmp"));
	save_bmp(m_heights,_T("heights.bmp"));
	save_bmp(m_treeh,_T("treesheight.bmp"));
	save_bmp(m_available4tanks.available_data(),_T("available.bmp"));
}

void CGameMap::create_tankavailablemap()
{
	m_available4tanks.init_unavailable(*this);
	m_available4tanks.create(*this,NULL);
	size_t sz = 1;
	for(;sz!=0;)
	{
		CAvailable4Tanks available;
		available.create(*this,&m_available4tanks);
		sz = available.size();
		if(available.size()>m_available4tanks.size())
		{
			available.set_unavailable(*this,m_available4tanks);
			m_available4tanks = available;
		}
		else
		{
			m_available4tanks.set_unavailable(*this,available);
		}
	}
}