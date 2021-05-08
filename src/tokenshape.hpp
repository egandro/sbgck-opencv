#ifndef TOKENSHAPE_H
#define TOKENSHAPE_H

#include "opencv2/opencv.hpp"
#include "log.hpp"
#include "token.hpp"

using namespace cv;


class ShapeLocation {
public:
    Rect boundRect;
    bool found;
    vector<Point> contours;

    ShapeLocation() {

    }

    ShapeLocation(const ShapeLocation &value)
    {
        boundRect = value.boundRect;
        found = value.found;

        std::vector<Point> points = value.contours;
        for (std::vector<Point>::iterator it = points.begin();
             it != points.end();
             ++it)
        {
            Point p(*it);
            contours.push_back(p);
        }
    }

    ~ShapeLocation()
    {
        contours.clear();
    }
};

class TokenShape
{
public:
    static vector<ShapeLocation> detectShape(const Mat frame, const Token &token);
};

#endif