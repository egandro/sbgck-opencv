#include <stdlib.h>
#include "camera.hpp"
#include "log.hpp"

Camera::Camera(CameraConfig &cfg)
{
    Log(typelog::INFO) << "Camera ctor";

    int camNum = 0;

    switch (cfg.mode)
    {
    case CameraMode::Default:
    case CameraMode::Camera0:
        camNum = 0;
    case CameraMode::Camera1:
        if(cfg.mode != CameraMode::Default && cfg.mode != CameraMode::Camera0) {
            camNum = 1;
        }
        Log(typelog::INFO) << "Starting VideoCapture hardware: " << camNum;
        if (!videoCapture.open(camNum))
        {
            Log(typelog::ERR) << "open failed";
        }
        break;
    case CameraMode::IPCamera:
        if (cfg.urlOrFileName.size() == 0 || cfg.urlOrFileName.empty())
        {
            Log(typelog::ERR) << "camera config has no URL set";
        }
        else
        {
            Log(typelog::INFO) << "Starting VideoCapture url: " << cfg.urlOrFileName;
            if (!videoCapture.open(cfg.urlOrFileName))
            {
                Log(typelog::ERR) << "open failed";
            }
        }
        break;
    case CameraMode::DebugFile:
        if (cfg.urlOrFileName.size() == 0 || cfg.urlOrFileName.empty())
        {
            Log(typelog::ERR) << "camera config has no fileName set";
        }
        else
        {
            Log(typelog::INFO) << "Starting VideoCapture DebugFile: " << cfg.urlOrFileName;
            fileMat = imread(cfg.urlOrFileName.c_str(), IMREAD_COLOR);
        }
        break;
    default:
        break;
    }
}

Camera::~Camera()
{
    Log(typelog::INFO) << "Camera destructor";

    if (videoCapture.isOpened())
    {
        Log(typelog::INFO) << "VideoCapture closed";
        videoCapture.release();
    }
}

Mat Camera::getFrame()
{
    if (videoCapture.isOpened())
    {
        Mat frame;
        if (!videoCapture.read(frame))
        {
            Log(typelog::ERR) << "Camera read failed";
        }
        return frame;
    }
    else
    {
        Log(typelog::INFO) << "Camera getFrame (DebugFile)";
        return fileMat;
    }
}

// void Camera::setZoom(float zoom) {
//     Log(typelog::INFO) << "Camera setZoom (HACK implementation)" << zoom;
//     // https://github.com/opencv/opencv/issues/15989
//     // https://stackoverflow.com/questions/11420748/setting-camera-parameters-in-opencv-python

//     // for URL we can use this: curl "http://zte:8080/ptz?zoom=22"
//     // requires IP Webcam

//     this->videoCapture.set(CAP_PROP_SETTINGS, true);
//     this->videoCapture.set(CAP_PROP_ZOOM, zoom);
// }