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

	bool wantsWindows() override { return false; };

};


