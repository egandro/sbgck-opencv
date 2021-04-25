#include "imagedetection.hpp"
#include "log.hpp"

// https://www.programmersought.com/article/18095237159/

// https://learnopencv.com/image-alignment-feature-based-using-opencv-c-python/
const int MAX_FEATURES = 1500;
const float GOOD_MATCH_PERCENT = 0.15f;
//const float GOOD_MATCH_PERCENT = 0.30f;

void alignImages(Mat &im1, Mat &im2, Mat &im1Reg, Mat &h)
{
    // Convert images to grayscale
    Mat im1Gray, im2Gray;
    //cvtColor(im1, im1Gray, cv::COLOR_BGR2GRAY);
    //cvtColor(im2, im2Gray, cv::COLOR_BGR2GRAY);
    im1Gray = im1;
    im2Gray = im2;

    // Variables to store keypoints and descriptors
    std::vector<KeyPoint> keypoints1, keypoints2;
    Mat descriptors1, descriptors2;

    // Detect features and compute descriptors.
    Ptr<Feature2D> detector = ORB::create(MAX_FEATURES);
    //Ptr<SIFT> detector = SIFT::create();
    detector->detectAndCompute(im1Gray, Mat(), keypoints1, descriptors1);
    detector->detectAndCompute(im2Gray, Mat(), keypoints2, descriptors2);

    // Match features.
    std::vector<DMatch> matches;
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming");
    //Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
    matcher->match(descriptors1, descriptors2, matches, Mat());

    // Sort matches by score
    std::sort(matches.begin(), matches.end());

    // Remove not so good matches
    const int numGoodMatches = (int)(matches.size() * GOOD_MATCH_PERCENT);
    matches.erase(matches.begin() + numGoodMatches, matches.end());

    // Draw top matches
    Mat imMatches;
    drawMatches(im1, keypoints1, im2, keypoints2, matches, imMatches);
    imshow("Good Matches & Object detection", imMatches);
    waitKey();

    // imwrite("matches.jpg", imMatches);

    // Extract location of good matches
    std::vector<Point2f> points1, points2;

    for (size_t i = 0; i < matches.size(); i++)
    {
        points1.push_back(keypoints1[matches[i].queryIdx].pt);
        points2.push_back(keypoints2[matches[i].trainIdx].pt);
    }

    // Find homography
    h = findHomography(points1, points2, RANSAC);

    // Use homography to warp image
    warpPerspective(im1, im1Reg, h, im2.size());
}

