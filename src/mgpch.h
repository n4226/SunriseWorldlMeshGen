#pragma once

#include "Sunrise.h"

// this is bad but i know it is so its ok :)
using namespace sunrise;
using namespace sunrise::math;


constexpr double radius = math::dEarthRad;

// local

#include "baseApp/MeshGenConfig.h"


// deifnes

#ifndef SR_PLATFORM_MACOS
#define USE_MARL 0
#else
#define USE_MARL 1
#endif



#ifndef SR_PLATFORM_MACOS
#define MG_EXECUION_POLOCY std::execution::par,
#else
#define MG_EXECUION_POLOCY
#endif



///////// system


#include <execution>
//#include <ranges> - only available in c++ 20+
#include <algorithm>

#include <iostream>
#include <memory>
#include <fstream>

#include <optional>

#include <string>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include <assert.h>
#include <numeric>


#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>

#include <filesystem>
