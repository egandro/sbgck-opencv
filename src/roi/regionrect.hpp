#ifndef REGIONRECT_H
#define REGIONRECT_H

#include "log.hpp"
#include "opencv2/opencv.hpp"
#include "region.hpp"

using namespace cv;

class RegionRect : public Region
{
public:
    Point tl;
    Point br;

    RegionRect() {
    }

    RegionRect(const RegionRect &value)
    {
        areaName = value.areaName;
        tl = Point(value.tl);
        br = Point(value.br);
    }
};

#endif