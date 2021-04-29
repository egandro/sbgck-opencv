#include "base.hpp"
#include "log.hpp"
#include "asset.hpp"
#include "components/board.hpp"
#include "imagediff.hpp"

structlog LOGCFG = {};

void testBackgroundRemoval(string boardEmptyFileName, string frameBoardEmptyFileName)
{
  SBGCK_TEST_BEGIN("testBackgroundRemoval");

  Board board;
  board.frameBoardEmpty = imread(boardEmptyFileName, IMREAD_COLOR);
  Mat frameNoTokens = imread(frameBoardEmptyFileName, IMREAD_COLOR);

  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width != 0);
  SBGCK_ASSERT_THROW(frameNoTokens.size().width != 0);

  Mat diff = ImageDiff::removeBackground(frameNoTokens, board.frameBoardEmpty);

  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width == diff.size().width);
  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().height == diff.size().height);

  // imshow("frameBoardEmpty", board.frameBoardEmpty);
  // imshow("frameNoTokens", frameNoTokens);
  // imshow("diff", diff);
  // waitKey();

  for (int j = 0; j < diff.rows; j++)
  {
    for (int i = 0; i < diff.cols; i++)
    {
      cv::Vec3b pix = diff.at<cv::Vec3b>(j, i);
      int val = (pix[0] + pix[1] + pix[2]);
      // must be pitch black
      SBGCK_ASSERT_THROW(val == 0);
    }
  }

  SBGCK_TEST_END();
}

void testExtractTokenFromFrame(string boardEmptyFileName, string frameBoardWithTokenFileName)
{
  SBGCK_TEST_BEGIN("testExtractTokenFromFrame");

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

  cv::Vec3b pix; // contains BGR value 0-255 as array[3]

  // red color token
  pix = diff.at<cv::Vec3b>(415, 850);
  SBGCK_ASSERT_THROW(pix[2] == 255);

  // area around the token must be pitch black
  pix = diff.at<cv::Vec3b>(385, 836);
  SBGCK_ASSERT_THROW(pix[0] == 0 && pix[1] == 0 && pix[2] == 0);
  pix = diff.at<cv::Vec3b>(385, 892);
  SBGCK_ASSERT_THROW(pix[0] == 0 && pix[1] == 0 && pix[2] == 0);
  pix = diff.at<cv::Vec3b>(440, 836);
  SBGCK_ASSERT_THROW(pix[0] == 0 && pix[1] == 0 && pix[2] == 0);
  pix = diff.at<cv::Vec3b>(440, 892);
  SBGCK_ASSERT_THROW(pix[0] == 0 && pix[1] == 0 && pix[2] == 0);

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

  testBackgroundRemoval(boardEmpty_png, frameEmpty_png);
  testExtractTokenFromFrame(boardEmpty_png, frame_token_bridge_png);
}
