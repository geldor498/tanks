#include "MapGenerator.h"
#include "Vector3D.h"

class GLArrays
{
private:
	size_t size_mountains;
	double* vertex_mountains;
	double* normal_mountains;
	size_t size_trees;
	double* vertex_trees;
	double* normal_trees;
public:
	GLArrays(CGameMap& map, double scale);
	size_t get_SizeMountains();
	double* get_VertexMountains();
	double* get_NormalMountains();
	size_t get_SizeTrees();
	double* get_VertexTrees();
	double* get_NormalTrees();
private:
	void createMountainsArrays(CGameMap& map, double scale);
	void createTreesArrays(CGameMap& map, double scale);
};