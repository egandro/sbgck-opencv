#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include "log.hpp"
#include "opencv2/opencv.hpp"
#include "strategy.hpp"

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
};

/**
 * @brief materialized asset - use this if you want to process and image and cache it's result for later usage
 *
 */
class AssetMat
{
public:
    ScaleMode scale;
    ColorMode color;
    Mat image;
    Mat descriptors;
    float scaleFactor;
    std::vector<KeyPoint> keypoints;

    AssetMat()
    {
        //Log(INFO) << "AssetMat";
        scale = ScaleMode::None;
        color = ColorMode::None;
        scaleFactor = 1.0;
    }

    AssetMat(const AssetMat &value)
    {
        //Log(INFO) << "AssetMat cctor";
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
    void fromFile(const char *imageFileName)
    {
        //Log(INFO) << "Asset (" << "fromFile" << " )";
        AssetMat am;
        fileName = imageFileName;
        am.image = imread(fileName, IMREAD_COLOR);
        assetMats.push_back(am);
    }

    void fromMat(const Mat &mat)
    {
        //Log(INFO) << "Asset (" << "fromMat" << ")";
        AssetMat am;
        am.image = Mat(mat);
        assetMats.push_back(am);
    }

    void fromMemory(const unsigned char *data, const int dataLen)
    {
        //Log(INFO) << "Asset (" << "fromMemory" << ")";

        // https://github.com/sumsuddin/ImageProcessingOpenCV
        vector<unsigned char> inputImageBytes(data, data + dataLen);
        Mat mat = imdecode(inputImageBytes, IMREAD_COLOR);

        fromMat(mat);
    }

public:
    AssetDetector assetDetector;
    std::string fileName;
    std::vector<AssetMat> assetMats;

    /**
     * @brief this is used to optimize the feature detection
     *
     * if a feature is found, we won't to an expensive detection again - we just use
     * the homography to cut out the relevant part of the frame
     */
    Mat homography;

    Asset()
        : assetDetector(AssetDetector::None)
    {
        //Log(INFO) << "Asset";
    }

    Asset(const char *imageFileName)
        : assetDetector(AssetDetector::None)
    {
        //Log(INFO) << "Asset (" << imageFileName << ")";
        fromFile(imageFileName);
    }

    Asset(string imageFileName)
        : assetDetector(AssetDetector::None)
    {
        //Log(INFO) << "Asset (" << imageFileName << ")";
        fromFile(imageFileName.c_str());
    }

    Asset(const unsigned char *data, const int dataLen)
        : assetDetector(AssetDetector::None)
    {
        //Log(INFO) << "Asset ( void* , " << size << ")";
        fromMemory(data, dataLen);
    }

    Asset(const Mat mat)
    {
        //Log(INFO) << "Asset (mat)";
        fromMat(mat);
    }

    Asset(const Asset &value)
    {
        //Log(INFO) << "Asset cctor";
        assetDetector = value.assetDetector;
        fileName = value.fileName;
        for (std::size_t i = 0; i < value.assetMats.size(); ++i)
        {
            AssetMat am = value.assetMats[i];
            assetMats.push_back(am);
        }
    }

    ~Asset()
    {
        assetMats.clear();
        //Log(INFO) << "~Asset";
    }

    AssetMat getDefault()
    {
        AssetMat result;

        if (assetMats.size() > 0)
        {
            result = assetMats.at(0);
        }

        return result;
    }

    AssetMat getScaled(const int width = DEFAULT_SCALE_WIDTH)
    {
        for (std::size_t i = 0; i < assetMats.size(); ++i)
        {
            if (assetMats[i].scale == ScaleMode::ScaledProportional)
            {
                // Log(INFO) << "cached ";
                return assetMats[i];
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