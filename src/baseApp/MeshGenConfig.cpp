#include "mgpch.h"
#include "MeshGenConfig.h"


MeshGenConfig::ConfigValues MeshGenConfig::getorReset()
{
	
	
	if (FileManager::exists(path())) {
		return get();
	}
	else {
		auto vals = defaults();

		write(vals);
	}

}

MeshGenConfig::ConfigValues MeshGenConfig::get()
{

	auto valuestr = FileManager::loadStringfromFile(path());

	auto values = split_string(std::move(valuestr), "\n");

	SR_ASSERT(values.size() >= 3);

	ConfigValues settings{};

	settings.osmCashDir = values[0];
	settings.coastlineDir = values[1];
	settings.osmServerURL = values[2];

	return settings;
}

void MeshGenConfig::write(const ConfigValues& values)
{
	std::vector<std::string> vals;

	vals.push_back(values.osmCashDir);
	vals.push_back(values.coastlineDir);
	vals.push_back(values.osmServerURL);

	auto valstr = std::reduce(vals.begin(), vals.end(), "");

	FileManager::saveStringToFile(valstr,path());
}

MeshGenConfig::ConfigValues MeshGenConfig::defaults()
{
	ConfigValues vals{};

	vals.osmCashDir = FileManager::baseEngineResourceDir() + "osmCash/";
	vals.coastlineDir = FileManager::baseEngineResourceDir() + "osmCoastlines/justNewYorkArea-land-polygons-split-4326/";
	vals.osmServerURL = "http://localhost";

}

MeshGenConfig::MeshGenConfig()
{
}
