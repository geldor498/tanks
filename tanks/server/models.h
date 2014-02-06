#pragma once

#include <math.h>
#include "../utility/graphics.h"
#include "MapGenerator.h"

#define M_PI       3.14159265358979323846

struct CTankModel
{
protected:
	// cube
	CFPoint3D m_p0;
	CFPoint3D m_p1;
	CFPoint3D m_p2;
	CFPoint3D m_p3;
	CFPoint3D m_p4;
	CFPoint3D m_p5;
	CFPoint3D m_p6;
	// turret
	CFPoint3D m_p7;
	CFPoint3D m_p8;
	CFPoint3D m_p9;
	CFPoint3D m_p10;
	CFPoint3D m_p11;
	CFPoint3D m_p12;
	CFPoint3D m_p13;
	// gun
	CFPoint3D m_p14;
	CFPoint3D m_p15;
	// tracks
	CFPoint3D m_p16;
	CFPoint3D m_p17;
	CFPoint3D m_p18;
	CFPoint3D m_p19;
	// center
	CFPoint3D m_p20;

	double m_track_l1;
	double m_track_r;
	//double m_track_l;

public:
	double m_turrentangle;
	double m_gunangle;
	double m_ltrack;
	double m_rtrack;

	CTankModel()
		:m_turrentangle(0)
		,m_gunangle(0)
		,m_ltrack(0)
		,m_rtrack(0)

		//cube
		,m_p0(0,0,0)
		,m_p1(0.25,-1,-1)
		,m_p2(4,0,0)
		,m_p3(0.25,2,-2)
		,m_p4(0,0,-10)
		,m_p5(0.25,2,-9)
		,m_p6(0.25,-1,-9)
		// turret
		,m_p7(2,2,-4)
		,m_p8(0.75,2,-4+1.5)
		,m_p9(0,0,0)
		,m_p10(0.5,2,-4)
		,m_p11(0,0,0)
		,m_p12(1,2,-4-2.25)
		,m_p13(0,0,0)
		// gun
		,m_p14(2,(2+3.75)/2,0)
		,m_p15(2,(2+3.75)/2,1)
		// tracks
		,m_p16(0,-0.5,-2.0)
		,m_p17(0,-0.5,-0.5)
		,m_p18(0,-0.5,-9)
		,m_p19(-0.8,-0.5,-2.0)
		// center
		,m_p20(2,-2,-5)
	{
		m_p9 = m_p8 - m_p7;
		m_p9 *= 0.7;
		m_p9 = m_p7 + m_p9;
		m_p9.y = 3.75;

		m_p11 = m_p10 - m_p7;
		m_p11 *= 0.7;
		m_p11 = m_p7 + m_p11;
		m_p11.y = 3.75;

		m_p13 = m_p12 - m_p7;
		m_p13 *= 0.7;
		m_p13 = m_p7 + m_p13;
		m_p13.y = 3.75;

		m_p14.z = (m_p8.z + m_p9.z)/2;

		m_track_l1 = fabs(m_p16.z - m_p18.z);
		m_track_r = fabs(m_p17.z - m_p16.z);
	}

	void norm()
	{
		m_ltrack = fmod(m_ltrack,1.0);
		m_rtrack = fmod(m_rtrack,1.0);
		minmaxr(m_ltrack,0.0,1.0);
		minmaxr(m_rtrack,0.0,1.0);
		minmaxr(m_turrentangle,0.0,360.0);
		minmax(m_gunangle,-10.0,60.0);
	}

	void draw_key_points()
	{
		glPushMatrix();
		glTranslated(-m_p20.x,-m_p20.y,-m_p20.z);
		glBegin(GL_POINTS);
		{
			glVertex(m_p0);
			glVertex(m_p1);
			glVertex(m_p2);
			glVertex(m_p3);
			glVertex(m_p4);
			glVertex(m_p5);
			glVertex(m_p6);

			glVertex(m_p7);
			glVertex(m_p8);
			glVertex(m_p9);
			glVertex(m_p10);
			glVertex(m_p11);
			glVertex(m_p12);
			glVertex(m_p13);

		}
		glEnd();
		glPopMatrix();
	}

