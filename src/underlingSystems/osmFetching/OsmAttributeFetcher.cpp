#include "mgpch.h"
#include "OsmAttributeFetcher.h"

double OsmAttributeFetcher::buildingHeight(const osm::element& building)
{
	double height;

	if (building.tags.count("height")) {
		try {
			height = std::stod(building.tags.at("height"));
		}
		catch (...) { height = 4; }
	}
	else height = 4;


	return height;

}
