#pragma once

#include "Sunrise.h"


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
	virtual void onDrawMainMenu() override;

	virtual void unload() override;

private:

	bool showGenerationWindow = true;

	glm::dvec2 defaultCenter = glm::dvec2(40.75699, -73.92144);

};

