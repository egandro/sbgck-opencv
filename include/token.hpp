#ifndef TOKEN_H
#define TOKEN_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

#include "internal/component.hpp"
#include "strategy.hpp"

using namespace cv;

class Token : public Component
{
public:
    Geometry geometry;
    /**
     * @brief this is a BGR color
     *
     */
    Scalar color;

    TokenDetector tokenDetector;

    Token() : geometry(Geometry::None),
              color(0, 0, 0),
              tokenDetector(TokenDetector::None)
    {
    }

    Token(const Token &value)
        : Component(value)
    {
        geometry = value.geometry;
        color = value.color;
        tokenDetector = value.tokenDetector;
    }
};

#endif