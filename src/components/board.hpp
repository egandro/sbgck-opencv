#ifndef BOARD_H
#define BOARD_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

#include "asset.hpp"
#include "component.hpp"
#include "roimanager.hpp"

using namespace cv;

class Board : public Component
{
private:
    Board(const Board &value) {}

public:
    RoiManager roi;

    Board()
    {
        // might be SIFT
        strategy = Strategy::Feature2D;
    }
};

#endif