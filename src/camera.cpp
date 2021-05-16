#include <stdlib.h>
#include "camera.hpp"

/**
   Check some of this


    https://docs.opencv.org/3.4/d6/d7f/samples_2cpp_2camshiftdemo_8cpp-example.html#_a17

    zoom

    https://stackoverflow.com/questions/50870405/how-can-i-zoom-my-webcam-in-open-cv-python

    https://github.com/harimkang/openCV-with-Zoom

    Camera properties Properties

    https://answers.opencv.org/question/175793/why-am-i-not-able-to-change-all-settings-of-logitech-c920-camera-using-opencv-python/

    https://answers.opencv.org/question/6713/how-to-set-or-get-videocapture-properties/

    https://stackoverflow.com/questions/11420748/setting-camera-parameters-in-opencv-python

    c++
    https://answers.opencv.org/question/6713/how-to-set-or-get-videocapture-properties/

    videostream:

    https://stackoverflow.com/questions/26577025/c-opencv-webcam-stream-to-html

    Named pipe

    https://stackoverflow.com/questions/35166111/opencv-python-reading-video-from-named-pipe
*/

bool Camera::open(CameraConfig &config)
{
    Log(typelog::INFO) << "Camera open";

    cfg = config;

    int camNum = 0;

    switch (cfg.mode)
    {
    case CameraMode::Default:
    case CameraMode::Camera0:
        camNum = 0;
    case CameraMode::Camera1:
        if (cfg.mode != CameraMode::Default && cfg.mode != CameraMode::Camera0)
        {
            camNum = 1;
        }
        Log(typelog::INFO) << "Starting VideoCapture hardware: " << camNum;
        if (!videoCapture.open(camNum))
        {
            Log(typelog::ERR) << "open failed";
            return false;
        }
        break;
    case CameraMode::IPCamera:
        if (cfg.urlOrFile.empty())
        {
            Log(typelog::ERR) << "camera config has no URL set";
            return false;
        }
        else
        {
            Log(typelog::INFO) << "Starting VideoCapture url: " << cfg.urlOrFile;
            if (!videoCapture.open(cfg.urlOrFile))
            {
                Log(typelog::ERR) << "open failed";
                return false;
            }
        }
        break;
    case CameraMode::DebugFile:
        if (!cfg.urlOrFile.empty())
        {
            // load from file
            cfg.frame = imread(cfg.urlOrFile, IMREAD_COLOR);
        }
        if (cfg.frame.empty())
        {
            Log(typelog::ERR) << "camera config has empty debugfile frame";
            return false;
        }
        else
        {
            Log(typelog::INFO) << "Starting VideoCapture with debugfile";
        }
        break;
    default:
        Log(typelog::ERR) << "camera config mode is not supported";
        return false;
        break;
    }

    return true;
}

bool Camera::reset()
{
    Log(typelog::INFO) << "Camera reset";
    close();
    return open(cfg);
}

void Camera::close()
{
    Log(typelog::INFO) << "Camera close";

    if (videoCapture.isOpened())
    {
        videoCapture.release();
    }
}

bool Camera::getFrame(Mat &result)
{
    if (videoCapture.isOpened())
    {
        //Log(typelog::INFO) << "Camera getFrame";
        if (!videoCapture.read(result))
        {
            Log(typelog::ERR) << "Camera read failed";
            return false;
        }
        return true;
    }
    else if (!cfg.frame.empty())
    {
        //Log(typelog::INFO) << "Camera getFrame (DebugFile)";
        cfg.frame.copyTo(result);
        return true;
    }
    Log(typelog::ERR) << "Camera read filed (not opened and no debug file)";
    return false;
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