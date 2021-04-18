#ifndef CAMERA_H
#define CAMERA_H

#include "opencv2/opencv.hpp"

using namespace cv;

enum CameraMode
{
    Unknown = 0,
    Default = 1,
    Camera0 = 2,
    Camera1 = 3,
    IPCamera = 4,
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
    // maybe we need a void setParameter(CameraParameter *param); here

    Mat getFrame();
};

#endif