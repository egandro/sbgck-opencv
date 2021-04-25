#ifndef BOARD_H
#define BOARD_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

#include "asset.hpp"
#include "roimanager.hpp"

using namespace cv;

class Board
{
public:
    Asset asset;
    RoiManager roiManager;
};

#endif