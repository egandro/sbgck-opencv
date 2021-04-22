#ifndef COLORCALIBRATION_H
#define COLORCALIBRATION_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;

class ColorCalibration
{
    Mat colorChecker;

public:
    ColorCalibration(const Mat &colorChecker) :
        colorChecker(colorChecker) {
    }

    void calibrate(const Mat &colorChecker);
};

#endif