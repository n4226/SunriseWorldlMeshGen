#pragma once

#include "Sunrise.h"
#include <underlingSystems/ShapeFileSystem.h>

#include "generation/GenerationSystem.h"

/// <summary>
/// misnamed bc may not be a ui scene
/// </summary>
class MainMeshGenUIScene: public sunrise::WorldScene
{
public:
	//using UIScene::UIScene;
	using WorldScene::WorldScene;

	virtual void load() override;

	virtual void onDrawUI() override;

	void meshGenChunkProgressUI(int minDivided, int maxDivided);

	void drawTreeNodeLevel(const std::vector<std::unordered_map<std::string, ChunkGenStatus>>& layeredChunks, int level, ImGuiTreeNodeFlags treeNodeFlags,
		Box* filterInside = nullptr);

	void setShowingChunks(std::array<float, 2>& center, int mindividedCount, int maxDividedCount, int currentDividedCount, std::array<int, 2>& size);

	virtual void onDrawMainMenu() override;

	virtual void unload() override;

private:

	void runMeshGen(const std::vector<Box>& chunks);

	bool showGenerationWindow = true;


	glm::dvec2 defaultCenter = glm::dvec2(40.75699, -73.92144);

	//ChunkGenStatus::Container lastRunGenStatus
	GenerationSystem* currentGenSys = nullptr;
	
	//ShapeFileSystem* shapeSys;
};

