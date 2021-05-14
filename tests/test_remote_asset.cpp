#include "base.hpp"
#include "log.hpp"
#include "asset.hpp"
#include "board.hpp"
#include "strategy.hpp"
#include "imagediff.hpp"
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

void testDetectFullRemote(string boardEmptyFileName, string frameBoardEmptyFileName, string remoteFileName,
                               int x, int y)
{
  SBGCK_TEST_BEGIN("testDetectFullRemote");

  Asset asset(imread(boardEmptyFileName, IMREAD_COLOR));
  Board board(asset);
  board.frameBoardEmpty = imread(boardEmptyFileName, IMREAD_COLOR);

  // put the token on the frame at the given position
  Mat frame = imread(frameBoardEmptyFileName, IMREAD_COLOR);
  Mat remote = imread(remoteFileName, IMREAD_UNCHANGED); // we need the transparency here
  Asset remoteToDetect(remoteFileName);
  remoteToDetect.assetDetector = AssetDetector::Feature2D;
  drawTransparency(frame, remote, x, y);

  // imshow("frame", frame);
  // imshow("remoteToDetect", remoteToDetect.getDefault().image);
  // waitKey();

  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width != 0);
  SBGCK_ASSERT_THROW(frame.size().width != 0);

  Mat diff = ImageDiff::removeBackground(frame, board.frameBoardEmpty);

  imshow("diff", diff);
  waitKey();

  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width == diff.size().width);
  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().height == diff.size().height);

  Asset detectedRemote;
  bool result = AssetDetection::detectAsset(diff, remoteToDetect, detectedRemote);
  SBGCK_ASSERT_THROW(result == true);

  SBGCK_ASSERT_THROW(detectedRemote.getDefault().image.size().width == remoteToDetect.getDefault().image.size().width);
  SBGCK_ASSERT_THROW(detectedRemote.getDefault().image.size().height == remoteToDetect.getDefault().image.size().height);

  // imshow("frame", frame);
  // imshow("detectedRemote", detectedRemote.getDefault().image);
  // waitKey();

  SBGCK_TEST_END();
}

void testNotDetectFullRemote(string boardEmptyFileName, string frameBoardEmptyFileName, string remoteFileName,
                               int x, int y, string remoteNotFileName)
{
  SBGCK_TEST_BEGIN("testNotDetectFullRemote");

  Asset asset(imread(boardEmptyFileName, IMREAD_COLOR));
  Board board(asset);
  board.frameBoardEmpty = imread(boardEmptyFileName, IMREAD_COLOR);

  // put the token on the frame at the given position
  Mat frame = imread(frameBoardEmptyFileName, IMREAD_COLOR);
  Mat remote = imread(remoteFileName, IMREAD_UNCHANGED); // we need the transparency here
  Asset remoteToDetect(remoteNotFileName);
  remoteToDetect.assetDetector = AssetDetector::SIFT;
  drawTransparency(frame, remote, x, y);

  // imshow("frame", frame);
  // imshow("remoteToDetect", remoteToDetect.getDefault().image);
  // waitKey();

  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width != 0);
  SBGCK_ASSERT_THROW(frame.size().width != 0);

  Mat diff = ImageDiff::removeBackground(frame, board.frameBoardEmpty);

  // imshow("diff", diff);
  // waitKey();

  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width == diff.size().width);
  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().height == diff.size().height);

  Asset detectedRemote;
  bool result = AssetDetection::detectAsset(diff, remoteToDetect, detectedRemote);
  if(result) {
    imshow("frame", frame);
    imshow("detectedRemote", detectedRemote.getDefault().image);
    waitKey();
  }
  SBGCK_ASSERT_THROW(result == false);

  SBGCK_ASSERT_THROW(detectedRemote.getDefault().image.size().width == 0);
  SBGCK_ASSERT_THROW(detectedRemote.getDefault().image.size().height == 0);

  SBGCK_TEST_END();
}

