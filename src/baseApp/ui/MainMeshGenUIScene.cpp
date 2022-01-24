#include "mgpch.h"
#include "MainMeshGenUIScene.h"

#include <shellapi.h>

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
	terrainMask = new std::vector<math::Box>{ math::Box(playerLLA, glm::dvec2(0.01)) };

	WorldScene::load();
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



		ImGui::Spacing();

		if (ImGui::Button("RunGeneration")) {

		}
		
		ImGui::Text("Will generate %d chunks", 0);
		ImGui::SameLine();


		ImGui::Separator();

		ImGui::Text("Debug Draw:");

		if (ImGui::Button("Reload Current")) {
			reloadTerrainInMask();
		}

		if (ImGui::Button("Draw first in generation list")) {

		}

		if (ImGui::Button("Move camera to first loaded chunk")) {

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
}
