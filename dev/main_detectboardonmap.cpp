#include <ctype.h>
#include <opencv2/opencv.hpp>
#include <log.hpp>

#include "camera.hpp"
#include "board.hpp"
#include "assetdetection.hpp"

using namespace cv;

structlog LOGCFG = {};

int main(int argc, char **argv)
{
    LOGCFG.prefix = (char *)"main_detectboardonmap";
    LOGCFG.headers = true;
    LOGCFG.level = typelog::DEBUG;

    if (argc != 4)
    {
        Log(typelog::DEBUG) << "usage: " << argv[0] << " <ImageFileOrUrl> <BoardFile> <OutFile>";
        return -1;
    }

    Asset asset(argv[2]);
    Board board(asset);

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

    Asset detectedBoard;
    AssetDetection::detectAsset(frame, board.asset, detectedBoard);

    //imshow("Good Matches & Object detection", detectedBoard.getDefault().image);
    //waitKey();

    Mat image = detectedBoard.getDefault().image;
    if(image.empty()) {
        Log(typelog::WARN) << "got empty board back";
    } else {
        imwrite(argv[3], image);
    }

    return 0;
}
