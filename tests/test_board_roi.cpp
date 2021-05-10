#include <iostream>
#include <fstream>

#include "base.hpp"
#include "log.hpp"
#include "board.hpp"

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

void testBoardRoiMask(string boardFileName, string boardMapName)
{
  SBGCK_TEST_BEGIN("testBoardRoiMask");

  Board board;

  board.asset = Asset(boardFileName);
  board.roiManager.initFromJsonFile(boardMapName);

  Mat mask;
  bool maskStatus;

  mask = Mat(board.asset.getDefault().image.size().height, board.asset.getDefault().image.size().width, CV_8UC1, Scalar(0, 0, 0));
  maskStatus = board.roiManager.addToMask(mask, "invalid");
  SBGCK_ASSERT_THROW(maskStatus == false);

  // imshow("mask", mask);
  // waitKey(0);

  mask = Mat(board.asset.getDefault().image.size().height, board.asset.getDefault().image.size().width, CV_8UC1, Scalar(0, 0, 0));
  maskStatus = board.roiManager.addToMask(mask, "#bridge");
  SBGCK_ASSERT_THROW(maskStatus == true);

  // imshow("mask", mask);
  // waitKey(0);

  mask = Mat(board.asset.getDefault().image.size().height, board.asset.getDefault().image.size().width, CV_8UC1, Scalar(0, 0, 0));
  maskStatus = board.roiManager.addToMask(mask);
  SBGCK_ASSERT_THROW(maskStatus == true);

  // imshow("mask", mask);
  // waitKey(0);

  mask = Mat(board.asset.getDefault().image.size().height, board.asset.getDefault().image.size().width, CV_8UC1, Scalar(0, 0, 0));
  maskStatus = board.roiManager.addToMask(mask, "#bridge");
  SBGCK_ASSERT_THROW(maskStatus == true);
  maskStatus = board.roiManager.addToMask(mask, "#home1");
  SBGCK_ASSERT_THROW(maskStatus == true);
  maskStatus = board.roiManager.addToMask(mask, "#home2");
  SBGCK_ASSERT_THROW(maskStatus == true);

  // imshow("mask", mask);
  // waitKey(0);

  // // apply the mask
  // Mat region;
  // bitwise_and(board.asset.getDefault().image, board.asset.getDefault().image, region, mask);

  // imshow("region", region);
  // waitKey(0);

  SBGCK_TEST_END();
}

int main(int, char **)
{
  // board
  string board_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");
  string board_json = CMAKE_SOURCE_DIR + string("/tests/images/board.json");

  LOGCFG.prefix = (char *)"test_board_roi";
  LOGCFG.headers = true;
  LOGCFG.level = typelog::INFO;

  testBoardRoi(board_png, board_json);
  testBoardRoiMask(board_png, board_json);
}