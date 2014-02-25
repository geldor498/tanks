#pragma once
#include "../utility/graphics.h"
#include "messages.h"
#include "MapGenerator.h"
#include <fstream>
#include <set>
#include "GameConsts.h"
using std::ofstream;
using std::set;

struct CTankInfoLI
{
	CString m_sTeamName;
	CString m_sTankName;
	double m_fArmor;
	double m_fFuel;

	CTankInfoLI()
		:m_fArmor(0)
		,m_fFuel(0)
	{}
};//struct CTankInfo

typedef std::list<CTankInfoLI> TanksInfoLst;

#define M_PI			3.14159265358979323846

class CPhisicsTank;
class CArtefact;
struct CGameMap;



class CPHelper{
public:
	static bool closeToZero(double num){
		return (fabs(num)<1.0e-5)?true: false;
	}
	static int signum(double x){
		return x>=0.?1:-1;
	}

	static double norm2_2d(double x, double y){
		return x*x+y*y;
	}
	static double norm2_2d(Point3DT<double> _point){
		return _point.x*_point.x+_point.y*_point.y;
	}

	static double norm2_3d(Point3DT<double> _point){
		return _point.x*_point.x+_point.y*_point.y+_point.z*_point.z;
	}

	static double norm_2d(double x, double y){
		return sqrt(x*x+y*y);
	}
	static double norm_2d(Point3DT<double> _point){
		return sqrt(_point.x*_point.x+_point.y*_point.y);
	}

	static double norm_3d(Point3DT<double> _point){
		return sqrt(_point.x*_point.x+_point.y*_point.y+_point.z*_point.z);
	}


	static double tr_x(double x){
		double scale = singleton<CGameConsts>::get().scale();
		double width = singleton<CGameConsts>::get().width();
		return x/scale + 0.5*width;
	}

	static double tr_y(double y){
		double scale = singleton<CGameConsts>::get().scale();
		double height = singleton<CGameConsts>::get().height();
		return y/scale + 0.5*height;
	}

	static void set_to_zero(Point3DT<double>& v){
		v[0] = 0.;
		v[1] = 0.;
		v[2] = 0.;
	}

	static void rotate2d(Point3DT<double>&v,double c, double s){
		double v_x = v[0];
		v[0] = c*v[0] + s*v[1];
		v[1] = -s*v_x + c*v[1];
	}

	static double scalar_product2d(Point3DT<double>&v1, Point3DT<double>&v2){
		return v1.x*v2.x + v1.y*v2.y;
	}

	static void print2dvect(Point3DT<double> &v, ofstream& out ){
		out<<v.x<<" "<<v.y<<std::endl;
	}
};
class CPhisicsWorld{
public:
	CPhisicsWorld(){};
	double getFrictionCoeficient(double x, double y){
		return 1.;
	}
};

class CMassPoint{
public:	
	double m_mass;//масса
	Point3DT<double> m_mass_center;//координата центра масс; 
	Point3DT<double> m_ort;//вектор направления;
	Point3DT<double> m_velocity;//вектор скорости; 
	Point3DT<double> m_acceleration;//вектор ускорения;
	Point3DT<double> m_const_forces;//постоянно действующие силы (сила тяжести, например)
	Point3DT<double> m_impulse_forces;//импульсные силы (действуют короткий промежуток времени)
	double m_sphereRadious;//радиус ограничивающей сферы;
	double m_friction_coefficient;//коэфициент сопративления качению
	double m_air_resistance_coefficient;//коэфициент сопративления воздуха  -  обусловливается формой объекта
	double m_internal_friction_coeficient;//коэфициент внетреннего трения
	double m_direction;//угол изменения направления
	bool force_applied;//флаг, определяющий были ли приложены уже силы
	CMassPoint()
		:m_mass_center(100,100,100,1)
	{
		m_sphereRadious = singleton<CGameConsts>::get().get_SphereRadius();
		m_air_resistance_coefficient = singleton<CGameConsts>::get().get_AirResistanceCoef();
		m_internal_friction_coeficient = singleton<CGameConsts>::get().get_InternalFrictionCoef();
		m_friction_coefficient = singleton<CGameConsts>::get().get_FrictionCoef();
		m_ort.x = m_ort.z = 0.;
		m_ort.y = 1.;
		m_ort.w = 1.;
		m_velocity.x =m_velocity.y =m_velocity.z = 0.;
		m_acceleration.x =m_acceleration.y = m_acceleration.z = 0.;
		m_const_forces.x = m_const_forces.y = m_const_forces.z = 0.;
		m_impulse_forces.x = m_impulse_forces.y = m_impulse_forces.z = 0.;
		force_applied = true;
		m_direction = 0;
	};

