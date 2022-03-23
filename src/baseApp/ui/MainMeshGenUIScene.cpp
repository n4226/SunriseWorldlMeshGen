#include "mgpch.h"
#include "MainMeshGenUIScene.h"

#include <shellapi.h>
#include <generation/GenerationSystem.h>

//void LoadingIndicator(uint32_t started_showing_at) {
//	ImVec2 cursor = ImGui::GetCursorScreenPos() + (ImVec2(12, 12));
//	const float speed_scale = 10.0f;
//	float cos = cosf(tick / speed_scale);
//	float sin = sinf(tick / speed_scale);
//	float size = scale * 10.0f;
//
//	u32 alpha = (u32)roundf(lerp(started_showing_at, tick, 255, 14));
//
//	ImGui::GetWindowDrawList()->AddQuadFilled(
//		cursor + ImRotate(ImVec2(-size, -size), cos, sin),
//		cursor + ImRotate(ImVec2(+size, -size), cos, sin),
//		cursor + ImRotate(ImVec2(+size, +size), cos, sin),
//		cursor + ImRotate(ImVec2(-size, +size), cos, sin),
//		IM_COL32(0, 255, 200, alpha)
//	);
//}


void MainMeshGenUIScene::load()
{
	terrainMask = new std::vector<math::Box>{ };

	WorldScene::load();

	SR_INFO("loading shape files");
	shapeFileSystem = new ShapeFileSystem();
	SR_INFO("finished loading shape files");

	//set location to Newark Airport
	glm::dvec3 ewr(40.689352, -74.174450,30);
	playerLLA = ewr;
	defaultCenter = ewr;
}

