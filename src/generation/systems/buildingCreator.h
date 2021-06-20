#pragma once

#include "mgpch.h"
#include "../creator.h"

class buildingCreator: public icreator
{
public:
	void createInto(Mesh& mesh, osm::osm& osm, const Box& frame, int lod) override;

private:

	std::map<osm::element*, Box*> buldingAABBS;

	void addBuilding(Mesh& mesh, osm::osm& osm, osm::element& building, const Box& frame, int lod);

};

