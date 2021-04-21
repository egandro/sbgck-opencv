#include <opencv2/opencv.hpp>
#include <log.hpp>

#include "imagediff.hpp"

using namespace cv;

structlog LOGCFG = {};

int main(int argc, char **argv)
{
    LOGCFG.prefix = (char *)"main_camera";
    LOGCFG.headers = true;
    LOGCFG.level = DEBUG;

    if (argc != 4)
    {
        Log(DEBUG) << "usage: " << argv[0] << " <SourceFile> <MaskFile> <TargetFile>";
        return -1;
    }


    Mat frame = imread(argv[1], IMREAD_COLOR);
    Mat background = imread(argv[2], IMREAD_COLOR);

    Mat result = ImageDiff::removeBackground(frame, background);

    // display
    imshow("result", result);
    waitKey();

    imwrite(argv[3], result);

    return 0;
}
