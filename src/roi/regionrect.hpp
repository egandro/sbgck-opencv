#ifndef REGIONRECT_H
#define REGIONRECT_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;

class RegionRect
{
public:
    std::string areaName;
    Point pt1;
    Point pt2;

    RegionRect() {
    }

    RegionRect(const RegionRect &value)
    {
        areaName = value.areaName;
        pt1 = Point(value.pt1);
        pt2 = Point(value.pt2);
    }
};

#endif