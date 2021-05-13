#include "assetdetection.hpp"
#include "log.hpp"

#define MIN_GOOD_POINTS 5

// clean everything and do it like this: https://github.com/vonzhou/opencv/blob/master/image-search/orb.cpp
// detector and descriptors: https://stackoverflow.com/questions/36691050/opencv-3-list-of-available-featuredetectorcreate-and-descriptorextractorc


void AssetDetection::calculateKeypoints(AssetDetector assetDetector, AssetMat &am)
{
    if (assetDetector == AssetDetector::Feature2D)
    {
        calculateKeypointsFeature2D(am);
    }
    else if (assetDetector == AssetDetector::SIFT)
    {
        calculateKeypointsSIFT(am);
    }
    else if (assetDetector == AssetDetector::ORB)
    {
        calculateKeypointsORB(am);
    }
    else
    {
        Log(typelog::ERR) << "calculateKeypoints - unknown assetDetector: " << (int)assetDetector;
    }
}

void AssetDetection::calculateKeypointsFeature2D(AssetMat &am)
{
    // https://www.programmersought.com/article/18095237159/
    static const int MAX_FEATURES = 1500;
    static Ptr<Feature2D> detector = ORB::create(MAX_FEATURES);


    detector->detectAndCompute(am.image, Mat(), am.keypoints, am.descriptors);
}

void AssetDetection::calculateKeypointsSIFT(AssetMat &am)
{
    static Ptr<SIFT> detector = SIFT::create();
    detector->detectAndCompute(am.image, Mat(), am.keypoints, am.descriptors);
}

void AssetDetection::calculateKeypointsORB(AssetMat &am)
{
    static Ptr<ORB> detector = ORB::create();
    detector->detectAndCompute(am.image, Mat(), am.keypoints, am.descriptors);
}

void AssetDetection::calculateMatches(AssetDetector assetDetector, std::vector<DMatch> &matches, const AssetMat &frame, const AssetMat &tpl)
{
    if (assetDetector == AssetDetector::Feature2D)
    {
        calculateMatchesFeature2D(matches, frame, tpl);
    }
    else if (assetDetector == AssetDetector::SIFT)
    {
        calculateMatchesSIFT(matches, frame, tpl);
    }
    else if (assetDetector == AssetDetector::ORB)
    {
        calculateMatchesORB(matches, frame, tpl);
    }
    else
    {
        Log(typelog::ERR) << "calculateMatches - unknown assetDetector: " << (int)assetDetector;
    }
}

void AssetDetection::calculateMatchesFeature2D(std::vector<DMatch> &matches, const AssetMat &frame, const AssetMat &tpl)
{
    // https://learnopencv.com/image-alignment-feature-based-using-opencv-c-python/
    static const float GOOD_MATCH_PERCENT = 0.15f;
    static Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming");

    if (frame.descriptors.dims == 0 || tpl.descriptors.dims == 0)
    {
        Log(typelog::INFO) << "detected matches: skipted - no descriptors";
        return;
    }

    matcher->match(frame.descriptors, tpl.descriptors, matches, Mat());
    Log(typelog::INFO) << "detected matches: " << matches.size();

    // Sort matches by score
    std::sort(matches.begin(), matches.end());

    // Remove not so good matches
    const int numGoodMatches = (int)(matches.size() * GOOD_MATCH_PERCENT);
    matches.erase(matches.begin() + numGoodMatches, matches.end());
}

void AssetDetection::calculateMatchesSIFT(std::vector<DMatch> &matches, const AssetMat &frame, const AssetMat &tpl)
{
    //  where the 0.7 is from - https://ahmetozlu93.medium.com/marker-less-augmented-reality-by-opencv-and-opengl-531b2af0a130
    // https://docs.opencv.org/3.4/d5/d6f/tutorial_feature_flann_matcher.html

    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
    std::vector<std::vector<DMatch>> knn_matches;
    matcher->knnMatch(frame.descriptors, tpl.descriptors, knn_matches, 2);
    Log(typelog::INFO) << "detected matches: " << knn_matches.size();

    //-- Filter matches using the Lowe's ratio test
    const float ratio_thresh = 0.7f;
    for (size_t i = 0; i < knn_matches.size(); i++)
    {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
        {
            matches.push_back(knn_matches[i][0]);
        }
    }
}

void AssetDetection::calculateMatchesORB(std::vector<DMatch> &matches, const AssetMat &frame, const AssetMat &tpl)
{
    // https://github.com/vonzhou/opencv/blob/master/image-search/orb.cpp

    //static const float GOOD_MATCH_PERCENT = 0.15f;
    static Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("ORB");

    std::vector<std::vector<DMatch>> knn_matches;
    matcher->knnMatch(frame.descriptors, tpl.descriptors, knn_matches, 2);
    Log(typelog::INFO) << "detected matches: " << knn_matches.size();

    //-- Filter matches using the Lowe's ratio test
    const float ratio_thresh = 0.7f;
    for (size_t i = 0; i < knn_matches.size(); i++)
    {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
        {
            matches.push_back(knn_matches[i][0]);
        }
    }
}

