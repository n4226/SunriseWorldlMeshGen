#include "mgpch.h"
#include "OsmFetcher.h"


#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <fstream>

#include <stdio.h>

#include "../../baseApp/MeshGenConfig.h"

#include "httplib.h"



OsmFetcher::OsmFetcher()
{
}

osm::osm OsmFetcher::fetchChunk(Box frame, bool onlyUseOSMCash, ChunkGenerationStatistics& stats)
{
#if !SERVER_LOCAL && USE_MARL
    auto ticket = waitforServerQueue.take();
#endif

    auto file = MeshGenConfig::get().osmCashDir + frame.toString() + ".osm";

    // check if in local cash if so return that
    printf("attempting to open osm cash file\n");
    std::ifstream f(file);
    if (f.good()) {
        try {
            stats.markOsmJSONReceived();
            printf("using osm cash to load osm\n");
            std::string str((std::istreambuf_iterator<char>(f)),
                std::istreambuf_iterator<char>());
            if (str.substr(0, 5) == "<?xml") {
                remove(file.c_str());
                throw std::runtime_error("osm data not json");
            }
            auto osmData = osm::makeOSM(str);
            stats.markOSMParsedFromJSON();
            return osmData;
        }
        catch (const std::exception& e) {
            if (onlyUseOSMCash)
                throw e;
        }
    }
    else if (onlyUseOSMCash) {
        SR_ERROR("Osm not in cash so loading chunk with empty osm");
        stats.markOsmJSONReceived();
        auto osmData = osm::osm{};
        stats.markOSMParsedFromJSON();
        return osmData;
        //throw std::runtime_error("not in osm cash");
    }
#if !SERVER_LOCAL && USE_MARL
    ticket.wait();
    defer(ticket.done());
#endif
    printf("fetching osm from server\n");
    //get from server 

    auto server = MeshGenConfig::get().osmServerURL;

    httplib::Client cli(server.c_str());//();

    cli.set_write_timeout(100, 0);
    cli.set_read_timeout(100, 0);
    cli.set_connection_timeout(100, 0);
    cli.set_keep_alive(true);

    auto str = getQuery(frame);

    auto response = cli.Post("/api/interpreter", str, "application/xml");

    //std::cout << response->body << "\n";

    //update cash - raw osm

    if (response == nullptr || response.error() == httplib::Error::Read)
        throw std::runtime_error("read error");
    if (response->body.empty())
        throw std::runtime_error("osmEmpty _ osm server is probably not running");

    {
        std::ofstream out;
        out.open(file, std::fstream::out);
        //out.open(file, std::fstream::out);
        out << response->body;
        out.close();
    }

    stats.markOsmJSONReceived();
    // pars to osm
    printf("parsing osm from json\n");

    osm::osm parsedOsm = osm::makeOSM(response->body);

    stats.markOSMParsedFromJSON();

    if (strcmp(server.c_str(), "http://127.0.0.1") != 0 && strcmp(server.c_str(), "http://localhost") != 0) {
        printf("sleeping with json data\n");
        Sleep(8000);
    }
    else {
        printf("not sleeping\n");
        Sleep(10);
    }

    return parsedOsm;
}


std::string OsmFetcher::getQuery(Box frame)
{
    // full osm data
    //timeout used to be 25
    const char* str = R"(
        <osm-script output="json" output-config="" timeout="200">
        <union into="_">
        <bbox-query s="%f" w="%f" n="%f" e="%f"/>
        <query into="_" type="way">
        <bbox-query s="%f" w="%f" n="%f" e="%f"/>
        </query>
        <query into="_" type="relation">
        <bbox-query s="%f" w="%f" n="%f" e="%f"/>
        </query>
        </union>
        <print e="" from="_" geometry="skeleton" ids="yes" limit="" mode="body" n="" order="id" s="" w=""/>
        <recurse from="_" into="_" type="down"/>
        <print e="" from="_" geometry="skeleton" ids="yes" limit="" mode="skeleton" n="" order="quadtile" s="" w=""/>
        </osm-script>
        )";
    // old only buildings
  /*  const char* str = R"(
        <osm-script output="json" output-config="" timeout="25">
        <union into="_">
        <query into="_" type="way">
        <has-kv k="building" modv="" v=""/>
        <bbox-query s="%f" w="%f" n="%f" e="%f"/>
        </query>
        <query into="_" type="relation">
        <has-kv k="building" modv = "" v = ""/>
        <bbox-query s="%f" w="%f" n="%f" e="%f"/>
        </query>
        </union>
        <print e="" from="_" geometry="skeleton" ids="yes" limit="" mode="body" n="" order="id" s="" w=""/>
        <recurse from="_" into="_" type="down"/>
        <print e="" from="_" geometry="skeleton" ids="yes" limit="" mode="skeleton" n="" order="quadtile" s="" w=""/>
        </osm-script>
        )";*/

    char buffer[1'000];

    sprintf_s(buffer, str, frame.start.x, frame.start.y, frame.getEnd().x, frame.getEnd().y, frame.start.x, frame.start.y, frame.getEnd().x, frame.getEnd().y, frame.start.x, frame.start.y, frame.getEnd().x, frame.getEnd().y);

    return std::string(buffer);

}

std::string OsmFetcher::getTestQuery()
{
    return
        R"(
        < osm - script output = "json" output - config = "" timeout = "25" >
        <union into = "_">
        <query into = "_" type = "way">
        <has - kv k = "building" modv = "" v = "" / >
        <bbox - query s = "40.610319941413" w = "-74.039182662964" n = "40.618635305267" e = "-74.027112722397" / >
        < / query>
        <query into = "_" type = "relation">
        <has - kv k = "building" modv = "" v = "" / >
        <bbox - query s = "40.610319941413" w = "-74.039182662964" n = "40.618635305267" e = "-74.027112722397" / >
        < / query>
        < / union>
        <print e = "" from = "_" geometry = "skeleton" ids = "yes" limit = "" mode = "body" n = "" order = "id" s = "" w = "" / >
        <recurse from = "_" into = "_" type = "down" / >
        <print e = "" from = "_" geometry = "skeleton" ids = "yes" limit = "" mode = "skeleton" n = "" order = "quadtile" s = "" w = "" / >
        < / osm - script>
        )";
}
