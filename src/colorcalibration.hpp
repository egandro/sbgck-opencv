#ifndef COLORCALIBRATION_H
#define COLORCALIBRATION_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;

class ImageStats
{
public:
    Mat l;
    Mat a;
    Mat b;

    Scalar lMean;
    Scalar lStd;

    Scalar aMean;
    Scalar aStd;

    Scalar bMean;
    Scalar bStd;

    ImageStats() {}

    ImageStats(const ImageStats &value) : l(value.l),
                                          a(value.a),
                                          b(value.b),
                                          lMean(value.lMean),
                                          lStd(value.lStd),
                                          aMean(value.aMean),
                                          aStd(value.aStd),
                                          bMean(value.bMean),
                                          bStd(value.bStd)
    {
    }
};

class ColorCalibration
{
    ImageStats statsColorChecker;
    ImageStats imageStats(const Mat &imgLAB);

public:
    ColorCalibration(const Mat &colorChecker);

    void calibrate(const Mat &colorChecker);
};

#endif
