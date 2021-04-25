#include "base.hpp"
#include "log.hpp"
#include "asset.hpp"

structlog LOGCFG = {};

void testAssetFromFile(string assetFileName)
{
  SBGCK_TEST_BEGIN("testAssetFromFile");

  Asset asset(assetFileName);
  SBGCK_ASSERT_THROW(asset.getDefault().image.size().width != 0);

  SBGCK_TEST_END();
}


int main(int, char **)
{
  SBGCK_TEST_INIT();
  string board_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");
  string frame_png = CMAKE_SOURCE_DIR + string("/tests/images/frame.png");

  // LOGCFG.prefix = (char *)"test_asset";
  // LOGCFG.headers = true;
  // LOGCFG.level = INFO;

  testAssetFromFile(board_png);
}