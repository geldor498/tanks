#include "stdafx.h"
#include "GLArrays.h"

struct NormalFan
{
	Vector3D west;
	Vector3D north;
	Vector3D east;
	Vector3D south;
	NormalFan()
	{
		west  = Vector3D(0, 0, 1);
		north = Vector3D(0, 0, 1);
		east  = Vector3D(0, 0, 1);
		south = Vector3D(0, 0, 1);
	}
};

GLArrays::GLArrays(CGameMap& map, double scale)
{
	createMountainsArrays(map, scale);
	createTreesArrays(map, scale);
}

size_t GLArrays::get_SizeMountains()
{
	return this->size_mountains;
}
double* GLArrays::get_VertexMountains()
{
	return this->vertex_mountains;
}
double* GLArrays::get_NormalMountains()
{
	return this->normal_mountains;
}

size_t GLArrays::get_SizeTrees()
{
	return this->size_trees;
}
double* GLArrays::get_VertexTrees()
{
	return this->vertex_trees;
}
double* GLArrays::get_NormalTrees()
{
	return this->normal_trees;
}

void addPoint(double* vertex, double* normal, size_t pos, Point3D p, Vector3D n)
{
	vertex[3*pos]   = p.get_x();
	vertex[3*pos+1] = p.get_z();
	vertex[3*pos+2] = p.get_y();
	normal[3*pos]   = n.get_x();
	normal[3*pos+1] = n.get_z();
	normal[3*pos+2] = n.get_y();
}

NormalFan** emptyNormalFans(size_t w, size_t h)
{
	NormalFan** array = new NormalFan*[w];
	for(size_t i=0; i<w; i++)
	{
		array[i] = new NormalFan[h];
	}
	return array;
}

NormalFan** normalFanArray_Mountains(size_t w, size_t h, CGameMap& map)
{
	NormalFan** normals = emptyNormalFans(w, h);
	for(size_t i=0; i<w; i++)
	{
		for(size_t j=0; j<h; j++)
		{
			if(map.get_height(i, j) == 0
			&& map.get_height(i+1, j) == 0
			&& map.get_height(i, j+1) == 0
			&& map.get_height(i+1, j+1) == 0
			) continue;
			double x0 = ((double)i-(double)0.5*w);
			double x1 = ((double)(i+1)-(double)0.5*w);
			double y0 = ((double)j-(double)0.5*h);
			double y1 = ((double)(j+1)-(double)0.5*h);
			double h00 = map.get_height(i, j);
			double h01 = map.get_height(i, j+1);
			double h10 = map.get_height(i+1, j);
			double h11 = map.get_height(i+1, j+1);
			double x_m = (x0 + x1) / 2;
			double y_m = (y0 + y1) / 2;
			double z_m = (h00 + h01 + h11 + h10) / 4;
			Point3D p00(x0, y0, h00);
			Point3D p01(x0, y1, h01);
			Point3D p10(x1, y0, h10);
			Point3D p11(x1, y1, h11);
			Point3D p_m(x_m, y_m, z_m);
			Vector3D v00(p_m, p00);
			Vector3D v01(p_m, p01);
			Vector3D v10(p_m, p10);
			Vector3D v11(p_m, p11);
			normals[i][j].west  = v00 * v01;
			normals[i][j].north = v01 * v11;
			normals[i][j].east  = v11 * v10;
			normals[i][j].south = v10 * v00;
		}
	}
	return normals;
}

