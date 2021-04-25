#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;

#define DEFAULT_SCALE_WIDTH 640

typedef enum e_DetectorMode
{
    None,
    DM_Feature2D,
    DM_SIFT
} DetectorMode;

typedef enum e_ScaleMode
{
    NotScaled,
    ScaledProportional // max 640x480 - honoring aspect
} ScaleMode;

typedef enum e_ColorMode
{
    Unchanged,
    GreyScale // COLOR_BGR2GRAY
    //Canny // Edge detection
} ColorMode;

/**
 * @brief materialized asset - use this if you want to process and image and cache it's result for later usage
 *
 */
class AssetMat
{
public:
    DetectorMode detector;
    ScaleMode scale;
    ColorMode color;
    Mat image;
    Mat descriptors;
    float scaleFactor;
    std::vector<KeyPoint> keypoints;

    AssetMat()
    {
        //Log(INFO) << "AssetMat";
        detector = None;
        scale = NotScaled;
        color = Unchanged;
        scaleFactor = 1.0;
    }

    AssetMat(const AssetMat &value)
    {
        //Log(INFO) << "AssetMat cctor";
        detector = value.detector;
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
            if( (*it).scale == ScaledProportional) {
                // Log(INFO) << "cached ";
                return (*it);
            }
        }

        // copy
        AssetMat am(getDefault());

        am.scale = ScaledProportional;
        am.scaleFactor = (float)width / (float)(am.image.size().width);

        // resize
        resize(am.image, am.image, Size(), am.scaleFactor, am.scaleFactor);
        // Log(INFO) << "resized ";

        assetMats.push_back(am);
        return am;
    }
};

#endif