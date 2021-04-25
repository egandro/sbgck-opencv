#ifndef REGIONCIRCLE_H
#define REGIONCIRCLE_H

#include "log.hpp"
#include "opencv2/opencv.hpp"
#include "region.hpp"

using namespace cv;

class RegionCircle : public Region
{
public:
    Point center;
    int radius;

    RegionCircle() {
        radius = 0;
    }

    RegionCircle(const RegionCircle &value)
    {
        areaName = value.areaName;
        center = Point(value.center);
        radius = value.radius;
    }
};

#endif