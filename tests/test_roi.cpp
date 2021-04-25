#include <iostream>
#include <fstream>

#include "base.hpp"
#include "log.hpp"
#include "roimanager.hpp"

structlog LOGCFG = {};

void testRoiFromFile(string jsonStr)
{
  SBGCK_TEST_BEGIN("testRoiFromFile");

  RoiManager rm;
  rm.initFromJsonString(jsonStr);

  //Asset asset(assetFileName);
  //SBGCK_ASSERT_THROW(asset.getDefault().image.size().width != 0);

  SBGCK_TEST_END();
}

int main(int, char **)
{
  SBGCK_TEST_INIT();

  std::string jsonStr =
      "{"
      "  \"map\": ["
      "    {"
      "      \"shape\": \"rect\","
      "      \"coords\": \"131,120,378,358\","
      "      \"area\": \"#rect\""
      "    },"
      "    {"
      "      \"shape\": \"circle\","
      "      \"coords\": \"1155,224,125\","
      "      \"area\": \"#circle\""
      "    },"
      "    {"
      "      \"shape\": \"poly\","
      "      \"coords\": \"125,442,24,620,109,851,410,853,557,603,354,416\","
      "      \"area\": \"#convex\""
      "    },"
      "    {"
      "      \"shape\": \"poly\","
      "      \"coords\": \"744,719,849,658,1045,556,1237,650,1429,806,1480,412,1028,375,801,161,860,463,515,384\","
      "      \"area\": \"#concave\""
      "    }"
      "  ],"
      "  \"name\": \"board\""
      "}";

  // LOGCFG.prefix = (char *)"test_asset";
  // LOGCFG.headers = true;
  // LOGCFG.level = INFO;

  testRoiFromFile(jsonStr);
}