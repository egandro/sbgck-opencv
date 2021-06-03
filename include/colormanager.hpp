#ifndef ROIMANAGER_H
#define ROIMANAGER_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;

class ColorMap
{
    std::map<Scalar, Scalar> mapping;

public:
    ColorMap()
    {
    }

    ColorMap(const ColorMap &value)
    {
        for (map<Scalar, Scalar>::const_iterator it = value.mapping.begin(); it != value.mapping.end(); it++)
        {
            mapping[Scalar(it->first)] = Scalar(it->second);
        }
    }

    Scalar getMappedColor(Scalar &color)
    {
        if (mapping.find(color) != mapping.end())
            return color;
        return mapping[color];
    }

    void setMappedColor(Scalar &color, Scalar &mapped)
    {
        mapping[Scalar(color)] = Scalar(mapped);
    }
};

class ColorManager
{
public:

    bool static calibrateColorMap(Mat &frame, Mat &referenceImage, int rows, int cols, ColorMap &result );

};

#endif