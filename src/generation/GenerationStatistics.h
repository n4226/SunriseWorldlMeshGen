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
	void logWholeMesh(Mesh* mesh);

	// time profiling

	void startTimer() { timerStart = std::chrono::high_resolution_clock::now(); }
	void markOsmJSONReceived() { jsonReceived = std::chrono::high_resolution_clock::now(); }
	void markOSMParsedFromJSON() { osmParsed = std::chrono::high_resolution_clock::now(); }
	void endTimer() { timerEnd = std::chrono::high_resolution_clock::now(); }

	std::string printLog();

protected:

	math::Box chunk;
	size_t lod;

	Mesh* wholeMesh = 0;

	std::unordered_map<VertUse, size_t> verts = {};
	size_t totalVerts = 0;

	// timers
	std::chrono::high_resolution_clock::time_point timerStart;
	std::chrono::high_resolution_clock::time_point jsonReceived;
	std::chrono::high_resolution_clock::time_point osmParsed;
	std::chrono::high_resolution_clock::time_point timerEnd;

};

