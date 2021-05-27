#ifndef DETECTOR_H
#define DETECTOR_H

#include "log.hpp"
#include "opencv2/opencv.hpp"
#include "board.hpp"

class Detector {
public:
    static bool calibrateReferenceFrame(Mat &frame, Board &board);
};

#endif