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
	};

	static ConfigValues getorReset();


protected:

	static ConfigValues get();
	static void write(const ConfigValues& values);
	static ConfigValues defaults();

	static std::string path() {return FileManager::appConfigDir() + "meshGen.cfg"; }


	MeshGenConfig();

};

