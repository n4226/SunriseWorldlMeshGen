#include "mgpch.h"
#include "GenerationSystem.h"

#include "marl/scheduler.h"
#include "marl/task.h"
#include "marl/waitgroup.h"
#include "marl/defer.h"

#include "systems/buildingCreator.h"
#include "systems/groundCreator.h"
#include "systems/RoadCreator.h"

#include <iostream>
#include <fstream>
#include <thread>

#ifdef SR_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include <filesystem>
#include <algorithm>
#include <execution>

#include <chrono>


#include"igl/writeOBJ.h"

GenerationSystem::GenerationSystem(std::vector<Box>&& chunks)
    : osmFetcher(),
    chunks(chunks), 
    creators({
    new groundCreator(),
    //new buildingCreator(),
    //new RoadCreator(),
    }),
    outputDir(FileManager::engineTerrainChunkDir()),
    attrOutputDir(FileManager::engineTerrainChunkAttributesDir())
{
}


// lod no longer taken in but calulated
void GenerationSystem::generate(int __lod)
{
    SR_INFO("attempting to generate {} chunks", chunks.size());

    auto onlyUseOSMCash = MeshGenConfig::get().onlyUseOsmCash;
    auto startTime = std::chrono::high_resolution_clock::now();
    //for (Box& chunk : chunks) {

    // Create a WaitGroup with an initial count of numTasks.
#if USE_MARL
    marl::WaitGroup finishedChunk(chunks.size());
    //auto ticket = osmFetcher.waitforServerQueue.take();

    for (size_t i = 0; i < chunks.size(); i++)
    {
        auto& chunk = chunks[i];
        marl::schedule([this, onlyUseOSMCash, lod, chunk,finishedChunk] {

#else

        std::for_each(MG_EXECUION_POLOCY chunks.begin(), chunks.end(), [this,onlyUseOSMCash](Box& chunk) {
#endif

            constexpr double baseSize = 90.0 / 4096;
            auto lod = static_cast<size_t>((chunk.size.x / baseSize) - 1);

            auto stats = ChunkGenerationStatistics(chunk, lod);

            stats.startTimer();

            auto file = outputDir + chunk.toString() + ".bmesh";
            auto attrFile = attrOutputDir + chunk.toString() + ".bmattr";
            try {

                if (std::filesystem::exists(file) && MeshGenConfig::get().skipExisting) {
                    printf("skipping an already saved chunk chunk\n");
                    throw std::runtime_error("already made");
                }

                BinaryMeshAttrributes binaryAttributes{};
                Mesh mesh;

                mesh.attributes = &binaryAttributes;

                printf("going to get Osm for a chunk\n");
                osm::osm osmData = osmFetcher.fetchChunk(chunk, onlyUseOSMCash,stats);


                printf("Got Osm for a chunk\n");

                for (icreator* creator : creators)
                    creator->createInto(mesh, osmData, chunk, lod,stats);

                // posibly temporary:
                /*for (size_t i = 0; i < mesh.indicies.size(); i++)
                {
                    if (mesh.indicies[i].size() == 0)

                }*/

                //TODO WARNING LIFE CYCLE OF THIS MESH LIMETED TO THIS SCOPE
                stats.logWholeMesh(&mesh);

                BinaryMeshSeirilizer binaryMesh(mesh);


                // write to file
                printf("writing to mesh file\n");
                {
                    FileManager::createIntermediateDirs(file);
                    FileManager::createIntermediateDirs(attrFile);

                    std::ofstream out;
                    out.open(file, std::fstream::out | std::fstream::binary);


                    out.write(reinterpret_cast<char*>(binaryMesh.mesh), binaryMesh.meshLength);
                    out.close();

                    binaryAttributes.saveTo(attrFile);

                    printf("wrote to mesh file\n");
                }
                stats.endTimer();

                SR_INFO("printing chunk statistics:");

                //printf(stats.printLog().c_str());

                FileManager::saveStringToFile(stats.printLog(), outputDir + chunk.toString() + ".stats");
            }
#if USE_MARL
            catch (...) {
                finishedChunk.done();
                return;
            }

            finishedChunk.done();
        });
    }

    finishedChunk.wait();
#else
            catch (const std::exception& e) {
                // moving onto next chunk nothing needed here
                SR_CRITICAL("UNHANDLED CHUNK creation exeption: {}", e.what());
                return;
            }

            // moving onto next chunk nothing needed here
    });
#endif


    auto endTime = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration<double>(endTime - startTime);
    printf("finished all chunks in %llf seconds \n",time);

}

void GenerationSystem::debugChunk(size_t index, int lod)
{
    const auto& chunk = chunks[index];

    auto stats = ChunkGenerationStatistics(chunk, lod);

    stats.startTimer();


    BinaryMeshAttrributes binaryAttributes{};
    Mesh mesh;

    mesh.attributes = &binaryAttributes;

    printf("going to get Osm for a chunk\n");
    osm::osm osmData = osmFetcher.fetchChunk(chunk, false,stats);


    printf("Got Osm for a chunk\n");

    for (icreator* creator : creators)
        creator->createInto(mesh, osmData, chunk,lod,stats);
    
    stats.logWholeMesh(&mesh);
    stats.endTimer();

    std::string path = "E:\\Sunrise-World-Data\\test_binary_ops";

    for (size_t i = 0; i < mesh.indicies.size(); i++)
    {

        Eigen::MatrixXd V;
        Eigen::MatrixXi I;

        makeLibiglMesh(mesh, i, V, I);


        igl::writeOBJ(path + "/cube(" + std::to_string(i) + ").obj", V, I);
    }


    FileManager::saveStringToFile(stats.printLog(), path + "/" + chunk.toString() + ".stats");
    //mesh.indicies.erase(mesh.indicies.begin());
    //mesh::displayMesh(mesh);
    

    //BinaryMeshSeirilizer binaryMesh(mesh);

}

std::vector<Box> GenerationSystem::genreateChunksAround(glm::dvec2 desired, int divided, glm::ivec2 formation)
{
    auto chunk = actualChunk(desired, divided);

    std::vector<Box> chunks;

    auto minx = static_cast<int>(floor(static_cast<double>(formation.x) / 2));
    auto miny = static_cast<int>(floor(static_cast<double>(formation.y) / 2));

    for (int x = -minx; x <= minx; x++)
    {
        for (int y = -miny; y <= miny; y++)
        {
            auto stride = glm::dvec2(chunk.size.x * static_cast<double>(x), chunk.size.y * static_cast<double>(y));
            auto newStart = chunk.start + stride;

            auto newChunk = Box(newStart, chunk.size);
            chunks.push_back(newChunk);
        }
    }
    
    return chunks;

}

Box GenerationSystem::actualChunk(glm::dvec2 desired, int divided)
{
    auto frame = Box(glm::dvec2(0, -90), glm::dvec2(90, 90));

    for (size_t i = 0; i < divided; i++)
    {
        frame.size /= 2;
    }

    auto x1 = floor((desired.x - 00) / frame.size.x) * frame.size.x;
    auto y1 = floor((desired.y + 00) / frame.size.y) * frame.size.y;

    return Box(glm::dvec2(x1, y1),frame.size);
}
