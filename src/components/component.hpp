#ifndef COMPONENT_H
#define COMPONENT_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

#include "asset.hpp"
#include "matching/strategy.hpp"

using namespace cv;
class Component
{
private:
    Component(const Component &value) {}

public:
    Asset asset;
    Strategy strategy;

    Component() {}
};

#endif