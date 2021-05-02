#include <iostream>
#include <fstream>

#include "base.hpp"
#include "log.hpp"
#include "components/board.hpp"

structlog LOGCFG = {};

void testBoardRoi(string boardFileName, string boardMapName)
{
  SBGCK_TEST_BEGIN("testBoardRoi");

  Board board;

  board.asset = Asset(boardFileName);
  board.roiManager.initFromJsonFile(boardMapName);

  SBGCK_ASSERT_THROW(board.asset.getDefault().image.size().width != 0);


  SBGCK_TEST_END();
}

int main(int, char **)
{
  // board
  string board_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");
  string board_json = CMAKE_SOURCE_DIR + string("/tests/images/board.json");

  LOGCFG.prefix = (char *)"test_board_roi";
  LOGCFG.headers = true;
  LOGCFG.level = INFO;

  testBoardRoi(board_png, board_json);
}