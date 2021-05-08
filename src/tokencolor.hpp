#ifndef TOKENCOLOR_H
#define TOKENCOLOR_H

#include "opencv2/opencv.hpp"
#include "log.hpp"
#include "token.hpp"

using namespace cv;

class TokenColor
{
public:
    static void detectColor(const Mat frame, Mat &result, const Token &token);
};

#endif