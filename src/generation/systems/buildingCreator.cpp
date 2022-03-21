#include "mgpch.h"
#include "buildingCreator.h"
#include "../../underlingSystems/osmFetching/OsmAttributeFetcher.h"

#include "../managment/GenerationLodInformer.h"

//void buildingCreator::createInto(Mesh& mesh, osm::osm& osm, const Box& frame, int lod, ChunkGenerationStatistics& stats)
//{
//	mesh.indicies.push_back({});
//	mesh.attributes->subMeshMats.push_back(2);
//	
//	auto startVerts = mesh.verts.size();
//
//	//TODO: somehow prevent each creator from ahving to loop through all the elements in the osm data after eachother by combining them into one 
//	// TODO remove duplicate pointes if they fall on a streight 2d line after being clipped to the chunk -> this could be for objecy such as buildings or more commanly to grounds of different surfaces eg ocean and land
//	//
//	for (osm::element& element : osm.elements) {
//		if (element.type == osm::type::way && element.tags.count("building") > 0) {
//			auto nodes = osm.nodesIn(element);
//
//			// first node IS duplicated - AT INDEX 0 AND INDEX SIZE - 1
//			std::vector<glm::dvec2> basePath(nodes.size());
//
//			std::transform(nodes.begin(), nodes.end(), basePath.begin(), [&](osm::element* element) {
//				return glm::dvec2(*element->lat, *element->lon);
//			});
//			auto bounds = new Box();
//			*bounds = mesh::bounds(basePath);
//			buldingAABBS.emplace(&element, bounds);
//		}
//	}
//
//
//
//	//TODO: somehow prevent each creator from ahving to loop through all the elements in the osm data after eachother by combining them into one loop
//	for (osm::element& element : osm.elements) {
//		
//		// TODO remove duplicate pointes if they fall on a streight 2d line after being clipped to the chunk -> this could be for objecy such as buildings or more commanly to grounds of different surfaces eg ocean and land
//
//
//
//		if (element.type == osm::type::way && element.tags.count("building") > 0) {
//			if (GenerationLodInformer::drawBuilding(frame,lod,element))
//				addBuilding(mesh, osm, element, frame,lod,stats);
//		}
//
//	}
//
//	stats.logVerts(mesh.verts.size() - startVerts, ChunkGenerationStatistics::VertUse::building);
//}

void buildingCreator::initInChunk(bool forMesh, Mesh& mesh)
{
	if (forMesh) {
		mesh.indicies.push_back({});
		mesh.attributes->subMeshMats.push_back(2);
	}
}

void buildingCreator::meshFromElement(Mesh& mesh, const osm::element& element)
{
	if (element.type == osm::type::way && element.tags.count("building") > 0) {
		if (GenerationLodInformer::drawBuilding(chunk.chunk,chunk.lod,element))
			addBuilding(mesh, *chunk.osm, element, chunk.chunk,chunk.lod,chunk.stats);
	}
}

