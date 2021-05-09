#include <stdlib.h>
#include "tokencolor.hpp"
using namespace cv;

Mat TokenColor::detectColor(const Mat frame, const Token &token)
{
    //Log(INFO) << "TokenColor detectColor";

    //float tolerance = 0.05f; // 5%
    float tolerance = 0.20f; // 20%

    int b = (int)token.color[0];
    int g = (int)token.color[1];
    int r = (int)token.color[2];

    int bmin = (int)(b - (255.0f * tolerance));
    if (bmin < 0)
        bmin = 0;
    int gmin = (int)(g - (255.0f * tolerance));
    if (gmin < 0)
        gmin = 0;
    int rmin = (int)(r - (255.0f * tolerance));
    if (rmin < 0)
        rmin = 0;

    int bmax = (int)(b + (255.0f * tolerance));
    if (bmax > 255)
        bmax = 255;
    int gmax = (int)(g + (255.0f * tolerance));
    if (gmax > 255)
        gmax = 255;
    int rmax = (int)(r + (255.0f * tolerance));
    if (rmax > 255)
        rmax = 255;

    Scalar lower(bmin, gmin, rmin);
    Scalar upper(bmax, gmax, rmax);

    Mat result;
    inRange(frame, lower, upper, result);

    return result;
}
