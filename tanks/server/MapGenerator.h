#pragma once

#include <vector>
#include <list>
#include "messages.h"
#include "GameConsts.h"
#include <utils/utils.h>

template<typename _Type>
class MapDataT
{
public:
	typedef typename _Type ItemType;

	MapDataT(size_t _width = 0,size_t _height=0)
		:m_width(_width),m_height(_height)
	{
		resize(m_width,m_height);
	}

	~MapDataT()
	{
		free();
	}

	size_t width() const {return m_width;}
	size_t height() const {return m_height;}

	void resize(size_t _width,size_t _height,const _Type& _defval = _Type())
	{
		if(m_width==_width && m_height==_height) return;
		free();
		m_width = _width;
		m_height = _height;
		m_data.resize(m_width*m_height,_defval);
	}

	void free()
	{
		m_data.resize(0);
	}

	const _Type& get(size_t _x,size_t _y) const
	{
		if(_x>=m_width || _y>=m_height) return Fish<_Type>::get();
		size_t pos = _y*m_width+_x;
		return m_data[pos];
	}

	void set(size_t _x,size_t _y,const _Type& _data)
	{
		if(_x>=m_width || _y>=m_height) return;
		size_t pos = _y*m_width+_x;
		m_data[pos] = _data;
	}

	template<typename _Functor>
	void for_each(_Functor& _func) const
	{
		size_t x=0,y=0;
		_func = get(x,y);
		for(y=0;y<height();y++)
			for(x=0;x<width();x++)
			{
				_func(x,y,get(x,y));
			}
	}

	template<typename _Functor>
	void transform(_Functor& _func)
	{
		size_t x=0,y=0;
		for(y=0;y<height();y++)
			for(x=0;x<width();x++)
			{
				set(x,y,
					_func(x,y,get(x,y))
					);
			}
	}

	typedef typename std::vector<_Type>::iterator iterator;
	typedef typename std::vector<_Type>::const_iterator const_iterator;

	iterator begin() {return m_data.begin();}
	iterator end() {return m_data.end();}
	const_iterator begin() const {return m_data.begin();}
	const_iterator end() const {return m_data.end();}

protected:
	size_t m_width;
	size_t m_height;
	std::vector<_Type> m_data;
};//template<> class MatrixT

//template<class _UnOp>
//struct ApplyUnOp
//{
//	typedef typename _UnOp _Base;
//	typedef typename _Base::argument_type argument_type;
//	typedef typename _Base::result_type result_type;
//
//	ApplyUnOp(_UnOp _op):m_op(_op)
//	{
//	}
//
//	result_type operator() (size_t _x,size_t _y,const argument_type& _d)
//	{
//		return m_op(_d);
//	}
//
//	_UnOp m_op;
//	operator _UnOp () {return m_op;}
//};
//
//template<class _UnOp> inline
//ApplyUnOp<_UnOp> applyunop(_UnOp _unop)
//{
//	return ApplyUnOp<_UnOp>(_unop);
//}

template<typename _Type>
struct FindMinMax : public std::unary_function<_Type,_Type>
{
	FindMinMax()
		:m_min(_Type())
		,m_max(_Type())
		,m_bfirst(true)
	{
	}

	_Type operator() (const _Type& _val)
	{
		if(m_bfirst)
		{
			m_max = m_min = _val;
			m_bfirst = false;
		}
		else
		{
			m_min = min(m_min,_val);
			m_max = max(m_max,_val);
		}
		return _val;
	}

	_Type m_min;
	_Type m_max;
	bool m_bfirst;
};

template<typename _Type>
struct Normalize : public std::unary_function<_Type,_Type>
{
	Normalize()
		:m_orig(_Type())
		,m_base(_Type())
	{
	}

	void operator=(const FindMinMax<_Type>& _mm)
	{
		m_base = _mm.m_max - _mm.m_min;
		m_orig = _mm.m_min;
	}

	_Type operator() (const _Type& _d)
	{
		return (_d-m_orig)/m_base;
	}

	_Type m_orig;
	_Type m_base;
};

template<typename _Type>
struct FillBMPData
{
	FillBMPData(
		std::vector<BYTE>& _bmpbuf
		,const MapDataT<_Type>& _matrix
		,const _Type& _min
		,const _Type& _max
		,unsigned int _nBytesWidth
		)
		:m_bmpbuf(_bmpbuf),m_min(_min),m_max(_max)
		,m_nBytesWidth(_nBytesWidth)
	{
		m_width = _matrix.width();
		m_height = _matrix.height();
	}

	FillBMPData<_Type>& operator = (const _Type& _val)
	{
		return *this;
	}

	FillBMPData<_Type>& operator() (size_t _x,size_t _y,const _Type& _val)
	{
		size_t line = m_height - _y - 1;
		size_t pos = line*m_nBytesWidth + (_x<<1) + _x;
		BYTE nGrayValue = (BYTE)(((_val-m_min)*255)/(m_max-m_min));
		m_bmpbuf[pos] = nGrayValue;
		m_bmpbuf[pos + 1] = nGrayValue;
		m_bmpbuf[pos + 2] = nGrayValue;
		return *this;
	}

protected:
	_Type m_min;
	_Type m_max;
	size_t m_width;
	size_t m_height;
	unsigned long m_nBytesWidth;
	std::vector<BYTE>& m_bmpbuf;
};//template<> struct FillBMPDataFunctor

