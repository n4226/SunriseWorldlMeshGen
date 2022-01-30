#pragma once

#include "mgpch.h"
#include "../creator.h"

class groundCreator: public icreator
{
public:

	using icreator::icreator;

	//void createInto(Mesh& mesh, osm::osm& osm, const Box& frame, int lod, ChunkGenerationStatistics& stats) override;

	virtual math::mesh::ShadedMultiPolygon2D polygonsFromOSM();
	virtual math::mesh::ShadedMultiPolygon2D polygonsFromElement(const osm::element& elm) override;



	static void createSubdividedQuadChunkMesh(Mesh & mesh, const Box& frame);

	void drawMultPolygonInChunk(std::vector<std::vector<glm::dvec2>>& polygon, Mesh& mesh, const Box& frame, ChunkGenerationStatistics* stats,bool geoCenter = true);

private:
	std::vector<std::vector<glm::dvec2>>* createLandPolygonChunkMesh(Mesh& mesh, const Box& frame, bool draw, ChunkGenerationStatistics& stats);
};