	void draw_cube()
	{
		CFPoint3D p1;
		p1 = m_p1 - m_p0;
		p1.x = -p1.x;
		p1 = m_p2 + p1;

		line(m_p0,m_p1);
		line(m_p0,m_p2);
		line(m_p1,p1);
		line(m_p2,p1);

		CFPoint3D p2 = m_p6-m_p4;
		p2.x = -p2.x;
		CFPoint3D p3 = m_p2-m_p0;
		p3 = m_p4 + p3;
		p2 = p3 + p2;

		line(m_p1,m_p6);
		line(p1,p2);
		line(m_p6,p2);

		line(m_p0,m_p4);
		line(m_p2,p3);
		line(m_p4,p3);

		line(m_p4,m_p6);
		line(p3,p2);

		CFPoint3D p4 = m_p3 - m_p0;
		p4.x = -p4.x;
		p4 = m_p2 + p4;

		CFPoint3D p5 = m_p5 - m_p4;
		p5.x = -p5.x;
		p5 = p3 + p5;

		line(m_p3,p4);
		line(m_p3,m_p5);
		line(p4,p5);
		line(m_p5,p5);

		line(m_p0,m_p3);
		line(m_p2,p4);
		line(m_p4,m_p5);
		line(p3,p5);
	}

	void draw_turret()
	{
		CFPoint3D p[20];
		p[8] = m_p8 - m_p7;
		p[9] = m_p9 - m_p7;
		p[10] = m_p10 - m_p7;
		p[11] = m_p11 - m_p7;
		p[12] = m_p12 - m_p7;
		p[13] = m_p13 - m_p7;

		p[18] = p[8];
		p[18].x = -p[18].x;

		p[19] = p[9];
		p[19].x = -p[19].x;

		p[16] = p[10];
		p[16].x = -p[16].x;

		p[17] = p[11];
		p[17].x = -p[17].x;

		p[14] = p[12];
		p[14].x = -p[14].x;

		p[15] = p[13];
		p[15].x = -p[15].x;

		CCoordinatesTransform t;
		t.rotate(m_turrentangle*M_PI/180,0,1,0);
		size_t i=0;
		for(i=8;i<_countof(p);i++)
		{
			p[i] = t.transform(p[i]);
		}
		for(i=8;i<_countof(p)-2;i+=2)
		{
			line(p[i]+m_p7,p[i+2]+m_p7);
			line(p[i+1]+m_p7,p[i+3]+m_p7);
			line(p[i]+m_p7,p[i+1]+m_p7);
		}
		line(p[8]+m_p7,p[18]+m_p7);
		line(p[9]+m_p7,p[19]+m_p7);
		line(p[18]+m_p7,p[19]+m_p7);
	}

	void draw_gun()
	{
		CFPoint3D p[8];
		p[0] = m_p14 + CFPoint3D(0,0.125,0);
		p[1] = m_p14 + CFPoint3D(0.125,0,0);
		p[2] = m_p14 + CFPoint3D(0,-0.125,0);
		p[3] = m_p14 + CFPoint3D(-0.125,0,0);
		p[4] = m_p15 + CFPoint3D(0,0.125,0);
		p[5] = m_p15 + CFPoint3D(0.125,0,0);
		p[6] = m_p15 + CFPoint3D(0,-0.125,0);
		p[7] = m_p15 + CFPoint3D(-0.125,0,0);
		size_t i=0;
		CCoordinatesTransform t;
		t.rotate(m_gunangle*M_PI/180,-1,0,0);
		for(i=0;i<_countof(p);i++)
		{
			p[i] = t.transform(p[i]-m_p14)+m_p14;
		}
		t.identity();
		t.rotate(m_turrentangle*M_PI/180,0,1,0);
		for(i=0;i<_countof(p);i++)
		{
			p[i] = t.transform(p[i]-m_p7) + m_p7;
		}
		for(i=0;i<4;i++)
		{
			line(p[i],p[i+4]);
		}
		for(i=0;i<3;i++)
		{
			line(p[i],p[i+1]);
			line(p[i+4],p[i+1+4]);
		}
		line(p[0],p[3]);
		line(p[0+4],p[3+4]);
	}