template<typename _Type>
struct FillRandom
{
	_Type operator() (size_t _x,size_t _y,const _Type& _val);
};

template<>
struct FillRandom<double>
{
	double operator() (size_t _x,size_t _y,const double& _val)
	{
		return rand(0.0,1.0);
	}
};

template<>
struct FillRandom<long>
{
	long operator() (size_t _x,size_t _y,const long& _val)
	{
		return (rand() << 16) | ((rand()&1)<<15) | rand();
	}
};


template<typename _Type> inline
bool save_bmp(
	const MapDataT<_Type>& _matrix,const CString& _sFileName
	,bool _bUseMinMax = false,const _Type& _min = _Type(),const _Type& _max = _Type()
	)
{
	_Type min = _min;
	_Type max = _max;
	if(!_bUseMinMax)
	{
		FindMinMax<_Type> minmax = std::for_each(_matrix.begin(),_matrix.end(),FindMinMax<_Type>());
		min = minmax.m_min;
		max = minmax.m_max;
	}

	// declare bmp structures 
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER info;

	// and initialize them to zero
	ZeroMemory(&bmfh,sizeof(bmfh));
	ZeroMemory(&info,sizeof(info));

	long nDataSize = 0;
	long nBytesWidth = (long)_matrix.width()*3;
	if(nBytesWidth%4!=0) nBytesWidth = (nBytesWidth & ~3) + 4;

	// fill the file header with data
	bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + nDataSize;
	bmfh.bfOffBits = 0x36;		// number of bytes to start of bitmap bits

	// fill the info header
	info.biSize = sizeof(BITMAPINFOHEADER);
	info.biWidth = (long)_matrix.width();
	info.biHeight = (long)_matrix.height();
	info.biPlanes = 1;			// we only have one bitplane
	info.biBitCount = 24;		// RGB mode is 24 bits
	info.biCompression = BI_RGB;	
	info.biSizeImage = 0;		// can be 0 for 24 bit images
	info.biXPelsPerMeter = 0x0ec4;     // paint and PSP use this values
	info.biYPelsPerMeter = 0x0ec4;     
	info.biClrUsed = 0;			// we are in RGB mode and have no palette
	info.biClrImportant = 0;    // all colors are important

	// now we open the file to write to
	HANDLE hfile = CreateFile(
		_sFileName
		,GENERIC_WRITE
		,FILE_SHARE_READ
		,NULL
		,CREATE_ALWAYS
		,FILE_ATTRIBUTE_NORMAL
		,NULL
		);

	if(hfile==INVALID_HANDLE_VALUE)
	{
		CloseHandle(hfile);
		return false;
	}

	// write file header
	DWORD bwritten = 0;
	if(!WriteFile(hfile,&bmfh, sizeof(BITMAPFILEHEADER ),&bwritten,NULL))
	{	
		CloseHandle(hfile);
		return false;
	}
	// write info header
	if(!WriteFile(hfile,&info,sizeof(BITMAPINFOHEADER ),&bwritten,NULL))
	{	
		CloseHandle(hfile);
		return false;
	}

	//CPtr<BYTE> pImageData = new BYTE[nDataWidth*_matrix.height()];
	std::vector<BYTE> buf;
	buf.resize(nBytesWidth*_matrix.height(),0);

	FillBMPData<_Type> fillbmp(buf,_matrix,min,max,nBytesWidth);
	_matrix.for_each(fillbmp);

	if(!WriteFile(hfile,&buf[0],(DWORD)buf.size(),&bwritten,NULL))
	{
		CloseHandle(hfile);
		return false;
	}
	CloseHandle(hfile);
	return true;
}

inline
double noise(int _x,int _y)
{
	int n = _x + _y*57;
	n = (n << 13) ^ n;
	return ( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) 
		/ 1073741824.0);
}

inline
double smooth_noise(int _x,int _y)
{
	double corners = (
		noise(_x-1,_y-1) 
		+ noise(_x+1,_y-1)
		+ noise(_x-1,_y+1)
		+ noise(_x+1,_y+1)
		)/16;
	double sides = (
		noise(_x-1, _y) 
		+ noise(_x+1, _y) 
		+ noise(_x, _y-1) 
		+ noise(_x, _y+1)
		)/8;
	double center = noise(_x,_y) / 4;
	return corners + sides + center;
}

inline
double linear_interpolate(double _v1,double _v2,double _x)
{
	return _v1*(1-_x) + _v2*_x;
}