	CMassPoint(
		double _mass,
		double _sphereRadious,//радиус ограничивающей сферы;
		Point3DT<double> _centerOfMass,
		Point3DT<double> _ort,
		Point3DT<double> _veolcity,//вектор скорости 
		Point3DT<double> _acceleration,
		Point3DT<double> _const_forces,
		Point3DT<double> _impulse_forces
		):
	m_mass(_mass)
		,m_sphereRadious(_sphereRadious)
		,m_mass_center(_centerOfMass)
		,m_ort(_ort)
		,m_velocity(_veolcity)
		,m_acceleration(_acceleration)
		,m_const_forces(_const_forces)
		,m_impulse_forces(_impulse_forces)
	{};

	virtual void update(double changeInTime)
	{
		//находим сумму сил, действующих на тело
		Point3DT<double> sum_forces = m_const_forces + m_impulse_forces;
		//находим ускорение
		m_acceleration = sum_forces*(1./m_mass);
		//находим скорость
		m_velocity += m_acceleration*changeInTime;	

		// находим координаты центра масс
		m_mass_center += m_velocity*changeInTime;
		//зануляем импульсные силы
		m_impulse_forces[0] = 0.;
		m_impulse_forces[1] = 0.;
		m_impulse_forces[2] = 0.;
	};

/*	double mass(){return m_mass;}
	void mass(double m){m_mass = m;} 

	Point3DT<double> mass_center(){return m_mass_center;}
	void mass_center(Point3DT<double> center_mass){ m_mass_center = center_mass;}

	Point3DT<double> velocity(){return m_velocity;}
	void velocity(Point3DT<double> vel){ m_velocity = vel;}

	Point3DT<double> acceleration(){return m_acceleration;}
	void acceleration(Point3DT<double> a){ m_acceleration = a;}

	Point3DT<double> const_forces(){return m_const_forces;}
	void const_forces(Point3DT<double> f){ m_const_forces = f;}

	Point3DT<double> impulse_forces(){return m_impulse_forces;}
	void impulse_forces(Point3DT<double> f){ m_impulse_forces = f;}

	double sphereRadious(){return m_sphereRadious;}
	void sphereRadious(double a){ m_sphereRadious = a;}

	double friction_coefficient(){return m_friction_coefficient;}
	void friction_coefficient(double c){ m_friction_coefficient = c;}

	double air_resistance_coefficient(){return m_air_resistance_coefficient;}
	void air_resistance_coefficient(double c){ m_air_resistance_coefficient = c;}*/
};

class CGameObject:public CMassPoint{
public:

	CGameObject(){};
	CGameObject(
		double _mass,
		double _sphereRadious,//радиус ограничивающей сферы;
		Point3DT<double> _centerOfMass,
		Point3DT<double> _ort,
		Point3DT<double> _veolcity,//вектор скорости 
		Point3DT<double> _acceleration,
		Point3DT<double> _const_forces,
		Point3DT<double> _impulse_forces
		):CMassPoint(_mass,_sphereRadious,_centerOfMass,_ort,_veolcity,_acceleration, _const_forces,_impulse_forces)
	{};
	virtual void update(double changeInTime)
	{
		//находим сумму сил, действующих на тело
		Point3DT<double> sum_forces = m_const_forces + m_impulse_forces;
		//находим ускорение
		m_acceleration = sum_forces*(1./m_mass);
		//находим скорость
		m_velocity += m_acceleration*changeInTime;	

		// находим координаты центра масс
		m_mass_center += m_velocity*changeInTime;
		//зануляем импульсные силы
		m_impulse_forces[0] = 0.;
		m_impulse_forces[1] = 0.;
		m_impulse_forces[2] = 0.;
	};
};

//детали танка 
struct CTurret{
	double circular_velosity; //скорость поворота башни
	double vertical_velosity; //скорость поднятия/спуска орудия
	double mass;//масса орудия

	double c_TurretAngle; //текущий угол поворота башни
	double n_TurretAngle; //необходимый угол поворота башни

	double c_GunAngle; //текущйи угол орудия
	double n_GunAngle; //текущйи угол орудия

	double up_factor; //ускоряющий коэффициент. что-то вроде калибра орудия
	bool m_bShot; //сигнализирует о необходимости выстрела
	CTurret()
		:vertical_velosity(0) 
		,circular_velosity(0)
		,mass(1)
		,c_TurretAngle(0)
		,n_TurretAngle(0)
		,c_GunAngle(0)
		,n_GunAngle(0)
		,up_factor(1)
		,m_bShot(false)
	{
	};
	CTurret(double _circular_velosity, 
		double _vertical_velosity,
		double _mass): 
	vertical_velosity(_vertical_velosity) 
		,circular_velosity(_circular_velosity)
		,mass(_mass)
		,c_TurretAngle(0)
		,n_TurretAngle(0)
		,c_GunAngle(0)
		,n_GunAngle(0)
		,up_factor(1)
		,m_bShot(false)
	{}
};


