#pragma once

#include "Sunrise.h"

class SunriseWorldGenApp : public sunrise::Application
{
public:
	SunriseWorldGenApp();
	~SunriseWorldGenApp();

	const char* getName() override;

	void startup() override;
	void run() override;
	void shutdown() override;

protected:

	ApplicationConfig configure() override {
		ApplicationConfig config{};

		config.enableMarl = true;
		config.marlThreadCountOffset = 1;
		config.vulkan = true;
		config.wantsWindows = false;
		config.enableAsioContextThread = false;
		config.enableAsioContext = false;
		config.useFileSys = true;

		return config;
	}
};


