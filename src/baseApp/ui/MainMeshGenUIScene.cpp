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
}

void MainMeshGenUIScene::onDrawUI()
{

	WorldScene::onDrawUI();

	//ImGui::ShowDemoWindow();

	if (showGenerationWindow) {

		ImGui::Begin("Generation Tasks");

		if (ImGui::Button("Open Config File")) {
			ShellExecuteA(NULL, "open", FileManager::engineConfigDir().c_str(), NULL, NULL, SW_SHOWDEFAULT);
		}

		ImGui::Separator();

		ImGui::Text("Generation:");

		//auto t = glm::value_ptr(glm::vec2(defaultCenter.x, defaultCenter.y));
		static std::array<float, 2> center{defaultCenter.x,defaultCenter.y};

		
		static int dividedCount = 11;

		ImGui::InputFloat2("Center",center.data());

		static std::array<int, 2> size{ 1,1 };

		ImGui::DragInt2("Generation dwSize",size.data(),1,0,100);

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
			(glm::make_vec2(center.data()), dividedCount, glm::make_vec2(size.data())));
		}
		ImGui::SameLine();
		
		ImGui::Text("Will generate ?? chunks");

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


		if (ImGui::Button("add first generated")) {

		}

		if (ImGui::Button("add all generated")) {
			auto newChunks = GenerationSystem::genreateChunksAround
			(glm::make_vec2(center.data()), dividedCount, glm::make_vec2(size.data()));
			(*terrainMask).insert(terrainMask->end(), newChunks.begin(), newChunks.end());
		}

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

	marl::schedule([=] {

		GenerationSystem genSys(std::move(chunks));

		genSys.generate();

		SR_INFO("Finished mesh gen process");
	});



}
