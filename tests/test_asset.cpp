#include "base.hpp"
#include "log.hpp"
#include "asset.hpp"

structlog LOGCFG = {};

void testOpenCVVersion()
{
  SBGCK_TEST_BEGIN("testOpenCVVersion");

  //  SBGCK_ASSERT_THROW(getOpenCVVersion() != NULL);

  SBGCK_TEST_END();
}

int main(int, char **)
{
  SBGCK_TEST_INIT();
  string board_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");
  string frame_png = CMAKE_SOURCE_DIR + string("/tests/images/frame.png");

  testOpenCVVersion();
}