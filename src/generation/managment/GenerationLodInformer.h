#pragma once

#include "mgpch.h"
#include "../../underlingSystems/osmFetching/osm.h"

class GenerationLodInformer
{
public:

	static bool drawBuilding(Box chunk, int lod, const osm::element& building);

};

