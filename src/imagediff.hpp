#ifndef IMAGEDIFF_H
#define IMAGEDIFF_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;

class ImageDiff
{
public:
    static Mat removeBackground(const Mat frame, const Mat background);
};

#endif