	void get_track_pt(size_t _i,double _rotk,size_t _cnt,const CFPoint3D& _p1,const CFPoint3D& _p2,CFPoint3D& _pt)
	{
		double p1 = (double)_i/_cnt;
		p1 += _rotk/_cnt;
		if(p1>=1) p1 -= 1.0;
		double p = (m_track_l1*2 + 2*M_PI*m_track_r)*p1;
		if(p<m_track_r*M_PI)
		{
			double a = p/m_track_r; 
			_pt = _p1;
			_pt.z += sin(a)*m_track_r;
			_pt.y += cos(a-M_PI)*m_track_r;
			return;
		}
		if(p<m_track_r*M_PI+m_track_l1)
		{
			double dz = p - m_track_r*M_PI;
			_pt = _p1;
			_pt.z -= dz;
			_pt.y += m_track_r;
			return;
		}
		if(p<2*m_track_r*M_PI+m_track_l1)
		{
			double a = (p-m_track_r*M_PI+m_track_l1)/m_track_r; 
			_pt = _p2;
			_pt.z += sin(a)*m_track_r;
			_pt.y -= cos(a)*m_track_r;
			return;
		}
		double dz = p - (2*m_track_r*M_PI+m_track_l1);
		_pt = _p2;
		_pt.z += dz;
		_pt.y -= m_track_r;
	}

	void draw_tracks()
	{
		CFPoint3D w = m_p19 - m_p16;
		size_t i=0,cnt = 25;
		for(i=0;i<cnt;i++)
		{
			CFPoint3D p1;
			CFPoint3D p2;
			get_track_pt(i,m_rtrack,cnt,m_p16,m_p18,p1);
			get_track_pt(i+1,m_rtrack,cnt,m_p16,m_p18,p2);
			CFPoint3D p3 = p1 + w;
			CFPoint3D p4 = p2 + w;
			line(p1,p2);
			line(p3,p4);
			line(p1,p3);
			line(p2,p4);
		}
		CFPoint3D p20 = m_p2 - m_p0;
		CFPoint3D p21 = m_p18 + p20;
		p20 = m_p16 + p20;
		w = m_p16 - m_p19;
		for(i=0;i<cnt;i++)
		{
			CFPoint3D p1;
			CFPoint3D p2;
			get_track_pt(i,m_ltrack,cnt,p20,p21,p1);
			get_track_pt(i+1,m_ltrack,cnt,p20,p21,p2);
			CFPoint3D p3 = p1 + w;
			CFPoint3D p4 = p2 + w;
			line(p1,p2);
			line(p3,p4);
			line(p1,p3);
			line(p2,p4);
		}
	}

	void draw()
	{
		glPushMatrix();
		glTranslated(-m_p20.x,-m_p20.y,-m_p20.z);
		float tankamb[] = {0.9f,0.9f,0.9f};
		glMaterialfv(GL_FRONT,GL_AMBIENT,tankamb);
		float tankdiff[] = {0.9f,0.9f,0.9f};
		glMaterialfv(GL_FRONT,GL_DIFFUSE,tankdiff);
		glBegin(GL_LINES);
		{
			// adjust points
			m_p6.x = m_p1.x;
			m_p6.y = m_p1.y;
			m_p4.x = m_p0.x;
			m_p4.y = m_p0.y;
			m_p5.x = m_p3.x;
			m_p5.y = m_p5.y;

			draw_cube();
			draw_turret();
			draw_gun();
			draw_tracks();
		}
		glEnd();
		glPopMatrix();
	}
};

struct CTreeModel
{
	std::vector<CFPoint3D> m_pts;
	CFPoint3D m_p;

