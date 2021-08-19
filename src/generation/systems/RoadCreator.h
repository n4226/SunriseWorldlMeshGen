#pragma once

#include "mgpch.h"
#include "../creator.h"

class RoadCreator : public icreator
{
public:
	void createInto(Mesh& mesh, osm::osm& osm, const Box& frame, int lod, ChunkGenerationStatistics& stats) override;

private:

	void addRoad(Mesh& mesh, osm::osm& osm, osm::element& road, const Box& frame, int lod);


};