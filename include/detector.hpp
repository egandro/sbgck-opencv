#ifndef DETECTOR_H
#define DETECTOR_H

#include "log.hpp"
#include "opencv2/opencv.hpp"
#include "board.hpp"
#include "token.hpp"


// https://stackoverflow.com/questions/31118209/how-to-store-cvscalar-objects-in-a-map-in-c
// https://stackoverflow.com/questions/1102392/how-can-i-use-stdmaps-with-user-defined-types-as-key
namespace std
{
    template<> struct less<Scalar>
    {
       bool operator() (const Scalar& lhs, const Scalar& rhs) const
       {
            return lhs[0] < rhs[0]; // bogus, i doubt, that you need *real* sorting
       }
    };
}


class ColorMap
{
    std::map<Scalar, Scalar> mapping;

public:
    ColorMap()
    {
    }

    ColorMap(const ColorMap &value)
    {
        for (map<Scalar, Scalar>::const_iterator it = value.mapping.begin(); it != value.mapping.end(); it++)
        {
            Scalar key = it->first;
            Scalar value = it->second;
            mapping[key] = value;
        }
    }

    bool hasMappedColor(Scalar &color)
    {
        if (mapping.find(color) != mapping.end())
           return true;
        return false;
    }

    Scalar getMappedColor(Scalar &color)
    {
        if (mapping.find(color) != mapping.end())
           return color;
        return mapping[color];
    }

    void setMappedColor(Scalar &color, Scalar &mapped)
    {
        mapping[color] = mapped;
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
    static bool calibrateReferenceFrame(Mat &frame, Board &board, const bool histogramCheck = true);
    static bool queryTokens(DetectorTokenConfig &cfg);
    static bool detectReferenceImage(Mat &frame, Asset &reference, Mat &result, const bool histogramCheck = true);
    static bool calibrateColorMap(Mat &frame, Asset &reference, std::vector<std::vector<Scalar>> referenceColors, ColorMap &result, const int border, double segmentPercentage = 0.20);
};

#endif