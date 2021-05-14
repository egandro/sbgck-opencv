#include "base.hpp"
#include "log.hpp"
#include "asset.hpp"
#include "board.hpp"
#include "token.hpp"
#include "strategy.hpp"
#include "imagediff.hpp"
#include "tokencolor.hpp"
#include "assetdetection.hpp"

#include "opencv2/opencv.hpp"
using namespace cv;

structlog LOGCFG = {};

/// https://stackoverflow.com/questions/40895785/using-opencv-to-overlay-transparent-image-onto-another-image
void drawTransparency(Mat frame, Mat transp, int xPos, int yPos)
{
  // Mat transp = cv::imread("dice.png", IMREAD_UNCHANGED);
  Mat mask;
  vector<Mat> layers;

  split(transp, layers); // separate channels
  Mat rgb[3] = {layers[0], layers[1], layers[2]};
  mask = layers[3];      // png's alpha channel used as mask
  merge(rgb, 3, transp); // put together the RGB channels, now transp isn't transparent
  transp.copyTo(frame.rowRange(yPos, yPos + transp.rows).colRange(xPos, xPos + transp.cols), mask);
}

void testExtractTokenFromFrame(string boardEmptyFileName, string frameBoardEmptyFileName, string tokenFileName,
                               int x, int y, Token &token)
{
  SBGCK_TEST_BEGIN("testExtractTokenFromFrame");

  Asset asset(imread(boardEmptyFileName, IMREAD_COLOR));
  Board board(asset);
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

  // imshow("frame", frame);
  // imshow("diff", diff);
  // waitKey();

  Asset detectedToken;
  bool result = AssetDetection::detectAsset(diff, token.asset, detectedToken);
  SBGCK_ASSERT_THROW(result == true);

  SBGCK_ASSERT_THROW(detectedToken.getDefault().image.size().width == token.asset.getDefault().image.size().width);
  SBGCK_ASSERT_THROW(detectedToken.getDefault().image.size().height == token.asset.getDefault().image.size().height);

  // imshow("frame", frame);
  // imshow("detectedToken", detectedToken.getDefault().image);
  // waitKey();

  SBGCK_TEST_END();
}

int main(int, char **)
{
  SBGCK_TEST_INIT();

  // board
  string boardEmpty_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");
  string frameEmpty_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");
  string card_yes_png = CMAKE_SOURCE_DIR + string("/tests/images/card_yes.png");
  string card_no_png = CMAKE_SOURCE_DIR + string("/tests/images/card_no.png");

  LOGCFG.prefix = (char *)"test_token_asset";
  LOGCFG.headers = true;
  LOGCFG.level = typelog::INFO;

  Token tokenCardYes;
  tokenCardYes.tokenDetector = TokenDetector::Asset;
  tokenCardYes.asset = Asset(card_yes_png);
  tokenCardYes.asset.assetDetector = AssetDetector::Feature2D;

  Token tokenCardNo;
  tokenCardNo.tokenDetector = TokenDetector::Asset;
  tokenCardNo.asset = Asset(card_no_png);
  tokenCardNo.asset.assetDetector = AssetDetector::Feature2D;

  testExtractTokenFromFrame(boardEmpty_png, frameEmpty_png, card_yes_png, 300, 200, tokenCardYes);
  testExtractTokenFromFrame(boardEmpty_png, frameEmpty_png, card_no_png, 300, 200, tokenCardNo);
 }
