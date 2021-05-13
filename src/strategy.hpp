#ifndef STRATEGY_H
#define STRATEGY_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;

enum class Geometry
{
    None, // match asset
    Triangle,
    Square,
    Rect,
    Hexagon,
    Pentagon,
    // Octagon, // OpenCV will match circles as octagons
    Circle
};

enum class AssetDetector
{
    None, // default
    Feature2D,
    SIFT,
    ORB
};

enum class TokenDetector
{
    None, // default
    Color,
    Geometry,
    Color_And_Geometry,
    Feature
};


string getGeometryString(Geometry geometry);

#endif