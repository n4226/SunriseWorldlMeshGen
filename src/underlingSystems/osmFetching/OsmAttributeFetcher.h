#pragma once

#include "mgpch.h"
#include "osm.h"

class OsmAttributeFetcher
{
public:

	static double buildingHeight(const osm::element& building);

	template<typename T>
	static T tryGetValue(const osm::element& element, const std::string& key, T defaultValue);

};

template<typename T>
inline T OsmAttributeFetcher::tryGetValue(const osm::element& element, const std::string& key, T defaultValue)
{
	static_assert(std::is_same<double, T>::value);

	T val;

	if (element.tags.count(key)) {
		try {
			val = std::stod(element.tags.at(key));
		}
		catch (...) { val = defaultValue; }
	}
	else val = defaultValue;


	return val;

}