inline
double interpolated_noise(double _x,double _y)
{
	int x0 = (int)floor(_x);
	int x1 = x0 + 1;
	double fraction_x = _x - x0;
	int y0 = (int)floor(_y);
	int y1 = y0 + 1;
	double fraction_y = _y - y0;

	double v1 = smooth_noise(x0,y0);
	double v2 = smooth_noise(x1,y0);
	double v3 = smooth_noise(x0,y1);
	double v4 = smooth_noise(x1,y1);

	double i1 = linear_interpolate(v1,v2,fraction_x);
	double i2 = linear_interpolate(v3,v4,fraction_x);

	return linear_interpolate(i1,i2,fraction_y);
}

struct FillPerlinNoise2D
{
protected:
	struct Freq
	{
		double m_fFrequency;
		double m_fAmplitude;
		double m_xbase;
		double m_ybase;

		Freq(double _fFrequency = 0.0,double _fAmplitude=0.0)
			:m_fFrequency(_fFrequency)
			,m_fAmplitude(_fAmplitude)
			,m_xbase(0)
			,m_ybase(0)
		{
		}

		void randxy()
		{
			m_xbase = rand();
			m_ybase = rand();
		}

		double getx(double _x) const
		{
			return (_x+m_xbase)*m_fFrequency;
		}

		double gety(double _y) const
		{
			return (_y+m_ybase)*m_fFrequency;
		}
	};

	typedef std::list<Freq> FreqLst;

public:
	FillPerlinNoise2D()
	{
	}

	void add_frequency(double _fFrequency,double _fAmplitude)
	{
		m_frequecies.push_back(Freq(_fFrequency,_fAmplitude));
	}

	void restart()
	{
		FreqLst::iterator
			it = m_frequecies.begin()
			,ite = m_frequecies.end()
			;
		for(;it!=ite;++it)
		{
			it->randxy();
		}
	}

	double operator () (size_t _x,size_t _y,const double& _val) const
	{
		return get(_x,_y);
	}

	double get(double _x,double _y) const
	{
		double total = 0.0;
		FreqLst::const_iterator
			it = m_frequecies.begin()
			,ite = m_frequecies.end()
			;
		for(;it!=ite;++it)
		{
			const Freq& freq = *it;
			total += interpolated_noise(freq.getx(_x),freq.gety(_y)) * freq.m_fAmplitude;
		}

		return total;
	}

protected:
	std::list<Freq> m_frequecies;
};//struct CPerlinNoiseFunctor

struct CGameMap;

struct CAvailable4Tanks
{
protected:
	MapDataT<signed char> m_available;
	size_t m_size;
	CAvailable4Tanks* m_prev;
	const CGameMap* m_pmap;
	struct line
	{
		line(size_t _x,size_t _y)
			:x1(_x)
			,x2(_x)
			,y(_y)
		{
		}
		size_t x1,x2,y;
	};
	std::list<line> m_lines;
public:
	CAvailable4Tanks();
	void create(const CGameMap& _map,CAvailable4Tanks* _pprev);
	bool available(size_t _x,size_t _y) const;
	size_t size() const {return m_size;}
	const MapDataT<signed char>& available_data() const {return m_available;}
	void set_unavailable(const CGameMap& _map,const CAvailable4Tanks& _a1);
	void init_unavailable(const CGameMap& _map);

protected:
	void find_available();
	void fill_line(const CAvailable4Tanks::line& _l);
	void find_lines(const CAvailable4Tanks::line& _l);
	CAvailable4Tanks::line get_line(size_t _x,size_t _y) const;

	bool find_first(size_t& _x,size_t& _y);
	signed char prev(size_t _x,size_t _y) const;
	bool available(const CGameMap& _map,size_t _x,size_t _y) const ;
};

struct CGameMap
{
protected:
	MapDataT<double> m_passability;
	MapDataT<BYTE> m_objects;
	MapDataT<double> m_heights;
	MapDataT<double> m_treeh;
	CAvailable4Tanks m_available4tanks;

public:
	void resize(size_t _width,size_t _height);

	size_t width() const {return m_objects.width();}
	size_t height() const {return m_objects.height();}
	ObjectEn get_object(size_t _x,size_t _y) const
	{
		return (ObjectEn)m_objects.get(_x,_y);
	}
	ObjectEn get_object(long _x,long _y) const
	{
		return (ObjectEn)m_objects.get(_x,_y);
	}
	double get_height(size_t _x,size_t _y) const
	{
		return m_heights.get(_x,_y);
	}
	double get_height(long _x,long _y) const
	{
		return m_heights.get(_x,_y);
	}
	double get_treeheight(size_t _x,size_t _y) const
	{
		return m_treeh.get(_x,_y);
	}
	double get_passability(size_t _x,size_t _y) const
	{
		return m_passability.get(_x,_y);
	}
	bool available(size_t _x,size_t _y) const 
	{
		return m_available4tanks.available(_x,_y);
	}

	void generate(double _sz,double _maxheight);
	void save();

protected:
	void create_tankavailablemap();
	void normdata(MapDataT<double>& _data);
	void set_objects(
		const MapDataT<double>& _data
		,ObjectEn _obj
		,double _measure
		,double _maxheight = 1.0
		);
};
