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

#include "GenerationHelpers.h"

GenerationSystem::GenerationSystem(const std::vector<Box>& chunks)
    : osmFetcher(),
    chunks(chunks),
    outputDir(FileManager::engineTerrainChunkDir()),
    attrOutputDir(FileManager::engineTerrainChunkAttributesDir())
{
}


/// <summary>
/// do not run multiple in parallel from the same system
/// </summary>
ChunkGenStatus::Container GenerationSystem::generate()
{
    if (running) {
        SR_WARN("another generate task already running so stopping this one");
        return {};
    }
    running = true;
    defer(running = false);

    SR_INFO("attempting to generate {} chunks", chunks.size());

    {
        auto stats = chunkStats.lock();
        stats->clear();
        stats->reserve(chunks.size());

        for (auto& chunk : chunks)
            (*stats)[chunk.toString()] = {};
    }

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

            SR_TRACE("making chunk with lod {}", lod);

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

                generateChunk(chunk, lod, mesh, stats, osmData);

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

                //SR_INFO("printing chunk statistics:");

                //printf(stats.printLog().c_str());

                FileManager::saveStringToFile(stats.printLog(), outputDir + chunk.toString() + ".stats");

                {
                    auto progressStats = chunkStats.lock();

                    (*progressStats)[chunk.toString()].completed = true;
                }
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

                {
                    auto progressStats = chunkStats.lock();

                    auto key = chunk.toString();

                    (*progressStats)[key].completed = true;
                    (*progressStats)[key].failed = true;
                }

                return;
            }

            // moving onto next chunk nothing needed here
    });
#endif


    auto endTime = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration<double>(endTime - startTime);
    SR_INFO("finished all chunks in {} seconds \n",time);

    {
        auto progressStats = chunkStats.lock();
        return *progressStats;
    }
}



void GenerationSystem::generateChunk(Box chunk, size_t lod, Mesh& mesh, ChunkGenerationStatistics& stats, const osm::osm& osmData)
{

    icreator::ChunkData chunkData = {chunk, lod, osmData, stats};

    auto creators = createCreators(chunkData);

    //creator->createInto(mesh, osmData, chunk, lod, stats);

    // generation of ground polygons

    mesh::ShadedMultiPolygon2D groundPolygons;

    for (icreator* creator : creators) {
        creator->initInChunk(false,mesh);
        auto pol = creator->polygonsFromOSM();
        groundPolygons.insert(groundPolygons.end(), pol.begin(), pol.end());
        
        for (const osm::element& element : osmData.elements) {
            auto pol = creator->polygonsFromElement(element);
            groundPolygons.insert(groundPolygons.end(), pol.begin(), pol.end());
        }
    }



    mesh::MultiPolygon2D allLandPolygons{};

    for (auto& p : groundPolygons)
        allLandPolygons.push_back(p.polygon);

    allLandPolygons = mesh::bunionAll(allLandPolygons);

    mesh::MultiPolygon2D framePoints = { { chunk.polygon() } };

    allLandPolygons = mesh::binterseciton(framePoints, allLandPolygons);


    // ocean should be (frame - (union of all landmasses)) where - is a difference of polygons
    auto ocean = mesh::bDifference({ {framePoints} }, allLandPolygons);

    //TODO: temporary has to be first thing in mesh for some reason
    //groundCreator::createSubdividedQuadChunkMesh(mesh, chunk);


    // projection / trianglulation of ground step

    /*for (auto& pol : groundPolygons) {
        if (pol.polygon.size() == 0) continue;
        mesh.indicies.push_back({});
        mesh.attributes->subMeshMats.push_back(pol.material);
        GenerationHelpers::drawMultPolygonInChunk(pol.polygon, mesh,chunk,&stats);
    }*/
    //draw ocean
    mesh.indicies.push_back({});
    mesh.attributes->subMeshMats.push_back(0);
    for (auto& pol : ocean) {
        if (pol.size() == 0) continue;
        GenerationHelpers::drawHPolygonInChunk(pol, mesh, chunk, &stats);
    }


    mesh.indicies.push_back({});
    mesh.attributes->subMeshMats.push_back(1);
    for (auto& pol : allLandPolygons) {
        if (pol.size() == 0) continue;
        GenerationHelpers::drawHPolygonInChunk(pol, mesh,chunk,&stats);
    }


    //todo: fix only the first drawn submesh apears in world -- for tomorrow

    // generation of rest of 3d mesh

    for (icreator* creator : creators) {
        creator->initInChunk(true,mesh);

        creator->meshFromOSM(mesh);

        for (const osm::element& element : osmData.elements)
            creator->meshFromElement(mesh, element);
    }

    //cleanup

    for (auto creator : creators)
        delete creator;

}

std::vector<icreator*> GenerationSystem::createCreators(icreator::ChunkData chunkData)
{
    return {
        new groundCreator(chunkData),
        new buildingCreator(chunkData),
        //new RoadCreator(chunkData),
    };
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

    generateChunk(chunk, lod, mesh, stats, osmData);
    
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
