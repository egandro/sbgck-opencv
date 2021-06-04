#ifndef DETECTOR_H
#define DETECTOR_H

#include "log.hpp"
#include "opencv2/opencv.hpp"
#include "board.hpp"
#include "token.hpp"

class ColorMap
{
    std::vector<Scalar> from;
    std::vector<Scalar> to;

public:
    ColorMap()
    {
    }

    ColorMap(const ColorMap &value)
    {
        for (size_t i = 0; i < value.from.size(); i++)
        {
            from.push_back(value.from[i]);
        }

        for (size_t i = 0; i < value.to.size(); i++)
        {
            to.push_back(value.to[i]);
        }
    }

    bool hasMappedColor(Scalar &color)
    {
        for (size_t i = 0; i < from.size(); i++)
        {
            if (from[i][0] == color[0] &&
                from[i][1] == color[1] &&
                from[i][2] == color[2])
            {
                return true;
            }
        }

        return false;
    }

    Scalar getMappedColor(Scalar &color)
    {
        for (size_t i = 0; i < from.size(); i++)
        {
            if (from[i][0] == color[0] &&
                from[i][1] == color[1] &&
                from[i][2] == color[2])
            {
                return to[i];
            }
        }

        return color;
    }

    void setMappedColor(Scalar &color, Scalar &mapped)
    {
        if (hasMappedColor(color))
        {
            // already have this
            return;
        }
        from.push_back(color);
        to.push_back(mapped);
    }

    void reset()
    {
        from.clear();
        to.clear();
    }
};

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
    static bool calibrateReferenceFrame(Mat &frame, Board &board, const double histogramCorrelationMin = 0.85);
    static bool queryTokens(DetectorTokenConfig &cfg);
    static bool detectReferenceImage(Mat &frame, Asset &reference, Mat &result, const double histogramCorrelationMin = 0.85);
    static bool calibrateColorMap(Mat &frame, Asset &reference, Mat &destination, std::vector<std::vector<Scalar>> referenceColors, ColorMap &result, const int border, const double segmentPercentage = 0.20);
};

#endif