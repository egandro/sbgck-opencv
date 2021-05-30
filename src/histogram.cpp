#include "histogram.hpp"

using namespace cv;

bool Histogram::createHistogramImage(Mat &src, Mat &dest)
{
    Log(typelog::INFO) << "Histogram createHistogramImage";

    // https://docs.opencv.org/3.4/d8/dbc/tutorial_histogram_calculation.html

    if (src.empty())
    {
        Log(typelog::ERR) << "image is empty";
        return false;
    }

    bool isColor = false;

    if (src.channels() > 1)
    {
        isColor = true;
    }

    vector<Mat> bgr_planes;
    split(src, bgr_planes);

    int histSize = 256;
    float range[] = {0, 256}; //the upper boundary is exclusive
    const float *histRange = {range};
    bool uniform = true, accumulate = false;
    Mat histImage;

    int hist_w = 512, hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);

    if (isColor)
    {
        Mat b_hist, g_hist, r_hist;

        calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
        calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
        calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);

        histImage = Mat(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

        normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
        normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
        normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

        for (int i = 1; i < histSize; i++)
        {
            line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
                 Point(bin_w * (i), hist_h - cvRound(b_hist.at<float>(i))),
                 Scalar(255, 0, 0), 2, 8, 0);
            line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
                 Point(bin_w * (i), hist_h - cvRound(g_hist.at<float>(i))),
                 Scalar(0, 255, 0), 2, 8, 0);
            line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
                 Point(bin_w * (i), hist_h - cvRound(r_hist.at<float>(i))),
                 Scalar(0, 0, 255), 2, 8, 0);
        }
    }
    else
    {
        Mat hist;

        calcHist(&bgr_planes[0], 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);

        histImage = Mat(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

        normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

        for (int i = 1; i < histSize; i++)
        {
            line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(hist.at<float>(i - 1))),
                 Point(bin_w * (i), hist_h - cvRound(hist.at<float>(i))),
                 Scalar(255, 255, 255), 2, 8, 0);
        }
    }

    histImage.copyTo(dest);

    return true;
}
