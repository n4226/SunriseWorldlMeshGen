#include "mgpch.h"
#include "MeshGenConfig.h"

inline const char* const boolToString(bool b)
{
	return b ? "true" : "false";
}

inline bool stringToBool(std::string s)
{
	return s.compare("true") == 0 ? true : false;
}

MeshGenConfig::ConfigValues MeshGenConfig::getorReset()
{
	
	
	if (FileManager::exists(path())) {
		return get();
	}
	else {
		auto vals = defaults();

		write(vals);

		return vals;
	}

}

MeshGenConfig::ConfigValues MeshGenConfig::get()
{

	auto valuestr = FileManager::loadStringfromFile(path());

	auto values = split_string(std::move(valuestr), "\n");

	SR_ASSERT(values.size() >= 4);

	ConfigValues settings{};

	settings.osmCashDir = values[0];
	settings.coastlineDir = values[1];
	settings.osmServerURL = values[2];
	settings.onlyUseOsmCash = stringToBool(values[3]);

	return settings;
}

void MeshGenConfig::write(const ConfigValues& values)
{
	std::vector<std::string> vals;

	vals.push_back(values.osmCashDir);
	vals.push_back(values.coastlineDir);
	vals.push_back(values.osmServerURL);
	vals.push_back(boolToString(values.onlyUseOsmCash));

	std::string valstr;

	for (auto& val : vals) {
		valstr.append(val);
		valstr.append("\n");
	}

	FileManager::saveStringToFile(valstr,path());
}

MeshGenConfig::ConfigValues MeshGenConfig::defaults()
{
	ConfigValues vals{};

	vals.osmCashDir = FileManager::baseEngineResourceDir() + "osmCash/";
	vals.coastlineDir = FileManager::baseEngineResourceDir() + "osmCoastlines/justNewYorkArea-land-polygons-split-4326/";
	vals.osmServerURL = "http://localhost";
	vals.onlyUseOsmCash = false;

	return vals;
}

MeshGenConfig::MeshGenConfig()
{
}
