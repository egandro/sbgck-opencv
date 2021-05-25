#ifndef COMPONENT_H
#define COMPONENT_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

#include "internal/asset.hpp"
#include "strategy.hpp"

using namespace cv;
class Component
{
public:
    /**
     * @brief this can also be the bounding box for tokens or dices
     *
     */
    Asset asset;

    std::string name;

    Component() {}

    Component(const Asset &value)
        : asset(value)
    {
    }

    Component(const Component &value)
    {
        asset = value.asset;
        name = value.name;
    }
};

#endif