//броня. фактически, количество жизни у танка. 
struct CArmor{
	double m_count; 
	double m_mass;
public:
	CArmor()
	{
		m_count = singleton<CGameConsts>::get().get_ArmorBase();
		m_mass = singleton<CGameConsts>::get().get_ArmorMass();
	}
};

//кусок танка, представляющий собой ходовую часть. 
//имеет массу, именно здесь устанавливаются ограничения на максимальную скорость
//для учета разгона/торможения можно аналогично турели ввести кроме текущей скорости еще и 
//небходимую скорость 
struct CChassis{
	double m_mass;//масса 
	double m_max_velocity;//максимальная скорость

	double m_power_left_track_current;//текущая мощность левой гусеницы
	double m_power_right_track_current;//текущая мощность правой гусеницы

	double m_power_left_track_required;//необходимая мощность левой гусеницы
	double m_power_right_track_required;//необходимая мощность правой гусеницы

	Point3DT<double> m_velocity_left_track;
	Point3DT<double> m_velocity_right_track;

	double m_width; //ширина - фактически, расстояние между гусеницами
	CChassis(){
		m_mass = singleton<CGameConsts>::get().get_ChassisMass();
		m_max_velocity = singleton<CGameConsts>::get().get_MaxVelocity();
		m_power_left_track_current = 0;
		m_power_right_track_current = 0;
		m_power_left_track_required = 0;
		m_power_right_track_required = 0;
		m_width = singleton<CGameConsts>::get().get_ChassisWidth();
	}
};

//двигатель. учитывается мощность и масса. Чем мощнее двишатель - 
//тем быстрее ускоряется танк и тем больше может на себе везти
//кроме того, здесь храним информацию о характеристиках расхода топлива
struct CEngine{
	double m_power;
	double m_mass;
	double m_fuel;

	CEngine(){
		m_power = singleton<CGameConsts>::get().get_EnginePower() 
			* singleton<CGameConsts>::get().HorseStrength()
			;
		m_mass = singleton<CGameConsts>::get().get_EngineMass();
		m_fuel = singleton<CGameConsts>::get().get_EngineFuel();
	}
	//считаем, что на расход топлива влияет можель двигателя - 
	//то есть мощность
	double fuel_consumption(
		double _tank_mass //масса танка
		,double _scalar_velocity //скорость движения
		,double _changeInTime)
	{
		if(CPHelper::closeToZero(_scalar_velocity))
			return 0;

		return _changeInTime*singleton<CGameConsts>::get().getFuelConsumptionCoef();
	}

	double get_power()
	{
		return m_fuel>0? m_power: m_power*0.05;
	}

};

// базовый класс артефактов.
class CArtefact: public CGameObject{
public:
	ArtefactTypeEn m_type;
	long m_ID;//глобальный идентефикатор артефакта
	bool m_occupied;//подобран ли артефакт каким-либо танком

	CArtefact(long _ID)
	{
			m_ID = _ID;
			m_mass_center = Point3DT<double>(0,0,0);
			m_mass = 100.;
			m_occupied = false;
			m_sphereRadious = 1.;
			m_velocity = Point3DT<double>(0,0,0);
			m_acceleration = Point3DT<double>(0,0,0);
			m_const_forces = Point3DT<double>(0,0,0);
			m_impulse_forces = Point3DT<double>(0,0,0);
	}
	virtual void apply(CPhisicsTank &_tank) = 0;
};

class CPhisicsTank: public CGameObject{
	
public:
	//typedef std::vector<CArtefact*> Artefacts; 
	typedef std::set<long> OwnArtefacts; 

protected:
	//запчасти танчика
	CArmor    m_armor;
	CChassis  m_chassis;
	CEngine   m_engine;
	CTurret   m_turret;

	CMassPoint m_left_track_point;//условная точка соприкосновения левой гусеницы с землей
	CMassPoint m_right_track_point;//условная точка соприкосновения правойй гусеницы с землей

	

	OwnArtefacts m_artefacts;
	CCriticalSection m_critsect;
	double m_r_velocity;
public:
	ofstream  m_logger;

	CPhisicsTank()
	{
		m_logger.open("log.txt");
		m_sphereRadious = singleton<CGameConsts>::get().get_TankSphereRadius();
		m_mass = m_engine.m_mass + m_engine.m_fuel + m_turret.mass + m_chassis.m_mass;	
		m_left_track_point.m_mass = m_right_track_point.m_mass = m_mass/2.;
		//будем считать, что тип поверхности определяется только центром масс самого танка для упрощения

		Point3DT<double> vect;
		vect.x = m_ort.y;
		vect.y = -m_ort.x;

		m_right_track_point.m_mass_center = m_mass_center + 0.5*m_chassis.m_width* vect;
		m_left_track_point.m_mass_center = m_mass_center  - 0.5*m_chassis.m_width* vect;
		m_left_track_point.m_ort = m_right_track_point.m_ort = m_ort;	

		m_r_velocity = 0.;
	}

