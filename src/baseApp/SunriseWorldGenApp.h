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

		config.marlThreadCountOffset = 1;
		config.enableAsioContext = false;
		config.vulkan = false;
		config.wantsWindows = false;

		return config;
	}


	ApplicationConfig configure() { 
		ApplicationConfig config{};

		config.enableMarl = true;
		config.vulkan = false;
		config.wantsWindows = false;
		config.enableAsioContextThread = false;
		config.enableAsioContext = false;
		config.useFileSys = false;

		return config;
	}
};