	CTreeModel(double _w,double _h,double _k1,double _k2,size_t _cnt,double _rs)
	{
		m_pts.resize(_cnt);
		size_t i=0;
		m_pts[0] = CFPoint3D(_w/10+_w/2,0,0);
		m_pts[_cnt-1] = CFPoint3D(-_w/10+_w/2,0,0);
		for(i=1;i<_cnt-1;i++)
		{
			double a = (i-1)*M_PI*2/(_cnt-1)-M_PI/2;
			double y = sin(a)*_h/2 + _h/2 + _h/10;
			y += rand(_rs);
			double k = _k2 + y*(_k1-_k2)/_h;
			double x = (cos(a)*_w/2+rand(_rs))*k + _w/2;
			m_pts[i] = CFPoint3D(x,y,0);
		}
		m_p = CFPoint3D(_w/2,0,0);
	}

	void draw(double _a)
	{
		glPushMatrix();
		glRotated(_a,0,1,0);
		glTranslated(-m_p.x,-m_p.y,-m_p.z);
		glBegin(GL_LINES);
		{
			size_t i=0;
			for(i=0;i<m_pts.size();i++)
			{
				line(m_pts[i],m_pts[(i+1)%m_pts.size()]);
			}
		}
		glEnd();
		glPopMatrix();
	}
};

struct CMountainInfo
{
	ObjectEn get_object() const 
	{
		return Obj_Rock;
	}

	double get_height(const CGameMap& _map,size_t _x,size_t _y) const
	{
		return _map.get_height(_x,_y);
	}

	void set_color(std::vector<double>& _vec,size_t& _i,double _h) const
	{
		if(eql(_h,0))
		{
			_vec[_i++] = 0.1;
			_vec[_i++] = 0.1;
			_vec[_i++] = 0.1;
		}
		else
		{
			_vec[_i++] = 0.3;
			_vec[_i++] = 0.3;
			_vec[_i++] = 0.3;
		}
	}
};

struct CTreeInfo
{
	ObjectEn get_object() const 
	{
		return Obj_Tree;
	}

	double get_height(const CGameMap& _map,size_t _x,size_t _y) const
	{
		return _map.get_treeheight(_x,_y);
	}

	void set_color(std::vector<double>& _vec,size_t& _i,double _h) const
	{
		if(eql(_h,0))
		{
			_vec[_i++] = 0.1;
			_vec[_i++] = 0.1;
			_vec[_i++] = 0.1;
		}
		else
		{
			_vec[_i++] = 0;
			_vec[_i++] = 0.7;
			_vec[_i++] = 0;
		}
	}
};

struct CWaterInfo
{
	CWaterInfo()
	{
		m_noise.add_frequency(2.0, 1.0);
		m_noise.restart();
	}

	ObjectEn get_object() const 
	{
		return Obj_Water;
	}

	double get_height(const CGameMap& _map,size_t _x,size_t _y) const
	{
		return m_noise.get(_x,_y)+0.1;
	}

	void set_color(std::vector<double>& _vec,size_t& _i,double _h) const
	{
		_vec[_i++] = 0;
		_vec[_i++] = 0;
		_vec[_i++] = 0.75;
	}
protected:
	FillPerlinNoise2D m_noise;
};

template<typename _ObjectInfo>
struct CGroundObject : public _ObjectInfo
{
protected:
	struct Normals
	{
		CFPoint3D m_north;
		CFPoint3D m_east;
		CFPoint3D m_south;
		CFPoint3D m_west;
		CFPoint3D m_n;
		CFPoint3D m_fan;
	};
public:

