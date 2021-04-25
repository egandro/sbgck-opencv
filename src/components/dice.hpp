#ifndef DICE_H
#define DICE_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

#include "component.hpp"
#include "matching/strategy.hpp"

using namespace cv;

class Dice : public Component
{
private:
    Dice(const Dice &value) {}

public:
    Geometry geometry;
    Scalar color;

    Dice() {}
};

#endif