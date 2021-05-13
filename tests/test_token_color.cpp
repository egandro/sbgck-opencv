#include "base.hpp"
#include "log.hpp"
#include "asset.hpp"
#include "board.hpp"
#include "token.hpp"
#include "strategy.hpp"
#include "imagediff.hpp"
#include "tokencolor.hpp"

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

  Mat mask = TokenColor::detectColor(diff, token);
  // imwrite("./mask.png", mask);
  // imshow("frame", frame);
  // imshow("diff", diff);
  // imshow("mask", mask);
  // waitKey();

  SBGCK_TEST_END();
}

int main(int, char **)
{
  SBGCK_TEST_INIT();

  // board
  string boardEmpty_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");
  string frameEmpty_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");
  string token_red_circle_png = CMAKE_SOURCE_DIR + string("/tests/images/token_red_circle.png");
  string token_green_circle_png = CMAKE_SOURCE_DIR + string("/tests/images/token_green_circle.png");
  string token_blue_circle_png = CMAKE_SOURCE_DIR + string("/tests/images/token_blue_circle.png");
  string token_yellow_circle_png = CMAKE_SOURCE_DIR + string("/tests/images/token_yellow_circle.png");
  string token_purple_circle_png = CMAKE_SOURCE_DIR + string("/tests/images/token_purple_circle.png");

  LOGCFG.prefix = (char *)"test_token_color";
  LOGCFG.headers = true;
  LOGCFG.level = typelog::INFO;

  // x,y = 57 pixel from Inkscape

  Token tokenRedCircle;
  tokenRedCircle.geometry = Geometry::Circle;
  tokenRedCircle.color = Scalar(0, 0, 255);
  tokenRedCircle.asset = Asset(Mat(57, 57, CV_8UC3, tokenRedCircle.color));

  Token tokenGreenCircle;
  tokenGreenCircle.geometry = Geometry::Circle;
  tokenGreenCircle.color = Scalar(0, 255, 0);
  tokenGreenCircle.asset = Asset(Mat(57, 57, CV_8UC3, tokenGreenCircle.color));

  Token tokenBlueCircle;
  tokenBlueCircle.geometry = Geometry::Circle;
  tokenBlueCircle.color = Scalar(255, 0, 0);
  tokenBlueCircle.asset = Asset(Mat(57, 57, CV_8UC3, tokenBlueCircle.color));

  Token tokenYellowCircle;
  tokenYellowCircle.geometry = Geometry::Circle;
  tokenYellowCircle.color = Scalar(0, 255, 255);
  tokenYellowCircle.asset = Asset(Mat(57, 57, CV_8UC3, tokenYellowCircle.color));

  Token tokenPurpleCircle;
  tokenPurpleCircle.geometry = Geometry::Circle;
  tokenPurpleCircle.color = Scalar(84, 0, 59);
  tokenPurpleCircle.asset = Asset(Mat(57, 57, CV_8UC3, tokenPurpleCircle.color));

  testExtractTokenFromFrame(boardEmpty_png, frameEmpty_png, token_red_circle_png, 838, 385, tokenRedCircle);
  testExtractTokenFromFrame(boardEmpty_png, frameEmpty_png, token_green_circle_png, 838, 385, tokenGreenCircle);
  testExtractTokenFromFrame(boardEmpty_png, frameEmpty_png, token_blue_circle_png, 838, 385, tokenBlueCircle);
  testExtractTokenFromFrame(boardEmpty_png, frameEmpty_png, token_yellow_circle_png, 838, 385, tokenYellowCircle);
  testExtractTokenFromFrame(boardEmpty_png, frameEmpty_png, token_purple_circle_png, 838, 385, tokenPurpleCircle);
}
