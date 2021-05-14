#include "base.hpp"
#include "log.hpp"
#include "asset.hpp"
#include "board.hpp"
#include "strategy.hpp"
#include "imagediff.hpp"
#include "assetdetection.hpp"
#include "token.hpp"
#include "tokenshape.hpp"
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

void showCaption(const vector<ShapeLocation> &locs, Mat &frame, Token &token)
{
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
    string name = getGeometryString(token.geometry);
    name += "(";
    name += std::to_string((int)token.color[2]);
    name += ",";
    name += std::to_string((int)token.color[0]);
    name += ",";
    name += std::to_string((int)token.color[1]);
    name += ")";

    //string name = token.name;
    putText(frame, name, {(*it).boundRect.x, (*it).boundRect.y - 5}, FONT_HERSHEY_PLAIN, 1.0, Scalar(0, 69, 255), 1);
  }

  imshow("frame", frame);
  waitKey(0);
}

void testDetectRemoteTokens(string boardEmptyFileName, string frameBoardEmptyFileName, string remoteFileName,
                            int x, int y, Token &token, int expectedCount)
{
  SBGCK_TEST_BEGIN("testDetectRemoteTokens");

  Asset asset(imread(boardEmptyFileName, IMREAD_COLOR));
  Board board(asset);
  board.frameBoardEmpty = imread(boardEmptyFileName, IMREAD_COLOR);

  // put the token on the frame at the given position
  Mat frame = imread(frameBoardEmptyFileName, IMREAD_COLOR);
  Mat remote = imread(remoteFileName, IMREAD_UNCHANGED); // we need the transparency here
  drawTransparency(frame, remote, x, y);

  // imshow("frame", frame);
  // imshow("remoteToDetect", remoteToDetect.getDefault().image);
  // waitKey();

  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width != 0);
  SBGCK_ASSERT_THROW(frame.size().width != 0);

  Mat diff = ImageDiff::removeBackground(frame, board.frameBoardEmpty);

  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width == diff.size().width);
  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().height == diff.size().height);

  Mat color = TokenColor::detectColor(diff, token);

  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().width == color.size().width);
  SBGCK_ASSERT_THROW(board.frameBoardEmpty.size().height == color.size().height);

  // imshow("diff", diff);
  // imshow("color", color);
  // waitKey(0);

  //Mat mask = diff;
  Mat mask = color;

  /// shape detection
  const vector<ShapeLocation> locs = TokenShape::detectShape(mask, token);
  const int count = locs.size();

  SBGCK_ASSERT_THROW(count == expectedCount);

  // showCaption(locs, frame, token);

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

  LOGCFG.prefix = (char *)"test_token_remote";
  LOGCFG.headers = true;
  LOGCFG.level = typelog::INFO;

  // x,y = 57 pixel from Inkscape

  Token tokenRemoteWhiteRect;
  tokenRemoteWhiteRect.geometry = Geometry::Rect;
  tokenRemoteWhiteRect.color = Scalar(220, 220, 220);
  tokenRemoteWhiteRect.asset = Asset(Mat(57, 57, CV_8UC3, tokenRemoteWhiteRect.color));

  Token tokenRemoteBlueCircle;
  tokenRemoteBlueCircle.geometry = Geometry::Circle;
  tokenRemoteBlueCircle.color = Scalar(255, 255, 0);
  tokenRemoteBlueCircle.asset = Asset(Mat(57, 57, CV_8UC3, tokenRemoteBlueCircle.color));

  Token tokenRemoteRedCircle;
  tokenRemoteRedCircle.geometry = Geometry::Circle;
  tokenRemoteRedCircle.color = Scalar(128, 128, 255);
  tokenRemoteRedCircle.asset = Asset(Mat(57, 57, CV_8UC3, tokenRemoteRedCircle.color));

  Token tokenRemoteBlueTriangle;
  tokenRemoteBlueTriangle.geometry = Geometry::Triangle;
  tokenRemoteBlueTriangle.color = Scalar(255, 255, 0);
  tokenRemoteBlueTriangle.asset = Asset(Mat(57, 57, CV_8UC3, tokenRemoteBlueTriangle.color));


  testDetectRemoteTokens(boardEmpty_png, frameEmpty_png, remote_front_png, 300, 10, tokenRemoteWhiteRect, 1);

  testDetectRemoteTokens(boardEmpty_png, frameEmpty_png, remote_front_png, 300, 10, tokenRemoteBlueCircle, 3);
  testDetectRemoteTokens(boardEmpty_png, frameEmpty_png, remote_front_png, 300, 10, tokenRemoteBlueTriangle, 2);
  testDetectRemoteTokens(boardEmpty_png, frameEmpty_png, remote_front_png, 300, 10, tokenRemoteRedCircle, 0);

  testDetectRemoteTokens(boardEmpty_png, frameEmpty_png, remote_front_skip_png, 300, 10, tokenRemoteBlueCircle, 0);
  testDetectRemoteTokens(boardEmpty_png, frameEmpty_png, remote_front_repeat_png, 300, 10, tokenRemoteBlueCircle, 1);
  testDetectRemoteTokens(boardEmpty_png, frameEmpty_png, remote_front_forward_png, 300, 10, tokenRemoteBlueCircle, 2);
  testDetectRemoteTokens(boardEmpty_png, frameEmpty_png, remote_front_volume_up_png, 300, 10, tokenRemoteBlueCircle, 3);
  testDetectRemoteTokens(boardEmpty_png, frameEmpty_png, remote_front_volume_down_png, 300, 10, tokenRemoteBlueCircle, 3);
  testDetectRemoteTokens(boardEmpty_png, frameEmpty_png, remote_front_volume_down_png, 300, 10, tokenRemoteBlueTriangle, 1);
  testDetectRemoteTokens(boardEmpty_png, frameEmpty_png, remote_front_neutral_png, 300, 10, tokenRemoteBlueCircle, 3);
  testDetectRemoteTokens(boardEmpty_png, frameEmpty_png, remote_front_neutral_png, 300, 10, tokenRemoteBlueTriangle, 2);


  testDetectRemoteTokens(boardEmpty_png, frameEmpty_png, remote_back_png, 300, 10, tokenRemoteWhiteRect, 1);

  testDetectRemoteTokens(boardEmpty_png, frameEmpty_png, remote_back_png, 300, 10, tokenRemoteRedCircle, 1);
  testDetectRemoteTokens(boardEmpty_png, frameEmpty_png, remote_back_png, 300, 10, tokenRemoteBlueCircle, 1);
  testDetectRemoteTokens(boardEmpty_png, frameEmpty_png, remote_back_png, 300, 10, tokenRemoteBlueTriangle, 0);

  testDetectRemoteTokens(boardEmpty_png, frameEmpty_png, remote_back_power_png, 300, 10, tokenRemoteRedCircle, 0);
  testDetectRemoteTokens(boardEmpty_png, frameEmpty_png, remote_back_undo_png, 300, 10, tokenRemoteRedCircle, 1);
  testDetectRemoteTokens(boardEmpty_png, frameEmpty_png, remote_back_neutral_png, 300, 10, tokenRemoteRedCircle, 1);
  testDetectRemoteTokens(boardEmpty_png, frameEmpty_png, remote_back_neutral_png, 300, 10, tokenRemoteBlueCircle, 1);
}
