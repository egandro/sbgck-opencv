#include <opencv2/opencv.hpp>
#include <log.hpp>

#include "colorcalibration.hpp"

using namespace cv;

structlog LOGCFG = {};

int main(int argc, char **argv)
{
    LOGCFG.prefix = (char *)"main_color_correction";
    LOGCFG.headers = true;
    LOGCFG.level = DEBUG;

    if (argc != 4)
    {
        Log(DEBUG) << "usage: " << argv[0] << " <CameraFile> <ColorCheckerFile> <TargetFile>";
        return -1;
    }


    Mat frame = imread(argv[1], IMREAD_COLOR);
    Mat colorChecker = imread(argv[2], IMREAD_COLOR);

    ColorCalibration colorCalibration(colorChecker);

    Mat result(frame);
    colorCalibration.calibrate(result);

    // display
    imshow("frame", frame);
    imshow("result", result);
    imshow("colorChecker", colorChecker);
    waitKey();

    imwrite(argv[3], frame);

    return 0;
}
