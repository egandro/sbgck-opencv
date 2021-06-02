#include "base.hpp"
#include "log.hpp"
#include "internal/asset.hpp"
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

void testDetectColorChecker(string boardEmptyFileName, string colorCheckerFileName,
                            int x, int y)
{
  SBGCK_TEST_BEGIN("testDetectColorChecker");

  Mat frame = imread(boardEmptyFileName, IMREAD_COLOR);
  Mat transp = imread(colorCheckerFileName, IMREAD_UNCHANGED); // we need the transparency here
  drawTransparency(frame, transp, x, y);

  Asset asset(colorCheckerFileName);

  SBGCK_ASSERT_THROW(asset.getDefault().image.size().width != 0);

  Asset detectedColorChecker;
  asset.assetDetector = AssetDetector::Feature2D;
  bool result = AssetDetection::detectAsset(frame, asset, detectedColorChecker);
  SBGCK_ASSERT_THROW(result == true);

  // imshow("frame", frame);
  // imshow("asset", asset.getDefault().image);
  // imshow("detectedColorChecker", detectedColorChecker.getDefault().image);
  // waitKey();

  SBGCK_TEST_END();
}

int main(int, char **)
{
  SBGCK_TEST_INIT();

  // board
  string boardEmpty_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");
  string color_checker_png = CMAKE_SOURCE_DIR + string("/tests/images/color_checker.png");
  string token_red_circle_png = CMAKE_SOURCE_DIR + string("/tests/images/token_red_circle.png");

  LOGCFG.prefix = (char *)"test_color_correction";
  LOGCFG.headers = true;
  LOGCFG.level = typelog::INFO;

  testDetectColorChecker(boardEmpty_png, color_checker_png, 30, 30);
}
