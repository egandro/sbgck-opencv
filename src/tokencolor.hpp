#ifndef TOKENCOLOR_H
#define TOKENCOLOR_H

#include "opencv2/opencv.hpp"
#include "log.hpp"
#include "token.hpp"

using namespace cv;

class TokenColor
{
public:
    static Mat detectColor(const Mat frame, const Token &token);
};

#endif