	//берем артефакт. при подборе артефакта он исключается из массива
	//игровых объектов и перемещается в танковое хранилище артефактов
	void take_artefact(const CArtefact* _art)
	{
		CAutoLock __al(m_critsect);

		m_artefacts.insert(_art->m_ID);
		m_mass+= _art->m_mass;
	};

	//используем артефакт. должна быть реализация для каждого типа имеющихся артефактов.
	void use_artefact(CArtefact* _art)
	{		
		CAutoLock __al(m_critsect);

		_art->apply(*this);
		m_artefacts.erase(m_artefacts.find(_art->m_ID));
	};
	//выбросить артефакт
	void put_artefact(CArtefact* _art)
	{
		CAutoLock __al(m_critsect);

		m_mass-=_art->m_mass;	
		_art->m_mass_center = m_mass_center;
		m_artefacts.erase(m_artefacts.find(_art->m_ID));
	};



	void set_move(CMoveCmd move)
	{
		CAutoLock __al(m_critsect);

		m_chassis.m_power_left_track_required = move.m_fVLeftTrack;
		m_chassis.m_power_right_track_required = move.m_fVRightTrack;
	}


	double get_dir() 
	{
		CAutoLock __al(m_critsect);

		return (this->m_direction*(180.)/M_PI);
	}

