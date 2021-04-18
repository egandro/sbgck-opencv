#include <opencv2/opencv.hpp>

#include "camera.hpp"

#include "log.hpp"

using namespace cv;

structlog LOGCFG = {};

int main(int argc, char **argv)
{
    LOGCFG.prefix = (char *)"main_camera";
    LOGCFG.headers = true;
    LOGCFG.level = DEBUG;

    if (argc != 3)
    {
        Log(DEBUG) << "usage: " << argv[0] << " <ImageFileOrUrl> <TargetFile>";
        return -1;
    }

    CameraMode mode = DebugFile;

    std::string urlOrFileName(argv[1]);

    std::string lower(urlOrFileName);
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower.rfind("http", 0) == 0)
    {
        mode = IPCamera;
    }

    CameraConfig cfg = {
        mode,
        urlOrFileName
    };

    Camera *camPtr = new Camera(cfg);

    Mat frame = camPtr->getFrame();

    // namedWindow("Display Image", WINDOW_AUTOSIZE);
    // imshow("Display Image", frame);
    // waitKey(0);

    imwrite(argv[2], frame);

    delete (camPtr);
    camPtr = NULL;

    return 0;
}
