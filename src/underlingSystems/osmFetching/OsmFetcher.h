#pragma once

#include "Sunrise.h"

#include "osm.h"
#include <string>
#include "marl/scheduler.h"
#include "marl/task.h"
#include "marl/waitgroup.h"
#include "marl/ticket.h"
#include "marl/defer.h"

#include "../../generation/GenerationStatistics.h"

class OsmFetcher
{
public:

	OsmFetcher();


	osm::osm fetchChunk(Box frame, bool onlyUseOSMCash, ChunkGenerationStatistics& stats);

	marl::Ticket::Queue waitforServerQueue;
private:
	std::string getQuery(Box frame);
	std::string getTestQuery();

};

