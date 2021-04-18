#ifndef AREAMANAGER_H
#define AREAMANAGER_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;

// https://docs.opencv.org/3.4/d3/d96/tutorial_basic_geometric_drawing.html

struct Circle
{
public:
    std::string areaName;
    Point center;
    int radius;

    Circle() {
        radius = 0;
    }

    Circle(const Circle &value)
    {
        areaName = value.areaName;
        center = Point(value.center);
        radius = value.radius;
    }
};

struct Square
{
public:
    std::string areaName;
    Point pt1;
    Point pt2;

    Square() {
    }

    Square(const Square &value)
    {
        areaName = value.areaName;
        pt1 = Point(value.pt1);
        pt2 = Point(value.pt2);
    }
};

struct Poly
{
public:
    std::string areaName;
    std::vector<cv::Point> points;

    Poly() {
    }

    Poly(const Poly &value)
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
};

class AreaManager
{
private:
    static std::list<Circle> circles;
    static std::list<Square> squares;
    static std::list<Poly> polygons;

    static bool parseCircle(const std::string areaName, const std::string coords, Circle &circle);
    static bool parseSquare(const std::string areaName, const std::string coords, Square &square);
    static bool parsePoly(const std::string areaName, const std::string coords, Poly &poly);

public:
    static void initFromJsonString(const std::string jsonStr);
};

#endif