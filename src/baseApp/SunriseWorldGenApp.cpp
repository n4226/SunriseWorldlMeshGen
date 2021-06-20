#include "mgpch.h"
#include "SunriseWorldGenApp.h"
#include "Sunrise/Sunrise/core/EntryPoint.h"

#include "MeshGenConfig.h"

#include "configure tasks/ConfigureTasksScript.h"

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
	: Application(nullptr)
{
}

SunriseWorldGenApp::~SunriseWorldGenApp()
{
}

const char* SunriseWorldGenApp::getName()
{
	return "World Gen";
}

void SunriseWorldGenApp::startup()
{
	Application::startup();

}

void SunriseWorldGenApp::run()
{
	Application::run();

	/*SR_INFO("Running");

	Sleep(300);
	printf("Hello World!\n");

	Sleep(300);
	printf("Welcome to super secret program 2\n");
	Sleep(500);
	printf("Don't ask what happened to the first one\n");

	Sleep(1000);
	
	printf("\nEnter your name to receive your free cake!\n");

	std::cin.get();*/

	printf("\n\nWelcome to mesh gen\n");


	printf("Will generate mesh to configured folder, press return to accept\n");

	std::cin.get();

	SR_INFO("Starting mesh gen");

	MeshGenConfig::getorReset();


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