void MainMeshGenUIScene::onDrawUI()
{

	WorldScene::onDrawUI();

	//ImGui::ShowDemoWindow();

	if (showGenerationWindow) {

		ImGui::Begin("Generation Tasks");

		if (ImGui::Button("Open Config File")) {
			ShellExecuteA(NULL, "open", FileManager::appConfigDir().c_str(), NULL, NULL, SW_SHOWDEFAULT);
		}

		static auto meshGenConfig = MeshGenConfig::get();
		
		if (ImGui::Checkbox("Skip Existing Chunks", &meshGenConfig.skipExisting)) {
			MeshGenConfig::write(meshGenConfig);
		}

		ImGui::Separator();

		ImGui::Text("Generation:");

		ImGui::Text("Chose a base chunk than a size and a start depth.");
		ImGui::Text("This creates a grid of chunks all the same size.\n");
		ImGui::Text("Then, chose a max depth this will generate child chunks each a 1 / 4 the size of their parent until the max depth is reached.");

		ImGui::Spacing();

		//auto t = glm::value_ptr(glm::vec2(defaultCenter.x, defaultCenter.y));
		static std::array<float, 2> center{defaultCenter.x,defaultCenter.y};

		
		static int dividedCount = 11;
		static int maxDividedCount = 12;

		ImGui::InputFloat2("Center",center.data());

		// point of scrub = 40.7063459, -74.0566669

		static std::array<int, 2> size{ 1,1 };

		ImGui::DragInt2("Generation Size",size.data(),1,0,100);

		if (ImGui::InputInt("Start Depth", &dividedCount) && dividedCount > maxDividedCount) {
			maxDividedCount = dividedCount;
		}

		if (ImGui::InputInt("Max Depth", &maxDividedCount) && maxDividedCount < dividedCount) {
			dividedCount = maxDividedCount;
		}

		if (ImGui::Button("Reset to Default")) {
			std::array<float,2> newcenter{defaultCenter.x,defaultCenter.y};
			center = newcenter;
		}
		ImGui::SameLine();
		if (ImGui::Button("Set to Current Position")) {
			std::array<float, 2> newcenter{ playerLLA.x,playerLLA.y};
			center = newcenter;
		}
		//ImGui::SameLine();


		if (ImGui::Button("RunGeneration")) {
			runMeshGen(GenerationSystem::genreateChunksAround
			(glm::make_vec2(center.data()), dividedCount, maxDividedCount, glm::make_vec2(size.data())));
		}
		ImGui::SameLine();
		
		if (ImGui::Button("Re Run ")) {
			if (currentGenSys) {
				auto statuses = currentGenSys->chunkStats.lock();
				std::vector<Box> chunks{};

				for (auto& [chunk, status] : *statuses)
					chunks.push_back(chunk);

				runMeshGen(chunks);
			}
		}

		ImGui::Separator();


		if (currentGenSys) {
			meshGenChunkProgressUI(dividedCount, maxDividedCount);

		}

		ImGui::Spacing();

		ImGui::Separator();

		ImGui::Text("Debug Draw:");
		ImGui::SameLine();
		if (ImGui::Button("Reload Current")) {
			reloadTerrainInMask();
		}

		ImGui::SameLine();
		if (ImGui::Button("Move cam to first")) {
			if (terrainMask->size() > 0) {
				auto first = (*terrainMask)[0];
				playerLLA.x = first.getCenter().x;
				playerLLA.y = first.getCenter().y;
			}
		}

		static int displayingDepth = dividedCount;
		static int currentDisplayDepth = 0;

		static bool displayingGenerating = true;



		if (ImGui::Checkbox("Draw Generated", &displayingGenerating)) {
			if (!displayingGenerating) {
				terrainMask->clear();
				currentDisplayDepth = 0;
				reloadTerrainInMask();
			}
			else {

			}
		}

		ImGui::SameLine();
		ImGui::Checkbox("Draw chunks not on disk",&doNotSparslyPopulateMask);

		if (displayingGenerating) {

			ImGui::SliderInt("Displaying Depth", &displayingDepth, dividedCount, maxDividedCount);

			if (displayingDepth != currentDisplayDepth) {
				currentDisplayDepth = displayingDepth;

				setShowingChunks(center, dividedCount, maxDividedCount, currentDisplayDepth, size);
				reloadTerrainInMask();
			}
		}

		/*if (ImGui::Button("add first generated")) {
			auto newChunks = GenerationSystem::genreateChunksAround
			(glm::make_vec2(center.data()), dividedCount, glm::make_vec2(size.data()));
			(*terrainMask).push_back(newChunks[0]);
		}

		if (ImGui::Button("add all generated")) {
			auto newChunks = GenerationSystem::genreateChunksAround
			(glm::make_vec2(center.data()), dividedCount, glm::make_vec2(size.data()));
			(*terrainMask).insert(terrainMask->end(), newChunks.begin(), newChunks.end());
		}*/

		ImGui::Spacing();

		static std::string addDrawString = "";

		ImGui::InputText("Add chunk", &addDrawString);

		ImGui::SameLine();

		if (ImGui::Button("add")) {
			auto chunk = math::Box(addDrawString);
			//SR_CORE_INFO("created chunk {}",chunk.toString());
			(*terrainMask).push_back(chunk);
		}

		if (ImGui::Button("Clear all")) {
			(*terrainMask).clear();
		}

		ImGui::Separator();
		ImGui::Text("Current Chunks (on disk)");



		ImGui::End();

	}

}

void MainMeshGenUIScene::meshGenChunkProgressUI(int minDivided, int maxDivided)
{
	auto treeNodeFlags = ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	auto statuses = currentGenSys->chunkStats.lock();

	auto completed = 0;
	auto total = 0;
	auto failed = 0;

	std::vector<std::unordered_map<std::string,ChunkGenStatus>> layeredChunks{};

	layeredChunks.resize(maxDivided - minDivided + 1);

	for (auto& [item, status] : *statuses) {
		total += 1;
		if (status.completed) completed += 1;
		if (status.failed) failed += 1;
		auto chunk = math::Box(item);
		auto chunkDivided = log(90 / chunk.size.x) / log(2);
		auto chunkDividedInt = static_cast<int>(round(chunkDivided));
		auto index = chunkDividedInt - minDivided;
		SR_ASSERT(index <= minDivided);
		layeredChunks[index].emplace(std::make_pair(chunk.toString(),status));
	}

	ImGui::ProgressBar(total == 0 ? 0 : ((float)completed / (float)total));
	ImGui::Text("%d of %d processed, %d errors", completed, total, failed);

	if (ImGui::TreeNodeEx("generatedTree", treeNodeFlags, "Last Generation Tasks")) {
		auto level = 0;
		drawTreeNodeLevel(layeredChunks, level, treeNodeFlags);

		ImGui::TreePop();
	}
}

