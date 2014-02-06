#include <math.h>
#include "Point3D.h"
class Vector3D
{
private:
	double x;
	double y;
	double z;
public:
	Vector3D();
	Vector3D(double x, double y, double z);
	Vector3D(Point3D& point1, Point3D& point2);
	double get_x();
	double get_y();
	double get_z();
	double length();
	void normalize();
	const Vector3D& operator = (const Vector3D& v);
	Vector3D operator + (const Vector3D& v);
	Vector3D operator - (const Vector3D& v);
	Vector3D operator * (double m);
	Vector3D operator / (double d);
	Vector3D operator * (const Vector3D& v);
	const Vector3D& operator += (const Vector3D& v);
	const Vector3D& operator -= (const Vector3D& v);
	const Vector3D& operator *= (double m);
	const Vector3D& operator /= (double d);
	const Vector3D& operator *= (const Vector3D& v);
};