#pragma once

#include "mgpch.h"

class DemoScene1: public Scene
{
public:

	using Scene::Scene;

	virtual void load();

	virtual void update();

	virtual void onDrawUI();

	virtual void unload();


	void lateLoad() override;

private:

	Entity cube;
	Entity testEnt;

};

