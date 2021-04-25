#ifndef REGIONPOLY_H
#define REGIONPOLY_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;

class RegionPoly
{
public:
    std::string areaName;
    std::vector<cv::Point> points;

    RegionPoly() {
    }

    RegionPoly(const RegionPoly &value)
    {
        areaName = value.areaName;

        for (std::vector<cv::Point>::const_iterator it = value.points.begin();
             it != value.points.end();
             ++it)
        {
            Point p(*it);
            points.push_back(p);
        }
    }

    ~RegionPoly()
    {
        points.clear();
    }
};


#endif