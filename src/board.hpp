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

    /**
     * @brief this is used to optimize the board detection
     *
     * if a board is found, we won't to an expensive detection again - we just use
     * the homography to cut out the relevant part of the frame
     */
    Mat homography;

    RoiManager roiManager;

    Board(const Asset &value)
        : Component(value)
    {
        // might be SIFT
        asset.strategy = Strategy::Feature2D;
    }
};

#endif