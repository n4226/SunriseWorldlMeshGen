#include "mgpch.h"
#include "GenerationStatistics.h"

ChunkGenerationStatistics::ChunkGenerationStatistics(math::Box chunk, size_t lod)
    : chunk(chunk), lod(lod) 
{
    for (auto t : allUses()) {
        verts[t] = 0;
    }
}

void ChunkGenerationStatistics::logVerts(size_t number, VertUse use)
{
    verts[use] = verts[use] + number;
    totalVerts += number;
}

void ChunkGenerationStatistics::logWholeMesh(Mesh* mesh)
{
    wholeMesh = mesh;
}

std::string ChunkGenerationStatistics::printLog()
{
    std::string output = "Chunk Statistics: \n";

    output.append(std::string("\n\nChunk: ") + chunk.toString());
    output.append(std::string("\n\nlod: ") + std::to_string(lod));


    output.append(std::string("\n\n\nVerticies: \n"));

    output.append(std::string("\nLand: ")
        + std::to_string(verts[VertUse::land])     + std::string(", % = ") + std::to_string(static_cast<double>(verts[VertUse::land])    / totalVerts));
    output.append(std::string("\nWater: ")
        + std::to_string(verts[VertUse::water])    + std::string(", % = ") + std::to_string(static_cast<double>(verts[VertUse::water])   / totalVerts));
    output.append(std::string("\nBuildings: ")
        + std::to_string(verts[VertUse::building]) + std::string(", % = ") + std::to_string(static_cast<double>(verts[VertUse::building]) / totalVerts));
    output.append(std::string("\n\nTotal (internal, actual mesh count): ") 
        + std::to_string(totalVerts) + std::string(", ") + std::to_string(wholeMesh->verts.size()));

    output.append(std::string("\n\nFile Size: \n") );


    output.append(std::string("\n\nTimes: \n"));


    output.append(std::string("\nTotal: ")
        + std::to_string(std::chrono::duration<double>(timerEnd - timerStart).count()) + " s");

    output.append(std::string("\nGetting Osm from server or cash: ")
        + std::to_string(std::chrono::duration<double>(jsonReceived - timerStart).count()) + " s");
    output.append(std::string("\nParsing Json to OSM classes: ")
        + std::to_string(std::chrono::duration<double>(osmParsed - jsonReceived).count()) + " s");
    output.append(std::string("\n Generating mesh from OSM: ")
        + std::to_string(std::chrono::duration<double>(timerEnd - osmParsed).count()) + " s");

    output.append("\n\n");

    return output;
}
