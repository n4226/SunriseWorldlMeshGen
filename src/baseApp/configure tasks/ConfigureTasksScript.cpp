#include "mgpch.h"
#include "ConfigureTasksScript.h"

#include "../../underlingSystems/ShapeFileSystem.h"
#include "../../generation/GenerationSystem.h"

#include  "Sunrise.h"

void testAllocator() {

	const size_t allocSize = 1001;
	auto allocator = sunrise::VaribleIndexAllocator(allocSize);

	SR_TRACE("allocator has {} indicies free, with a largest allocation size of {}", allocator.totalSize - allocator.allocatedSize, allocator.maxAllocationSizeAvailable());

	struct allocObject {
		size_t loc;
		size_t size;
	};

	std::vector<allocObject> allocations;

	for (size_t i = 0; i < 100; i++)
	{
		auto allocSize = i % 10;
		allocObject ob = { allocator.alloc(allocSize), allocSize };
		allocations.push_back(ob);
		SR_TRACE("allocator has {} indicies free, with a largest allocation size of {}", allocator.totalSize - allocator.allocatedSize, allocator.maxAllocationSizeAvailable());
	}

	SR_TRACE("allocator has {} indicies free, with a largest allocation size of {}", allocator.totalSize - allocator.allocatedSize, allocator.maxAllocationSizeAvailable());

	for (auto alloc : allocations)
		allocator.free(alloc.loc,alloc.size);

	SR_TRACE("allocator has {} indicies free, with a largest allocation size of {}", allocator.totalSize - allocator.allocatedSize, allocator.maxAllocationSizeAvailable());


}


void testBox() {

	// test1
	{
		auto b1 = math::Box({ 0,0 }, { 1,1 });
		auto b2 = math::Box({ 1,0 }, { 1,1 });

		SR_ASSERT(!b1.overlaps(b2) && !b2.overlaps(b1));
	}

	// test2
	{
		auto b1 = math::Box({ 0,0 }, { 1,1 });
		auto b2 = math::Box({ 1.8,2.3 }, { 1,1 });

		SR_ASSERT(!b1.overlaps(b2) && !b2.overlaps(b1));
	}

	// test3
	{
		auto b1 = math::Box({ 0,0 }, { 2,2 });
		auto b2 = math::Box({ 1,1 }, { 2,2 });

		SR_ASSERT(b1.overlaps(b2) && b2.overlaps(b1));
	}

	// test4
	{
		auto b1 = math::Box({ 0,0 }, { 4,4 });
		auto b2 = math::Box({ 1,1 }, { 2,2 });

		SR_ASSERT(b1.overlaps(b2) && b2.overlaps(b1));
	}

	// test5
	{
		auto b1 = math::Box({ -10,-10 }, { 2,2 });
		auto b2 = math::Box({ -20,-20 }, { 22,22 });

		SR_ASSERT(b1.overlaps(b2) && b2.overlaps(b1));
	}

}


void ConfigureTasksScript::runScript()
{

	//testAllocator();
	/*testBox();
	return;*/

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


	glm::dvec2 newNYTestPoint(40.75699, -73.92144);


	std::vector<std::vector<math::Box>> chunks = {
		GenerationSystem::genreateChunksAround(newNYTestPoint, 13, glm::ivec2(6, 6)),
		//GenerationSystem::genreateChunksAround(newNYTestPoint, 12, glm::ivec2(3, 3)),
		//GenerationSystem::genreateChunksAround(newNYTestPoint, 11, glm::ivec2(2, 2)),
		//GenerationSystem::genreateChunksAround(desired, 10, glm::ivec2(6, 6)),


		//GenerationSystem::genreateChunksAround(desired, 12, glm::ivec2(27, 27)),
		//GenerationSystem::genreateChunksAround(desired, 11, glm::ivec2(9, 9))
	};

	std::vector<Box> flatChunks = {};

	for (auto& r : chunks) {
		for (auto& c : r) {
			flatChunks.push_back(c);
		}
	}

	//flatChunks = { chunks[0][1] };

	GenerationSystem genSys(std::move(flatChunks));


	// lod nolonger taken in but calculated per chunk
	genSys.generate(-1);

	// first parm is chunk index and second is lod level
	//genSys.debugChunk(0,1);



	delete shapeFileSystem;
}
