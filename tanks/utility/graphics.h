#pragma once

#include <utils/serialize.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <math.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

extern double EPS;

template<typename _Type> inline
bool eql(const _Type& _d1,const _Type& _d2)
{
	return _d1==_d2;
}

inline
bool eql(double _d1,double _d2)
{
	return fabs(_d2-_d1)<=EPS;
}

inline
bool eql(double _d1,int _d2)
{
	return fabs(_d2-_d1)<=EPS;
}

inline
bool eql(double _d1,long _d2)
{
	return fabs(_d2-_d1)<=EPS;
}

template<typename _Type>
inline
_Type sqr(const _Type& _d)
{
	return _d*_d;
}

template<typename _Type>
struct Point2DT
{
	_Type x,y;

	Point2DT(const _Type& _x = 0,const _Type& _y = 0)
		:x(_x),y(_y)
	{}

	template<typename _Type2>
	Point2DT(const Point2DT<_Type2>& _pt)
		:x(_pt.x),y(_pt.y)
	{
	}

	Point2DT(const CPoint& _pt)
		:x(_pt.x),y(_pt.y)
	{
	}

	template<typename _Type2>
	Point2DT<_Type>& operator = (const Point2DT<_Type2>& _pt)
	{
		x = _pt.x;
		y = _pt.y;
		return *this;
	}

	Point2DT<_Type>& operator = (const CPoint& _pt)
	{
		x = _pt.x;
		y = _pt.y;
		return *this;
	}

	template<typename _Type2>
	bool operator == (const Point2DT<_Type2>& _pt) const
	{
		return eql(x, _pt.x)
			&& eql(y, _pt.y)
			;
	}

	template<typename _Type2>
	Point2DT<_Type>& operator += (const Point2DT<_Type2>& _pt)
	{
		x += _pt.x;
		y += _pt.y;
		return *this;
	}

	template<typename _Type2>
	Point2DT<_Type>& operator -= (const Point2DT<_Type2>& _pt)
	{
		x -= _pt.x;
		y -= _pt.y;
		return *this;
	}

	void rand_init(const Point2DT<_Type>& _tl,const Point2DT<_Type>& _rb)
	{
		x = ::rand(_tl.x,_rb.x);
		y = ::rand(_tl.y,_rb.y);
	}
};//template<_Type> struct PointT

template<typename _Type>
inline
Point2DT<_Type> operator + (const Point2DT<_Type>& _p1,const Point2DT<_Type>& _p2)
{
	Point2DT<_Type> p = _p1;
	p += _p2;
	return p;
}

template<typename _Type>
inline
Point2DT<_Type> operator - (const Point2DT<_Type>& _p1,const Point2DT<_Type>& _p2)
{
	Point2DT<_Type> p = _p1;
	p -= _p2;
	return p;
}

template<typename _Type>
inline
double distance(const Point2DT<_Type>& _p1,const Point2DT<_Type>& _p2)
{
	return sqrt(sqr(_p1.x - _p2.x) + sqr(_p1.y - _p2.y));
}

typedef Point2DT<double> CFPoint2D;

template<typename _Type>
struct Point2DTDefaultLayoutT : public Layout<Point2DT<_Type> >
{
	Point2DTDefaultLayoutT()
	{
		add_simple(_T("x"),&Point2DT<_Type>::x);
		add_simple(_T("y"),&Point2DT<_Type>::y);
	}
};


typedef Point2DTDefaultLayoutT<double> CFPoint2DTDefaultLayout;

DECLARE_OUTER_DEFAULT_LAYOUT(CFPoint2D,CFPoint2DTDefaultLayout);

template<typename _Type>
struct RectT
{
	Point2DT<_Type> m_tl;
	Point2DT<_Type> m_br;

	RectT(const Point2DT<_Type>& _tl,const Point2DT<_Type>& _br)
		:m_tl(_tl),m_br(_br)
	{
	}

	bool inside(const Point2DT<_Type>& _pt)
	{
		return 
			_pt.x>=m_tl.x && _pt.x<=m_br.x
			&& _pt.y>=m_tl.y && _pt.y<=m_br.y
			;
	}
};

typedef RectT<double> CFRect;

