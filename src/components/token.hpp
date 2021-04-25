#ifndef TOKEN_H
#define TOKEN_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

#include "component.hpp"
#include "matching/strategy.hpp"

using namespace cv;

class Token : public Component
{
private:
    Token(const Token &value) {}

public:
    Geometry geometry;
    Scalar color;

    Token() {}
};

#endif