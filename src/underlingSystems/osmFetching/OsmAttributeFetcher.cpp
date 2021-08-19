#include "mgpch.h"
#include "OsmAttributeFetcher.h"

double OsmAttributeFetcher::buildingHeight(const osm::element& building)
{
	return OsmAttributeFetcher::tryGetValue(building, "height", 4.0);
}
