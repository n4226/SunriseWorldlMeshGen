#include "mgpch.h"
#include "DemoScene1.h"
#include "Sunrise/math/mesh/MeshPrimatives.h"
#include "systems/SimpleCameraMovement.h"


void DemoScene1::load()
{
	Scene::load();
	
	sunLL.x = 70;
	systems.push_back(new SimpleCameraMovement());

	


	MeshRenderer mRenderer{ 3 };

	Transform testEntTranse;
	testEntTranse.position = { 0,0,0 };
	testEntTranse.scale = { 1,1,1 };

	testEnt = registry.create();
	registry.emplace<Transform>(testEnt, testEntTranse);
	registry.emplace<Mesh>(testEnt, MeshPrimatives::cube());
	registry.emplace<MeshRenderer>(testEnt, MeshRenderer(9));


	floor = registry.create();
	registry.emplace<Transform>(floor);
	registry.emplace<Mesh>(floor, MeshPrimatives::square());
	registry.emplace<MeshRenderer>(floor, MeshRenderer(1));

	auto& mesh2 = registry.get<Mesh>(floor);
	
	for (int i = 0; i < mesh2.uvs.size(); i++)
	{
		//mesh2.uvs[i] *= 50;
	}

	auto& floorTransform = registry.get<Transform>(floor);
	floorTransform.position.z = 0;
	floorTransform.position.y = -0.5;
	floorTransform.scale *= 10;
	floorTransform.scale.z = 1;
	floorTransform.rotation = glm::angleAxis(glm::radians((180.f + 90.f) * 1), glm::vec3(1, 0, 0));



	Transform sunCubeTrans{};
	sunCubeTrans.position = { 0,0,0 };
	sunCubeTrans.scale *= 1;

	sunCube = registry.create();
	registry.emplace<Transform>(sunCube, sunCubeTrans);
	registry.emplace<Mesh>(sunCube, MeshPrimatives::cube());
	registry.emplace<MeshRenderer>(sunCube, MeshRenderer(3));


	//auto fenceMesh = Mesh();

	/* create path - then extrude path
	*/
	//auto path = {};

	/*Transform trans2{};
	trans2.position = { 0,0,0 };
	trans2.scale *= 0.1;

	auto fence = registry.create();
	registry.emplace<Transform>(fence, trans2);
	registry.emplace<Mesh>(fence, MeshPrimatives::cube());
	registry.emplace<MeshRenderer>(fence, MeshRenderer(3));*/


}

void DemoScene1::lateLoad()
{
	Scene::lateLoad();

	
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

	
	auto& cubeTrans = registry.get<Transform>(sunCube);
	cubeTrans.position = math::LlatoGeo({ sunLL,0 }, glm::dvec3(), 20);

	auto& testCubeTrans = registry.get<Transform>(testEnt);
	auto& testCubeMesh = registry.get<Mesh>(testEnt);
	testCubeTrans.rotation *= glm::angleAxis(glm::radians(0.01f), glm::vec3(0,1,0));

	auto& floorMesh = registry.get<Mesh>(floor);
	auto& floorTrans = registry.get<Transform>(floor);


	for (auto ren : app.renderers) {
		testCubeMesh.debugDrawNormals(ren, testCubeTrans);
		//floorMesh.debugDrawNormals(ren, floorTrans);
	}
		//ren->debugDraw->drawLine({ -10,-10,-10 }, { 10,10,10 });

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
