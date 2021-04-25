#include "base.hpp"
#include "log.hpp"
#include "asset.hpp"
#include "components/board.hpp"
#include "imagedetection.hpp"

structlog LOGCFG = {};

void testBoardFromFile(string boardFileName, string frameBoardEmptyFileName)
{
  SBGCK_TEST_BEGIN("testBoardFromFile");

  Board board;

  board.asset = Asset(boardFileName);
  board.frameBoardEmpty = imread(frameBoardEmptyFileName, IMREAD_UNCHANGED);

  SBGCK_ASSERT_THROW(board.asset .getDefault().image.size().width != 0);
  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width != 0);

  SBGCK_TEST_END();
}

void testDetectBoardInFrameDetection(string boardFileName, string frameFileName)
{
  SBGCK_TEST_BEGIN("testDetectBoardInFrameDetection");

  Mat frame = imread(frameFileName, IMREAD_UNCHANGED);

  Board board;
  board.asset = Asset(boardFileName);

  SBGCK_ASSERT_THROW(board.asset.getDefault().image.size().width != 0);
  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width == 0);

  Asset detectedBoard = ImageDetection::detectBoard(frame, board);
  SBGCK_ASSERT_THROW(detectedBoard.getDefault().image.size().width != 0);

  // the size of the detected board must match
  SBGCK_ASSERT_THROW(detectedBoard.getDefault().image.size().width == board.asset.getDefault().image.size().width);
  SBGCK_ASSERT_THROW(detectedBoard.getDefault().image.size().height == board.asset.getDefault().image.size().height);

  imshow("detectedBoard", detectedBoard.getDefault().image);
  waitKey();

  SBGCK_TEST_END();
}

int main(int, char **)
{
  SBGCK_TEST_INIT();

  // board
  string board_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");
  // in production we do this in the calibration step - here we use the same board
  string frameEmpty_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");
  // artifical frame
  string frame_png = CMAKE_SOURCE_DIR + string("/tests/images/frame.png");

  // LOGCFG.prefix = (char *)"test_asset";
  // LOGCFG.headers = true;
  // LOGCFG.level = INFO;

  testBoardFromFile(board_png, frameEmpty_png);

  testDetectBoardInFrameDetection(board_png, frame_png);
}