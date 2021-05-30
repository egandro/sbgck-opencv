#ifndef DETECTOR_H
#define DETECTOR_H

#include "log.hpp"
#include "opencv2/opencv.hpp"
#include "board.hpp"
#include "token.hpp"

class DetectorTokenConfig
{
    DetectorTokenConfig(const DetectorTokenConfig &value)
        : showColorDiff(false), showAllROIs(false), showContours(false), frame(NULL), board(NULL)
    {
    }

public:
    bool showColorDiff;
    bool showAllROIs;
    bool showContours;

    Mat *frame;
    Board *board;
    std::vector<const Token *> tokens;
    std::vector<string> ROIs;
    std::vector<pair<std::string, std::string>> result;

    DetectorTokenConfig()
        : showColorDiff(false), showAllROIs(false), showContours(false), frame(NULL), board(NULL)
    {
    }
};

class Detector
{
public:
    static bool calibrateReferenceFrame(Mat &frame, Board &board);
    static bool queryTokens(DetectorTokenConfig &cfg);
};

#endif