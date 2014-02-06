#include "stdafx.h"
#include "Vector3D.h"

Vector3D::Vector3D()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

Vector3D::Vector3D(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3D::Vector3D(Point3D& point1, Point3D& point2)
{
	x = point2.get_x() - point1.get_x();
	y = point2.get_y() - point1.get_y();
	z = point2.get_z() - point1.get_z();
}

double Vector3D::get_x()
{
	return this->x;
}

double Vector3D::get_y()
{
	return this->y;
}

double Vector3D::get_z()
{
	return this->z;
}

double Vector3D::length()
{
	double length = sqrt(x*x + y*y + z*z);
	return length;
}

void Vector3D::normalize()
{
	double length = this->length();
	x/= length;
	y/= length;
	z/= length;
}

const Vector3D& Vector3D::operator = (const Vector3D& v)
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	return *this;
}

Vector3D Vector3D::operator + (const Vector3D& v)
{
	double new_x = this->x + v.x;
	double new_y = this->y + v.y;
	double new_z = this->z + v.z;
	return Vector3D(new_x, new_y, new_z);
}
Vector3D Vector3D::operator - (const Vector3D& v)
{
	double new_x = this->x - v.x;
	double new_y = this->y - v.y;
	double new_z = this->z - v.z;
	return Vector3D(new_x, new_y, new_z);
}
Vector3D Vector3D::operator * (double m)
{
	double new_x = m * this->x;
	double new_y = m * this->y;
	double new_z = m * this->z;
	return Vector3D(new_x, new_y, new_z);
}
Vector3D Vector3D::operator / (double d)
{
	double new_x = this->x / d;
	double new_y = this->y / d;
	double new_z = this->z / d;
	return Vector3D(new_x, new_y, new_z);
}
Vector3D Vector3D::operator * (const Vector3D& v)
{
	double x1 = this->x;
	double y1 = this->y;
	double z1 = this->z;
	double x2 = v.x;
	double y2 = v.y;
	double z2 = v.z;
	double new_x = y1*z2 - y2*z1;
	double new_y = x1*z2 - x2*z1;
	double new_z = x1*y2 - x2*y1;
	return Vector3D(new_x, new_y, new_z);
}

const Vector3D& Vector3D::operator += (const Vector3D& v)
{
	*this = (*this) + v;
	return *this;
}

const Vector3D& Vector3D::operator -= (const Vector3D& v)
{
	*this = (*this) - v;
	return *this;
}

const Vector3D& Vector3D::operator *= (double m)
{
	*this = (*this) * m;
	return *this;
}

const Vector3D& Vector3D::operator /= (double d)
{
	*this = (*this) / d;
	return *this;
}

const Vector3D& Vector3D::operator *= (const Vector3D& v)
{
	*this = (*this) * v;
	return *this;
}