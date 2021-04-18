#include <opencv2/opencv.hpp>

#include "version.hpp"

using namespace cv;

const char * getOpenCVVersion()
{
    return CV_VERSION;
}
