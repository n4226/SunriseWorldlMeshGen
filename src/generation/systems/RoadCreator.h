#pragma once

#include "mgpch.h"
#include "../creator.h"

class RoadCreator : public icreator
{
public:
	using icreator::icreator;
	//void createInto(Mesh& mesh, osm::osm& osm, const Box& frame, int lod, ChunkGenerationStatistics& stats) override;

	virtual void initInChunk(bool forMesh, Mesh& mesh);

	virtual void meshFromElement(Mesh& mesh, const osm::element& elm);

private:

	void addRoad(Mesh& mesh,const osm::osm& osm,const osm::element& road, const Box& frame, int lod);


};