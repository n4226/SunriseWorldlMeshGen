#pragma once

#include "Sunrise.h"
#include "../underlingSystems/osmFetching/OsmFetcher.h"
#include "GenerationStatistics.h"



/// <summary>
/// used to generate terrain
/// a new creater will be instantited for for every chunk so thread safety will not be a factor
/// </summary>
class icreator
{
public:
	
	/// depricated
	/// </summary>
	/// <param name="mesh"></param>
	/// <summary>
	/// <param name="osm"></param>
	/// <param name="frame"></param>
	/// <param name="lod"></param>
	/// <param name="stats"></param>
	//virtual void createInto(Mesh& mesh, osm::osm& osm, const Box& frame,int lod, ChunkGenerationStatistics& stats);

	struct ChunkData {
		Box chunk;
		int lod;
		const osm::osm* osm;
		//const osm::osm& osm;
		ChunkGenerationStatistics& stats;
	};

	icreator(ChunkData chunk)
		: chunk(chunk)
	{}


	/// <summary>
	/// called twice - first time with forMesh set to false right before polygon gens and second before mesh gen with the var set true
	/// </summary>
	/// <param name="forMesh"></param>
	virtual void initInChunk(bool forMesh, Mesh& mesh) {};

	// either func can be used to generate ground polygons

	virtual math::mesh::ShadedMultiPolygon2D polygonsFromElement(const osm::element& elm) { return {}; };
	virtual math::mesh::ShadedMultiPolygon2D polygonsFromOSM() { return {}; };

	// either func can be used to generate mesh

	virtual void meshFromElement	(Mesh& mesh,const osm::element& elm) {};
	virtual void meshFromOSM		(Mesh& mesh) {};

protected:

	/// <summary>
	/// returns an open (last point not equal to first point in array) counter clockwise oriented polygon
	/// </summary>
	/// <param name="way"></param>
	/// <returns></returns>
	math::mesh::Polygon2D polygonFromWay(const osm::element& way);


	ChunkData chunk;
};
