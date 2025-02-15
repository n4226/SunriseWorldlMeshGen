#include "mgpch.h"
#include "RoadCreator.h"

#include "../../underlingSystems/osmFetching/OsmAttributeFetcher.h"

#define LANEWIDTH 1

void RoadCreator::createInto(Mesh& mesh, osm::osm& osm, const Box& frame, int lod, ChunkGenerationStatistics& stats)
{
	// only lods < 1 have roads as 3d structures
	if (lod > 0) return;

	mesh.indicies.push_back({});
	mesh.attributes->subMeshMats.push_back(3);

	//TODO: somehow prevent each creator from ahving to loop through all the elements in the osm data after eachother by combining them into one 
	// TODO remove duplicate pointes if they fall on a streight 2d line after being clipped to the chunk -> this could be for objecy such as buildings or more commanly to grounds of different surfaces eg ocean and land
	//
	for (osm::element& element : osm.elements) {
		if (element.type == osm::type::way && element.tags.count("highway") > 0) {
			
			addRoad(mesh, osm, element, frame, lod);

		}
	}

}

void RoadCreator::addRoad(Mesh& mesh, osm::osm& osm, osm::element& road, const Box& frame, int lod)
{

	constexpr double radius = math::dEarthRad;
	const glm::dvec3 center_geo = math::LlatoGeo(glm::dvec3(frame.getCenter(), 0), {}, radius);

	auto startVertOfset = mesh.verts.size();

	auto nodes = osm.nodesIn(road);



	//TODO: make this acurate
	double lanes = OsmAttributeFetcher::tryGetValue<double>(road,"lanes",2);
	double width = OsmAttributeFetcher::tryGetValue<double>(road,"width",lanes*LANEWIDTH);


	// first node IS duplicated - AT INDEX 0 AND INDEX SIZE - 1
	std::vector<glm::dvec2> _basePath(nodes.size());

	std::transform(nodes.begin(), nodes.end(), _basePath.begin(), [&, road, _basePath,this](osm::element* element) {
		if (element->type != osm::type::node) { 
			auto msg = std::string("tried to get lat lon of non node at: ") + __FILE__ + std::string(" ") + std::string(std::to_string(__LINE__));
			printf(msg.c_str());
			throw std::runtime_error(msg.c_str());
		}
		auto posLatLon = glm::dvec2(*element->lat, *element->lon);

		//posLatLon.x = glm::max(posLatLon.x, frame.start.x);
		//posLatLon.y = glm::max(posLatLon.y, frame.start.y);
		//posLatLon.x = glm::min(posLatLon.x, frame.getEnd().x);
		//posLatLon.y = glm::min(posLatLon.y, frame.getEnd().y);

		return posLatLon;
	});

	//auto chunkPoly = frame.polygon();
	//std::reverse(chunkPoly.begin(), chunkPoly.end());
	//std::vector<std::vector<glm::dvec2>> clippedPath = mesh::intersectionOf(_basePath,chunkPoly); 

	//SR_ASSERT(clippedPath.size() > 0);

	std::vector<glm::dvec2> basePath = _basePath;//clippedPath[0];

	int vertIndex = 0;

	//TODO: detect if closed path and adjust mesh acordingly
	for (size_t i = 0; i < basePath.size(); i++)
	{

		auto p1 = math::LlatoGeo(glm::dvec3(basePath[i + 0], 0), glm::dvec3(0), radius) - center_geo;
		//auto p2 = math::LlatoGeo(glm::dvec3(basePath[i + 1], 0), glm::dvec3(0), radius) - center_geo;

		auto forward = glm::dvec3(0);

		if (i < basePath.size() - 1) {
			forward += math::LlatoGeo(glm::dvec3(basePath[i + 1], 0), glm::dvec3(0), radius) - math::LlatoGeo(glm::dvec3(basePath[i + 0], 0), glm::dvec3(0), radius);
		}
		if (i > 0) {
			forward += math::LlatoGeo(glm::dvec3(basePath[i + 0], 0), glm::dvec3(0), radius) - math::LlatoGeo(glm::dvec3(basePath[i - 1], 0), glm::dvec3(0), radius);
		}

		forward = glm::normalize(forward);

		auto normalUP = glm::normalize(center_geo);
		auto left = glm::cross(forward,normalUP);

		auto v1 = p1 + left * width * 0.5;
		auto v2 = p1 - left * width * 0.5;

		mesh.verts.push_back(glm::vec3(v1));
		mesh.verts.push_back(glm::vec3(v2));


		mesh.normals.push_back(glm::vec3(normalUP));
		mesh.normals.push_back(glm::vec3(normalUP));

		mesh.tangents.push_back(glm::vec3(left));
		mesh.tangents.push_back(glm::vec3(left));
		mesh.bitangents.push_back(glm::vec3(forward));
		mesh.bitangents.push_back(glm::vec3(forward));

		// scalled uvs

		auto uvProgress = glm::distance(p1, math::LlatoGeo(glm::dvec3(basePath[0], 0), glm::dvec3(0), radius) - center_geo);
		glm::vec2 scale(2);

		mesh.uvs.push_back(glm::vec2(0, uvProgress) * scale);
		mesh.uvs.push_back(glm::vec2(width, uvProgress) * scale);


		if (i < basePath.size() - 1) {
			mesh.indicies[mesh.indicies.size() - 1].push_back(vertIndex + 0 + startVertOfset);
			mesh.indicies[mesh.indicies.size() - 1].push_back(vertIndex + 2 + startVertOfset);
			mesh.indicies[mesh.indicies.size() - 1].push_back(vertIndex + 1 + startVertOfset);

			mesh.indicies[mesh.indicies.size() - 1].push_back(vertIndex + 1 + startVertOfset);
			mesh.indicies[mesh.indicies.size() - 1].push_back(vertIndex + 2 + startVertOfset);
			mesh.indicies[mesh.indicies.size() - 1].push_back(vertIndex + 3 + startVertOfset);
		}
		vertIndex += 2;
	}


}