template<typename _Type>
struct Point3DT
{
	_Type x,y,z,w;

	Point3DT(
		const _Type& _x = _Type()
		,const _Type& _y = _Type()
		,const _Type& _z = _Type()
		,const _Type& _w = 1
		)
		:x(_x),y(_y),z(_z),w(_w)
	{}

	template<typename _Type2>
	Point3DT(const Point3DT<_Type2>& _pt)
		:x(_pt.x),y(_pt.y),y(_pt.z),w(_pt.w)
	{
	}

	Point3DT(const CPoint& _pt)
		:x(_pt.x),y(_pt.y),z(_Type()),w(1)
	{
	}

	template<typename _Type2>
	Point3DT<_Type>& operator = (const Point3DT<_Type2>& _pt)
	{
		x = _pt.x;
		y = _pt.y;
		z = _pt.z;
		w = _pt.w;
		return *this;
	}

	Point3DT<_Type>& operator = (const CPoint& _pt)
	{
		x = _pt.x;
		y = _pt.y;
		z = _Type();
		w = 1;
		return *this;
	}

	template<typename _Type2>
	bool operator == (const Point3DT<_Type2>& _pt) const
	{
		return eql(x/w, _pt.x/_pt.w)
			&& eql(y/w, _pt.y/_pt.w)
			&& eql(z/w, _pt.z/_pt.w)
			;
	}

	template<typename _Type2>
	Point3DT<_Type>& operator += (const Point3DT<_Type2>& _pt)
	{
		x = x/w + _pt.x/_pt.w;
		y = y/w + _pt.y/_pt.w;
		z = z/w + _pt.z/_pt.w;
		w = 1;
		return *this;
	}

	template<typename _Type2>
	Point3DT<_Type>& operator -= (const Point3DT<_Type2>& _pt)
	{
		x = x/w - _pt.x/_pt.w;
		y = y/w - _pt.y/_pt.w;
		z = z/w - _pt.z/_pt.w;
		w = 1;
		return *this;
	}

	template<typename _Type2>
	_Type operator * (const Point3DT<_Type2>& _pt) const
	{
		return x/w*_pt.x/_pt.w + y/w*_pt.y/_pt.w + z/w*_pt.z/_pt.w;
	}

	Point3DT<_Type>& operator *= (const _Type& _k)
	{
		x *= _k;
		y *= _k;
		z *= _k;
		return *this;
	}

	void norm()
	{
		double len = sqrt(sqr(x/w)+sqr(y/w)+sqr(z/w));
		if(len!=0.0)
		{
			*this *= 1.0/len;
		}
		w = 1;
	}

	void rew()
	{
		x /= w;
		y /= w;
		z /= w;
		w = 1;
	}

	_Type& operator [] (size_t _i)
	{
		switch(_i)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		}
		return Fish<_Type>::get();
	}

	const _Type& operator [] (size_t _i) const
	{
		switch(_i)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		}
		return Fish<_Type>::get();
	}

	Point3DT<_Type> cross(const Point3DT<_Type>& _b)
	{
		Point3DT<_Type> pt;
		pt.x = (y*_b.z - z*_b.y)/(w*_b.w);
		pt.y = (z*_b.x - x*_b.z)/(w*_b.w);
		pt.z = (x*_b.y - y*_b.x)/(w*_b.w);
		pt.w = 1;
		return pt;
	}
};//template<_Type> struct PointT


template<typename _Type>
inline
Point3DT<_Type> operator + (const Point3DT<_Type>& _p1,const Point3DT<_Type>& _p2)
{
	Point3DT<_Type> p = _p1;
	p += _p2;
	return p;
}

template<typename _Type>
inline
Point3DT<_Type> operator - (const Point3DT<_Type>& _p1,const Point3DT<_Type>& _p2)
{
	Point3DT<_Type> p = _p1;
	p -= _p2;
	return p;
}

template<typename _Type>
inline
Point3DT<_Type> operator * (const _Type& _k,const Point3DT<_Type>& _p)
{
	Point3DT<_Type> p = _p;
	p *= _k;
	return p;
}

