#include "base.hpp"
#include "log.hpp"
#include "asset.hpp"
#include "components/board.hpp"
#include "imagediff.hpp"

structlog LOGCFG = {};

/// https://stackoverflow.com/questions/40895785/using-opencv-to-overlay-transparent-image-onto-another-image
void drawTransparency(Mat frame, Mat transp, int xPos, int yPos) {
   // Mat transp = cv::imread("dice.png", IMREAD_UNCHANGED);
    Mat mask;
    vector<Mat> layers;

    split(transp, layers); // separate channels
    Mat rgb[3] = { layers[0],layers[1],layers[2] };
    mask = layers[3]; // png's alpha channel used as mask
    merge(rgb, 3, transp);  // put together the RGB channels, now transp isn't transparent
    transp.copyTo(frame.rowRange(yPos, yPos + transp.rows).colRange(xPos, xPos + transp.cols), mask);
}

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

void testExtractTokenFromFrame(string boardEmptyFileName, string frameBoardEmptyFileName, string tokenFileName, int x, int y)
{
  SBGCK_TEST_BEGIN("testExtractTokenFromFrame");

  Board board;
  board.frameBoardEmpty = imread(boardEmptyFileName, IMREAD_COLOR);

  // put the token on the frame at the given position
  Mat frame = imread(frameBoardEmptyFileName, IMREAD_COLOR);
  Mat transp = imread(tokenFileName, IMREAD_UNCHANGED); // we need the transparency here
  drawTransparency(frame, transp, x, y);

  // imshow("frame", frame);
  // waitKey();

  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width != 0);
  SBGCK_ASSERT_THROW(frame.size().width != 0);

  Mat diff = ImageDiff::removeBackground(frame, board.frameBoardEmpty);

  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width == diff.size().width);
  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().height == diff.size().height);

  // imshow("frameBoardEmpty", board.frameBoardEmpty);
  // imshow("frame", frame);
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
  string frameEmpty_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");
  string token_red_circle = CMAKE_SOURCE_DIR + string("/tests/images/token_red_circle.png");

  LOGCFG.prefix = (char *)"test_diff";
  LOGCFG.headers = true;
  LOGCFG.level = INFO;

  testBackgroundRemoval(boardEmpty_png, frameEmpty_png);
  testExtractTokenFromFrame(boardEmpty_png, frameEmpty_png, token_red_circle, 838, 385);
}
