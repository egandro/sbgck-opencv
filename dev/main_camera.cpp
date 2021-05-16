#include <opencv2/opencv.hpp>
#include <log.hpp>

#include "camera.hpp"

using namespace cv;

structlog LOGCFG = {};

int main(int argc, char **argv)
{
    LOGCFG.prefix = (char *)"main_camera";
    LOGCFG.headers = true;
    LOGCFG.level = typelog::DEBUG;

    if (argc != 3)
    {
        Log(typelog::DEBUG) << "usage: " << argv[0] << " <ImageFileOrUrl> <TargetFile>";
        return -1;
    }

    CameraMode mode = CameraMode::DebugFile;

    std::string urlOrFileName(argv[1]);

    std::string lower(urlOrFileName);
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower.rfind("http", 0) == 0)
    {
        mode = CameraMode::IPCamera;
    }

    // camera config
    CameraConfig cfg(mode, urlOrFileName);

    // open the camera
    Camera cam;
    if (!cam.open(cfg))
    {
        Log(typelog::ERR) << "can't open camera";
        return 1;
    }

    Mat frame;
    if (!cam.getFrame(frame)) {
        Log(typelog::ERR) << "can't read frame";
        return 1;
    }

    // namedWindow("Display Image", WINDOW_AUTOSIZE);
    // imshow("Display Image", frame);
    // waitKey(0);

    imwrite(argv[2], frame);

    return 0;
}
