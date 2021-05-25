#ifndef REGIONCIRCLE_H
#define REGIONCIRCLE_H

#include "../log.hpp"
#include "opencv2/opencv.hpp"
#include "region.hpp"

using namespace cv;

class RegionCircle : public Region
{
    bool isInRectangle(const Point p)
    {
        // https://stackoverflow.com/questions/481144/equation-for-testing-if-a-point-is-inside-a-circle
        return p.x >= center.x - radius && p.x <= center.x + radius &&
               p.y >= center.y - radius && p.y <= center.y + radius;
    }

public:
    Point center;
    int radius;

    RegionCircle()
    {
        radius = 0;
    }

    RegionCircle(const RegionCircle &value)
    {
        areaName = value.areaName;
        center = Point(value.center);
        radius = value.radius;
    }

    bool isInside(const Point p)
    {
        // https://stackoverflow.com/questions/481144/equation-for-testing-if-a-point-is-inside-a-circle

        if (!isInRectangle(p))
        {
            return false;
        }

        double dx = center.x - p.x;
        double dy = center.y - p.y;
        dx *= dx;
        dy *= dy;
        double distanceSquared = dx + dy;
        double radiusSquared = radius * radius;

        return distanceSquared <= radiusSquared;
    }

    Rect getBoundingRect()
    {
        std::vector<cv::Point> points;
        points.push_back(Point(center.x - radius, center.y - radius));
        points.push_back(Point(center.x + radius, center.y + radius));

        return boundingRect(points);
    }
};

#endif