NormalFan** normalFanArray_Trees(size_t w, size_t h, CGameMap& map)
{
	NormalFan** normals = emptyNormalFans(w, h);
	for(size_t i=0; i<w; i++)
	{
		for(size_t j=0; j<h; j++)
		{
			if(map.get_treeheight(i, j) == 0
			&& map.get_treeheight(i+1, j) == 0
			&& map.get_treeheight(i, j+1) == 0
			&& map.get_treeheight(i+1, j+1) == 0
			) continue;
			double x0 = ((double)i-(double)0.5*w);
			double x1 = ((double)(i+1)-(double)0.5*w);
			double y0 = ((double)j-(double)0.5*h);
			double y1 = ((double)(j+1)-(double)0.5*h);
			double h00 = map.get_treeheight(i, j);
			double h01 = map.get_treeheight(i, j+1);
			double h10 = map.get_treeheight(i+1, j);
			double h11 = map.get_treeheight(i+1, j+1);
			double x_m = (x0 + x1) / 2;
			double y_m = (y0 + y1) / 2;
			double z_m = (h00 + h01 + h11 + h10) / 4;
			Point3D p00(x0, y0, h00);
			Point3D p01(x0, y1, h01);
			Point3D p10(x1, y0, h10);
			Point3D p11(x1, y1, h11);
			Point3D p_m(x_m, y_m, z_m);
			Vector3D v00(p_m, p00);
			Vector3D v01(p_m, p01);
			Vector3D v10(p_m, p10);
			Vector3D v11(p_m, p11);
			normals[i][j].west  = v00 * v01;
			normals[i][j].north = v01 * v11;
			normals[i][j].east  = v11 * v10;
			normals[i][j].south = v10 * v00;
		}
	}
	return normals;
}

Vector3D** mainNormalArray(size_t w, size_t h, NormalFan** fanArr)
{
	Vector3D** arr = new Vector3D*[w];
	for(size_t i=0; i<w; i++)
	{
		arr[i] = new Vector3D[h];
	}

	for(size_t i=0; i<w; i++)
	{
		for(size_t j=0; j<h; j++)
		{
			Vector3D normal;
			if(i>0 && j>0)
				normal += fanArr[i-1][j-1].north + fanArr[i-1][j-1].east;
			if(i>0 && j<h-1)
				normal += fanArr[i-1][j].east + fanArr[i-1][j].south;
			if(i<w-1 && j<h-1)
				normal += fanArr[i][j].south + fanArr[i][j].west;
			if(i<w-1 && j>0)
				normal += fanArr[i][j-1].west + fanArr[i][j-1].north;
			normal.normalize();
			arr[i][j] = normal;
		}
	}
	return arr;
}

Vector3D** medianNoramlArray(size_t w, size_t h, NormalFan** fanNormals)
{
	Vector3D** arr = new Vector3D*[w];
	for(size_t i=0; i<w; i++)
	{
		arr[i] = new Vector3D[h];
	}

	for(size_t i=0; i<w; i++)
	{
		for(size_t j=0; j<h; j++)
		{
			NormalFan fan = fanNormals[i][j];
			Vector3D normal = fan.west + fan.north + fan.east + fan.south;
			normal.normalize();
			arr[i][j] = normal;
		}
	}
	return arr;
}

void GLArrays::createMountainsArrays(CGameMap& map, double scale)
{
	size_t w = map.width();
	size_t h = map.height();

	size_mountains = 0;
	for(size_t i=0; i<w-1; i++)
	{
		for(size_t j=0; j<h-1; j++)
		{
			if(!(map.get_object(i, j)==Obj_Rock
			|| map.get_object(i+1, j)==Obj_Rock
			|| map.get_object(i, j+1)==Obj_Rock
			|| map.get_object(i+1, j+1)==Obj_Rock
			)) continue;
			size_mountains += 6;
		}
	}
	vertex_mountains = new double[3*size_mountains];
	normal_mountains = new double[3*size_mountains];

	NormalFan** fanNormals   = normalFanArray_Mountains(w-1, h-1, map);
	Vector3D** mainNormals   = mainNormalArray(w, h, fanNormals);
	Vector3D** medianNormals = medianNoramlArray(w-1, h-1, fanNormals);

	size_t counter = 0;
	for(size_t i=0; i<w-1; i++)
	{
		for(size_t j=0; j<h-1; j++)
		{
			if(!(map.get_object(i, j)==Obj_Rock
			|| map.get_object(i+1, j)==Obj_Rock
			|| map.get_object(i, j+1)==Obj_Rock
			|| map.get_object(i+1, j+1)==Obj_Rock
			)) continue;
			double x0 = ((double)i-(double)0.5*w)*scale;
			double x1 = ((double)(i+1)-(double)0.5*w)*scale;
			double y0 = ((double)j-(double)0.5*h)*scale;
			double y1 = ((double)(j+1)-(double)0.5*h)*scale;
			double h00 = map.get_height(i, j) * scale;
			double h01 = map.get_height(i, j+1) * scale;
			double h10 = map.get_height(i+1, j) * scale;
			double h11 = map.get_height(i+1, j+1) * scale;
			double x_m = (x0 + x1) / 2;
			double y_m = (y0 + y1) / 2;
			double z_m = (h00 + h01 + h11 + h11) / 4;
			Point3D p00(x0, y0, h00);
			Point3D p01(x0, y1, h01);
			Point3D p10(x1, y0, h10);
			Point3D p11(x1, y1, h11);
			Point3D p_m(x_m, y_m, z_m);

			addPoint(vertex_mountains, normal_mountains, counter, p_m, medianNormals[i][j]);
			counter++;
			addPoint(vertex_mountains, normal_mountains, counter, p00, mainNormals[i][j]);
			counter++;
			addPoint(vertex_mountains, normal_mountains, counter, p01, mainNormals[i][j+1]);
			counter++;
			addPoint(vertex_mountains, normal_mountains, counter, p11, mainNormals[i+1][j+1]);
			counter++;
			addPoint(vertex_mountains, normal_mountains, counter, p10, mainNormals[i+1][j]);
			counter++;
			addPoint(vertex_mountains, normal_mountains, counter, p00, mainNormals[i][j]);
			counter++;

		}
	}

}