	void set_gun(CShotCmd shot)
	{
		CAutoLock __al(m_critsect);

		m_turret.c_GunAngle = shot.m_fGunAngle;
		m_turret.c_TurretAngle  = shot.m_fTurretAngle;
	}
	//пересчитываем физику 
	void log()
	{
		m_logger<<"==========log=========="<<std::endl;
		m_logger<<"m_mass_crnter: \t"<<m_mass_center.x<<" "<<m_mass_center.y<<std::endl;
		m_logger<<"m_velocity: \t "<<m_velocity.x<<" "<<m_velocity.y<<std::endl;
		m_logger<<"m_acceleration: \t"<<m_acceleration.x<<" "<<m_acceleration.y<<std::endl;
		m_logger<<"m_const_forces: \t"<<m_const_forces.x<<" "<<m_const_forces.y<<std::endl;
		m_logger<<"m_impulse_forces: \t"<<m_impulse_forces.x<<" "<<m_impulse_forces.y<<std::endl;
		m_logger<<"m_lt cur req: \t"<<m_chassis.m_power_left_track_current<<" "<<m_chassis.m_power_left_track_required<<std::endl;
		m_logger<<"m_rt cur req: \t"<<m_chassis.m_power_right_track_current<<" "<<m_chassis.m_power_right_track_required<<std::endl;
		m_logger<<"m_lt imp forces: \t"<<m_left_track_point.m_impulse_forces.x <<"  "<<m_left_track_point.m_impulse_forces.y<<std::endl;
		m_logger<<"m_lt con forces: \t"<<m_left_track_point.m_const_forces.x <<"  "<<m_left_track_point.m_const_forces.y<<std::endl;
		m_logger<<"m_rt imp forces: \t"<<m_left_track_point.m_impulse_forces.x <<"  "<<m_right_track_point.m_impulse_forces.y<<std::endl;
		m_logger<<"m_rt con forces: \t"<<m_left_track_point.m_const_forces.x <<"  "<<m_right_track_point.m_const_forces.y<<std::endl;
		m_logger<<"m_lt   velocity: \t"<<m_left_track_point.m_velocity.x <<"  "<<m_left_track_point.m_velocity.y<<std::endl;
		m_logger<<"m_rt   velocity: \t"<<m_right_track_point.m_velocity.x <<"  "<<m_right_track_point.m_velocity.y<<std::endl;

		m_logger<<std::endl<<std::endl<<std::endl;
	}
	void update(double changeInTime)
	{
		CAutoLock __al(m_critsect);

		//если у танка иссякла броня, то он не может двигается 
		if(changeInTime == 0. || CPHelper::closeToZero(m_armor.m_count))
			return;
		
		m_chassis.m_power_left_track_required  = fabs(m_chassis.m_power_left_track_required)<1.?m_chassis.m_power_left_track_required:CPHelper::signum(m_chassis.m_power_left_track_required);
		m_chassis.m_power_right_track_required = fabs(m_chassis.m_power_right_track_required)<1.?m_chassis.m_power_right_track_required:CPHelper::signum(m_chassis.m_power_right_track_required);
		double delta_left_track = m_chassis.m_power_left_track_required - m_chassis.m_power_left_track_current;
		double delta_right_track= m_chassis.m_power_right_track_required - m_chassis.m_power_right_track_current;
		//double delta_rot = m_chassis.m_power_right_track_required - m_chassis.m_power_left_track_required;

		double signum_const_forces_lt = CPHelper::signum( m_ort.x!=0.?m_left_track_point.m_velocity.x/m_ort.x:m_left_track_point.m_velocity.y/m_ort.y);
		double signum_const_forces_rt = CPHelper::signum( m_ort.x!=0.?m_right_track_point.m_velocity.x/m_ort.x:m_right_track_point.m_velocity.y/m_ort.y);
		//m_right_track_point
		//проверяем, двигается ли танк сонаправленно орту или в противоположном напрвлении

		m_left_track_point.m_const_forces = -1.*signum_const_forces_lt* m_ort * m_left_track_point.m_mass * singleton<CGameConsts>::get().PH_g() *(1.- m_friction_coefficient);									
		m_left_track_point.m_const_forces += (-1.)*m_air_resistance_coefficient * m_left_track_point.m_velocity;
		m_left_track_point.m_const_forces += (-1.)*m_internal_friction_coeficient * m_left_track_point.m_velocity *m_left_track_point.m_mass;

		m_right_track_point.m_const_forces = -1.*signum_const_forces_rt* m_ort * m_right_track_point.m_mass * singleton<CGameConsts>::get().PH_g() *(1.- m_friction_coefficient);									
		m_right_track_point.m_const_forces += (-1.)*m_air_resistance_coefficient * m_right_track_point.m_velocity;
		m_right_track_point.m_const_forces += (-1.)*m_internal_friction_coeficient * m_right_track_point.m_velocity *m_right_track_point.m_mass;

		//модуль силы, которая сейчас приложено к каждой из гусениц
		double force_left_track = m_engine.m_power * m_chassis.m_power_left_track_current;
		double force_right_track= m_engine.m_power * m_chassis.m_power_right_track_current;

		//модуль силы, которая будет приложена
		force_left_track +=delta_left_track*m_engine.get_power();
		force_right_track+=delta_right_track*m_engine.get_power();



		m_left_track_point.m_impulse_forces  = m_ort*force_left_track;
		m_right_track_point.m_impulse_forces = m_ort*force_right_track;
//m_logger<<" lt
//print2dvect()
//log();
		if(CPHelper::norm2_2d(m_left_track_point.m_const_forces) > CPHelper::norm2_2d( m_left_track_point.m_impulse_forces) 
			&& CPHelper::norm2_2d(m_right_track_point.m_const_forces)>CPHelper::norm2_2d( m_right_track_point.m_impulse_forces)
			&& CPHelper::closeToZero(CPHelper::norm_2d(m_velocity))
			&& CPHelper::closeToZero(CPHelper::norm_2d(m_acceleration))){
			//m_logger<<"      no move!!! so sad!"<<std::endl;
			return;
		}
		
		m_left_track_point.m_acceleration = 1./m_left_track_point.m_mass *(m_left_track_point.m_const_forces + m_left_track_point.m_impulse_forces );
		m_right_track_point.m_acceleration = 1./m_right_track_point.m_mass *(m_right_track_point.m_const_forces + m_right_track_point.m_impulse_forces );

		//есть все силы - как импульсные, так и векторные. поэтому можно найти результирующую
		//силу для рассчета линейной динамики. сразу же найдем и линейное ускорение
		m_acceleration = 1./m_mass *(m_left_track_point.m_const_forces + m_left_track_point.m_impulse_forces + m_right_track_point.m_const_forces + m_right_track_point.m_impulse_forces);
		m_velocity += m_acceleration * changeInTime;
		m_mass_center += m_velocity * changeInTime;

		//теперь у нас есть почти честно посчитанные модули скоростей
		//нужно определить скорость центар масс (т.е. направление и величину).


		m_chassis.m_power_left_track_current = m_chassis.m_power_left_track_required;
		m_chassis.m_power_right_track_current = m_chassis.m_power_right_track_required;
		
		m_chassis.m_velocity_left_track = m_left_track_point.m_velocity;// =R>0? CPHelper::norm_2d(m_velocity)/R*r_lt*m_ort:m_velocity;
		m_chassis.m_velocity_right_track = m_right_track_point.m_velocity;//s =R>0? CPHelper::norm_2d(m_velocity)/R*r_rt*m_ort:m_velocity;
		//находим расстояние до оси вращения
		long double a11,a12,a21,a22,b1,b2;
		Point3DT<double> vect = m_ort;
		vect.x = m_ort.y;
		vect.y = -m_ort.x;

		Point3DT<double> p1, p2, v1,v2, tt;
		p1 = m_mass_center + vect*m_chassis.m_width*0.5;//правая гусеница
		p2 = m_mass_center - vect*m_chassis.m_width*0.5;//левая гусеница

		a11 = p1.y-p2.y;
		a12 = p2.x-p1.x;
		b1 = -(p1.x*p2.y-p1.y*p2.x);

		v1 = p1 + m_right_track_point.m_impulse_forces + m_right_track_point.m_const_forces;//m_right_track_point.m_velocity+m_right_track_point.m_acceleration*changeInTime ;//;//
		v2 = p2 + m_left_track_point.m_const_forces + m_left_track_point.m_impulse_forces;//m_left_track_point.m_velocity + m_left_track_point.m_acceleration*changeInTime ;//;//
		a21 = v1.y-v2.y;
		a22 = v2.x-v1.x;
		b2 = -(v1.x*v2.y-v1.y*v2.x);
		long double det = a11*a22-a21*a12;
		Point3DT<double> rotation_point;

		if(!CPHelper::closeToZero(det)){
			rotation_point.x = (b1*a22-b2*a12)/det;
			rotation_point.y = (a11*b2 - a21*b1)/det;
		}else{
			rotation_point= m_mass_center;
		}
		p1 -=rotation_point;
		p2 -=rotation_point;
		double R = CPHelper::norm_2d(m_mass_center - rotation_point);
		Point3DT<double> v = 
			p2.cross(m_left_track_point.m_const_forces + m_left_track_point.m_impulse_forces)
			+p1.cross(m_right_track_point.m_impulse_forces + m_right_track_point.m_const_forces);//vect_rt.cross(m_right_track_point.m_impulse_forces + m_right_track_point.m_const_forces);
		double moment_force = v.z;
		double moment_inertia = pow(CPHelper::norm_2d(p2),2.)*m_left_track_point.m_mass + pow(CPHelper::norm_2d(p1),2.)*m_right_track_point.m_mass;
		double r_acceleration = !CPHelper::closeToZero(moment_force)? moment_force/moment_inertia:0.;

		m_r_velocity += r_acceleration * changeInTime;
		double delta_direction = m_r_velocity * changeInTime;
		m_direction += delta_direction;
		//и повернуть танк в нужном направлении
		double c = cos(delta_direction);
		double s = sin(delta_direction);

		m_right_track_point.m_velocity += m_right_track_point.m_acceleration * changeInTime;
		m_left_track_point.m_velocity += m_left_track_point.m_acceleration * changeInTime;

		CPHelper::rotate2d(m_right_track_point.m_velocity,c,s);
		CPHelper::rotate2d(m_left_track_point.m_velocity,c,s);
		CPHelper::rotate2d(m_ort,c,s);
		CPHelper::rotate2d(m_velocity,c,s);

		CPHelper::set_to_zero(m_impulse_forces);
		CPHelper::set_to_zero(m_left_track_point.m_impulse_forces);
		CPHelper::set_to_zero(m_right_track_point.m_impulse_forces);

		m_chassis.m_velocity_left_track = m_left_track_point.m_velocity;// =R>0? 1./R*CPHelper::norm_2d(p2)*m_velocity:m_velocity;
		m_chassis.m_velocity_right_track = m_right_track_point.m_velocity;// =R>0? 1./R*CPHelper::norm_2d(p1)*m_velocity:m_velocity;

		if(CPHelper::closeToZero(CPHelper::norm_2d(m_chassis.m_velocity_left_track)) ){
			CPHelper::set_to_zero(m_chassis.m_velocity_left_track);
			CPHelper::set_to_zero(m_left_track_point.m_velocity);
		}

		if(CPHelper::closeToZero(CPHelper::norm_2d(m_chassis.m_velocity_right_track)) ){
			CPHelper::set_to_zero(m_chassis.m_velocity_right_track);
			CPHelper::set_to_zero(m_right_track_point.m_velocity);
		}

		double d_fuel = m_engine.fuel_consumption(m_mass,sqrt( CPHelper::norm2_2d(m_velocity) ), changeInTime);
		m_engine.m_fuel -= d_fuel;
		m_mass -=d_fuel;
		m_left_track_point.m_mass = m_right_track_point.m_mass = m_mass/2.;

	};
	void stop_tank()
	{
		CAutoLock __al(m_critsect);
		
		Point3DT<double> zeroPoint(0.,0.,0.);
		
		m_velocity = zeroPoint;
		m_acceleration = zeroPoint;

		m_chassis.m_power_left_track_current = 0.;
		m_chassis.m_power_right_track_current = 0.;

		m_chassis.m_power_left_track_required = 0.;
		m_chassis.m_power_right_track_required = 0.;

		m_const_forces = zeroPoint;
		m_impulse_forces = zeroPoint;

		m_left_track_point.m_const_forces = zeroPoint;
		m_left_track_point.m_impulse_forces = zeroPoint;

		m_right_track_point.m_const_forces = zeroPoint;
		m_right_track_point.m_impulse_forces = zeroPoint;

		m_right_track_point.m_velocity = zeroPoint;
		m_left_track_point.m_velocity = zeroPoint;
	};


