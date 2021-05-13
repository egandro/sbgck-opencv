#ifndef TOKEN_H
#define TOKEN_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

#include "component.hpp"
#include "strategy.hpp"

using namespace cv;

class Token : public Component
{
private:
    Token(const Token &value) {}

public:
    Geometry geometry;
    /**
     * @brief this is a BGR color
     *
     */
    Scalar color;

    TokenDetector tokenDetector;

    Token() {}
};

#endif