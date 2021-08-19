#include "mgpch.h"
#include "ShapeFileSystem.h"

#include <iostream>

#define Sunrise

// ha fun artifact of figuring out the name of the engine -->

#define SR_PROFILNG
#define SRE_PROFILNG

#define SR_GFX
#define SRE_GFX

#define SR_API

SR_API


namespace Sunrise {
	namespace Graphics {

	}
}


// <--

ShapeFileSystem::ShapeFileSystem()
{
	auto path = MeshGenConfig::get().coastlineDir + "land_polygons.shp";

	shapeFile = SHPOpen(path.c_str(),"rb");

	SHPGetInfo(shapeFile, &numberOfShapes, &fileShapeType, padfMinBound.data(), padfMaxBound.data());

	shapes.resize(numberOfShapes);
	polygons.resize(numberOfShapes);
	polygonBounds.resize(numberOfShapes);

	for (size_t i = 0; i < numberOfShapes; i++)
	{
		auto shape = SHPReadObject(shapeFile, i);
		shapes[i] = shape;

		auto mesh = new mesh::TriangulatedMesh();

		mesh->verts.resize(shape->nVertices);
		for (size_t i = 0; i < shape->nVertices; i++)
		{
			mesh->verts[i] = glm::dvec2(shape->padfY[i], shape->padfX[i]);
		}
		polygons[i] = mesh;
		polygonBounds[i] = mesh::bounds(mesh->verts);
	}


	//std::cout << "finished loading shape file" << std::endl;
}

ShapeFileSystem::~ShapeFileSystem()
{
	SHPClose(shapeFile);
}

void ShapeFileSystem::getCoastlineInChunk(Box chunk)
{
	//TODO impliment
	SR_ERROR("{} function not implimented", __FUNCTION__);
} 

ShapeFileSystem* shapeFileSystem = nullptr;