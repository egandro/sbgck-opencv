#include <ctype.h>
#include <opencv2/opencv.hpp>
#include <log.hpp>

#include "camera.hpp"
#include "imagedetection.hpp"

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

    Board board;
    board.asset = Asset(argv[2]);

    Mat img;
    CameraMode mode = CameraMode::DebugFile;

    std::string urlOrFileName(argv[1]);

    std::string lower(urlOrFileName);
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower.rfind("http", 0) == 0)
    {
        mode = CameraMode::IPCamera;
    } else {
        img = imread(urlOrFileName, IMREAD_COLOR);
    }

    // camera config
    CameraConfig cfg(mode, urlOrFileName, img);

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
    ImageDetection::detectBoard(frame, board, detectedBoard);

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
