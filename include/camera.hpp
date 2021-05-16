#ifndef CAMERA_H
#define CAMERA_H

#include "opencv2/opencv.hpp"
#include "log.hpp"

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

// TODO: add camera parameters like zoomlevel  check the cpp file for links
class CameraConfig
{
public:
    CameraMode mode;
    std::string urlOrFile;
    Mat frame;

    CameraConfig() { }

    CameraConfig(CameraMode mode, std::string urlOrFile, Mat frame = Mat())
        : mode(mode), urlOrFile(urlOrFile), frame(frame)
    {
    }

    CameraConfig(const CameraConfig &value)
    {
        mode = value.mode;
        urlOrFile = value.urlOrFile;
        if (!value.frame.empty())
        {
            value.frame.copyTo(frame);
        }
    }
};

class Camera
{
private:
    int camNum;
    VideoCapture videoCapture;
    CameraConfig cfg;

    Camera(const Camera &value) {}

public:
    Camera()
    {
        Log(typelog::INFO) << "Camera ctor";
    }

    ~Camera()
    {
        Log(typelog::INFO) << "Camera dtor";
        close();
    }

    bool open(CameraConfig &config);

    bool reset();

    void close();

    bool getFrame(Mat &result);
};

#endif