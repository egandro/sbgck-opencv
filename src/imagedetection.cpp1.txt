#include "imagedetection.hpp"
#include "log.hpp"

// https://www.programmersought.com/article/18095237159/


void ImageDetection::detectTemplate(Mat frame, Mat templateImage)
{
    Log(INFO) << "ImageDetection detectTemplate";

#ifdef xxx
    const int min_match_count = 5;

    //-- Step 1: Detect the keypoints using SIFT Detector, compute the descriptors

    Ptr<SIFT> detector = SIFT::create();
    std::vector<KeyPoint> keypoints1, keypoints2;
    Mat descriptors1, descriptors2;
    detector->detectAndCompute( frame, noArray(), keypoints1, descriptors1 );
    detector->detectAndCompute( templateImage, noArray(), keypoints2, descriptors2 );

    //-- Step 2: Matching descriptor vectors with a FLANN based matcher
    // Since SURF is a floating-point descriptor NORM_L2 is used
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
    std::vector< std::vector<DMatch> > knn_matches;
    matcher->knnMatch( descriptors1, descriptors2, knn_matches, 2 );

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
    drawMatches( frame, keypoints1, templateImage, keypoints2, good_matches, img_matches, Scalar::all(-1),
                 Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

    //-- Show detected matches
    imshow("Good Matches", img_matches );
    waitKey();
#endif


    //-- Step 1: Detect the keypoints using SIFT Detector, compute the descriptors
    Ptr<SIFT> detector = SIFT::create();
    std::vector<KeyPoint> keypoints_object, keypoints_scene;
    Mat descriptors_object, descriptors_scene;
    detector->detectAndCompute( templateImage, noArray(), keypoints_object, descriptors_object );
    detector->detectAndCompute( frame, noArray(), keypoints_scene, descriptors_scene );
    //-- Step 2: Matching descriptor vectors with a FLANN based matcher
    // Since SURF is a floating-point descriptor NORM_L2 is used
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
    std::vector< std::vector<DMatch> > knn_matches;
    matcher->knnMatch( descriptors_object, descriptors_scene, knn_matches, 2 );
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
    Mat img_matches;
    drawMatches( templateImage, keypoints_object, frame, keypoints_scene, good_matches, img_matches, Scalar::all(-1),
                 Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    //-- Localize the object
    std::vector<Point2f> obj;
    std::vector<Point2f> scene;
    for( size_t i = 0; i < good_matches.size(); i++ )
    {
        //-- Get the keypoints from the good matches
        obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
        scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
    }
    Mat H = findHomography( obj, scene, RANSAC );
    //-- Get the corners from the image_1 ( the object to be "detected" )
    std::vector<Point2f> obj_corners(4);
    obj_corners[0] = Point2f(0, 0);
    obj_corners[1] = Point2f( (float)templateImage.cols, 0 );
    obj_corners[2] = Point2f( (float)templateImage.cols, (float)templateImage.rows );
    obj_corners[3] = Point2f( 0, (float)templateImage.rows );
    std::vector<Point2f> scene_corners(4);
    perspectiveTransform( obj_corners, scene_corners, H);
    //-- Draw lines between the corners (the mapped object in the scene - image_2 )
    line( img_matches, scene_corners[0] + Point2f((float)templateImage.cols, 0),
          scene_corners[1] + Point2f((float)templateImage.cols, 0), Scalar(0, 255, 0), 4 );
    line( img_matches, scene_corners[1] + Point2f((float)templateImage.cols, 0),
          scene_corners[2] + Point2f((float)templateImage.cols, 0), Scalar( 0, 255, 0), 4 );
    line( img_matches, scene_corners[2] + Point2f((float)templateImage.cols, 0),
          scene_corners[3] + Point2f((float)templateImage.cols, 0), Scalar( 0, 255, 0), 4 );
    line( img_matches, scene_corners[3] + Point2f((float)templateImage.cols, 0),
          scene_corners[0] + Point2f((float)templateImage.cols, 0), Scalar( 0, 255, 0), 4 );
    //-- Show detected matches
//    imshow("Good Matches & Object detection", img_matches );
//    waitKey();

// prespective transform


// https://stackoverflow.com/questions/7838487/executing-cvwarpperspective-for-a-fake-deskewing-on-a-set-of-cvpo


    // Assemble a rotated rectangle out of that info
    RotatedRect box = minAreaRect(cv::Mat(scene_corners));
    std::cout << "Rotated box set to (" << box.boundingRect().x << "," << box.boundingRect().y << ") " << box.size.width << "x" << box.size.height << std::endl;

    Point2f pts[4];

    box.points(pts);

    // Does the order of the points matter? I assume they do NOT.
    // But if it does, is there an easy way to identify and order
    // them as topLeft, topRight, bottomRight, bottomLeft?

    cv::Point2f src_vertices[3];
    src_vertices[0] = pts[0];
    src_vertices[1] = pts[1];
    src_vertices[2] = pts[3];
    //src_vertices[3] = not_a_rect_shape[3];

    Point2f dst_vertices[4];
    dst_vertices[0] = Point(0, 0);
    dst_vertices[1] = Point(box.boundingRect().width-1, 0); // Bug was: had mistakenly switched these 2 parameters
    dst_vertices[2] = Point(0, box.boundingRect().height-1);
    dst_vertices[3] = Point(box.boundingRect().width-1, box.boundingRect().height-1);


   /* Mat warpMatrix = getPerspectiveTransform(src_vertices, dst_vertices);

    cv::Mat rotated;
    cv::Size size(box.boundingRect().width, box.boundingRect().height);
    warpPerspective(src, rotated, warpMatrix, size, INTER_LINEAR, BORDER_CONSTANT);*/
    Mat warpAffineMatrix = getAffineTransform(src_vertices, dst_vertices);

    cv::Mat rotated;
    cv::Size size(box.boundingRect().width, box.boundingRect().height);
    warpAffine(frame, rotated, warpAffineMatrix, size, INTER_LINEAR, BORDER_CONSTANT);

   //  imwrite("rotated.jpg", rotated);

      imshow("Good Matches & Object detection", rotated );
       waitKey();

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