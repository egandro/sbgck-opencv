#include "imagedetection.hpp"
#include "log.hpp"

#define MIN_GOOD_POINTS 5

//DetectorMode ImageDetection::detectorMode = DM_Feature2D;
DetectorMode ImageDetection::detectorMode = DM_SIFT;

void ImageDetection::calculateKeypoints(AssetMat &am)
{
    if (detectorMode == DM_Feature2D)
    {
        calculateKeypointsFeature2D(am);
    }
    else if (detectorMode == DM_SIFT)
    {
        calculateKeypointsSIFT(am);
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

void ImageDetection::calculateMatches(std::vector<DMatch> &matches, const AssetMat &frame, const AssetMat &tpl)
{
    if (detectorMode == DM_Feature2D)
    {
        calculateMatchesFeature2D(matches, frame, tpl);
    }
    else if (detectorMode == DM_SIFT)
    {
        calculateMatchesSIFT(matches, frame, tpl);
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

Asset ImageDetection::detectBoard(Mat camFrame, Asset board)
{
    // https://learnopencv.com/feature-based-image-alignment-using-opencv-c-python/

    Log(INFO) << "ImageDetection detectBoard";

    Asset frame(camFrame);

#ifndef xxx
    AssetMat downscaleFrame = frame.getScaled();

    // do a proportioned scaled version of the board (relative to the frame)
    double scaleFactor = (double)board.getDefault().image.size().width / (double)frame.getDefault().image.size().width;
    double size = (double)DEFAULT_SCALE_WIDTH * scaleFactor;

    AssetMat downscaleBoard = board.getScaled(size);
#else
    AssetMat downscaleFrame = frame.getDefault();
    AssetMat downscaleBoard = board.getDefault();
#endif

    ImageDetection::calculateKeypoints(downscaleFrame);
    ImageDetection::calculateKeypoints(downscaleBoard);

    std::vector<DMatch> matches;
    ImageDetection::calculateMatches(matches, downscaleFrame, downscaleBoard);

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

    double scaleUp1 = (double)1.0 / (double)downscaleFrame.scaleFactor;
    double scaleUp2 = (double)1.0 / (double)downscaleBoard.scaleFactor;

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

    Mat imgResult;

    if (matches.size() >= MIN_GOOD_POINTS)
    {
        // Find homography
        Mat h = findHomography(points1, points2, RANSAC);

        // Use homography to warp image
        warpPerspective(camFrame, imgResult, h, board.getDefault().image.size());

        // Print estimated homography
        // Log(INFO) <<  "Estimated homography : \n" << h;
    }

    Asset result(imgResult);
    return result;
}

Rect ImageDetection::detectTemplate(Asset assetFrame, Asset assetTpl)
{
    Log(INFO) << "ImageDetection detectTemplate";

    AssetMat frame = assetFrame.getDefault();
    AssetMat tpl = assetTpl.getDefault();

    //cvtColor(frame.image, frame.image, COLOR_BGR2GRAY);
    //cvtColor(tpl.image, tpl.image, COLOR_BGR2GRAY);
    // https://answers.opencv.org/question/216383/how-could-do-sharpness-images/
#ifdef xxx
    Mat sharpening_kernel = (Mat_<double>(3, 3) << -1, -1, -1,
        -1, 9, -1,
        -1, -1, -1);
    filter2D(frame.image, frame.image, -1, sharpening_kernel);
    filter2D(tpl.image, tpl.image, -1, sharpening_kernel);
#endif

    // https://docs.opencv.org/2.4/doc/tutorials/imgproc/gausian_median_blur_bilateral_filter/gausian_median_blur_bilateral_filter.html


    GaussianBlur(frame.image, frame.image, Size( 5,5  ), 0, 0 );
    GaussianBlur(tpl.image, tpl.image, Size( 3,3 ), 0, 0 );
    // medianBlur ( tpl.image, tpl.image, 3 );

    ImageDetection::calculateKeypoints(frame);
    ImageDetection::calculateKeypoints(tpl);

    std::vector<DMatch> matches;
    ImageDetection::calculateMatches(matches, frame, tpl);

#ifndef xxx
    // Draw top matches
    Mat imMatches;
    std::vector<DMatch> m;
    std::vector<KeyPoint> k;
    //drawMatches(frame.image, k, tpl.image, k, m, imMatches,  Scalar::all(-1),
    //            Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    drawMatches(frame.image, k, tpl.image, k, m, imMatches);

    imwrite("/tmp/debug1.png", imMatches);
    imshow("Empty", imMatches);
    imMatches = Mat();
    //drawMatches(frame.image, frame.keypoints, tpl.image, tpl.keypoints, matches, imMatches, Scalar::all(-1),
    //             Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    drawMatches(frame.image, frame.keypoints, tpl.image, tpl.keypoints, matches, imMatches);

    imwrite("/tmp/debug2.png", imMatches);
    imshow("Good Matches & Object detection", imMatches);

    waitKey();
#endif

    Log(INFO) << "frame kps: " << frame.keypoints.size();
    Log(INFO) << "board kps: " << tpl.keypoints.size();
    Log(INFO) << "matches: " << matches.size();

    // Extract location of good matches
    std::vector<Point2f> points1, points2;

    for (size_t i = 0; i < matches.size(); i++)
    {
        points1.push_back(frame.keypoints[matches[i].queryIdx].pt);
        points2.push_back(tpl.keypoints[matches[i].trainIdx].pt);
    }

    Mat imgResult = Mat(frame.image);

    if (matches.size() >= MIN_GOOD_POINTS)
    {
        std::vector<Point2f> obj;
        std::vector<Point2f> scene;
        for (size_t i = 0; i < matches.size(); i++)
        {
            //-- Get the keypoints from the good matches
            obj.push_back(tpl.keypoints[matches[i].queryIdx].pt);
            scene.push_back(frame.keypoints[matches[i].trainIdx].pt);
        }
        Mat h = findHomography(obj, scene, RANSAC);

        std::vector<Point2f> obj_corners(4);
        obj_corners[0] = Point2f(0, 0);
        obj_corners[1] = Point2f((float)tpl.image.size().width, 0);
        obj_corners[2] = Point2f((float)tpl.image.size().width, (float)frame.image.size().height);
        obj_corners[3] = Point2f(0, (float)tpl.image.size().height);

        std::vector<Point2f> scene_corners(4);

        perspectiveTransform(obj_corners, scene_corners, h);

        //-- Draw lines between the corners (the mapped object in the scene - image_2 )
        line(imgResult, scene_corners[0], scene_corners[1], Scalar(255, 0, 255), 4);
        line(imgResult, scene_corners[1], scene_corners[2], Scalar(255, 0, 255), 4);
        line(imgResult, scene_corners[2], scene_corners[3], Scalar(255, 0, 255), 4);
        line(imgResult, scene_corners[3], scene_corners[0], Scalar(255, 0, 255), 4);

        imshow("imgResult", imgResult);
        waitKey();
    }

    Rect result;
    return result;
}