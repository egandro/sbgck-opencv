#ifndef IMAGEDIFF_H
#define IMAGEDIFF_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;

class ImageDiff
{
public:
    // ensure images are both in RGB / BRG ...
    static Mat removeBackground(const Mat frame, const Mat background);
};

#endif