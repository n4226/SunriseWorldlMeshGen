#include "mgpch.h"
#include "DemoScene1.h"
#include "Sunrise/math/mesh/MeshPrimatives.h"
#include "systems/SimpleCameraMovement.h"


void DemoScene1::load()
{
	Scene::load();
	
	sunLL.x = 70;

	MeshRenderer mRenderer{ 1 };

	testEnt = registry.create();
	registry.emplace<Transform>(testEnt);
	registry.emplace<Mesh>(testEnt, MeshPrimatives::cube());
	registry.emplace<MeshRenderer>(testEnt, mRenderer);

	auto& transform = registry.get<Transform>(testEnt);
	transform.scale = { 1,1,1 };
	transform.position.z = 10;
	transform.position.y = 0;
	transform.rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(1, 0, 0));

	Entity testEnt2 = registry.create();
	registry.emplace<Transform>(testEnt2);
	registry.emplace<Mesh>(testEnt2, MeshPrimatives::square());
	registry.emplace<MeshRenderer>(testEnt2, MeshRenderer(1));

	auto& mesh2 = registry.get<Mesh>(testEnt2);
	
	for (int i = 0; i < mesh2.uvs.size(); i++)
	{
		mesh2.uvs[i] *= 500;
	}

	auto& transform2 = registry.get<Transform>(testEnt2);
	transform2.position.z = 0;
	transform2.position.y = -2;
	transform2.scale *= 100;
	transform2.scale.z = 1;
	transform2.rotation = glm::angleAxis(glm::radians((180.f + 90.f) * 1), glm::vec3(1, 0, 0));

	systems.push_back(new SimpleCameraMovement());


	Transform trans{};
	trans.position.z = 10;
	trans.position.y = -1;
	trans.scale *= 10;

	cube = registry.create();
	registry.emplace<Transform>(cube, trans);
	registry.emplace<Mesh>(cube, MeshPrimatives::cube());
	registry.emplace<MeshRenderer>(cube, MeshRenderer(3));
}

void DemoScene1::lateLoad()
{
	Scene::lateLoad();

	for (auto ren : app.renderers)
		ren->materialManager->loadStaticEarth();
}

void DemoScene1::update()
{
	Scene::update();

	//TODO: switch to all windows?
	for (size_t i = 0; i < app.windows.size(); i++)
	{
		auto& window = app.windows[i];

		auto camTrans = configSystem.global().cameras[i];

		auto mat = window->camera.view();
		auto m1 = mat;
		//window->camera.transform.position = { 0,0,sin(timef) * 20};
		//window->camera.transform.rotation = glm::qua<glm::float32>({ 0,timef,0 });
	}

	
	auto& cubeTrans = registry.get<Transform>(cube);
	auto& testTrans = registry.get<Transform>(testEnt);

	//cubeTrans.position.y = sin(timef) * 1;
	cubeTrans.position = math::LlatoGeo({ sunLL,0 }, glm::dvec3(), 100);

	//testTrans.position.y = sin(timef) * 1.5;
	//testTrans.rotation = glm::angleAxis(glm::radians(timef * 10), glm::vec3(0, 1, 0));
}

void DemoScene1::onDrawUI()
{
	ImGui::Begin("Rot");

	ImGui::InputFloat3("pos", &app.windows[0]->camera.transform.position.x);
	ImGui::InputFloat4("rot", &app.windows[0]->camera.transform.rotation.x);

	ImGui::SliderFloat2("Sun Lat Lon", &sunLL.x, -180, 180);

	ImGui::End();
}

void DemoScene1::unload()
{
	Scene::unload();
}
