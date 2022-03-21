#include <mgpch.h>
#include "SimpleCameraMovement.h"


void SimpleCameraMovement::setup()
{
	
}

void SimpleCameraMovement::update()
{
	if (scene->app.imguiValid())
		if (ImGui::IsAnyItemActive())
			return;

	auto movement = glm::vec3(0, 0, 0);
	if (scene->app.getKey(GLFW_KEY_W)) {
		movement += glm::vec3(0, 0, 1);
	}
	if (scene->app.getKey(GLFW_KEY_S)) {
		movement += glm::vec3(0, 0, -1);

	}
	if (scene->app.getKey(GLFW_KEY_A)) {
		movement += glm::vec3(-1, 0, 0);

	}
	if (scene->app.getKey(GLFW_KEY_D)) {
		movement += glm::vec3(1, 0, 0);
	}


	//movement *= 0.0001 * 5;

	if (scene->app.getKey(GLFW_KEY_Q)) {
		//movement *= 10;
		movement += glm::vec3(0, -1, 0);
	}

	if (scene->app.getKey(GLFW_KEY_E)) {
		movement += glm::vec3(0, 1, 0);
		//movement /= 10;
	}

	if (scene->app.getKey(GLFW_KEY_LEFT_SHIFT)) {
		movement *= 10;
	}

	if (scene->app.getKey(GLFW_KEY_LEFT_CONTROL)) {
		movement /= 10;
	}

	movement *= std::min(scene->deltaTimef,1.f);

	movement *= 10;

	auto sensativity = glm::dvec2(1, 1) * 60.0 * 180.0 * 20.0 * 0.7;

	if (scene->app.mouseLeft) {
		auto mouseDelta = scene->app.mousePosFrameDelta * scene->deltaTime * sensativity;
		manualMouseTranslation += mouseDelta * scene->deltaTime * 40.0 * 10.0;
	}

	//SR_CORE_INFO("mouseDelta: {},{}", world->app.mousePosFrameDelta.x, world->app.mousePosFrameDelta.y);

	movement = 
				glm::angleAxis(glm::radians((float)(manualMouseTranslation.x)), glm::vec3(0, 1, 0)) *
				glm::angleAxis(glm::radians((float)(manualMouseTranslation.y)), glm::vec3(1, 0, 0)) *
		movement;
	for (size_t i = 0; i < scene->app.windows.size(); i++)
	{
		auto& window = scene->app.windows[i];

		auto camTrans = configSystem.global().cameras[i];


		window->camera.transform.rotation =
			//glm::angleAxis(glm::radians(90.f), glm::vec3(-1, 0, 0)) *
			glm::angleAxis(glm::radians((float)(manualMouseTranslation.x)), glm::vec3(0, 1, 0)) *
			glm::angleAxis(glm::radians((float)(manualMouseTranslation.y)), glm::vec3(1, 0, 0));


		window->camera.transform.position += movement;

	
	}
}

void SimpleCameraMovement::cleanup()
{
	
}
