#include "base.hpp"
#include "log.hpp"
#include "camera.hpp"

structlog LOGCFG = {};


void testOpenCameraDebugFile(string assetFileName)
{
  SBGCK_TEST_BEGIN("testOpenCameraDebugFile");

  // load static frame
  Mat img = imread(assetFileName, IMREAD_COLOR);
  SBGCK_ASSERT_THROW(img.empty() == false);

  Camera cam;
  CameraConfig cfg(CameraMode::DebugFile, "", img);

  // open camera
  SBGCK_ASSERT_THROW(cam.open(cfg) == true);

  Mat frame;

  // read frame
  SBGCK_ASSERT_THROW(cam.getFrame(frame) == true);
  SBGCK_ASSERT_THROW(frame.empty() == false);

  SBGCK_TEST_END();
}

int main(int, char **)
{
  SBGCK_TEST_INIT();
  string board_png = CMAKE_SOURCE_DIR + string("/tests/images/board.png");

  LOGCFG.prefix = (char *)"test_camera";
  LOGCFG.headers = true;
  LOGCFG.level = typelog::INFO;

  testOpenCameraDebugFile(board_png);
}