bool AssetDetection::detectAsset(const Mat &camFrame, Asset &inputAsset, Asset &result, bool reuseHomography)
{
    // https://learnopencv.com/feature-based-image-alignment-using-opencv-c-python/

    // Log(typelog::INFO) << "ImageDetection detectAsset";

    if (!inputAsset.homography.empty() && reuseHomography)
    {
        // Log(typelog::INFO) << "ImageDetection detectAsset reusing homography";

        Mat imgResult;

        // Use homography to warp image
        warpPerspective(camFrame, imgResult, inputAsset.homography, inputAsset.getDefault().image.size());

        result = imgResult;
        return true;
    }

    Asset frame(camFrame);

#ifndef xxx
    AssetMat downscaleFrame = frame.getScaled();

    // do a proportioned scaled version of the InputAsset (relative to the frame)
    float scaleFactor = (float)inputAsset.getDefault().image.size().width / (float)frame.getDefault().image.size().width;
    int size = (int)((float)DEFAULT_SCALE_WIDTH * scaleFactor);

    AssetMat downscaleInputAsset = inputAsset.getScaled(size);
#else
    AssetMat downscaleFrame = frame.getDefault();
    AssetMat downscaleInputAsset = InputAsset.getDefault();
#endif

    AssetDetector assetDetector = inputAsset.assetDetector;

    AssetDetection::calculateKeypoints(assetDetector, downscaleFrame);
    AssetDetection::calculateKeypoints(assetDetector, downscaleInputAsset);

    std::vector<DMatch> matches;
    AssetDetection::calculateMatches(assetDetector, matches, downscaleFrame, downscaleInputAsset);

#ifdef xxx
    // Draw top matches
    Mat imMatches;
    drawMatches(downscaleFrame.image, downscaleFrame.keypoints, downscaleInputAsset.image, downscaleInputAsset.keypoints, matches, imMatches);
    imshow("Good Matches & Object detection", imMatches);
    waitKey();
#endif
    Log(typelog::INFO) << "frame kps: " << downscaleFrame.keypoints.size();
    Log(typelog::INFO) << "InputAsset kps: " << downscaleInputAsset.keypoints.size();
    Log(typelog::INFO) << "good matches: " << matches.size();

    // Extract location of good matches
    std::vector<Point2f> points1, points2;

    float scaleUp1 = 1.0f / downscaleFrame.scaleFactor;
    float scaleUp2 = 1.0f / downscaleInputAsset.scaleFactor;

    for (size_t i = 0; i < matches.size(); i++)
    {
#ifdef xxx
        // this was the original idea
        points1.push_back(downscaleFrame.keypoints[matches[i].queryIdx].pt);
        points2.push_back(downscaleInputAsset.keypoints[matches[i].trainIdx].pt);
#else
        // upscale the points relative to the original frame and InputAsset
        Point2f p;

        p = downscaleFrame.keypoints[matches[i].queryIdx].pt;
        p.x *= scaleUp1;
        p.y *= scaleUp1;
        points1.push_back(p);

        p = downscaleInputAsset.keypoints[matches[i].trainIdx].pt;
        p.x *= scaleUp2;
        p.y *= scaleUp2;
        points2.push_back(p);
#endif
    }

    if (matches.size() < MIN_GOOD_POINTS)
        return false;

    Mat imgResult;

    // Find homography
    //Mat h = findHomography(points1, points2, RANSAC);

    // remove false positives:
    // https://stackoverflow.com/questions/57075719/filtering-out-false-positives-from-feature-matching-homography-opencv

    int minInliers = 6;           //can be any value > 4
    double reprojectionError = 3; // default value, you can change it to some lower to get more reliable estimation.
    Mat mask;

    Mat h = findHomography(points1, points2, RANSAC, reprojectionError, mask);
    int inliers = 0;
    for (int i = 0; i < mask.rows; ++i)
    {
        if(mask.at<unsigned char>(0,i) == 1) inliers++;
    }
    if (inliers < minInliers)
    {
        //homography is bad - final call - don't crash
        Log(typelog::INFO) << "homography is bad";
        return false;
    }

    inputAsset.homography = h;

    // Use homography to warp image
    warpPerspective(camFrame, imgResult, inputAsset.homography, inputAsset.getDefault().image.size());

    // Print estimated homography
    // Log(typelog::INFO) <<  "Estimated homography : \n" << h;
    result = imgResult;

    return true;
}
