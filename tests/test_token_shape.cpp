#include "base.hpp"
#include "log.hpp"
#include "asset.hpp"
#include "board.hpp"
#include "token.hpp"
#include "strategy.hpp"
#include "imagediff.hpp"

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

void getContours(Mat &imgDil, Mat &img)
{
  vector<vector<Point>> contours;
  vector<Vec4i> hierarchy;

  // https://stackoverflow.com/questions/37691479/opencv-detection-of-rectangle-or-pentagon
  findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

  // findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // original values
  // drawContours(img, contours, -1, Scalar(255, 0, 255), 2);

  if (contours.size() == 0)
  {
    return;
  }

  vector<vector<Point>> conPoly(contours.size());
  vector<Rect> boundRect(contours.size());

  for (int i = 0; i < contours.size(); i++)
  {
    double area = contourArea(contours[i]); // area of the contours we detected - here we need the min dimensions
    if (area < 1000)                        // get the 1000 from somewhere
      continue;

    // approximate polygon
    double peri = arcLength(contours[i], true);
    // this uses 0.01 - https://stackoverflow.com/questions/37691479/opencv-detection-of-rectangle-or-pentagon
    // this uses 0.04 - https://stackoverflow.com/questions/60177653/how-to-detect-an-octagonal-shape-in-python-and-opencv
    double epsilon = 0.04 * peri; // original value was 0.02

    // epsilon = 7.5; // suggestion was 3.0
    // int s = contours[i].size();
    // https://docs.opencv.org/3.4/dc/dcf/tutorial_js_contour_features.html < try convexHull here for circle?
    approxPolyDP(contours[i], conPoly[i], epsilon, true); // makes octagons of circles

    // bounding box
    boundRect[i] = boundingRect(conPoly[i]);

    //int rawCor = (int)contours[i].size();
    int objCor = (int)conPoly[i].size();
    string objectType;

    if (objCor == 3)
    {
      objectType = "Tri";
    }
    else if (objCor == 4)
    {
      float aspRatio = (float)boundRect[i].width / (float)boundRect[i].height;
      if (aspRatio > 0.95 && aspRatio < 1.05)
      {
        objectType = "Square";
      }
      else
      {
        objectType = "Rect";
      }
    }
    else if (objCor == 5)
    {
      objectType = "Pentagon";
    }
    else if (objCor == 6)
    {
      objectType = "Hexagon";
    }
    else if (objCor > 6)
    {
      objectType = "Circle";
    }

    // draw contour according to the approxymated polygon
    drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);
    // rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0,255,0), 2);
    putText(img, objectType, {boundRect[i].x, boundRect[i].y - 5}, FONT_HERSHEY_PLAIN, 1.0, Scalar(0, 69, 255), 1);
  }
}

void testExtractTokenFromFrame(string boardEmptyFileName, string frameBoardEmptyFileName, string tokenFileName,
                               int x, int y, Token &token)
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

  Mat imgGray, imgBlur, imgCanny, imgDil, imgErode, imgContours;

  // preprocessing

  //cvtColor(mask, imgGray, COLOR_BGR2GRAY);
  imgGray = mask;
  GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0);
  Canny(imgBlur, imgCanny, 25, 75);
  Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
  dilate(imgCanny, imgDil, kernel);

  // getContours(imgDil, frame);
  // imshow("frame", frame);

  // imshow("imgGray", imgGray);
  // imshow("imgBlur", imgBlur);
  // imshow("imgCanny", imgCanny);
  // imshow("imgDil", imgDil);

  waitKey();

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
  string token_red_octagon_png = CMAKE_SOURCE_DIR + string("/tests/images/token_red_octagon.png");
  string token_red_circle_png = CMAKE_SOURCE_DIR + string("/tests/images/token_red_circle.png");

  LOGCFG.prefix = (char *)"test_token_shape";
  LOGCFG.headers = true;
  LOGCFG.level = INFO;

  // x,y = 57 pixel from Inkscape

  Token tokenRedCircle;
  tokenRedCircle.geometry = Geometry::Circle;
  tokenRedCircle.color = Scalar(0, 0, 255);
  tokenRedCircle.asset = Asset(Mat(57, 57, CV_8UC3, tokenRedCircle.color));

  testExtractTokenFromFrame(boardEmpty_png, frameEmpty_png, token_red_square_png, 838, 385, tokenRedCircle);
  testExtractTokenFromFrame(boardEmpty_png, frameEmpty_png, token_red_triangle_png, 838, 385, tokenRedCircle);
  testExtractTokenFromFrame(boardEmpty_png, frameEmpty_png, token_red_pentagon_png, 838, 385, tokenRedCircle);
  testExtractTokenFromFrame(boardEmpty_png, frameEmpty_png, token_red_hexagon_png, 838, 385, tokenRedCircle);
  testExtractTokenFromFrame(boardEmpty_png, frameEmpty_png, token_red_octagon_png, 838, 385, tokenRedCircle); // same as circle
  testExtractTokenFromFrame(boardEmpty_png, frameEmpty_png, token_red_circle_png, 838, 385, tokenRedCircle);
}
