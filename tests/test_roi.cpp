#include "base.hpp"
#include "log.hpp"
#include "roimanager.hpp"

structlog LOGCFG = {};

void testRoiFromFile(string jsonFileName)
{
  SBGCK_TEST_BEGIN("testRoiFromFile");

  //Asset asset(assetFileName);
  //SBGCK_ASSERT_THROW(asset.getDefault().image.size().width != 0);

  SBGCK_TEST_END();
}


int main(int, char **)
{
  SBGCK_TEST_INIT();
  string board_json = CMAKE_SOURCE_DIR + string("/tests/images/board.json");

  // LOGCFG.prefix = (char *)"test_asset";
  // LOGCFG.headers = true;
  // LOGCFG.level = INFO;

  testRoiFromFile(board_json);
}