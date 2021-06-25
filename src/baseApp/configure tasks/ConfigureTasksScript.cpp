#include "mgpch.h"
#include "ConfigureTasksScript.h"

#include "../../underlingSystems/ShapeFileSystem.h"
#include "../../generation/GenerationSystem.h"

void ConfigureTasksScript::runScript()
{
	// marl created and configured by engine
	//auto confic = marl::Scheduler::Config();

	//confic.setWorkerThreadCount(std::thread::hardware_concurrency());

	//auto scheduler = new marl::Scheduler(confic);

	//scheduler->bind();

	//defer(scheduler->unbind());

	shapeFileSystem = new ShapeFileSystem();


	glm::dvec2 desired(40.610319941413, -74.039182662964);

	//glm::dvec2 kewrCenter(-2.4 + 40.6901525, -48.2 + -74.1684951);
	glm::dvec2 kewrCenter(37.5128831, -122.3585615);

	glm::dvec2 ksfoApproach(37.6128831, -122.3585615);

	//GenerationSystem genSys(GenerationSystem::genreateChunksAround(desired, 12, glm::ivec2(3, 3)));
	//GenerationSystem genSys(GenerationSystem::genreateChunksAround(desired, 11, glm::ivec2(3, 3)));

	//GenerationSystem genSys(GenerationSystem::genreateChunksAround(desired,12, glm::ivec2(12,12)));
	//GenerationSystem genSys(GenerationSystem::genreateChunksAround(desired,12, glm::ivec2(27,27)));


	auto chunks = {
		//GenerationSystem::genreateChunksAround(desired, 12, glm::ivec2(12, 12)),
		GenerationSystem::genreateChunksAround(desired, 11, glm::ivec2(6, 6)),
		//GenerationSystem::genreateChunksAround(desired, 11, glm::ivec2(1, 1)),


		//GenerationSystem::genreateChunksAround(desired, 12, glm::ivec2(27, 27)),
		//GenerationSystem::genreateChunksAround(desired, 11, glm::ivec2(9, 9))
	};

	std::vector<Box> flatChunks = {};

	for (auto& r : chunks) {
		for (auto& c : r) {
			flatChunks.push_back(c);
		}
	}

	GenerationSystem genSys(std::move(flatChunks));


	genSys.generate(0);

	// first parm is chunk index and second is lod level
	//genSys.debugChunk(0,1);



	delete shapeFileSystem;
}
