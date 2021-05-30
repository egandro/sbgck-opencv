#include "base.hpp"
#include "log.hpp"
#include "histogram.hpp"

structlog LOGCFG = {};

void testHistogramImage(string frameFileName)
{
  SBGCK_TEST_BEGIN("testHistogramImage");

  Mat src = imread(frameFileName, IMREAD_COLOR);
  Mat histImage;

  SBGCK_ASSERT_THROW(src.empty() == false);
  SBGCK_ASSERT_THROW(Histogram::createHistogramImage(src, histImage) == true);
  SBGCK_ASSERT_THROW(histImage.empty() == false);

  // imshow("src", src);
  // imshow("histImage", histImage);
  // waitKey();

  SBGCK_TEST_END();
}

void testHistogramImageGrayScale(string frameFileName)
{
  SBGCK_TEST_BEGIN("testHistogramImageGrayScale");

  Mat color = imread(frameFileName, IMREAD_COLOR);
  Mat src;
  // to grayscale
  cvtColor(color, src, COLOR_BGR2GRAY);

  Mat histImage;

  SBGCK_ASSERT_THROW(src.empty() == false);
  SBGCK_ASSERT_THROW(Histogram::createHistogramImage(src, histImage) == true);
  SBGCK_ASSERT_THROW(histImage.empty() == false);

  // imshow("color", color);
  // imshow("src", src);
  // imshow("histImage", histImage);
  // waitKey();

  SBGCK_TEST_END();
}


int main(int, char **)
{
  SBGCK_TEST_INIT();
  string board_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");

  LOGCFG.prefix = (char *)"test_histogram";
  LOGCFG.headers = true;
  LOGCFG.level = typelog::INFO;

  testHistogramImage(board_png);
  testHistogramImageGrayScale(board_png);
}