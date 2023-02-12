#include "mgpch.h"
#include "AirportCreator.h"
#include "../../underlingSystems/osmFetching/OsmAttributeFetcher.h"


void AirportCreator::initInChunk(bool forMesh, Mesh& mesh)
{
	
}

sunrise::math::mesh::ShadedMultiPolygon2D AirportCreator::polygonsFromElement(const osm::element& elm)
{
	return {};
}

sunrise::math::mesh::ShadedMultiPolygon2D AirportCreator::polygonsFromOSM()
{
	constexpr double radius = math::dEarthRad;
	mesh::ShadedHPolygon2d airpotPoly{};
	mesh::MultiPolygon2D disruptions{};

	mesh::ShadedMultiPolygon2D outputPolygon{};

	auto& frame = chunk.chunk;

	//width in meters
	auto nonLLAFrameWidth = math::llaDistance(frame.start, glm::vec2(frame.start.x, frame.start.y + frame.size.y));
	auto nonLLAFrameHeight = math::llaDistance(frame.start, glm::vec2(frame.start.x + frame.size.x, frame.start.y));

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

		//make a cutout for runways
		if (elm.type == osm::type::way && osm::isAny(elm, {
			{"aeroway", "runway"}
			})) {

			//default to 30 feet
			double width = 0.0007;//OsmAttributeFetcher::tryGetValue(elm, "width", 40.0 * 0.304);

			//get points -- this will not be closed but a line
			auto line = polygonFromWay(elm);

			//for each point on line generate generate one point to the left (normal vec cross tangent) and one to the right each half the width away
			//then make a polygon out of the points

			std::vector<glm::dvec2> polygon(line.size()*2);
			if (line.size() > 1) {
				for (int i = 0; i < line.size(); i++) {
					glm::dvec2 tangent;

					if (i < line.size() - 1) {
						tangent = line[i + 1] - line[i];
					}
					else {
						tangent = line[i] - line[i - 1];
					}

					glm::dvec2 normal = glm::normalize(glm::dvec2(-tangent.y, tangent.x));

					//normal and tangent are in normalize lla coordinates. they need to be scalled to meters through unevan acces scallings
					//the nonLLAFrameWidth gives X scalling from lla to meters 
					//the nonLLAFrameHeight gives Y scalling from lla to meters

					//use a matrix to transform the normal and tangent to meters
					//glm::dmat2x2 scallingMatrix = glm::dmat2x2(nonLLAFrameWidth, 0, 0, nonLLAFrameHeight);
					//normal = scallingMatrix * normal;
					//tangent = scallingMatrix * tangent;

					polygon[i] = line[i] + normal * width / 2.0;
					polygon[line.size() * 2 - i - 1] = line[i] - normal * width / 2.0;
				}
				if (!polygon.size() == 0) {
					disruptions.push_back({ polygon });
					outputPolygon.push_back({ {polygon}, StaticMaterialTable::entries.at("asphalt2") });
				}
			}
		}

	}
	//disruptions = mesh::bunionAll(disruptions);
	if (airpotPoly.polygon.size() > 0) {
		airpotPoly.polygon = mesh::bDifference({ airpotPoly.polygon }, disruptions)[0];
	}

	outputPolygon.push_back(airpotPoly);
	return outputPolygon;
}



void AirportCreator::meshFromElement(Mesh& mesh, const osm::element& elm)
{

}

void AirportCreator::meshFromOSM(Mesh& mesh)
{
	
}
