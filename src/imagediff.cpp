#include <stdlib.h>
#include "imagediff.hpp"
#include "log.hpp"

Mat ImageDiff::removeBackground(const Mat frame, const Mat background)
{
    Log(INFO) << "ImageDiff removeBackground";

    // idea: https://stackoverflow.com/questions/27035672/cv-extract-differences-between-two-images
    //
    // https://docs.opencv.org/3.4/d1/dc5/tutorial_background_subtraction.html

    // https://answers.opencv.org/question/216383/how-could-do-sharpness-images/
    // https://stackoverflow.com/questions/21374148/remove-background-using-opencv-in-c

    Mat image_frame;
    Mat image_background;

    // GaussianBlur(frame, image_frame, Size(3, 3), 0);
    // GaussianBlur(background, image_background, Size(3, 3), 0);

    GaussianBlur(frame, image_frame, Size(9, 9), 0);
    GaussianBlur(background, image_background, Size(9, 9), 0);

    //image_frame = Mat(frame);
    //image_background = Mat(background);

    // cvtColor(background, image_background, COLOR_RGBA2RGB);
    // cvtColor(frame, image_frame, COLOR_RGBA2RGB);

    if(image_frame.channels() != image_background.channels()) {
        Log(ERROR) << "ImageDiff removeBackground - channel missmatch";
    }

    if( (image_frame.size().width != image_background.size().width) ||
        (image_frame.size().height != image_background.size().height) ) {
        Log(ERROR) << "ImageDiff removeBackground - size missmatch";
    }

#ifdef xxx

    //create Background Subtractor objects
    Ptr<BackgroundSubtractor> pBackSub;
    pBackSub = createBackgroundSubtractorMOG2();
    //pBackSub = createBackgroundSubtractorKNN();

    Mat mask;
    //update the background model
    pBackSub->apply(image_background, mask);
    pBackSub->apply(image_frame, mask);
#else
    // calc the difference
    Mat diff;
    absdiff(image_frame, image_background, diff);

    // imshow("diff", diff);
    // waitKey(0);

    // // https://docs.opencv.org/3.4/db/df6/tutorial_erosion_dilatation.html
    Mat erosion_dst;

    int erosion_size = 3;

    //int erosion_type = MORPH_RECT;
    int erosion_type = MORPH_CROSS;
    //int erosion_type = MORPH_ELLIPSE;

    Mat element = getStructuringElement(erosion_type,
                                        Size(2 * erosion_size + 1, 2 * erosion_size + 1),
                                        Point(erosion_size, erosion_size));
    erode(diff, erosion_dst, element);

    // imshow("erosion_dst", erosion_dst);
    // waitKey(0);

    Mat erosion_blured;
    GaussianBlur(erosion_dst, erosion_blured, Size(3, 3), 0);
    erosion_dst = erosion_blured;

    diff = erosion_dst;

    imshow("diff", diff);
    waitKey(0);

    // Get the mask if difference greater than th
    int th = 10; // High value for solid colors!
    Mat mask(image_frame.size(), CV_8UC1);
    //Mat mask(image_frame.size(), CV_8UC3);
    for (int j = 0; j < diff.rows; ++j)
    {
        for (int i = 0; i < diff.cols; ++i)
        {
            cv::Vec3b pix = diff.at<cv::Vec3b>(j, i);

            // this is bad!
            int val = (pix[0] + pix[1] + pix[2]);

            // a simple checksum is too stupid to handle
            // the full color spectrum - this works nice if we
            // want to match R, G, B - but not a color like yellow
            // yellow is 255,255,0 so we squash all colors
            // we need something better here

            if (val > th)
            {
                mask.at<unsigned char>(j, i) = 255;
            }
            else
            {
                mask.at<unsigned char>(j, i) = 0;
            }
        }
    }
#endif

    imshow("mask", mask);
    waitKey(0);

    Mat mask_blured;
    GaussianBlur(mask, mask_blured, Size(3, 3), 0);
    mask = mask_blured;

    // //show the current frame and the fg masks
    // // imshow("Frame", frame);
    // imshow("mask", mask);
    // waitKey(0);

    // get the foreground
    Mat res;
    bitwise_and(image_frame, image_frame, res, mask);

    // display
    imshow("res", res);
    waitKey();

    return res;
}
