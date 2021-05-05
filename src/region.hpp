#ifndef REGION_H
#define REGION_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;

class Region
{
public:
    std::string areaName;
    bool isInside(const Point p);
    Rect getBoundingRect();
};

#endif