#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include "log.hpp"
#include "opencv2/opencv.hpp"
#include "matching/strategy.hpp"

using namespace cv;

#define DEFAULT_SCALE_WIDTH 640

enum class ScaleMode
{
    None,
    ScaledProportional // max 640x480 - honoring aspect
};

enum class ColorMode
{
    None,
    GreyScale // COLOR_BGR2GRAY
    //Canny // Edge detection
};

/**
 * @brief materialized asset - use this if you want to process and image and cache it's result for later usage
 *
 */
class AssetMat
{
public:
    Strategy strategy;
    ScaleMode scale;
    ColorMode color;
    Mat image;
    Mat descriptors;
    float scaleFactor;
    std::vector<KeyPoint> keypoints;

    AssetMat()
    {
        //Log(INFO) << "AssetMat";
        strategy = Strategy::None;
        scale = ScaleMode::None;
        color = ColorMode::None;
        scaleFactor = 1.0;
    }

    AssetMat(const AssetMat &value)
    {
        //Log(INFO) << "AssetMat cctor";
        strategy = value.strategy;
        scale = value.scale;
        color = value.color;
        image = Mat(value.image);
        descriptors = Mat(value.descriptors);
        scaleFactor = value.scaleFactor;

        for (std::vector<KeyPoint>::const_iterator it = value.keypoints.begin();
             it != value.keypoints.end();
             ++it)
        {
            KeyPoint kp = KeyPoint(*it);
            keypoints.push_back(kp);
        }
    }

    ~AssetMat()
    {
        image.release();
        keypoints.clear();
        descriptors.release();
        //Log(INFO) << "~AssetMat";
    }
};

/**
 * @brief Asseds (boards, tokens, images used in OpenCV create a AssetMat for caching processed images)
 *
 */
class Asset
{
    void fromFile(const char *imageFileName) {
        //Log(INFO) << "Asset (" << "fromFile" << " )";
        AssetMat am;
        fileName = imageFileName;
        am.image = imread(fileName, IMREAD_UNCHANGED);
        assetMats.push_back(am);
    }

    void fromMat(const Mat &mat) {
        //Log(INFO) << "Asset (" << "fromMat" << ")";
        AssetMat am;
        am.image = Mat(mat);
        assetMats.push_back(am);
    }

public:
    std::string fileName;
    std::list<AssetMat> assetMats;

    Asset() {
        //Log(INFO) << "Asset";
    }

    Asset(const char *imageFileName) {
        //Log(INFO) << "Asset (" << imageFileName << ")";
       fromFile(imageFileName);
    }

    Asset(string imageFileName) {
        //Log(INFO) << "Asset (" << imageFileName << ")";
        fromFile(imageFileName.c_str());
    }

    Asset(const Mat mat) {
        //Log(INFO) << "Asset (mat)";
        fromMat(mat);
    }

    Asset(const Asset &value)
    {
        //Log(INFO) << "Asset cctor";
        fileName = value.fileName;
        std::list<AssetMat> am = value.assetMats;
        for (std::list<AssetMat>::iterator it = am.begin();
             it != am.end();
             ++it)
        {
            AssetMat am = AssetMat(*it);
            assetMats.push_back(am);
        }
    }

    ~Asset()
    {
        assetMats.clear();
        //Log(INFO) << "~Asset";
    }

    AssetMat getDefault() {
        std::list<AssetMat>::iterator it = assetMats.begin();
        std::advance(it, 0);

        return *it;
    }

    AssetMat getScaled(const int width=DEFAULT_SCALE_WIDTH) {
        for (std::list<AssetMat>::iterator it = assetMats.begin();
             it != assetMats.end();
             ++it)
        {
            if( (*it).scale == ScaleMode::ScaledProportional) {
                // Log(INFO) << "cached ";
                return (*it);
            }
        }

        // copy
        AssetMat am(getDefault());

        am.scale = ScaleMode::ScaledProportional;
        am.scaleFactor = (float)width / (float)(am.image.size().width);

        // resize
        resize(am.image, am.image, Size(), am.scaleFactor, am.scaleFactor);
        // Log(INFO) << "resized ";

        assetMats.push_back(am);
        return am;
    }

};

#endif