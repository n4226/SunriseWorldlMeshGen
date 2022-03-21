#pragma once

#include "mgpch.h"

class SimpleCameraMovement: public System
{
public:

	void update() override;


	void setup() override;


	void cleanup() override;

protected:

	glm::dvec2 manualMouseTranslation = glm::dvec2(0, 0);
private:
};