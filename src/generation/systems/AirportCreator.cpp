#include "mgpch.h"
#include "AirportCreator.h"


void AirportCreator::initInChunk(bool forMesh, Mesh& mesh)
{
	
}

sunrise::math::mesh::ShadedMultiPolygon2D AirportCreator::polygonsFromElement(const osm::element& elm)
{
	return {};
}

sunrise::math::mesh::ShadedMultiPolygon2D AirportCreator::polygonsFromOSM()
{
	mesh::ShadedHPolygon2d airpotPoly{};
	mesh::MultiPolygon2D disruptions{};

	for (auto& elm : chunk.osm->elements) {

		if (elm.type == osm::type::way && elm.tags.count("aeroway") > 0 && elm.tags.at("aeroway") == "aerodrome") {
			const glm::dvec3 center_geo = math::LlatoGeo(glm::dvec3(chunk.chunk.getCenter(), 0), {}, radius);

			auto poly = polygonFromWay(elm);

			mesh::ShadedHPolygon2d pol = { {poly}, StaticMaterialTable::entries.at("asphalt1") };

			airpotPoly = pol;
		}

		if (elm.type == osm::type::way && osm::isAny(elm, {
			{"landuse","grass"},
			{"natural", "*"},
			//{ "amenity", "parking"},
			}) && elm.nodes.size() > 3) {

			//todo: have to check if way is closed?
			auto poly = polygonFromWay(elm);
			if (!poly.size() == 0)
				disruptions.push_back({ poly });
			//SR_CORE_INFO("making poly with size: {}", poly.size());
		}

	}
	//disruptions = mesh::bunionAll(disruptions);
	if (airpotPoly.polygon.size() > 0) {
		airpotPoly.polygon = mesh::bDifference({ airpotPoly.polygon }, disruptions)[0];
	}

	return { airpotPoly };
}

void AirportCreator::meshFromElement(Mesh& mesh, const osm::element& elm)
{
}

void AirportCreator::meshFromOSM(Mesh& mesh)
{
	
}
