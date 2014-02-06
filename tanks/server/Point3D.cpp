#include "stdafx.h"
#include "Point3D.h"

Point3D::Point3D(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

double Point3D::get_x()
{
	return this->x;
}

double Point3D::get_y()
{
	return this->y;
}

double Point3D::get_z()
{
	return this->z;
}