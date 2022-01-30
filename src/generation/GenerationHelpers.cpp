#include "mgpch.h"
#include "GenerationHelpers.h"




/// <summary>
/// will draw in the last submesh so push this back before hand for new material
/// </summary>
/// <param name="polygon"></param>
/// <param name="mesh"></param>
/// <param name="frame"></param>
/// <param name="stats"></param>
void GenerationHelpers::drawMultPolygonInChunk(
    const mesh::HPolygon2D& polygon, Mesh& mesh,
    const Box& frame, ChunkGenerationStatistics* stats, bool geoCenter)
{


    SR_ASSERT(polygon.size() > 0);
    auto roofMesh = mesh::triangulate(polygon).first;
    //auto roofMesh = mesh::triangulateEarcut(polygon);

    glm::dvec3 center_geo;
    if (geoCenter)
        center_geo = math::LlatoGeo(glm::dvec3(frame.getCenter(), 0), {}, radius);
    else {
        center_geo = glm::dvec3(frame.getCenter(), 0);
    }

    auto startVertOfset = mesh.verts.size();


    for (size_t i = 0; i < roofMesh.verts.size(); i++)
    {
        auto posLatLon = roofMesh.verts[i];

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

    for (size_t i = 0; i < roofMesh.indicies[0].size(); i++)
    {
        mesh.indicies[mesh.indicies.size() - 1].push_back(roofMesh.indicies[0][i] + startVertOfset);
    }

}
