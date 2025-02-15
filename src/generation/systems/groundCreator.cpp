#include "mgpch.h"

#include "groundCreator.h"

#include "../../underlingSystems/ShapeFileSystem.h"


constexpr double radius = math::dEarthRad;

bool isAny(osm::element& element, std::vector<std::array<std::string, 2>>&& wantedKeyValues) {
    
    for (size_t i = 0; i < wantedKeyValues.size(); i++)
    {
        auto key = wantedKeyValues[i][0];
        auto value = wantedKeyValues[i][1];

        if (element.tags.count(key) > 0) {
            if (value != "*") {
                for (auto& kv : element.tags) {
                    if (kv.first == key && kv.second == value) {
                        return true;
                    }
                }
            }
            else
            {
                return true;
            }
        }
    }

    return false;
}


void groundCreator::createInto(Mesh& mesh, osm::osm& osm, const Box& frame, int lod, ChunkGenerationStatistics& stats)
{


    // last param is weather to draw the land polygon
    //TODO: for now assuming only one but this is not correct
    auto lands = createLandPolygonChunkMesh(mesh, frame, true, stats);

    // if there was no land than make a full flat chunk to eventually become oscean
    //if (lands->size() == 0)
       // createSubdividedQuadChunkMesh(mesh, frame);


    // embedded surfaces

    // airports

    //{
    //    bool airport = false;
    //    std::vector<std::vector<glm::dvec2>> airportPolygone;
    //    for (osm::element& element : osm.elements) {
    //        if (element.type == osm::type::way && element.tags.count("aeroway") > 0 && element.tags.at("aeroway") == "aerodrome") {

    //            auto nodes = osm.nodesIn(element);

    //            std::vector<glm::dvec2> basePath(nodes.size());

    //            std::transform(nodes.begin(), nodes.end(), basePath.begin(), [&](osm::element* element) {
    //                if (element->type != osm::type::node) {
    //                    auto msg = std::string("tried to get lat lon of non node at: ") + __FILE__ + std::string(" ") + std::string(std::to_string(__LINE__));
    //                    printf(msg.c_str());
    //                    throw std::runtime_error(msg.c_str());
    //                }
    //                auto posLatLon = glm::dvec2(*element->lat, *element->lon);

    //                posLatLon.x = glm::max(posLatLon.x, frame.start.x);
    //                posLatLon.y = glm::max(posLatLon.y, frame.start.y);
    //                posLatLon.x = glm::min(posLatLon.x, frame.getEnd().x);
    //                posLatLon.y = glm::min(posLatLon.y, frame.getEnd().y);

    //                return posLatLon;
    //                });
    //            airport = true;
    //            airportPolygone.insert(airportPolygone.begin(), basePath);
    //        }
    //        else if (element.type == osm::type::way && isAny(element, { 
    //            {"landuse","grass"},
    //            {"natural", "*"},
    //            //{ "amenity", "parking"},
    //            })) {
    //            auto nodes = osm.nodesIn(element);

    //            std::vector<glm::dvec2> basePath(nodes.size());

    //            std::transform(nodes.begin(), nodes.end(), basePath.begin(), [&](osm::element* element) {
    //                if (element->type != osm::type::node) {
    //                    auto msg = std::string("tried to get lat lon of non node at: ") + __FILE__ + std::string(" ") + std::string(std::to_string(__LINE__));
    //                    printf(msg.c_str());
    //                    throw std::runtime_error(msg.c_str());
    //                }
    //                auto posLatLon = glm::dvec2(*element->lat, *element->lon);

    //                posLatLon.x = glm::max(posLatLon.x, frame.start.x);
    //                posLatLon.y = glm::max(posLatLon.y, frame.start.y);
    //                posLatLon.x = glm::min(posLatLon.x, frame.getEnd().x);
    //                posLatLon.y = glm::min(posLatLon.y, frame.getEnd().y);

    //                return posLatLon;
    //                });

    //            airportPolygone.push_back(basePath);
    //        }

    //    }

    //    if (airportPolygone.size() > 0 && airport) {
    //        mesh.indicies.push_back({});
    //        mesh.attributes->subMeshMats.push_back(1);
    //        drawMultPolygonInChunk(airportPolygone, mesh, frame,stats);
    //    }
    //
    //}




    auto framePoints = frame.polygon();

    std::reverse(framePoints.begin(), framePoints.end());

    if (lands->size() == 0) {
        // falback
    }

    //mesh::Polygon2D landMass = std::accumulate(lands->begin(), lands->end(), mesh::Polygon2D(), [](mesh::Polygon2D a,mesh::Polygon2D b) {
    //    return mesh::unionOf(a, b)[0];
    //});


    // combines overlapping land masses
    mesh::HPolygon2D filteredLandMasses = *lands;

    //for (size_t i = 0; i < lands->size(); i++)
    //{
    //    bool added = false;
    //    for (size_t j = 0; j < filteredLandMasses.size(); j++)
    //    {
    //        if (mesh::overlap(filteredLandMasses[j], (*lands)[i])) {
    //            filteredLandMasses[j] = mesh::unionOf(filteredLandMasses[j], (*lands)[i])[0];
    //            added = true;
    //            break;
    //        }
    //    }

    //    if (!added)
    //        filteredLandMasses.push_back((*lands)[i]);
    //}

    std::vector<mesh::Polygon2D> oceanMass = {framePoints};
    

    for (size_t i = 0; i < filteredLandMasses.size(); i++)
    {
        oceanMass = { mesh::bunionSMDifference(oceanMass[0], filteredLandMasses[i])};
    }
    

    // remove when wanting lands and oceans to be different
    // so that the main frame is the only thing drawn
    //oceanMass.clear();

    //if (oceanMass.size() == 0) {
        //SR_ASSERT(oceanMass.size() < 199999);


        //oceanMass.insert(oceanMass.begin(), framePoints);
        //oceanMass.insert(oceanMass.begin(), frame.polygon());

        //oceanMass.erase(std::prev(oceanMass.end()));

        //oceanMass.resize(1);

        mesh.indicies.push_back({});
        mesh.attributes->subMeshMats.push_back(0);
        //std::vector<std::vector<glm::dvec2>> p = { (*lands)[i] };
        drawMultPolygonInChunk(oceanMass, mesh, frame, &stats);
    //}
}

