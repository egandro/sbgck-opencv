#include "base.hpp"
#include "log.hpp"
#include "internal/asset.hpp"
#include "detector.hpp"

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

void testCalibrateColorMap(string boardEmptyFileName, string colorCheckerFileName, int border, int xoffset, int yoffset,
                           std::vector<std::vector<Scalar>> referenceColors)
{
  // put the token on the frame at the given position
  Mat frame = imread(boardEmptyFileName, IMREAD_COLOR);
  Mat transp = imread(colorCheckerFileName, IMREAD_UNCHANGED); // we need the transparency here
  drawTransparency(frame, transp, xoffset, yoffset);

  // imshow("frame", frame);
  // waitKey();

  Asset reference(colorCheckerFileName);
  reference.assetDetector = AssetDetector::Feature2D;
  SBGCK_ASSERT_THROW(reference.getDefault().image.size().width != 0);

  //Mat colorChecker;
  //SBGCK_ASSERT_THROW(Detector::detectReferenceImage(frame, reference, colorChecker, false) == true);

  // imshow("frame", frame);
  // imshow("reference", reference.getDefault().image);
  // imshow("colorChecker", colorChecker);
  // waitKey();

  SBGCK_ASSERT_THROW(referenceColors.size() != 0);
  SBGCK_ASSERT_THROW(referenceColors[0].size() != 0);

  ColorMap result;
  Mat destination;
  SBGCK_ASSERT_THROW(Detector::calibrateColorMap(frame, reference, destination, referenceColors, result, border) == true);

  // imshow("destination", destination);
  // waitKey();

  const int max_y = (int)referenceColors.size();
  const int max_x = (int)referenceColors[0].size();

  for (int y = 0; y < max_y; y++)
  {
    for (int x = 0; x < max_x; x++)
    {
      Scalar refColor = referenceColors[y][x];
      Scalar mapColor = result.getMappedColor(refColor);

      float valRef = (float)(refColor[0] * refColor[0] + refColor[1] * refColor[1] + refColor[2] * refColor[2]);
      valRef = sqrt(valRef);

      float valMap = (float)(mapColor[0] * mapColor[0] + mapColor[1] * mapColor[1] + mapColor[2] * mapColor[2]);
      valMap = sqrt(valMap);

      const float diff = fabs(valRef - valMap);
      SBGCK_ASSERT_THROW(diff < 1.00);
    }
  }
}

int main(int, char **)
{
  SBGCK_TEST_INIT();

  // board
  string boardEmpty_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");
  string color_checker_png = CMAKE_SOURCE_DIR + string("/tests/images/color_checker.png");

  LOGCFG.prefix = (char *)"test_color_calibration";
  LOGCFG.headers = true;
  LOGCFG.level = typelog::INFO;

  std::vector<std::vector<Scalar>> referenceColors = {
      {Scalar(0, 0, 255), Scalar(255, 0, 0), Scalar(0, 255, 0), Scalar(255, 255, 0), Scalar(59, 0, 84), Scalar(0, 255, 255)},
      {Scalar(255, 128, 128), Scalar(255, 255, 255), Scalar(0, 0, 0), Scalar(255, 255, 255), Scalar(255, 255, 255), Scalar(255, 255, 255)},
      {Scalar(255, 255, 255), Scalar(255, 255, 255), Scalar(255, 255, 255), Scalar(255, 255, 255), Scalar(255, 255, 255), Scalar(255, 255, 255)},
      {Scalar(243, 243, 243), Scalar(200, 200, 200), Scalar(160, 160, 160), Scalar(122, 122, 121), Scalar(85, 85, 85), Scalar(52, 52, 52)}};

  for (int x = 0; x < 6; x++)
  {
    for (int y = 0; y < 4; y++)
    {
      // RGB to BGR
      Scalar rgb = referenceColors[y][x];
      Scalar bgr(rgb[2], rgb[1], rgb[0]);
      referenceColors[y][x] = bgr;
    }
  }

  int border = 20;

  testCalibrateColorMap(boardEmpty_png, color_checker_png, border, 30, 30, referenceColors);
}
