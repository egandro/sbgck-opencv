#include "base.hpp"
#include "log.hpp"
#include "internal/asset.hpp"
#include "board.hpp"
#include "assetdetection.hpp"

structlog LOGCFG = {};

void testBoardFromFile(string boardFileName, string frameBoardEmptyFileName)
{
  SBGCK_TEST_BEGIN("testBoardFromFile");

  Asset asset(boardFileName);
  Board board(asset);
  board.frameBoardEmpty = imread(frameBoardEmptyFileName, IMREAD_COLOR);

  SBGCK_ASSERT_THROW(board.asset.getDefault().image.size().width != 0);
  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width != 0);

  SBGCK_TEST_END();
}

void testDetectBoardInFrameDetectionAssetDetectorFeature2D(string boardFileName, string frameFileName)
{
  SBGCK_TEST_BEGIN("testDetectBoardInFrameDetectionAssetDetectorFeature2D");

  Mat frame = imread(frameFileName, IMREAD_COLOR);

  Asset asset(boardFileName);
  Board board(asset);
  board.asset.assetDetector = AssetDetector::Feature2D; // default strategy

  SBGCK_ASSERT_THROW(board.asset.getDefault().image.size().width != 0);
  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width == 0);

  Asset detectedBoard;
  bool result = AssetDetection::detectAsset(frame, board.asset, detectedBoard);
  SBGCK_ASSERT_THROW(result == true);

  SBGCK_ASSERT_THROW(detectedBoard.getDefault().image.size().width != 0);

  // the size of the detected board must match
  SBGCK_ASSERT_THROW(detectedBoard.getDefault().image.size().width == board.asset.getDefault().image.size().width);
  SBGCK_ASSERT_THROW(detectedBoard.getDefault().image.size().height == board.asset.getDefault().image.size().height);

  SBGCK_TEST_END();
}

void testDetectBoardInFrameDetectionAssetDetectorSIFT(string boardFileName, string frameFileName)
{
  SBGCK_TEST_BEGIN("testDetectBoardInFrameDetectionAssetDetectorSIFT");

  Mat frame = imread(frameFileName, IMREAD_COLOR);

  Asset asset(boardFileName);
  Board board(asset);
  board.asset.assetDetector = AssetDetector::SIFT;

  SBGCK_ASSERT_THROW(board.asset.getDefault().image.size().width != 0);
  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width == 0);

  Asset detectedBoard;
  bool result = AssetDetection::detectAsset(frame, board.asset, detectedBoard);
  SBGCK_ASSERT_THROW(result == true);

  SBGCK_ASSERT_THROW(detectedBoard.getDefault().image.size().width != 0);

  // the size of the detected board must match
  SBGCK_ASSERT_THROW(detectedBoard.getDefault().image.size().width == board.asset.getDefault().image.size().width);
  SBGCK_ASSERT_THROW(detectedBoard.getDefault().image.size().height == board.asset.getDefault().image.size().height);

  // imshow("detectedBoard", detectedBoard.getDefault().image);
  // waitKey();

  SBGCK_TEST_END();
}

void testDetectBoardInFrameDetectionAssetDetectorORB(string boardFileName, string frameFileName)
{
  SBGCK_TEST_BEGIN("testDetectBoardInFrameDetectionAssetDetectorORB");

  Mat frame = imread(frameFileName, IMREAD_COLOR);

  Asset asset(boardFileName);
  Board board(asset);
  board.asset.assetDetector = AssetDetector::ORB;

  SBGCK_ASSERT_THROW(board.asset.getDefault().image.size().width != 0);
  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width == 0);

  Asset detectedBoard;
  bool result = AssetDetection::detectAsset(frame, board.asset, detectedBoard);
  SBGCK_ASSERT_THROW(result == true);

  SBGCK_ASSERT_THROW(detectedBoard.getDefault().image.size().width != 0);

  // the size of the detected board must match
  SBGCK_ASSERT_THROW(detectedBoard.getDefault().image.size().width == board.asset.getDefault().image.size().width);
  SBGCK_ASSERT_THROW(detectedBoard.getDefault().image.size().height == board.asset.getDefault().image.size().height);

  // imshow("detectedBoard", detectedBoard.getDefault().image);
  // waitKey();

  SBGCK_TEST_END();
}

