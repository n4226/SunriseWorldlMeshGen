#pragma once

#include "mgpch.h"

#include "GenerationStatistics.h"

class GenerationHelpers
{
public:
    /// <summary>
    /// will draw in the last submesh so push this back before hand for new material
    /// </summary>
    /// <param name="polygon"></param>
    /// <param name="mesh"></param>
    /// <param name="frame"></param>
    /// <param name="stats"></param>
    static void drawHPolygonInChunk(
        const mesh::HPolygon2D& polygon, Mesh& mesh,
        const Box& frame, ChunkGenerationStatistics* stats, bool geoCenter = true);
};