	//применяется при столкновениях с ландшафтом.
	//возвращает танк на некоторое расстояние в обратном направлении не разворачивая его
	void retrieve_tank(double _len, int _sign){
		CAutoLock __al(m_critsect);
		m_mass_center -= _sign * _len * m_ort;
	}

	double get_turret_angle() const 
	{
		CAutoLock __al(m_critsect);
		return m_turret.n_TurretAngle;
	}

	double get_gun_angle() const 
	{
		CAutoLock __al(m_critsect);
		return m_turret.n_GunAngle;
	}

	void modify_armor(double _delta_armor)
	{
		CAutoLock __al(m_critsect);		
		m_armor.m_count += _delta_armor;
		if(m_armor.m_count <=0.)
			m_armor.m_count = 0;
	}

	void modify_fuel(double _delta_fuel)
	{
		CAutoLock __al(m_critsect);
		m_engine.m_fuel += _delta_fuel;
	}

	bool find_artefact(long _nArtefactID) const
	{
		CAutoLock __al(m_critsect);
		return m_artefacts.find(_nArtefactID)!=m_artefacts.end();
	}

	double get_armor() const
	{
		CAutoLock __al(m_critsect);
		return m_armor.m_count;
	}

	double get_fuel() const 
	{
		CAutoLock __al(m_critsect);
		return m_engine.m_fuel;
	}

