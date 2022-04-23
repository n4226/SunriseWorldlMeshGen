#pragma once

#include "mgpch.h"

class TestMGPUScene : public sunrise::Scene {



public:

	TestMGPUScene(Application* app);

	void load() override;


	void update() override;


	void unload() override;

};

class TestMGPUCoord : public gfx::SceneRenderCoordinator {



public:
	using gfx::SceneRenderCoordinator::SceneRenderCoordinator;

	void createPasses() override;

protected:
	gfx::ComposableRenderPass::CreateOptions renderpassConfig(vk::Format swapChainFormat) override;

};

class EmptyStage : public gfx::GPURenderStage {

public:

	using gfx::GPURenderStage::GPURenderStage;

protected:
	void setup() override;


	void cleanup() override;


	vk::CommandBuffer* encode(RunOptions options) override;

};