#include <stdlib.h>
#include "colorcalibration.hpp"
#include "log.hpp"

// idea from here: https://www.pyimagesearch.com/2014/06/30/super-fast-color-transfer-images/

void ColorCalibration::calibrate(const Mat &frame)
{
    Log(INFO) << "ColorCalibration calibrate";

    Mat frameLab;
    cvtColor(frame, frameLab, cv::COLOR_BGR2Lab);

    Mat lab[3];           //destination array
    split(frameLab, lab); //split source

    //cv::Scalar tempVal = cv::mean( myMat );
    //float myMAtMean = tempVal.val[0];

    // cv::Scalar tempVal = cv::stan( lab[0] );
    // float myMAtMean = tempVal.val[0];
}
