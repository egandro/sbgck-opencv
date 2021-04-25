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
    /**
     * @brief empty board used for token detection
     *
     */
    Mat frameBoardEmpty;

    RoiManager roi;

    Board()
    {
        // might be SIFT
        strategy = Strategy::Feature2D;
    }
};

#endif