void GLArrays::createTreesArrays(CGameMap& map, double scale)
{
	size_t w = map.width();
	size_t h = map.height();

	size_trees = 0;
	for(size_t i=0; i<w-1; i++)
	{
		for(size_t j=0; j<h-1; j++)
		{
			if(!(map.get_object(i, j)==Obj_Tree
			|| map.get_object(i+1, j)==Obj_Tree
			|| map.get_object(i, j+1)==Obj_Tree
			|| map.get_object(i+1, j+1)==Obj_Tree
			)) continue;
			size_trees += 6;
		}
	}
	vertex_trees = new double[3*size_trees];
	normal_trees = new double[3*size_trees];

	NormalFan** fanNormals   = normalFanArray_Trees(w-1, h-1, map);
	Vector3D** mainNormals   = mainNormalArray(w, h, fanNormals);
	Vector3D** medianNormals = medianNoramlArray(w-1, h-1, fanNormals);

	size_t counter = 0;
	for(size_t i=0; i<w-1; i++)
	{
		for(size_t j=0; j<h-1; j++)
		{
			if(!(map.get_object(i, j)==Obj_Tree
			|| map.get_object(i+1, j)==Obj_Tree
			|| map.get_object(i, j+1)==Obj_Tree
			|| map.get_object(i+1, j+1)==Obj_Tree
			)) continue;
			double x0 = ((double)i-(double)0.5*w)*scale;
			double x1 = ((double)(i+1)-(double)0.5*w)*scale;
			double y0 = ((double)j-(double)0.5*h)*scale;
			double y1 = ((double)(j+1)-(double)0.5*h)*scale;
			double h00 = map.get_treeheight(i, j) * scale;
			double h01 = map.get_treeheight(i, j+1) * scale;
			double h10 = map.get_treeheight(i+1, j) * scale;
			double h11 = map.get_treeheight(i+1, j+1) * scale;
			double x_m = (x0 + x1) / 2;
			double y_m = (y0 + y1) / 2;
			double z_m = (h00 + h01 + h11 + h11) / 4;
			Point3D p00(x0, y0, h00);
			Point3D p01(x0, y1, h01);
			Point3D p10(x1, y0, h10);
			Point3D p11(x1, y1, h11);
			Point3D p_m(x_m, y_m, z_m);

			addPoint(vertex_trees, normal_trees, counter, p_m, medianNormals[i][j]);
			counter++;
			addPoint(vertex_trees, normal_trees, counter, p00, mainNormals[i][j]);
			counter++;
			addPoint(vertex_trees, normal_trees, counter, p01, mainNormals[i][j+1]);
			counter++;
			addPoint(vertex_trees, normal_trees, counter, p11, mainNormals[i+1][j+1]);
			counter++;
			addPoint(vertex_trees, normal_trees, counter, p10, mainNormals[i+1][j]);
			counter++;
			addPoint(vertex_trees, normal_trees, counter, p00, mainNormals[i][j]);
			counter++;

		}
	}

}