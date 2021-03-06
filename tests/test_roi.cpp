#include <iostream>
#include <fstream>

#include "base.hpp"
#include "log.hpp"
#include "internal/roimanager.hpp"

structlog LOGCFG = {};

void testRoiFromFile(string jsonStr)
{
  SBGCK_TEST_BEGIN("testRoiFromFile");

  RoiManager rm;
  SBGCK_ASSERT_THROW(rm.initFromJsonString(jsonStr) == true);

  SBGCK_TEST_END();
}

void testPointInside(RoiManager &rm, Point &p, string name)
{
  SBGCK_TEST_BEGIN("testRectInside");

  string str;
  SBGCK_ASSERT_THROW(rm.isInsideRegion(p, str) == true);
  SBGCK_ASSERT_THROW(str == name);

  SBGCK_TEST_END();
}

void testPointOutside(RoiManager &rm, Point &p)
{
  SBGCK_TEST_BEGIN("testPointOutside");

  string str;
  SBGCK_ASSERT_THROW(rm.isInsideRegion(p, str) == false);
  SBGCK_ASSERT_THROW(str == "");

  SBGCK_TEST_END();
}

void testIsRegion(RoiManager &rm)
{
  SBGCK_TEST_BEGIN("testIsRegion");

  SBGCK_ASSERT_THROW(rm.isRegion("invalid") == false);
  SBGCK_ASSERT_THROW(rm.isRegion("#rect") == true);
  SBGCK_ASSERT_THROW(rm.isRegion("#convex") == true);

  SBGCK_TEST_END();
}

int main(int, char **)
{
  SBGCK_TEST_INIT();

  std::string jsonStr =
      "{"
      "    \"map\": ["
      "      {"
      "        \"shape\": \"rect\","
      "        \"coords\": \"56,35,303,273\","
      "        \"area\": \"#rect\""
      "      },"
      "      {"
      "        \"shape\": \"circle\","
      "        \"coords\": \"1075,159,125\","
      "        \"area\": \"#circle\""
      "      },"
      "      {"
      "        \"shape\": \"poly\","
      "        \"coords\": \"139,367,38,545,175,630,384,642,454,524,368,341\","
      "        \"area\": \"#convex\""
      "      },"
      "      {"
      "        \"shape\": \"poly\","
      "        \"coords\": \"583,688,649,543,805,425,955,458,1046,684,1183,386,839,320,601,46,660,348,425,239\","
      "        \"area\": \"#concave\""
      "      }"
      "    ],"
      "    \"name\": \"board\""
      "}";

  Point rectInside(180, 155);
  Point polyConvexInside(270, 560);
  Point polyConcaveInside(740, 370);
  Point polyConcaveOutside(830, 560);
  Point circleInside(1080, 160);
  Point outside(1200, 660);

  // test if we can parse the json
  testRoiFromFile(jsonStr);

  // use one rm to optimize speed
  RoiManager rm;
  rm.initFromJsonString(jsonStr);

  testPointInside(rm, rectInside, "#rect");
  testPointInside(rm, polyConvexInside, "#convex");
  testPointInside(rm, polyConcaveInside, "#concave");
  testPointInside(rm, circleInside, "#circle");

  testPointOutside(rm, polyConcaveOutside);
  testPointOutside(rm, outside);

  testIsRegion(rm);
}