void buildingCreator::addBuilding(Mesh& mesh,const osm::osm& osm,const osm::element& building, const Box& frame, int lod, ChunkGenerationStatistics& stats)
{

	constexpr double radius = math::dEarthRad;
	const glm::dvec3 center_geo = math::LlatoGeo(glm::dvec3(frame.getCenter(), 0), {}, radius);
	double height = OsmAttributeFetcher::buildingHeight(building);

	auto startVertOfset = mesh.verts.size();

	auto nodes = osm.nodesIn(building);

	// first node IS duplicated - AT INDEX 0 AND INDEX SIZE - 1
	std::vector<glm::dvec2> basePath(nodes.size());

	std::transform(nodes.begin(), nodes.end(), basePath.begin(), [&](osm::element* element) {
		auto posLatLon = glm::dvec2(*element->lat, *element->lon);

		/*posLatLon.x = glm::max(posLatLon.x, frame.start.x);
		posLatLon.y = glm::max(posLatLon.y, frame.start.y);
		posLatLon.x = glm::min(posLatLon.x, frame.getEnd().x);
		posLatLon.y = glm::min(posLatLon.y, frame.getEnd().y);*/

		return posLatLon;
	});

	//limit footprint to chunk boundary
	// [0][0] is ok becasue interseciton of two simple non holed polygons is garented to return same thing
	basePath = mesh::binterseciton({{basePath}},{{chunk.chunk.polygon()}})[0][0];

	if (basePath.size() == 0) {
		//SR_DEBUGBREAK();
		return; // the building is not in the chunk
	}
	auto bounds = mesh::bounds(basePath);

	glm::dvec2 min = bounds.start;
	glm::dvec2 max = bounds.getEnd();


	if (!frame.contains(min) && !frame.contains(max))
		return;



	//for (osm::element& element : osm.elements) {
	//	if (element.type == osm::type::way && element.tags.count("building") > 0) {
	//		
	//		if (buldingAABBS.count(&element) > 0) {
	//			auto other_bounds = buldingAABBS.at(&element);

	//			if (lod > 0 && (bounds.containsAny(other_bounds->polygon()) || other_bounds->containsAny(bounds.polygon()))) {
	//				return;
	//			}
	//		}
	//	}
	//}



	// roof 
	



	// creates a po.ygon with one set of points, the base points dropping the last point since it is just the first point
	std::vector<std::vector<glm::dvec2>> roofPoly = { std::vector<glm::dvec2>(basePath.begin(),std::prev(basePath.end())) };

	auto [roofMesh, isCLockWise] = mesh::triangulate(roofPoly);

	
	//TODO --- deal with duplicate verticies her this is important

	startVertOfset = mesh.verts.size();

	for (size_t i = 0; i < roofMesh.verts.size(); i++)
	{
		auto posLatLon = roofMesh.verts[i];
		auto posLLA = glm::dvec3(posLatLon.x, posLatLon.y, height);
		auto pos1 = math::LlatoGeo(posLLA, glm::dvec3(0), radius) - center_geo;
		mesh.verts.push_back(pos1);

		auto geo_unCentered = math::LlatoGeo(posLLA, {}, radius);

		auto normal = static_cast<glm::vec3>(glm::normalize(geo_unCentered));

		mesh.normals.push_back(normal);

		mesh.tangents.push_back(glm::vec3(0));
		mesh.bitangents.push_back(glm::vec3(0));

		auto uv = glm::vec2(((posLatLon - min) / (max - min)) * math::llaDistance(min, max));
		mesh.uvs.push_back(uv);
	}

	for (size_t i = 0; i < roofMesh.indicies[0].size(); i++)
	{
		mesh.indicies[mesh.indicies.size() - 1].push_back(roofMesh.indicies[0][i] + startVertOfset);
	}


	// walls 
	for (size_t i = 0; i < (basePath.size() - 1); i++)
	{
		auto localOff = static_cast<uint32_t>(mesh.verts.size());

		auto pos1 = math::LlatoGeo(glm::dvec3(basePath[i]    , 0), glm::dvec3(0), radius) - center_geo;
		auto pos2 = math::LlatoGeo(glm::dvec3(basePath[i + 1], 0), glm::dvec3(0), radius) - center_geo;


		auto pos3 = math::LlatoGeo(glm::dvec3(basePath[i], height), glm::dvec3(0), radius) - center_geo;
		auto pos4 = math::LlatoGeo(glm::dvec3(basePath[i + 1], height), glm::dvec3(0), radius) - center_geo;



		auto normal = glm::normalize(glm::cross(pos2 - pos3, pos2 - pos1));

		if (isCLockWise) {
			normal = -normal;
		}

		mesh.verts.push_back(glm::vec3(pos1));
		mesh.verts.push_back(glm::vec3(pos2));
		mesh.verts.push_back(glm::vec3(pos3));
		mesh.verts.push_back(glm::vec3(pos4));

		//stats.logVerts(4, ChunkGenerationStatistics::VertUse::building);

		mesh.normals.push_back(glm::vec3(normal));
		mesh.normals.push_back(glm::vec3(normal));
		mesh.normals.push_back(glm::vec3(normal));
		mesh.normals.push_back(glm::vec3(normal));

		mesh.tangents.push_back(glm::vec3(0));
		mesh.tangents.push_back(glm::vec3(0));
		mesh.tangents.push_back(glm::vec3(0));
		mesh.tangents.push_back(glm::vec3(0));
		mesh.bitangents.push_back(glm::vec3(0));
		mesh.bitangents.push_back(glm::vec3(0));
		mesh.bitangents.push_back(glm::vec3(0));
		mesh.bitangents.push_back(glm::vec3(0));

		// scalled uvs

		mesh.uvs.push_back(glm::vec2(0, 0));
		mesh.uvs.push_back(glm::vec2(glm::distance(pos2, pos1), 0));
		mesh.uvs.push_back(glm::vec2(0, height));
		mesh.uvs.push_back(glm::vec2(glm::distance(pos2, pos1), height));


		//mesh.uvs.push_back(glm::vec2(0,0));
		//mesh.uvs.push_back(glm::vec2(1,0));
		//mesh.uvs.push_back(glm::vec2(0,1));
		//mesh.uvs.push_back(glm::vec2(1,1));

		//TODO: later add wall inside detection

		if (!isCLockWise) {
			// wall side 1
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 0);
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 1);
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 2);
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 1);
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 3);
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 2);
		}
		else {
			// wall side 2
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 2);
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 1);
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 0);
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 2);
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 3);
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 1);
		}
	}

}
