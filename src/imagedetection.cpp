#include "imagedetection.hpp"
#include "log.hpp"

#define MIN_GOOD_POINTS 5

void ImageDetection::calculateKeypoints(Strategy strategy, AssetMat &am)
{
    if (strategy == Strategy::Feature2D)
    {
        calculateKeypointsFeature2D(am);
    }
    else if (strategy == Strategy::SIFT)
    {
        calculateKeypointsSIFT(am);
    }
    else
    {
        Log(ERROR) << "calculateKeypoints - unknown strategy: " << (int)strategy;
    }
}

void ImageDetection::calculateKeypointsFeature2D(AssetMat &am)
{
    // https://www.programmersought.com/article/18095237159/
    static const int MAX_FEATURES = 1500;
    static Ptr<Feature2D> detector = ORB::create(MAX_FEATURES);

    detector->detectAndCompute(am.image, Mat(), am.keypoints, am.descriptors);
}

void ImageDetection::calculateKeypointsSIFT(AssetMat &am)
{
    static Ptr<SIFT> detector = SIFT::create();
    detector->detectAndCompute(am.image, Mat(), am.keypoints, am.descriptors);
}

void ImageDetection::calculateMatches(Strategy strategy, std::vector<DMatch> &matches, const AssetMat &frame, const AssetMat &tpl)
{
    if (strategy == Strategy::Feature2D)
    {
        calculateMatchesFeature2D(matches, frame, tpl);
    }
    else if (strategy == Strategy::SIFT)
    {
        calculateMatchesSIFT(matches, frame, tpl);
    }
    else
    {
        Log(ERROR) << "calculateMatches - unknown strategy: " << (int)strategy;
    }
}

void ImageDetection::calculateMatchesFeature2D(std::vector<DMatch> &matches, const AssetMat &frame, const AssetMat &tpl)
{
    // https://learnopencv.com/image-alignment-feature-based-using-opencv-c-python/
    static const float GOOD_MATCH_PERCENT = 0.15f;
    static Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming");

    matcher->match(frame.descriptors, tpl.descriptors, matches, Mat());
    Log(INFO) << "detected matches: " << matches.size();

    // Sort matches by score
    std::sort(matches.begin(), matches.end());

    // Remove not so good matches
    const int numGoodMatches = (int)(matches.size() * GOOD_MATCH_PERCENT);
    matches.erase(matches.begin() + numGoodMatches, matches.end());
}

void ImageDetection::calculateMatchesSIFT(std::vector<DMatch> &matches, const AssetMat &frame, const AssetMat &tpl)
{
    //  where the 0.7 is from - https://ahmetozlu93.medium.com/marker-less-augmented-reality-by-opencv-and-opengl-531b2af0a130
    // https://docs.opencv.org/3.4/d5/d6f/tutorial_feature_flann_matcher.html

    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
    std::vector<std::vector<DMatch>> knn_matches;
    matcher->knnMatch(frame.descriptors, tpl.descriptors, knn_matches, 2);
    Log(INFO) << "detected matches: " << knn_matches.size();

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

bool ImageDetection::detectBoard(const Mat &camFrame, Board &board, Asset &result)
{
    // https://learnopencv.com/feature-based-image-alignment-using-opencv-c-python/

    Log(INFO) << "ImageDetection detectBoard";

    Asset frame(camFrame);

#ifndef xxx
    AssetMat downscaleFrame = frame.getScaled();

    // do a proportioned scaled version of the board (relative to the frame)
    float scaleFactor = (float)board.asset.getDefault().image.size().width / (float)frame.getDefault().image.size().width;
    int size = (int)((float)DEFAULT_SCALE_WIDTH * scaleFactor);

    AssetMat downscaleBoard = board.asset.getScaled(size);
#else
    AssetMat downscaleFrame = frame.getDefault();
    AssetMat downscaleBoard = board.getDefault();
#endif

    ImageDetection::calculateKeypoints(board.strategy, downscaleFrame);
    ImageDetection::calculateKeypoints(board.strategy, downscaleBoard);

    std::vector<DMatch> matches;
    ImageDetection::calculateMatches(board.strategy, matches, downscaleFrame, downscaleBoard);

#ifdef xxx
    // Draw top matches
    Mat imMatches;
    drawMatches(downscaleFrame.image, downscaleFrame.keypoints, downscaleBoard.image, downscaleBoard.keypoints, matches, imMatches);
    imshow("Good Matches & Object detection", imMatches);
    waitKey();
#endif
    Log(INFO) << "frame kps: " << downscaleFrame.keypoints.size();
    Log(INFO) << "board kps: " << downscaleBoard.keypoints.size();
    Log(INFO) << "good matches: " << matches.size();

    // Extract location of good matches
    std::vector<Point2f> points1, points2;

    float scaleUp1 = 1.0f / downscaleFrame.scaleFactor;
    float scaleUp2 = 1.0f / downscaleBoard.scaleFactor;

    for (size_t i = 0; i < matches.size(); i++)
    {
#ifdef xxx
        // this was the original idea
        points1.push_back(downscaleFrame.keypoints[matches[i].queryIdx].pt);
        points2.push_back(downscaleBoard.keypoints[matches[i].trainIdx].pt);
#else
        // upscale the points relative to the original frame and board
        Point2f p;

        p = downscaleFrame.keypoints[matches[i].queryIdx].pt;
        p.x *= scaleUp1;
        p.y *= scaleUp1;
        points1.push_back(p);

        p = downscaleBoard.keypoints[matches[i].trainIdx].pt;
        p.x *= scaleUp2;
        p.y *= scaleUp2;
        points2.push_back(p);
#endif
    }

    if (matches.size() < MIN_GOOD_POINTS)
        return false;

    Mat imgResult;

    // Find homography
    Mat h = findHomography(points1, points2, RANSAC);

    // Use homography to warp image
    warpPerspective(camFrame, imgResult, h, board.asset.getDefault().image.size());

    // Print estimated homography
    // Log(INFO) <<  "Estimated homography : \n" << h;
    result = imgResult;

    return true;
}