	double get_mass() const 
	{
		CAutoLock __al(m_critsect);
		return m_mass;
	}

	OwnArtefacts get_artefacts() const
	{
		CAutoLock __al(m_critsect);
		return m_artefacts;
	}
};




//сие есть игровой объект типа снаряд. 
//при каждом выстреле создается новый.
//
class CShell: public CGameObject
{
	double m_time_step;
public:
	double m_impulse_forces_module;//модуль сил, выталкивающих снаряд из орудия
	double m_killability; //поражающая способность. зависит от типа снаряда
	double m_killability2;//поражающая способность осколков и взрывной волны (непрямое попадание)
	double m_sphere2;// радиус второй поражающей сферы - для учета взрывной волны
	double m_time_passed;//сколько прошло времени с момента старта снаряда
	std::vector<Point3DT<double> > m_path;//путь пройденный снарядом
	bool m_interaction;//флаг взаиммодействия - если оно произошло, проверим близлежащие объектыф
	long m_id;
	long m_nTankID;
	double m_fHit;

	CShell(long _id = -1,long _nTankID=-1)
		:m_id(_id)
		,m_nTankID(_nTankID)
		,m_fHit(0)
	{
		m_time_step= 1./50.;
		m_mass = singleton<CGameConsts>::get().get_ShellMass();
		m_impulse_forces_module = singleton<CGameConsts>::get().get_ShellImpulseForcesModule();
		m_killability = singleton<CGameConsts>::get().get_ShellKillability();
		m_killability2 = singleton<CGameConsts>::get().get_ShellKillability2();
		m_sphere2 = singleton<CGameConsts>::get().get_ShellSphere2();
		m_sphereRadious = singleton<CGameConsts>::get().get_ShellSphereRadius();
		m_interaction = false;
		m_time_passed = 0;
	};

	void init(const CPhisicsTank& _tank)
	{
		
		Point3DT<double> ort = _tank.m_ort;
		
		double c = cos(_tank.get_turret_angle()*M_PI/180.);
		double s = sin(_tank.get_turret_angle()*M_PI/180.);
		double tmp = 0;
		//поворот в горизонтальной плоскости - Оxy
		tmp = ort.x;
		ort.x = c*ort.x + s* ort.y;
		ort.y = -s*tmp + c*ort.y;

		c = cos(_tank.get_gun_angle()*M_PI/180.);
		s = sin(_tank.get_gun_angle()*M_PI/180.);
		ort.z = s*sqrt(sqr(ort.x) + sqr(ort.y) + sqr(ort.z));
		ort.x *= c;
		ort.y *= c;
		
		m_mass_center = _tank.m_mass_center;
		m_mass_center.z = 2.;
		m_mass_center+= 1.5*_tank.m_sphereRadious*ort;
		m_ort = ort;

		m_impulse_forces = m_ort * m_impulse_forces_module*singleton<CGameConsts>::get().scale();
		
		evaluate_path();
	};


	void evaluate_path()
	{
		while(m_mass_center.z>0.)
		{			
			m_const_forces = (-1)* m_air_resistance_coefficient * m_velocity ;
			m_const_forces.z += (-1)*m_mass*singleton<CGameConsts>::get().PH_g();
			CGameObject::update(m_time_step);
			m_path.push_back(m_mass_center);
		}
	};


	void update(double changeInTime)
	{
		m_time_passed +=changeInTime;

		unsigned int index = (unsigned int)(m_time_passed/m_time_step);	
		if(index <m_path.size())
		{
			m_mass_center = m_path[index];
		}
		else if(m_path.size()>0)
		{
			m_mass_center = m_path[m_path.size()-1];
		}
	}

	double get_fly_time() const 
	{
		return m_time_passed;
	}

	long get_id() const
	{
		return m_id;
	}

	long get_tank_id() const
	{
		return m_nTankID;
	}

