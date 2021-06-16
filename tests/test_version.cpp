#include "base.hpp"
#include "log.hpp"
#include "version.hpp"

structlog LOGCFG = {};

void testOpenCVVersion()
{
  SBGCK_TEST_BEGIN("testOpenCVVersion");

  SBGCK_ASSERT_THROW(getOpenCVVersion() != NULL);

  SBGCK_TEST_END();
}

int main(int, char **)
{
  SBGCK_TEST_INIT();

  LOGCFG.prefix = (char *)"test_version";
  LOGCFG.headers = true;
  LOGCFG.level = typelog::INFO;

  testOpenCVVersion();
}