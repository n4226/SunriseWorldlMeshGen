#pragma once

#include "mgpch.h"

//todo make a sunrise type config file to remove boilerplate of each config file

class MeshGenConfig
{
public:

	struct ConfigValues {
		std::string osmCashDir;
		std::string coastlineDir;
		std::string osmServerURL;
		bool onlyUseOsmCash;
		bool skipExisting;
	};

	static ConfigValues get();
	static void write(const ConfigValues& values);
	static void invalidateStored();


	static std::string path() {return FileManager::appConfigDir() + "meshGen.cfg"; }
protected:
	static ConfigValues getorReset();
	static ConfigValues load();

	static ConfigValues defaults();

	static bool invalidated;

	MeshGenConfig();

};

