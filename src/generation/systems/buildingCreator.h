#pragma once

#include "mgpch.h"
#include "../creator.h"

class buildingCreator: public icreator
{
public:

	using icreator::icreator;

	void initInChunk(bool forMesh,Mesh& mesh) override;

	//void createInto(Mesh& mesh, osm::osm& osm, const Box& frame, int lod, ChunkGenerationStatistics& stats) override;

	virtual void meshFromElement(Mesh& mesh, const osm::element& elm) override;

private:

	std::map<osm::element*, Box*> buldingAABBS;

	void addBuilding(Mesh& mesh,const osm::osm& osm,const osm::element& building, const Box& frame, int lod, ChunkGenerationStatistics& stats);

};

