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

std::string ChunkGenerationStatistics::printLog()
{
    std::string output = "Chunk Statistics: \n";

    output.append(std::string("\nLand: ")
        + std::to_string(verts[VertUse::land])     + std::to_string(static_cast<double>(verts[VertUse::land])    / totalVerts));
    output.append(std::string("\nWater: ") 
        + std::to_string(verts[VertUse::water])    + std::to_string(static_cast<double>(verts[VertUse::water])   / totalVerts));
    output.append(std::string("\nBuildings: ") 
        + std::to_string(verts[VertUse::building]) + std::to_string(static_cast<double>(verts[VertUse::building]) / totalVerts));
    output.append(std::string("\n\nTotal: ") 
        + std::to_string(totalVerts));

    return output;
}
