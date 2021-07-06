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

	//bool wantsWindows() override { return false; };


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