template<typename _Type>
inline
Point3DT<_Type> operator * (const Point3DT<_Type>& _p,const _Type& _k)
{
	Point3DT<_Type> p = _p;
	p *= _k;
	return p;
}

template<typename _Type>
inline
double distance(const Point3DT<_Type>& _p1,const Point3DT<_Type>& _p2)
{
	return sqrt(
		sqr(_p1.x/_p1.w - _p2.x/_p2.w) 
		+ sqr(_p1.y/_p1.w - _p2.y/_p2.w) 
		+ sqr(_p1.z/_p1.w - _p2.z/_p2.w)
		);
}

typedef Point3DT<double> CFPoint3D;

template<typename _Type>
struct Point3DTDefaultLayoutT : public Layout<Point3DT<_Type> >
{
	Point3DTDefaultLayoutT()
	{
		add_simple(_T("x"),&Point3DT<_Type>::x);
		add_simple(_T("y"),&Point3DT<_Type>::y);
		add_simple(_T("z"),&Point3DT<_Type>::z);
		add_simple(_T("w"),&Point3DT<_Type>::w);
	}
};

typedef Point3DTDefaultLayoutT<double> CFPoint3DTDefaultLayout;

DECLARE_OUTER_DEFAULT_LAYOUT(CFPoint3D,CFPoint3DTDefaultLayout);


CFPoint3D scalar(const CFPoint3D& _a,const CFPoint3D& _b);

template<typename _Type>
inline
_Type rand(_Type _min,_Type _max)
{
	return (::rand()*(_max-_min)/RAND_MAX) + _min;
}

template<typename _Type>
inline
_Type rand(_Type _max)
{
	return ::rand(_Type(),_max);
}

inline
double rand(double _min,double _max)
{
	return ((double)rand()/(double)RAND_MAX)*(_max-_min) + _min;
}

inline
double rand(double _max)
{
	return rand(0.0,_max);
}

inline 
bool rand(bool _max)
{
	return (::rand()&1)==0;
}

inline
void rand_init(CFPoint3D& _pt)
{
	_pt.x = ::rand(-1.0,1.0);
	_pt.y = ::rand(-1.0,1.0);
	_pt.z = ::rand(-1.0,1.0);
	_pt.norm();
}


struct CMatrix
{
	double d[16];


//	struct DefaultLayout : public Layout<CMatrix>
//	{
//		DefaultLayout()
//		{
//			add_array(_T("d"),&CMatrix::d,get_primitivelayout<double>());
//		}
//	};

	CMatrix()
	{
		zero();
	}

	void zero()
	{
		size_t i=0;
		for(i=0;i<_countof(d);i++) d[i] = 0;
	}

	void identity()
	{
		set(
			1,	0,	0,	0,
			0,	1,	0,	0,
			0,	0,	1,	0,
			0,	0,	0,	1
			);
	}

	void translate(double _x,double _y,double _z)
	{
		zero();
		set(
			1,	0,	0,	_x,
			0,	1,	0,	_y,
			0,	0,	1,	_z,
			0,	0,	0,	1
			);
	}

	void rotate(double _angle,double _x,double _y,double _z)
	{
		zero();
		CFPoint3D pt(_x,_y,_z);
		pt.norm();
		double x = pt.x;
		double y = pt.y;
		double z = pt.z;
		double c = cos(_angle);
		double s = sin(_angle);

		set(
			x*x*(1-c) + c,		x*y*(1-c) - z*s,	x*z*(1-c) + y*s,	0,
			y*x*(1-c) + z*s,	y*y*(1-c) + c,		y*z*(1-c) - x*s,	0,
			x*z*(1-c) - y*s,	y*z*(1-c) + x*s,	z*z*(1-c) + c,		0,
			0,					0,					0,					1
			);
	}

	double& m(size_t _x,size_t _y)
	{
		return *(d + _x*4+_y);
	}

	const double& m(size_t _x,size_t _y) const
	{
		return *(d + _x*4+_y);
	}

	double& m(double* _m,size_t _x,size_t _y)
	{
		return *(_m + _x*4+_y);
	}

