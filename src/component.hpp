#ifndef COMPONENT_H
#define COMPONENT_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

#include "asset.hpp"
#include "strategy.hpp"

using namespace cv;
class Component
{
private:
    Component(const Component &value) {}

public:
    /**
     * @brief this can also be the bounding box for tokens or dices
     *
     */
    Asset asset;
    Strategy strategy;

    Component() {}
};

#endif