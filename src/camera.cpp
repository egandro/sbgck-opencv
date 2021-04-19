#include <stdlib.h>
#include "camera.hpp"
#include "log.hpp"

Camera::Camera(CameraConfig &cfg)
{

    Log(INFO) << "Camera ctor";

    int camNum = 0;

    switch (cfg.mode)
    {
    case Default:
    case Camera0:
        camNum = 0;
    case Camera1:
        camNum = 1;
        Log(INFO) << "Starting VideoCapture hardware: " << camNum;
        if (!videoCapture.open(camNum))
        {
            Log(ERROR) << "open failed";
        }
        break;
    case IPCamera:
        if (cfg.urlOrFileName.size() == 0 || cfg.urlOrFileName.empty())
        {
            Log(ERROR) << "camera config has no URL set";
        }
        else
        {
            Log(INFO) << "Starting VideoCapture url: " << cfg.urlOrFileName;
            if (!videoCapture.open(cfg.urlOrFileName))
            {
                Log(ERROR) << "open failed";
            }
        }
        break;
    case DebugFile:
        if (cfg.urlOrFileName.size() == 0 || cfg.urlOrFileName.empty())
        {
            Log(ERROR) << "camera config has no fileName set";
        }
        else
        {
            Log(INFO) << "Starting VideoCapture DebugFile: " << cfg.urlOrFileName;
            fileMat = imread(cfg.urlOrFileName.c_str(), IMREAD_UNCHANGED);
        }
        break;
    }
}

Camera::~Camera()
{
    Log(INFO) << "Camera destructor";

    if (videoCapture.isOpened())
    {
        Log(INFO) << "VideoCapture closed";
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
            Log(ERROR) << "Camera read failed";
        }
        return frame;
    }
    else
    {
        Log(INFO) << "Camera getFrame (DebugFile)";
        return fileMat;
    }
}


void Camera::setZoom(float zoom) {
    Log(INFO) << "Camera setZoom (HACK implementation)" << zoom;
    // https://github.com/opencv/opencv/issues/15989
    // https://stackoverflow.com/questions/11420748/setting-camera-parameters-in-opencv-python

    // for URL we can use this: curl "http://zte:8080/ptz?zoom=22"
    // requires IP Webcam

    this->videoCapture.set(CAP_PROP_SETTINGS, true);
    this->videoCapture.set(CAP_PROP_ZOOM, zoom);
}