void testDetectBoardNotInFrameDetection(string boardFileName, string frameFileName)
{
  SBGCK_TEST_BEGIN("testDetectBoardNotInFrameDetection");

  Mat frame = imread(frameFileName, IMREAD_COLOR);

  Asset asset(imread(boardFileName, IMREAD_COLOR));
  Board board(asset);

  SBGCK_ASSERT_THROW(board.asset.getDefault().image.size().width != 0);
  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width == 0);

  Asset detectedBoard;
  bool result = AssetDetection::detectAsset(frame, board.asset, detectedBoard);

  // if(result) {
  //   imshow("detectedBoard", detectedBoard.getDefault().image);
  //   waitKey();
  // }

  SBGCK_ASSERT_THROW(result == false);
  SBGCK_ASSERT_THROW(detectedBoard.getDefault().image.size().width == 0);

  SBGCK_TEST_END();
}

void testDetectBoardReuseHomography(string boardFileName, string frameFileName)
{
  SBGCK_TEST_BEGIN("testDetectBoardInFrameDetection");

  Mat frame = imread(frameFileName, IMREAD_COLOR);

  Asset asset(boardFileName);
  Board board(asset);

  SBGCK_ASSERT_THROW(board.asset.getDefault().image.size().width != 0);
  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width == 0);

  Asset detectedBoard;
  bool result = AssetDetection::detectAsset(frame, board.asset, detectedBoard);
  SBGCK_ASSERT_THROW(result == true);
  SBGCK_ASSERT_THROW(detectedBoard.getDefault().image.size().width != 0);

  // the size of the detected board must match
  SBGCK_ASSERT_THROW(detectedBoard.getDefault().image.size().width == board.asset.getDefault().image.size().width);
  SBGCK_ASSERT_THROW(detectedBoard.getDefault().image.size().height == board.asset.getDefault().image.size().height);

  // imshow("detectedBoard1", detectedBoard.getDefault().image);
  // waitKey();

  Mat frame2 = imread(frameFileName, IMREAD_COLOR);
  Asset asset2(boardFileName);
  Board board2(asset2);
  board2.asset.homography = board.asset.homography; // reuse homography

  Asset detectedBoard2;
  result = AssetDetection::detectAsset(frame2, board2.asset, detectedBoard2);
  SBGCK_ASSERT_THROW(result == true);
  SBGCK_ASSERT_THROW(detectedBoard2.getDefault().image.size().width != 0);

  // the size of the detected board must match
  SBGCK_ASSERT_THROW(detectedBoard2.getDefault().image.size().width == board.asset.getDefault().image.size().width);
  SBGCK_ASSERT_THROW(detectedBoard2.getDefault().image.size().height == board.asset.getDefault().image.size().height);

  // imshow("detectedBoard2", detectedBoard.getDefault().image);
  // waitKey();

  SBGCK_TEST_END();
}

int main(int, char **)
{
  SBGCK_TEST_INIT();

  // board
  string board_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");
  string solid_png = CMAKE_SOURCE_DIR + string("/tests/images/solid.png");
  // in production we do this in the calibration step - here we use the same board
  string frameEmpty_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");
  // artifical frame
  string frame_png = CMAKE_SOURCE_DIR + string("/tests/images/frame.png");

  LOGCFG.prefix = (char *)"test_board";
  LOGCFG.headers = true;
  LOGCFG.level = typelog::INFO;

  testBoardFromFile(board_png, frameEmpty_png);

  testDetectBoardInFrameDetectionAssetDetectorFeature2D(board_png, frame_png);
  //xxx testDetectBoardInFrameDetectionAssetDetectorSIFT(board_png, frame_png); // broken (histogram showed this)

  //xxx testDetectBoardInFrameDetectionAssetDetectorORB(board_png, frame_png); // broken

  testDetectBoardNotInFrameDetection(solid_png, frame_png);
  testDetectBoardReuseHomography(board_png, frame_png);
}
