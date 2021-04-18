#include <opencv2/opencv.hpp>
#include <log.hpp>

#include "areamanager.hpp"


structlog LOGCFG = {};

int main(int argc, char **argv)
{
    LOGCFG.prefix = (char *)"main_json_maps";
    LOGCFG.headers = true;
    LOGCFG.level = DEBUG;

    std::string jsonStr =
        "{"
        "    \"map\": ["
        "      {"
        "        \"shape\": \"poly\","
        "        \"coords\": \"146,278,149,488,235,488,234,289,220,278\","
        "        \"area\": \"#storageRoom\""
        "      },"
        "      {"
        "        \"shape\": \"rect\","
        "        \"coords\": \"249,376,489,587\","
        "        \"area\": \"#livingRoom\""
        "      },"
        "      {"
        "        \"shape\": \"poly\","
        "        \"coords\": \"230,503,114,504,115,679,300,684,301,609,227,607\","
        "        \"area\": \"#yard\""
        "      },"
        "      {"
        "        \"shape\": \"circle\","
        "        \"coords\": \"112,144,47\","
        "        \"area\": \"#bridge\""
        "      },"
        "      {"
        "        \"shape\": \"rect\","
        "        \"coords\": \"883,212,1040,374\","
        "        \"area\": \"#controlPost\""
        "      }"
        "    ],"
        "    \"name\": \"Arctic-base\""
        "}";

    AreaManager::initFromJsonString(jsonStr);

    return 0;
}