/// <summary>
/// returns all land masses
/// </summary>
/// <param name="mesh"></param>
/// <param name="frame"></param>
/// <param name="draw"></param>
/// <param name="stats"></param>
/// <returns></returns>
std::vector<std::vector<glm::dvec2>>* groundCreator::createLandPolygonChunkMesh(Mesh& mesh, const Box& frame,bool draw, ChunkGenerationStatistics& stats)
{
    if (draw) {

        mesh.indicies.push_back({});
        mesh.attributes->subMeshMats.push_back(1);
    }


    auto landPolygonsTruncated = new std::vector<std::vector<glm::dvec2>>();

    // loop through all polygons in the shape file for land polygons
    for (size_t i = 0; i < shapeFileSystem->polygons.size(); i++)
    {

        const auto& bounds = shapeFileSystem->polygonBounds[i];//meshAlgs::bounds(poly.verts);
        auto boundPath = bounds.polygon();//meshAlgs::bounds(poly.verts);
        auto& poly = *shapeFileSystem->polygons[i];

        if (!frame.overlaps(bounds)) { //(!frame.containsAny(poly.verts) && !bounds.containsAny(frame.polygon())) {
            continue;
        // for me tomorrow - make box interseciton fucntion and if passes move to actuall polygon interseciton fuction
        }

        std::vector<glm::dvec2> polyPoints = { poly.verts.begin(),std::prev(poly.verts.end()) };

        if (poly.verts[0] == poly.verts[poly.verts.size() - 1]) {

        }



        // truncate the polygon to be bounded by the chunk edges


        //auto cutPolyVerts = meshAlgs::intersectionOf(polyPoints, boundPath);

        auto frameP = frame.polygon();

        std::reverse(frameP.begin(), frameP.end());


        std::vector<std::vector<glm::dvec2>> polyVerts = mesh::intersectionOf(frameP, polyPoints);

        if (polyVerts.size() == 0) continue;



        //landPolygonsTruncated->push_back(polyPoints);
        landPolygonsTruncated->push_back(polyVerts[0]);

        //auto roofMesh = meshAlgs::triangulate(*cutPolyVerts).first;


       // draw this truncated to bounds polygone if draw is true
       if (draw)
        drawMultPolygonInChunk(polyVerts,mesh,frame,&stats);
       
    }

    // return all the truncated polygons
    return landPolygonsTruncated;
}


/// <summary>
/// will draw in the last submesh so push this back before hand for new material
/// </summary>
/// <param name="polygon"></param>
/// <param name="mesh"></param>
/// <param name="frame"></param>
/// <param name="stats"></param>
void groundCreator::drawMultPolygonInChunk(std::vector<std::vector<glm::dvec2>>& polygon, Mesh& mesh, const Box& frame, ChunkGenerationStatistics* stats, bool geoCenter)
{
    SR_ASSERT(polygon.size() > 0);
    auto roofMesh = mesh::triangulate(polygon).first;
    //auto roofMesh = mesh::triangulateEarcut(polygon);

    glm::dvec3 center_geo;
    if (geoCenter)
        center_geo = math::LlatoGeo(glm::dvec3(frame.getCenter(), 0), {}, radius);
    else {
        center_geo = glm::dvec3(frame.getCenter(),0);
    }

    auto startVertOfset = mesh.verts.size();


    for (size_t i = 0; i < roofMesh->verts.size(); i++)
    {
        auto posLatLon = roofMesh->verts[i];

        auto posLLA = glm::dvec3(posLatLon.x, posLatLon.y, 0);
        glm::dvec3 pos1;
        if (geoCenter)
            pos1 = math::LlatoGeo(posLLA, glm::dvec3(0), radius) - center_geo;
        else
            pos1 = posLLA - center_geo;
            
        mesh.verts.push_back(pos1);
        if (stats)
            stats->logVerts(1, ChunkGenerationStatistics::VertUse::land);

        auto geo_unCentered = math::LlatoGeo(posLLA, {}, radius);

        auto normal = static_cast<glm::vec3>(glm::normalize(geo_unCentered));

        mesh.normals.push_back(normal);

        mesh.tangents.push_back(glm::vec3(0));
        mesh.bitangents.push_back(glm::vec3(0));

        auto uv = glm::vec2(((posLatLon - frame.start) / (frame.getEnd() - frame.start)) * math::llaDistance(frame.start, frame.getEnd()));

        mesh.uvs.push_back(uv);
    }

    for (size_t i = 0; i < roofMesh->indicies[0].size(); i++)
    {
        mesh.indicies[mesh.indicies.size() - 1].push_back(roofMesh->indicies[0][i] + startVertOfset);
    }

}