	bool fallback() const 
	{
		unsigned int index = (unsigned int)(m_time_passed/m_time_step);	
		return index>=m_path.size();
	}

	void get_nears(double _dt,CFPoint3D& _pt0,CFPoint3D& _pt1)
	{
		int index = (int)((m_time_passed-_dt)/m_time_step);	
		if(index<0) index=0;
		if(index<(signed int)m_path.size())
			_pt0 = m_path[index];
		else if(m_path.size()>0)
			_pt0 = m_path[m_path.size()-1];
		else 
			_pt0 = m_mass_center;
		_pt1 = m_mass_center;
	}
};


class CCollissionFinder{
	
public:
	static void collision(CPhisicsTank& _tank, CShell &_shell){
		Point3DT<double> tmp = _tank.m_mass_center - _shell.m_mass_center;
		if(!_shell.m_interaction)
		{
			if(CPHelper::norm2_3d(tmp)<pow(_tank.m_sphereRadious + _shell.m_sphereRadious,2))
			{
				//танк, в который попали не должен быть второй раз задет ударной волной. 
				//поэтому просто добавим ее к броне
				_tank.modify_armor(-_shell.m_killability+_shell.m_killability2);
				_shell.m_interaction = true;
				_shell.m_fHit += _shell.m_killability-_shell.m_killability2;
			}
		}
		else
		{
			if(CPHelper::norm2_3d(tmp)<pow(_tank.m_sphereRadious + _shell.m_sphere2,2))
			{
				_tank.modify_armor(-_shell.m_killability2);
				_shell.m_fHit += _shell.m_killability2;
			}
		}	
	};
	
	//столкновение с ландшафтом.
	static void collision(CPhisicsTank& _tank, const CGameMap& _map)
	{
		//скорость танка до столкновения. 
		Point3DT<double> tmp_velocity = _tank.m_velocity;
		int sign = CPHelper::signum( _tank.m_ort.x!=0.?tmp_velocity.x/_tank.m_ort.x:tmp_velocity.y/_tank.m_ort.y);
		
		//дистанция, которую прошел танк в препятствии. вычисляется приблизительно - 
		//для простоты считаем, что время движения с указанной скоростью - 0.1 секунды
		double distance = 0.3 * CPHelper::norm_2d(tmp_velocity);
		
		//находим точки, которые будем проверять на столкновение 
		Point3DT<double> tmp = _tank.m_mass_center;
		tmp =  _tank.m_mass_center+_tank.m_sphereRadious * _tank.m_ort;

		if( _map.get_height((size_t)CPHelper::tr_x(tmp.x), (size_t)CPHelper::tr_y(tmp.y)) > 0 
			|| _map.get_passability((size_t)CPHelper::tr_x(tmp.x), (size_t)CPHelper::tr_y(tmp.y)) == 0.){
			//_tank.m_logger<<"  badabooom!!!!!"<<std::endl;
			_tank.stop_tank();
			_tank.retrieve_tank(distance, sign);
		}
		tmp =  _tank.m_mass_center-_tank.m_sphereRadious * _tank.m_ort;

		if(_map.get_height((size_t)CPHelper::tr_x(tmp.x), (size_t)CPHelper::tr_y(tmp.y)) > 0 
			|| _map.get_passability((size_t)CPHelper::tr_x(tmp.x), (size_t)CPHelper::tr_y(tmp.y)) == 0.){
			//_tank.m_logger<<"  badabooom!!!!!"<<std::endl;
			_tank.stop_tank();
			_tank.retrieve_tank(distance, sign);
		}
	};

	// столкновения снаряда с ландшафтом
	static void collision(CShell& _shell, const CGameMap& _map){
		double scale = singleton<CGameConsts>::get().scale();
		if(CPHelper::closeToZero(_shell.m_mass_center.z)
			|| ( _shell.m_mass_center.z <= scale*_map.get_height((size_t)CPHelper::tr_x(_shell.m_mass_center.x), (size_t)CPHelper::tr_y(_shell.m_mass_center.y))))
			_shell.m_interaction = true;
	};
};

class CArmorArtefact: public CArtefact{
public:
	double m_armor_cnt;
	CArmorArtefact(long _ID):CArtefact(_ID)
	{
		m_armor_cnt = singleton<CGameConsts>::get().get_ArmorArtefactCount();
		m_mass = singleton<CGameConsts>::get().get_ArmorArtefactMass();
	}

	void apply(CPhisicsTank &_tank)
	{
		_tank.modify_armor(m_armor_cnt);
	}
};


class CFuelArtefact: public CArtefact{
public:
	CFuelArtefact(long _ID):CArtefact(_ID)
	{
		m_type = Artefact_Fuel;
		m_mass = singleton<CGameConsts>::get().get_FuelArtefactMass();
	}

	void apply(CPhisicsTank &_tank)
	{
		_tank.modify_fuel(m_mass); 
	}
};