void testDetectSegmentOnFullRemote(string boardEmptyFileName, string frameBoardEmptyFileName, string remoteFileName,
                               int x, int y, string remoteSegmentFileName)
{
  SBGCK_TEST_BEGIN("testDetectSegmentOnFullRemote");

  Asset asset(imread(boardEmptyFileName, IMREAD_COLOR));
  Board board(asset);
  board.frameBoardEmpty = imread(boardEmptyFileName, IMREAD_COLOR);

  // put the token on the frame at the given position
  Mat frame = imread(frameBoardEmptyFileName, IMREAD_COLOR);
  Mat remote = imread(remoteFileName, IMREAD_UNCHANGED); // we need the transparency here
  Asset remoteToDetect(remoteSegmentFileName);
  remoteToDetect.assetDetector = AssetDetector::Feature2D;
  drawTransparency(frame, remote, x, y);

  // imshow("frame", frame);
  // imshow("remoteToDetect", remoteToDetect.getDefault().image);
  // waitKey();

  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width != 0);
  SBGCK_ASSERT_THROW(frame.size().width != 0);

  Mat diff = ImageDiff::removeBackground(frame, board.frameBoardEmpty);

  // imshow("diff", diff);
  // imshow("remoteToDetect", remoteToDetect.getDefault().image);
  // waitKey();

  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width == diff.size().width);
  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().height == diff.size().height);

  Asset detectedRemote;
  bool result = AssetDetection::detectAsset(diff, remoteToDetect, detectedRemote);
  SBGCK_ASSERT_THROW(result == true);

  SBGCK_ASSERT_THROW(detectedRemote.getDefault().image.size().width == remoteToDetect.getDefault().image.size().width);
  SBGCK_ASSERT_THROW(detectedRemote.getDefault().image.size().height == remoteToDetect.getDefault().image.size().height);

  // imshow("frame", frame);
  // imshow("detectedRemote", detectedRemote.getDefault().image);
  // waitKey();

  SBGCK_TEST_END();
}


int main(int, char **)
{
  SBGCK_TEST_INIT();

  // board
  string boardEmpty_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");
  string frameEmpty_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");

  string remote_back_png = CMAKE_SOURCE_DIR + string("/tests/images/remote_back.png");
  string remote_back_power_png = CMAKE_SOURCE_DIR + string("/tests/images/remote_back_power.png");
  string remote_back_undo_png = CMAKE_SOURCE_DIR + string("/tests/images/remote_back_undo.png");
  string remote_back_neutral_png = CMAKE_SOURCE_DIR + string("/tests/images/remote_back_neutral.png");

  string remote_front_png = CMAKE_SOURCE_DIR + string("/tests/images/remote_front.png");
  string remote_front_skip_png = CMAKE_SOURCE_DIR + string("/tests/images/remote_front_skip.png");
  string remote_front_repeat_png = CMAKE_SOURCE_DIR + string("/tests/images/remote_front_repeat.png");
  string remote_front_forward_png = CMAKE_SOURCE_DIR + string("/tests/images/remote_front_forward.png");
  string remote_front_volume_up_png = CMAKE_SOURCE_DIR + string("/tests/images/remote_front_volume_up.png");
  string remote_front_volume_down_png = CMAKE_SOURCE_DIR + string("/tests/images/remote_front_volume_down.png");
  string remote_front_neutral_png = CMAKE_SOURCE_DIR + string("/tests/images/remote_front_neutral.png");

  LOGCFG.prefix = (char *)"test_remote_asset";
  LOGCFG.headers = true;
  LOGCFG.level = typelog::INFO;


  testDetectFullRemote(boardEmpty_png, frameEmpty_png, remote_front_png, 300, 10);
  // testNotDetectFullRemote(boardEmpty_png, frameEmpty_png, remote_front_png, 300, 10, remote_back_png);
  testDetectSegmentOnFullRemote(boardEmpty_png, frameEmpty_png, remote_front_png, 300, 10, remote_front_skip_png);
  testDetectSegmentOnFullRemote(boardEmpty_png, frameEmpty_png, remote_front_png, 300, 10, remote_front_repeat_png);
  testDetectSegmentOnFullRemote(boardEmpty_png, frameEmpty_png, remote_front_png, 300, 10, remote_front_forward_png);
  testDetectSegmentOnFullRemote(boardEmpty_png, frameEmpty_png, remote_front_png, 300, 10, remote_front_volume_up_png);
  testDetectSegmentOnFullRemote(boardEmpty_png, frameEmpty_png, remote_front_png, 300, 10, remote_front_volume_down_png);
  testDetectSegmentOnFullRemote(boardEmpty_png, frameEmpty_png, remote_front_png, 300, 10, remote_front_neutral_png);
  testDetectFullRemote(boardEmpty_png, frameEmpty_png, remote_back_png, 300, 10);
  testDetectSegmentOnFullRemote(boardEmpty_png, frameEmpty_png, remote_back_png, 300, 10, remote_back_power_png);
  testDetectSegmentOnFullRemote(boardEmpty_png, frameEmpty_png, remote_back_png, 300, 10, remote_back_undo_png);
  testDetectSegmentOnFullRemote(boardEmpty_png, frameEmpty_png, remote_back_png, 300, 10, remote_back_neutral_png);
 }
