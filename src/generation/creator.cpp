#include "mgpch.h"
#include "creator.h"


//void icreator::createInto(Mesh& mesh, osm::osm& osm, const Box& frame,int lod, ChunkGenerationStatistics& stats)
//{
//
//}

sunrise::math::mesh::Polygon2D icreator::polygonFromWay(const osm::element& way)
{
	auto nodes = chunk.osm->nodesIn(way);

	std::vector<glm::dvec2> basePath(nodes.size() - 1);

	std::transform(nodes.begin(), std::prev(nodes.end()), basePath.begin(), [&](osm::element* element) {
		auto posLatLon = glm::dvec2(*element->lat, *element->lon);
		return posLatLon;
		});

	if (math::mesh::clockwiseOriented(basePath)) {
		std::reverse(basePath.begin(), basePath.end());
	}

	return { basePath };
}