	void init(CGameMap& _map)
	{
		size_t w = _map.width();
		size_t h = _map.height();
		size_t x=0,y=0;
		size_t cnt = 0;
		for(y=0;y<h;y++)
		{
			for(x=0;x<w;x++)
			{
				if(_map.get_object(x,y)!=get_object())
					continue;
				cnt++;
			}
		}
		m_fVertex.resize(cnt*9*4);
		m_fNormals.resize(cnt*9*4);
		m_Colors.resize(cnt*9*4);
		Normals** normals = new Normals*[w];
		size_t i=0;
		for(i=0;i<w;i++) normals[i] = new Normals[h];
		size_t it=0,in=0,ic=0;
		double scale = singleton<CGameConsts>::get().scale();
		for(y=0;y<h;y++)
		{
			for(x=0;x<w;x++)
			{
				if(_map.get_object(x,y)!=get_object())
					continue;
				double x0 = ((double)x-(double)0.5*w)*scale;
				double x1 = ((double)(x+1)-(double)0.5*w)*scale;
				double y0 = ((double)y-(double)0.5*h)*scale;
				double y1 = ((double)(y+1)-(double)0.5*h)*scale;
				double hx0y0 = get_height(_map,x, y) * scale;
				double hx0y1 = get_height(_map,x, y+1) * scale;
				double hx1y0 = get_height(_map,x+1, y) * scale;
				double hx1y1 = get_height(_map,x+1, y+1) * scale;
				correct_height(_map,x,y,hx0y0,hx0y1,hx1y1,hx1y0);
				double xm = (x0 + x1)*0.5;
				double ym = (y0 + y1)*0.5;
				double hm = (hx0y0 + hx0y1 + hx1y0 + hx1y1)*0.25;
				CFPoint3D px0y0(x0,hx0y0,y0);
				CFPoint3D px0y1(x0,hx0y1,y1);
				CFPoint3D px1y0(x1,hx1y0,y0);
				CFPoint3D px1y1(x1,hx1y1,y1);
				CFPoint3D pm(xm,hm,ym);
				add_triangle(it,ic,pm,px0y0,px1y0,normals[x][y].m_north);
				add_triangle(it,ic,pm,px1y0,px1y1,normals[x][y].m_east);
				add_triangle(it,ic,pm,px1y1,px0y1,normals[x][y].m_south);
				add_triangle(it,ic,pm,px0y1,px0y0,normals[x][y].m_west);
			}
		}
		for(y=0;y<h;y++)
		{
			for(x=0;x<w;x++)
			{
				if(y>0 && x>0) 
					normals[x][y].m_n = normals[x-1][y-1].m_south + normals[x-1][y-1].m_east;
				if(y>0)
					normals[x][y].m_n = normals[x][y].m_n + normals[x][y-1].m_south + normals[x][y-1].m_west;
				if(x>0)
					normals[x][y].m_n = normals[x][y].m_n + normals[x-1][y].m_north + normals[x-1][y].m_east;
				normals[x][y].m_n = normals[x][y].m_n + normals[x][y].m_west + normals[x][y].m_north;
				normals[x][y].m_n.norm();
				normals[x][y].m_fan = normals[x][y].m_east + normals[x][y].m_south
					+ normals[x][y].m_west + normals[x][y].m_north
					;
				normals[x][y].m_fan.norm();
			}
		}
		for(y=0;y<h;y++)
		{
			for(x=0;x<w;x++)
			{
				if(_map.get_object(x,y)!=get_object())
					continue;
				{
					if(x<w-1)
						add_normals(in,normals[x][y].m_fan,normals[x][y].m_n,normals[x+1][y].m_n);
					else
						add_normals(in,normals[x][y].m_fan,normals[x][y].m_n,normals[x][y].m_n);

					if(x<w-1 && y<h-1)
					{
						add_normals(in,normals[x][y].m_fan,normals[x+1][y].m_n,normals[x+1][y+1].m_n);
						add_normals(in,normals[x][y].m_fan,normals[x+1][y+1].m_n,normals[x][y+1].m_n);
					}
					else
					{
						add_normals(in,normals[x][y].m_fan,normals[x][y].m_n,normals[x][y].m_n);
						add_normals(in,normals[x][y].m_fan,normals[x][y].m_n,normals[x][y].m_n);
					}

					if(y<h-1)
						add_normals(in,normals[x][y].m_fan,normals[x][y+1].m_n,normals[x][y].m_n);
					else
						add_normals(in,normals[x][y].m_fan,normals[x][y].m_n,normals[x][y].m_n);
				}
			}
		}
		for(i=0;i<w;i++) delete normals[i];
		delete normals;
	}

