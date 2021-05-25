#ifndef ASSETDETECTION_H
#define ASSETDETECTION_H

#include "opencv2/opencv.hpp"
#include "internal/asset.hpp"
#include "internal/component.hpp"

using namespace cv;

// RECT: https://cpp.hotexamples.com/examples/cv/Rect/-/cpp-rect-class-examples.html


class AssetDetection
{
private:
    static void calculateKeypoints(AssetDetector assetDetector, AssetMat & am);

    static void calculateKeypointsFeature2D(AssetMat & am);

    static void calculateKeypointsSIFT(AssetMat & am);

    static void calculateKeypointsORB(AssetMat & am);

    static void calculateMatches(AssetDetector assetDetector, std::vector<DMatch> &matches, const AssetMat &frame, const AssetMat &tpl);

    static void calculateMatchesFeature2D(std::vector<DMatch> &matches, const AssetMat &frame, const AssetMat &tpl);

    static void calculateMatchesSIFT(std::vector<DMatch> &matches, const AssetMat &frame, const AssetMat &tpl);

    static void calculateMatchesORB(std::vector<DMatch> &matches, const AssetMat &frame, const AssetMat &tpl);

public:
    // make this all const
    static bool detectAsset(const Mat &camFrame, Asset &inputAsset, Asset &result, const bool reuseHomography = true);
};

#endif