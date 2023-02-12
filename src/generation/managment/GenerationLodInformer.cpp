#include "mgpch.h"
#include "GenerationLodInformer.h"
#include "../../underlingSystems/osmFetching/OsmAttributeFetcher.h"

// constants:
// at or above this height all are visible 
// at or above half - more likely to be visible
constexpr double buildingCullHeightCutof = 20;//10;

constexpr double buildingProbLOD1 = 0.4;//0.60;
constexpr double buildingProbLOD2 = 0.20;
constexpr double buildingProbLOD3 = 0.10; //should be applied to div 10 chunks



bool GenerationLodInformer::drawBuilding(Box chunk, int lod, const osm::element& building)
{

	auto height = OsmAttributeFetcher::buildingHeight(building);

	auto cullHeight = buildingCullHeightCutof;

	if (lod >= 2) {
		cullHeight *= 5;
	}

	if (height > cullHeight) return true;
	
	static std::random_device rd;
	static std::default_random_engine eng(rd());
	static std::uniform_real_distribution<double> dist(0, 1);
	auto prob  = dist(eng);
	auto prob2 = dist(eng);

	// so that if above half of max height twice as likely to be visible
	if (height > (cullHeight / 2) && prob2 < 0.5) return true;

	switch (lod)
	{

	case 1:
		if (prob > buildingProbLOD1) return false;

		break;
	
	case 2:
		if (prob > buildingProbLOD2) return false;

		break;

	case 3:
		//if it's taller than normal limit give it lod1 chance of drawling
		if (height > buildingCullHeightCutof)
			if (prob > buildingProbLOD1) return false;
		if (prob > buildingProbLOD3) return false;

		break;
	default:
		break;
	}

	return true;
}
