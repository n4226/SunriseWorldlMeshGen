#pragma once
#include "mgpch.h"

#include <shapefil.h>
#include <array>
#include <vector>
#include "Sunrise/Sunrise/math/mesh/Triangulation.h"

class ShapeFileSystem
{
public:

	ShapeFileSystem();
	~ShapeFileSystem();

	void getCoastlineInChunk(Box chunk);


	std::vector<mesh::TriangulatedMesh*> polygons;
	std::vector<Box> polygonBounds;

private:

	int numberOfShapes;
	int fileShapeType;
	std::array<double, 4> padfMinBound;
	std::array<double, 4> padfMaxBound;

	std::vector<SHPObject*> shapes;


	SHPHandle shapeFile;
};

extern ShapeFileSystem* shapeFileSystem;