//TODO: right now this assumes it is the first creator to act on the mesh
void groundCreator::createSubdividedQuadChunkMesh(Mesh & mesh, const Box& frame)
{
    mesh.indicies.push_back({});
    mesh.attributes->subMeshMats.push_back(0);
        

    const glm::dvec3 center_geo = math::LlatoGeo(glm::dvec3(frame.getCenter(), 0), {}, radius);

    size_t resolution = 10;
    auto d_resolution = static_cast<double>(resolution);
    //const Box& frame = frame;

    glm::uint32 vert = 0;
    const glm::uint32 startVertOfset = 0;


    auto nonLLAFrameWidth =  math::llaDistance(frame.start,glm::vec2(frame.start.x,frame.start.y + frame.size.y));
    auto nonLLAFrameHeight = math::llaDistance(frame.start,glm::vec2(frame.start.x + frame.size.x,frame.start.y));
    
            
    for (size_t x = 0; x <= resolution; x++)
    {
        for (size_t y = 0; y <= resolution; y++)
        {
            // vertex creation

            auto chunkStrideLat = (frame.size.x / d_resolution) * static_cast<double>(x);
            auto chunkStrideLon = (frame.size.y / d_resolution) * static_cast<double>(y);

            auto lat = frame.start.x + chunkStrideLat;
            auto lon = frame.start.y + chunkStrideLon;
            glm::dvec3 lla(lat, lon, 0);

            auto geo_unCentered = math::LlatoGeo(lla, {}, radius);

            mesh.verts.emplace_back(geo_unCentered - center_geo);//math::LlatoGeo(glm::dvec3(frame.getCenter(), 0), {}, radius));
#if SSSS && _DEBUG
            mesh.normals.emplace_back(glm::vec3(0, 0.8, 0.1));//static_cast<glm::vec3>(glm::normalize(geo_unCentered)) * (static_cast<float>(chunk.lodLevel + 1) / 13.f));
#else
            mesh.normals.emplace_back(static_cast<glm::vec3>(glm::normalize(geo_unCentered)));
#endif


            // chunk uvs    
            auto uvx = (chunkStrideLat / frame.size.x) * nonLLAFrameHeight;
            auto uvy = (chunkStrideLon / frame.size.y) * nonLLAFrameWidth;
            // world uvs
//                    let uvx = (y.double - frame.origin.y) / resolution.double / (360 / frame.size.y) + ((frame.origin.y + 180) / 360)
//                    let uvy = (x.double - frame.origin.x) / resolution.double / (180 / frame.size.x) + ((frame.origin.x + 90) / 180)
            // world uvs 2
//                    let uvy = ((lla.x + 90) / 180)
//                    let uvx = ((lla.y + 180) / 360)
//                    if uvx > 1 {
//                        uvx -= 1
//                    }

            mesh.uvs.emplace_back(static_cast<float>(uvx), static_cast<float>(uvy));

        }
    }

    for (size_t x = 0; x < resolution; x++)
    {
        for (size_t y = 0; y < resolution; y++)
        {
            // indicies creation

            auto xSize = resolution;

            mesh.indicies[mesh.indicies.size() - 1].emplace_back(vert + startVertOfset);
            mesh.indicies[mesh.indicies.size() - 1].emplace_back(vert + startVertOfset + xSize + 1);
            mesh.indicies[mesh.indicies.size() - 1].emplace_back(vert + startVertOfset + 1);
            mesh.indicies[mesh.indicies.size() - 1].emplace_back(vert + startVertOfset + 1);
            mesh.indicies[mesh.indicies.size() - 1].emplace_back(vert + startVertOfset + xSize + 1);
            mesh.indicies[mesh.indicies.size() - 1].emplace_back(vert + startVertOfset + xSize + 2);

            vert++;
        }
        vert++;
    }

    mesh.tangents.resize(mesh.verts.size());
    mesh.bitangents.resize(mesh.verts.size());

}
