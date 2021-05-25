#ifndef REGIONRECT_H
#define REGIONRECT_H

#include "../log.hpp"
#include "opencv2/opencv.hpp"
#include "region.hpp"

using namespace cv;

class RegionRect : public Region
{
public:
    Point tl;
    Point br;

    RegionRect()
    {
    }

    RegionRect(const RegionRect &value)
    {
        areaName = value.areaName;
        tl = Point(value.tl);
        br = Point(value.br);
    }

    bool isInside(const Point p)
    {
        // https://www.geeksforgeeks.org/check-if-a-point-lies-on-or-inside-a-rectangle-set-2/
        if (p.x > tl.x && p.x < br.x && p.y > tl.y && p.y < br.y)
            return true;

        return false;
    }

    Rect getBoundingRect()
    {
        std::vector<cv::Point> points;
        points.push_back(tl);
        points.push_back(br);

        return boundingRect(points);
    }
};

#endif