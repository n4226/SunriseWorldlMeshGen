#pragma once

#include "mgpch.h"

#include "creator.h"
#include "../underlingSystems/osmFetching/OsmFetcher.h"

#include "GenerationStatistics.h"


class GenerationSystem
{
public:
	GenerationSystem(const std::vector<Box>& chunks);

	void generate();

	void debugChunk(size_t index, int lod);

	static std::vector<Box> genreateChunksAround(glm::dvec2 desired,int divided, glm::ivec2 formation);
	static Box actualChunk(glm::dvec2 desired,int divided = 12);
private:

	void generateChunk(Box chunk,size_t lod, Mesh& mesh, ChunkGenerationStatistics& stats,const osm::osm& osmData);

	static std::vector<icreator*> createCreators(icreator::ChunkData chunkData);

	OsmFetcher osmFetcher;

	std::vector<Box> chunks;
	//std::vector<icreator*> creators;

	const std::string outputDir;
	const std::string attrOutputDir;
};

