#include "base.hpp"
#include "log.hpp"
#include "asset.hpp"
#include "components/board.hpp"
#include "imagediff.hpp"

structlog LOGCFG = {};

// void testBackgroundRemoval(string boardEmptyFileName, string frameBoardEmptyFileName)
// {
//   SBGCK_TEST_BEGIN("testBackgroundRemoval");

//   Board board;
//   board.asset = Asset(boardEmptyFileName);
//   board.frameBoardEmpty = imread(frameBoardEmptyFileName, IMREAD_COLOR);

//   SBGCK_ASSERT_THROW(board.asset.getDefault().image.size().width != 0);
//   SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width != 0);

//   // wrong
//   Mat diff = ImageDiff::removeBackground(board.asset.getDefault().image, board.frameBoardEmpty);

//   // imshow("board", board.asset.getDefault().image);
//   // imshow("diff", diff);
//   // waitKey();

//   SBGCK_TEST_END();
// }

void xtestBackgroundRemoval(string boardEmptyFileName, string frameBoardWithTokenFileName)
{
  SBGCK_TEST_BEGIN("xtestBackgroundRemoval");

  Board board;
  board.frameBoardEmpty = imread(boardEmptyFileName, IMREAD_COLOR);
  Mat frameBoardWithToken = imread(frameBoardWithTokenFileName, IMREAD_COLOR);

  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width != 0);
  SBGCK_ASSERT_THROW(frameBoardWithToken.size().width != 0);

  Mat diff = ImageDiff::removeBackground(frameBoardWithToken, board.frameBoardEmpty);

  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width == diff.size().width);
  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().height == diff.size().height);

  // imshow("frameBoardEmpty", board.frameBoardEmpty);
  // imshow("frameBoardWithToken", frameBoardWithToken);
  // imshow("diff", diff);
  // waitKey();

  imwrite("./diff.png", diff);

  SBGCK_TEST_END();
}

int main(int, char **)
{
  SBGCK_TEST_INIT();

  // board
  string boardEmpty_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");
  // in production we do this in the calibration step - here we use the same board
  string frameEmpty_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");
  // in production we do this in the calibration step - here we use the same board
  string frame_token_bridge_png = CMAKE_SOURCE_DIR + string("/tests/images/frame_token_bridge.png");

  LOGCFG.prefix = (char *)"test_token";
  LOGCFG.headers = true;
  LOGCFG.level = INFO;

  //testBackgroundRemoval(boardEmpty_png, frameEmpty_png);
  xtestBackgroundRemoval(boardEmpty_png, frame_token_bridge_png);
}
