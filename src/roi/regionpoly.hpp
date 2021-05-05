#ifndef REGIONPOLY_H
#define REGIONPOLY_H

#include "log.hpp"
#include "opencv2/opencv.hpp"
#include "region.hpp"

using namespace cv;

class RegionPoly : public Region
{
public:
    std::vector<cv::Point> points;

    RegionPoly()
    {
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

    bool isInside(const Point p)
    {
        size_t i, j, nvert = points.size();
        bool c = false;

        // https://stackoverflow.com/questions/11716268/point-in-polygon-algorithm
        for (i = 0, j = nvert - 1; i < nvert; j = i++)
        {
            if (((points[i].y >= p.y) != (points[j].y >= p.y)) &&
                (p.x <= (points[j].x - points[i].x) * (p.y - points[i].y) / (points[j].y - points[i].y) + points[i].x))
                c = !c;
        }

        return c;
    }

    Rect getBoundingRect()
    {
        return boundingRect(points);
    }
};

#endif