#ifndef DETECTOR_H
#define DETECTOR_H

#include "log.hpp"
#include "opencv2/opencv.hpp"
#include "board.hpp"
#include "token.hpp"

class Detector
{
public:
    static bool calibrateReferenceFrame(Mat &frame, Board &board);
    static bool queryTokens(Mat &frame, Board &board, std::vector<const Token *> tokens, std::vector<string> ROIs, std::vector<pair<std::string, std::string>> &result);
};

#endif