void ImageDetection::detectTemplate(Mat frame, Mat templateImage)
{
    Log(INFO) << "ImageDetection detectTemplate";

    Mat templateMatch, h;

    alignImages(frame, templateImage, templateMatch, h);

    imshow("Planar template, templateMatch", templateMatch);
    waitKey();

    return;

#ifdef xxx
    const int min_match_count = 5;

    //-- Step 1: Detect the keypoints using SIFT Detector, compute the descriptors

    Ptr<SIFT> detector = SIFT::create();
    std::vector<KeyPoint> keypoints1, keypoints2;
    Mat descriptors1, descriptors2;
    detector->detectAndCompute(frame, noArray(), keypoints1, descriptors1);
    detector->detectAndCompute(templateImage, noArray(), keypoints2, descriptors2);

    //-- Step 2: Matching descriptor vectors with a FLANN based matcher
    // Since SURF is a floating-point descriptor NORM_L2 is used
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
    std::vector<std::vector<DMatch>> knn_matches;
    matcher->knnMatch(descriptors1, descriptors2, knn_matches, 2);

    //-- Filter matches using the Lowe's ratio test
    const float ratio_thresh = 0.7f;
    std::vector<DMatch> good_matches;
    for (size_t i = 0; i < knn_matches.size(); i++)
    {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
        {
            good_matches.push_back(knn_matches[i][0]);
        }
    }

    //-- Draw matches
    Mat img_matches;
    drawMatches(frame, keypoints1, templateImage, keypoints2, good_matches, img_matches, Scalar::all(-1),
                Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    //-- Show detected matches
    imshow("Good Matches", img_matches);
    waitKey();
#endif

#ifdef xxx
    //-- Step 1: Detect the keypoints using SIFT Detector, compute the descriptors
    Ptr<SIFT> detector = SIFT::create();
    std::vector<KeyPoint> keypoints_object, keypoints_scene;
    Mat descriptors_object, descriptors_scene;
    detector->detectAndCompute(templateImage, noArray(), keypoints_object, descriptors_object); // TODO - cache this!
    detector->detectAndCompute(frame, noArray(), keypoints_scene, descriptors_scene);

    //-- Step 2: Matching descriptor vectors with a FLANN based matcher
    // Since SIFT is a floating-point descriptor NORM_L2 is used
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
    std::vector<std::vector<DMatch>> knn_matches;
    matcher->knnMatch(descriptors_object, descriptors_scene, knn_matches, 2);

    //-- Filter matches using the Lowe's ratio test
    const float ratio_thresh = 0.75f;
    std::vector<DMatch> good_matches;
    for (size_t i = 0; i < knn_matches.size(); i++)
    {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
        {
            good_matches.push_back(knn_matches[i][0]);
        }
    }

    //-- Draw matches
    // Mat img_matches;
    // drawMatches( templateImage, keypoints_object, frame, keypoints_scene, good_matches, img_matches, Scalar::all(-1),
    //             Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

    Mat img_matches = frame;

    //-- Localize the object
    std::vector<Point2f> obj;
    std::vector<Point2f> scene;
    for (size_t i = 0; i < good_matches.size(); i++)
    {
        //-- Get the keypoints from the good matches
        obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
        scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
    }
    Mat H = findHomography(obj, scene, RANSAC);
    //-- Get the corners from the image_1 ( the object to be "detected" )
    std::vector<Point2f> obj_corners(4);
    obj_corners[0] = Point2f(0, 0);
    obj_corners[1] = Point2f((float)templateImage.cols, 0);
    obj_corners[2] = Point2f((float)templateImage.cols, (float)templateImage.rows);
    obj_corners[3] = Point2f(0, (float)templateImage.rows);
    std::vector<Point2f> scene_corners(4);
    perspectiveTransform(obj_corners, scene_corners, H);

    //-- Draw lines between the corners (the mapped object in the scene - image_2 )
    // line( img_matches, scene_corners[0] + Point2f((float)templateImage.cols, 0),
    //       scene_corners[1] + Point2f((float)templateImage.cols, 0), Scalar(0, 255, 0), 4 );
    // line( img_matches, scene_corners[1] + Point2f((float)templateImage.cols, 0),
    //       scene_corners[2] + Point2f((float)templateImage.cols, 0), Scalar( 0, 255, 0), 4 );
    // line( img_matches, scene_corners[2] + Point2f((float)templateImage.cols, 0),
    //       scene_corners[3] + Point2f((float)templateImage.cols, 0), Scalar( 0, 255, 0), 4 );
    // line( img_matches, scene_corners[3] + Point2f((float)templateImage.cols, 0),
    //       scene_corners[0] + Point2f((float)templateImage.cols, 0), Scalar( 0, 255, 0), 4 );

    //-- Show detected matches
    //imshow("Good Matches & Object detection", img_matches );
    //waitKey();

    // prespective transform

    // https://stackoverflow.com/questions/7838487/executing-cvwarpperspective-for-a-fake-deskewing-on-a-set-of-cvpo

    // Use homography to warp image
    // https://learnopencv.com/image-alignment-feature-based-using-opencv-c-python/

    // Find perspective transform matrix

    Mat templateMatch;
    warpPerspective(frame, templateMatch, H, templateMatch.size());

    imshow("Good Matches & Object detection", templateMatch);
    waitKey();
#endif
}

// https://stackoverflow.com/questions/7838487/executing-cvwarpperspective-for-a-fake-deskewing-on-a-set-of-cvpo

/*
// https://docs.opencv.org/3.4/d7/dff/tutorial_feature_homography.html


// https://docs.opencv.org/3.4/d5/d6f/tutorial_feature_flann_matcher.html

// https://www.programmersought.com/article/18095237159/

   int minHessian = 400;
    //Select SURF feature
    Ptr<SURF>detector = SURF::create(minHessian);
    std::vector<KeyPoint>keypoints1;
    std::vector<KeyPoint>keypoints2;
    Mat descriptor1, descriptor2;
    //Detect key points and calculate descriptor
    detector->detectAndCompute(src1, Mat(), keypoints1, descriptor1);
    detector->detectAndCompute(src2, Mat(), keypoints2, descriptor2);


*/