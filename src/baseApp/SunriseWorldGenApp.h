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
		config.marlThreadCountOffset = 2;
		config.vulkan = true;
		config.wantsWindows = true;
		config.enableAsioContextThread = true;
		config.enableAsioContext = true;
		config.useFileSys = true;

		config.wantsHighThreadPriority = false;

		return config;
	}
};


