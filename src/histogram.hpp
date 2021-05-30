#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;

class Histogram
{
public:
    static bool createHistogramImage(Mat &src, Mat &dest);
};

#endif