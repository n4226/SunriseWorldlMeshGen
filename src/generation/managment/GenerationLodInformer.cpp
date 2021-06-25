#include "mgpch.h"
#include "GenerationLodInformer.h"
#include "../../underlingSystems/osmFetching/OsmAttributeFetcher.h"

// constants:
constexpr double buildingCullHeightCutof = 10;

constexpr double buildingProbLOD1 = 0.80;
constexpr double buildingProbLOD2 = 0.60;
constexpr double buildingProbLOD3 = 0.50;



bool GenerationLodInformer::drawBuilding(Box chunk, int lod, const osm::element& building)
{

	auto height = OsmAttributeFetcher::buildingHeight(building);

	if (height > buildingCullHeightCutof) return true;
	
	static std::random_device rd;
	static std::default_random_engine eng(rd());
	static std::uniform_real_distribution<double> dist(0, 1);
	auto prob = dist(eng);

	switch (lod)
	{

	case 1:
		if (prob > buildingProbLOD1) return false;

		break;
	
	case 2:
		if (prob > buildingProbLOD2) return false;

		break;
	default:
		break;
	}

	return true;
}
