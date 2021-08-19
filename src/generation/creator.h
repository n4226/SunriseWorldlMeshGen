#pragma once

#include "Sunrise.h"
#include "../underlingSystems/osmFetching/OsmFetcher.h"
#include "GenerationStatistics.h"



class icreator
{
public:
	
	virtual void createInto(Mesh& mesh, osm::osm& osm, const Box& frame,int lod, ChunkGenerationStatistics& stats);

private:

};
