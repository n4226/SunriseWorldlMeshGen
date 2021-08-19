#pragma once

#include "mgpch.h"
#include "../creator.h"

class groundCreator: public icreator
{
public:
	void createInto(Mesh& mesh, osm::osm& osm, const Box& frame, int lod, ChunkGenerationStatistics& stats) override;



	void createSubdividedQuadChunkMesh(Mesh & mesh, const Box& frame);

	void drawMultPolygonInChunk(std::vector<std::vector<glm::dvec2>>& polygon, Mesh& mesh, const Box& frame, ChunkGenerationStatistics* stats,bool geoCenter = true);

private:
	std::vector<std::vector<glm::dvec2>>* createLandPolygonChunkMesh(Mesh& mesh, const Box& frame, bool draw, ChunkGenerationStatistics& stats);
};

