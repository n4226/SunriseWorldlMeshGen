#pragma once

#include "Sunrise.h"
#include "../underlingSystems/osmFetching/OsmFetcher.h"



class icreator
{
public:
	
	virtual void createInto(BinaryMeshSeirilizer::Mesh& mesh, osm::osm& osm, const Box& frame,int lod);

private:

};
