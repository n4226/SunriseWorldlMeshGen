#include "mgpch.h"
#include "TestMGPUScene.h"




TestMGPUScene::TestMGPUScene(Application* app)
	: Scene(app)
{
	coordinatorCreator = ([this](auto renderer) {
		return new TestMGPUCoord(this, renderer);
	});
}

void TestMGPUScene::load()
{
	Scene::load();
}

void TestMGPUScene::update()
{
	Scene::update();
}

void TestMGPUScene::unload()
{
	Scene::unload();
}


void TestMGPUCoord::createPasses()
{
	generateImguiStage = true;

	auto stage = new EmptyStage(this,"empty");

	registerStage(stage, {}, {}, {});
	setLastStage(stage);

	

}

sunrise::gfx::ComposableRenderPass::CreateOptions TestMGPUCoord::renderpassConfig(vk::Format swapChainFormat)
{

	auto deferredA = gfx::ComposableRenderPass::CreateOptions::VAttatchment();

	deferredA.type = gfx::ComposableRenderPass::CreateOptions::AttatchmentType::Color;
	deferredA.format = swapChainFormat;
	deferredA.loadOp = vk::AttachmentLoadOp::eClear;
	deferredA.initialLayout = vk::ImageLayout::eUndefined;
	deferredA.transitionalToAtStartLayout = vk::ImageLayout::eColorAttachmentOptimal;
	//deferredA.transitionalToAtStartLayout = vk::ImageLayout::eColorAttachmentOptimal;//vk::ImageLayout::eColorAttachmentOptimal;
	deferredA.finalLayout = vk::ImageLayout::ePresentSrcKHR;
	//glm::vec3 selectedColor{ 13, 63, 143 };
	glm::vec3 selectedColor{ 11, 50, 114 };
	selectedColor /= 255;
	deferredA.clearColor = { selectedColor.r, selectedColor.g, selectedColor.b, 1.0f };
	deferredA.name = "FinalRenderTarget";

	gfx::ComposableRenderPass::CreateOptions options = { {deferredA}, 0 };

	return options;
}

void EmptyStage::setup()
{
}

void EmptyStage::cleanup()
{
}

vk::CommandBuffer* EmptyStage::encode(RunOptions options)
{
	auto buff = selectAndSetupCommandBuff(options);
	buff->end();

	return buff;
}
