#ifndef IMAGEDETECTION_H
#define IMAGEDETECTION_H

#include "opencv2/opencv.hpp"
#include "asset.hpp"
#include "component.hpp"
#include "board.hpp"

using namespace cv;

// RECT: https://cpp.hotexamples.com/examples/cv/Rect/-/cpp-rect-class-examples.html


class ImageDetection
{
private:
    static void calculateKeypoints(Strategy strategy, AssetMat & am);

    static void calculateKeypointsFeature2D(AssetMat & am);

    static void calculateKeypointsSIFT(AssetMat & am);

    static void calculateMatches(Strategy strategy, std::vector<DMatch> &matches, const AssetMat &frame, const AssetMat &tpl);

    static void calculateMatchesFeature2D(std::vector<DMatch> &matches, const AssetMat &frame, const AssetMat &tpl);

    static void calculateMatchesSIFT(std::vector<DMatch> &matches, const AssetMat &frame, const AssetMat &tpl);

public:
    static bool detectBoard(const Mat &camFrame, Board &board, Asset &result); // make this all const

    //static Rect detectTemplate(Asset assetFrame, Asset assetTpl);
};

#endif