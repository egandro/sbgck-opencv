#include "base.hpp"
#include "log.hpp"
#include "internal/asset.hpp"
#include "board.hpp"
#include "token.hpp"
#include "strategy.hpp"
#include "imagediff.hpp"
#include "tokenshape.hpp"

#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;

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

  float tolerance = 0.05f; // 5%

  int b = (int)token.color[0];
  int g = (int)token.color[1];
  int r = (int)token.color[2];

  int bmin = (int)(b - (255.0f * tolerance));
  if (bmin < 0)
    bmin = 0;
  int gmin = (int)(g - (255.0f * tolerance));
  if (gmin < 0)
    gmin = 0;
  int rmin = (int)(r - (255.0f * tolerance));
  if (rmin < 0)
    rmin = 0;

  int bmax = (int)(b + (255.0f * tolerance));
  if (bmax > 255)
    bmax = 255;
  int gmax = (int)(g + (255.0f * tolerance));
  if (gmax > 255)
    gmax = 255;
  int rmax = (int)(r + (255.0f * tolerance));
  if (rmax > 255)
    rmax = 255;

  Scalar lower(bmin, gmin, rmin);
  Scalar upper(bmax, gmax, rmax);

  Mat mask;
  inRange(frame, lower, upper, mask);
  // imwrite("./mask.png", mask);

  // imshow("frame", frame);
  // imshow("mask", mask);
  // waitKey();

  /// shape detection
  vector<ShapeLocation> locs = TokenShape::detectShape(mask, token);

  // Using a for loop with iterator
  for (auto it = std::begin(locs); it != std::end(locs); ++it)
  {
    // we need a vector of vector of points
    vector<vector<Point>> conPoly;
    conPoly.push_back((*it).contours);

    // draw contour according to the approxymated polygon
    drawContours(frame, conPoly, 0, Scalar(255, 0, 255), 2);

    // bounding box
    rectangle(frame, (*it).boundRect.tl(), (*it).boundRect.br(), Scalar(0, 255, 0), 2);

    // text what we detected
    putText(frame, getGeometryString(token.geometry), {(*it).boundRect.x, (*it).boundRect.y - 5}, FONT_HERSHEY_PLAIN, 1.0, Scalar(0, 69, 255), 1);

    // imshow("frame", frame);
    // imshow("mask", mask);
    // waitKey();
  }

  SBGCK_TEST_END();
}

int main(int, char **)
{
  SBGCK_TEST_INIT();

  // board
  string boardEmpty_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");
  string frameEmpty_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");
  string token_red_square_png = CMAKE_SOURCE_DIR + string("/tests/images/token_red_square.png");
  string token_red_triangle_png = CMAKE_SOURCE_DIR + string("/tests/images/token_red_triangle.png");
  string token_red_pentagon_png = CMAKE_SOURCE_DIR + string("/tests/images/token_red_pentagon.png");
  string token_red_hexagon_png = CMAKE_SOURCE_DIR + string("/tests/images/token_red_hexagon.png");
  string token_red_circle_png = CMAKE_SOURCE_DIR + string("/tests/images/token_red_circle.png");

  LOGCFG.prefix = (char *)"test_token_shape";
  LOGCFG.headers = true;
  LOGCFG.level = typelog::INFO;

  // x,y = 57 pixel from Inkscape

  Token tokenRedCircle;
  tokenRedCircle.geometry = Geometry::Circle;
  tokenRedCircle.color = Scalar(0, 0, 255);
  tokenRedCircle.asset = Asset(Mat(57, 57, CV_8UC3, tokenRedCircle.color));

  testExtractTokenFromFrame(boardEmpty_png, frameEmpty_png, token_red_square_png, 838, 385, tokenRedCircle);
  testExtractTokenFromFrame(boardEmpty_png, frameEmpty_png, token_red_triangle_png, 838, 385, tokenRedCircle);
  testExtractTokenFromFrame(boardEmpty_png, frameEmpty_png, token_red_pentagon_png, 838, 385, tokenRedCircle);
  testExtractTokenFromFrame(boardEmpty_png, frameEmpty_png, token_red_hexagon_png, 838, 385, tokenRedCircle);
  testExtractTokenFromFrame(boardEmpty_png, frameEmpty_png, token_red_circle_png, 838, 385, tokenRedCircle);
}
