#include "base.hpp"
#include "log.hpp"

#include "runtimeprop.hpp"

#include <nlohmann/json.hpp>

using namespace nlohmann;

structlog LOGCFG = {};

void testEmptyRuntimeProp()
{
  SBGCK_TEST_BEGIN("testEmptyRuntimeProp");

  RuntimeProp::init("{}");

  SBGCK_ASSERT_THROW(RuntimeProp::getString("string", "my string") == "my string");
  SBGCK_ASSERT_THROW(RuntimeProp::getInt("integer", 17) == 17);
  SBGCK_ASSERT_THROW(RuntimeProp::getDouble("double", 123.45) == 123.45);
  SBGCK_ASSERT_THROW(RuntimeProp::getBool("boolean", true) == true);

  SBGCK_TEST_END();
}

void testSimpleRuntimeProp()
{
  SBGCK_TEST_BEGIN("testSimpleRuntimeProp");

  json j_input = R"(
    {
      "string": "string value",
      "integer": 31,
      "double": 123.51,
      "boolean": false
    }
  )"_json;
  string input = j_input.dump();

  RuntimeProp::init(input);

  SBGCK_ASSERT_THROW(RuntimeProp::getString("string", "") == "string value");
  SBGCK_ASSERT_THROW(RuntimeProp::getInt("integer", 0) == 31);
  SBGCK_ASSERT_THROW(RuntimeProp::getDouble("double", 0.0) == 123.51);
  SBGCK_ASSERT_THROW(RuntimeProp::getBool("boolean", true) == false);

  SBGCK_TEST_END();
}

void testNestedRuntimeProp()
{
  SBGCK_TEST_BEGIN("testNestedRuntimeProp");

  json j_input = R"(
    {
      "foo": {
        "bar": {
          "item": {
            "string": "string value",
            "integer": 31,
            "double": 123.51,
            "boolean": false
          }
        }
      }
    }
  )"_json;
  string input = j_input.dump();

  RuntimeProp::init(input);

  SBGCK_ASSERT_THROW(RuntimeProp::getString("foo.bar.item.string", "") == "string value");
  SBGCK_ASSERT_THROW(RuntimeProp::getInt("foo.bar.item.integer", 0) == 31);
  SBGCK_ASSERT_THROW(RuntimeProp::getDouble("foo.bar.item.double", 0.0) == 123.51);
  SBGCK_ASSERT_THROW(RuntimeProp::getBool("foo.bar.item.boolean", true) == false);

  SBGCK_TEST_END();
}

void testInvalidRuntimeProp()
{
  SBGCK_TEST_BEGIN("testInvalidRuntimeProp");

  json j_input = R"(
    {
      "foo": {
        "bar": {
          "item": {
            "string": "cheese cake",
            "integer": 17,
            "double": -1.3,
            "boolean": true
          }
        }
      }
    }
  )"_json;
  string input = j_input.dump();

  RuntimeProp::init(input);

  SBGCK_ASSERT_THROW(RuntimeProp::getString("foo.bar.item.string", "") != "string value");
  SBGCK_ASSERT_THROW(RuntimeProp::getInt("foo.bar.item.integer", 0) != 31);
  SBGCK_ASSERT_THROW(RuntimeProp::getDouble("foo.bar.item.double", 0.0) != 123.51);
  SBGCK_ASSERT_THROW(RuntimeProp::getBool("foo.bar.item.boolean", true) != false);

  // this will give the default values
  SBGCK_ASSERT_THROW(RuntimeProp::getString("string", "my string") == "my string");
  SBGCK_ASSERT_THROW(RuntimeProp::getInt("whatever.integer", 17) == 17);
  SBGCK_ASSERT_THROW(RuntimeProp::getDouble("foo.bar.item.whatever", 123.45) == 123.45);
  SBGCK_ASSERT_THROW(RuntimeProp::getBool("boolean", true) == true);

  SBGCK_TEST_END();
}

int main(int, char **)
{
  SBGCK_TEST_INIT();

  LOGCFG.prefix = (char *)"test_runtimeprop";
  LOGCFG.headers = true;
  LOGCFG.level = typelog::INFO;

  testEmptyRuntimeProp();
  testSimpleRuntimeProp();
  testNestedRuntimeProp();
  testInvalidRuntimeProp();
}