	void set(
		double _a00,double _a01,double _a02,double _a03
		,double _a10,double _a11,double _a12,double _a13
		,double _a20,double _a21,double _a22,double _a23
		,double _a30,double _a31,double _a32,double _a33
		)
	{
		m(0,0) = _a00;
		m(1,0) = _a01;
		m(2,0) = _a02;
		m(3,0) = _a03;
		m(0,1) = _a10;
		m(1,1) = _a11;
		m(2,1) = _a12;
		m(3,1) = _a13;
		m(0,2) = _a20;
		m(1,2) = _a21;
		m(2,2) = _a22;
		m(3,2) = _a23;
		m(0,3) = _a30;
		m(1,3) = _a31;
		m(2,3) = _a32;
		m(3,3) = _a33;
	}

	void multiply(const CMatrix& _matrix)
	{
		double res[16];
		size_t i=0,j=0;
		for(i=0;i<_countof(res);i++) res[i] = 0;
		for(i=0;i<4;i++)
			for(j=0;j<4;j++)
			{
				size_t k=0;
				for(k=0;k<4;k++)
					m(res,j,i) += m(k,i)*_matrix.m(j,k);
			}
			for(i=0;i<_countof(res);i++) d[i] = res[i];
	}

	CMatrix& operator = (const CMatrix& _m)
	{
		size_t i=0;
		for(i=0;i<_countof(d);i++) d[i] = _m.d[i];
		return *this;
	}

	CFPoint3D operator * (const CFPoint3D& _pt) const
	{
		CFPoint3D pt(0,0,0,0);
		size_t i=0;
		for(i=0;i<4;i++)
		{
			size_t k=0;
			for(k=0;k<4;k++)
				pt[i] += m(k,i)*_pt[k];
		}
		return pt;
	}
};

struct CCoordinatesTransform
{
	std::list<CMatrix> m_stack;

	CCoordinatesTransform()
	{
		CMatrix m;
		m.identity();
		m_stack.push_back(m);
	}

	CMatrix& top()
	{
		VERIFY_EXIT1(m_stack.size()>0,Fish<CMatrix>::get());
		return m_stack.front();
	}

	const CMatrix& top() const
	{
		VERIFY_EXIT1(m_stack.size()>0,Fish<CMatrix>::get());
		return m_stack.front();
	}

	void push()
	{
		m_stack.push_front(m_stack.front());
	}

	void pop()
	{
		VERIFY_EXIT(m_stack.size()>1);
		m_stack.erase(m_stack.begin());
	}

	void set(const CMatrix& _m)
	{
		VERIFY_EXIT(m_stack.size()>0);
		m_stack.front() = _m;
	}

	void zero()
	{
		VERIFY_EXIT(m_stack.size()>0);
		m_stack.front().zero();
	}

	void identity()
	{
		VERIFY_EXIT(m_stack.size()>0);
		m_stack.front().identity();
	}

	void translate(double _x,double _y,double _z)
	{
		VERIFY_EXIT(m_stack.size()>0);
		CMatrix m;
		m.translate(_x,_y,_z);
		m_stack.front().multiply(m);
	}

	void rotate(double _angle,double _x,double _y,double _z)
	{
		VERIFY_EXIT(m_stack.size()>0);
		CMatrix m;
		m.rotate(_angle,_x,_y,_z);
		m_stack.front().multiply(m);
	}

	CFPoint3D transform(const CFPoint3D& _pt)
	{
		VERIFY_EXIT1(m_stack.size()>0,_pt);
		return m_stack.front()*_pt;
	}
};//struct CCoordinatesTransform


inline
void glVertex(const CFPoint3D& _pt)
{
	glVertex3d(_pt.x,_pt.y,_pt.z);
}

inline
void line(const CFPoint3D& _p1,const CFPoint3D& _p2)
{
	glVertex(_p1);
	glVertex(_p2);
}

template<typename _Type>
void minmax(_Type& _dta,const _Type& _min,const _Type& _max)
{
	_dta = max(_dta,_min);
	_dta = min(_dta,_max);
}

template<typename _Type>
void minmaxr(_Type& _dta,const _Type& _min,const _Type& _max)
{
	if(_dta<_min) _dta = _max + _dta - _min;
	if(_dta>_max) _dta = _dta - _max + _min;
}

