#ifndef CAMERA_H
#define CAMERA_H

#include "opencv2/opencv.hpp"

using namespace cv;

enum class CameraMode
{
    Unknown,
    Default,
    Camera0,
    Camera1,
    IPCamera,
    DebugFile
};

struct CameraConfig
{
    CameraMode mode;
    std::string urlOrFileName;
};

class Camera
{
private:
    int camNum;
    VideoCapture videoCapture;
    // in case we are a file
    Mat fileMat;

public:
    Camera(CameraConfig &cfg);
    ~Camera();

    // maybe we need a void init(); here
    // maybe we need a void setParameter(CameraParameter &param); here

    Mat getFrame();
    // void setZoom(float zoom); // make this with setParameter()
};

#endif