void MainMeshGenUIScene::drawTreeNodeLevel(const std::vector<std::unordered_map<std::string, ChunkGenStatus>>& layeredChunks,
	int level, ImGuiTreeNodeFlags treeNodeFlags,Box* filterInside)
{
	if (layeredChunks.size() <= level) { return; };
	for (auto& [chunk, status] : layeredChunks[level]) {

		auto first = math::Box(chunk);
		if (filterInside) {
			//must be inside given box
			/*auto childs = filterInside->children();
			std::array<Box, 4>::iterator pos = std::find_if(childs.begin(), childs.end(), [first](auto& i) {
				return i.overlaps(first);
			});
			std::array<Box, 4>::iterator end = childs.end();
			if (pos == end) {
				continue;
			}*/
			//todo: fix acuracy
			if (!filterInside->overlaps(first)) {
				continue;
			}
		}

		if (!status.completed)
			ImGui::PushStyleColor(ImGuiCol_Text, { 0.8,0.8,0.1,1 });
		else if (status.failed)
			ImGui::PushStyleColor(ImGuiCol_Text, { 0.8,0,0.1,1 });
		else
			ImGui::PushStyleColor(ImGuiCol_Text, { 0,0.8,0.1,1 });


		ImGui::PushID(chunk.c_str());
		if (ImGui::TreeNodeEx(chunk.c_str(), treeNodeFlags, chunk.c_str())) {


			if (ImGui::Button("Move here")) {
				playerLLA.x = first.getCenter().x;
				playerLLA.y = first.getCenter().y;
			}

			if (ImGui::Button("Regenerate"))
				runMeshGen({ first });
			ImGui::Button("Regenerate including all levels of children (not functional yet)");


			if (ImGui::Button("Copy"))
				ImGui::SetClipboardText(chunk.c_str());

			//draw children
			if (layeredChunks.size() > (level + 1)) {
				drawTreeNodeLevel(layeredChunks, level + 1, treeNodeFlags, &first);
			}

			ImGui::TreePop();
		}
		ImGui::PopID();

		ImGui::PopStyleColor();
	}
}

void MainMeshGenUIScene::setShowingChunks(std::array<float, 2>& center, int mindividedCount, int maxDividedCount, int currentDividedCount, std::array<int, 2>& size)
{
	auto glmCenter = glm::make_vec2(center.data());
	auto newChunks = GenerationSystem::genreateChunksAround
	(glmCenter, mindividedCount,maxDividedCount, glm::make_vec2(size.data()));

	std::vector<Box> filteredChunks{};
	filteredChunks.resize(newChunks.size());
	const auto correctSize = GenerationSystem::actualChunk(glmCenter, currentDividedCount).size.x;
	auto newEnd = std::copy_if(newChunks.begin(), newChunks.end(), filteredChunks.begin(), [correctSize](auto& elm) {
		return elm.size.x == correctSize;
	});
	filteredChunks.erase(newEnd, filteredChunks.end());

	terrainMask->clear();
	(*terrainMask).insert(terrainMask->end(), filteredChunks.begin(), filteredChunks.end());
}

void MainMeshGenUIScene::onDrawMainMenu()
{

	if (ImGui::BeginMenu("Generation")) {

		ImGui::MenuItem("Generation Window", "", &showGenerationWindow);

		ImGui::EndMenu();
	}
}

void MainMeshGenUIScene::unload()
{
	delete shapeFileSystem;
}

void MainMeshGenUIScene::runMeshGen(const std::vector<Box>& chunks)
{

	SR_INFO("About to start mesh gen process");


	if (currentGenSys)
		delete currentGenSys;

	currentGenSys = new GenerationSystem(std::move(chunks));

	//marl::WaitGroup wait(1);

	marl::schedule([=] {
		currentGenSys->generate();

		SR_INFO("Finished mesh gen process");
		//wait.done();
	});

}
