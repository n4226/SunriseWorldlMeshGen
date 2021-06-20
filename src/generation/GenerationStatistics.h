#pragma once

#include "mgpch.h"

class ChunkGenerationStatistics
{
public:
	
	enum class VertUse {
		land,
		water,
		building,
	};
	static constexpr std::array<VertUse, 3> allUses() {
		return { VertUse::land, VertUse::water, VertUse::building };
	}
	
	ChunkGenerationStatistics(math::Box chunk, size_t lod);
	~ChunkGenerationStatistics() = default;

	void logVerts(size_t number, VertUse use);

	std::string printLog();

protected:

	math::Box chunk;
	size_t lod;

	std::unordered_map<VertUse, size_t> verts = {};
	size_t totalVerts = 0;
};

