#include "mgpch.h"
#include "SunriseWorldGenApp.h"
#include "Sunrise/Sunrise/core/EntryPoint.h"

#include "MeshGenConfig.h"

#include "configure tasks/ConfigureTasksScript.h"

#include "ui/MainMeshGenUIScene.h"

#include "../testingScenes/DemoScene1.h"

#include "../test/TestMGPUScene.h"

using namespace sunrise;


class EmptyScene : public sunrise::Scene {
public:

	using sunrise::Scene::Scene;

	void load() override {

	}

	virtual void update() override {

	}

	virtual void unload() override {

	}
};


SunriseWorldGenApp::SunriseWorldGenApp()
	//: Application(new MainMeshGenUIScene(this))
	: Application(new WorldScene(this))

	//: Application(new TestMGPUScene(this))
	//: Application(new DemoScene1(this))
{
}

SunriseWorldGenApp::~SunriseWorldGenApp()
{
}

const char* SunriseWorldGenApp::getName()
{
	return "Sunrise World Generation Utility";
}

void SunriseWorldGenApp::startup()
{
	Application::startup();

}

void SunriseWorldGenApp::run()
{
	Application::run();

	return;

	printf("\n\nWelcome to mesh gen\n");


	printf("Will generate mesh to configured folder, press return to accept\n");

	std::cin.get();

	SR_INFO("Starting mesh gen");

	MeshGenConfig::get();


	ConfigureTasksScript::runScript();
}

void SunriseWorldGenApp::shutdown()
{
	Application::shutdown();
}


// entrypoint

sunrise::Application* sunrise::CreateApplication() {
	return new SunriseWorldGenApp();
	//return new NO_APPLICATION();
}