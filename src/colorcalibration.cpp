#include <stdlib.h>
#include "colorcalibration.hpp"
#include "log.hpp"

// idea from here: https://www.pyimagesearch.com/2014/06/30/super-fast-color-transfer-images/
// https://github.com/jrosebr1/color_transfer
// https://github.com/jrosebr1/color_transfer/blob/master/color_transfer/__init__.py

ColorCalibration::ColorCalibration(const Mat &colorChecker)
{
    Log(INFO) << "ColorCalibration ctor";

    Mat colorCheckerLab;
    cvtColor(colorChecker, colorCheckerLab, cv::COLOR_BGR2Lab);

    statsColorChecker = imageStats(colorCheckerLab);
}

ImageStats ColorCalibration::imageStats(const Mat &imgLAB)
{
    ImageStats res;

    Mat lab[3];         //destination array (l, a, b)
    split(imgLAB, lab); //split source

    res.l = lab[0];
    res.a = lab[1];
    res.b = lab[2];

    meanStdDev(res.l, res.lMean, res.lStd);
    meanStdDev(res.a, res.aMean, res.aStd);
    meanStdDev(res.b, res.bMean, res.bStd);

    return res;
}

void ColorCalibration::calibrate(const Mat &frame)
{
    Log(INFO) << "ColorCalibration calibrate";

    Mat frameLab;
    cvtColor(frame, frameLab, cv::COLOR_BGR2Lab);

    ImageStats statsFrame = imageStats(frameLab);

    // subtract the means from the target image
    statsFrame.l -= statsColorChecker.lMean;
    statsFrame.a -= statsColorChecker.aMean;
    statsFrame.b -= statsColorChecker.bMean;

#ifdef PRESERVE_PAPER
    // scale by the standard deviations using paper proposed factor
    statsFrame.l = (statsFrame.lStd / statsColorChecker.lStd) * statsFrame.l;
    statsFrame.a = (statsFrame.aStd / statsColorChecker.aStd) * statsFrame.a;
    statsFrame.b = (statsFrame.bStd / statsColorChecker.bStd) * statsFrame.b;
#else
    // scale by the standard deviations using reciprocal of paper proposed factor
    statsFrame.l = (statsColorChecker.lStd / statsFrame.lStd) * statsFrame.l;
    statsFrame.a = (statsColorChecker.aStd / statsFrame.aStd) * statsFrame.a;
    statsFrame.b = (statsColorChecker.bStd / statsFrame.bStd) * statsFrame.b;
#endif

	// add in the source mean
	statsFrame.l += statsColorChecker.lMean;
	statsFrame.a += statsColorChecker.aMean;
	statsFrame.b += statsColorChecker.bMean;

}