	void correct_height(const CGameMap& _map,size_t _x,size_t _y
		,double& _hx0y0,double& _hx0y1,double& _hx1y1,double& _hx1y0
		)
	{
		if(_x==0 || _x>0 && _map.get_object(_x-1,_y)!=get_object())
		{
			_hx0y0 = 0;
			_hx0y1 = 0;
		}
		if(_x==0 || _x>0 && _map.get_object(_x,_y-1)!=get_object())
		{
			_hx0y0 = 0;
			_hx1y0 = 0;
		}
		if(_x==_map.width()-1 || _x<_map.width()-1 && _map.get_object(_x+1,_y)!=get_object())
		{
			_hx1y0 = 0;
			_hx1y1 = 0;
		}
		if(_y==_map.height()-1 || _y<_map.height()-1 && _map.get_object(_x,_y+1)!=get_object())
		{
			_hx0y1 = 0;
			_hx1y1 = 0;
		}
		if(_x==0 || _y==0 || _x>0 && _y>0 && _map.get_object(_x-1,_y-1)!=get_object())
		{
			_hx0y0 = 0;
		}
		if(_x==0 || _y==_map.height()-1 || _x>0 && _y<_map.height()-1 && _map.get_object(_x-1,_y+1)!=get_object())
		{
			_hx0y1 = 0;
		}
		if(_y==0 || _x==_map.width()-1 || _y>0 && _x<_map.width()-1 && _map.get_object(_x+1,_y-1)!=get_object())
		{
			_hx1y0 = 0;
		}
		if(_x==_map.width()-1 || _y==_map.height()-1 || _y<_map.height()-1 && _x<_map.width()-1 && _map.get_object(_x+1,_y+1)!=get_object())
		{
			_hx1y1 = 0;
		}
	}

	double* get_vertex() 
	{
		return &m_fVertex[0];
	}

	double* get_normals()
	{
		return &m_fNormals[0];
	}

	double* get_colors()
	{
		return &m_Colors[0];
	}

	size_t size()
	{
		return m_fVertex.size()/3;
	}

	void draw()
	{
		glBegin(GL_LINES);
		for(size_t i=0;i<m_fVertex.size();i+=9)
		{
			CFPoint3D p0,p1,p2;
			get_pt(m_fVertex,i+0,p0);
			get_pt(m_fVertex,i+3,p1);
			get_pt(m_fVertex,i+6,p2);
			line(p0,p1);
			line(p1,p2);
			line(p2,p0);
		}
		glEnd();
	}

protected:
	void add_triangle(size_t& _i,size_t& _ic
		,const CFPoint3D& _p0,const CFPoint3D& _p1,const CFPoint3D& _p2
		,CFPoint3D& _n)
	{
		add_pt(m_fVertex,_i,_p0);
		set_color(m_Colors,_ic,_p0.y);
		add_pt(m_fVertex,_i,_p1);
		set_color(m_Colors,_ic,_p1.y);
		add_pt(m_fVertex,_i,_p2);
		set_color(m_Colors,_ic,_p2.y);
		CFPoint3D v1 = _p0 - _p1;
		CFPoint3D v2 = _p2 - _p1;
		_n = v1.cross(v2);
		_n.norm();
	}

	void add_normals(size_t& _i,const CFPoint3D& _p0,const CFPoint3D& _p1,const CFPoint3D& _p2)
	{
		add_pt(m_fNormals,_i,_p0);
		add_pt(m_fNormals,_i,_p1);
		add_pt(m_fNormals,_i,_p2);
	}

	static void add_pt(std::vector<double>& _vec,size_t& _i,const CFPoint3D& _pt)
	{
		VERIFY_EXIT(_i+3<=_vec.size());
		CFPoint3D pt = _pt;
		pt.rew();
		_vec[_i++] = pt.x;
		_vec[_i++] = pt.y;
		_vec[_i++] = pt.z;
	}

	static void get_pt(const std::vector<double>& _vec,size_t _i,CFPoint3D& _pt)
	{
		VERIFY_EXIT(_i+3<=_vec.size());
		_pt.x = _vec[_i+0];
		_pt.y = _vec[_i+1];
		_pt.z = _vec[_i+2];
	}

	std::vector<double> m_fVertex;
	std::vector<double> m_fNormals;
	std::vector<double> m_Colors;
};

typedef CGroundObject<CMountainInfo> CMountains;
typedef CGroundObject<CTreeInfo> CTrees;
typedef CGroundObject<CWaterInfo> CWaters;
